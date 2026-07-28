#ifndef __UTIL_VERY_LONG_INT_H
#define __UTIL_VERY_LONG_INT_H

#include <iostream>

#ifdef _MBCS
  const int LONG_SIZE = 4;
  const int INT_SIZE  = 4;
#else
  const int LONG_SIZE = 4;
  const int INT_SIZE  = 4;
#endif

namespace util{

/** this class handles large integer values 
 *  used for byte precicion on giga byte values
 *  it can be used as integer 
 */
class VeryLongInt{

  public:

    /** contructor0 */
    VeryLongInt();

    /** contructor1 */
    VeryLongInt(long value);
  
    /** copy contructor */
    VeryLongInt(const VeryLongInt& other);

    /** destructor */
    ~VeryLongInt();

    /** operator */
    VeryLongInt& operator=(const VeryLongInt& other);

    /** operator */
    VeryLongInt& operator+=(const VeryLongInt& other);

    /** operator */
    VeryLongInt operator+(const VeryLongInt& other);

    /** operator */
    VeryLongInt& operator-=(const VeryLongInt& other);

    /** operator */
    VeryLongInt operator-(const VeryLongInt& other);

    /** operator */
    VeryLongInt& operator*=(const VeryLongInt& other);

    /** operator */
    VeryLongInt operator*(const VeryLongInt& other);

    /** operator */
    VeryLongInt& operator/=(const VeryLongInt& other);

    /** operator */
    VeryLongInt operator/(const VeryLongInt& other);

    /** operator */
    bool operator<(const VeryLongInt& other);

    /** operator */
    bool operator>(const VeryLongInt& other);

    /** operator */
    bool operator<=(const VeryLongInt& other);

    /** operator */
    bool operator>=(const VeryLongInt& other);

    /** operator */
    bool operator==(const VeryLongInt& other);

    /** cast operator */
    operator long();

  private:

    

};


} //end of namespace


/** operator */
util::VeryLongInt operator+(long i, const util::VeryLongInt& other);

/** operator */
util::VeryLongInt operator-(long i, const util::VeryLongInt& other);

/** operator */
util::VeryLongInt operator*(long i, const util::VeryLongInt& other);

/** operator */
util::VeryLongInt operator/(long i, const util::VeryLongInt& other);

/** operator */
bool operator<(long i, const util::VeryLongInt& other);

/** operator */
bool operator>(long i, const util::VeryLongInt& other);

/** operator */
bool operator<=(long i, const util::VeryLongInt& other);

/** operator */
bool operator>=(long i, const util::VeryLongInt& other);

/** operator */
bool operator==(long i, const util::VeryLongInt& other);

#endif