#ifndef TTSIMPLE_MEMORY_IMAGE_H
#define TTSIMPLE_MEMORY_IMAGE_H

#include "TTImageFiles.h"

namespace image{


//**************************************************************************
//*                         TTSimpleMemoryImage                             *           
//*                                                                        *
//*              a simple implementation of a memory image                 *
//*                                                                        *
//*                    it uses alway t_proc as data type                   *
//*                                                                        *
//* NOTE: There are no checks!                                             *
//* E.g. if you want to set a blue pixel in a gray image it can crash!     *
//**************************************************************************
class TTSimpleMemoryImage : public virtual TTMemoryImage{

  public:

    //constructor
    TTSimpleMemoryImage();


    //destructor
    virtual ~TTSimpleMemoryImage();

    //creates the image in memory
    //with default properties
    virtual bool Create(unsigned int w, unsigned int h);

    //creates the image in memory
    //with given properties
    //WARNING: Datatyp is ignored!
    virtual bool Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype);

    //returns the width
    virtual unsigned int Width() const;

    //returns the height
    virtual unsigned int Height() const;

    //returns the number of alpha channels
    virtual unsigned int AlphaChannels() const;

    //returns the number of whole channels (== color channel + alpha channel)
    virtual unsigned int WholeChannels() const;

    //returns the number of color channels
    virtual unsigned int ColorChannels() const;

    //return the colormode
    virtual ColormodeType Colormode() const;

    //return the ColorSpace
    virtual ColorSpaceType ColorSpace() const;

    //return bit depth per channel
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

    //for information
    virtual std::string GetImageInfos();

  protected:

    //frees all resources
    void Close();

    //image data
    t_proc* ImageDataProc = nullptr;

    //image properties
    unsigned int width = 0;
    unsigned int height = 0;
    ColormodeType colormode = UNKNOWN_COLORMODE;
    ColormodeType simpleColormode = UNKNOWN_COLORMODE;
    unsigned int colorChannels = 0;
    unsigned int alphaChannels = 0;
    
};


} // end of namespace image

#endif
