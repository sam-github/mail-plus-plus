//
//
//

#ifndef M_UTIL_H
#define M_UTIL_H

#include <mail++/rope.h>

class MQuoter
{
private:
	crope quotable_;
	crope internally_quotable_;

	int Quotable(char c);
	int InternallyQuotable(char c);

public:
	int	cr;		// '\r'
	int nl;		// '\n'
	int nul;	// '\0'
	int ctl;	// 1 - 31, 127 (DEL)
	int ext;	// 128 - 255
	int other;
	int total;

	/**
Quoter quotes (with '"') text that contains controls, DEL, space,
double-quote or backslash characters.
Inside text that is quoted CR, NL, double-quote,
and backslash characters must be escaped with a backslash. The constructor
allows additional characters to be added to the set of characters that
force the text to be quoted, as well as to the set of characters that
are escaped inside the text.
	*/
	MQuoter(const crope& quotable = "");
	MQuoter(const crope& quotable = "", const crope& internally_quotable = "");
	void Quote(crope& text);
	void ClearCounts();
};

class MLocalPartQuoter : public MQuoter
{
public:
	/**
	*/
	MLocalPartQuoter() : MQuoter("()<>@,;:/[]")
	{
	}
};

class MParameterQuoter : public MQuoter
{
public:
	/**
	* Parameters need all the tspecials from RFC 2045 quoted.
	*/
	MParameterQuoter() : MQuoter("()<>@,;:/[]?=")
	{
	}
};

#endif

