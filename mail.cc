//
// mail replacement that can send MIME attachments
//

#include <options.h>

#include <string>
#include <vector>

#include <iostream.h>
#include <stdlib.h>

#include "msg_822.h"
#include "m_os.h"

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
	TheOptions() : verbosity(0) {}

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

//
// Mainline
//

void ParseOptions(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
	ParseOptions(argc, argv);

	MMessage mail;

	if(options.subject.empty() && MOs::IsaTty(0))
	{
		crope& subj = options.subject;

		// empty test not strictly correct, fails on -s ""
		cout << "Subject: " << flush;
		cin >> subj;

		MChopLF(subj);
	}

	mail.Head().Field("Subject", options.subject);

	if(options.addresses.from.empty()) {
		options.addresses.from =
			crope(MOs::UserName()) + "@" + crope(MOs::HostName());
	}
	mail.Head().Field("From", options.addresses.from);


	// stdin is mail body if nowait not optioned
	if(!options.nowait)
	{
		crope body;

		while(cin) // read until EOF or error
			cin >> body;

		MConvertLFToCRLF(body);

		mail.Body(body);
	}


	mail.Head().Field("From", options.addresses.from);

	cout << mail.Text();

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

	Options	o(argv[0], optv, Options::PARSE_POS|Options::PLUS);

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


