#pragma once

#include <time.h>

namespace ttutil{
	 
/** this class represents a valid Date 
 *  days in month from 1 to 28,29,30,31
 *  month from 1 to 12 (this is different from the C and Java style which goes from 0 to 11)
 *  Thiele 
 */
class Date{

	public:
		
		/** contructor 1 / default constructor 
		 *  @param milliseconds since 01.01.1970 
		 */
		Date(long seconds = 0);
		
		/** contructor 2 
		 *  @param year 
		 *  @param month
		 *  @param day
		 *  @param hour
		 *  @param minute
		 *  @param seconds
		 *  @param milliseconds
		 */
		Date(         int year, 
		     unsigned int month,
		     unsigned int day,
		     unsigned int hour    = 0,
		     unsigned int minute  = 0,
		     unsigned int seconds = 0);
		     
		/** copy contructor */
		Date(const Date &other);
		     
		/** destructor */
		~Date();
		
		/** assignment operator */
		Date& operator=(const Date &other);
		
		/** add assign operator */
		Date& operator+=(const Date &other);
		
		/** sub assign operator */
		Date& operator-=(const Date &other);
		
		/** addition operator */
		Date operator+(const Date &other);
		
		/** subtraction operator */
		Date operator-(const Date &other);
		
		/** compare operator < */
		bool operator<(const Date &other) const;
		
		/** compare operator > */
		bool operator>(const Date &other) const;
		
		/** compare operator == */
		bool operator==(const Date &other) const;
		
		/** compare operator <= */
		bool operator<=(const Date &other) const;
		
		/** compare operator >= */
		bool operator>=(const Date &other) const;
		
		/** returns the time value 
		 *  @param time
		 */
		time_t getTime() const;
		
		
		/** fills the given values
	   *  @param year 
		 *  @param month
		 *  @param day
		 *  @param hour
		 *  @param minute
		 *  @param seconds
		 *  @return success 
		 */
		bool getLocalTime(int          &year, 
								      unsigned int &month,
								      unsigned int &day,
								      unsigned int &hour,
								      unsigned int &minute,
								      unsigned int &seconds) const;   
								      
		/** set the date
	   *  @param year 
		 *  @param month
		 *  @param day
		 *  @param hour
		 *  @param minute
		 *  @param seconds
		 *  @return success 
		 */
		bool setLocalTime(int          year, 
								      unsigned int month,
								      unsigned int day,
								      unsigned int hour,
								      unsigned int minute,
								      unsigned int seconds);   
								      
		     
		/** converts the normal date values into milliseconds since  01.01.1970 
		 *  @valid
	   *  @param year 
		 *  @param month
		 *  @param day
		 *  @param hour
		 *  @param minute
		 *  @param seconds
		 *  @return unix style time
		 */
		static time_t date2unix(bool        &valid,
		                        int          year    = 0, 
								            unsigned int month   = 1,
								            unsigned int day     = 1,
								            unsigned int hour    = 0,
								            unsigned int minute  = 0,
								            unsigned int seconds = 0);
		 
		 
		/** converts the unix date format into normal date values
		 *  @param unix time 
	   *  @param year 
		 *  @param month
		 *  @param day
		 *  @param hour
		 *  @param minute
		 *  @param seconds
		 *  @return true if successfully 
		 */
		static bool unix2date(time_t       timevalue,
		                      int          &year, 
								          unsigned int &month,
								          unsigned int &day,
								          unsigned int &hour,
								          unsigned int &minute,
								          unsigned int &seconds);
					          
		/** returns validity of date 
		 *  @return valid 
		 */
		bool isValid(); 
		

	private:
	
		/** checks if the given date is valid one
	   *  @param year 
		 *  @param month
		 *  @param day
		 *  @param hour
		 *  @param minute
		 *  @param seconds
		 *  @return true if valid 
		 */
		static bool _isValid(         int year, 
					               unsigned int month,
					               unsigned int day,
					               unsigned int hour,
					               unsigned int minute,
					               unsigned int seconds);
					              
		/** returns true if year is an leap year
		 *  @param year
		 *  @return boolean
		 */
		static bool isLeapYear(int year);
		
		/** returns the number of days in the given month
		 *  @param month 
		 *  @param isLeapYear
		 */
		static unsigned int daysInMonth(int month, bool isleapyear = false); 
		
		/** stores the date value 
		 *  REMARK: if this is not enough the original values year etc. 
		 *  can be strored as well 
	   */
		time_t time;
		
		/** stores if date is valid 
		 *  this is set after any validation 
		 *  e.g. after setting or after contruction
		 * 
		 *  valid does not affect no operator 
		 *  an invalid date is handled like a valid one
		 */
		bool valid;  
};

}

