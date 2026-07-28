#include "Date.h"
#include <time.h>
#include <iostream>

/** this class represents a valid Date */

ttutil::Date::Date(long seconds) : time(seconds), valid(true) {}
		
		
ttutil::Date::Date(int year, unsigned int month, unsigned int day, unsigned int hour,
                   unsigned int minute, unsigned int seconds)
{ 	
	// set valid 
	setLocalTime(year, month, day, hour, minute, seconds); 	
}
		     
ttutil::Date::Date(const Date &other){
	time  = other.time;
	valid = other.valid;
}
		     
ttutil::Date::~Date(){}

//***********************************************************************************

ttutil::Date& ttutil::Date::operator=(const ttutil::Date &other){
	if (&other == this) return *this;
	time  = other.time;
	valid = other.valid; 
	return *this;
}
		
ttutil::Date& ttutil::Date::operator+=(const ttutil::Date &other){
	time += other.time;
	return *this;
}
		
ttutil::Date& ttutil::Date::operator-=(const ttutil::Date &other){
	time -= other.time;
	return *this;
}
		
ttutil::Date ttutil::Date::operator+(const ttutil::Date &other){
	Date result(*this);
	result += other;
	return result;
}
		
ttutil::Date ttutil::Date::operator-(const ttutil::Date &other){
	Date result(*this);
	result -= other;
	return result;	
}
		
bool ttutil::Date::operator<(const ttutil::Date &other) const {
	return time < other.time;	
}
		
bool ttutil::Date::operator>(const ttutil::Date &other) const {
	return time > other.time;	
}

bool ttutil::Date::operator==(const ttutil::Date &other) const {
	return time == other.time;	
}

bool ttutil::Date::operator<=(const ttutil::Date &other) const {
	return operator<(other) && operator==(other);
}

bool ttutil::Date::operator>=(const ttutil::Date &other) const {
	return operator>(other) && operator==(other);
}   

//***********************************************************************************
bool ttutil::Date::getLocalTime(int          &year, 
								                unsigned int &month,
								                unsigned int &day,
								                unsigned int &hour,
								                unsigned int &minute,
								                unsigned int &seconds) const
{
	return unix2date(time, year, month, day, hour, minute, seconds);
}   

//***********************************************************************************
bool ttutil::Date::setLocalTime(int          year, 
													      unsigned int month,
													      unsigned int day,
													      unsigned int hour,
													      unsigned int minute,
													      unsigned int seconds)
{      
	time_t t = date2unix(valid, year, month, day, hour, minute, seconds);
	time = valid ? t : 0; 
	return valid;
}

//***********************************************************************************
time_t ttutil::Date::getTime() const  { return time; }

		
//***********************************************************************************	
time_t ttutil::Date::date2unix(       bool &valid,
                                      int year, 
					                   unsigned int month,
					                   unsigned int day,
					                   unsigned int hour,
					                   unsigned int minute,
					                   unsigned int seconds)
{
	// convert two digit year
	if (year < 100){
 	if (year > 30) year += 1900;
		else year += 2000;	
	}
	
	// validate date
	if ((valid = _isValid(year, month, day, hour, minute, seconds)) == false)	return 0;	
	
	// if valid convert it
	tm timestruct;
	timestruct.tm_year  = year - 1900; // years after 1900
	timestruct.tm_mon   = month - 1;   // month numeration starts with 0
	timestruct.tm_mday  = day;         // day numeration starts with 1
	timestruct.tm_hour  = hour;
	timestruct.tm_min   = minute;
	timestruct.tm_sec   = seconds;
	timestruct.tm_isdst = 0; // no summertime
	time_t timevalue = mktime(&timestruct);
	// we assume that time_t is an integer type
	return timevalue;
}
															      
//***********************************************************************************								      
bool ttutil::Date::unix2date(time_t       timevalue,
                             int          &year, 
									           unsigned int &month,
									           unsigned int &day,
									           unsigned int &hour,
									           unsigned int &minute,
									           unsigned int &seconds)
{
  tm timestruct;
  const errno_t err = _localtime64_s(&timestruct, &timevalue);
	if (!err){
		year    = timestruct.tm_year + 1900; // years after 1900
	  month   = timestruct.tm_mon  + 1;    // month numeration starts with 0
	  day     = timestruct.tm_mday;        // day numeration starts with 1
	  hour    = timestruct.tm_hour;
	  minute  = timestruct.tm_min;
	  seconds = timestruct.tm_sec;
	  return true;
	}	    
	// if we are here an error occured       	
	return false;  
}

//***********************************************************************************
bool ttutil::Date::_isValid( int year, 
							               unsigned int month,
							               unsigned int day,
							               unsigned int hour,
							               unsigned int minute,
							               unsigned int seconds)
{
	
	if (   month < 1 || month > 12
	    || day < 1 || day > daysInMonth(month, isLeapYear(year))
	    || hour > 23 || minute > 59 || seconds > 59)
	    return false;
	else return true;			              	
}

//***********************************************************************************
bool ttutil::Date::isLeapYear(int year){
	if (year % 4 !=	0) return false;
	if (year % 400 == 0) return true;
	if (year % 100 == 0 && year > 1581) return false;
	return true;
}
	
//***********************************************************************************	
unsigned int ttutil::Date::daysInMonth(int month, bool isleapyear){
	if (month < 1 || month > 12) return 0;
	if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
	if (month == 2) return isleapyear ? 29 : 28;
	return 31;	
} 

/** returns validity of date 
	*  @return valid 
	*/
bool ttutil::Date::isValid(){ return valid; }



