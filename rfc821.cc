//
//
//

#include <mail++/rfc821.h>
#include <mail++/address.h>
#include <mail++/parse822.h>

int MRfc821Tokenizer::IsC(char c)
{
	return isascii(c) && !IsSpecial(c) && c != ' ';
}
int MRfc821Tokenizer::IsSpecial(char c)
{
	if(strchr("<>()[]\\.,;:@\"", c))
		return 1;

	if(iscntrl(c))
		return 1;

	if(c == 127)
		return 1;

	return 0;
}

/**
* Convert an RFC 822 mailbox to an RFC 821 (SMTP) "path", used as
* the argument to the MAIL and RCPT commands, for instance. The
* path syntax is defined in 4.1.2.
*/
crope MMailBoxToSmtpPath(const MMailBox& mailbox)
{
	// Relevant EBNF (in the syntax of RFC822, not RFC821):
	//
	// path = "<" mailbox ">"
	// mailbox = local-part "@" domain
	// local-part = dot-string / quoted-string
	// dot-string = string *("." string)
	// string = 1*char
	// char = c / "\" x
	// etc....

	// Anyhow, the summary is that if any un-safe characters
	// occur in the local-part, then the whole thing can be made
	// a quoted string, and a few specials have to be quoted
	// inside that string. There's other valid syntax, but we're
	// making a path, not parsing one.

	crope local_part = mailbox.LocalPart();
	MRfc821Tokenizer token;
	int	quoteit = 0;
	for(
		crope::iterator p = local_part.mutable_begin();
		p != local_part.mutable_end();
		++p)
	{
		if(*p == '.')
			continue; // allowed in local-part

		if(token.IsSpecial(*p) || *p == ' ')
		{
			quoteit = 1;
			// some chars need quoting even in a string...
			if(strchr("\r\n\"\\", *p))
				local_part.insert(p, '\\');
		}
	}
	if(quoteit)
	{
		local_part.push_front('"');
		local_part.push_back('"');
	}
	// build the path
	crope path;

	path += '<';
	path += local_part;
	if(!mailbox.Domain().empty())
	{
		path += '@';
		path += mailbox.Domain();
	}
	path += '>';

	return path;
};

