#include "Double2String.h"

//************
//constructor
//************
util::Double2String::Double2String(const double d) : mustSetRoundDir(false),
                                                     fromHex(false),
                                                     roundDir(0)
{

  __int64 dBits = *reinterpret_cast<const __int64*>(&d); //get IEEE 754 bits
  __int32  nSignificantBits;

  // discover and delete sign
  if (dBits & signMask){
    isNegative = true;
    dBits ^= signMask;
  } 
  else{
    isNegative = false;
  }

  // Begin to unpack
  // Discover obvious special cases of NaN and Infinity.
  __int32 binExp = static_cast<__int32>( (dBits & expMask) >> expShift );
  __int64 fractBits = dBits & fractMask;

  if (binExp == static_cast<__int32>(expMask >> expShift)){
    isExceptional = true;
    if (fractBits == 0){
      for (unsigned ic = 0; ic < sizeof(infinity); ++ic) digits.push_back(infinity[ic]);
    } 
    else {
      for (unsigned ic = 0; ic < sizeof(notANumber); ++ic) digits.push_back(notANumber[ic]);
      isNegative = false; // NaN has no sign!
    }
    nDigits = digits.size();
    return;
  }

  isExceptional = false;
  // Finish unpacking
  // Normalize denormalized numbers.
  // Insert assumed high-order bit for normalized numbers.
  // Subtract exponent bias.
  if (binExp == 0){
    if (fractBits == 0L){
      // not a denorm, just a 0!
      decExponent = 0;
      for (unsigned ic = 0; ic < sizeof(zero); ++ic) digits.push_back(zero[ic]);
      nDigits = 1;
      return;
    }
    while ( (fractBits & fractHOB) == 0L ){
      fractBits <<= 1;
      binExp -= 1;
    }
    nSignificantBits = expShift + binExp +1; // recall binExp is  - shift count.
    binExp += 1;
  } 
  else {
    fractBits |= fractHOB;
    nSignificantBits = expShift + 1;
  }
  
  binExp -= expBias;
  
  // call the routine that actually does all the hard work.
  dtoa( binExp, fractBits, nSignificantBits );
}
  
//****************
// convertToString
//****************
std::string util::Double2String::convertToString() {
  //char result[] = (char[])(perThreadBuffer.get());
  //__int32 i = getChars(result);
  return "";// new String(result, 0, i);
}
  
