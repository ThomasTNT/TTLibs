#pragma once

#include <string>
#include <fstream>
#include "TTColor.h"
#include "stringutil.h"
#include "ImageInfoStructs.h"

namespace image{

//*****************************************************************************
//*                            NumberedFilename                               *
//*                                                                           *
//*                      help class for file handling                         *
//*****************************************************************************
class NumberedFilename{
  public:

    //setter
    void SetPrefix(const std::string& prefix);
    void SetSuffix(const std::string& suffix);
    void SetDigits(unsigned int digits);
    void SetStart(unsigned int start);
    void SetEnde(unsigned int ende);
    void SetNumber(unsigned int number);

    //getter
    std::string GetPrefix() const;
    std::string GetSuffix() const;
    unsigned int GetDigits() const;
    unsigned int GetStart() const;
    unsigned int GetEnde() const;
    unsigned int GetNumber() const;

    //builds the filename
    std::string BuildFilename(const unsigned int current) const;

  private:
    std::string Prefix;
    std::string Suffix;
    unsigned int Digits = 0;
    unsigned int Start = 0;
    unsigned int Ende = 0;
    unsigned int Number = 0;
};

//************************************************************************
//*                                                                      *
//*                          TTImageException                            *
//*                                                                      *
//*                  exception class for TTImage etc.                    *
//*                                                                      *
//************************************************************************
class TTImageException{

  public: 
    TTImageException(const std::string& errormessage);

    std::string GetErrorMessage() const;

  protected:
    std::string ErrorMessage;

};


//************************************************************************
//*                                                                      *
//*                           abstract TTImage                           *
//*                                                                      *
//*                abstract image class for image handling               *
//*                                                                      *
//************************************************************************
class TTImage{

  public: 

    //constructor
    TTImage();

    //destructor
    virtual ~TTImage(){}

    //creates the image in memory
    //with default properties, e.g. 8bit RGB without alpha
    virtual bool Create(unsigned int w, unsigned int h) = 0;

    //creates the image in memory
    //with given properties
    virtual bool Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype) = 0;

    //return image data as image properties
    ImageProperties GetImageProperties() const;

    //builds tile info where te whole image is one tile
    ImageTileInfos GetTileInfosWholeImage() const;

    //returns the width
    virtual unsigned int Width() const = 0;

    //returns the height
    virtual unsigned int Height() const = 0;

    //returns the number of whole channels (== color channel + alpha channel)
    virtual unsigned int WholeChannels() const = 0;

    //returns the number of color channels
    virtual unsigned int ColorChannels() const = 0;

    //returns the number of channels
    virtual unsigned int AlphaChannels() const = 0;

    //return the colormode
    virtual ColormodeType Colormode() const = 0;

    //return the ColorSpace
    virtual ColorSpaceType ColorSpace() const = 0;

    //returns the simple colormode 
    ColormodeType SimpleColormode() const;

    //return the colormodebit depth per channel
    virtual Datatyp Datatype() const = 0;

    //return interlaced or not 
    virtual bool Interlaced() const = 0;

    //getter for pixel data
    virtual t_proc GetRed(const unsigned int x, const unsigned int y) const  = 0;
    virtual t_proc GetGreen(const unsigned int x, const unsigned int y) const = 0;
    virtual t_proc GetBlue(const unsigned int x, const unsigned int y) const  = 0;
    virtual t_proc GetGray(const unsigned int x, const unsigned int y) const = 0;
    virtual t_proc GetAlpha(const unsigned int x, const unsigned int y) const = 0;
    virtual image::TTColor GetColor(const unsigned int x, const unsigned int y);

    //setter for pixel data
    virtual void SetZero(unsigned int x, unsigned int y);
    virtual void SetImageZero();

