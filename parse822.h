//
// Parses syntatic elements defined in RFC 822
//

#ifndef M_PARSE822_H
#define M_PARSE822_H

#include <rope>
#include <vector>

/**
* Reads an RFC822 defined lexical token from an input, if some of
* the names seem strange, they are taken from the names as defined
* in the extended BNF of the RFC.
*/
class MRfc822Tokenizer
{
private:
	typedef crope::const_iterator Ptr;

	int	errors_;
	int	unterminated_qstr_;
	int unterminated_comment_;
	int invalid_chars_;

public:
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

	//
	// Lexical Analysis - these tokens are all from RFC822,
	// section 3.3, Lexical Tokens, though not all tokens are
	// implemented.
	//

	int SkipWs(Ptr& p, const Ptr& e);
	int GetSpecial(char c, Ptr& p, const Ptr& e);
	int GetComment(crope& comment, Ptr& p, const Ptr& e);
	int SkipComments(Ptr& p, const Ptr& e);
	int GetAtom(crope& atom, Ptr& p, const Ptr& e);
	int GetQuotedPair(crope& qpair, Ptr& p, const Ptr& e);
	int GetQuotedString(crope& qstr, Ptr& p, const Ptr& e);
	int GetWord(crope& word, Ptr& p, const Ptr& e);
	int GetPhrase(crope& phrase, Ptr& p, const Ptr& e);

private:
};

#include <mail++/address.h>

class MAddressParser
{
public:
	int Ok() const;

	int MailBox(MMailBox& mailbox, const crope& text)
	{
		text_	= text;
		p		= text_.begin();
		e		= text_.end();

		if(GetMailBox())
		{
			mailbox = mailboxes_.back();
			return 1;
		}
		return 0;
	}
private:
	MRfc822Tokenizer lexer;

	typedef crope::const_iterator Ptr;

	// the input
	crope	text_;
	Ptr		p;
	Ptr		e;

	// the output
	MMailBoxList	mailboxes_;

	// need a real error reporting object
	int	ok_;

	int	GetAddressList();
	int GetAddress();
	int GetGroup();
	int GetMailBox();
	int GetRouteAddr(crope& local_part, crope& domain);
	int GetAddrSpec(crope& local_part, crope& domain);
	int GetLocalPart(crope& local_part);
	int GetDomain(crope& domain);
	int GetSubDomain(crope& sub_domain);
	int GetDomainRef(crope& domain_ref);
};

#endif

