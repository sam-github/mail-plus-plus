//
// mail replacement that can send MIME attachments
//

#include <options.h>
#include <smtp.h>

#include <string>
#include <vector>

#include <iostream.h>
#include <stdlib.h>

#include <mail++/msg_822.h>
#include <mail++/address.h>
#include <mail++/os.h>

typedef crope String;

//
// Utilities
//

int AtoI(const char* s)
{
	return strtol(s, 0, 0);
}

//
// Typedefs
//

typedef vector< String >	StringList;

//
// Options
//

const char* optv[] = {
	"h|help",
	"v",
	"s:subject    <subject>",
	"a+attach     <attachment>",
	"n|nowait",
	"c+cc         <cc-address>",
	"b+bcc        <bcc-address>",
	"f:from       <from>",
	"r:replyto    <replyto-address>",
	"m:mailhost   <mailhost>",
	"p:port       <port>",
	0
};

struct TheOptions
{
	TheOptions() : verbosity(0), nowait(0), mailhost("localhost"), port(25)
	{
		MMailBox mb(MOs::UserName(), MOs::HostName(), MOs::Comment());
		addresses.from = mb.Text();
	}

	int verbosity;

	String subject;

	StringList attachments;

	int	nowait;

	struct
	{
		StringList	to;
		StringList	cc;
		StringList	bcc;
		String		from;
		String		replyto;
	} addresses;

	String	mailhost;
	int		port;
};

TheOptions options;

void Address(MMessage& mail, const StringList& addresses, const crope& fieldname)
{
	if(!addresses.empty())
	{
		crope fieldvalue;
		for(StringList::const_iterator p = addresses.begin();
			p != addresses.end(); ++p)
		{
			//MMailBox mb(*p);

			if(!fieldvalue.empty())
				fieldvalue += ", ";
			fieldvalue += *p; // mb.Text();
		}
		mail.Head().Field(fieldname, fieldvalue);
	}
}

void Address(MMessage& mail, const crope& address, const crope& fieldname)
{

/*
	// need to make sure from is a valid address...
	MMailBox mb(address);

	if(!mb)
	{
		cerr << "from field invalid: " << mb.Text() << endl;
		return 1;
	}
*/

	if(!address.empty())
		mail.Head().Field(fieldname, address);
}

void RcptTo(smtp& client, const MMessage& mail, const StringList& addrs)
{
	for(StringList::const_iterator p = addrs.begin();
		p != addrs.end(); ++p)
	{
		client->rcpt(p->c_str());
	}
}

//
// Mainline
//

void ParseOptions(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
	ParseOptions(argc, argv);

	MMessage mail;

	// Subject:

	if(options.subject.empty() && MOs::IsaTty(0))
	{
		crope& subj = options.subject;

		// empty test not strictly correct, fails on -s ""
		cout << "Subject: " << flush;
		cin >> subj;

		MChopLF(subj);
	}

	if(!options.subject.empty())
		mail.Head().Field("Subject", options.subject);

	// From:

	Address(mail, options.addresses.from, "From");

	// Reply-To:

	Address(mail, options.addresses.replyto, "Reply-To");

	// To:

	Address(mail, options.addresses.to, "To");

	// Cc:

	Address(mail, options.addresses.cc, "Cc");

	// Bcc: not added to the message (that's the "blind" part)

	//
	// Attachments - not currently implemented.
	//

	// Body - stdin is mail body if nowait not optioned

	if(!options.nowait)
	{
		crope body;

		while(cin) // read until EOF or error
			cin >> body;

		MConvertLFToCRLF(body);

		mail.Body(body);
	}

	// Force build of mail message into text.

	mail.Text();

	// debug dump
	cout << mail.Text();

	// Connect to the server and send the mail via SMTP.

	smtp  client(&cout);

	client->connect(options.mailhost.c_str(), options.port);
	client->helo();
	client->mail(options.addresses.from.c_str());

	RcptTo(client, mail, options.addresses.to);

	client->data(mail.Text().c_str(), mail.Text().size());

	client->quit();

	return 0;
}

void LogOpt(const char* name, const char* value)
{
	if(options.verbosity)
	{
		cout << name << ": '" << value << "'" << endl;
	}
}

void ParseOptions(int argc, const char* argv[])
{
	int			opt;
	const char*	optarg;
	unsigned	optctrls =
		Options::PARSE_POS|	// return to-addrs in the loop
		Options::PLUS|		// allow '+' to indicate a long option
		Options::NOGUESSING	// don't allow abbreviations for long options
		;

	Options	o(argv[0], optv, optctrls);

	OptArgvIter	iter(--argc, ++argv);

	for(opt = o(iter, optarg); opt; opt = o(iter, optarg))
	{
		switch(opt)
		{
		case 'h':
			o.usage(cout, "<to-address>...");
			exit(0);
		case 'v':
			options.verbosity++;
			break;
		case 's':
			LogOpt("subject", optarg);
			options.subject = optarg;
			break;
		case 'a':
			LogOpt("attach", optarg);
			options.attachments.push_back(optarg);
			break;
		case 'n':
			LogOpt("nowait", "true");
			options.nowait = 1;
			break;
		case 'c':
			LogOpt("cc-addr", optarg);
			options.addresses.cc.push_back(optarg);
			break;
		case 'b':
			LogOpt("bcc-addr", optarg);
			options.addresses.bcc.push_back(optarg);
			break;
		case 'f':
			LogOpt("from", optarg);
			options.addresses.from = optarg;
			break;
		case 'r':
			LogOpt("replyto-addr", optarg);
			options.addresses.replyto = optarg;
			break;
		case 'm':
			LogOpt("mailhost", optarg);
			options.mailhost = optarg;
			break;
		case 'p':
			LogOpt("port", optarg);
			options.port = AtoI(optarg);
			break;
		case Options::POSITIONAL:
			LogOpt("to-address", optarg);
			options.addresses.to.push_back(optarg);
			break;

		// for debugging:
		case Options::AMBIGUOUS:
			LogOpt("ambiguous", optarg);
			exit(1);
		case Options::BADKWD:
			LogOpt("badkwd", optarg);
			exit(1);
		case Options::BADCHAR:
			LogOpt("badchar", optarg);
			exit(1);
		default:
			cout << "opt: " << hex << opt << " option '" << optarg << "'" << endl;
			exit(1);
		}
	}
}

