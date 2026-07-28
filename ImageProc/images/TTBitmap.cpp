#include "TTBitmap.h"


//******************************************************************************
//*                                 TTBitmap                                   *
//*                                                                            *
//*                  Bitmap class only for 24bit uncompressed .bmp             *
//******************************************************************************

//************
// constructor
//************
image::TTBitmap::TTBitmap() : TTMemoryImage(), ImageDataU8(NULL), width(0), height(0) {}

//***********
// destructor
//***********
image::TTBitmap::~TTBitmap(){ 
  Close(); 
}

//******************
// Width
//
// returns the width
//******************
unsigned int  image::TTBitmap::Width() const { return width; }

//*******************
// Height
//
// returns the height
//*******************
unsigned int  image::TTBitmap::Height() const { return height; }

//***********************************************************************
// WholeChannels
//
// returns the number of whole channels (== color channel + alpha channel)
//************************************************************************
inline unsigned int image::TTBitmap::WholeChannels() const { return 3; };

//*************************************
// ColorChannels
//
// returns the number of color channels
//*************************************
inline unsigned int image::TTBitmap::ColorChannels() const { return 3; };

//*************************************
// AlphaChannels
//
// returns the number of alpha channels
//*************************************
inline unsigned int image::TTBitmap::AlphaChannels() const { return 0; };

//*********************
// Colormode
//
// return the colormode
//*********************
inline image::ColormodeType image::TTBitmap::Colormode() const { return BGR; };

//*********************
// ColorSpace
//
// return the ColorSpace
//*********************
inline image::ColorSpaceType image::TTBitmap::ColorSpace() const { return COLORSPACE_UNKNOWN; };

//******************************************
// Datatype
//
// return the colormodebit depth per channel
//******************************************
image::Datatyp image::TTBitmap::Datatype() const { return INT8; }

//*************************
// Interlaced
//
// return interlaced or not 
//*************************
inline bool image::TTBitmap::Interlaced() const { return true; };

//************************
// open
//
// opens a image from file
//************************
bool image::TTBitmap::Open(const std::string& filename){
  return OpenInternal(filename, true);
}

//************************
// open
//
// opens a image from file
//************************
bool image::TTBitmap::OpenFileInfo(const std::string& filename){
  return OpenInternal(filename, false);
}

//************************
// open
//
// opens a image from file
//************************
bool image::TTBitmap::OpenInternal(const std::string& filename, const bool readImageData){

  //open file
  file = new std::fstream();
  file->open(filename.c_str(), std::ios::in|std::ios::binary);
  if (!file->good()){
    Close();
		return false;
	}

  //read BMP-header
  //----------------------------------------------
  /*file->read((char *)&header, sizeof(header));
  if (file->gcount() != sizeof(header)){
    Close();
		return false;
	}*/
  //----------------------------------------------
  unsigned char buffer[sizeof(header)];
  file->read((char *)&buffer, sizeof(header));
	//file->read((char *)&header, sizeof(header));
  if (file->gcount() != sizeof(header)) {
   	Close();
		return false;
	}
	CopyBufferIntoHeader(buffer);
  //----------------------------------------------

  //check 24bit uncompressed windows bitmap
	if (header.Win3xBitmapHead.ImageFileType != MagicNumberBmp){
		Close();
		return false;
	}
	if (  (header.Win3xBitmapInfoHead.NumberOfImagePlanes != 1)
		  ||(header.Win3xBitmapInfoHead.BitsPerPixel != 24))
  {
		Close();
		return false;
	}
	if (header.Win3xBitmapInfoHead.CompressionMethod != 0){
		Close();
		return false;
	}

  width  = header.Win3xBitmapInfoHead.ImageWidth;
	height = header.Win3xBitmapInfoHead.ImageHeight;
  
  //read image data
  file->seekg(header.Win3xBitmapHead.ImageDataOffset, std::ios::beg);
	if (!file->good()){
		Close();
		return false;
	}

  if (readImageData){
    int size = width * height * 3;
    ImageDataU8 = new U8[size];
    if (!ImageDataU8){
      Close();
      return false;
    }
    file->read((char *)ImageDataU8, size);
    /*if (file->gcount() != size){
      Close();
      return false;
      }*/

    imageData = static_cast<void*>(ImageDataU8);
  }
  return true;
}