    virtual void SetRed(const unsigned int x, const unsigned int y, const t_proc r) = 0;
    virtual void SetGreen(const unsigned int x, const unsigned int y, const t_proc g) = 0;
    virtual void SetBlue(const unsigned int x, const unsigned int y, const t_proc b) = 0;
    virtual void SetGray(const unsigned int x, const unsigned int y, const t_proc v);
    virtual void SetRGB(const unsigned int x, const unsigned int y, const t_proc r, const t_proc g, const t_proc b);
    virtual void SetColor(const unsigned int x, const unsigned int y, const TTColor& color);
    virtual void SetAlpha(const unsigned int x, const unsigned int y, const t_proc a) = 0;

    virtual void AddRed(const unsigned int x, const unsigned int y, const t_proc r);
    virtual void AddGreen(const unsigned int x, const unsigned int y, const t_proc g);
    virtual void AddBlue(const unsigned int x, const unsigned int y, const t_proc b);
    virtual void AddGray(const unsigned int x, const unsigned int y, const t_proc v);
    virtual void AddAlpha(const unsigned int x, const unsigned int y, const t_proc b);

    //global image manipulation
    virtual void CopyImage(const TTImage& other);
    virtual void CopyAlphaFromGray(const TTImage& other);

    //sets the software name
    void SetSoftware(const std::string software);

    //sets the software name
    std::string GetSoftware();

    //for information
    virtual std::string GetImageInfos() = 0;

    //gets a aproximate size in memory in MB if loaded
    virtual unsigned int GetMemoryUsage();

    //is valid 
    virtual bool IsValid() const ; 

  protected:

    std::string Software; 

};

//************************************************************************
//*                            TTMemoryImage                             *           
//*                                                                      *
//* abstract image class for images that are complete strored in memory  *
//* with direct access to all pixels                                     *
//************************************************************************
class TTMemoryImage : public virtual TTImage{

  public: 
    //constructor
    TTMemoryImage();

    //destructor
    virtual ~TTMemoryImage();

    //returns the pointer to raw imagedata
    virtual void* ImageData();

    //IsValid
    virtual bool IsValid() const ;

 protected:

    //imagedata 
    void * imageData = nullptr; 

};

//************************************************************************
//*                                                                      *
//*                         abstract TTFileImage                         *
//*                                                                      *
//*                  base class for image file handling                  *
//*                                                                      *
//************************************************************************
class TTFileImage : public virtual TTImage {

  public: 

    //constructor
    TTFileImage();

    //destructor
    virtual ~TTFileImage();

    //opens a image from file
    virtual bool Open(const std::string& filename) = 0;

    //opens a image from file but does not load image data
    virtual bool OpenFileInfo(const std::string& filename) = 0;

    //saves the image
    virtual bool Save(const std::string& filename) = 0;

    //closes the image and frees all resources
    virtual void Close();

    //resets the filepointer
    virtual bool ResetFilePointer() = 0;

    //returns a TTFileImage for this given filename
    //static TTFileImage* GetTTFileImageByEnding(const std::string& filename);

  protected:

    //file
    std::fstream* file;
};



//************************************************************************
//*                                                                      *
//*                       abstract TTByteOrderFile                       *
//*                                                                      *
//*             abstract class capsulating the byte order issus          *
//*                                                                      *
//************************************************************************
class TTByteOrderFile : public virtual TTFileImage {

  public:

    //constructor
    TTByteOrderFile();

    //reorder: swappes if necessary
    U16 Reorder(const U16 v) const;
    U32 Reorder(const U32 v) const;

    //swap methods
    U16 SwapBytes(const U16 v) const;
    U32 SwapBytes(const U32 v) const;

    //file read methods
    U8  ReadU8();
    U16 ReadU16();
    U32 ReadU32();
    std::string ReadString(const unsigned int length);

    //file write methods
    void WriteU8(const U8 v);
    void WriteU16(U16 v);
    void WriteU32(U32 v);
    void WriteString(const std::string& s);


  protected:

    bool ByteOrderIntel;



};


} //end of namespace image

