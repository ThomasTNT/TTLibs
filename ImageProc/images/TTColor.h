#pragma once

#include <iostream>
#include <fstream>
#include <string>

typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef          int   S32;
typedef float          F32; 

namespace image{


//******************************************************************************
//*                        general definition and constants                    *
//******************************************************************************

enum ColormodeType {UNKNOWN_COLORMODE, BIT, BIT_INVERTED, GRAYSCALE, GRAY_INVERTED, RGB, RRGGBB, BGR, LAB, CMYK, CCMMYYKK, RGBA, RRGGBBAA, YUV, OTHERMODE};
enum Datatyp { UNKNOWN_DATATYPE, INT8, INT16, INT16PS, INT32, FLOAT32, OTHERTYPE, PROCTYPE };
enum Channel {BINARY, GRAY, RED, GREEN, BLUE, CYAN, MAGANTA, YELLOW, BLACK, L, A, B};
enum ColorSpaceType { COLORSPACE_UNKNOWN, COLORSPACE_SRGB, COLORSPACE_ADOBERGB, COLORSPACE_YUV };

//internal logical data type
const Datatyp PROC_TYPE = FLOAT32;

//internal physical data type for image values
//WARNING: if you change this, check all methods that uses t_proc for implicit assumptions that it is float
typedef F32 t_proc;

//internal physical data type for integers
typedef U32 t_integertype;

//methods for printing 
std::string GetColormodeTypeName(const ColormodeType colormode);
std::string GetDatatypName(const Datatyp type);
std::string GetChannelName(const Channel channel);

//informations methods
ColormodeType GetSimpleColormodeType(const ColormodeType colormode);
bool GetInterlacedByColormodeType(const ColormodeType colormode);
unsigned int GetChannelsByColormodeType(const ColormodeType colormode);
unsigned int GetAlphaChannelsByColormodeType(const ColormodeType colormode);
unsigned int GetBytesByDatatyp(const Datatyp datatyp);


//******************************************************************************
//*                               TTColorConv                                  *
//*                                                                            *
//*                  class for color handling and conversion                   *
//******************************************************************************
class TTColorConv{
  
  public:

    //----------------------- static untility functions -----------------------

    //simple type conversions
    //NOTE for 16 bit: PS == photoshop 100% white == 0x8000 
    static t_integertype U16FromU8(const t_integertype v);
    static t_integertype U16FromU8PS(const t_integertype v);
    static t_integertype U8FromU16(const t_integertype v);
    static t_integertype U8FromU16PS(const t_integertype v);
    static F32 F32FromU8(const t_integertype v);
    static t_integertype U8FromF32(const F32 v);
    static F32 F32FromU16(const t_integertype v);
    static F32 F32FromU16PS(const t_integertype v);
    static t_integertype U16FromF32(const F32 v);
    static t_integertype U16FromF32PS(const F32 v);

    //16 bit conversions
    static t_integertype U16FromU16PS(const t_integertype v);
    static t_integertype U16PSFromU16(const t_integertype v);

    //process conversions
    static t_proc ProcFromU8(const t_integertype v);
    static t_integertype U8FromProc(const t_proc v);
    static t_proc ProcFromU16(const t_integertype v);
    static t_integertype U16FromProc(const t_proc v);

    //color conversions
    static t_proc GetSimpleGrayByRGB(const t_proc R, const t_proc G, const t_proc B);

    //linear <-> monitor
    static t_proc GetLinearFromMonitor(const t_proc v);
    static t_proc GetMonitorFromLinear(const t_proc v);

    //some constants for flexible use
    static t_proc ProcZero();
    static t_proc ProcMax();
    static t_proc ProcAlphaOpaque();
    static t_proc ProcAlphaTransparent();
};

//******************************************************************************
//*                               TTColorMetric                                *
//*                                                                            *
//*                 class for colormetric color conversions                    *
//*                                                                            *
//* NOTE: all values are float from 0.0 ... 1.0                                *
//******************************************************************************
class TTColorMetric{

  public:

    //XYZ <-> xyY 
    static void yxYFromXYZ(const F32 x, const F32 y, const F32 z, F32& xo, F32& yo, F32& Yo);
    static void XYZFromyxY(const F32 x, const F32 y, const F32 Y, F32& xo, F32& yo, F32& zo);

    //sRGB <-> XYZ
    static void sRGBFromXYZ(const F32 x, const F32 y, const F32 z, F32& r, F32& g, F32& b);
    static void XYZFromysRGB(const F32 r, const F32 g, const F32 b, F32& x, F32& y, F32& z);

    //sRGB <-> xyY
    static void sRGBFromxyY(const F32 x, const F32 y, const F32 Y, F32& r, F32& g, F32& b);
    static void xyYFromysRGB(const F32 r, const F32 g, const F32 b, F32& x, F32& y, F32& Y);

    //sRGB <-> YUV
    static void sRGBFromYUV(const F32 y, const F32 u, const F32 v, F32& r, F32& g, F32& b);
    static void YUVFromsRGB(const F32 r, const F32 g, const F32 b, F32& y, F32& u, F32& v);
    static void sRGBlinFromYUV(const F32 y, const F32 u, const F32 v, F32& r, F32& g, F32& b);
    static void YUVFromsRGBlin(const F32 r, const F32 g, const F32 b, F32& y, F32& u, F32& v);

