#include "TTColor.h"


//******************************************************************************
//*                                TTColorConv                                 *
//*                                                                            *
//*                  class for color handling and conversion                   *
//******************************************************************************


//------------------------- methods for printing ----------------------------


//**********************
// GetColormodeTypeName 
//**********************
std::string image::GetColormodeTypeName(const ColormodeType colormode){
  switch (colormode){
    case BIT:           return "bitmap normal";
    case BIT_INVERTED:  return "bitmap inverted";
    case GRAYSCALE:     return "grayscale";
    case GRAY_INVERTED: return "grayscale inverted";
    case RGB:           return "RGB interlaced";
    case RRGGBB:        return "RGB planes";
    case BGR:           return "BRG (ms bitmap)";
    case LAB:           return "Lab";
    case CMYK:          return "CMYK interlaced";
    case CCMMYYKK:      return "CMYK planes";
    case RGBA:          return "RGB with alpha interlaced";
    case RRGGBBAA:      return "RGB with alpha planes";
    case YUV:           return "YUV";
    case OTHERMODE:     return "other";
    default:            return "unknown";
  }
}

//**********************
// GetDatatypName 
//**********************
std::string image::GetDatatypName(const Datatyp type){
  switch (type){
    case INT8:      return "8 bit";
    case INT16:     return "16 bit";
    case INT32:     return "32 bit";
    case FLOAT32:   return "32 bit float";
    case OTHERTYPE: return "other";
    default: return "unknown";
  }
}

//****************
// GetChannelName 
//****************
std::string image::GetChannelName(const Channel channel){
  switch (channel){
    case BINARY:  return "Bit";
    case GRAY:    return "Gray";
    case RED:     return "Red";
    case GREEN:   return "Green";
    case BLUE:    return "Blue";
    case CYAN:    return "Cyan";
    case MAGANTA: return "Magenta";
    case YELLOW:  return "Yellow";
    case BLACK:   return "Black";
    case L:       return "Luminance";
    case A:       return "A-Channel";
    case B:       return "B-Channel";
    default:      return "other";
  }
}


//-------------------------- informations methods --------------------------

//***********************
// GetSimpleColormodeType 
//***********************
image::ColormodeType image::GetSimpleColormodeType(const ColormodeType colormode){
  switch (colormode){
    case BIT:          
    case BIT_INVERTED:
    case GRAYSCALE:    
    case GRAY_INVERTED: return GRAYSCALE;
    case RGB:        
    case RGBA:          
    case RRGGBB: 
    case RRGGBBAA:
    case BGR:           return RGB;
    case LAB:           return LAB;
    case CMYK:        
    case CCMMYYKK:      return CMYK;
    case YUV:           return YUV;
    case OTHERMODE:     return OTHERMODE;
    default:            return UNKNOWN_COLORMODE;
  }
}

//*****************************
// GetInterlacedByColormodeType 
//*****************************
bool image::GetInterlacedByColormodeType(const ColormodeType colormode){
  switch (colormode){
    case BIT:
    case BIT_INVERTED:
    case GRAYSCALE:
    case GRAY_INVERTED: 
    case RGB:
    case RGBA:
    case BGR:           
    case LAB:          
    case CMYK:
    case YUV:          
    case OTHERMODE:    return true;  //is default
    case RRGGBB:
    case RRGGBBAA:
    case CCMMYYKK:     return false;
    default:           return true;  //is default
  }
}

//***************************
// GetChannelsByColormodeType 
//***************************
unsigned int image::GetChannelsByColormodeType(const ColormodeType colormode){
  switch (colormode){
    case BIT:
    case BIT_INVERTED:
    case GRAYSCALE:
    case GRAY_INVERTED: return 1;
    case RGB:
    case RGBA:
    case RRGGBB:
    case RRGGBBAA:
    case BGR:           return 3;
    case LAB:           return 3;
    case CMYK:
    case CCMMYYKK:      return 4;
    case YUV:           return 3;
    case OTHERMODE:     return 0;
    default:            return 0;
  }
}

//********************************
// GetAlphaChannelsByColormodeType 
//********************************
unsigned int image::GetAlphaChannelsByColormodeType(const ColormodeType colormode){
  switch (colormode){
    case RGBA:
    case RRGGBBAA:  return 1;
    default:        return 0;
  }
}

