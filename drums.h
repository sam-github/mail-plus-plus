//
// Parses syntatic elements defined in RFC 822
//

#ifndef M_DRUMS_H
#define M_DRUMS_H

#include <mail++/rope.h>

#include <vector>

#include <assert.h>
#include <ctype.h>

/**
* Core rules from Appendix A of RFC 2234.
ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z
BIT            =  "0" / "1"
CHAR           =  %x01-7F ; any 7-bit US-ASCII character, excluding NUL
CR             =  %x0D ; carriage return
CRLF           =  CR LF ; Internet standard newline
CTL            =  %x00-1F / %x7F ; controls
DIGIT          =  %x30-39 ; 0-9
DQUOTE         =  %x22 ; " (Double Quote)
HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
HTAB           =  %x09 ; horizontal tab
LF             =  %x0A ; linefeed
LWSP           =  *(WSP / CRLF WSP) ; linear white space (past newline)
OCTET          =  %x00-FF ; 8 bits of data
SP             =  %x20 ; space
VCHAR          =  %x21-7E ; visible (printing) characters
WSP            =  SP / HTAB ; white space
*/
class MCoreTokenizer
{
private:
	typedef crope::const_iterator Ptr;

public:
	int	IsDIGIT			(Ptr& p, const Ptr& e)
	{
		// DIGIT = %x30-39		; 0-9

		if(p == e)
			return 0;

		if(*p < 0x30)
			return 0;

		if(*p > 0x39)
			return 0;

		return 1;
	}
	int IsHTAB(Ptr& p, const Ptr& e)
	{
		// HTAB = %x09			; horizontal tab

		return p != e && *p == 0x09;
	}
	int IsSP(Ptr& p, const Ptr& e)
	{
		// SP = %x20

		return p != e && *p == 0x20;
	}
	int	IsWSP			(Ptr& p, const Ptr& e)
	{
		// WSP = SP / HTAB		; white space

		return IsSP(p, e) || IsHTAB(p, e);
	}

	int GetCRLF			(Ptr& p, const Ptr& e)
	{
		Ptr save = p;

		if(GetChar(p, e, '\r') && GetChar(p, e, '\n')) {
			return 1;
		}
		p = save;

		return 0;
	}
	int GetWSP			(Ptr& p, const Ptr& e)
	{
		int i = 0;

		while(IsWSP(p, e)) {
			++p;
			++i;
		}
		return i;
	}

	/**
	* Utility function to get a char, if it exits.
	*/
	int	GetChar			(Ptr& p, const Ptr& e, char c)
	{
		if(p != e && *p == c) {
			++p;
			return 1;
		}
		return 0;
	}
};

class MDrumsTokenizer : public MCoreTokenizer
{
private:
	typedef crope::const_iterator Ptr;

	int	errors_;
	int	unterminated_qstr_;
	int unterminated_comment_;
	int invalid_chars_;

	int strict_;

public:
	/**
	* @param strict set to true if obsolete syntax is NOT to be
	*               parsed, useful for verifying conformance.
	*/
	MDrumsTokenizer(int strict = 0) : strict_(strict)
	{
	}
#if 0
	int Errors() const { return errors_; }

	int UnterminatedQuotedString() const { return unterminated_qstr_; }
	int UnterminatedComment() const { return unterminated_comment_; }
	int InvalidCharacters() const { return invalid_chars_; }

	void ResetErrors()
	{
		errors_ = 0;
		unterminated_qstr_ = 0;
		unterminated_comment_ = 0;
		invalid_chars_ = 0;
	}

	//
	// Character Classification - could be rewritten in a C library
	// independent way, my system's C library matches the RFC
	// definitions, but I don't know if that's guaranteed.
	//
	int IsCHAR(char c) const;
	int IsCTL(char c) const;
	int IsSPACE(char c) const;
	int IsHTAB(char c) const;
	int IsLWSPChar(char c) const;
	int IsSpecial(char c) const;
	int IsAtomChar(char c) const;
	int IsQText(char c) const;
	int IsSmtpQ(char c) const;
		// SMTP's version of qtext, called <q> in the RFC 821 syntax,
		// also exludes <LF>

