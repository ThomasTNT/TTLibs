#include "InterpolatorFloat.h"

//************************************************************************
//*                                                                      *
//*                            Interpolator                              *
//*                                                                      *
//*                 class for image pixel interpolation                  *
//*                                                                      *
//*  only float and grey images without padding                          *
//*                                                                      *
//*                                                                      *
//************************************************************************


//************    
// constructor 
//************
imgproc::InterpolatorFloat::InterpolatorFloat(const unsigned int width,
                                              const unsigned int height,
                                              const InterpolationType type)
                                             :ImgWidth(width),
                                              ImgHeight(height),
                                              Type(type),
                                              BiCubicLUT(NULL)
{
  CreateBicubicLUT();
}


//***********  
// destructor 
//***********
imgproc::InterpolatorFloat::~InterpolatorFloat() {
  delete[] BiCubicLUT;
}



//**************************************************************
// GetGrayPixel          
//
// actual interpolation function for images with one color layer
//**************************************************************
float imgproc::InterpolatorFloat::GetGrayPixel(const float x, const float y, const float* const imageData) const
{
  switch (Type) {
    case BICUBIC:           return BiCubic(x, y, imageData);
    case BILINEAR:          return BiLinear(x, y, imageData);
    case NEAREST_NEIGHBOUR: return NearestNeighbour(x, y, imageData);
    default: return 0.0f;
  }
}



//**************************************************************
// NearestNeighbour          
//
// actual interpolation function for images with one color layer
//**************************************************************
float imgproc::InterpolatorFloat::NearestNeighbour(const float x, const float y, const float* const imageData) const
{
  const int xg = static_cast<int>(x + 0.5f);
  const int yg = static_cast<int>(y + 0.5f);
  return   (xg >= 0 && xg < static_cast<int>(ImgWidth) && yg >= 0 && yg < static_cast<int>(ImgHeight))
         ? (imageData[xg + yg * ImgWidth])
         : 0.0f;
}


//**************************************************************
// BiLinear          
//
// actual interpolation function for images with one color layer
//**************************************************************
float imgproc::InterpolatorFloat::BiLinear(const float x, const float y, const float* const imageData) const
{
  const int x0 = static_cast<int>(x);
  const int y0 = static_cast<int>(y);
  const int x1 = x0 + 1;
  const int y1 = y0 + 1;

  const float value00 =   (x0 >= 0 && x0 < static_cast<int>(ImgWidth) && y0 >= 0 && y0 < static_cast<int>(ImgHeight))
                        ? imageData[x0 + y0 * ImgWidth]
                        : 0.0f;
  const float value01 =   (x1 >= 0 && x1 < static_cast<int>(ImgWidth) && y0 >= 0 && y0 < static_cast<int>(ImgHeight))
                        ? imageData[x1 + y0 * ImgWidth]
                        : 0.0f;
  const float value10 =   (x0 >= 0 && x0 < static_cast<int>(ImgWidth) && y1 >= 0 && y1 < static_cast<int>(ImgHeight))
                        ? imageData[x0 + y1 * ImgWidth]
                        : 0.0f;
  const float value11 =   (x1 >= 0 && x1 < static_cast<int>(ImgWidth) && y1 >= 0 && y1 < static_cast<int>(ImgHeight))
                        ? imageData[x1 + y1 * ImgWidth]
                        : 0.0f;

  const float dx = (x - static_cast<float>(x0)); 
  const float dy = (y - static_cast<float>(y0)); 
 
  const float value0 = value00 + ((dx * (value01 - value00)));
  const float value1 = value10 + ((dx * (value11 - value10)));
  const float value =  value0 + (dy * (value1 - value0));

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
float imgproc::InterpolatorFloat::BiCubic(const float x, const float y, const float* const imageData) const
{
  const int x1 = static_cast<int>(x);
  const int y1 = static_cast<int>(y);

  const int dx = static_cast<int>((x - static_cast<float>(x1)) * BICUBIC_LUT_ONE + 0.5f);
  const int dy = static_cast<int>((y - static_cast<float>(y1)) * BICUBIC_LUT_ONE + 0.5f);

  const int x0 = x1 - 1;
  const int y0 = y1 - 1;

  float value = 0;  
  int xg, yg;     //coordinates in org image
  int xl, yl;     //indecies of LUT
  float currvalue;

  for (unsigned int i = 0; i < 4; i++) {
    yg = y0 + i;
    if (yg >= 0 && yg < static_cast<int>(ImgHeight)) {
      yl = (static_cast<int>(i) - 1) * BICUBIC_LUT_ONE - dy + BICUBIC_LUT_ZERO;
      if (yl >= 0 && yl < RESOLUTION_BICUBIC_LUT) {
        for (unsigned int j = 0; j < 4; j++) {
          xg = x0 + j;
          if (xg >= 0 && xg < static_cast<int>(ImgWidth)) {
            xl = (j - 1) * BICUBIC_LUT_ONE - dx + BICUBIC_LUT_ZERO;
            if (xl >= 0 && xl < RESOLUTION_BICUBIC_LUT) {
              currvalue = BiCubicLUT[xl] * BiCubicLUT[yl];
              currvalue *= imageData[xg + yg * ImgWidth];
              value += currvalue;
            }
          }
        }
      }
    }
  }

  const float result = value < 0.0f ? 0.0f : value;
  return result;
}



//******************************************************
// CreateBicubicLUT          
//
// calculates lookup table for bicubic interpolation
//
// convolution matrix:
// (a + 2) |x|^3 - (a + 3) |x|^2 + 1  [0 <= x <= 1]
//  a|x|^3 - 5a|x|^2 + 8a|x| -4a      [1 < x <= 2]
//  0                                 [otherwise]
//
// a is usually set to -0.5 or -0.75 
//*****************************************************
void imgproc::InterpolatorFloat::CreateBicubicLUT() {

  BiCubicLUT = new float[RESOLUTION_BICUBIC_LUT];

  for (unsigned int i = 0; i < RESOLUTION_BICUBIC_LUT; i++) {
    const float d = static_cast<float>(static_cast<int>(i) - static_cast<int>(BICUBIC_LUT_ZERO)) / static_cast<float>(BICUBIC_LUT_ONE);
    BiCubicLUT[i] = CalcBicucMatrixOneDir(d);
  }
}



//***********************************************
// CalcMatrixOneDir          
//
// calculates the matrix factors in one direction
//***********************************************
float imgproc::InterpolatorFloat::CalcBicucMatrixOneDir(float d) {
  const float a = -0.5f; //-0.75
  float result;
  if (d < 0) d = -d;
  if (d > 2)       result = 0; // d > 2
  else if (d <= 1) result = (a + 2.0f) * (d * d * d) - (a + 3.0f) * (d * d) + 1.0f;         // 0 <= d <= 1
  else             result = a * (d * d * d) - 5.0f * a * (d * d) + 8.0f * a * d - 4.0f * a; // 1 < d <= 2
  return result;
}