//******************
// GetBytesByDatatyp 
//******************
unsigned int image::GetBytesByDatatyp(const Datatyp datatyp){
  switch (datatyp){
    case INT8: return 1;
    case INT16: 
    case INT16PS:  
                  return 2; 
    case INT32:
    case FLOAT32: return 4;
    default: return 0;
  }
}


//----------------------- static untility functions -----------------------

//**********
// U8 -> U16
//**********
image::t_integertype image::TTColorConv::U16FromU8(const t_integertype v){
  return (v * 2570 + 5) / 10;  //TIFF
}

//**********
// U8 -> U16
//**********
image::t_integertype image::TTColorConv::U16FromU8PS(const t_integertype v){
  return (v * 1285 + 5) / 10;   //photoshop
}


//**********
// U16 -> U8
//**********
image::t_integertype image::TTColorConv::U8FromU16(const t_integertype v){
  return v * 10 / 2570;  //TIFF
}

//**********
// U16 -> U8
//**********
image::t_integertype image::TTColorConv::U8FromU16PS(const t_integertype v){
  return v * 10 / 1285; //photoshop
}

//**********
// U8 -> F32
//**********
F32 image::TTColorConv::F32FromU8(const t_integertype v){
  return static_cast<F32>(v) / 255.0f;
}

//***********
// F32 -> U8
//***********
image::t_integertype image::TTColorConv::U8FromF32(const F32 v){
  F32 v1 = v * 255.0f;
  if (v1 < 0.0f) v1 = 0.0f;
  if (v1 > 255.0f) v1 = 255.0f;
  return static_cast<t_integertype>(v1);
}

//***********
// U16 -> F32
//***********
F32 image::TTColorConv::F32FromU16(const t_integertype v){
  return static_cast<F32>(v) / 65535.0f; //TIFF
}

//***********
// U16 -> F32
//***********
F32 image::TTColorConv::F32FromU16PS(const t_integertype v){
  return static_cast<F32>(v) / 32768.0f;//photoshop
}

//***********
// F32 -> U16
//***********
image::t_integertype image::TTColorConv::U16FromF32(const F32 v){
  //TIFF
  F32 v1 = v * 65535.0f;
  if (v1 < 0.0f) v1 = 0.0f;
  if (v1 > 65535.0f) v1 = 65535.0f; 
  return static_cast<t_integertype>(v1); 
}

//***********
// F32 -> U16
//***********
image::t_integertype image::TTColorConv::U16FromF32PS(const F32 v){
  //photoshop
  F32 v1 = v * 32768.0f;
  if (v1 < 0.0f) v1 = 0.0f;
  if (v1 > 32768.0f) v1 = 32768.0f;  //do we need to clip at 32768?
  return static_cast<t_integertype>(v1);
}



//*************
// U16PS -> U16
//*************
image::t_integertype image::TTColorConv::U16FromU16PS(const t_integertype v){
  return (v * 0xffff) / 0x8000;
}

//*************
// U16 -> U16PS
//*************
image::t_integertype image::TTColorConv::U16PSFromU16(const t_integertype v){
  return (v * 0x8000) / 0xffff;
}


//***********
// U8 -> PROC
//***********
image::t_proc image::TTColorConv::ProcFromU8(const t_integertype v){
  switch (PROC_TYPE){
    case INT16:   return static_cast<t_proc>(U16FromU8(v));
    case FLOAT32: return F32FromU8(v);
    default:    return 0;
  }
}

//***********
// PROC -> U8
//***********
image::t_integertype image::TTColorConv::U8FromProc(const t_proc v){
  switch (PROC_TYPE){
    case INT16:   return U8FromU16(static_cast<t_integertype>(v));
    case FLOAT32: return U8FromF32(v);
    default:    return 0;
  }
}

  
//************
// U16 -> PROC
//************
image::t_proc image::TTColorConv::ProcFromU16(const t_integertype v){
  switch (PROC_TYPE){
    case INT16:   return static_cast<t_proc>(v);
    case FLOAT32: return F32FromU16(v);
    default:    return 0;
  }
}

//************
// PROC -> U16
//************
image::t_integertype image::TTColorConv::U16FromProc(const t_proc v){
  switch (PROC_TYPE){
    case INT16:   return static_cast<t_integertype>(v);
    case FLOAT32: return U16FromF32(v);
    default:    return 0;
  }
}