//******
// dtoa
//******
void util::Double2String::dtoa(const __int32 binExp, __int64 fractBits, const __int32 nSignificantBits)
{
  unsigned int nFractBits; // number of significant bits of fractBits;
  __int32 nTinyBits;  // number of these to the right of the po__int32.
  __int32 decExp;

  // Examine number. Determine if it is an easy case,
  // which we can do pretty trivially using float/long conversion,
  // or whether we must do real work.
  nFractBits = countBits(fractBits);
  nTinyBits = static_cast<int>(nFractBits) < (binExp + 1) ? 0 : nFractBits - binExp - 1;
  if (binExp <= maxSmallBinExp && binExp >= minSmallBinExp ){
    // Look more closely at the number to decide if,
    // with scaling by 10^nTinyBits, the result will fit in
    // a __int64.
    if ( (nTinyBits < sizeof(long5pow)) && ((nFractBits + n5bits[nTinyBits]) < 64 ) ){
      // We can do this:
      // take the fraction bits, which are normalized.
      // (a) nTinyBits == 0: Shift left or right appropriately
      //     to align the binary po__int32 at the extreme right, i.e.
      //     where a __int64 __int32 po__int32 is expected to be. The __int32eger
      //     result is easily converted to a string.
      // (b) nTinyBits > 0: Shift right by expShift-nFractBits,
      //     which effectively converts to __int64 and scales by
      //     2^nTinyBits. Then multiply by 5^nTinyBits to
      //     complete the scaling. We know this won't overflow
      //     because we just counted the number of bits necessary
      //     in the result. The __int32eger you get from this can
      //     then be converted to a string pretty easily.
      __int64 halfULP;
      if (nTinyBits == 0) {
        if (binExp > nSignificantBits){
          halfULP = static_cast<__int64>(1) << (binExp - nSignificantBits - 1);
        } 
        else{
          halfULP = 0;
        }
        if (binExp >= expShift){
          fractBits <<= (binExp-expShift);
        } 
        else {
          fractBits >>= (expShift - binExp);
        }
        developLongDigits(0, fractBits, halfULP);
        return;
      }
            
      // The following causes excess digits to be printed
      // out in the single-float case. Our manipulation of
      // halfULP here is apparently not correct. If we
      // better understand how this works, perhaps we can
      // use this special case again. But for the time being,
      // we do not.
      // else {
      //     fractBits >>>= expShift+1-nFractBits;
      //     fractBits //= __int645pow[ nTinyBits ];
      //     halfULP = __int645pow[ nTinyBits ] >> (1+nSignificantBits-nFractBits);
      //     develop__int64Digits( -nTinyBits, fractBits, halfULP );
      //     return;
      // }         
    }
  }

  // This is the hard case. We are going to compute large positive
  // __int32egers B and S and __int32eger decExp, s.t.
  //      d = ( B / S ) // 10^decExp
  //      1 <= B / S < 10
  // Obvious choices are:
  //      decExp = floor( log10(d) )
  //      B      = d // 2^nTinyBits // 10^max( 0, -decExp )
  //      S      = 10^max( 0, decExp) // 2^nTinyBits
  // (noting that nTinyBits has already been forced to non-negative)
  // I am also going to compute a large positive __int32eger
  //      M      = (1/2^nSignificantBits) // 2^nTinyBits // 10^max( 0, -decExp )
  // i.e. M is (1/2) of the ULP of d, scaled like B.
  // When we iterate through dividing B/S and picking off the
  // quotient bits, we will know when to stop when the remainder
  // is <= M.
  //
  // We keep track of powers of 2 and powers of 5.

  // Estimate decimal exponent. (If it is small-ish,
  // we could double-check.)
  //
  // First, scale the mantissa bits such that 1 <= d2 < 2.
  // We are then going to estimate
  //          log10(d2) ~=~  (d2-1.5)/1.5 + log(1.5)
  // and so we can estimate
  //      log10(d) ~=~ log10(d2) + binExp // log10(2)
  // take the floor and call it decExp.
  // FIXME -- use more precise constants here. It costs no more.
  const unsigned __int64 di = expOne | (fractBits & ~fractHOB);
  double d2 = *reinterpret_cast<const double*>(&di); //set IEEE 754 bits
  
  decExp = static_cast<__int32>(floor( (d2 - 1.5) * 0.289529654 + 0.176091259 + static_cast<double>(binExp) * 0.301029995663981));

  unsigned __int32 B2, B5; // powers of 2 and powers of 5, respectively, in B
  unsigned __int32 S2, S5; // powers of 2 and powers of 5, respectively, in S
  unsigned __int32 M2, M5; // powers of 2 and powers of 5, respectively, in M
  unsigned int Bbits;    // binary digits needed to represent B, approx.
  unsigned int tenSbits; // binary digits needed to represent 10*S, approx.
  //BigInteger Sval, Bval, Mval;

  B5 = std::max(0, -decExp);
  B2 = B5 + nTinyBits + binExp;

  S5 = std::max(0, decExp);
  S2 = S5 + nTinyBits;

  M5 = B5;
  M2 = B2 - nSignificantBits;

  // the __int64 __int32eger fractBits contains the (nFractBits) __int32eresting
  // bits from the mantissa of d ( hidden 1 added if necessary) followed
  // by (expShift+1-nFractBits) zeros. In the __int32erest of compactness,
  // I will shift out those zeros before turning fractBits __int32o a
  // FDBig__int32. The resulting whole number will be
  //      d // 2^(nFractBits-1-binExp).
  fractBits >>= (expShift + 1 - nFractBits);
  B2 -= nFractBits-1;
  const unsigned __int32 common2factor = std::min(B2, S2);
  B2 -= common2factor;
  S2 -= common2factor;
  M2 -= common2factor;

  // HACK!! For exact powers of two, the next smallest number
  // is only half as far away as we think (because the meaning of
  // ULP changes at power-of-two bounds) for this reason, we
  // hack M2. Hope this works.
  if (nFractBits == 1) M2 -= 1;

  if (M2 < 0){
    // oops.
    // since we cannot scale M down far enough,
    // we must scale the other values up.
    B2 -= M2;
    S2 -= M2;
    M2 =  0;
  }
  // Construct, Scale, iterate.
  // Some day, we'll write a stopping test that takes
  // account of the asymmetry of the spacing of floating-po__int32
  // numbers below perfect powers of 2
  // 26 Sept 96 is not that day.
  // So we use a symmetric test.

  //char digits[] = this.digits = new char[18];
  __int32  ndigit = 0;
  bool low, high;
  __int64 lowDigitDifference;
  __int32 q;

  // Detect the special cases where all the numbers we are about
  // to compute will fit in __int32 or __int64 __int32egers.
  // In these cases, we will avoid doing FDBig__int32 arithmetic.
  // We use the same algorithms, except that we "normalize"
  // our FDBig__int32s before iterating. This is to make division easier,
  // as it makes our fist guess (quotient of high-order words)
  // more accurate!
  //
  // Some day, we'll write a stopping test that takes
  // account of the asymmetry of the spacing of floating-po__int32
  // numbers below perfect powers of 2
  // 26 Sept 96 is not that day.
  // So we use a symmetric test.
  Bbits = nFractBits + B2 + (( B5 < sizeof(n5bits) )? n5bits[B5] : ( B5 * 3 ));
  tenSbits = S2 + 1 + (( (S5 + 1) < sizeof(n5bits) )? n5bits[(S5 + 1)] : ( (S5 + 1) * 3 ));

  if ( Bbits < 64 && tenSbits < 64){
    if ( Bbits < 32 && tenSbits < 32){
      // wa-hoo! They're all __int32s!
      __int32 b = (static_cast<__int32>(fractBits * small5pow[B5] ) << B2);
      __int32 s = small5pow[S5] << S2;
      __int32 m = small5pow[M5] << M2;
      __int32 tens = s * 10;
            
      // Unroll the first iteration. If our decExp estimate
      // was too high, our first quotient will be zero. In this
      // case, we discard it and decrement decExp.
      ndigit = 0;
      q = b / s;
      b = 10 * ( b % s );
      m *= 10;
      low  = (b <  m );
      high = (b+m > tens );
      if ( (q == 0) && !high ){
        // oops. Usually ignore leading zero.
        --decExp;
      } 
      else {
        digits[ndigit++] = static_cast<char>('0' + q);
      }
      // HACK! Java spec sez that we always have at least
      // one digit after the . in either F- or E-form output.
      // Thus we will need more than one digit if we're using
      // E-form
      if ( decExp < -3 || decExp >= 8 ){
        high = low = false;
      }
      while( !low && !high ){
        q = b / s;
        b = 10 * ( b % s );
        m *= 10;
        if ( m > 0L ){
          low  = (b <  m );
          high = (b+m > tens );
        } 
        else {
          // hack -- m might overflow!
          // in this case, it is certainly > b,
          // which won't
          // and b+m > tens, too, since that has overflowed
          // either!
          low = true;
          high = true;
        }
        digits[ndigit++] = static_cast<char>('0' + q);
      }
      lowDigitDifference = (b<<1) - tens;
    } 
    else{
      // still good! they're all __int64s!
      __int64 b = (fractBits * long5pow[B5] ) << B2;
      __int64 s = long5pow[S5] << S2;
      __int64 m = long5pow[M5] << M2;
      __int64 tens = s * 10L;
      // Unroll the first iteration. If our decExp estimate
      // was too high, our first quotient will be zero. In this
      // case, we discard it and decrement decExp.
      ndigit = 0;
      q = static_cast<__int32>( b / s );
      b = 10 * ( b % s );
      m *= 10;
      low  = (b < m);
      high = (b + m > tens);
            
      if ( (q == 0) && ! high ){
        // oops. Usually ignore leading zero.
        decExp--;
      } 
      else {
        digits[ndigit++] = static_cast<char>('0' + q);
      }
      // HACK! Java spec sez that we always have at least
      // one digit after the . in either F- or E-form output.
      // Thus we will need more than one digit if we're using
      // E-form
      if ( decExp < -3 || decExp >= 8 ){
        high = false;
        low = false;
      }
      while( !low && !high ){
        q = static_cast<__int32>( b / s );
        b = 10 * ( b % s );
        m *= 10;
        if (m > 0){
          low  = (b <  m);
          high = (b + m > tens);
        } 
        else {
          // hack -- m might overflow!
          // in this case, it is certainly > b,
          // which won't
          // and b+m > tens, too, since that has overflowed
          // either!
          low = true;
          high = true;
        }
        digits[ndigit++] = static_cast<char>('0' + q);
      }
      lowDigitDifference = (b<<1) - tens;
     }
    } 
    else{
       

    // We really must do FDBig__int32 arithmetic.
    // Fist, construct our FDBig__int32 initial values.
    BigInteger Bval(multPow52(BigInteger(fractBits), B5, B2));
    BigInteger Sval(constructPow52(S5, S2));
    BigInteger Mval(constructPow52(M5, M2));

    // normalize so that division works better
    const __int32 shiftBias = Sval.normalizeMe();
    Bval << shiftBias;
    Mval << shiftBias;
    const BigInteger tenSval = Sval * 10;
      // Unroll the first iteration. If our decExp estimate
      // was too high, our first quotient will be zero. In this
      // case, we discard it and decrement decExp.
      ///
    ndigit = 0;
    q = Bval.quoRemIteration( Sval );
    Mval = Mval * 10;
    low  = (Bval.cmp( Mval ) < 0);
    high = (Bval + Mval).cmp(tenSval) > 0;
    if ((q == 0) && !high ){
      // oops. Usually ignore leading zero.
      decExp--;
    } 
    else{
      digits[ndigit++] = static_cast<char>('0' + q);
    }
    // HACK! Java spec sez that we always have at least
    // one digit after the . in either F- or E-form output.
    // Thus we will need more than one digit if we're using
    // E-form
    if (decExp < -3 || decExp >= 8){
      high = false;
      low = false;
    }
    while( !low && !high ){
      q = Bval.quoRemIteration( Sval );
      Mval = Mval *  10;
      low  = (Bval.cmp( Mval ) < 0);
      high = (Bval + Mval).cmp(tenSval) > 0;
      digits[ndigit++] = static_cast<char>('0' + q);
    }
    if (high && low){
      Bval << 1;
      lowDigitDifference = Bval.cmp(tenSval);
    } 
    else{
      lowDigitDifference = 0; // this here only for flow analysis!
    }
  }
  decExponent = decExp + 1;
  digits = digits;
  nDigits = ndigit;
  
  //Last digit gets rounded based on stopping condition.
  if (high){
    if (low){
      if (lowDigitDifference == 0){
        // it's a tie!
        // choose based on which digits we like.
        if ((digits[nDigits - 1] & 1) != 0 ) roundup();
      } 
      else if (lowDigitDifference > 0){
        roundup();
      }
    } 
    else{
      roundup();
    }
  }
}

