//
// Mime Text Type
//

#ifndef M_TEXT_H
#define M_TEXT_H

#include <mail++/rope.h>
#include <mail++/mime.h>

class MText
{
private:
	typedef crope Rope;
	typedef Rope::const_iterator Ptr;

	static const Rope type_;

	Rope    subtype_;
	Rope	charset_;

	Rope	text_;

	Rope	encoded_text_;
	MEncoding	encoding_;

public:
	MText(const Rope& subtype = "plain");
	MText(const Rope& subtype, const Rope& text);

		// accessors
	const Rope&		Type() const;
	const Rope&		SubType() const;
	const Rope&		Text() const;
	MMessage&		Fill(MMessage& mail) const;

		// modifiers
	const Rope&		Text(const Rope& text);
};

/**
A file is Unix (lines seperated by a single NL (12)) text if:
- all lines are composed of printable ASCII (32 - 126), or TAB (11),
- no lines (not including the NL) are over a particular maximum length.
*/
template <class T>
inline int IsUnixText(T p, const T& e, int maxll)
{
	int ll = 0;

	while(p != e)
	{
		int c = *p;

		switch(c)
		{
		case '\n':
			ll = 0;
			break;
		case '\t':
			ll++;
			break;
		default:
			if(c < 32 || c > 126)
				return 0;
			ll++;
		}
		if(ll > maxll)
			return 0;

		++p;
	}

	return 1;
}

inline int IsUnixText(const crope& text, int maxll = 120)
{
	return IsUnixText(text.begin(), text.end(), maxll);
}

#endif

