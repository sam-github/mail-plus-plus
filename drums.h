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
* Core rules from Appendix A of RFC 2234, ABNF.
*/
class MCoreTokenizer
{
private:
	typedef crope::const_iterator Ptr;

public:
	int	IsALPHA			(Ptr& p, const Ptr& e)
	{
		// ALPHA =  %x41-5A / %x61-7A ; A-Z / a-z

		if(p == e)
			return 0;

		// Use the C library here, and maybe write our own version
		// of these functions for all the following Is*() functions,
		// they'll be faster than all these comparisons.

		if(isalpha(*p)) {
			++p;
			return 1;
		}
		return 0;
	}
	// BIT =  "0" / "1"
	// CHAR =  %x01-7F ; any 7-bit US-ASCII character, excluding NUL
	// CR =  %x0D ; carriage return
	// CRLF =  CR LF ; Internet standard newline
	// CTL =  %x00-1F / %x7F ; controls

	int	IsDIGIT			(Ptr& p, const Ptr& e)
	{
		// DIGIT = %x30-39 ; 0-9

		if(p == e)
			return 0;

		if(*p < '\x30')
			return 0;

		if(*p > '\x39')
			return 0;

		return 1;
	}
	// DQUOTE =  %x22 ; " (Double Quote)
	// HEXDIG =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"

	int IsHTAB(Ptr& p, const Ptr& e)
	{
		// HTAB =  %x09 ; horizontal tab

		return p != e && *p == '\x09';
	}
	// LF =  %x0A ; linefeed
	// LWSP =  *(WSP / CRLF WSP) ; linear white space (past newline)
	// OCTET =  %x00-FF ; 8 bits of data

	int IsSP(Ptr& p, const Ptr& e)
	{
		// SP =  %x20 ; space

		return p != e && *p == 0x20;
	}
	// VCHAR =  %x21-7E ; visible (printing) characters

