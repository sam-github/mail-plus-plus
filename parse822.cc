//
//
//

#include <mail++/parse822.h>

#include <assert.h>
#include <ctype.h>

//
// MRfc822Tokenizer
//

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
	int MRfc822Tokenizer::GetSpecial(char c, Ptr& p, const Ptr& e)
	{
		assert(IsSpecial(c));

		SkipWs(p, e); // not comments, they start with a special...

		if((p != e) && *p == c) {
			++p;
			return 1;
		}
		return 0;
	}
	int MRfc822Tokenizer::GetComment(crope& comment, Ptr& p, const Ptr& e)
	{
		// comment = "(" *(ctext / quoted-pair / comment) ")"
		// ctext = <any CHAR except "(", ")", "\", & CR, including LWSP>

		if(!GetSpecial('(', p, e))
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
				GetComment(comment, p, e);
			}
			else if(c == '\\')
			{
				GetQuotedPair(comment, p, e);
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
	int MRfc822Tokenizer::SkipComments(Ptr& p, const Ptr& e)
	{
		int comments;

		crope ignored;

		while(GetComment(ignored, p, e))
			comments++;

		return comments++;
	}
	int MRfc822Tokenizer::GetAtom(crope& atom, Ptr& p, const Ptr& e)
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
	int MRfc822Tokenizer::GetQuotedPair(crope& qpair, Ptr& p, const Ptr& e)
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
	int MRfc822Tokenizer::GetQuotedString(crope& qstr, Ptr& p, const Ptr& e)
	{
		// quoted-string = <"> *(qtext/quoted-pair) <">
		// qtext = CHAR except <">, "\", & CR, including LWSP-char

		SkipComments(p, e);

		if(!GetSpecial('"', p, e))
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
				GetQuotedPair(qstr, p, e);
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
	int MRfc822Tokenizer::GetWord(crope& word, Ptr& p, const Ptr& e)
	{
		// word = atom / quoted-string
		SkipComments(p, e);

		Ptr save = p;

		crope qstr;
		if(GetQuotedString(qstr, p, e)) {
			word += qstr;
			return 1;
		}

		p = save;
			// Necessary because the quoted string could have found
			// a partial string (invalid syntax). Thus reset, the atom
			// will fail to if the syntax is invalid.

		crope atom;
		if(GetAtom(atom, p, e)) {
			word += atom;
			return 1;
		}
		p = save;

		return 0;
	}
	int MRfc822Tokenizer::GetPhrase(crope& phrase, Ptr& p, const Ptr& e)
	{
		// phrase = 1*word

		if(!GetWord(phrase, p, e)) {
			return 0;
		}
		// ok, got the 1 word, now append all the others we can
		crope word;
		while(GetWord(word, p, e)) {
			phrase += " ";
			phrase += word;
			word = "";
		}
		return 1;
	}

//
// MAddressParser
//

	int MAddressParser::GetAddressList()
	{
		// address-list = #(address)

		int ok = 0;

		if(!GetAddress())
			return 0;

		++ok;

		lexer.SkipComments(p, e);

		while(lexer.GetSpecial(',', p, e))
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
	// Not supported, is it deprecated? If so, I can feel better
	// about this.
	int MAddressParser::GetGroup()
	{
		// group = phrase ":" [#mailbox] ";"

		return 0;
	}
	int MAddressParser::GetMailBox()
	{
		// mailbox = addr-spec / phrase route-addr

		crope local_part;
		crope domain;
		crope display_name;

		// -> addr-spec
		if(GetAddrSpec(local_part, domain)) {
			MMailBox mbox(local_part, domain);
			mailboxes_.push_back(mbox);

			return 1;
		}

		// -> phrase route-addr
		Ptr save = p;

		if(!lexer.GetPhrase(display_name, p, e))
			return 0;

		if(!GetRouteAddr(local_part, domain)) {
			p = save;
			return 0;
		}

		MMailBox mbox(local_part, domain, display_name);

		mailboxes_.push_back(mbox);

		return 1;
	}
	int MAddressParser::GetRouteAddr(crope& local_part, crope& domain)
	{
		// route-addr = "<" [route] addr-spec ">"
		//   The optional route is not supported.

		lexer.SkipComments(p, e);

		Ptr save = p;

		if(!lexer.GetSpecial('<', p, e))
			return 0;

		crope lp;
		crope d;

		if(!GetAddrSpec(lp, d)) {
			p = save;
			return 0;
		}

		lexer.SkipComments(p, e);

		if(!lexer.GetSpecial('>', p, e)) {
			p = save;
			return 0;
		}

		local_part = lp;
		domain = d;

		return 1;
	}
	int MAddressParser::GetAddrSpec(crope& local_part, crope& domain)
	{
		// addr-spec = local-part "@" domain

		Ptr save = p;

		crope lp;

		if(!GetLocalPart(lp))
			return 0;

		lexer.SkipComments(p, e);

		if(!lexer.GetSpecial('@', p, e)) {
			p = save;
			return 0;
		}

		crope d;

		if(!GetDomain(d)) {
			p = save;
			return 0;
		}

		local_part = lp;
		domain = d;

		return 1;
	}
	int MAddressParser::GetLocalPart(crope& local_part)
	{
		// local-part = word *("." word)
		//   rewrite as ->  word ("." local-part)

		Ptr save = p;

		if(!lexer.GetWord(local_part, p, e)) {
			p = save;
			return 0;
		}
		// we've got a local-part, but keep looking for more

		save = p;

		lexer.SkipComments(p, e);

		if(!lexer.GetSpecial('.', p, e)) {
			p = save;
			return 1;
		}
		crope more;

		if(!GetLocalPart(more)) {
			p = save;
			return 1;
		}
		// append more
		local_part += ".";
		local_part += more;

		return 1;
	}
	int MAddressParser::GetDomain(crope& domain)
	{
		// domain = sub-domain *("." sub-domain)
		//  rewrite as -> sub-domain ("." domain)

		if(!GetSubDomain(domain))
			return 0;

		// we've go the 1, keep looking for more

		lexer.SkipComments(p, e);

		if(!lexer.GetSpecial('.', p, e)) {
			return 1;
		}

		crope more;
		if(!GetDomain(more)) {
			return 1;
		}

		domain += ".";
		domain += more;

		return 1;
	}
	int MAddressParser::GetSubDomain(crope& sub_domain)
	{
		// sub-domain = domain-ref / domain-literal
		//   A domain-literal doesn't appear to be used or useable
		//	(at least for dotted-quad or DNS style domains),
		//   so only domain refs are supported.

		return GetDomainRef(sub_domain);
	}
	int MAddressParser::GetDomainRef(crope& domain_ref)
	{
		// domain-ref = atom

		return lexer.GetAtom(domain_ref, p, e);
	}

