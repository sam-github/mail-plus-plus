//
// test utility to determine whether a file is representable as
// unix text.
//

#include <mail++/rope.h>
#include <mail++/istext.h>

#include <iostream.h>

int main()
{
	crope text;

	if(!slurp(cin, text))
	{
		cerr << "read from stdin failed!" << endl;
		return 1;
	}

	cout << "stdin is ";
	if(!IsUnixText(text))
		cout << "not ";

	cout << "Unix text" << endl;

	return 0;
}

