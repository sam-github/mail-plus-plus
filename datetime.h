//
// Mail date-time field
//

#ifndef M_DATETIME_H
#define M_DATETIME_H

#include <iomanip.h>
#include <strstream.h>

#include <mail++/rope.h>

#include <time.h>

class MDateTime
{
private:
	typedef const crope& Rope;

	crope	dow;

	int		day;
	crope	month;
	int		year;

	int		hour;
	int		minute;
	int		sec;

	int		zone;

	crope text_;

	void Build()
	{
		strstream ss;

		if(!dow.empty()) {
			ss
				<< ""
				<< dow
				<< ", ";
		}

		ss << setw(2) << day << " " << month << " " << setw(4) << year << " ";
		ss << setfill('0');
		ss << setw(2) << hour << ":";
		ss << setw(2) << minute << ":";
		ss << setw(2) << sec << " ";

		// convert zone in minutes to hhmm
		int z = (zone / 60) * 100 + (zone % 60);
		ss << setiosflags(ios::showpos|ios::internal) << setw(5) << z;

		ss << '\0';

		text_.clear();

		const char* t = ss.str();

		text_ = t;
	}

public:
	MDateTime() :
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
	MDateTime(
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
	}
	MDateTime(time_t utc);

	Rope Text() const { return text_; }
};

#endif

