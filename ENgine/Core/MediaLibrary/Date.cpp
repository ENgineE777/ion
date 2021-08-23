#include "Date.h"

#ifdef ANDROID
#include <time.h>
#endif

#ifdef PC
void Date::Init(const FILETIME& file_time)
{
	SYSTEMTIME sys_time;
	::FileTimeToSystemTime(&file_time, &sys_time);

	hour = sys_time.wHour;
	minute = sys_time.wMinute;
	second = sys_time.wSecond;

	day = sys_time.wDay;
	month = sys_time.wMonth;
	year = sys_time.wYear;
}
#endif

#ifdef ANDROID
void Date::Init(time_t file_time)
{
	struct tm tm;
	gmtime_r(&file_time, &tm);

	hour = tm.tm_hour;
	minute = tm.tm_min;
	second = tm.tm_sec;

	day = tm.tm_mday;
	month = tm.tm_mon + 1;
	year = tm.tm_year + 1900;
}
#endif

#ifdef IOS
void Date::Init(int day, int month, int year)
{
    this->day = day;
    this->month = month;
    this->year = year;
}
#endif

bool Date::operator<(const Date& rhs) const
{
	if (year == rhs.year)
	{
		if (month == rhs.month)
		{
			if (day == rhs.day)
			{
				if (hour == rhs.hour)
				{
					if (minute == rhs.minute)
					{
						return (second < rhs.second);
					}

					return (minute < rhs.minute);
				}

				return (hour < rhs.hour);
			}

			return (day < rhs.day);
		}

		return (month < rhs.month);
	}

	return (year < rhs.year);
}