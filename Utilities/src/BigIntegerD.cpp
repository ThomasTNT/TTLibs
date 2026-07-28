#include "BigIntegerD.h"


//*******************************************************************
//*                          BigInteger                             *
//*                                                                 *        
//* class taken from sun.misc.FloatingDecimal for use in DoubleConv *
//*******************************************************************
   
//*******************
// constructor int TT
//*******************
util::BigInteger::BigInteger(const __int32 v) : data(1), nWords(1){
  data.push_back(v);
}

//********************
// constructor long TT
//********************
util::BigInteger::BigInteger(const __int64 v) : data(2){
  data.push_back(static_cast<__int32>(v));
  data.push_back(static_cast<__int32>(v >> 3));
  nWords = (data[1] == 0) ? 1 : 2;
}

//********************
// copy constructor TT
//********************
util::BigInteger::BigInteger(const BigInteger& other) : data(other.data), nWords(other.nWords)
{
}

//***********************
// private constructor TT //??????
//***********************
util::BigInteger::BigInteger(const std::vector<__int32>& d, const unsigned int n ) : data(d), nWords(n){
}

//*****************
// constructor 4 TT
//*****************
util::BigInteger::BigInteger(const __int64 seed, const std::vector<char>& digit, const int nd0, const int nd) 
                 : data(10)
{
  data[0] = static_cast<__int32>(seed);    // starting value
  data[1] = static_cast<__int32>(seed >> 32);
  nWords = (data[1] == 0) ? 1 : 2;

  int i = nd0;
  int limit = nd - 5;       // slurp digits 5 at a time.
  int v;
  while (i < limit){
    int ilim = i + 5;
    v = static_cast<__int32>(digit[i++]) - static_cast<__int32>('0');
    while(i < ilim){
      v = 10 * v + static_cast<__int32>(digit[i++]) - static_cast<__int32>('0');
    }
    multaddMe(100000, v); // ... where 100000 is 10^5.
  }
  int factor = 1;
  v = 0;
  while (i < nd){
    v = 10*v + static_cast<__int32>(digit[i++]) - static_cast<__int32>('0');
    factor *= 10;
  }
  if (factor != 1){
    multaddMe(factor, v);
  }
}


//**************
// destructor TT
//**************
util::BigInteger::~BigInteger(){
}

//********************************************
// lshiftMe TT
//
// Left shift by c bits. Shifts this in place.
//******************************************
void util::BigInteger::lshiftMe(const unsigned int c){
  
  if (c == 0) return; // silly.
  
  const unsigned int wordcount = c >> 5;
  const unsigned int bitcount  = c & 0x1f;
  const unsigned int anticount = 32 - bitcount;
  
  while (data.size() < nWords + wordcount + 1) data.push_back(0);
  
  int target = nWords + wordcount;
  int src    = nWords - 1;
  if (bitcount == 0){
    target = wordcount - 1;
  }
  else{
    data[target--] = data[src] >> anticount;
    while (src >= 1){
      data[target--] = (data[src] << bitcount) | (data[--src] >> anticount);
    }
    data[target--] = data[src] << bitcount;
  }
  while(target >= 0){
    data[target--] = 0;
  }
  nWords += wordcount + 1;
  // may have constructed high-order word of 0.
  // if so, trim it
  while (nWords > 1 && data[nWords - 1] == 0) --nWords;
}

//***************
// operator << TT
//***************
util::BigInteger& util::BigInteger::operator<<(const unsigned int c){
  lshiftMe(c);
  return *this;
}
    
