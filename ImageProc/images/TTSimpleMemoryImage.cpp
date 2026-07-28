#include "TTSimpleMemoryImage.h"

//************************************************************************
//*                         TTSimpleMemoryImage                           *           
//*                                                                      *
//*              a simple implementation of a memory image               *
//*                                                                      *
//*                      it uses alway t_proc as data type               *
//************************************************************************


//************
// constructor
//************
image::TTSimpleMemoryImage::TTSimpleMemoryImage() : TTMemoryImage()                                               
{
}

//***********
// destructor
//***********
image::TTSimpleMemoryImage::~TTSimpleMemoryImage(){
  Close();
}

//******************
// Width
//
// returns the width
//******************
unsigned int  image::TTSimpleMemoryImage::Width() const { return width; }

//*******************
// Height
//
// returns the height
//*******************
unsigned int  image::TTSimpleMemoryImage::Height() const { return height; }

//***********************************************************************
// WholeChannels
//
// returns the number of whole channels (== color channel + alpha channel)
//************************************************************************
unsigned int image::TTSimpleMemoryImage::WholeChannels() const {
  return alphaChannels + colorChannels;
}

//*************************************
// ColorChannels
//
// returns the number of color channels
//*************************************
unsigned int image::TTSimpleMemoryImage::ColorChannels() const {
  return colorChannels;
}

//*************************************
// AlphaChannels
//
// returns the number of alpha channels
//*************************************
unsigned int image::TTSimpleMemoryImage::AlphaChannels() const { return alphaChannels; }

//*********************
// Colormode
//
// return the colormode
//*********************
image::ColormodeType image::TTSimpleMemoryImage::Colormode() const { return colormode; }

//*********************
// ColorSpace
//
// return the ColorSpace
//*********************
image::ColorSpaceType image::TTSimpleMemoryImage::ColorSpace() const { return COLORSPACE_UNKNOWN; }

//******************************************
// Datatype
//
// return the colormodebit depth per channel
//******************************************
image::Datatyp image::TTSimpleMemoryImage::Datatype() const { return FLOAT32; }

//*************************
// Interlaced
//
// return interlaced or not 
//*************************
bool image::TTSimpleMemoryImage::Interlaced() const { return true; };

//****************************
// create
// 
// creates the image in memory
//****************************
bool image::TTSimpleMemoryImage::Create(unsigned int w, unsigned int h){
  return Create(w, h, RGB, FLOAT32);
}

// creates the image in memory
// with given properties
bool image::TTSimpleMemoryImage::Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype){

  //validate first
  if (colormode != GRAYSCALE && colormode != RGB && colormode != RGBA){
    throw TTImageException("Invalid colormode. Only GRAYSCALE, RGB or RGBA allowed!");
  }

  width = w;
  height = h;

  this->colormode = colormode;
  simpleColormode = GetSimpleColormodeType(colormode);
  colorChannels = GetChannelsByColormodeType(colormode);
  alphaChannels = GetAlphaChannelsByColormodeType(colormode);

  //create image memory 
  const unsigned int size = width * height * (colorChannels + alphaChannels);
  ImageDataProc = new t_proc[size];
  imageData = static_cast<void*>(ImageDataProc);

  return true;

}

//*****************************************
// Close
//
// closes the image and frees all resources
//*****************************************
void image::TTSimpleMemoryImage::Close(){
  delete[] ImageDataProc;
  ImageDataProc = nullptr;
  imageData = nullptr;
}

//**********************
// GetRed
//
// getter for pixel data 
//**********************
image::t_proc image::TTSimpleMemoryImage::GetRed(const unsigned int x, const unsigned int y) const{
  return ImageDataProc[((width * y + x) * (colorChannels + alphaChannels))];
}

//**********************
// GetGreen
//
// getter for pixel data 
//**********************
image::t_proc image::TTSimpleMemoryImage::GetGreen(const unsigned int x, const unsigned int y) const{
  return ImageDataProc[((width * y + x) * (colorChannels + alphaChannels)) + 1];
}

//**********************
// GetBlue
//
// getter for pixel data 
//**********************
image::t_proc image::TTSimpleMemoryImage::GetBlue(const unsigned int x, const unsigned int y) const{
  return ImageDataProc[((width * y + x) * (colorChannels + alphaChannels)) + 2];
}

//**********************
// SetGray
//
// setter for pixel data 
//**********************
image::t_proc image::TTSimpleMemoryImage::GetGray(const unsigned int x, const unsigned int y) const{
  return ImageDataProc[(width * y + x)];
}

//**********************
// GetAlpha
//
// getter for pixel data 
//**********************
image::t_proc image::TTSimpleMemoryImage::GetAlpha(const unsigned int x, const unsigned int y) const{
  return ImageDataProc[((width * y + x) * (colorChannels + alphaChannels)) + colorChannels];
}

//**********************
// SetZero
//
// set all to 0 (black)
//**********************
void image::TTSimpleMemoryImage::SetZero(unsigned int x, unsigned int y){
  if (this->simpleColormode == RGB){
    SetRed(x, y, 0);
    SetGreen(x, y, 0);
    SetBlue(x, y, 0);
  }
  else{
    SetGray(x, y, 0);
  }
}

//**********************
// SetRed
//
// setter for pixel data 
//**********************
void image::TTSimpleMemoryImage::SetRed(const unsigned int x, const unsigned int y, const t_proc r){
  ImageDataProc[((width * y + x) * (colorChannels + alphaChannels))] = r;
}

//**********************
// SetGreen
//
// setter for pixel data 
//**********************
void image::TTSimpleMemoryImage::SetGreen(const unsigned int x, const unsigned int y, const t_proc g){
  ImageDataProc[((width * y + x) * (colorChannels + alphaChannels)) + 1] = g;
}

//**********************
// SetBlue
//
// setter for pixel data 
//**********************
void image::TTSimpleMemoryImage::SetBlue(const unsigned int x, const unsigned int y, const t_proc b){
  ImageDataProc[((width * y + x) * (colorChannels + alphaChannels)) + 2] = b;
}


//**********************
// SetGray
//
// setter for pixel data 
//**********************
void image::TTSimpleMemoryImage::SetGray(const unsigned int x, const unsigned int y, const t_proc g){
  ImageDataProc[(width * y + x)] = g;
}


//**********************
// SetAlpha
//
// setter for pixel data 
//**********************
void image::TTSimpleMemoryImage::SetAlpha(const unsigned int x, const unsigned int y, const t_proc a){
  ImageDataProc[((width * y + x) * (colorChannels + alphaChannels)) + colorChannels] = a;
}

//****************
// GetImageInfos
//
// for information
//****************
std::string image::TTSimpleMemoryImage::GetImageInfos(){
  std::string s;
  s += "TTSimpleMemoryImage: " + ttutil::StringUtil::uint2string(Width()) + " x " + ttutil::StringUtil::uint2string(Height()) + "\n";
  s += "color channels:  " + ttutil::StringUtil::uint2string(ColorChannels()) + "\n";
  s += "alpha channels:  " + ttutil::StringUtil::uint2string(AlphaChannels()) + "\n";
  s += "color type: " + GetColormodeTypeName(Colormode()) + "\n";
  s += "date type: " + GetDatatypName(Datatype()) + "\n";
  return s;
}

