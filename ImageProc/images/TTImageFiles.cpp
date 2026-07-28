#include "TTImageFiles.h"

//*****************************************************************************
//*                            NumberedFilename                               *
//*                                                                           *
//*                      help class for file handling                         *
//*****************************************************************************

//*******
// setter
//*******
void image::NumberedFilename::SetPrefix(const std::string& prefix){
  Prefix = prefix;
}

//*******
// setter
//*******
void image::NumberedFilename::SetSuffix(const std::string& suffix){
  Suffix = suffix;
}

//*******
// setter
//*******
void image::NumberedFilename::SetDigits(unsigned int digits){
  Digits = digits;
}

//*******
// setter
//*******
void image::NumberedFilename::SetStart(unsigned int start){
  if (Ende > 0 && start > Ende){
    throw TTImageException("Start must not be greater than End!" + ttutil::StringUtil::uint2string(Start) + " > " + ttutil::StringUtil::uint2string(Ende));
  }
  Start = start;
}

//*******
// setter
//*******
void image::NumberedFilename::SetEnde(unsigned int ende){
  if (ende < Start){
    throw TTImageException("End must not be smaller than End!" + ttutil::StringUtil::uint2string(ende) + " < " + ttutil::StringUtil::uint2string(Start));
  }
  Ende = ende;
  Number = Ende - Start + 1;
}

//*******
// setter
//*******
void image::NumberedFilename::SetNumber(unsigned int number){
  if (Ende > 0){
    const unsigned int numberShould = Ende - Start + 1;
    if (number > numberShould){
      throw TTImageException("number " + ttutil::StringUtil::uint2string(number) + " is to big! There are only" + ttutil::StringUtil::uint2string(numberShould) + " Frames.");
    }
  }
  if (number == 0){
    throw TTImageException("number must not be null!");
  }
  Number = number;
  Ende = Start + Number - 1;
}

//*******
// getter
//*******
std::string image::NumberedFilename::GetPrefix() const{
  return Prefix;
}

//*******
// getter
//*******
std::string image::NumberedFilename::GetSuffix() const{
  return Suffix;
}

//*******
// getter
//*******
unsigned int image::NumberedFilename::GetDigits() const{
  return Digits;
}

//*******
// getter
//*******
unsigned int image::NumberedFilename::GetStart() const{
  return Start;
}

//*******
// getter
//*******
unsigned int image::NumberedFilename::GetEnde() const{
  return Ende;
}

//*******
// getter
//*******
unsigned int image::NumberedFilename::GetNumber() const{
  return Number;
}


//********************
// BuildFilename
//
// builds the filename
//********************
std::string image::NumberedFilename::BuildFilename(const unsigned int current) const{
  const std::string filename = Prefix + ttutil::StringUtil::formatRight(ttutil::StringUtil::int2string(current), Digits, '0') + Suffix;
  return filename;
}


//************************************************************************
//*                                                                      *
//*                          TTImageException                            *
//*                                                                      *
//*                  exception class for TTImage etc.                    *
//*                                                                      *
//************************************************************************

//************
// constructor
//************
image::TTImageException::TTImageException(const std::string& errormessage) : ErrorMessage(errormessage) 
{
  std::cerr << ErrorMessage << std::endl;
}

//************
// GetErrorMessage
//************
std::string image::TTImageException::GetErrorMessage() const{
  return ErrorMessage;

}


//************************************************************************
//*                                                                      *
//*                           abstract TTImage                           *
//*                                                                      *
//*                abstract image class for image handling               *
//*                                                                      *
//************************************************************************

//************
// constructor
//************
image::TTImage::TTImage() : Software("Thomas Thiele"){}

//**************************************
// GetImageProperties
//
// return image data as image properties
//**************************************
image::ImageProperties image::TTImage::GetImageProperties() const{
  return ImageProperties(Width(), Height(), Colormode(), Datatype(), ColorChannels(), AlphaChannels());
}

//**************************************************
// GetTileInfosWholeImage
//
// builds tile info where te whole image is one tile
//**************************************************
image::ImageTileInfos image::TTImage::GetTileInfosWholeImage() const{
  return GetImageProperties().GetTileInfosWholeImage();
}


