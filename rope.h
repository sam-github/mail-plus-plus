//
// Wrapper for <rope>, and in addition defines some useful utility
// routines, many modelled after the C string handling functions.
//

#ifndef M_ROPE_H
#define M_ROPE_H

#pragma warning 4 9

#include <rope>

#include <ctype.h>

/*
inline void RopeDeleteStr(crope& r)
{
	r.delete_c_str();
}
inline void RopeDeleteStr(const crope& r)
{
	crope& r_ = (crope&) r;

	r_.delete_c_str();
}
*/
inline crope& RopeLower(crope& r)
{
	crope::iterator p = r.mutable_begin();
	while(p != r.mutable_end())
	{
		char orig = *p;
		char lower = tolower(orig);

		if(orig != lower)
			*p = lower;

		++p;
	}
	return r;
}
inline int RopeCaseCmp(const crope& l, const crope& r)
{
	return l.compare(r);
#if 0
	int cmp = stricmp(l.c_str(), r.c_str());

	RopeDeleteStr(l);
	RopeDeleteStr(r);

	return cmp;
#endif
#if 0
	struct CaseCmp
	{
		int operator () (char l, char r)
		{
			char ls[2] = { l, 0 };
			char rs[2] = { r, 0 };
			return stricmp(ls, rs);
		}
	};


	crope::const_iterator
		lb = l.begin(),
		le = l.end(),
		rb = r.begin(),
		re = r.end();

	// The following expression is copied from the SGI docs for
	// lexicographical_compare_3way():
	return
		lexicographical_compare(lb,le,rb,re,CaseCmp()) ?
			-1 : (lexicographical_compare (rb,re,lb,le,CaseCmp()) ?
				1 : 0);
#endif
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
inline void assign(crope& to, crope::const_iterator b, crope::const_iterator e)
{
	to.clear();
	to.append(b, e);
}
#endif

