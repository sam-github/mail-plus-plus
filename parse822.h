//
// Parses syntatic elements defined in RFC 822
//

#ifndef M_PARSE822_H
#define M_PARSE822_H

#include <mail++/rope.h>
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
};

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