//*******************************
// GetSimpleColormode
// 
// returns the simple colormode
//*******************************
image::ColormodeType image::TTImage::SimpleColormode() const{
  const ColormodeType colormode = Colormode();
  switch (colormode){
    case BIT:       return BIT;
    case GRAYSCALE: return GRAYSCALE;
    case RGB: 
    case RRGGBB:
    case BGR:
    case RGBA:
    case RRGGBBAA:  return RGB;
    case CMYK:
    case CCMMYYKK:  return CMYK;
    case LAB:       return LAB;
    case YUV:       return YUV;
    default:        return colormode;
  }
}

//***************************************
// SetBlack
//
// sets the pixel to black 
// no alpha channels, color channels only
//***************************************
void image::TTImage::SetZero(unsigned int x, unsigned int y){
  //internal processing is done with 16 bits
  //nevertheless its doesn't matter here. 0 is 0
  SetRed(x, y, 0);
  SetGreen(x, y, 0);
  SetBlue(x, y, 0);
  SetAlpha(x, y, 1);
}

//***************************************
// SetImageZero
//
// sets all pixels to black 
// no alpha channels, color channels only
//***************************************
void image::TTImage::SetImageZero(){
  for (unsigned int y = 0; y < Height(); ++y){
    for (unsigned int x = 0; x < Width(); ++x){
      SetZero(x, y);
    }
  }
}

//**********************
// SetGray
//
// setter for pixel data 
//**********************
void image::TTImage::SetGray(const unsigned int x, const unsigned int y, const t_proc g) {
  SetRed(x, y, g);
  SetGreen(x, y, g);
  SetBlue(x, y, g);
}


//**********************
// SetRGB
//
// setter for pixel data 
//**********************
void image::TTImage::SetRGB(const unsigned int x, const unsigned int y, const t_proc r, const t_proc g, const t_proc b) {
  SetRed(x, y, r);
  SetGreen(x, y, g);
  SetBlue(x, y, b);
}

//**********************
// SetColor
//
// setter for pixel data 
//**********************
void image::TTImage::SetColor(const unsigned int x, const unsigned int y, const TTColor& color) {
  //TODO color space check
  SetRed(x, y, color.GetRed());
  SetGreen(x, y, color.GetGreen());
  SetBlue(x, y, color.GetBlue());
}


//**********************
// GetColor
//
// setter for pixel data 
//**********************
image::TTColor image::TTImage::GetColor(const unsigned int x, const unsigned int y) {
  //TODO color space check
  return TTColor(GetRed(x, y), GetGreen(x, y), GetBlue(x, y));
}

//********
// AddRed
//********
void image::TTImage::AddRed(const unsigned int x, const unsigned int y, const t_proc r){
  SetRed(x, y, GetRed(x, y) + r);
}

//*********
// AddGreen
//*********
void image::TTImage::AddGreen(const unsigned int x, const unsigned int y, const t_proc g){
  SetGreen(x, y, GetGreen(x, y) + g);
}

//*********
// AddGreen
//*********
void image::TTImage::AddBlue(const unsigned int x, const unsigned int y, const t_proc b){
  SetBlue(x, y, GetBlue(x, y) + b);
}

//*********
// AddGreen
//*********
void image::TTImage::AddGray(const unsigned int x, const unsigned int y, const t_proc v){
  SetGray(x, y, GetGray(x, y) + v);
}

//*********
// AddGreen
//*********
void image::TTImage::AddAlpha(const unsigned int x, const unsigned int y, const t_proc a) {
  SetAlpha(x, y, GetAlpha(x, y) + a);
}

