//
// Parses syntatic elements defined in RFC 2045
//

#include <mail++/parse2045.h>

int MRfc2045Parser::IsTSpecial(/*const*/ Ptr& p, const Ptr& e)
{
	// tspecials = "(" / ")" / "<" / ">" / "@" /
	//             "," / ";" / ":" / "\" / <"> /
	//             "/" / "[" / "]" / "?" / "="

	if(p == e)
		return 0;

	char c = *p;

	if(strchr("()<>@,;:\\\"/[]?=", c))
		return 1;

	return 0;
}
int MRfc2045Parser::GetToken(Ptr& p, const Ptr& e, Rope& token)
{
	// token = 1*<any CHAR except SPACE, CTL, and tspecial>

	int i = 0;

	while(p != e) {
		char c = *p;

		if(!IsCHAR(c))
			break;

		if(IsSPACE(c) || IsCTL(c) || IsTSpecial(p, e))
			break;

		token.append(c);
		++p;
		++i;
	}
	return i;
}
int MRfc2045Parser::GetParameter(Ptr& p, const Ptr& e, Rope& attr, Rope& value)
{
	// parameter = ";" attribute "=" value
	// attribute = token
	// value = token / quoted-string

	Ptr save = p;

	Rope a;
	Rope v;

	SkipComments(p, e);

	if(!GetSpecial(p, e, ';')) {
		p = save;
		return 0;
	}
	SkipComments(p, e);

	if(!GetToken(p, e, a)) {
		p = save;
		return 0;
	}
	SkipComments(p, e);

	if(!GetSpecial(p, e, '=')) {
		p = save;
		return 0;
	}
	SkipComments(p, e);

	if(!GetToken(p, e, v) && !GetQuotedString(p, e, v)) {
		p = save;
		return 0;
	}

	attr	= a;
	value	= v;

	return 1;
}
int MRfc2045Parser::GetContent(Ptr& p, const Ptr& e, Rope& type, Rope& subtype)
{
	// content = type "/" subtype

	Ptr save = p;

	Rope t;
	Rope st;

	SkipComments(p, e);

	if(!GetToken(p, e, t)) {
		p = save;
		return 0;
	}
	SkipComments(p, e);

	if(!GetSpecial(p, e, '/')) {
		p = save;
		return 0;
	}
	SkipComments(p, e);

	if(!GetToken(p, e, st)) {
		p = save;
		return 0;
	}

	type	= t;
	subtype	= st;

	return 1;
}
int MRfc2045Parser::GetEncoding(Ptr& p, const Ptr& e, Rope& encoding)
{
	// encoding = "content-transfer-encoding" ":" mechanism
	// mechanism = "7bit" / "8bit" / "binary" / "quoted-printable"
	//             "base64" / ietf-token / x-token

	Ptr save = p;

	SkipComments(p, e);

	if(!GetToken(p, e, encoding)) {
		p = save;
		return 0;
	}

	return 1;
}

