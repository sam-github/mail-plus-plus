//
// Mail date-time field
//

#include <iomanip.h>
#include <strstream.h>

#include <mail++/datetime.h>

void MDateTime::Build()
{
	strstream ss;

	if(!dow.empty()) {
		ss
			<< ""
			<< dow
			<< ", ";
	}

	ss << setfill('0');
	ss << day << " " << month << " " << setw(4) << year << " ";
	ss << setw(2) << hour << ":";
	ss << setw(2) << minute << ":";
	ss << setw(2) << sec << " ";

	// convert zone in minutes to hhmm
	int z = (zone / 60) * 100 + (zone % 60);
	ss << setiosflags(ios::showpos|ios::internal) << setw(5) << z;
	if(!zonename.empty())
		ss << " (" << zonename << ")";

	ss << '\0';

	text_.clear();

	const char* t = ss.str();

	text_ = t;
}

void MDateTime::ToTime()
{
}
void MDateTime::FromTime()
{
	// tzset();

	struct tm* tm = localtime(&utc_);

	char cday[4];
	strftime(cday, sizeof(cday), "%a", tm);

	char cmonth[4];
	strftime(cmonth, sizeof(cmonth), "%h", tm);

	dow		= cday;
	day		= tm->tm_mday;
	month	= cmonth;
	year	= 1900 + tm->tm_year;
	hour	= tm->tm_hour;
	minute	= tm->tm_min;
	sec		= tm->tm_sec;

	zone	= -timezone / 60; // convert secs from UTC to minutes

	zonename	= daylight ? tzname[0] : tzname[1];
}

MDateTime::MDateTime() :
		dow		(""),
		day		(0),
		month	(""),
		year	(0),
		hour	(0),
		minute	(0),
		sec		(0),
		zone	(0)
{
}
MDateTime::MDateTime(
	const crope& dow,
	int day, const crope& month, int year,
	int hour, int minute, int sec,
	int zone
	) :
		dow		(dow),
		day		(day),
		month	(month),
		year	(year),
		hour	(hour),
		minute	(minute),
		sec		(sec),
		zone	(zone)
{
	Build();
	ToTime();
}

MDateTime::MDateTime(time_t utc) :
		utc_(utc)
{
	FromTime();
	Build();
}

MDateTime::Rope MDateTime::Text() const
{
	return text_;
}