//************
// CopyImage
//************
void image::TTImage::CopyImage(const TTImage& other){
  const ColormodeType ownSimpleType = GetSimpleColormodeType(Colormode());
  const ColormodeType otherSimpeType = GetSimpleColormodeType(other.Colormode());

  //TODO: add some validations here, when we need them ...

  unsigned int colorCase = 0; 
  if      (ownSimpleType == RGB && otherSimpeType == RGB) colorCase = 0; //RGB -> RGB
  else if (ownSimpleType == GRAYSCALE && otherSimpeType == RGB) colorCase = 1; //RGB -> GRAY
  else if (ownSimpleType == RGB && otherSimpeType == GRAYSCALE) colorCase = 2; //GRAY -> RGB
  else if (ownSimpleType == GRAYSCALE && otherSimpeType == GRAYSCALE) colorCase = 3; //GRAY -> GRAY

  unsigned int alphaCase = 0;
  if      (AlphaChannels() == 0) alphaCase = 0; //no Alpha Channel
  else if (other.AlphaChannels() == 0) alphaCase = 1; //source has no alpha channel but we -> all opaque
  else alphaCase = 3; //copy alpha channel, too (at least the first ...)

  const unsigned int w = Width();
  const unsigned int h = Height();
  const unsigned int wo = other.Width();
  const unsigned int ho = other.Height();

  //render own image, fill missing with black
  for (unsigned int y = 0; y < h; ++y){
    for (unsigned int x = 0; x < w; ++x){
      switch (colorCase){
        case 0: //RGB -> RGB
          if (x < wo && y < ho){
            SetRed(x, y, other.GetRed(x, y));
            SetGreen(x, y, other.GetGreen(x, y));
            SetBlue(x, y, other.GetBlue(x, y));
          }
          else{
            SetZero(x, y);
          }
          break;
        case 1: //RGB -> GRAY
          if (x < wo && y < ho){
            SetGray(x, y, other.GetGray(x, y));
          }
          else{
            SetGray(x, y, 0.0f);
          }
          break;
        case 2: //GRAY -> RGB
          if (x < wo && y < ho){
            SetGray(x, y, other.GetGray(x, y));
          }
          else{
            SetZero(x, y);
          }
          break;
        case 3: //GRAY -> GRAY
          if (x < wo && y < ho){
            SetGray(x, y, other.GetGray(x, y));
          }
          else{
            SetGray(x, y, 0.0f);
          }
          break;
      }
      switch (alphaCase){
        case 1: 
          SetAlpha(x, y, 1.0f);
          break;
        case 3: 
          SetAlpha(x, y, other.GetAlpha(x, y));
          break;
      }
    }
  }
}


//******************
// CopyAlphaFromGray
//******************
void image::TTImage::CopyAlphaFromGray(const TTImage& other){
  if (AlphaChannels() == 0){
    throw TTImageException("Image has no alpha channel!");
  }
  const unsigned int w = Width();
  const unsigned int h = Height();
  const unsigned int wo = other.Width();
  const unsigned int ho = other.Height();

  //render own image, fill missing with tranparent
  for (unsigned int y = 0; y < h; ++y){
    for (unsigned int x = 0; x < w; ++x){
      if (x < wo && y < ho){
        SetAlpha(x, y, other.GetGray(x, y));
      }
      else{
        SetAlpha(x, y, 0.0f);
      }
    }
  }
}

//*************
// SetSoftware
//*************
void image::TTImage::SetSoftware(const std::string software){
  Software = software;
}

//*************
// GetSoftware
//*************
std::string image::TTImage::GetSoftware(){
  return Software;
}

//*************************************************
// GetMemoryUsage
//
// gets a aproximate size in memory in MB if loaded
//*************************************************
unsigned int image::TTImage::GetMemoryUsage(){
  const unsigned int width = Width();
  const unsigned int height = Height();
  const unsigned int channels = WholeChannels();
  const unsigned int bytesPerValue = GetBytesByDatatyp(Datatype());
  
  const unsigned int bytes = width * height * channels * bytesPerValue;
  const unsigned int mbfactor = 1024 * 1024;

  const unsigned int usedMegaBytes = bytes / mbfactor + (bytes % mbfactor > 0 ? 1 : 0);
  return usedMegaBytes;
}

//*********
// IsValid 
//*********
bool image::TTImage::IsValid() const {
  return false;
}

//************************************************************************
//*                            TTMemoryImage                             *           
//*                                                                      *
//* abstract image class for images that are complete strored in memory  *
//* with direct access to all pixels                                     *
//************************************************************************

//************
// constructor
//************
image::TTMemoryImage::TTMemoryImage() 
                     : TTImage()
{}

//***********
// destructor
//***********
image::TTMemoryImage::~TTMemoryImage(){
  delete[] imageData;
}

//*************************************
// ImageData
// 
// returns the pointer to raw imagedata
//*************************************
void* image::TTMemoryImage::ImageData(){ return imageData; }
    
//*********
// IsValid 
//*********
bool image::TTMemoryImage::IsValid() const {
  return imageData != nullptr;
}

//************************************************************************
//*                                                                      *
//*                         abstract TTFileImage                         *
//*                                                                      *
//*                  base class for image file handling                  *
//*                                                                      *
//************************************************************************
 
//************
// constructor
//************
image::TTFileImage::TTFileImage() :  file(NULL){}

//************
// destructor
//************
image::TTFileImage::~TTFileImage() { 
  Close(); 
}

