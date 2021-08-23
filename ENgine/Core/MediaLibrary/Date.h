#pragma once

#ifdef PC
#include <windows.h>
#endif

#ifdef ANDROID
#include <sys/types.h>
#endif

class Date
{
public:
	Date() : hour(0), minute(0), second(0), day(0), month(0), year(0)
	{
	}

	#ifdef PC
	void Init(const FILETIME& file_time);
	#endif

	#ifdef ANDROID
	void Init(time_t file_time);
	#endif
    
    #ifdef IOS
    void Init(int day, int month, int year);
    #endif

	inline int GetHour() const { return hour; }
	inline int GetMinute() const { return minute; }
	inline int GetSecond() const { return second; }

	inline int GetDay() const { return day; }
	inline int GetMonth() const { return month; }
	inline int GetYear() const { return year; }

	bool operator<(const Date& rhs) const;

private:
	int hour;
	int minute;
	int second;

	int day;
	int month;
	int year;
};