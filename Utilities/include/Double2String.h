#ifndef __DOUBLE_TO_STRING_H
#define __DOUBLE_TO_STRING_H

#include <string>
#include <vector>
#include <algorithm>

#include "BigIntegerD.h"


namespace util{

//*******************************************************************
//*                          Double2String                          *
//*                                                                 *        
//*              class derived from sun.misc.FloatingDecimal        *
//*******************************************************************  

//------------------------------ CONSTANTS -----------------------------------
  
const unsigned __int64 signMask = 0x8000000000000000L;
const unsigned __int64 expMask  = 0x7ff0000000000000L;
const unsigned __int64 fractMask= ~(signMask | expMask);
const unsigned __int32 expShift = 52;
const unsigned __int32 expBias  = 1023;
const unsigned __int64 fractHOB = ( static_cast<unsigned __int64>(1) << expShift ); // assumed High-Order bit
const unsigned __int64 expOne   = static_cast<__int64>(expBias) << expShift; // exponent of 1.0
const __int32          maxSmallBinExp = 62;
const __int32          minSmallBinExp = -( 63 / 3 );
  
const unsigned __int64 highbyte = 0xff00000000000000L;
const unsigned __int64 highbit  = 0x8000000000000000L;
const unsigned __int64 lowbytes = ~highbyte;
  
  
const unsigned char infinity[] = { 'I', 'n', 'f', 'i', 'n', 'i', 't', 'y' };
const unsigned char notANumber[] = { 'N', 'a', 'N' };
const unsigned char zero[] = { '0', '0', '0', '0', '0', '0', '0', '0' };

//private static DoubleBigInt b5p[];

const unsigned __int32 small5pow[] = {
    1,
    5,
    5*5,
    5*5*5,
    5*5*5*5,
    5*5*5*5*5,
    5*5*5*5*5*5,
    5*5*5*5*5*5*5,
    5*5*5*5*5*5*5*5,
    5*5*5*5*5*5*5*5*5,
    5*5*5*5*5*5*5*5*5*5,
    5*5*5*5*5*5*5*5*5*5*5,
    5*5*5*5*5*5*5*5*5*5*5*5,
    5*5*5*5*5*5*5*5*5*5*5*5*5
};
const unsigned int small5powLength = 14;

const unsigned __int64 long5pow[] = {
    static_cast<unsigned __int64>(1),
    static_cast<unsigned __int64>(5),
    static_cast<unsigned __int64>(5)*5,
    static_cast<unsigned __int64>(5)*5*5,
    static_cast<unsigned __int64>(5)*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
    static_cast<unsigned __int64>(5)*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5*5,
};
const unsigned int long5powLength = 27;

// approximately ceil( log2( long5pow[i] ) )
const unsigned __int32 n5bits[] = {
    0,
    3,
    5,
    7,
    10,
    12,
    14,
    17,
    19,
    21,
    24,
    26,
    28,
    31,
    33,
    35,
    38,
    40,
    42,
    45,
    47,
    49,
    52,
    54,
    56,
    59,
    61,
};


class Double2String{
  
  public:

    //constructor
    Double2String(const double d);
  
    //convert it to string
    std::string convertToString();
  
  private:

    //------------------------ PRIVATE METHODS ------------------------

    void dtoa(const __int32 binExp, __int64 fractBits, const __int32 nSignificantBits);

    static unsigned int countBits(unsigned __int64 v );

    // This is the easy subcase --
    // all the significant bits, after scaling, are held in lvalue.
    // negSign and decExponent tell us what processing and scaling
    // has already been done. Exceptional cases have already been
    // stripped out.
    // In particular:
    // lvalue is a finite number (not Inf, nor NaN)
    // lvalue > 0L (not zero, nor negative).
    //
    // The only reason that we develop the digits here, rather than
    // calling on Long.toString() is that we can do it a little faster,
    // and besides want to treat trailing 0s specially. If Long.toString
    // changes, we should re-evaluate this strategy!
    void developLongDigits(__int32 decExponent, __int64 lvalue, __int64 insignificant);

    //add one to the least significant digit.
    //in the unlikely event there is a carry out,
    //deal with it.
    //assert that this will only happen where there
    //is only one digit, e.g. (float)1e-44 seems to do it.
    void roundup();

    //a common operation
    static BigInteger multPow52(BigInteger& v, int p5, int p2 );

    //another common operation
    static BigInteger constructPow52(const unsigned __int32 p5, const unsigned __int32 p2);

    static BigInteger big5pow(__int32 p);

    int getChars(const std::vector<char>& result);

    //-------------------------------------- FIELDS ------------------------------
  
    std::vector<BigInteger> b5p;
    bool isExceptional;
    bool isNegative;
    int  decExponent;
    std::vector<char> digits;
    int  nDigits;
    int  bigIntExp;
    int  bigIntNBits;
    bool mustSetRoundDir;
    bool fromHex;
    int  roundDir; // set by doubleValue
};



} //end of namespace util


#endif