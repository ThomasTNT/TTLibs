#ifndef _IMGPROC_INTERPOLATOR_
#define _IMGPROC_INTERPOLATOR_

#include <cmath>
#include "TTColor.h" //for data types

namespace imgproc{
  
//************************************************************************
//*                                                                      *
//*                            Interpolator                              *
//*                                                                      *
//*                 class for image pixel interpolation                  *
//*                                                                      *
//* tested ok 30th April 2008 by Thomas Thiele                           *
//*                                                                      *
//*                                                                      *
//************************************************************************
enum  InterpolationType {NEAREST_NEIGHBOUR, BILINEAR, BICUBIC};

const unsigned int RESOLUTION_BICUBIC_LUT = 1024;                 //-2.0 ... 2.0 
const unsigned int BICUBIC_LUT_ZERO = RESOLUTION_BICUBIC_LUT / 2; //0.0
const unsigned int BICUBIC_LUT_ONE  = RESOLUTION_BICUBIC_LUT / 4; //1.0

template<class T>
class Interpolator{
  
  public:
    
    //constructor 
    Interpolator(const T* const imagedata, 
                 const unsigned int width,
                 const unsigned int height,
                 const InterpolationType type);
    
    //destructor 
    ~Interpolator();
    
    //interpolation
    T GetGrayPixel(const double x, const double y);
    
  private:
    
    //nearest neighbour interpolation
    T NearestNeighbour(const double x, const double y);
    
    //bilinear interpolation
    T BiLinear(const double x, const double y);
    
    //bicubic interpolation
    T BiCubic(const double x, const double y);
    
    //creates the lookup table for bicubic interpolation
    void CreateBicubicLUT();
    
    //calculates the matrix factors in one direction
    inline double CalcBicucMatrixOneDir(double d);
    
    //pointer to image data
    const T* const ImageData; 
    
    const unsigned int ImgWidth;
    const unsigned int ImgHeight;
    const InterpolationType Type;
    