	int	IsWSP			(Ptr& p, const Ptr& e)
	{
		// WSP =  SP / HTAB ; white space

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
	int GetFWS			(Ptr& p, const Ptr& e)
	{
		crope fws;

		return GetFWS(p, e, fws);
	}
	int GetFWS			(Ptr& p, const Ptr& e, crope& fws)
	{
		// FWS = ([*WSP CRLF] 1*WSP) / obs-FWS
		//
		// rewrite: ([*WSP CRLF] 1*WSP) -> 1*WSP / (*WSP CRLF 1*WSP)

		Ptr save = p;

		int wsp = GetWSP(p, e);

		if(GetCRLF(p, e)) {
			wsp = GetWSP(p, e); 
		}

		if(wsp == 0) {
			p = save;
			return 0;
		}

		fws.append(' ');

		return 1;
	}
		// NO-WS-CTL = %d1-8 / %d11 / %d12 / %d14-31 / %d127
		//      ; ctl chars minus the CR, LF, and WSP
	int GetCText		(Ptr& p, const Ptr& e, crope& ctext)
	{
		// ctext = NO-WS-CTL /		; Non white space controls
		//         %d33-39 /		; The rest of the US-ASCII
		//         %d42-91 /		; chars, except "(", ")",
		//         %d93-126			; or "\".

		if(p == e)
			return 0;

		// do this by searching for disallowed charcters:
		char* notok = "\x009\x00A\x00D\x020()\\";
		char c = *p;

		if(c == '\0' || strchr(notok, c))
			return 0;

		ctext.append(c);
		++p;

		return 1;
	}
	int GetText			(Ptr& p, const Ptr& e, crope& text)
	{
		// text = %d1-9 /		; characters excluding CR and LF
		//        %d11-12 /
		//        %d14-127 /
		//        obs-text

		if(p == e)
			return 0;

		Ptr save = p;

		char c = *p;

		++p;

		if(c >= 1 && c <= 9) {
			text.append(c);
			return 1;
		}
		if(c >= 11 && c <= 12) {
			text.append(c);
			return 1;
		}
		if(c >= 14 && c <= 127) {
			text.append(c);
			return 1;
		}

		p = save;
	
		return GetObsText(p, e, text);
	}
	int GetQuotedPair	(Ptr& p, const Ptr& e, crope& qp)
	{
		// quoted-pair = ("\" text) / obs-qp

		Ptr save = p;

		if(GetChar(p, e, '\\') && GetText(p, e, qp))
			return 1;

		save = p;

		return 0;
	}
	int GetCContent		(Ptr& p, const Ptr& e, crope& cc)
	{
		// ccontent = ctext / quoted-pair / comment

		return
			GetCText(p, e, cc) ||
			GetQuotedPair(p, e, cc) ||
			GetComment(p, e, cc);
	}
	int GetComment		(Ptr& p, const Ptr& e)
	{
		crope comment;

		return GetComment(p, e, comment);
	}
	int GetComment		(Ptr& p, const Ptr& e, crope& comment)
	{
		// comment = "(" *([FWS] ccontent) [FWS] ")"
		//
		// --> "(" *(FWS / ccontent) ")"

		Ptr save = p;

		crope c;

		if(!GetChar(p, e, '('))
			return 0;

		while(GetFWS(p, e, c) || GetCContent(p, e, c))
			;

		if(!GetChar(p, e, ')')) {
			p = save;
			return 0;
		}

		comment = c;

		return 1;
	}
	int GetCFWS			(Ptr& p, const Ptr& e)
	{
		// CFWS = *([FWS] comment) (([FWS] comment) / FWS)
		//
		// --> 1*(FWS / comment)

		int count = 0;

		while(GetFWS(p, e) || GetComment(p, e))
			++count;

		if(!strict_)
			return 1;

		return count;
	}

	// Obsolete Syntax
	int GetObsText			(Ptr& p, const Ptr& e, crope& text)
	{
		// obs-text = *LF *CR *(obs-char *LF *CR)

		int count = 0;

		while(GetChar(p, e, '\n')) {
			++count;
			text.append('\n');
		}

		while(GetChar(p, e, '\r')) {
			++count;
			text.append('\r');
		}

		for(;;)
		{
			if(!GetObsChar(p, e, text))
				return count;
			
			while(GetChar(p, e, '\n')) {
				++count;
				text.append('\n');
			}

			while(GetChar(p, e, '\r')) {
				++count;
				text.append('\r');
			}
		}
	}
	int GetObsChar		(Ptr& p, const Ptr& e, crope& text)
	{
		// obs-char = %d0-9 /		; %d0-127 excluding CR and LF
		//        %d11 /
		//        %d12 /
		//        %d14-127

		if(p == e)
			return 0;

		char c = *p;

		if(c == '\r' || c == '\n')
			return 0;
		
		if(c > 0 && c <= 127) {
			text.append(c);
			++p;
			return 1;
		}
	
		return 0;
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
		p = save;

		return 0;
	}

protected:
	int GetString		(Ptr& p, const Ptr& e, const char* n, crope& name)
	{
		const char* names[] = {
			0,
			0
		};
		names[0] = n;

		return GetEnumString(p, e, names, name);
	}
	int GetEnumString	(Ptr& p, const Ptr& e, const char* names[], crope& name)
	{
		if(p == e)
			return 0;

		Ptr save = p;

		crope t(p, e);
		const char* text = t.c_str();

		for(int j = 0; names[j]; ++j) {
			int l = strlen(names[j]);

			if(strnicmp(text, names[j], l) == 0) {
				p += l;
				name = names[j];
				return 1;
			}
		}

		return 0;
	}
};

#include <mail++/datetime.h>

class MDateTimeParser : public MDrumsTokenizer
{
private:
	typedef crope::const_iterator Ptr;

public:
	MDateTimeParser(int strict = 0) :
		MDrumsTokenizer(strict)
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

		Ptr save = p;

		crope day_name;

		if(GetDayOfWeek(p, e, day_name)) {
			if(!GetChar(p, e, ',')) {
				p = save;
				return 0;
			}
		}

		int		day;
		crope	month;
		int		year;

		if(!GetDate(p, e, day, month, year)) {
			p = save;
			return 0;
		}

		if(!GetFWS(p, e)) {
//			p = save;
//			return 0;
		}

		int hour;
		int	min;
		int	sec;
		int	zone;

		if(!GetTime(p, e, hour, min, sec, zone)) {
			p = save;
			return 0;
		}

		GetCFWS(p, e);

		// should I confirm that p == e?

		date_time = MDateTime(day_name, day, month, year, hour, min, sec, zone);