//********************************
// GetGrayByRGB
//
// simple gray conversion method
// without colormetric correctness
//********************************
image::t_proc image::TTColorConv::GetSimpleGrayByRGB(const t_proc R, const t_proc G, const t_proc B){
  return static_cast<t_proc>(0.3f * R + 0.59f * G + 0.11f * B);
}

//********************************
// GetLinearFromMonitor
//
// monitor -> linear values 2.2
//********************************
image::t_proc image::TTColorConv::GetLinearFromMonitor(const t_proc v){
  F32 vf; 
  switch (PROC_TYPE){
    case INT16:   vf = F32FromU16(static_cast<image::t_integertype>(v)); break;
    case FLOAT32: vf = v; break;
    default:      return 0;
  }

  F32 lin;
  if (vf <= 0.04045f){
    lin = vf / 12.92f;
  }
  else{
    const F32 a = 0.055f;
    lin = (vf + a) / (1.0f + a);
    lin = pow(lin, 2.4f);
  }

  switch (PROC_TYPE){
    case INT16:   return static_cast<image::t_proc>(U16FromF32(lin));
    case FLOAT32: return lin;
    default:      return 0;
  }
}

//********************************
// GetMonitorFromLinear
//
// linear values -> monitor 2.2
//********************************
image::t_proc image::TTColorConv::GetMonitorFromLinear(const t_proc v){
  F32 vf;
  switch (PROC_TYPE){
    case INT16:   vf = F32FromU16(static_cast<image::t_integertype>(v)); break;
    case FLOAT32: vf = v; break;
    default:      return 0;
  }

  F32 mon;
  if (vf <= 0.0031308f){
    mon = vf * 12.92f;
  }
  else{
    const F32 a = 0.055f;
    mon = (1.0f + a) * pow(vf, 1.0f / 2.4f) - a;
  }

  switch (PROC_TYPE){
    case INT16:   return static_cast<image::t_proc>(U16FromF32(mon));
    case FLOAT32: return mon;
    default:      return 0;
  }
}




//*********
// ProcZero
//*********
image::t_proc image::TTColorConv::ProcZero(){
  switch (PROC_TYPE){
    case INT16:   return 0;
    case FLOAT32: return 0.0f;
    default:    return 0;
  }
}

//********
// ProcMax
//********
image::t_proc image::TTColorConv::ProcMax(){
  switch (PROC_TYPE){
    case INT16:   return 0xFFFF;
    case FLOAT32: return 1.0f;
    default:    return 0;
  }
}


//****************
// ProcAlphaOpaque
//****************
image::t_proc image::TTColorConv::ProcAlphaOpaque(){
  switch (PROC_TYPE){
  case INT16:   return 0xFFFF;
  case FLOAT32: return 1.0f;
  default:    return 0;
  }
}


//*********************
// ProcAlphaTransparent
//*********************
image::t_proc image::TTColorConv::ProcAlphaTransparent(){
  switch (PROC_TYPE){
  case INT16:   return 0;
  case FLOAT32: return 0.0f;
  default:    return 0;
  }
}



//******************************************************************************
//*                               TTColorMetric                                *
//*                                                                            *
//*                 class for colormetric color conversions                    *
//*                                                                            *
//* NOTE: all values are float from 0.0 ... 1.0                                *
//******************************************************************************

//************
// XYZ -> xyY 
//************
void image::TTColorMetric::yxYFromXYZ(const F32 x, const F32 y, const F32 z, F32& xo, F32& yo, F32& Yo){

}

//************
// XYZ <- xyY 
//************
void image::TTColorMetric::XYZFromyxY(const F32 x, const F32 y, const F32 Y, F32& xo, F32& yo, F32& zo){

}

//*************
// sRGB <- XYZ
//*************
void image::TTColorMetric::sRGBFromXYZ(const F32 x, const F32 y, const F32 z, F32& r, F32& g, F32& b){

}

//*************
// sRGB -> XYZ
//*************
void image::TTColorMetric::XYZFromysRGB(const F32 r, const F32 g, const F32 b, F32& x, F32& y, F32& z){

}