    S32* BiCubicLUT;
  
};
  
//DEFINTIONS OF FUNCTIONS IS HERE IN HEADER FILE, BECAUSE OF TEMPLATE

//************    
// constructor 
//************
template<class T>
imgproc::Interpolator<T>::Interpolator(const T* const imagedata, 
                                    const unsigned int width,
                                    const unsigned int height,
                                    const InterpolationType type)
                      :ImageData(imagedata),
                       ImgWidth(width),
                       ImgHeight(height),
                       Type(type),
                       BiCubicLUT(NULL)
{
  CreateBicubicLUT();
}

//***********  
// destructor 
//***********
template<class T>
imgproc::Interpolator<T>::~Interpolator(){
  delete [] BiCubicLUT;
}

//**************************************************************
// GetGrayPixel          
//
// actual interpolation function for images with one color layer
//**************************************************************
template<class T>
T imgproc::Interpolator<T>::GetGrayPixel(const double x, const double y)
{
  switch (Type){
    case BICUBIC:           return BiCubic(x, y);
    case BILINEAR:          return BiLinear(x, y);
    case NEAREST_NEIGHBOUR: return NearestNeighbour(x, y);
    default: return 0;
  }
}

//**************************************************************
// NearestNeighbour          
//
// actual interpolation function for images with one color layer
//**************************************************************
template<class T>
T imgproc::Interpolator<T>::NearestNeighbour(const double x, const double y)
{
  const int xg = static_cast<int>(x + 0.5);
  const int yg = static_cast<int>(y + 0.5);
  return   (xg >= 0 && xg < static_cast<int>(ImgWidth) && yg >= 0 && yg < static_cast<int>(ImgHeight))
         ? static_cast<T>(ImageData[xg + yg * ImgWidth])
         : 0;
}


//**************************************************************
// BiLinear          
//
// actual interpolation function for images with one color layer
//**************************************************************
template<class T>
T imgproc::Interpolator<T>::BiLinear(const double x, const double y)
{
  const int x0 = static_cast<int>(x);
  const int y0 = static_cast<int>(y);
  const int x1 = x0 + 1;
  const int y1 = y0 + 1;
  
  const U32 value00 =    (x0 >= 0 && x0 < static_cast<int>(ImgWidth) && y0 >= 0 && y0 < static_cast<int>(ImgHeight))
                       ? static_cast<T>(ImageData[x0 + y0 * ImgWidth])
                       : 0;
  const U32 value01 =    (x1 >= 0 && x1 < static_cast<int>(ImgWidth) && y0 >= 0 && y0 < static_cast<int>(ImgHeight))
                       ? static_cast<T>(ImageData[x1 + y0 * ImgWidth])
                       : 0;
  const U32 value10 =    (x0 >= 0 && x0 < static_cast<int>(ImgWidth) && y1 >= 0 && y1 < static_cast<int>(ImgHeight))
                       ? static_cast<T>(ImageData[x0 + y1 * ImgWidth])
                       : 0;
  const U32 value11 =    (x1 >= 0 && x1 < static_cast<int>(ImgWidth) && y1 >= 0 && y1 < static_cast<int>(ImgHeight))
                       ? static_cast<T>(ImageData[x1 + y1 * ImgWidth])
                       : 0;
                     
  const S32 dx = static_cast<S32>((x - static_cast<double>(x0)) * 0x2000);  // 0x2000 = 2^13 that should keep 
  const S32 dy = static_cast<S32>((y - static_cast<double>(y0)) * 0x2000);  // all values in the range of U32
  
  //QUESTION higher precision?  
  const S32 value0 = static_cast<S32>(value00) + ((dx * (static_cast<S32>(value01) - static_cast<S32>(value00))) / 0x2000);
  const S32 value1 = static_cast<S32>(value10) + ((dx * (static_cast<S32>(value11) - static_cast<S32>(value10))) / 0x2000);
  
  const U32 value = static_cast<U32>(value0 + ((dy * (value1 - value0)) / 0x2000));
  
  return value;  
}


//**************************************************************
// BiCubic          
//
// actual interpolation function for images with one color layer
//
//
// the matrix factors a 2^9 = 512 times bigger than real
// (real flaot factor * 512) 
//
//**************************************************************
template<class T>
T imgproc::Interpolator<T>::BiCubic(const double x, const double y){
  
  const int x1 = static_cast<int>(x);
  const int y1 = static_cast<int>(y);
  
  const int dx = static_cast<int>((x - static_cast<double>(x1)) * BICUBIC_LUT_ONE + 0.5);
  const int dy = static_cast<int>((y - static_cast<double>(y1)) * BICUBIC_LUT_ONE + 0.5);
  
  const int x0 = x1 - 1;
  const int y0 = y1 - 1;
  
  int value = 0;  //must be int because it can be negative
  int xg, yg;     //coordinates in org image
  int xl, yl;     //indecies of LUT
  S32 currvalue;  
  
  for (unsigned int i = 0; i < 4; i++){
    yg = y0 + i;
    if (yg >= 0 && yg < static_cast<int>(ImgHeight)){
      yl = (static_cast<int>(i) - 1) * BICUBIC_LUT_ONE - dy + BICUBIC_LUT_ZERO;
      if (yl >= 0 && yl < RESOLUTION_BICUBIC_LUT){
        for (unsigned int j = 0; j < 4; j++){
          xg = x0 + j;
          if (xg >= 0 && xg < static_cast<int>(ImgWidth)){
            xl = (j - 1) * BICUBIC_LUT_ONE - dx + BICUBIC_LUT_ZERO;
            if (xl >= 0 && xl < RESOLUTION_BICUBIC_LUT){
              currvalue = BiCubicLUT[xl] * BiCubicLUT[yl];
              currvalue /= 512;
              currvalue *= static_cast<S32>(ImageData[xg + yg * ImgWidth]);
              value += currvalue; 
            }
          }    
        }
      }
    }
  }
  
  U32 result = value < 0 ? 0 : (static_cast<U32>(value) >> 9);
  return static_cast<T>(result); 
}

//******************************************************
// CreateBicubicLUT          
//
// calculates lookup table for bicubic interpolation
//
// convolution matrix:
// (a + 2) |x|^3 - (a + 3) |x|^2 + 1  [0 <= x <= 1]
//  a|x|^3 - 5a|x|^2 + 8a|x| -4a      [1 < x <= 2]
//  0                               [otherwise]
//
// a is usually set to -0.5 or -0.75 
//
// each value is int times 512 
// resolution is 1024 x 1024
// 0    == -2.0 
// 1023 == +2.0 (ok...not exactly)
// 512  == 0 
//*****************************************************
template <class T>
void imgproc::Interpolator<T>::CreateBicubicLUT(){
  
  BiCubicLUT = new S32[RESOLUTION_BICUBIC_LUT];
  
  const unsigned int center = BICUBIC_LUT_ZERO; 
  
  double d;
  double value;
  
  for (unsigned int i = 0; i < RESOLUTION_BICUBIC_LUT; i++){
    d = static_cast<double>(static_cast<int>(i) - static_cast<int>(BICUBIC_LUT_ZERO)) / static_cast<double>(BICUBIC_LUT_ONE);
    value = CalcBicucMatrixOneDir(d);
    BiCubicLUT[i] = static_cast<int>(value * 512);
  }

}


//***********************************************
// CalcMatrixOneDir          
//
// calculates the matrix factors in one direction
//***********************************************
template <class T>
inline double imgproc::Interpolator<T>::CalcBicucMatrixOneDir(double d){
  const double a = -0.5; //-0.75
  double result; 
  if (d < 0) d = -d;
  if (d > 2)       result = 0; // d > 2
  else if (d <= 1) result = (a + 2) * (d * d * d) - (a + 3) * (d * d) + 1.0;  // 0 <= d <= 1
  else             result = a * (d * d * d) - 5.0 * a * (d * d) + 8.0 * a * d - 4.0 * a; // 1 < d <= 2
  return result;
}




} // end of namespace imgproc


#endif
