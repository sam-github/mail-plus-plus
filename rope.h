//
// Wrapper for <rope>, and in addition defines some useful utility
// routines, many modelled after the C string handling functions.
//

#ifndef M_ROPE_H
#define M_ROPE_H

#pragma warning 4 9

#include <rope>

#include <ctype.h>

inline crope LowerCase(const crope& r)
{
	crope lower;

	for(crope::const_iterator p = r.begin(); p != r.end(); ++p)
	{
		lower.append(char(tolower(*p)));
	}
	return lower;
}
inline int CaseCompare(const crope& l, const crope& r)
{
	crope L = LowerCase(l);
	crope R = LowerCase(r);

	return L.compare(R);
}
inline istream& operator >> (istream& is, crope& r)
{
	int c;

	while((c = is.get()) != EOF) {
		r.append(c);
		if(c == '\n')
			break;
	}

	return is;
}
inline istream& slurp(istream& is, crope& r)
{
	while(is.good())
		is >> r;

	return is;
}
inline crope& assign(crope& to, crope::const_iterator b, crope::const_iterator e)
{
	to.clear();
	to.append(b, e);

	return to;
}

#if 0
#	define ROUT(X) cout << #X << "(" << __FILE__ << ":" << __LINE__ << ") '" << (X) << "'" << endl;
#else
#	define ROUT(X)
#endif

#endif

