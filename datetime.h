//
// Mail date-time field
//

#ifndef M_DATETIME_H
#define M_DATETIME_H

#include <mail++/rope.h>

#include <time.h>

class MDateTime
{
private:
	typedef const crope& Rope;

	// the time fields used by mail
	crope	dow;

	int		day;
	crope	month;
	int		year;

	int		hour;
	int		minute;
	int		sec;

	int		zone;
	crope	zonename;

	// the local time info
	time_t	utc_;

	// the time in mail format
	crope text_;

	void Build();
	void ToTime();
	void FromTime();

public:
	MDateTime();
	MDateTime(
		const crope& dow,
		int day, const crope& month, int year,
		int hour, int minute, int sec,
		int zone
		);
	MDateTime(time_t utc);

	time_t Time();

	Rope Text() const;
};

#endif

