//
// Tests the DRUMS parser.
//
// $Log$
// Revision 1.2  2000/02/07 05:12:00  sam
// parsing new syntax, but some problems with CFWS in drums
// and grammer ambiguities with obsolete syntax
//

#include <mail++/drums.h>

#include <iostream.h>
#include <iomanip.h>

void t_datetime()
{
	const char* dts[] = {

		"Fri,(c) 21 Nov 1997 09:55:06 -0600",

		"Thu,\r\n"
		"    13\r\n"
		"       Feb\r\n"
		"          1969\r\n"
		"                23:32\r\n"
		"      €       -0330 (Newfoundland Time)",

		"Fri , 21 Nov 1997 09():55:06 -0600",
		"Fri(), 21 Nov 1997 09():55:06 -0600",
		"Fri (), 21 Nov 1997 09():55:06 -0600",
		"Fri() , 21 Nov 1997 09():55:06 -0600",

		"Fri,()21 Nov 1997 09():55:06 -0600",
		"Fri, ()21 Nov 1997 09():55:06 -0600",
		"Fri, () 21 Nov 1997 09():55:06 -0600",
		"Fri,() 21 Nov 1997 09():55:06 -0600",

		"Fri, 21()Nov 1997 09():55:06 -0600",
		"Fri, 21 ()Nov 1997 09():55:06 -0600",
		"Fri, 21 () Nov 1997 09():55:06 -0600",
		"Fri, 21() Nov 1997 09():55:06 -0600",

		"21 Nov 97 13:40:65 -0600",
		"Fri, 21 Nov 1997 09():55:06 -0600",
		"Fri, 21 Nov 1997 09 :   55  :  06 -0600",
		"Fri, 21 Nov 1997 09 (comment) :   55  :  06 -0600",
		"Fri, 21 Nov 1997 ()09 :55:06 -0600",
		"Fri, 21 Nov 1997 09:   55:06 -0600",
		"(hello!) Fri, 21 Nov 1997(comment)09:55:06 -0600",
		"21 Nov 97 09:55:06 GMT",
		"21 Nov 97 13:40:65 -0600",
		"Fri, 21 Nov 1997 10:01:10 -0600",
		"Tue, 1 Jul 20003 10:52:37 +0200",
		"Thu, 13 Feb 48 23:32 -0330",
		"Thu, 13 Feb 49 23:32 -0330",
		"Thu, 13 Feb 50 23:32 -0330",
		"Fri, 21 Nov 1997 09:55 -0600",
		"Tue, 1 Jul 2063 10:52:37 +0200",
		"Fri, 21 Nov 1997 09:55:06 -0600",
		0
	};

	MDateTimeParser parser;

	for(int i = 0; dts[i]; ++i)
	{
		MDateTime dt;

		crope text = dts[i];

		int ok = parser.DateTime(text, dt);

		cout << setw(2) << i << ": " << dts[i] << "\t-> ";

		if(ok) {
			cout << dt.Text() << endl;
		} else {
			cout << "failed!" << endl;
		}
	}
}

void t_localtime()
{
	MDateTime dt(time(0));

	cout << "localtime -> " << dt.Text() << endl;
}

int main()
{
	//t_datetime();
	t_localtime();

	return 0;
}