	//
	// Lexical Analysis - these tokens are all from RFC822,
	// section 3.3, Lexical Tokens, though not all tokens are
	// implemented.
	//

	int SkipWs			(Ptr& p, const Ptr& e);
	int SkipComments	(Ptr& p, const Ptr& e);
	int GetSpecial		(Ptr& p, const Ptr& e, char c);
	int GetComment		(Ptr& p, const Ptr& e, crope& comment);
	int GetAtom			(Ptr& p, const Ptr& e, crope& atom);
	int GetQuotedPair	(Ptr& p, const Ptr& e, crope& qpair);
	int GetQuotedString	(Ptr& p, const Ptr& e, crope& qstr);
	int GetWord			(Ptr& p, const Ptr& e, crope& word);
	int GetPhrase		(Ptr& p, const Ptr& e, crope& phrase);
#endif

	// DRUMS lexer elements
	int GetFWS			(Ptr& p, const Ptr& e)
	{
		// FWS = ([*WSP CRLF] 1*WSP) / obs-FWS
		//
		// rewrite: ([*WSP CRLF] 1*WSP) -> 1*WSP / (*WSP CRLF 1*WSP)

		Ptr save = p;

		int wsp = GetWSP(p, e);

		if(GetCRLF(p, e)) {
			wsp = GetWSP(p, e); 
		}

		if(wsp == 0)
			p = save;

		return wsp > 0;
	}
	int GetCFWS			(Ptr& p, const Ptr& e)
	{
		// CFWS = *([FWS] comment) (([FWS] comment) / FWS)

		return 1;
	}
	/**
	* Get's at least min, and at most max digit characters, where max
	* can be 0 to indicate no-limit.
	* @return the number of digits matched.
	*/
	int GetDigits		(Ptr& p, const Ptr& e, int min, int max, crope& digits)
	{
		assert( (max == 0) || (min <= max) );

		Ptr save = p;

		crope accum;
		int	i = 0;

		while(IsDIGIT(p, e)) {
			accum.push_back(*p);
			++p;
			++i;
			if(max != 0 && i == max)
				break;
		}
		if(i >= min) {
			digits = accum;
			return i;
		}
		return 0;
	}
};

#include <mail++/datetime.h>

class MDateTimeParser : public MDrumsTokenizer
{
private:
	typedef crope::const_iterator Ptr;