//*************
// sRGB <- xyY
//*************
void image::TTColorMetric::sRGBFromxyY(const F32 x, const F32 y, const F32 Y, F32& r, F32& g, F32& b){

}

//*************
// sRGB -> xyY
//*************
void image::TTColorMetric::xyYFromysRGB(const F32 r, const F32 g, const F32 b, F32& x, F32& y, F32& Y){

}

//*************
// sRGB <- YUV
//*************
void image::TTColorMetric::sRGBFromYUV(const F32 y, const F32 u, const F32 v, F32& r, F32& g, F32& b){
  const F32 rlin = v + y;
  const F32 blin = u + y;
  const F32 glin = (y - 0.2126f * rlin - 0.0722f * blin) / 0.7152f;

  r = TTColorConv::GetMonitorFromLinear(rlin);
  g = TTColorConv::GetMonitorFromLinear(glin);
  b = TTColorConv::GetMonitorFromLinear(blin);
}

//**********
// sRGB -> Y
//**********
F32 image::TTColorMetric::YFromsRGB(const F32 r, const F32 g, const F32 b){
  const F32 rlin = TTColorConv::GetLinearFromMonitor(r);
  const F32 glin = TTColorConv::GetLinearFromMonitor(g);
  const F32 blin = TTColorConv::GetLinearFromMonitor(b);
  const F32 y = 0.2126f * rlin + 0.7152f * glin + 0.0722f * blin;
  return y;
}

//*************
// sRGB -> YUV
//*************
void image::TTColorMetric::YUVFromsRGB(const F32 r, const F32 g, const F32 b, F32& y, F32& u, F32& v){
  const F32 rlin = TTColorConv::GetLinearFromMonitor(r);
  const F32 glin = TTColorConv::GetLinearFromMonitor(g);
  const F32 blin = TTColorConv::GetLinearFromMonitor(b);
  y = 0.2126f * rlin + 0.7152f * glin + 0.0722f * blin;
  v = rlin - y;
  u = blin - y;
}

//*************
// sRGB <- YUV
//*************
void image::TTColorMetric::sRGBlinFromYUV(const F32 y, const F32 u, const F32 v, F32& r, F32& g, F32& b){
  r = v + y;
  g = u + y;
  b = (y - 0.2126f * r - 0.0722f * b) / 0.7152f;
}

//*************
// sRGB -> YUV
//*************
F32 image::TTColorMetric::YFromsRGBlin(const F32 r, const F32 g, const F32 b){
  const F32 y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
  return y;
}

//*************
// sRGB -> YUV
//*************
void image::TTColorMetric::YUVFromsRGBlin(const F32 r, const F32 g, const F32 b, F32& y, F32& u, F32& v){
  y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
  v = r - y;
  u = b - y;
}

//***********************
// sRGB <- UV for viewing
//***********************
void image::TTColorMetric::sRGBFromUVmaxY(const F32 u, const F32 v, F32& r, F32& g, F32& b){

  const float f1 = 0.2126f / 0.7152f;
  const float f2 = 0.0722f / 0.7152f;

  F32 Y_min = f2 * u + f1 * v;
  Y_min = (-u > Y_min) ? -u : Y_min;
  Y_min = (-v > Y_min) ? -v : Y_min;

  F32 Y_max = f2 * u + f1 * v;
  Y_max = (-u < Y_max) ? -u : Y_max;
  Y_max = (-v < Y_max) ? -v : Y_max;
  Y_max = 1.0f - Y_max;

  const F32 Y = Y_min;

  //vorläufig ...
  //const F32 Y = 0.05;
  sRGBFromYUV(Y, u, v, r, g, b);
}

//******************************************************************************
//*                                TTColor                                     *
//*                                                                            *
//*               color class for color handling                               *
//******************************************************************************

//*************
// contructor 1
//*************
image::TTColor::TTColor(){}

//****************
// copy contructor
//****************
image::TTColor::TTColor(const TTColor& other) : C0(other.C0), C1(other.C1), C2(other.C2), C3(other.C3), Valid(other.Valid) {}

//****************
// contructor gray
//****************
image::TTColor::TTColor(const t_proc gray) : C0(gray), Valid(true){}

//***************
// contructor rgb
//***************
image::TTColor::TTColor(const t_proc r, const t_proc g, const t_proc b) : C0(r), C1(g), C2(b), Valid(true) {}