//****************************
// create
// 
// creates the image in memory
//****************************
bool image::TTBitmap::Create(unsigned int w, unsigned int h){
  return Create(w, h, BGR, INT8);
}

// creates the image in memory
// with given properties
bool image::TTBitmap::Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype){

  width = w;
  height = h;

  //create header
  header.Win3xBitmapHead.ImageFileType = MagicNumberBmp;
  header.Win3xBitmapHead.Reserved1 = 0;
  header.Win3xBitmapHead.Reserved2 = 0;
  header.Win3xBitmapHead.ImageDataOffset = sizeof(header);
  header.Win3xBitmapInfoHead.HeaderSize = 40;
  header.Win3xBitmapInfoHead.ImageWidth = width;
  header.Win3xBitmapInfoHead.ImageHeight = height;
  header.Win3xBitmapInfoHead.NumberOfImagePlanes = 1;
  header.Win3xBitmapInfoHead.BitsPerPixel = 24;
  header.Win3xBitmapInfoHead.CompressionMethod = 0;
  header.Win3xBitmapInfoHead.SizeOfBitmap = width * height
    * (header.Win3xBitmapInfoHead.BitsPerPixel / 8)
    * header.Win3xBitmapInfoHead.NumberOfImagePlanes;

  header.Win3xBitmapInfoHead.NumColorsUsed = 0;
  header.Win3xBitmapInfoHead.NumSignificantColors = 0xeedd;

  header.Win3xBitmapHead.FileSize = header.Win3xBitmapInfoHead.SizeOfBitmap
    + header.Win3xBitmapInfoHead.HeaderSize;

  header.Win3xBitmapInfoHead.HorzResolution = 2834;//todo!!!
  header.Win3xBitmapInfoHead.VertResolution = 2834;//todo!!!

  //create image memory 
  const unsigned int size = width * height * 3;
  ImageDataU8 = new U8[size];
  imageData = static_cast<void*>(ImageDataU8);

  return true;
}

//****************
// save
//
// saves the image
//****************
bool image::TTBitmap::Save(const std::string& filename){
  delete file;
  file = new std::fstream();
  file->open(filename.c_str(), std::ios::out|std::ios::binary);
  if (!file->good()){
    Close();
    throw TTImageException("Could not open file " + filename + "!");
		return false;
	}
	
	//write header 
	unsigned char buffer[sizeof(header)];
	CopyHeaderIntoBuffer(buffer);
  file->write((char *)&buffer, sizeof(header));
  std::cout << "sizeof(header): " << sizeof(header) << std::endl;
  
	//write data
	int size = width * height * 3; 
  file->write((char *)ImageDataU8, size);
	
	delete file;
  file = NULL;
  std::cout << "image saved: " << filename << std::endl;
  return true;
}


//*****************************************
// Close
//
// closes the image and frees all resources
//*****************************************
void image::TTBitmap::Close(){
  delete[] ImageDataU8;
  ImageDataU8 = NULL;
  imageData = NULL;  
  delete file;
  file = NULL;
}

//***********************
// ResetFilePointer
//
// resets the filepointes
//***********************
bool image::TTBitmap::ResetFilePointer(){
  if (!file) return false;
  file->seekg(header.Win3xBitmapHead.ImageDataOffset, std::ios::beg);
  if (!file->good()) return false;
  return true;
}

