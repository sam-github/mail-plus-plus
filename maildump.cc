//
//
//

#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>

#include <options.h>
#include <vector>

#include <mail++/message.h>
#include <mail++/datetime.h>
#include <mail++/drums.h>
#include <mail++/mime.h>

//
// Options
//

const char* optv[] = {
	"h|help",
	"m:mesg       <file>",
	"B|body",
	"H|head",
	"F|fields",
	"i:interpret  <field>",
	"f:field      <field>",
	0
};

//
// Mainline
//

int main(int argc, const char* argv[])
{
	int			opt;
	const char*	optarg;
	unsigned	optctrls =
		Options::PARSE_POS|	// return to-addrs in the loop
		Options::PLUS|		// allow '+' to indicate a long option
		Options::NOGUESSING	// don't allow abbreviations for long options
		;

	Options	o(argv[0], optv, optctrls);

	if(argc == 1) {
		o.usage(cout, "");
		return 0;
	}
	OptArgvIter	iter(--argc, ++argv);

	MMessage*	mail = 0;

# define MAIL if(!mail) { \
			cerr << "-" << opt << " failed, no mesg open!" << endl; \
			break; \
		}

	for(opt = o(iter, optarg); opt; opt = o(iter, optarg))
	{
		switch(opt)
		{
		case 'h':
			o.usage(cout, "");
			exit(0);
	
		case 'm': {
			fstream	file(optarg, ios::in|ios::nocreate);
			if(!file) {
				cerr << "failed to open " << optarg << endl;
				break;
			}
			crope	text;

			while(file >> text)
				; // read all the lines until EOF

			mail = new MMessage(text);
			if(!mail) {
				cerr << "failed to allocate an MMessageRfc822" << endl;
				break;
			}
		  }	break;

		case 'H':
			MAIL
			cout << "--head:" << endl;
			cout << mail->Head().Text() << endl;
			break;

		case 'B':
			MAIL
			cout << "--body:" << endl;
			cout << mail->Body() << endl;
			break;

		case 'F': {
			MAIL
			cout << "--fields:" << endl;
			MField field; // seems to be trouble declaring it in the for-loop
			for(int f = 0; field = mail->Head().Field(f); ++f)
			{
				cout
					<< "#" << f << ": "
					<< "'" << field.Name() << "' -> "
					<< "'" << field.Value() << "'" << endl;
			}
		  }	break;

		case 'i' : {
			MAIL
			if(stricmp(optarg, "date") == 0) {
				MDateTime dt;
				MDateTimeParser parser;
				parser.DateTime(mail->Head().Field(optarg).Value(), dt);

				cout << "--interpreted field: "
					<< "'" << optarg << "' -> "
					<< "'" << dt.Text() << "'" << endl;

				break;
			}
			if(stricmp(optarg, "mime-version") == 0) {
				MMimeVersion version(mail->Head().Field(optarg).Value());

				cout << "--interpreted field: '" << optarg << "' -> ";
				if(version)
					cout << "'" << version.Text() << "'" << endl;
				else
					cout << "no such field" << endl;

				break;
			}
		  	if(stricmp(optarg, "content-type") == 0) {
				MContentType content(mail->Head().Field(optarg).Value());

				cout << "--interpreted field: '" << optarg << "' -> ";
				if(content)
					cout << "'" << content.Text() << "'" << endl;
				else
					cout << "no such field" << endl;

				break;
			}
		  }
		  // drop into the regular field case if field is not interpretable

		case 'f': {
			MAIL
			cout << "--field: '" << optarg << "' -> ";
			if(mail->Head().Field(optarg))
				cout << "'" << mail->Head().Field(optarg).Value() << "'" << endl;
			else
				cout << "no such field" << endl;

		  }	break;

		default:
			cout << "opt: " << hex << opt << " option '" << optarg << "'" << endl;
			exit(1);
		}
	}

	return 0;
}