//*********************
// contructor cmyk etc.
//**********************
image::TTColor::TTColor(const t_proc c, const t_proc m, const t_proc y, const t_proc k) : C0(c), C1(m), C2(y), C3(k), Valid(true) {}

//******************************
// IsValid
//
// returns if the color is valid
//******************************
bool image::TTColor::IsValid() const{
  return Valid;
}

//***********************************************
// IsNoRGBChannelZero
//
// returns if the color is valid for whitebalance
//***********************************************
bool image::TTColor::IsNoRGBChannelZero() const{
  return C1 > 0 && C2 > 0 && C3 > 0;
}

//********
// getter
//********
image::t_proc image::TTColor::GetGray() const {
  return C0;
}

//********
// getter
//********
image::t_proc image::TTColor::GetRed() const {
  return C0;
}

//********
// getter
//********
image::t_proc image::TTColor::GetGreen() const {
  return C1;
}

//********
// getter
//********
image::t_proc image::TTColor::GetBlue() const {
  return C2;
}

//********
// getter
//********
image::t_proc image::TTColor::GetCyan() const {
  return C0;
}

//********
// getter
//********
image::t_proc image::TTColor::GetMagenta() const {
  return C1;
}

//********
// getter
//********
image::t_proc image::TTColor::GetYellow() const {
  return C2;
}

//********
// getter
//********
image::t_proc image::TTColor::GetBlack() const {
  return C3;
}

//***********
//operator =
//***********
image::TTColor& image::TTColor::operator=(const TTColor& other){
  if (&other == this) return *this;
  C0 = other.C0;
  C1 = other.C1;
  C2 = other.C2;
  C3 = other.C3;
  Valid = other.Valid;
  return *this;
}

//***********
//operator +=
//***********
image::TTColor& image::TTColor::operator+=(const TTColor& other){
  C0 += other.C0;
  C1 += other.C1;
  C2 += other.C2;
  C3 += other.C3;
  return *this;
}

//***********
//operator +
//***********
image::TTColor image::TTColor::operator+(const TTColor& other) const {
  TTColor c(*this);
  c += other;
  return c;
}

//***********
//operator +=
//***********
image::TTColor& image::TTColor::operator+=(const t_proc offset){
  C0 += offset;
  C1 += offset;
  C2 += offset;
  C3 += offset;
  return *this;
}

//***********
//operator +
//***********
image::TTColor image::TTColor::operator+(const t_proc offset) const {
  TTColor c(*this);
  c += offset;
  return c;
}

//***********
//operator -=
//***********
image::TTColor& image::TTColor::operator-=(const TTColor& other){
  C0 -= other.C0;
  C1 -= other.C1;
  C2 -= other.C2;
  C3 -= other.C3;
  return *this;
}

//***********
//operator -
//***********
image::TTColor image::TTColor::operator-(const TTColor& other) const {
  TTColor c(*this);
  c -= other;
  return c;
}

//***********
//operator -=
//***********
image::TTColor& image::TTColor::operator-=(const t_proc offset){
  C0 -= offset;
  C1 -= offset;
  C2 -= offset;
  C3 -= offset;
  return *this;
}

//***********
//operator -
//***********
image::TTColor image::TTColor::operator-(const t_proc offset) const {
  TTColor c(*this);
  c += offset;
  return c;
}

//***********
//operator *=
//***********
image::TTColor& image::TTColor::operator*=(const TTColor& other){
  C0 *= other.C0;
  C1 *= other.C1;
  C2 *= other.C2;
  C3 *= other.C3;
  return *this;
}

//***********
//operator *
//***********
image::TTColor image::TTColor::operator*(const TTColor& other) const {
  TTColor c(*this);
  c *= other;
  return c;
}

//***********
//operator *=
//***********
image::TTColor& image::TTColor::operator*=(const float scalar){
  C0 *= scalar;
  C1 *= scalar;
  C2 *= scalar;
  C3 *= scalar;
  return *this;
}


//***********
//operator *
//***********
image::TTColor image::TTColor::operator*(const float scalar) const {
  TTColor c(*this);
  c *= scalar;
  return c;
}

//***********
//operator <
//***********
bool image::TTColor::operator<(const TTColor& other) const{
  if (C0 != other.C0) return C0 < other.C0;
  if (C1 != other.C1) return C1 < other.C1;
  if (C2 != other.C2) return C2 < other.C2;
  return C3 < other.C3;
}