//***************************************************************
// normalizeMe TT
//
// normalize this number by shifting until
// the MSB of the number is at 0x08000000.
// This is in preparation for quoRemIteration, below.
// The idea is that, to make division easier, we want the
// divisor to be "normalized" -- usually this means shifting
// the MSB into the high words sign bit. But because we know that
// the quotient will be 0 < q < 10, we would like to arrange that
// the dividend not span up into another word of precision.
// (This needs to be explained more clearly!)
//***************************************************************
unsigned int util::BigInteger::normalizeMe(){

  __int32 v = 0;
  unsigned int src;
  unsigned int wordcount = 0;
  for (src = nWords - 1; src >= 0 && (v = data[src]) == 0 ; src--){
    wordcount += 1;
  }
  
  // In most cases, we assume that wordcount is zero. This only
  // makes sense, as we try not to maintain any high-order
  // words full of zeros. In fact, if there are zeros, we will
  // simply SHORTEN our number at this point. Watch closely...
  nWords -= wordcount;
  
  // Compute how far left we have to shift v s.t. its highest-
  // order bit is in the right place. Then call lshiftMe to
  // do the work.
  unsigned int bitcount  = 0;
  if ( (v & 0xf0000000) != 0 ){
    //will have to shift up into the next word.
    // too bad.
    for(bitcount = 32 ; (v & 0xf0000000) != 0 ; --bitcount) v >>= 1;
  } 
  else{
    while ( v <= 0x000fffff ){
      // hack: byte-at-a-time shifting
      v <<= 8;
      bitcount += 8;
    }
    while ( v <= 0x07ffffff ){
      v <<= 1;
      bitcount += 1;
    }
  }
  if (bitcount != 0) lshiftMe(bitcount);
  return bitcount;
}


//*********************************
// mult TT
// 
// Multiply a BigInteger by an int.
// Result is a new BigInteger.
//*********************************
util::BigInteger util::BigInteger::mult(const __int32 iv){
  
  __int64 v = iv;
  std::vector<__int32> r;
  __int64 p = 0;
 
  for(unsigned int i = 0; i < nWords; ++i){
    p += v * (static_cast<__int64>(data[i]) & 0xffffffffL);
    r.push_back(static_cast<__int32>(p));
    p >>= 32;
  }
  
  if (p == 0){
    return BigInteger(r, nWords);
  } 
  else{
    r.push_back(static_cast<__int32>(p));
    return BigInteger(r, nWords + 1);
  }
}

//*************
//operator * TT
//*************
util::BigInteger util::BigInteger::operator*(const __int32 iv){
  return mult(iv);
}
    
//***************************************************
// multaddMe TT
//
// Multiply a FDBigInt by an int and add another int.
// Result is computed in place.
// Hope it fits!
//***************************************************
void util::BigInteger::multaddMe(const __int32 iv, const __int32 addend){
  const __int64 v = iv;
  //unroll 0th iteration, doing addition.
  __int64 p = v * (static_cast<__int64>(data[0]) & 0xffffffffL) + (static_cast<__int64>(addend) & 0xffffffffL);
  
  data[0] = static_cast<__int32>(p);
  p >>= 32;
  for(unsigned int i = 1; i < nWords; ++i) {
    p += v * (static_cast<__int64>(data[i]) & 0xffffffffL);
    data[i] = static_cast<__int32>(p);
    p >>= 32;
  }
  if (p != 0){
    if (data.size() > nWords){
      data[nWords] = static_cast<__int32>(p); 
    }
    else{
      data.push_back(static_cast<__int32>(p));
    }
    ++nWords;
  }
}

//*********************************************
// mult TT
//
// Multiply a BigInteger by another BigInteger.
// Result is a new BigInteger.
//*********************************************
util::BigInteger util::BigInteger::mult(const BigInteger& other){
  // crudely guess adequate size for r
  std::vector<__int32> r;
  for (unsigned int i = 0 ; i < (nWords + other.nWords); ++i) r.push_back(0);
  
  // I think I am promised zeros...
  unsigned int i;
  for(i = 0; i < nWords; ++i){
    const __int64 v = static_cast<__int64>(data[i]) & 0xffffffffL; // UNSIGNED CONVERSION
    __int64 p = 0;
    unsigned int j;
    for(j = 0; j < other.nWords; ++j){
      p += (static_cast<__int64>(r[i + j]) & 0xffffffffL) + v * (static_cast<__int64>(other.data[j]) & 0xffffffffL); // UNSIGNED CONVERSIONS ALL 'ROUND.
      r[i + j] = static_cast<__int32>(p);
      p >>= 32;
    }
    r[i + j] = static_cast<__int32>(p);
  }
  // compute how much of r we actually needed for all that.
  for (i = r.size() - 1; i > 0; --i){
    if (r[i] != 0) break;
  }
  return BigInteger(r, i + 1);
}