//***************************************************************
// countBits
// 
// count number of bits from high-order 1 bit to low-order 1 bit,
// inclusive.
//***************************************************************
unsigned int util::Double2String::countBits(unsigned __int64 v ){
  // the strategy is to shift until we get a non-zero sign bit
  // then shift until we have no bits left, counting the difference.
  // we do byte shifting as a hack. Hope it helps.
  //
  if ( v == 0L ) return 0;

  while ( ( v & highbyte ) == 0L ){
    v <<= 8;
  }
  while ( v > 0L ) { // i.e. while ((v&highbit) == 0L )
    v <<= 1;
  }

  unsigned int n = 0;
  while (( v & lowbytes ) != 0L ){
    v <<= 8;
    n += 8;
  }
  while ( v != 0L ){
    v <<= 1;
    n += 1;
  }
  return n;
}


//*********************************************************************
// developLongDigits
//
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
// calling on __int64.toString() is that we can do it a little faster,
// and besides want to treat trailing 0s specially. If __int64.toString
// changes, we should re-evaluate this strategy!
//********************************************************************
void util::Double2String::developLongDigits(__int32 decExponent, __int64 lvalue, __int64 insignificant){
      /*char digits[];
      __int32  ndigits;
      __int32  digitno;
      __int32  c;
      //
      // Discard non-significant low-order bits, while rounding,
      // up to insignificant value.
      __int32 i;
      for ( i = 0; insignificant >= 10L; i++ )
          insignificant /= 10L;
      if ( i != 0 ){
          __int64 pow10 = __int645pow[i] << i; // 10^i == 5^i * 2^i;
          __int64 residue = lvalue % pow10;
          lvalue /= pow10;
          decExponent += i;
          if ( residue >= (pow10>>1) ){
              // round up based on the low-order bits we're discarding
              lvalue++;
          }
      }
      if ( lvalue <= __int32eger.MAX_VALUE ){
          assert lvalue > 0L : lvalue; // lvalue <= 0
          // even easier subcase!
          // can do __int32 arithmetic rather than __int64!
          __int32  ivalue = static_cast<__int32>(lvalue;
          ndigits = 10;
          digits = (char[])(perThreadBuffer.get());
          digitno = ndigits-1;
          c = ivalue%10;
          ivalue /= 10;
          while ( c == 0 ){
              decExponent++;
              c = ivalue%10;
              ivalue /= 10;
          }
          while ( ivalue != 0){
              digits[digitno--] = (char)(c+'0');
              decExponent++;
              c = ivalue%10;
              ivalue /= 10;
          }
          digits[digitno] = (char)(c+'0');
      } else {
          // same algorithm as above (same bugs, too )
          // but using __int64 arithmetic.
          ndigits = 20;
          digits = (char[])(perThreadBuffer.get());
          digitno = ndigits-1;
          c = static_cast<__int32>((lvalue%10L);
          lvalue /= 10L;
          while ( c == 0 ){
              decExponent++;
              c = static_cast<__int32>((lvalue%10L);
              lvalue /= 10L;
          }
          while ( lvalue != 0L ){
              digits[digitno--] = (char)(c+'0');
              decExponent++;
              c = static_cast<__int32>((lvalue%10L);
              lvalue /= 10;
          }
          digits[digitno] = (char)(c+'0');
      }
      char result [];
      ndigits -= digitno;
      result = new char[ ndigits ];
      System.arraycopy( digits, digitno, result, 0, ndigits );
      this.digits = result;
      this.decExponent = decExponent+1;
      this.nDigits = ndigits;*/
  }