//*********************
// CopyBufferIntoHeader
//*********************
void image::TTBitmap::CopyBufferIntoHeader(const U8 * const buffer){
	if (buffer){
		int index = 0;
		header.Win3xBitmapHead.ImageFileType=GetU16(buffer, index);
		header.Win3xBitmapHead.FileSize=GetU32(buffer, index);
		header.Win3xBitmapHead.Reserved1=GetU16(buffer, index);
		header.Win3xBitmapHead.Reserved2=GetU16(buffer, index);
		header.Win3xBitmapHead.ImageDataOffset=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.HeaderSize=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.ImageWidth=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.ImageHeight=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.NumberOfImagePlanes=GetU16(buffer, index);
		header.Win3xBitmapInfoHead.BitsPerPixel=GetU16(buffer, index);
		header.Win3xBitmapInfoHead.CompressionMethod=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.SizeOfBitmap=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.HorzResolution=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.VertResolution=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.NumColorsUsed=GetU32(buffer, index);
		header.Win3xBitmapInfoHead.NumSignificantColors=GetU32(buffer, index);
	}
}

//*********************
// CopyHeaderIntoBuffer
//*********************
void image::TTBitmap::CopyHeaderIntoBuffer(U8 * const buffer){
	if (buffer){
		int index = 0;
		SetU16( header.Win3xBitmapHead.ImageFileType, buffer, index);
		SetU32( header.Win3xBitmapHead.FileSize, buffer, index);
		SetU16( header.Win3xBitmapHead.Reserved1, buffer, index);
		SetU16( header.Win3xBitmapHead.Reserved2, buffer, index);
		SetU32( header.Win3xBitmapHead.ImageDataOffset, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.HeaderSize, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.ImageWidth, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.ImageHeight, buffer, index);
		SetU16( header.Win3xBitmapInfoHead.NumberOfImagePlanes, buffer, index);
		SetU16( header.Win3xBitmapInfoHead.BitsPerPixel, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.CompressionMethod, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.SizeOfBitmap, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.HorzResolution, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.VertResolution, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.NumColorsUsed, buffer, index);
		SetU32( header.Win3xBitmapInfoHead.NumSignificantColors, buffer, index);
	}
}

//************
// HELP SetU16
//************
U16 image::TTBitmap::GetU16(const U8 * const buffer, int &index){
	U16 ret = 0;
	if (buffer){
		ret = ((U16)buffer[index]) + ((U16)buffer[index+1] << 8);
		index += 2;
	}
	return ret;
}

//************
// HELP SetU32
//************
U32 image::TTBitmap::GetU32(const U8 * const buffer, int &index){
	U32 ret = 0;
	if (buffer){
		ret = ((U32)buffer[index])+((U32)buffer[index+1]<<8) + ((U32)buffer[index+2] << 16) + ((U32)buffer[index+3] << 24);
		index += 4;
	}
	return ret;
}

//************
// HELP PutU16
//************
void image::TTBitmap::SetU16(const U16 value, U8 * const buffer, int &index){
	if (buffer){
		buffer[index]   = value & 0x00FF;
		buffer[index+1] = (value & 0xFF00) >> 8;
		index += 2;
	}
}

//************
// HELP PutU32
//************
void image::TTBitmap::SetU32(const U32 value, U8 * const buffer, int &index){
	if (buffer){
		buffer[index]   = value & 0x000000FF;
		buffer[index+1] = (value & 0x0000FF00) >> 8;
		buffer[index+2] = (value & 0x00FF0000) >> 16;
		buffer[index+3] = (value & 0xFF000000) >> 24;
		index += 4;
	}
}

//**********************
// GetRed
//
// getter for pixel data 
//**********************
image::t_proc image::TTBitmap::GetRed(const unsigned int x, const unsigned int y) const{
  return ImageDataU8 ? TTColorConv::ProcFromU8(ImageDataU8[((width * y + x) * 3 + 2)]) : 0;
}

//**********************
// GetGreen
//
// getter for pixel data 
//**********************
image::t_proc image::TTBitmap::GetGreen(const unsigned int x, const unsigned int y) const{
  return ImageDataU8 ? TTColorConv::ProcFromU8(ImageDataU8[((width * y + x) * 3 + 1)]) : 0;
}

