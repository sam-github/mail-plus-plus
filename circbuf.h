//
// Circular Buffer Class
//
// $Id$
// $Log$
// Revision 1.1  1999/12/13 02:31:50  sam
// Initial revision
//

#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <stddef.h>

class circ_buffer
{
private:
	size_t	sz_;	// current buffer size => e_ - b_

	char*	b_;		// beginning of buffer
	char*	e_;		// one past end of buffer

	char*	r_;		// read/pop point
	char*	w_;		// write/push point

	size_t	i_;		// initial size to allocate

	void incr(char*& p)
	{
		if(++p == e_) {
			p = b_;
		}
	}
	int empty()
	{
		return r_ == w_;
	}
	int full()
	{
		if(!sz_) {
			return 1;
		}
		char* p = w_;
		incr(p);
		if(p == r_) {
			return 1;
		}
		return 0;
	}
	int realloc()
	{
		if(!sz_) {
			// first time is a special case

			b_ = new char[i_];
			if(!b_) {
				return 0;
			}
			e_ = b_ + i_;
			sz_ = i_;
			r_ = w_ = b_;
			return 1;
		}
		char* b = new char[2 * sz_];
		if(!b) {
			return 0;
		}
		w_ = copy(b, this);
		delete[] b_;
		sz_ = 2 * sz_;
		b_ = b;
		e_ = b + sz_;
		r_ = b;

		return 1;
	}
	char* copy(char* b, const circ_buffer* cb)
	{
		char* r = cb->r_;
		char* w = cb->w_;
		while(r != w) {
			*b++ = *r++;
			if(r == cb->e_) {
				r = cb->b_;
			}
		}
		return b;
	}
public:
	circ_buffer(int initial = 2) :
		i_(1), sz_(0), b_(0), e_(0), r_(0), w_(0)
	{
		if(initial <= 0) {
			initial = 1;
		}
		int m = sizeof(size_t) * 8 - 1;
		if(initial > m) {
			initial = m;
		}
		i_ <<= initial;
		if(!i_) {
			i_ = 4;
		}
	}
	circ_buffer(const circ_buffer& cb)
	{
		new (this) circ_buffer; // reuse initialization code
		*this = cb;
	}
	~circ_buffer()
	{	
		delete[] b_;
	}
	circ_buffer& operator = (const circ_buffer& cb)
	{
		if(sz_ < cb.sz_)
		{
			delete[] b_;

			// clear state in case realloc() doesn't work, and
			// so it appears as the first alloc to realloc()
			b_ = e_ = r_ = w_ = 0;
			sz_ = 0;

			// realloc the size we need
			i_ = cb.sz_;
			if(!realloc()) {
				return *this;
			}
		}
		// now we know we have the space, copy the data
		w_ = copy(b_, &cb);

		return *this;
	}
	size_t initial() const
	{
		return i_;
	}
	size_t reserve() const
	{
		return sz_;
	}
	size_t size() const
	{
		if(w_ >= r_) {
			return w_ - r_;
		}
		return (e_ - r_) + (w_ - b_);
	}
	int push(char c)
	{
		if(full() && !realloc()) {
			return 0;
		}
		*w_ = c;
		incr(w_);
		return 1;
	}
	int pop(char& c)
	{
		if(empty()) {
			return 0;
		}
		c = *r_;
		incr(r_);
		return 1;
	}
	int pop()
	{
		char c;
		if(!pop(c)) {
			return -1;
		}
		return c;
	}
};

#endif

