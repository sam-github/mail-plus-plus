//
//
//

#ifndef M_RFC_821_H
#define M_RFC_821_H

#include <mail++/rope.h>

class MMailBox;

class MRfc821Tokenizer
{
public:
	int IsC(char c);
	int IsSpecial(char c);
};

/**
* Convert an RFC 822 mailbox to an RFC 821 (SMTP) "path", used as
* the argument to the MAIL and RCPT commands, for instance. The
* path syntax is defined in 4.1.2.
*/
crope MMailBoxToSmtpPath(const MMailBox& mailbox);

#endif

