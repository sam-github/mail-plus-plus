//
//
//

#ifndef M_ISTEXT_H
#define M_ISTEXT_H

#include <mail++/rope.h>

/**
A file is Unix (lines seperated by a single NL (12)) text if:
- all lines are composed of printable ASCII (32 - 126), or TAB (11),
- no lines are over a particular maximum length.
*/
template <class T>
int IsUnixText(T p, const T& e, int maxll)
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

int IsUnixText(const crope& text, int maxll = 120)
{
	return IsUnixText(text.begin(), text.end(), maxll);
}

#endif