//***********
//operator ==
//***********
bool image::TTColor::operator==(const TTColor& other) const{
  return this == &other || (   C0 == other.C0
                            && C1 == other.C1
                            && C2 == other.C2
                            && C3 == other.C3);
    
   
}

//***********
//operator !=
//***********
bool image::TTColor::operator!=(const TTColor& other) const{
  return this != &other && (   C0 != other.C0
                            || C1 != other.C1
                            || C2 != other.C2
                            || C3 != other.C3);
}


//***********
//operator *
//***********
image::TTColor operator*(const float scalar, const image::TTColor& other){
  image::TTColor result(other);
  result *= scalar;
  return result;
}


//***********
//operator <<
//***********
std::ostream& image::operator<<(std::ostream &output, const image::TTColor& color){
  output.write(reinterpret_cast<const char*>(&color.C0), sizeof(float));
  output.write(reinterpret_cast<const char*>(&color.C1), sizeof(float));
  output.write(reinterpret_cast<const char*>(&color.C2), sizeof(float));
  output.write(reinterpret_cast<const char*>(&color.C3), sizeof(float));
  return output;
}

//***********
//operator >>
//***********
std::istream& image::operator>>(std::istream &input, image::TTColor& color){
  input.read(reinterpret_cast<char*>(&color.C0), sizeof(float));
  input.read(reinterpret_cast<char*>(&color.C1), sizeof(float));
  input.read(reinterpret_cast<char*>(&color.C2), sizeof(float));
  input.read(reinterpret_cast<char*>(&color.C3), sizeof(float));
  return input;
}


//******************************************************************************
//*                                ColorPair                                   *
//*                                                                            *
//*                   struct for stroring color definitions                    *
//*                                                                            *                                             *                     
//******************************************************************************

//**********************************
// ColorDefinitionStruct::operator<<
//**********************************
std::ostream& image::operator<<(std::ostream &output, const image::ColorPair& cds){
  output << cds.Target;
  output << cds.Source;
  output.write(reinterpret_cast<const char*>(&cds.ColorValid), sizeof(bool));
  output.write(reinterpret_cast<const char*>(&cds.Used), sizeof(bool));
  return output;
}

//**********************************
// ColorDefinitionStruct::operator>>
//**********************************
std::istream& image::operator>>(std::istream &input, image::ColorPair& cds){
  input >> cds.Target;
  input >> cds.Source;
  input.read(reinterpret_cast<char*>(&cds.ColorValid), sizeof(bool));
  input.read(reinterpret_cast<char*>(&cds.Used), sizeof(bool));
  return input;
}


//********************
// default constructor
//********************
image::ColorPair::ColorPair(){}

//************
// constructor
//************
image::ColorPair::ColorPair(const image::TTColor& target, const image::TTColor& source) : Target(target), Source(source) {}

//*************
// SortGrayRamp
//**************
bool image::ColorPair::operator<(const image::ColorPair& other){
  switch (Sort){
  case SORTBY_RED:
    return Source.GetRed() < other.Source.GetRed();
  case SORTBY_GREEN:
    return Source.GetGreen() < other.Source.GetGreen();
  case SORTBY_BLUE:
    return Source.GetBlue() < other.Source.GetBlue();
  default:
    const image::t_proc y1 = image::TTColorMetric::YFromsRGB(Source.GetRed(), Source.GetGreen(), Source.GetBlue());
    const image::t_proc y2 = image::TTColorMetric::YFromsRGB(other.Source.GetRed(), other.Source.GetGreen(), other.Source.GetBlue());
    return y1 < y2;
  }
}



//******************************************************************************
//*                                TTColorArea                                 *
//*                                                                            *
//*                object to store an area where to look for colors            *
//******************************************************************************

//********************
// default constructor
//********************
image::TTColorArea::TTColorArea() : Valid(false){}


//************
// constructor
//************
image::TTColorArea::TTColorArea(const TTColorAreaType type, const float v0, const float v1, const float v2, const float v3)
                   :Type(type), V0(v0), V1(v1), V2(v2), V3(v3), Valid(true)
{
  if (Type == COLORAREATYPE_RECT){
    if (V0 > V2) {
      const float temp = V0;
      V0 = V2;
      V2 = temp;
    }
    if (V1 > V3) {
      const float temp = V1;
      V1 = V3;
      V3 = temp;
    }
  }
}


