#ifndef __BIGINTERGER_D_H
#define __BIGINTERGER_D_H

#include <string>
#include <vector>

namespace util{

  
//**********************************************************************
//*                          BigInteger                                *
//*                                                                    *        
//* class taken from sun.misc.FloatingDecimal for use in Double2String *
//**********************************************************************
class BigInteger{

  public: 

    //--------------- CONTRUCTION AND INITIALISATION ----------------

    explicit BigInteger(__int32 v);
    explicit BigInteger(__int64 v);
    BigInteger(const __int64 seed, const std::vector<char>& digit, const int nd0, const int nd);
    BigInteger(const BigInteger& other);
    ~BigInteger();

  private:

    BigInteger(const std::vector<__int32>& d, const unsigned int n);

  public: 

    //Left shift by c bits. Shifts this in place.
    void lshiftMe(const unsigned int c);

    //operator <<
    BigInteger& operator<<(const unsigned int c);

    //normalize this number by shifting until
    //the MSB of the number is at 0x08000000.
    //This is in preparation for quoRemIteration, below.
    //The idea is that, to make division easier, we want the
    //divisor to be "normalized" -- usually this means shifting
    //the MSB into the high words sign bit. But because we know that
    //the quotient will be 0 < q < 10, we would like to arrange that
    //the dividend not span up into another word of precision.
    //(This needs to be explained more clearly!)
    unsigned int normalizeMe();

    //Compute
    //q = (int)( this / S )
    //this = 10 * ( this mod S )
    //Return q.
    //This is the iteration step of digit development for output.
    //We assume that S has been normalized, as above, and that
    //"this" has been lshift'ed accordingly.
    //Also assume, of course, that the result, q, can be expressed
    //as an integer, 0 <= q < 10.
    __int32 quoRemIteration(const BigInteger& S);

    //Multiply a BigInteger by an int.
    //Result is a new BigInteger.
    BigInteger mult(const __int32 iv);
    
    //operator *
    BigInteger operator*(const __int32 iv);

    //Multiply a BigInteger by an int and add another int.
    //Result is computed in place.
    //Hope it fits!
    void multaddMe(int iv, int addend);

    //Multiply a BigInteger by another BigInteger.
    //Result is a new FDBigInt.
    BigInteger mult(const BigInteger& other);

    //operator *
    BigInteger operator*(const BigInteger& other);

    //Add one FDBigInt to another. Return a BigInteger
    BigInteger add(const BigInteger& other);

    //operator +
    BigInteger operator+(const BigInteger& other);

    //Subtract one BigInteger from another. Return a BigInteger
    //Assert that the result is positive.
    BigInteger sub(const BigInteger& other);

    //operator -
    BigInteger operator-(const BigInteger& other);

    // Compare BigInteger with another BigInteger. Return an integer
    // >0: this > other
    //  0: this == other
    // <0: this < other
    int cmp(const BigInteger& other);

    //return the long value
    __int64 longValue();

    //to string representation
    //std::string toString();

  private:

    bool dataInRangeIsZero(unsigned int i, const unsigned int m, const BigInteger& bi);

    //----------------------- FIELDS -------------------------
    
    std::vector<__int32> data; // value: data[0] is least significant
    unsigned int nWords; // number of words used
   

};



} //end of namespace util


#endif