//*****************************************
// closes the image and frees all resources
//*****************************************
void image::TTFileImage::Close(){
  if (file){
    file->close();
    delete file;
  }
}

//************************************************************************
//*                                                                      *
//*                       abstract TTByteOrderFile                       *
//*                                                                      *
//*             abstract class capsulating the byte order issus          *
//*                                                                      *
//************************************************************************

//************
// constructor
//************
image::TTByteOrderFile::TTByteOrderFile() : ByteOrderIntel(true) {}

//************************************
// ReorderShort   ????? is this correct ????
//
// switches a short value if necassary
//************************************
U16 image::TTByteOrderFile::Reorder(const U16 v) const{
  return ByteOrderIntel ? SwapBytes(v) : v;
}

//*****************************************************************
// Reorder
// 
// switches a int value if necassary   ????? is this correct ????
//*****************************************************************
U32 image::TTByteOrderFile::Reorder(const U32 v) const{
  return ByteOrderIntel ? SwapBytes(v) : v;
}


//**************
// SwapBytes U16
//**************
U16 image::TTByteOrderFile::SwapBytes(const U16 v) const{
  return (((v & 0x00FF) << 8) | ((v & 0xFF00) >> 8));
}

//**************
// SwapBytes U32
//**************
U32 image::TTByteOrderFile::SwapBytes(const U32 v) const{
  return (((v & 0x000000FF) << 24) | ((v & 0x0000FF00) << 8)) | ((v & 0x00FF0000) >> 8) | ((v & 0xFF000000) >> 24);
}


//*********
// ReadU8
//*********
U8 image::TTByteOrderFile::ReadU8(){
  U16 result = 0;
  char c;
  file->read(&c, 1);
  if (file->gcount() != 1){
    std::cout << "ReadU8: error while reading!" << std::endl;
  }
  return c;
}

//*********
// ReadU16
//*********
U16 image::TTByteOrderFile::ReadU16(){
  U16 result = 0;
  char buffer[2];
  file->read(buffer, 2);
  if (file->gcount() != 2){
    std::cout << "ReadU16: error while reading!" << std::endl;
  }
  if (ByteOrderIntel){
    result = buffer[0] & 0x00FF;
    result |= ((buffer[1] & 0x00FF) << 8);
  }
  else
  {
    result = buffer[1] & 0x00FF;
    result |= ((buffer[0] & 0x00FF) << 8);
  }

  return (short)result;
}

//*********
// ReadU32
//*********
U32 image::TTByteOrderFile::ReadU32(){
  U32 result = 0;
  char buffer[4];
  file->read(buffer, 4);
  if (file->gcount() != 4){
    std::cout << "ReadU32: error while reading!" << std::endl;
  }
  if (ByteOrderIntel){
    result += buffer[0] & 0x00FF;
    result += ((buffer[1] & 0x00FF) << 8);
    result += ((buffer[2] & 0x00FF) << 16);
    result += ((buffer[3] & 0x00FF) << 24);
  }
  else{
    result += buffer[3] & 0x00FF;
    result += ((buffer[2] & 0x00FF) << 8);
    result += ((buffer[1] & 0x00FF) << 16);
    result += ((buffer[0] & 0x00FF) << 24);
  }
  return result;
}

//***********
// ReadString
//***********
std::string image::TTByteOrderFile::ReadString(const unsigned int length){
  std::string s;
  char* const buffer = new char[length + 1];
  buffer[length] = 0;
  file->read(buffer, length);
  if (file->gcount() != length){
    std::cout << "ReadString: error while reading!" << std::endl;
  }
  s = buffer;
  delete[] buffer;
  return s;
}
                 
//********
// WriteU8
//********
void image::TTByteOrderFile::WriteU8(const U8 v){
  file->write(reinterpret_cast<const char*>(&v), 1);
}

//*********
// WriteU16
//*********
void image::TTByteOrderFile::WriteU16(U16 v){
  if (!ByteOrderIntel) v = SwapBytes(v);
  file->write(reinterpret_cast<const char *>(&v), 2);
}

//*********
// WriteU32
//*********
void image::TTByteOrderFile::WriteU32(U32 v){
  if (!ByteOrderIntel) v = SwapBytes(v);
  file->write(reinterpret_cast<const char *>(&v), 4);
}

//************
// WriteString
//************
void image::TTByteOrderFile::WriteString(const std::string& s){
  file->write(s.c_str(), s.length());
}


