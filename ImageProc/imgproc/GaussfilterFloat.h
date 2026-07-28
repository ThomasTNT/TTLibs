#pragma once

#include <cmath>

namespace imgproc {


const double PI = 3.14159265;
const double SIGMA_PIXEL = 0.84932;  //sigma where gauss(1) = 0.5
const double FILTER_EXPSILON = 0.01; //cooeficients smaller that FILTER_EXPSILON are set to zero
                                     //and matrix is truncanted
const double MAX_MATRIXSIZE_FACTOR = sqrt(-2.0 * log(FILTER_EXPSILON)); //will be multiplyed with sigma
const double ROOT_2PI = sqrt(2 * PI);


//*****************************************************************************
//*                              GaussfilterFloat                             *
//*                                                                           *
//*                      help class for gaussfiltering                        *
//*                              float variant                                *
//*****************************************************************************
class GaussfilterFloat {

  public:

    //creates gauss curve
    //you have to delete this buffer with delete[]!
    static double* GaussCurve(const double sigma, const bool normize, unsigned int &size);

    //constructor
    GaussfilterFloat();

    //destructor
    ~GaussfilterFloat();

    //creates a one dimensional filterwith given sigma
    void CreateMatrix1D(const double sigma, const bool normize);

    //creates a one dimensional filter with fixed sigma
    void CreateMatrix1DbySize(const unsigned int size);

    //filters one scanline
    //scanline and tempscanline must have size >= width
    //filter must be 1D-filtering
    void FilterScanline(float* scanline, float* tempscanline, const unsigned int width, const bool copyBack);

    //filters one value
    //the point on given index
    //contrary to FilterScanline borders are extende and filled with last pixel
    float FilterSingleValue(float* scanline, const unsigned int index, const unsigned int width);


    //gets the size 
    //neded for precalculation
    unsigned int GetSize();

  private:

    
    float * Matrix = nullptr;
    unsigned int Size = 0;
    unsigned int Dimensions = 0;
};

}