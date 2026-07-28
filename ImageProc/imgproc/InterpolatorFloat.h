#pragma once

#include <cmath>

namespace imgproc{
  
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
enum  InterpolationType {NEAREST_NEIGHBOUR, BILINEAR, BICUBIC};

const unsigned int RESOLUTION_BICUBIC_LUT = 1024;                 //-2.0 ... 2.0 
const unsigned int BICUBIC_LUT_ZERO = RESOLUTION_BICUBIC_LUT / 2; //0.0
const unsigned int BICUBIC_LUT_ONE  = RESOLUTION_BICUBIC_LUT / 4; //1.0

class InterpolatorFloat{
  
  public:
    
    //constructor 
    InterpolatorFloat(const unsigned int width,
                      const unsigned int height,
                      const InterpolationType type);
    
    //destructor 
    ~InterpolatorFloat();

    //interpolation
    float GetGrayPixel(const float x, const float y, const float* const imageData) const;


    
  private:
    
    //nearest neighbour interpolation
    float NearestNeighbour(const float x, const float y, const float* const imageData) const;
    
    //bilinear interpolation
    float BiLinear(const float x, const float y, const float* const imageData) const;
    
    //bicubic interpolation
    float BiCubic(const float x, const float y, const float* const imageData) const;
    
    //creates the lookup table for bicubic interpolation
    void CreateBicubicLUT();
    
    //calculates the matrix factors in one direction
    inline float CalcBicucMatrixOneDir(float d);
    
    //pointer to image data
    const float* ImageData;
    
    const unsigned int ImgWidth;
    const unsigned int ImgHeight;
    const InterpolationType Type;
    
    float* BiCubicLUT;
  
};
  
} // end of namespace imgproc