//*****************************************************
// roundup
//
// add one to the least significant digit.
// in the unlikely event there is a carry out,
// deal with it.
// assert that this will only happen where there
// is only one digit, e.g. (float)1e-44 seems to do it.
//*****************************************************
void util::Double2String::roundup(){
  __int32 i = nDigits - 1;
  __int32 q = digits[i];
  if (q == '9' ){
    while (q == '9' && i > 0){
      digits[i] = '0';
      q = digits[--i];
    }
    if (q == '9'){
      // carryout! High-order 1, rest 0s, larger exp.
      decExponent += 1;
      digits[0] = '1';
      return;
    }
    // else fall through.
  }
  digits[i] = (char)(q+1);
}

//*******************
// multPow52 TT
//
// a common operation
//*******************
util::BigInteger util::Double2String::multPow52(util::BigInteger& v, const __int32 p5, const __int32 p2){
  if (p5 == 0) return v;
  BigInteger result(0);
  if (p5 < small5powLength){
    result = v * static_cast<int>(small5pow[p5]);
  }
  else{
    result = v * big5pow(p5);
  }
  if (p2 != 0) result << p2;
  return result;
}

//*************************
// multPow52 TT
//
// another common operation
//*************************
util::BigInteger util::Double2String::constructPow52(const unsigned __int32 p5, const unsigned __int32 p2){
  BigInteger v(big5pow(p5));
  if (p2 != 0) v << p2 ;
  return v;
}
  
