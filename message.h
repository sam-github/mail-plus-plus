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

/**
* Convert text (line-oriented data) from LF to cannonical CRLF line endings.
* The result is not necessarily 7bit, it may need encoding.
*/
inline crope MCanonicalizeText(const crope& text)
{
	crope canonical;

	for(crope::const_iterator p = text.begin(); p != text.end(); ++p)
	{
		if(*p == '\n') {
			canonical.append('\r');
		}
		canonical.append(*p);
	}
	// If last line is not LF terminated, do so.
	if(!canonical.empty() && canonical.back() != '\n') {
		canonical.append('\r');
		canonical.append('\n');
	}

	return canonical;
}

/**
* If last character in container is a LF, chop it off.
*/
inline crope MCharChop(const crope& line)
{
	crope chopped = line;

	if(!chopped.empty() && chopped.back() == '\n')
		chopped.pop_back();

	return chopped;
}

#endif

