// A test for cascaded streambufs...
//
// Try and write an unbuffered stub, fully buffered, and
// one that uses the others buffer directly?

#include <iostream.h>

class Buf : public streambuf
{
	streambuf*	b_;

public:
	Buf(streambuf* buf) : b_(buf) {}

	int overflow(int c)
	{
		c = c;
		return EOF;
	}
	int underflow()
	{
		if(!base()) {
			if((allocate()) == EOF)
				return EOF;
			setp(0,0);
		} else {
			if(in_avail())
				return (unsigned char) *gptr();
		}
		int result = fillbuf();
		if(result == EOF) {
			setg(0,0,0);
			return EOF;
		}
		return (unsigned char) *gptr();
	}
	int fillbuf()
	{
		int count = b_->sgetn(base(), blen());
		if(count == 0)
			return EOF;
		setg(base(), base(), base() + count);
		return 0;
	}

};

int main()
{
	Buf buf(cin.rdbuf());

	cout << &buf;

	return 0;
}