//***********************************************
// IsInside
//
// returns if point is inside (including border)
//***********************************************
bool image::TTColorArea::IsInside(const float x, const float y) const{
  switch (Type){

    case image::COLORAREATYPE_POINT:   return (V0 == x && V1 == y);

    case image::COLORAREATYPE_CIRCLE: {
                                        const float dx = x - V0;
                                        const float dy = y - V1;
                                        const float d = sqrt(dx * dx + dy * dy);
                                        return d <= V2;
                                      }

    case image::COLORAREATYPE_RECT:   return x >= V0 && y >= V1 && x <= V2 && y <= V3;

    default:                           return false;
  }
}

//*********
// IsValid
//*********
bool image::TTColorArea::IsValid() const{
  return Valid;
}

//*********
// SetValid
//*********
void image::TTColorArea::SetValid(const bool valid){
  Valid = valid;
}

//*********
// GetType
//*********
image::TTColorAreaType image::TTColorArea::GetType() const{
  return Type;
}

//**********
// GetValue0
//**********
float image::TTColorArea::GetValue0() const{
  return V0;
}

//**********
// GetValue1
//**********
float image::TTColorArea::GetValue1() const{
  return V1;
}

//**********
// GetValue2
//**********
float image::TTColorArea::GetValue2() const{
  return V2;
}

//**********
// GetValue3
//**********
float image::TTColorArea::GetValue3() const{
  return V3;
}

//***********
//operator ==
//***********
bool image::TTColorArea::operator==(const TTColorArea& other) const{
  if (Type != other.Type) return false;
  switch (Type){
    case image::COLORAREATYPE_POINT:  return V0 == other.V0 && V1 == other.V1;
    case image::COLORAREATYPE_CIRCLE: return V0 == other.V0 && V1 == other.V1 && V2 == other.V2;
    case image::COLORAREATYPE_RECT:   return V0 == other.V0 && V1 == other.V1 && V2 == other.V2 && V3 == other.V3;
    default:                          return V0 == other.V0 && V1 == other.V1 && V2 == other.V2 && V3 == other.V3;
  }
}

//***********
//operator !=
//***********
bool image::TTColorArea::operator!=(const TTColorArea& other) const{
  if (Type != other.Type) return true;
  switch (Type){
    case image::COLORAREATYPE_POINT:  return V0 != other.V0 || V1 != other.V1;
    case image::COLORAREATYPE_CIRCLE: return V0 != other.V0 || V1 != other.V1 || V2 != other.V2;
    case image::COLORAREATYPE_RECT:   return V0 != other.V0 || V1 != other.V1 || V2 != other.V2 || V3 != other.V3;
    default:                          return V0 != other.V0 || V1 != other.V1 || V2 != other.V2 || V3 != other.V3;
  }
}


//***********
//operator <<
//***********
std::ostream& image::operator<<(std::ostream &output, const image::TTColorArea& area){
  output.write(reinterpret_cast<const char*>(&area.Type), sizeof(TTColorAreaType));
  output.write(reinterpret_cast<const char*>(&area.V0), sizeof(float));
  output.write(reinterpret_cast<const char*>(&area.V1), sizeof(float));
  output.write(reinterpret_cast<const char*>(&area.V2), sizeof(float));
  output.write(reinterpret_cast<const char*>(&area.V3), sizeof(float));
  output.write(reinterpret_cast<const char*>(&area.Valid), sizeof(bool));
  return output;
}

//***********
//operator >>
//***********
std::istream& image::operator>>(std::istream &input, image::TTColorArea& area){
  input.read(reinterpret_cast<char*>(&area.Type), sizeof(TTColorAreaType));
  input.read(reinterpret_cast<char*>(&area.V0), sizeof(float));
  input.read(reinterpret_cast<char*>(&area.V1), sizeof(float));
  input.read(reinterpret_cast<char*>(&area.V2), sizeof(float));
  input.read(reinterpret_cast<char*>(&area.V3), sizeof(float));
  input.read(reinterpret_cast<char*>(&area.Valid), sizeof(bool));
  return input;
}