	MDrumsTokenizer& lexer;

public:
	MDateTimeParser(int strict = 0) :
		MDrumsTokenizer(strict),
		lexer(*this)
	{
	}
	int DateTime		(const crope& text, MDateTime& date_time)
	{
		Ptr	b = text.begin();
		Ptr e = text.end();

		return GetDateTime(b, e, date_time);
	}
	int GetDateTime		(Ptr& p, const Ptr& e, MDateTime& date_time)
	{
		// date-time = [ day-of-week "," ] date FWS time [CFWS]

		crope day_name;

		if(GetDayOfWeek(p, e, day_name)) {
			if(!lexer.GetChar(p, e, ','))
				return 0;
		}

		int		day;
		crope	month;
		int		year;

		if(!GetDate(p, e, day, month, year))
			return 0;

		if(!lexer.GetFWS(p, e))
			return 0;

		int hour;
		int	min;
		int	sec;
		int	zone;

		if(!GetTime(p, e, hour, min, sec, zone))
			return 0;

		lexer.GetCFWS(p, e);

		// should I confirm that p == e?

		date_time = MDateTime(day_name, day, month, year, hour, min, sec, zone);

		return 1;
	}
	int GetDayOfWeek	(Ptr& p, const Ptr& e, crope& day_name)
	{
		// day-of-week = ([FWS] day-name) / obs-day-of-week

		Ptr save = p;

		lexer.GetFWS(p, e);

		if(GetDayName(p, e, day_name))
			return 1;

		p = save;

		if(GetObsDayOfWeek(p, e, day_name))
			return 1;

		p = save;

		return 0;
	}
	int GetDayName		(Ptr& p, const Ptr& e, crope& day_name)
	{
		// day-name = "Mon" / "Tue" / "Wed" / "Thu" / "Fri" / "Sat" / "Sun"

		const char*	days[] = {
			"Mon",
			"Tue",
			"Wed",
			"Thu",
			"Fri",
			"Sat",
			"Sun",
			0
		};

		return GetName(p, e, days, day_name);
	}
	int GetDate			(Ptr& p, const Ptr& e, int& day, crope& month, int& year)
	{
		// day = day month year

		Ptr save = p;

		int		d;
		crope	m;
		int		y;

		if(!GetDay(p, e, d)) {
			p = save; return 0;
		}
		if(!GetMonth(p, e, m)) {
			p = save; return 0;
		}
		if(!GetYear(p, e, y)) {
			p = save; return 0;
		}

		day		= d;
		month	= m;
		year	= y;

		return 1;
	}
	int GetDay			(Ptr& p, const Ptr& e, int& day)
	{
		// ([FWS] 1*2DIGIT) / obs-day

		Ptr save = p;

		crope digits;

		lexer.GetFWS(p, e);
		if(lexer.GetDigits(p, e, 1, 2, digits)) {
			day = atoi(digits.c_str());
			return 1;
		}
		p = save;

		if(GetObsDay(p, e, day))
			return 1;

		return 0;
	}
	int GetMonth		(Ptr& p, const Ptr& e, crope& month)
	{
		// month = (FWS month-name FWS) / obs-month

		crope m;

		Ptr save = p;

		if(lexer.GetFWS(p, e) && GetMonthName(p, e, m) && lexer.GetFWS(p, e)) {
			month = m;
			return 1;
		}
		p = save;

		if(GetObsMonth(p, e, month)) {
			return 1;
		}

		return 0;
	}
	int GetMonthName	(Ptr& p, const Ptr& e, crope& month)
	{
		// month-name = "Jan" / "Feb" / "Mar" / "Apr" /
		//				"May" / "Jun" / "Jul" / "Aug" /
		//				"Sep" / "Oct" / "Nov" / "Dec";

		const char* month_names[] = {
			"Jan",
			"Feb",
			"Mar",
			"Apr",
			"May",
			"Jun",
			"Jul",
			"Aug",
			"Sep",
			"Oct",
			"Nov",
			"Dec",
			0
		};

		return GetName(p, e, month_names, month);
	}
	int GetYear			(Ptr& p, const Ptr& e, int& year)
	{
		// year = 4*DIGIT / obs-year

		crope digits;

		if(lexer.GetDigits(p, e, 4, 0, digits)) {
			year = atoi(digits.c_str());
			return 1;
		}
		if(GetObsYear(p, e, year))
			return 1;

		return 0;
	}

	int GetTime			(Ptr& p, const Ptr& e, int& hour, int& min, int& sec, int& zone)
	{
		// time = time-of-day FWS zone

		Ptr save = p;

		int h, m, s, z;

		if(!GetTimeOfDay(p, e, h, m, s)) {
			p = save; return 0;
		}
		if(!lexer.GetFWS(p, e)) {
			p = save; return 0;
		}
		if(!GetZone(p, e, z)) {
			p = save; return 0;
		}

		hour	= h;
		min		= m;
		sec		= s;
		zone	= z;

		return 1;
	}
	int GetTimeOfDay	(Ptr& p, const Ptr& e, int& hour, int& min, int& sec)
	{
		// time-of-day = hour ":" minute [ ":" second ]

		Ptr save = p;

		int	h;
		int m;
		int s;

		if(!GetHMS(p, e, h)) {
			p = save; return 0;
		}
		if(!lexer.GetChar(p, e, ':')) {
			p = save; return 0;
		}
		if(!GetHMS(p, e, m)) {
			p = save; return 0;
		}

		if(lexer.GetChar(p, e, ':')) {
			if(!GetHMS(p, e, s)) {
				p = save; return 0;
			}
		} else {
			s = 0;
		}

		hour	= h;
		min		= m;
		sec		= s;

		return 1;
	}
	int GetHMS			(Ptr& p, const Ptr& e, int& hms)
	{
		// hour   = 2DIGIT / obs-hour
		// minute = 2DIGIT / obs-minute
		// sec    = 2DIGIT / obs-second

		crope digits;

		if(lexer.GetDigits(p, e, 2, 2, digits)) {
			hms = atoi(digits.c_str());
			return 1;
		}

		return GetObsHMS(p, e, hms);
	}
	int GetZone			(Ptr& p, const Ptr& e, int& zone)
	{
		// zone = (( "+" / "-" ) 4DIGIT / obs-zone
		//
		// remarks: the 4DIGIT is actually hhmm

		Ptr save = p;

		if(p == e)
			return 0;

		char sign = *p;

		++p;

		crope digits;

		if(lexer.GetDigits(p, e, 4, 4, digits)) {
			zone = atoi(digits.c_str());

			int h = zone / 100;
			int m = zone % 100;

			zone = h * 60 + m;

			if(sign == '-')
				zone = -zone;

			return 1;
		}

		p = save;

		return GetObsZone(p, e, zone);
	}