//**********************
// GetBlue
//
// getter for pixel data 
//**********************
image::t_proc image::TTBitmap::GetBlue(const unsigned int x, const unsigned int y) const{
  return ImageDataU8 ? TTColorConv::ProcFromU8(ImageDataU8[((width * y + x) * 3 + 0)]) : 0;
}

//**********************
// SetGray
//
// setter for pixel data 
//**********************
image::t_proc image::TTBitmap::GetGray(const unsigned int x, const unsigned int y) const{
  return TTColorConv::GetSimpleGrayByRGB(GetRed(x, y), GetGreen(x, y), GetBlue(x, y));
}

//**********************
// GetAlpha
//
// getter for pixel data 
//**********************
image::t_proc image::TTBitmap::GetAlpha(const unsigned int x, const unsigned int y) const{
  return 0;
}

//**********************
// SetZero
//
// set all to 0 (black)
//**********************
void image::TTBitmap::SetZero(unsigned int x, unsigned int y){
  ImageDataU8[((width * y + x) * 3 + 2)] = 0;
  ImageDataU8[((width * y + x) * 3 + 1)] = 0;
  ImageDataU8[((width * y + x) * 3 + 0)] = 0;
}

//**********************
// SetRed
//
// setter for pixel data 
//**********************
void image::TTBitmap::SetRed(const unsigned int x, const unsigned int y, const t_proc r){
  ImageDataU8[((width * y + x) * 3 + 2)] = TTColorConv::U8FromProc(r);
}

//**********************
// SetGreen
//
// setter for pixel data 
//**********************
void image::TTBitmap::SetGreen(const unsigned int x, const unsigned int y, const t_proc g){
  ImageDataU8[((width * y + x) * 3 + 1)] = TTColorConv::U8FromProc(g);
}

//**********************
// SetBlue
//
// setter for pixel data 
//**********************
void image::TTBitmap::SetBlue(const unsigned int x, const unsigned int y, const t_proc b){
  ImageDataU8[((width * y + x) * 3 + 0)] = TTColorConv::U8FromProc(b);
}


//**********************
// SetGray
//
// setter for pixel data 
//**********************
void image::TTBitmap::SetGray(const unsigned int x, const unsigned int y, const t_proc g){
  //we have only RGB images at the moment
  SetRed(x, y, g);
  SetGreen(x, y, g);
  SetBlue(x, y, g);
}

//**********************
// SetAlpha
//
// setter for pixel data 
//**********************
void image::TTBitmap::SetAlpha(const unsigned int x, const unsigned int y, const t_proc a){
  //not supported!
}

//**************************************
// SetPixelU8Direct
//
// direct data access to the 8bit values
//**************************************
void image::TTBitmap::SetPixelU8Direct(unsigned int x, unsigned int y, const U8 r, const U8 g, const U8 b){
  if (ImageDataU8){ 
    ImageDataU8[((width * y + x) * 3 + 2)] = r;
    ImageDataU8[((width * y + x) * 3 + 1)] = g;
    ImageDataU8[((width * y + x) * 3 + 0)] = b;
  }
}

//****************
// GetImageInfos
//
// for information
//****************
std::string image::TTBitmap::GetImageInfos(){
  std::string s;
  s += "TTBitmap: " + ttutil::StringUtil::uint2string(width) + " x " + ttutil::StringUtil::uint2string(height) + "\n";
  s += "color channels:  " + ttutil::StringUtil::uint2string(ColorChannels()) + "\n";
  s += "alpha channels:  " + ttutil::StringUtil::uint2string(AlphaChannels()) + "\n";
  s += "color type: " + GetColormodeTypeName(Colormode()) + "\n";
  s += "date type: " + GetDatatypName(Datatype()) + "\n";
  return s;
}