//**************
// operator * TT
//**************
util::BigInteger util::BigInteger::operator*(const BigInteger& other){
  return mult(other);  
}

//******************************************************
// add TT
// 
// Add one BigInteger to BigInteger. Return a BigInteger
//******************************************************
util::BigInteger util::BigInteger::add(const BigInteger& other){
  std::vector<__int32> const *a, *b;
  unsigned int n, m;
  
  // arrange such that a.nWords >= b.nWords;
  // n = a.nWords, m = b.nWords
  if (nWords >= other.nWords ){
    a = &data;
    n = nWords;
    b = &other.data;
    m = other.nWords;
  } 
  else{
    a = &other.data;
    n = other.nWords;
    b = &data;
    m = nWords;
  }
  std::vector<__int32> r;
  __int64 c = 0;
  unsigned int i;
  for (i = 0; i < n; ++i){
    c += static_cast<__int64>((*a)[i]) & 0xffffffffL;
    if (i < m){
      c += static_cast<__int64>((*b)[i]) & 0xffffffffL;
    }
    r.push_back(static_cast<__int32>(c));
    c >>= 32; // signed shift.
  }
  if (c != 0){
    //oops -- carry out -- need longer result.
    r.push_back(static_cast<__int32>(c));
  }
  return BigInteger(r, i);
}

//**************
// operator + TT
//**************
util::BigInteger util::BigInteger::operator+(const BigInteger& other){
  return add(other);  
}
    
//**********************************************************
// sub TT
//
// Subtract one BigInteger from another. Return a BigInteger
// Assert that the result is positive.
//**********************************************************
util::BigInteger util::BigInteger::sub(const BigInteger& other){
  std::vector<__int32> r;
  const unsigned int n = nWords;
  const unsigned int m = other.nWords;
  unsigned int nzeros = 0;
  __int64 c = 0;
  unsigned int i;
  for (i = 0; i < n; ++i){
    c += static_cast<__int64>(data[i]) & 0xffffffffL;
    if (i < m){
      c -= static_cast<__int64>(other.data[i]) & 0xffffffffL;
    }
    r.push_back(static_cast<__int32>(c));
    if (c == 0){
      nzeros++;
    }
    else{
      nzeros = 0;
    }
    c >>= 32; // signed shift
  }
  //assert c == 0L : c; // borrow out of subtract
  //assert dataInRangeIsZero(i, m, other); // negative result of subtract
  return BigInteger(r, n - nzeros);
}

//**************
// operator + TT
//**************
util::BigInteger util::BigInteger::operator-(const BigInteger& other){
  return sub(other);  
}

//**************************************************************
// cmp TT
// 
// Compare BigInteger with another BigInteger. Return an integer
// >0: this > other
//  0: this == other
// <0: this < other
//**************************************************************
int util::BigInteger::cmp(const BigInteger& other){
  
  unsigned int i;
  if (nWords > other.nWords){
    //if any of my high-order words is non-zero,
    //then the answer is evident
    const unsigned int j = other.nWords - 1;
    for (i = nWords - 1; i > j ; --i){
      if (data[i] != 0 ) return 1;
    }
  }
  else if (nWords < other.nWords){
    //if any of other's high-order words is non-zero,
    //then the answer is evident
    const unsigned int j = nWords - 1;
    for (i = other.nWords - 1; i > j ; --i){
      if (other.data[i] != 0) return -1;
    }
  } 
  else{
    i = nWords - 1;
  }
  
  
  for (; i > 0 ; --i){
    if (data[i] != other.data[i]) break;
  }
  
  // careful! want unsigned compare!
  // use brute force here.
  __int32 a = data[i];
  __int32 b = other.data[i];
  if (a < 0){
    //a is really big, unsigned
    if (b < 0){
      return a - b; // both big, negative
    } 
    else{
      return 1; // b not big, answer is obvious;
    }
  } 
  else{
    // a is not really big
    if (b < 0){
      // but b is really big
      return -1;
    } 
    else {
      return a - b;
    }
  }
}

