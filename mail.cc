//
// mail replacement that can send MIME attachments
//

#include <options.h>
#include <smtp.h>

#include <iostream.h>
#include <stdlib.h>

#include <mail++/msg_822.h>
#include <mail++/address.h>
#include <mail++/parse822.h>
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
	"v|verbose",
	"d|dump",
	"s?subject    <subject>",	// optional is used to suppress prompt
	"a+attach     <attachment>",
	"n|nowait",
	"t+to         <to-address>",
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
	TheOptions() :
		verbosity(0), nowait(0), mailhost("localhost"), port(25),
		from(MOs::UserName(), MOs::HostName(), MOs::Comment())
	{
	}

	int				verbosity;
	int				dump;
	int				nowait;
	String			subject;
	StringList		attachments;
	MMailBoxList	to;
	MMailBoxList	cc;
	MMailBoxList	bcc;
	MMailBox		from;
	MMailBox		replyto;
	String			mailhost;
	int				port;
};

TheOptions options;

void RcptTo(smtp& client, const MMessage& mail, const MMailBoxList& addrs)
{
	for(MMailBoxList::const_iterator p = addrs.begin();
		p != addrs.end(); ++p)
	{
		client->rcpt(p->Text().c_str());
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

	// From:

	mail.Head().Field("From", options.from.Text());

	// Reply-To:

	if(!options.replyto.Text().empty())
	{
		mail.Head().Field("Reply-To", options.replyto.Text());
	}

	// To:

	if(!options.to.empty())
	{
		mail.Head().Field("To", MMailBoxListText(options.to));
	}

	// Cc:

	if(!options.cc.empty())
	{
		mail.Head().Field("Cc", MMailBoxListText(options.cc));
	}

	// Bcc:  Not added to the message, that's the "blind" part!

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

	if(options.dump)
	{
		// debug dump
		cout << mail.Text();
		return 0;
	}

	// Connect to the server and send the mail via SMTP.

	smtp  client(&cout);

	client->connect(options.mailhost.c_str(), options.port);
	client->helo();
	client->mail(options.from.Text().c_str());

	RcptTo(client, mail, options.to);
	RcptTo(client, mail, options.cc);
	RcptTo(client, mail, options.bcc);

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
void Address(MMailBox& mbox, const char* field, const crope& address)
{
	if(!MAddressParser().MailBox(mbox, address)) {
		cerr << address << " isn't a valid " << field << " address" << endl;
		exit(1);
	}
}
void Address(MMailBoxList& mboxlist, const char* field, const crope& address)
{
	MMailBox mbox;
	Address(mbox, field, address);
	mboxlist.push_back(mbox);
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
		case 'd':
			LogOpt("dump", "true");
			options.dump = 1;
			break;
		case 'v':
			options.verbosity++;
			break;
		case 's':
			LogOpt("subject", optarg);
			options.subject = optarg ? optarg : " ";
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
			Address(options.cc, "cc", optarg);
			break;
		case 'b':
			LogOpt("bcc-addr", optarg);
			Address(options.cc, "bcc", optarg);
			break;
		case 'f':
			LogOpt("from", optarg);
			Address(options.from, "from", optarg);
			break;
		case 'r':
			LogOpt("replyto-addr", optarg);
			Address(options.replyto, "replyto", optarg);
			break;
		case 'm':
			LogOpt("mailhost", optarg);
			options.mailhost = optarg;
			break;
		case 'p':
			LogOpt("port", optarg);
			options.port = AtoI(optarg);
			break;
		case 't':
		case Options::POSITIONAL:
			LogOpt("to-address", optarg);
			Address(options.to, "to", optarg);
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

