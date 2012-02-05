//
// tests the b64buf classes as both an istream and ostream filter
//

#include <mail++/b64buf.h>

#include <fstream.h>
#include <stdio.h>
#include <stdiobuf.h>
#include <stdlib.h>
#include <strstream.h>
#include <unistd.h>

#include <assert.h>

char* _; // for debugging

void Assert(const char* x, const char* y, int line)
{
	if(strcmp(x, y) != 0)
	{
		fprintf(stderr, "failed on %d: x %s y %s\n", line, x, y);
	}
}

#define Assert(X,Y) Assert(X,Y,__LINE__)

void Ostream(const char* str)
{
	// encode into a ss
	strstream ess;

	b64encbuf e(ess.rdbuf());

	ostream eos(&e);

	eos << str << flush;

	ess << '\0'; // terminate ess, don't put a null into the encoder!

	_ = ess.str();

	// decode into a ss
	strstream dss;

	b64decbuf	d(dss.rdbuf());

	ostream dos(&d);

	dos << ess.rdbuf() << flush;

	dss << '\0';

	_ = dss.str();

	Assert(dss.str(), str);
}

void Istream(const char* str)
{
	// encode into a ss
	strstream ss((char*)str, strlen(str));

	b64encbuf e(ss.rdbuf());

	istream eis(&e);

	strstream ess;

	ess << eis.rdbuf() << '\0';

	// decode into a ss
	ostrstream dss;

	b64decbuf	d(ess.rdbuf());

	dss << &d;
	dss << '\0';

	_ = dss.str();

	Assert(ss.str(), dss.str());
}

int main(int argc, char* argv[])
{
	for(int i = 1; i < argc; i++)
	{
		Ostream(argv[i]);
		Istream(argv[i]);
	}

	{
		// write a b64 encoded file with our favorite string...
		const char* ostring = "Hello, Small World!";

		filebuf		ofb;
		if(!ofb.open("tmp.b64", ios::out)) {
			cerr << "ofb.open() of 'tmp.b64' failed" << endl;
			exit(1);
		}
		b64encbuf	eb(&ofb);
		ostream		os(&eb);

		os << ostring << flush;

		// ok, now lets slurp the file back into a strstream, decoding
		// it on the way.

		filebuf		ifb;
		if(!ifb.open("tmp.b64", ios::in)) {
			cerr << "ifb.open() of 'tmp.b64' failed" << endl;
			exit(1);
		}
		b64decbuf	db(&ifb);

		ostrstream	istring;

		istring << &db << '\0';

		Assert(istring.str(), ostring);

		cout << "Yep, '" << istring.str()
			<< "' is the same as '" << ostring << "'" << endl;
	}

	return 1;
}

