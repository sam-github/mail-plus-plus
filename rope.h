//
// Wrapper for <rope>
//

#ifndef M_ROPE_H
#define M_ROPE_H

#pragma warning 4 9

#include <rope>

inline int RopeCaseCmp(const crope& l, const crope& r)
{
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


#endif

