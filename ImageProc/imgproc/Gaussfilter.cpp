#include "Gaussfilter.h"


//*****************************************************************************
//*                           global functions                                *
//*****************************************************************************

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
double* imgproc::GaussCurve(const double sigma, const bool normize, unsigned int &size){

  const unsigned int maxindex = static_cast<unsigned int>(MAX_MATRIXSIZE_FACTOR * sigma);
  size = maxindex * 2 + 1;
  double* curve = new double[size];

  if (sigma == 0.0){
    curve[0] = 1.0;
    return curve;
  }

  const double factor = normize ? 1.0 : 1.0 / (sigma * ROOT_2PI);
  double value;
  for (unsigned int i = 0; i < size; i++){
    value = static_cast<double>(static_cast<int>(i) - static_cast<int>(maxindex));
    curve[i] = factor * exp( -0.5 * value * value / (sigma * sigma) );
  }
  return curve;
}

//*****************************************************************************
//*                               Gaussfilter                                 *
//*                                                                           *
//*                      help class for gaussfiltering                        *
//*                                                                           *
//*****************************************************************************
 
//************
// constructor
//************
imgproc::Gaussfilter::Gaussfilter() : matrix(0), Size(0), Dimensions(0) {}
 
//***********
// destructor
//***********
imgproc::Gaussfilter::~Gaussfilter(){
  delete[] matrix;
}

//***********************************************************************
// CreateMatrix2D
//
// if factortype == 0 then the curve not normized 
// if factortype == 1 then the curve is normized to 1
// if factortype >  1 then the curve is normized to 1 regarding scanlines
//***********************************************************************
void imgproc::Gaussfilter::CreateMatrix2D(const double sigma, const unsigned int factortype){
  if (sigma == 0){
    Size = 0; //indicating no filtering
    return;
  }
  //Size is set after this
  //sum is slightly smaller than 1, (theoretically 1)
  double* curve = GaussCurve(sigma, (factortype > 0), Size);
  //getting the _real_ sum
  double sum = 0;
  if (factortype == 1){
    for (unsigned int iy = 0; iy < Size; iy++){
      for (unsigned int ix = 0; ix < Size; ix++){
        sum += curve[iy] * curve[ix]; 
      }
    }
  }
  else if (factortype > 1){
    for (unsigned int iy = (Size/2) % factortype; iy < Size; iy+= factortype){
      for (unsigned int ix = 0; ix < Size; ix++){
        sum += curve[iy] * curve[ix]; 
      }
    }
  }
  
  //setting te integer matrix
  //divide by the real sum
  Dimensions = 2;
  delete[] matrix;
  matrix = new U32[Size*Size];
  for (unsigned int iy = 0; iy < Size; iy++){
    for (unsigned int ix = 0; ix < Size; ix++){
      if (factortype == 0){
        matrix[ix + iy * Size] = static_cast<U32>(static_cast<double>(FILTER_ONE) * curve[iy] * curve[ix] + 0.5); 
      }
      else{
        matrix[ix + iy * Size] = static_cast<U32>(static_cast<double>(FILTER_ONE) * curve[iy] * curve[ix] / sum + 0.5); 
      }
    }
  }

  //#############################################
  /*Logger::getInstance().println("************************GAUSSFILTER ***********************");
  for (unsigned int iy = 0; iy < Size; iy++){
    for (unsigned int ix = 0; ix < Size; ix++){
      Logger::getInstance().print(util::StringUtil::uint2string(matrix[ix + iy * Size]) + " ");
    }
    Logger::getInstance().println("");
  }
  Logger::getInstance().println("******************************************************************");*/
  //############################################


  delete[] curve;
}

