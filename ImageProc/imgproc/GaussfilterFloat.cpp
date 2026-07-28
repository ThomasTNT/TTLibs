#include "GaussfilterFloat.h"



//*****************************************************************************
//*                              GaussfilterFloat                             *
//*                                                                           *
//*                      help class for gaussfiltering                        *
//*                              float variant                                *
//*****************************************************************************

//************
// constructor
//************
imgproc::GaussfilterFloat::GaussfilterFloat() {}

//***********
// destructor
//***********
imgproc::GaussfilterFloat::~GaussfilterFloat() {
  delete[] Matrix;
}

//*********
// GetSize
//*********
unsigned int imgproc::GaussfilterFloat::GetSize() {
  return Size;
}


//******************************************************************************
// GaussCurve
//
// Creates a one dimensional dicrete gausscurve // where x = ..., -1, 0, +1, ...
// Size is set then
//
// if normize == true then the curve is normized to 1
//
// you have to delete this buffer with delete[]!
//******************************************************************************
double* imgproc::GaussfilterFloat::GaussCurve(const double sigma, const bool normize, unsigned int &size) {

  const unsigned int maxindex = static_cast<unsigned int>(MAX_MATRIXSIZE_FACTOR * sigma);
  size = maxindex * 2 + 1;
  double* curve = new double[size];

  if (sigma == 0.0) {
    curve[0] = 1.0;
    return curve;
  }

  const double factor = normize ? 1.0 : 1.0 / (sigma * ROOT_2PI);
  double value;
  for (unsigned int i = 0; i < size; i++) {
    value = static_cast<double>(static_cast<int>(i) - static_cast<int>(maxindex));
    curve[i] = factor * exp(-0.5 * value * value / (sigma * sigma));
  }
  return curve;
}

//**************************************************
// CreateMatrix1D
//
// if normize == true then the curve is normized to 1
//**************************************************
void imgproc::GaussfilterFloat::CreateMatrix1D(const double sigma, const bool normize) {
  if (sigma == 0) {
    Size = 0; //indicating no filtering
    return;
  }

  //Size is set after this
  //sum is slightly smaller than 1, (theoretically 1)
  double* curve = GaussCurve(sigma, normize, Size);

  //getting the _real_ sum
  double sum = 0;
  if (!normize) for (unsigned int i = 0; i < Size; i++) sum += curve[i];

  //setting te integer matrix
  //divide by the real sum
  Dimensions = 1;
  delete[] Matrix;
  Matrix = new float[Size];
  for (unsigned int i = 0; i < Size; i++) {
    if (normize) {
      Matrix[i] =  static_cast<float>(curve[i]);
    }
    else {
      Matrix[i] = static_cast<float>(curve[i] / sum);
    }
  }

  delete[] curve;
}


//******************************************************
// CreateMatrix1DbySize
//
// creates a one dimensional filter via binomenal method // fixed sigma
//******************************************************
void imgproc::GaussfilterFloat::CreateMatrix1DbySize(const unsigned int size) {
  // 0.2 gave best results, simply by trying
  CreateMatrix1D(SIGMA_PIXEL * static_cast<double>(size) * 0.2, false);
}

//**************************************************
// FilterScanline
//
// filters one scanline
// scanline and tempscanline must have size >= width
// filter must be 1D-filtering
//**************************************************
void imgproc::GaussfilterFloat::FilterScanline(float* scanline, float* tempscanline, const unsigned int width, const bool copyBack) {
  const int center = Size / 2; //it is always odd...

  //filter templine
  int xg;
  for (unsigned int x = 0; x < width; ++x) {
    tempscanline[x] = 0;
    for (unsigned int i = 0; i < Size; ++i) {
      xg = static_cast<int>(x + i) - center;
      if (xg < 0) xg = 0;
      if (xg >= static_cast<int>(width)) xg = width - 1;
      tempscanline[x] += scanline[xg] * Matrix[i];
    }
  }

  //copy templine back to scanline
  for (unsigned int i = 0; i < width; i++) scanline[i] = tempscanline[i];
}


//*****************************************************************
// FilterSingleValue
//
// filters one value
// the point on given index
// to FilterScanline borders are extende and filled with last pixel
//*****************************************************************
float imgproc::GaussfilterFloat::FilterSingleValue(float* scanline, const unsigned int index, const unsigned int width) {
  float value = 0;
  const unsigned int center = Size / 2; //it is always odd...
  for (unsigned int i = 0; i < Size; ++i) {
    const unsigned int xg =    ((index + i) > center) 
                            ?  ((index + i) < width) ? index + i - center : width - 1 
                            : 0;
    value += scanline[xg] * Matrix[i];
  }
  return value;
}




