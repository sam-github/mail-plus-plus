//
//
//

#include <assert.h>

#include <mail++/quote.h>

static const char* QINTERNAL = "\"\\\r\n";
static const char* QEXTERNAL = " \x1f";
	// as well as all control charcters and QINTERNAL

int MQuoter::Quotable(char c)
{
	if(iscntrl(c))
		return 1;

	if(strchr(quotable_.c_str(), c))
		return 1;

	return 0;
}
int MQuoter::InternallyQuotable(char c)
{

	// this returns true when c is '\0'
	if(c && strchr(internally_quotable_.c_str(), c))
		return 1;

	return 0;
}
MQuoter::MQuoter(const crope& quotable) :
	quotable_ (quotable)
{
	ClearCounts();

	quotable_ += QINTERNAL;
	quotable_ += QEXTERNAL;
	internally_quotable_ += QINTERNAL;
}
MQuoter::MQuoter(const crope& quotable, const crope& internally_quotable) :
	quotable_ (quotable), internally_quotable_ (internally_quotable)
{
	ClearCounts();

	quotable_ += QINTERNAL;
	quotable_ += QEXTERNAL;
	internally_quotable_ += QINTERNAL;
}
void MQuoter::Quote(crope& text)
{
	int		quoteit = 0;
	crope	qtext;

	crope::const_iterator p = text.begin();
	crope::const_iterator e = text.end();

	for( ; p != e; ++p)
	{
		char c = *p;

		// does this char require the text to be quoted?
		if(!quoteit) {
			if(Quotable(c))
				quoteit = 1;
		}

		// does this char require quoting even inside the quoted text?
		if(InternallyQuotable(c))
		{
			quoteit = 1;
			qtext.push_back('\\');
		}

		if(c == '\r') {
			cr++;
		} else if(c == '\n') {
			nl++;
		} else if(c == '\0') {
			nul++;
		} else if(c > '\0' && (c <= '\x1f' || c == '\x7f')) {
			ctl++;
		} else if(c >> 7) {
			ext++;
		} else {
			other++;
		}
		total++;

		qtext.push_back(c);
	}

	if(quoteit)
	{
		qtext.push_front('"');
		qtext.push_back('"');
	}

	text.swap(qtext);
}
void MQuoter::ClearCounts()
{
	ctl	= 0;
	cr	= 0;
	nl	= 0;
	nul	= 0;
	ext = 0;
	other = 0;
	total = 0;
}

#if 0

/**
* Some chars are not allowed, even in quoted string, these will
* do their best but return false if these illegal characters are
* found in the unquoted input.
*
* @return the number of mail unsafe-characters found in the input.
*/
int MQuoteUnsafeChars(crope& quoted)
{
	crope::iterator p = quoted.mutable_begin();
	crope::iterator e = quoted.mutable_end();

	int quoteit = 0;
	int unsafe = 0;

	while(p != e)
	{
	}
	return unsafe;
}
int MQuoteUnsafeChars(crope& quoted, const crope& unquoted)
{
	quoted = unquoted;

	return MQuoteUnsafeChars(quoted);
}

#endif