//**************************************************
// CreateMatrix1D
//
// if normize == true then the curve is normized to 1
//**************************************************
void imgproc::Gaussfilter::CreateMatrix1D(const double sigma, const bool normize){
  if (sigma == 0){
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
  delete[] matrix;
  matrix = new U32[Size];
  for (unsigned int i = 0; i < Size; i++){
    if (normize){
      matrix[i] = static_cast<U32>(static_cast<double>(FILTER_ONE) * curve[i] + 0.5); 
    }
    else{
      matrix[i] = static_cast<U32>(static_cast<double>(FILTER_ONE) * curve[i]/ sum + 0.5); 
    }
  }

  delete[] curve;
}


//**************************************************
// CreateMatrix2D
//
// creates a size * size matrix via binomenal method // fixed sigma
//**************************************************
void imgproc::Gaussfilter::CreateMatrix2DbySize(const unsigned int size){
  // 0.18 gave best results
  // itis good compromise between a blured image and 
  // the theoretical low pass filterring (0.5)  
  CreateMatrix2D(SIGMA_PIXEL * static_cast<double>(size) * 0.18, false); 
}

//******************************************************
// CreateMatrix1D
//
// creates a one dimensional filter via binomenal method // fixed sigma
//******************************************************
void imgproc::Gaussfilter::CreateMatrix1DbySize(const unsigned int size){
  // 0.2 or gave best results
  CreateMatrix1D(SIGMA_PIXEL * static_cast<double>(size) * 0.2, false);
}

//***************************************
// Filter
//
// filter function
// returns the weighted sum of all pixels
// make sure we have 2D filterting
// and valid Size
//
// Borderhandling: expand with same value
//***************************************
U32 imgproc::Gaussfilter::Filter(const U32* image, const int x, const int y, const int width, const int height){
  //no filering if Size <= 1
  if (Size <= 1){
    return (x >= 0 && x < width && y >= 0 && y < height) ? image[x + y * width] : 0;
  }

  const int center = Size / 2; //it is always odd...
  int xg, yg;
  U32 value = 0;
  for (unsigned int i = 0; i < Size; i++){
    yg = static_cast<int>(y + i) - center;
    if (yg < 0) yg = 0;
    if (yg >= height) yg = height - 1;
    for (unsigned int j = 0; j < Size; j++){
      xg = static_cast<int>(x + j) - center;
      if (xg < 0) xg = 0;
      if (xg >= width) xg = width - 1;
      value += ((matrix[j + i * Size] * image[xg + yg * width]) >> 16);
    }
  }
  //value >> 16;
  return value;
}

//**************************************************
// FilterScanline
//
// filters one scanline
// scanline and tempscanline must have size >= width
// filter must be 1D-filtering
//**************************************************
void imgproc::Gaussfilter::FilterScanline(U32* scanline, U32* tempscanline, const unsigned int width, const bool copyBack){
  const int center = Size / 2; //it is always odd...

  //filter templine
  int xg;
  for (unsigned int x = 0; x < width; x++){
    tempscanline[x] = 0;
    for (unsigned int i = 0; i < Size; i++){
      xg = static_cast<int>(x + i) - center;
      if (xg < 0) xg = 0;
      if (xg >= static_cast<int>(width)) xg = width - 1;
      tempscanline[x] += (scanline[xg] * matrix[i]) >> 16;
    }
  }

  //copy templine back to scanline
  for (unsigned int i = 0; i < width; i++) scanline[i] = tempscanline[i];
}

//**************************************************
// FilterScanline
//
// filters one line
//**************************************************
void imgproc::Gaussfilter::FilterLine(double* scanline, double* tempscanline, const unsigned int width, const bool copyBack){
  const int center = Size / 2; //it is always odd...

  //filter templine
  int xg;
  for (unsigned int x = 0; x < width; x++){
    tempscanline[x] = 0;
    for (unsigned int i = 0; i < Size; i++){
      xg = static_cast<int>(x + i) - center;
      if (xg < 0) xg = 0;
      if (xg >= static_cast<int>(width)) xg = width - 1;
      tempscanline[x] += scanline[xg] * static_cast<double>(matrix[i]) / static_cast<double>(FILTER_ONE);
    }
  }

  //copy templine back to scanline
  for (unsigned int i = 0; i < width; i++) scanline[i] = tempscanline[i];
}

//***************************************
// Filter
//
// filter function
// returns the weighted sum of all pixels
// make sure we have 2D filterting
// and valid Size
//
// Borderhandling: expand with same value
//***************************************
U8 imgproc::Gaussfilter::Filter(const U8* image, const int x, const int y, const int width, const int height){
  //no filering if Size <= 1
  if (Size <= 1){
    return (x >= 0 && x < width && y >= 0 && y < height) ? image[x + y * width] : 0;
  }

  const int center = Size / 2; //it is always odd...
  int xg, yg;
  U32 value = 0;
  for (unsigned int i = 0; i < Size; i++){
    yg = static_cast<int>(y + i) - center;
    if (yg < 0) yg = 0;
    if (yg >= height) yg = height - 1;
    for (unsigned int j = 0; j < Size; j++){
      xg = static_cast<int>(x + j) - center;
      if (xg < 0) xg = 0;
      if (xg >= width) xg = width - 1;
      value += ((matrix[j + i * Size] * static_cast<U32>(image[xg + yg * width])));
    }
  }
  //value >> 16;
  return static_cast<U8>(value >> 16);
}

//******************************************************
// Smear
//
// smears the value according to the filter to the image
// make sure we have 2D filterting
// and valid Size
// all pixels of image must be set to zero before
//******************************************************
void imgproc::Gaussfilter::Smear(U32* image, const U32 value, const int x, const int y, const int width, const int height){
  
  //no filtering if size == 0
  if (Size == 0){
    if (x >= 0 && x < width && y >= 0 && y < height){
      image[x + y * width] += value;
    }
    return;
  }
  else{
    const int center = Size / 2; //it is always odd...
    int xg, yg;
    for (unsigned int i = 0; i < Size; i++){
      yg = y + i - center;
      if (yg >= 0 && yg < height){
        for (unsigned int j = 0; j < Size; j++){
          xg = x + j - center;
          if (xg >= 0 && xg < width){
            image[xg + yg * width] += (value * matrix[j + i * Size]) >> 16;
          }
        }
      }
    }
  }
}

//******************************************************
// Smear
//
// smears the value according to the filter to the image
// make sure we have 2D filterting
// and valid Size
// all pixels of image must be set to zero before
//
// double version with bilinear interpolation
//******************************************************
void imgproc::Gaussfilter::Smear(U32* image, const U32 value, const double x, const double y, const int width, const int height){

  //no filtering if size == 0
  if (Size == 0){
    if (x >= 0 && x < width && y >= 0 && y < height){
      SmearAddInterpolate(image, value, x, y, width, height); 
    }
    return;
  }
  else{
    const int center = Size / 2; //it is always odd...
    int xg, yg;
    for (unsigned int i = 0; i < Size; i++){
      yg = static_cast<int>(y) + i - center;
      if (yg >= 0 && yg < height){
        for (unsigned int j = 0; j < Size; j++){
          xg = static_cast<int>(x) + j - center;
          if (xg >= 0 && xg < width){
            SmearAddInterpolate(image, (value * matrix[j + i * Size]) >> 16, x, y, width, height);
          }
        }
      }
    }
  }
}

//********************************************************
// AddInterpolateOne
//
// adds one value interpolated to the image
// TODO may be we could add this in the Interpolator-class
//********************************************************
void imgproc::Gaussfilter::SmearAddInterpolate(U32* image, const U32 value, const double x, const double y, const int width, const int height){

  const int x0 = static_cast<int>(x);
  const int y0 = static_cast<int>(y);
  const int x1 = x0 + 1;
  const int y1 = y0 + 1;

  const float dx = static_cast<float>((x - static_cast<float>(x0)));
  const float dy = static_cast<float>((y - static_cast<float>(y0)));
  const float dx1 = 1.0f - dx;
  const float dy1 = 1.0f - dy;

  if (x0 >= 0 && x0 < static_cast<int>(width) && y0 >= 0 && y0 < static_cast<int>(height)){
    const U32 v = static_cast<U32>(value * dx1 * dy1);
    image[x0 + y0 * width] += v;
  }

  if (x1 >= 0 && x1 < static_cast<int>(width) && y0 >= 0 && y0 < static_cast<int>(height)){
    const U32 v = static_cast<U32>(value * dx * dy1);
    image[x1 + y0 * width] += v;
  }

  if (x0 >= 0 && x0 < static_cast<int>(width) && y1 >= 0 && y1 < static_cast<int>(height)){
    const U32 v = static_cast<U32>(value * dx1 * dy);
    image[x0 + y1 * width] += v;
  }

  if (x1 >= 0 && x1 < static_cast<int>(width) && y1 >= 0 && y1 < static_cast<int>(height)){
    const U32 v = static_cast<U32>(value * dx * dy);
    image[x1 + y1 * width] += v;
  }

  /*const S32 dx = static_cast<S32>((x - static_cast<double>(x0)) * 0x2000);  // 0x2000 = 2^13 that should keep 
  const S32 dy = static_cast<S32>((y - static_cast<double>(y0)) * 0x2000);  // all values in the range of U32
  const S32 dx1 = 0x2000 - dx;
  const S32 dy1 = 0x2000 - dy;

  if (x0 >= 0 && x0 < static_cast<int>(width) && y0 >= 0 && y0 < static_cast<int>(height)){
    const U32 v = value * dx1 * dy1 / 0x4000000;
    image[x0 + y0 * width] += v;
  }

  if (x1 >= 0 && x1 < static_cast<int>(width) && y0 >= 0 && y0 < static_cast<int>(height)){
    const U32 v = value * dx * dy1 / 0x4000000;
    image[x1 + y0 * width] += v;
  }
    
  if (x0 >= 0 && x0 < static_cast<int>(width) && y1 >= 0 && y1 < static_cast<int>(height)){
    const U32 v = value * dx1 * dy / 0x4000000;
    image[x0 + y1 * width] += v;
  }

  if (x1 >= 0 && x1 < static_cast<int>(width) && y1 >= 0 && y1 < static_cast<int>(height)){
    const U32 v = value * dx * dy / 0x4000000;
    image[x1 + y1 * width] += v;
  }*/

}


//--------------------------------------------------------------------------------------------------------------------


//*****************************************************************************
//*                           FastImgGaussfilter                              *
//*                                                                           *
//*                   class for gaussfiltering but faster                     *
//*                                                                           *
//*****************************************************************************

//************
// constructor
//************
imgproc::FastImgGaussfilter::FastImgGaussfilter() : matrix(0), Size(0){}
 
//***********
// destructor
//***********
imgproc::FastImgGaussfilter::~FastImgGaussfilter(){
  delete[] matrix;
}

//***********************************************************************
// CreateMatrix2D
//
// if factortype == 0 then the curve not normized 
// if factortype == 1 then the curve is normized to 1
//***********************************************************************
void imgproc::FastImgGaussfilter::CreateMatrix(const double sigma, const unsigned int factortype){

  const bool normize = (factortype > 0);
  
  //Size is set after this
  //sum is slightly smaller than 1, (theoretically 1)
  double* curve = GaussCurve(sigma, normize, Size);

  //getting the _real_ sum
  double sum = 0;
  if (!normize) for (unsigned int i = 0; i < Size; i++) sum += curve[i]; 
 
  //setting te integer matrix
  //divide by the real sum
  delete[] matrix;
  matrix = new U32[Size];
  for (unsigned int i = 0; i < Size; i++){
    if (normize){
      matrix[i] = static_cast<U32>(static_cast<double>(FILTER_ONE) * curve[i] + 0.5); 
    }
    else{
      matrix[i] = static_cast<U32>(static_cast<double>(FILTER_ONE) * curve[i]/ sum + 0.5); 
    }
  }

  delete[] curve;
}
    
//filter function, returns the weighted sum of all pixels
void imgproc::FastImgGaussfilter::Filter(const U32* inimg, 
                                         U32* tempBuffer, 
                                         U32* outimg, 
                                         const int width, 
                                         const int height,
                                         const bool normize)
{

  //reset temp buffer
  for (unsigned int i = 0; i < static_cast<unsigned int>(width * height); i++) tempBuffer[i] = 0;

  const int center = Size / 2; //it is always odd...

  int g;

  //filter rows
  for (unsigned int yout = 0; yout < static_cast<unsigned int>(height); yout++){
    for (unsigned int xout = 0; xout < static_cast<unsigned int>(width); xout++){
      for (unsigned int i = 0; i < Size; i++){
        g = static_cast<int>(xout + i) - center;
        if (g < 0) g = 0;
        if (g >= width) g = width - 1;
        tempBuffer[xout + yout * width] += ((matrix[i] * inimg[xout + yout * width]) >> 16);
      }
    }
  }

  //filter columns
  for (unsigned int xout = 0; xout < static_cast<unsigned int>(width); xout++){
    for (unsigned int yout = 0; yout < static_cast<unsigned int>(height); yout++){
      for (unsigned int i = 0; i < Size; i++){
        g = static_cast<int>(yout + i) - center;
        if (g < 0) g = 0;
        if (g >= height) g = height - 1;
        outimg[xout + yout * width] += ((matrix[i] * tempBuffer[xout + yout * width]) >> 16);
      }
    }
  }

}

//filter function, returns the weighted sum of all pixels
void imgproc::FastImgGaussfilter::Filter(const U8* inimg,   //size = inwidth * inheight (whole image) 
                                         U32* tempBuffer,   //size = outwidth * outheight, make sure that it is!
                                         U8* outimg,        //size = outwidth * outheight (tile) 
                                         const unsigned int inwidth, const unsigned int inheight,   //dimension of input buffer (whole image) 
                                         const unsigned int outwidth, const unsigned int outheight, //dimension of output buffer (tile) 
                                         const int offsetX, const int offsetY,
                                         const bool normize)                      //offset output buffer to inputbuffer (tile in image) 
{
  //reset temp and out buffer
  for (unsigned int i = 0; i < outwidth * outheight; i++){ 
    tempBuffer[i] = 0;
    outimg[i] = 0;
  }
  const int center = Size / 2; //it is always odd...

  //filter rows
  // input is 8bit 
  // matrix is 16bit 0...65535
  // tempBuffer is 16bit (8bit * 16bit / 8bit)
  int xin, yin, xg, yg; 
  U32 value; 
  for (unsigned int yout = 0; yout < outheight; yout++){
    yin = static_cast<int>(yout) + offsetY;
    if (yin >= 0 && static_cast<unsigned int>(yin) < inheight){
      for (unsigned int xout = 0; xout < outwidth; xout++){
        xin = static_cast<int>(xout) + offsetX;
        if (xin >= 0 && static_cast<unsigned int>(xin) < inwidth){
          value = static_cast<U32>(inimg[xin + yin * inwidth]);  //8bit 0...255
          //process only pixel with value > 0 for faster processing
          if (value > 0){
            //smear it horizontally
            for (unsigned int i = 0; i < Size; i++){
              xg = static_cast<int>(xout + i) - center;
              if (xg >= 0 && static_cast<unsigned int>(xg) < outwidth){
                tempBuffer[xg + yout * outwidth] += (matrix[i] * value) >> 8; // currInvalue(0...255) * 256 * matrix(0....1)
              }
            }
          }
        }
      }
    }
  }
  
  //--------------------- normize rows ----------------------
  U32 maxvalue = 0;
  U32 currvalue; 
  if (normize){
    for (unsigned int y = 0; y < outheight; y++){
      for (unsigned int x = 0; x < outwidth; x++){
        currvalue = tempBuffer[x + y * outwidth];
        if (currvalue > maxvalue) maxvalue = currvalue;
      }
    }
    if (maxvalue > 0){
      for (unsigned int y = 0; y < outheight; y++){
        for (unsigned int x = 0; x < outwidth; x++){
          currvalue = static_cast<U32>(tempBuffer[x + y * outwidth]);
          tempBuffer[x + y * outwidth] = (FILTER_ONE * currvalue) / maxvalue;
        }
      }
    }
  }
  
  //filter columns
  // tempbuffer is 16bit
  // matrix is 16bit
  // value is 16bit (16bit * 16bit /16bit) during summation
  // outimg is 8bit (16bit / 8bit)
  for (unsigned int xout = 0; xout < outwidth; xout++){
    for (unsigned int yout = 0; yout < outheight; yout++){
      value = 0;
      for (unsigned int i = 0; i < Size; i++){
        if (matrix[i] > 0){
          yg = static_cast<int>(yout + i) - center;
          if (yg >= 0 && static_cast<unsigned int>(yg) < outheight){
            if (tempBuffer[xout + yg * outwidth] > 0){
              value += (matrix[i] * tempBuffer[xout + yg * outwidth]) >> 16;
            }
          }
        }
      }
      outimg[xout + yout * outwidth] = (value >> 8);
    }
  }
  
  //------------------------- normize columns -------------------------------
  maxvalue = 0;
  if (normize){
    for (unsigned int y = 0; y < static_cast<unsigned int>(outheight); y++){
      for (unsigned int x = 0; x < static_cast<unsigned int>(outwidth); x++){
        currvalue = static_cast<U32>(outimg[x + y * outwidth]);
        if (currvalue > maxvalue) maxvalue = currvalue;
      }
    }
    if (maxvalue > 0){
      for (unsigned int y = 0; y < static_cast<unsigned int>(outheight); y++){
        for (unsigned int x = 0; x < static_cast<unsigned int>(outwidth); x++){
          currvalue = static_cast<U32>(outimg[x + y * outwidth]);
          if (currvalue > 0){
            outimg[x + y * outwidth] = static_cast<U8>((255 * currvalue) / maxvalue);
          }
        }
      }
    }
  }
}
 