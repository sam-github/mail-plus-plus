//
//
//

#include <mail++/drums.h>

void t_datetime()
{
	const char* dts[] = {
		"Fri, 21 Nov 1997 09:55:06 -0600",
		"Fri, 21 Nov 1997 09:55:06 -0600",
		"Tue,  1 Jul 2003 10:52:37 +0200",
		"Thu, 13 Feb 1969 23:32:54 -0330",
		"Fri, 21 Nov 1997 09:55:06 -0600",
		"Fri, 21 Nov 1997 10:01:10 -0600",
		"Fri, 21 Nov 1997 11:00:00 -0600",
		"Fri, 21 Nov 1997 09:55:06 -0600",
		"Mon, 24 Nov 1997 14:22:01 -0800",
		"Fri, 21 Nov 1997 09:55:06 -0600",
		"Fri, 21 Nov 1997 09:55:06 -0600",
		"Tue,  1 Jul 2003 10:52:37 +0200",
		"     21 Nov 97 09:55:06 GMT",
		"Fri, 21 Nov 1997 09(comment):   55  :  06 -0600",
		"     21 Nov 1997 10:05:43 -0600",
		"     21 Nov 1997 10:01:22 -0600",
		"Thu,\r\n"
		"    13\r\n"
		"       Feb\r\n"
		"          1969\r\n"
		"                23:32\r\n"
		"             -0330 (Newfoundland Time)",
		"Fri, 21 Nov 1997 09(comment):   55  :  06 -0600",
		0
	};

	MDateTimeParser parser;

	for(int i = 0; dts[i]; ++i)
	{
		MDateTime dt;

		crope text = dts[i];

		int ok = parser.DateTime(text, dt);

		cout
			<< setw(2) << i << ": "
			<< dts[i] << " -> " << dt.Text() << endl;
	}
}

int main()
{
	t_datetime();

	return 0;
}