	//
	// Obsolete syntax, not yet implemented.
	//

	int	GetObsDayOfWeek	(Ptr& p, const Ptr& e, crope& day_name)
	{
		return 0;
	}
	int GetObsMonth		(Ptr& p, const Ptr& e, crope& month)
	{
		return 0;
	}
	int GetObsYear		(Ptr& p, const Ptr& e, int& year)
	{
		return 0;
	}
	int GetObsDay		(Ptr& p, const Ptr& e, int& day)
	{
		return 0;
	}
	int GetObsHMS		(Ptr& p, const Ptr& e, int& day)
	{
		// obs-hour =
		// obs-minute =
		// obs-second =

		return 0;
	}
	int GetObsZone		(Ptr& p, const Ptr& e, int& zone)
	{
		return 0;
	}

private:
	int GetName			(Ptr& p, const Ptr& e, const char* names[], crope& name)
	{
		Ptr save = p;

		char str[] = "123";
		for(int i = 0; i < 3; ++i) {
			if(p != e) {
				str[i] = *p;
				++p;
			} else {
				str[i] = '\0';
			}
		}

		for(int j = 0; names[j]; ++j) {
			if(stricmp(str, names[j]) == 0) {
				name = str;
				return 1;
			}
		}
		p = save;

		return 0;
	}
};

#if 0
	#include <mail++/address.h>

	class MAddressParser
	{
	public:
	// Need some kind of error object here.

		int AddrSpec	(const crope& text, MAddrSpec& mailbox);
		int AddrSpecList(const crope& text, MAddrSpecList& mboxlist);
		int MailBox		(const crope& text, MMailBox& mailbox);
		int MailBoxList	(const crope& text, MMailBoxList& mboxlist);

	private:
		MRfc822Tokenizer lexer;

		typedef crope::const_iterator Ptr;

		// the output
		MMailBoxList	mailboxes_;

		// need a real error reporting object
		int	ok_;

	// groups aren't supported, so neither is an address that can be a
	// mailbox or a group, or a list of such addresses...
	//	int	GetAddressList();
	//	int GetAddress();
	//	int GetGroup();
		int GetMailBox		(Ptr& p, const Ptr& e, MMailBox& mailbox);
		int GetMailBoxList	(Ptr& p, const Ptr& e, MMailBoxList& mailboxes);
		int GetRouteAddr	(Ptr& p, const Ptr& e, MAddrSpec& addrspec);
		int GetAddrSpec		(Ptr& p, const Ptr& e, MAddrSpec& addrspec);
	//	int GetAddrSpecList	(Ptr& p, const Ptr& e, MAddrSpecList& addrspecs);
		int GetLocalPart	(Ptr& p, const Ptr& e, crope& local_part);
		int GetDomain		(Ptr& p, const Ptr& e, crope& domain);
		int GetSubDomain	(Ptr& p, const Ptr& e, crope& sub_domain);
		int GetDomainRef	(Ptr& p, const Ptr& e, crope& domain_ref);
	};

#endif

#endif

