//
// RFC822 Mail Message
//

#ifndef M_MESSAGE_H
#define M_MESSAGE_H

#include <mail++/rope.h>
#include <mail++/header.h>

class MMessage
{
protected:
	typedef crope Rope;
	typedef crope::const_iterator Ptr;

private:
	MHeader	head_;
	crope	body_;
	crope	text_;

	Ptr Search(Ptr b, Ptr e, const char* s)
	{
		return search(b, e, s, s + strlen(s));
	}

	int dirty_;

	void Dirty() { dirty_ = 1; }
	void Clean() { dirty_ = 0; }

public:
	MMessage();
	MMessage(const crope& text);

	// accessors
	const	MHeader&	Head() const;
			MHeader&	Head();
	const	crope&		Body() const;
	const	crope&		Text() const;
	const	crope&		Text();

	int IsDirty() const { return dirty_ || head_.IsDirty(); }

	// modifiers
	const	crope&		Body(const crope& body);
	const	crope&		Text(const crope& text);
};

#endif

