//
//
//

#include <mail++/quote.h>
#include <assert.h>

int main()
{
	crope ascii;

	// these are NOT C strings, they have an embedded nul!
	char input[]  = {      '\0', '\a',       '\r', 'a',       '\n', ' ',       '"', '&', '%', '3', '\x90', '9', '?', 'F', '_', '`', 's', 'm', '}', '~', '\x7F'      };
	char output[] = { '"', '\0', '\a', '\\', '\r', 'a', '\\', '\n', ' ', '\\', '"', '&', '%', '3', '\x90', '9', '?', 'F', '_', '`', 's', 'm', '}', '~', '\x7F', '"' };

	int cr	= 1;
	int nl	= 1;
	int nul	= 1;
	int ctl	= 2;
	int ext	= 1;

	int szin = sizeof(input);
	int szout = sizeof(output);

	crope rin(input,  szin);
	crope rout(output, szout);

	assert(rin.size()  == szin);
	assert(rout.size() == szout);

	MQuoter	q; crope quoted = rin;

	assert(quoted.size() == rin.size());

	q.Quote(quoted);

	assert(quoted.size() == szout);
	assert(quoted == rout);

	assert(cr	== q.cr);
	assert(nl	== q.nl);
	assert(nul	== q.nul);
	assert(ctl	== q.ctl);
	assert(ext	== q.ext);
	assert(q.total == rin.size());

	return 0;
}

/*
	for(int i = 0; i < szout; ++i) {
		char q = quoted[i];
		char c = output[i];
		if(q != c)
			;
	}
*/