//*************************************************************
// quoRemIteration TT
// 
// Compute
// q = (int)( this / S )
// this = 10 * ( this mod S )
// Return q.
// This is the iteration step of digit development for output.
// We assume that S has been normalized, as above, and that
// "this" has been lshift'ed accordingly.
// Also assume, of course, that the result, q, can be expressed
// as an integer, 0 <= q < 10.
//*************************************************************
__int32 util::BigInteger::quoRemIteration(const BigInteger& S ){
  
  // ensure that this and S have the same number of
  // digits. If S is properly normalized and q < 10 then
  // this must be so.
  if (nWords != S.nWords){
    //ERROR
  }
  
  // estimate q the obvious way. We will usually be
  // right. If not, then we're only off by a little and
  // will re-add.
  unsigned int n = nWords - 1;
  __int64 q = (static_cast<__int64>(data[n]) & 0xffffffffL) / static_cast<__int64>(S.data[n]);
  __int64 diff = 0L;
  for (unsigned int i = 0; i <= n ; ++i){
    diff += (static_cast<__int64>(data[i]) & 0xffffffffL) -  q * (static_cast<__int64>(S.data[i]) & 0xffffffffL);
    data[i] = static_cast<__int32>(diff);
    diff >>= 32; // N.B. SIGNED shift.
  }
  if (diff != 0L){
    // damn, damn, damn. q is too big.
    // add S back in until this turns +. This should
    // not be very many times!
    __int64 sum = 0L;
    while (sum == 0L){
      sum = 0L;
      for (unsigned int i = 0; i <= n; ++i){
        sum += (static_cast<__int64>(data[i]) & 0xffffffffL) +  (static_cast<__int64>(S.data[i]) & 0xffffffffL);
        data[i] = static_cast<__int32>(sum);
        sum >>= 32; // Signed or unsigned, answer is 0 or 1
      }
      // Originally the following line read
      // "if ( sum !=0 && sum != -1 )"
      // but that would be wrong, because of the
      // treatment of the two values as entirely unsigned,
      // it would be impossible for a carry-out to be interpreted
      // as -1 -- it would have to be a single-bit carry-out, or
      // +1.
      //assert(sum == 0 || sum == 1); // carry out of division correction
      q -= 1;
    }
  }
  // finally, we can multiply this by 10.
  // it cannot overflow, right, as the high-order word has
  // at least 4 high-order zeros!
  __int64 p = 0L;
  for (unsigned int i = 0; i <= n; ++i){
    p += 10 * (static_cast<__int64>(data[i]) & 0xffffffffL);
    data[i] = static_cast<__int32>(p);
    p >>= 32; // SIGNED shift.
  }
  //assert(p == 0L); // Carry out of *10
  return static_cast<__int32>(q);
}

//*************
// longValue TT
//*************
__int64 util::BigInteger::longValue(){
  // if this can be represented as a long, return the value
  if (nWords == 1) return (static_cast<__int64>(data[0]) & 0xffffffffL);
  //assert dataInRangeIsZero(2, this.nWords, this); // value too big
  //assert data[1] >= 0;  // value too big
  return (static_cast<__int64>(data[1]) << 32) | (static_cast<__int64>(data[0]) & 0xffffffffL);
}

//****************************
// dataInRangeIsZero TT
//
// returns true is all is zero
//****************************
bool util::BigInteger::dataInRangeIsZero(unsigned int i, const unsigned int m, const BigInteger& bi) {
  while (i < m){
    if (bi.data[i++] != 0) return false;
  }
  return true;
}


//NOT USED
/*std::string toString() {
  StringBuffer r = new StringBuffer(30);
  r.append('[');
  int i = Math.min( nWords-1, data.length-1) ;
  if ( nWords > data.length ){
      r.append( "("+data.length+"<"+nWords+"!)" );
  }
  for( ; i> 0 ; i-- ){
      r.append( Integer.toHexString( data[i] ) );
      r.append(' ');
  }
  r.append( Integer.toHexString( data[0] ) );
  r.append(']');
  return new String( r );
}
*/




