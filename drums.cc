//
//
//

#include <mail++/drums.h>

#include <assert.h>
#include <ctype.h>

//
// MRfc822Tokenizer
//
#if 0
	//
	// Character Classification - could be rewritten in a C library
	// independent way, my system's C library matches the RFC
	// definitions, but I don't know if that's guaranteed.
	//
	int MRfc822Tokenizer::IsCHAR(char c) const
	{
		return isascii(c);
	}
	int MRfc822Tokenizer::IsCTL(char c) const
	{
		return iscntrl(c) || c == 127 /* DEL */;
	}
	int MRfc822Tokenizer::IsSPACE(char c) const
	{
		return c == ' ';
	}
	int MRfc822Tokenizer::IsHTAB(char c) const
	{
		return c == '\t';
	}
	int MRfc822Tokenizer::IsLWSPChar(char c) const
	{
		return IsSPACE(c) || IsHTAB(c);
	}
	int MRfc822Tokenizer::IsSpecial(char c) const
	{
		return strchr("()<>@,;:\\\".[]", c) ? 1 : 0;
	}
	int MRfc822Tokenizer::IsAtomChar(char c) const
	{
		return IsCHAR(c) && !IsSpecial(c) && !IsSPACE(c) && !IsCTL(c);
	}
	int MRfc822Tokenizer::IsQText(char c) const
	{
		return IsCHAR(c)
			&& c != '"'
			&& c != '\\'
			&& c != '\r';
	}
	int MRfc822Tokenizer::IsSmtpQ(char c) const
	{
		return IsQText(c)
			&& c != '\n';
	}

	//
	// Lexical Analysis - these tokens are all from RFC822,
	// section 3.3, Lexical Tokens, though not all tokens are
	// implemented.
	//

	int MRfc822Tokenizer::SkipWs(Ptr& p, const Ptr& e)
	{
		int ws = 0;

		while((p != e) && IsLWSPChar(*p)) {
			++ws;
			++p;
		}

		return ws;
	}
	int MRfc822Tokenizer::SkipComments(Ptr& p, const Ptr& e)
	{
		int comments;

		crope ignored;

		while(GetComment(p, e, ignored))
			comments++;

		return comments++;
	}
	int MRfc822Tokenizer::GetSpecial(Ptr& p, const Ptr& e, char c)
	{
		assert(IsSpecial(c));

		SkipWs(p, e); // not comments, they start with a special...

		if((p != e) && *p == c) {
			++p;
			return 1;
		}
		return 0;
	}
	int MRfc822Tokenizer::GetComment(Ptr& p, const Ptr& e, crope& comment)
	{
		// comment = "(" *(ctext / quoted-pair / comment) ")"
		// ctext = <any CHAR except "(", ")", "\", & CR, including LWSP>

		if(!GetSpecial(p, e, '('))
			return 0;

		while(p != e)
		{
			char c = *p;

			if(c == ')') {
				++p;
				return 1; // found end-of-comment
			}
			else if(c == '(')
			{
				GetComment(p, e, comment);
			}
			else if(c == '\\')
			{
				GetQuotedPair(p, e, comment);
			}
			else if(c == '\r')
			{
				// invalid character...
				++invalid_chars_;
				++p;
			}
			else if(IsCHAR(c))
			{
				comment.append(c);
				++p;
			}
			else
			{
				// invalid character...
				++invalid_chars_;
				++p;
			}
		}
		++unterminated_comment_;
		return 0; // end-of-comment not found
	}

	int MRfc822Tokenizer::GetAtom(Ptr& p, const Ptr& e, crope& atom)
	{
		// atom = 1*<an atom char>

		SkipComments(p, e);

		int ok = 0;
		while((p != e) && IsAtomChar(*p))
		{
			++ok;
			atom.append(*p);
			++p;
		}
		return ok;
	}
	int MRfc822Tokenizer::GetQuotedPair(Ptr& p, const Ptr& e, crope& qpair)
	{
		// quoted-pair = "\" CHAR

		if(p == e)
			return 0;

		if(*p != '\\')
			return 0;

		++p;
		if(p == e)
			return 0;

		qpair.append(*p);

		++p;

		return 1;
	}
	int MRfc822Tokenizer::GetQuotedString(Ptr& p, const Ptr& e, crope& qstr)
	{
		// quoted-string = <"> *(qtext/quoted-pair) <">
		// qtext = CHAR except <">, "\", & CR, including LWSP-char

		SkipComments(p, e);

		if(!GetSpecial(p, e, '"'))
			return 0;

		while(p != e)
		{
			char c = *p;

			if(c == '"') {
				++p;
				return 1; // found end-of-qstr
			}
			else if(c == '\\')
			{
				GetQuotedPair(p, e, qstr);
			}
			else if(c == '\r')
			{
				// invalid character...
				++p;
				++invalid_chars_;
			}
			else if(IsCHAR(c))
			{
				qstr.append(c);
				++p;
			}
			else
			{
				// invalid character...
				++p;
				++invalid_chars_;
			}
		}
		++unterminated_qstr_;
		return 0; // end-of-qstr not found
	}
	int MRfc822Tokenizer::GetWord(Ptr& p, const Ptr& e, crope& word)
	{
		// word = atom / quoted-string
		SkipComments(p, e);

		Ptr save = p;

		crope qstr;
		if(GetQuotedString(p, e, qstr)) {
			word += qstr;
			return 1;
		}

		p = save;
			// Necessary because the quoted string could have found
			// a partial string (invalid syntax). Thus reset, the atom
			// will fail to if the syntax is invalid.

		crope atom;
		if(GetAtom(p, e, atom)) {
			word += atom;
			return 1;
		}
		p = save;

		return 0;
	}
	int MRfc822Tokenizer::GetPhrase(Ptr& p, const Ptr& e, crope& phrase)
	{
		// phrase = 1*word

		if(!GetWord(p, e, phrase)) {
			return 0;
		}
		// ok, got the 1 word, now append all the others we can
		crope word;
		while(GetWord(p, e, word)) {
			phrase += " ";
			phrase += word;
			word = "";
		}
		return 1;
	}

