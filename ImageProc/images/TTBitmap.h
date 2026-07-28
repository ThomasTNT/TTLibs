#pragma once

#include <iostream>
#include <fstream>

#include "TTImageFiles.h"
#include "stringutil.h"

namespace image{


//******************************************************************************
//*                                 TTBitmap                                   *
//*                                                                            *
//*                  Bitmap class only for 24bit uncompressed .bmp             *
//******************************************************************************


const U16 MagicNumberBmp = 0x4d42;

typedef struct{ 
	U16 ImageFileType;
	U32 FileSize;
	U16 Reserved1;
	U16 Reserved2;
	U32 ImageDataOffset;
} WIN3XHEAD;   

typedef	struct{
	U32 HeaderSize;
	U32 ImageWidth;
	U32 ImageHeight;
	U16 NumberOfImagePlanes;
	U16 BitsPerPixel;
	U32 CompressionMethod;
	U32 SizeOfBitmap;
	U32 HorzResolution;
	U32 VertResolution;
	U32 NumColorsUsed;
	U32 NumSignificantColors;
} WIN3XINFOHEAD;

typedef struct{
	WIN3XHEAD		Win3xBitmapHead;
	WIN3XINFOHEAD	Win3xBitmapInfoHead;

} BMPHeader;

#pragma warning( disable : 4250 )
class TTBitmap : public virtual TTMemoryImage, public virtual TTFileImage {
	
  public:

    //constructor
		TTBitmap();

    //destructor
		virtual ~TTBitmap();


    //----------------- methods from TTFileImage -----------

		//opens a image from file
    virtual bool Open(const std::string& filename);

    //opens a image from file but does not load image data
    virtual bool OpenFileInfo(const std::string& filename);

    //saves the image
    virtual bool Save(const std::string& filename);

    //closes the image and frees all resources
    virtual void Close();

    //resets the filepointer
		virtual bool ResetFilePointer();

    //--------------- methods from TTMemoryImage -----------

    //------------------ methods from TTImage --------------

    //creates the image in memory
    //with default properties
    virtual bool Create(unsigned int w, unsigned int h);

    //creates the image in memory
    //with given properties
    virtual bool Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype);

    //returns the width
    virtual unsigned int Width() const;

    //returns the height
    virtual unsigned int Height() const;

    //returns the number of whole channels (== color channel + alpha channel)
    virtual unsigned int WholeChannels() const;

    //returns the number of color channels
    virtual unsigned int ColorChannels() const;

    //returns the number of alpha channels
    virtual unsigned int AlphaChannels() const;

    //return the colormode
    virtual ColormodeType Colormode() const;

    //return the ColorSpace
    virtual ColorSpaceType ColorSpace() const;

    //return the colormodebit depth per channel
    virtual Datatyp Datatype() const;

    //return interlaced or not 
    virtual bool Interlaced() const;
   
    //getter for pixel data
    virtual t_proc GetRed(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetGreen(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetBlue(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetGray(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetAlpha(const unsigned int x, const unsigned int y) const;

    //setter for pixel data
    virtual void SetZero(unsigned int x, unsigned int y);

    virtual void SetRed(const unsigned int x, const unsigned int y, const t_proc r);
    virtual void SetGreen(const unsigned int x, const unsigned int y, const t_proc g);
    virtual void SetBlue(const unsigned int x, const unsigned int y, const t_proc b);
    virtual void SetGray(const unsigned int x, const unsigned int y, const t_proc g);
    virtual void SetAlpha(const unsigned int x, const unsigned int y, const t_proc a);

    void SetPixelU8Direct(unsigned int x, unsigned int y, const U8 r, const U8 g, const U8 b);

    //for information
    std::string GetImageInfos();

  private:

    //opens a image from file
    bool OpenInternal(const std::string& filename, const bool readImageData);

    void CopyBufferIntoHeader(const U8 * const buffer);
    void CopyHeaderIntoBuffer(U8 * const buffer);

    U16 GetU16(const U8 * const buffer, int &index);
    U32 GetU32(const U8 * const buffer, int &index);
    void SetU16(const U16 value, U8 * const buffer, int &index);
    void SetU32(const U32 value, U8 * const buffer, int &index);

    //image properties
    unsigned int width, height;

    //private fields 
    BMPHeader header;
    U8* ImageDataU8;

    

};

} //end of namespace images
