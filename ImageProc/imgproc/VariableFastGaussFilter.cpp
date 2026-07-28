#include "VariableFastGaussFilter.h"


//*****************************************************************************
//*                           GaussFilterMatrix1D                             *
//*                                                                           *
//*****************************************************************************

//*************
// constructor
//*************
imgproc::GaussFilterMatrix1D::GaussFilterMatrix1D(){

}

//************
// destructor 
//************
imgproc::GaussFilterMatrix1D::~GaussFilterMatrix1D() {
  delete[] Matrix;
}


//*****************************************************************************
//*                         VariableFastGaussFilter                           *
//*                                                                           *
//*  implementing fast (two past) and variable sigma gauss filtering          *
//*  cached values can be stored for fast frame processing                    *
//*                                                                           *
//*****************************************************************************

//*************
// constructor
//*************
imgproc::VariableFastGaussFilter::VariableFastGaussFilter() 
{
}

//************
// destructor 
//************
imgproc::VariableFastGaussFilter::~VariableFastGaussFilter() {
  for (auto const&[key, val] : Matrizes) {
    delete val;
  }
}

//***************************
// FilterHor
//
// filter method horizontally 
//***************************
float imgproc::VariableFastGaussFilter::FilterHor(const ImageSource& source, const float radius, const unsigned int x, const unsigned  int y) {
  const GaussFilterMatrix1D* matrix = GetMatrix(radius);
  if (!matrix) {
    Logger::getInstance().println("FilterHor: matrix is null for: " + ttutil::StringUtil::double2string(radius) + " (" + ttutil::StringUtil::uint2string(x) + ", " + ttutil::StringUtil::uint2string(y) + ")");
  }
  float result = 0.0f;
  if (matrix->Size > 0) {
    const int center = matrix->Size / 2; //it is always odd...
    const int width = static_cast<int>(source.GetWidth());
    int xg;
    for (unsigned int i = 0; i < matrix->Size; ++i) {
      xg = static_cast<int>(x + i) - center;
      if (xg < 0) xg = 0;
      if (xg >= width) xg = width - 1;
      const float v = source.GetPixelValueAt(xg, y) * matrix->Matrix[i];
      result += v;
    }
  }
  else {
    result = source.GetPixelValueAt(x, y);
  }
  if (result < 0.0f) result = 0.0f;
  if (result > 1.0f) result = 1.0f;
  return result;
}

//*************************
// FilterVer
//
// filter method vertically
//*************************
float imgproc::VariableFastGaussFilter::FilterVer(const ImageSource& source, const float radius, const unsigned int x, const unsigned  int y) {
  const GaussFilterMatrix1D* const matrix = GetMatrix(radius);
  if (!matrix) {
    Logger::getInstance().println("FilterVer: matrix is null for: " + ttutil::StringUtil::double2string(radius) + " (" + ttutil::StringUtil::uint2string(x) + ", " + ttutil::StringUtil::uint2string(y) + ")");
  }
  float result = 0.0f;
  if (matrix->Size > 0) {
    const int center = matrix
      ->Size / 2; //it is always odd...
    const int height = static_cast<int>(source.GetHeight());
    int yg;
    for (unsigned int i = 0; i < matrix->Size; ++i) {
      yg = static_cast<int>(y + i) - center;
      if (yg < 0) yg = 0;
      if (yg >= height) yg = height - 1;
      const float v = source.GetPixelValueAt(x, yg) * matrix->Matrix[i];
      result += v;
    }
  }
  else {
    result = source.GetPixelValueAt(x, y);
  }

  if (result < 0.0f) result = 0.0f;
  if (result > 1.0f) result = 1.0f;
  return result;
}


//**********************************************
// GetMatrix
//
// returns a matrix for the given radius (range)
// if none exists it will be created
//**********************************************
const imgproc::GaussFilterMatrix1D* imgproc::VariableFastGaussFilter::GetMatrix(const float radius) {
  //this must be thread safe, to prevent undefined status of existing filter 
  {
    std::mutex m;
    std::lock_guard<std::mutex> lockGuard(m);
    
    const float range = GetRadiusRage(radius);
    if (Matrizes.find(range) == Matrizes.end()) {
      Logger::getInstance().println("GetMatrix: create matrix for: " + ttutil::StringUtil::double2string(radius) + " -> " + ttutil::StringUtil::double2string(range));
      const GaussFilterMatrix1D* const matrix = CreateMatrix(range);
      Matrizes[range] = matrix;
    }
    return Matrizes.at(range);
  }
}


//**************************************
// GetRadiusRage
//
// returns the begin of the radius range
//**************************************
float imgproc::VariableFastGaussFilter::GetRadiusRage(const float radius) {
  if (radius < 1.0f) return 0.0f;
  if (radius < 2.0f) return 1.5f;
  if (radius < 4.0f) return 3.0f;
  if (radius < 8.0f) return 6.0f;
  
  //default
  return 10.0f;
}


//*******************
// CreateMatrix
//
// creates the matrix
//*******************
imgproc::GaussFilterMatrix1D* imgproc::VariableFastGaussFilter::CreateMatrix(const float radius) {
  GaussFilterMatrix1D* const matrix = new GaussFilterMatrix1D();
  if (radius > 0.0f) {
    const float sigma = radius; //or radius/2
    const unsigned int maxindex = static_cast<unsigned int>(MAX_MATRIXSIZE_FACTOR * sigma);
    
    unsigned int size;
    const double* curve = GaussCurve(sigma, false, size);

    matrix->Matrix = new float[size];
    matrix->Size = size;

    for (unsigned int i = 0; i < size; i++) {
      matrix->Matrix[i] = static_cast<float>(curve[i]);
    }

    delete[] curve;
  }
  else {
    matrix->Matrix = nullptr;
    matrix->Size = 0;
  }
  return matrix;
}

