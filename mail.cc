//
// mail replacement that can send MIME attachments
//

#include <options.h>
#include <smtp.h>

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>

#include <mail++/address.h>
#include <mail++/application.h>
#include <mail++/entity.h>
#include <mail++/text.h>
#include <mail++/mime.h>
#include <mail++/message.h>
#include <mail++/multipart.h>
#include <mail++/os.h>
#include <mail++/parse822.h>
#include <mail++/rfc821.h>

typedef crope Rope;

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

typedef vector<Rope>	RopeList;

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
	Rope			subject;
	RopeList		attachments;
	MMailBoxList	to;
	MMailBoxList	cc;
	MMailBoxList	bcc;
	MMailBox		from;
	MMailBox		replyto;
	Rope			mailhost;
	int				port;
};

TheOptions options;

//
// Mainline
//

void ParseOptions(int argc, const char* argv[]);
int RcptTo(smtp& client, const MMessage& mail, const MMailBoxList& addrs);

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
		mail.Head().Field("To", MAddressListToText(options.to));
	}

	// Cc:

	if(!options.cc.empty())
	{
		mail.Head().Field("Cc", MAddressListToText(options.cc));
	}

	// Bcc:  Not added to the message, that's the "blind" part!

	// Subject: Read from stdin if it's a tty, and not provided.

	// Can I read the subject from /dev/tty?

	if(!options.subject.empty())
	{
		mail.Head().Field("Subject", options.subject);
	}
	else if(MOs::IsaTty(0))
	{
		// empty test not strictly correct, fails on -s ""
		cout << "Subject: " << flush;
		cin >> options.subject;

		options.subject = ChopChar(options.subject, '\n');

		mail.Head().Field("Subject", options.subject);
	}

	//
	// Now we collect a vector of mail data, with the first
	// being that collected from stdin (if allowed), followed by
	// any attachments.
	//

	vector<Rope>	parts;

	//
	// Body - stdin is mail body, unless nowait option was selected
	//

	if(!options.nowait)
	{
		crope text;

		while(cin) // read until EOF or error
			cin >> text;

		parts.push_back(text);
	}

	//
	// Attachments
	//

	for(int i = 0; i < options.attachments.size(); ++i)
	{
		const Rope& a = options.attachments[i];

		ifstream	file(a.c_str());

		crope text;

		if(!slurp(file, text))
		{
			cerr << "read from '" << a << "' failed..." << endl;
			return 1;
		}
		ROUT(text)

		parts.push_back(text);
	}

	//
	// Composing the message body.
	//

	MMultipart multipart("mixed");

	// -- make a multipart

	for(int j = 0; j < parts.size(); ++j)
	{
		// Currently classify all attachments as:
		//	a) Unix text
		//	b) application/octet-stream
		//
		// It would be nice to figure out what the type of the file is.

		MEntity e;

		if(IsUnixText(parts[j]))
		{
			MText	text("plain", CanonicalizeText(parts[j]));

			text.Fill(e);
		}
		else
		{
			MApplication app("octet-stream", parts[j]);

			app.Fill(e);

		}
		multipart.Add(e);
	}

	// Fill the mail with the multipart, unless there's only one
	// entity, then fill the mail with that entity.

	if(multipart.Entities())
		multipart.Fill(mail);

	// Dump the message and exit, if requested.

	if(options.dump)
	{
		// debug dump
		cout << mail.Text();
		return 0;
	}

	// Connect to the server and send the mail via SMTP.

	if((options.to.size() + options.cc.size() + options.bcc.size()) == 0)
	{
		cerr << "no need to send this, no addresses provided" << endl;
		return 1;
	}

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
	if(!MAddressParser().MailBox(address, mbox)) {
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

int RcptTo(smtp& client, const MMessage& mail, const MMailBoxList& addrs)
{
	int count = 0;

	for(MMailBoxList::const_iterator p = addrs.begin();
		p != addrs.end(); ++p)
	{
		client->rcpt(MMailBoxToSmtpPath(*p).c_str());

		++count;
	}
	return count;
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

