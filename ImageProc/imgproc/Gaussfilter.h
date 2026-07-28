#ifndef __GAUSSFILTER_H_
#define __GAUSSFILTER_H_

#include <cmath>
#include "TTColor.h"

namespace imgproc{

const double PI = 3.14159265;
const double SIGMA_PIXEL = 0.84932;  //sigma where gauss(1) = 0.5
const double FILTER_EXPSILON = 0.01; //cooeficients smaller that FILTER_EXPSILON are set to zero
                                     //and matrix is truncanted
const double MAX_MATRIXSIZE_FACTOR = sqrt(-2.0 * log(FILTER_EXPSILON)); //will be multiplyed with sigma
const double ROOT_2PI = sqrt(2 * PI);

//*****************************************************************************
//*                           global functions                                *
//*****************************************************************************

//creates gauss curve
//you have to delete this buffer with delete[]!
double* GaussCurve(const double sigma, const bool normize, unsigned int &size);



//*****************************************************************************
//*                               Gaussfilter                                 *
//*                                                                           *
//*                      help class for gaussfiltering                        *
//*                                                                           *
//*****************************************************************************
const unsigned int FILTER_ONE = 0x10000;   //the value that represents 1 during integer filtering
                                           //where FILTER_ONE * 0x8000 <= 0xFFFFFFFF!
                                           //but as high as possible -> better resolution
                                           //0x10000 == 2<16

class Gaussfilter{
  public:
   
    //constructor
    Gaussfilter();

    //destructor
    ~Gaussfilter();

    //creates matrix method with given sigma
    void CreateMatrix2D(const double sigma, const unsigned int factortype);

    //creates a size * size matrix method with fixed sigma
    void CreateMatrix2DbySize(const unsigned int size);

    //creates a one dimensional filterwith given sigma
    void CreateMatrix1D(const double sigma, const bool normize);

    //creates a one dimensional filter with fixed sigma
    void CreateMatrix1DbySize(const unsigned int size);

    //filter function, returns the weighted sum of all pixels
    U32 Filter(const U32* image, const int x, const int y, const int width, const int height);
    U8  Filter(const U8*  image, const int x, const int y, const int width, const int height);

    //smears the value according to the filter to the image
    void Smear(U32* image, const U32 value, const int x, const int y, const int width, const int height);
    void Smear(U32* image, const U32 value, const double x, const double y, const int width, const int height);

    //filters one scanline
    void FilterScanline(U32* scanline, U32* tempscanline, const unsigned int width, const bool copyBack);

    //filters one scanline
    void FilterLine(double* scanline, double* tempscanline, const unsigned int width, const bool copyBack);

    //returns the number of needed neigtbor pixels in each direction
    unsigned int Neighborpixels();

  private:

    void SmearAddInterpolate(U32* image, const U32 value, const double x, const double y, const int width, const int height);

    U32* matrix;
    unsigned int Size;
    unsigned int Dimensions;
};


//*****************************************************************************
//*                           FastImgGaussfilter                              *
//*                                                                           *
//*                   class for gaussfiltering but faster                     *
//*                                                                           *
//*****************************************************************************
class FastImgGaussfilter{
  public:
   
    //constructor
    FastImgGaussfilter();

    //destructor
    ~FastImgGaussfilter();

    //creates matrix method with given sigma
    void CreateMatrix(const double sigma, const unsigned int factortype);

    //filter function, returns the weighted sum of all pixels
    void Filter(const U32* inimg, U32* tempBuffer, U32* outimg, const int width, const int height, const bool normize);

    //filter function, returns the weighted sum of all pixels
    void Filter(const U8* inimg,   //size = inwidth * inheight (whole image) 
                U32* tempBuffer,   //size = outwidth * outheight, make sure that it is!
                U8* outimg,        //size = outwidth * outheight (tile) 
                const unsigned int inwidth, const unsigned int inheight,   //dimension of input buffer (whole image) 
                const unsigned int outwidth, const unsigned int outheight, //dimension of output buffer (tile) 
                const int offsetX, const int offsetY,                      //offset output buffer to inputbuffer (tile in image) 
                const bool normize);                     

    //returns the number of needed neigtbor pixels in each direction
    unsigned int Neighborpixels();

  private:

    U32* matrix;
    unsigned int Size;
};

} //end of namespace imgproc

#endif