    static F32  YFromsRGB(const F32 r, const F32 g, const F32 b);
    static F32  YFromsRGBlin(const F32 r, const F32 g, const F32 b);

    static void sRGBFromUVmaxY(const F32 u, const F32 v, F32& r, F32& g, F32& b);
};


//******************************************************************************
//*                                TTColor                                     *
//*                                                                            *
//*               color class for color handling                               *
//******************************************************************************
class TTColor{

  friend std::ostream& operator<<(std::ostream &output, const TTColor& color);
  friend std::istream& operator>>(std::istream &input, TTColor& color);

  public:

    //contructor 1
    TTColor();

    //copy contructor
    TTColor(const TTColor& other);

    //contructor gray
    TTColor(const t_proc gray);

    //contructor rgb
    TTColor(const t_proc r, const t_proc g, const t_proc b);

    //contructor cmyk etc.
    TTColor(const t_proc c, const t_proc m, const t_proc y, const t_proc k);

    //returns if the color is valid
    bool IsValid() const;

    // returns if the color is valid for whitebalance
    bool IsNoRGBChannelZero() const;

    //operators
    TTColor& operator=(const TTColor& other);
    TTColor& operator+=(const TTColor& other);
    TTColor operator+(const TTColor& other) const;
    TTColor& operator+=(const t_proc offset);
    TTColor operator+(const t_proc offset) const;
    TTColor& operator-=(const TTColor& other);
    TTColor operator-(const TTColor& other) const;
    TTColor& operator-=(const t_proc offset);
    TTColor operator-(const t_proc offset) const;
    TTColor& operator*=(const TTColor& other);
    TTColor operator*(const TTColor& other) const;
    TTColor& operator*=(const float scalar);
    TTColor operator*(const float scalar) const;
    bool operator<(const TTColor& other) const;
    bool operator==(const TTColor& other) const;
    bool operator!=(const TTColor& other) const;

   

    //getters
    t_proc GetGray() const;
    t_proc GetRed() const;
    t_proc GetGreen() const;
    t_proc GetBlue() const;
    t_proc GetCyan() const;
    t_proc GetMagenta() const;
    t_proc GetYellow() const;
    t_proc GetBlack() const;

  private:

    t_proc C0 = TTColorConv::ProcZero();
    t_proc C1 = TTColorConv::ProcZero();
    t_proc C2 = TTColorConv::ProcZero();
    t_proc C3 = TTColorConv::ProcZero();
    bool Valid = false;
};

TTColor& operator*(const float scalar, const TTColor& other);

std::ostream& operator<<(std::ostream &output, const TTColor& color);
std::istream& operator>>(std::istream &input, TTColor& color);


//******************************************************************************
//*                                ColorPair                                   *
//*                                                                            *
//*                   struct for stroring color definitions                    *
//*                                                                            *                                             *                     
//******************************************************************************

enum ColorPairSortCriterium{ SORTBY_LUM, SORTBY_RED, SORTBY_GREEN, SORTBY_BLUE };

class ColorPair{

  public:

    ColorPair();
    ColorPair(const image::TTColor& target, const image::TTColor& source);

    bool operator<(const ColorPair& other);

    image::TTColor Target;
    image::TTColor Source;
    bool ColorValid = false;
    bool Used = false;
    ColorPairSortCriterium Sort = SORTBY_LUM;
};


std::ostream& operator<<(std::ostream &output, const ColorPair& cp);
std::istream& operator>>(std::istream &input, ColorPair& cp);



//******************************************************************************
//*                                TTColorArea                                 *
//*                                                                            *
//*                object to store an area where to look for colors            *
//******************************************************************************
enum TTColorAreaType{ COLORAREATYPE_UNDEF, COLORAREATYPE_POINT, COLORAREATYPE_CIRCLE, COLORAREATYPE_RECT };
class TTColorArea{
  
  friend std::ostream& operator<<(std::ostream &output, const TTColorArea& area);
  friend std::istream& operator>>(std::istream &input, TTColorArea& area);

  public:
    
    //COLORAREATYPE_POINT: v0 == x, v1 == y
    //COLORAREATYPE_CIRCLE : v0 == x, v1 == y, v2 == r
    //COLORAREATYPE_RECT : v0 == x1, v1 == y1, v2 == x2, v3 = y2
    TTColorArea();
    TTColorArea(const TTColorAreaType type, const float v0, const float v1, const float v2 = 0.0f, const float v3 = 0.0f);

    bool IsInside(const float x, const float y) const;

    bool IsValid() const;

    void SetValid(const bool valid);

    TTColorAreaType GetType() const;

    float GetValue0() const;
    float GetValue1() const;
    float GetValue2() const;
    float GetValue3() const;

    bool operator==(const TTColorArea& other) const;
    bool operator!=(const TTColorArea& other) const;

  private:

    TTColorAreaType Type = COLORAREATYPE_UNDEF;
    float V0 = 0.0f;
    float V1 = 0.0f;
    float V2 = 0.0f;
    float V3 = 0.0f;

    bool Valid = true;

};

std::ostream& operator<<(std::ostream &output, const TTColorArea& color);
std::istream& operator>>(std::istream &input, TTColorArea& color);



} //end of namespace images