//***********
// big5pow 
//***********
util::BigInteger util::Double2String::big5pow(const __int32 p){
  /*
  if (b5p == null){
    b5p = new BigInteger[ p+1 ];
  }
  else if (b5p.length <= p ){
    BigInteger t[] = new BigInteger[ p+1 ];
      System.arraycopy( b5p, 0, t, 0, b5p.length );
      b5p = t;
  }
  if (b5p[p] != null){
      return b5p[p];
  }
  else if (p < small5powLength){
    return b5p[p] = new BigInteger(small5pow[p]);
  }
  else if ( p < __int645pow.length ){
    return b5p[p] = new BigInteger(long5pow[p]);
  }
  else {
    // construct the value.
    // recursively.
    __int32 q, r;
    // in order to compute 5^p,
    // compute its square root, 5^(p/2) and square.
    // or, let q = p / 2, r = p -q, then
    // 5^p = 5^(q+r) = 5^q * 5^r
    q = p >> 1;
    r = p - q;
    BigInteger bigq = b5p[q];
    if (bigq == null){
        bigq = big5pow (q);
    }
    if (r < small5pow.length){
      return b5p[p] = bigq * small5pow[r];
    }
    else{
      BigInteger bigr = b5p[r];
      if (bigr == null){
        bigr = big5pow(r);
      }
      return (b5p[p] = bigq.mult(bigr));
    }
  }*/
  return BigInteger(0);
}

  
//*********
// getChars
//*********
unsigned int getChars(const std::vector<char>& result) {
  /*
    __int32 i = 0;
    if (isNegative) { result[0] = '-'; i = 1; }
    if (isExceptional) {
        System.arraycopy(digits, 0, result, i, nDigits);
        i += nDigits;
    } 
    else {
        if (decExponent > 0 && decExponent < 8) {
            // pr__int32 digits.digits.
            __int32 charLength = Math.min(nDigits, decExponent);
            System.arraycopy(digits, 0, result, i, charLength);
            i += charLength;
            if (charLength < decExponent) {
                charLength = decExponent-charLength;
                System.arraycopy(zero, 0, result, i, charLength);
                i += charLength;
                result[i++] = '.';
                result[i++] = '0';
            } 
            else {
                result[i++] = '.';
                if (charLength < nDigits) {
                    __int32 t = nDigits - charLength;
                    System.arraycopy(digits, charLength, result, i, t);
                    i += t;
                } else {
                    result[i++] = '0';
                }
            }
        } 
        else if (decExponent <=0 && decExponent > -3) {
          result[i++] = '0';
            result[i++] = '.';
            if (decExponent != 0) {
                System.arraycopy(zero, 0, result, i, -decExponent);
                i -= decExponent;
            }
            System.arraycopy(digits, 0, result, i, nDigits);
            i += nDigits;
        } 
        else {
            result[i++] = digits[0];
            result[i++] = '.';
            if (nDigits > 1) {
                System.arraycopy(digits, 1, result, i, nDigits-1);
                i += nDigits-1;
            } else {
                result[i++] = '0';
            }
            result[i++] = 'E';
            __int32 e;
            if (decExponent <= 0) {
                result[i++] = '-';
                e = -decExponent+1;
            } else {
                e = decExponent-1;
            }
            // decExponent has 1, 2, or 3, digits
            if (e <= 9) {
                result[i++] = (char)(e+'0');
            } 
            else if (e <= 99) {
                result[i++] = (char)(e/10 +'0');
                result[i++] = (char)(e%10 + '0');
            } 
            else {
                result[i++] = (char)(e/100+'0');
                e %= 100;
                result[i++] = (char)(e/10+'0');
                result[i++] = (char)(e%10 + '0');
            }
        }
    }
    return i;*/
  return 0;
}