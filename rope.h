//
// Wrapper for <rope>, and in addition defines some useful utility
// routines, many modelled after the C string handling functions.
//

#ifndef M_ROPE_H
#define M_ROPE_H

#pragma warning 4 9

#include <rope>

#include <assert.h>
#include <ctype.h>

/**
* @r is the rope to convert to lower case.
* @return the lower-cased rope.
*/
inline crope LowerCase(const crope& r)
{
	crope lower;

	for(crope::const_iterator p = r.begin(); p != r.end(); ++p)
	{
		lower.append(char(tolower(*p)));
	}
	return lower;
}
/**
* @param r is the rope to convert to lower case.
* @return the lower-cased rope.
*/
inline int CaseCompare(const crope& l, const crope& r)
{
	crope L = LowerCase(l);
	crope R = LowerCase(r);

	return L.compare(R);
}
/**
* Reads a single line onto the end of the output crope. Stops after
* appending a NL or EOF is read.
* @param is is the input stream to read from.
* @param r is the rope to append to.
* @return a reference to is.
*/
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
/**
* Slurps all the data from an istream, appending it to a crope.
* @param is is the input stream to read from.
* @param r is the rope to append to.
* @return a reference to is.
*/
inline istream& slurp(istream& is, crope& r)
{
	while(is.good())
		is >> r;

	return is;
}
/**
* Assigns the range [p, e) to a crope.
* @param to is the crope to assign to.
* @param b is the beginning of the range.
* @param e is the end of the range.
* @return to, the rope assigned to.
*/
inline crope& assign(crope& to, crope::const_iterator b, crope::const_iterator e)
{
	to.clear();
	to.append(b, e);

	return to;
}
/**
* Convert text (line-oriented data) from LF to the RFC cannonical CRLF
* line seperators. Note that the result is not necessarily 7bit, it may
* need encoding.
*/
inline crope CanonicalizeText(const crope& text)
{
	crope canonical = text;
	for(
		crope::iterator p = canonical.mutable_begin();
			p != canonical.mutable_end();
				++p)
	{
		if(*p == '\n') {
			p = canonical.insert(p, '\r');
			++p;
			assert(*(p - 1) == '\r');
			assert(*p == '\n');
		}
	}
/*
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
*/
	return canonical;
}

/**
* Return a copy of line with the last character removed if it is
* equal to the chop character.
* @param line is the line to chop.
* @param chop is the character to chop.
* @return the chopped line.
*/
inline crope ChopChar(const crope& line, char chop)
{
	crope chopped = line;

	if(!chopped.empty() && chopped.back() == chop)
		chopped.pop_back();

	return chopped;
}

#if 0
#	define ROUT(X) cout << #X << "(" << __FILE__ << ":" << __LINE__ << ") '" << (X) << "'" << endl;
#else
#	define ROUT(X)
#endif

#endif