		return 1;
	}
	int GetDayOfWeek	(Ptr& p, const Ptr& e, crope& day_name)
	{
		// day-of-week = ([FWS] day-name) / obs-day-of-week

		Ptr save = p;

		GetFWS(p, e);

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

		return GetEnumString(p, e, days, day_name);
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

		GetFWS(p, e);
		if(GetDigits(p, e, 1, 2, digits)) {
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

		if(GetFWS(p, e) && GetMonthName(p, e, m) && GetFWS(p, e)) {
//			month = m;
//			return 1;
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

		return GetEnumString(p, e, month_names, month);
	}
	int GetYear			(Ptr& p, const Ptr& e, int& year)
	{
		// year = 4*DIGIT / obs-year

		crope digits;

		if(GetDigits(p, e, 4, 0, digits)) {
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
		if(!GetFWS(p, e)) {
//			p = save; return 0;
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
		if(!GetChar(p, e, ':')) {
			p = save; return 0;
		}
		if(!GetHMS(p, e, m)) {
			p = save; return 0;
		}

		if(GetChar(p, e, ':')) {
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

		if(GetObsHMS(p, e, hms))
			return 1;

		crope digits;

		// Because of the CFWS amibiguity, re-order these rules to
		// try and match the most liberal first.

		if(GetDigits(p, e, 2, 2, digits)) {
			hms = atoi(digits.c_str());
			return 1;
		}

		return 0;
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

		if(GetDigits(p, e, 4, 4, digits)) {
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
		// obs-day-of-week = [CFWS] day-name [CFWS]

		GetCFWS(p, e);

		int ok = GetDayName(p, e, day_name);

		GetCFWS(p, e);

		return ok;
	}
	int GetObsMonth		(Ptr& p, const Ptr& e, crope& month)
	{
		// obs-month = CFWS month-name CFWS

		Ptr save = p;

		if(GetCFWS(p, e) && GetMonthName(p, e, month) && GetCFWS(p, e))
			return 1;

		p = save;

		return 0;
	}
	int GetObsYear		(Ptr& p, const Ptr& e, int& year)
	{
		// obs-year = [CFWS] 2*DIGIT [CFWS]

		Ptr save = p;

		GetCFWS(p, e);

		crope digits;

		if(!GetDigits(p, e, 2, 0, digits)) {
			p = save;
			return 0;
		}

		// I didn't make this up, its from DRUMS 4.3, Obsolete Date
		// and Time

		int y = atoi(digits.c_str());

		if(digits.size() == 2 && y <= 49) {
			year = 2000 + y;
		} else if(digits.size() == 2 || digits.size() == 3) {
			year = 1900 + y;
		} else {
			year = y;
		}

		GetCFWS(p, e);

		return 1;
	}
	int GetObsDay		(Ptr& p, const Ptr& e, int& day)
	{
		// obs-day = [CFWS] 1*2DIGIT [CFWS]

		Ptr save = p;

		GetCFWS(p, e);

		crope digits;

		if(!GetDigits(p, e, 1, 2, digits)) {
			p = save;
			return 0;
		}

		day = atoi(digits.c_str());

		GetCFWS(p, e);

		return 1;
	}
	int GetObsHMS		(Ptr& p, const Ptr& e, int& hms)
	{
		// obs-hour = [CFWS] 2DIGIT [CFWS]
		// obs-minute = [CFWS] 2DIGIT [CFWS]
		// obs-second = [CFWS] 2DIGIT [CFWS]

		Ptr save = p;

		GetCFWS(p, e);

		crope digits;

		if(!GetDigits(p, e, 2, 2, digits)) {
			p = save;
			return 0;
		}

		hms = atoi(digits.c_str());

		GetCFWS(p, e);

		return 1;
	}
	int GetObsZone		(Ptr& p, const Ptr& e, int& zone)
	{
		// obs-zone = "UT" / "GMT" /	; Universal Time
		//
		//								; North American UT offsets
		//            "EST" / "EDT" /	; Eastern: -5 / -4
		//            "CST" / "CDT" /	; Central: -6 / -5
		//            "MST" / "MDT" /	; Mountain: -7 / -6
		//            "PST" / "PDT" /	; Pacific: -8 / -7
		//
		//            %d65-73 /			; Military zones "A" - "I"
		//            %d75-90 /			; and "K" through "Z", either
		//            %d97-105 /		; case.
		//            %d107-122

		if(p == e)
			return 0;

		struct TZMap
		{
			char*	name;
			int		offset;
		};

		static TZMap tzs[] = {
			{ "UT",		0 },
			{ "GMT",	0 },
			{ "EST",	-5 },
			{ "EDT",	-4 },
			{ "CST",	-6 },
			{ "CDT",	-5 },
			{ "MST",	-7 },
			{ "MDT",	-6 },
			{ "PST",	-8 },
			{ "PDT",	-7 },
			0
		};

		crope name;

		for(int i = 0; tzs[i].name; ++i) {
			if(GetString(p, e, tzs[i].name, name)) {
				zone = tzs[i].offset;
				return 1;
			}
		}

		// Any single ALPHA except "J" is so-called military time.
		// It is to be interpreted as "unknown" or UTC time.
		if(IsALPHA(p, e)) {
			if(*p != 'J' && *p != 'j') {
				++p;
				zone = 0;
				return 1;
			}
		}

		// DRUMS suggests that other alphabetic time zones SHOULD be
		// parsed and interpreted as UTC. 

		return 0;
	}
};

#endif