#endif

//
// MAddressParser
//

#if 0
int MAddressParser::AddrSpec (const crope& text, MAddrSpec& addrspec)
{
	Ptr p = text.begin();
	Ptr e = text.end();

	MAddrSpec as;

	if(!GetAddrSpec(p, e, as))
		return 0;

	lexer.SkipComments(p, e);

	if(p != e)
		return 0;

	addrspec = as;

	return 1;
}
int MAddressParser::AddrSpecList(const crope& text, MAddrSpecList& mboxlist)
{
	return 0;
}
int MAddressParser::MailBox (const crope& text, MMailBox& mailbox)
{
	Ptr p = text.begin();
	Ptr e = text.end();

	MMailBox mb;

	if(!GetMailBox(p, e, mb))
		return 0;

	lexer.SkipComments(p, e);

	if(p != e)
		return 0;

	mailbox = mb;

	return 1;
}
int MAddressParser::MailBoxList (const crope& text, MMailBoxList& mboxlist)
{
	return 1;
}
#if 0
int MAddressParser::GetAddressList()
{
	// address-list = #(address)

	int ok = 0;

	if(!GetAddress())
		return 0;

	++ok;

	lexer.SkipComments(p, e);

	while(lexer.GetSpecial(p, e, ','))
	{
		// Remember that 'a,,b' is a valid list!
		if(GetAddress())
			++ok;
	}

	// A little problem here in that we return the number of
	// addresses found, but if their was trainling garbage on
	// in the text, then we'll just be ignoring that.

	return ok;
}
int MAddressParser::GetAddress()
{
	// address = mailbox / group

	return GetMailBox() || GetGroup();
}
int MAddressParser::GetGroup()
{
	// group = phrase ":" [#mailbox] ";"

	return 0;
}
#endif
int MAddressParser::GetMailBox(Ptr& p, const Ptr& e, MMailBox& mailbox)
{
	// mailbox = addr-spec / phrase route-addr
	//  phrase is called display-name in the revision to RFC 822

	crope display_name;
	MAddrSpec addrspec;

	// -> addr-spec
	if(GetAddrSpec(p, e, addrspec)) {
		mailbox = MMailBox(addrspec);

		return 1;
	}

	// -> phrase route-addr
	Ptr save = p;

	if(!lexer.GetPhrase(p, e, display_name))
		return 0;

	if(!GetRouteAddr(p, e, addrspec)) {
		p = save;
		return 0;
	}

	mailbox = MMailBox(addrspec, display_name);

	return 1;
}
int MAddressParser::GetRouteAddr(Ptr& p, const Ptr& e, MAddrSpec& addrspec)
{
	// route-addr = "<" [route] addr-spec ">"
	//   The optional route is not supported.

	lexer.SkipComments(p, e);

	Ptr save = p;

	if(!lexer.GetSpecial(p, e, '<'))
		return 0;

	MAddrSpec as;

	if(!GetAddrSpec(p, e, as)) {
		p = save;
		return 0;
	}

	lexer.SkipComments(p, e);

	if(!lexer.GetSpecial(p, e, '>')) {
		p = save;
		return 0;
	}

	addrspec = as;

	return 1;
}
int MAddressParser::GetAddrSpec(Ptr& p, const Ptr& e, MAddrSpec& addrspec)
{
	// addr-spec = local-part "@" domain

	Ptr save = p;

	crope local_part;

	if(!GetLocalPart(p, e, local_part))
		return 0;

	lexer.SkipComments(p, e);

	if(!lexer.GetSpecial(p, e, '@')) {
		p = save;
		return 0;
	}

	crope domain;

	if(!GetDomain(p, e, domain)) {
		p = save;
		return 0;
	}

	addrspec = MAddrSpec(local_part, domain);

	return 1;
}
int MAddressParser::GetLocalPart(Ptr& p, const Ptr& e, crope& local_part)
{
	// local-part = word *("." word)
	//   rewrite as ->  word ("." local-part)

	Ptr save = p;

	if(!lexer.GetWord(p, e, local_part)) {
		p = save;
		return 0;
	}
	// we've got a local-part, but keep looking for more

	save = p;

	lexer.SkipComments(p, e);

	if(!lexer.GetSpecial(p, e, '.')) {
		p = save;
		return 1;
	}
	crope more;

	if(!GetLocalPart(p, e, more)) {
		p = save;
		return 1;
	}
	// append more
	local_part += ".";
	local_part += more;

	return 1;
}
int MAddressParser::GetDomain(Ptr& p, const Ptr& e, crope& domain)
{
	// domain = sub-domain *("." sub-domain)
	//  rewrite as -> sub-domain ("." domain)

	if(!GetSubDomain(p, e, domain))
		return 0;

	// we've go the 1, keep looking for more

	lexer.SkipComments(p, e);

	if(!lexer.GetSpecial(p, e, '.')) {
		return 1;
	}

	crope more;
	if(!GetDomain(p, e, more)) {
		return 1;
	}

	domain += ".";
	domain += more;

	return 1;
}
int MAddressParser::GetSubDomain(Ptr& p, const Ptr& e, crope& sub_domain)
{
	// sub-domain = domain-ref / domain-literal
	//   A domain-literal doesn't appear to be used or useable
	//	(at least for dotted-quad or DNS style domains),
	//   so only domain refs are supported.

	return GetDomainRef(p, e, sub_domain);
}
int MAddressParser::GetDomainRef(Ptr& p, const Ptr& e, crope& domain_ref)
{
	// domain-ref = atom

	return lexer.GetAtom(p, e, domain_ref);
}

#endif

