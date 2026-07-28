#ifndef TTIMAGEWIN_H
#define TTIMAGEWIN_H

#include <string>

#include <windows.h>

#include "stringutil.h"
#include "TTWin.h"

#include "ImageProcessor.h"


namespace ttwin{

//*****************************************************************************
//*                                 TTImageWin                                *
//*                                                                           *
//*                                                                           *
//*          TTWin to show an manipulate an image or image sequence           *
//*                                                                           *
//*****************************************************************************
enum TTImageWinMode{ TTIMGWIN_NORMAL, TTIMGWIN_SELCOLOR };


class TTImageWin : public TTWin, public TTCallback {

  public:

    //constructor
    TTImageWin();

    //destructor
    ~TTImageWin();

    //setter for imgProcessor
    void SetImgProcessor(imgproc::ImageProcessor* const imgProcessor);

    //opens image or image sequence
    void OpenImages(const std::string& directory, const std::vector<std::string>& filenames, const unsigned int before, const unsigned int after);

    //reloads the image to show
    void UpdateImage();

    //returns true if an image is loaded
    bool IsImageLoaded();

    //returns true if the processed image is to show 
    bool IsShowProcessed();

    //returns true if the image processing is still processed 
    bool IsInProcess();

    //returns true if the processed image is upto date
    bool IsUptodate();

    //sets showProcessed
    void SetShowProcessed(const bool showProcessed);

    //processes current image
    void ProcessImage(const imgproc::ImageProcessParameters& imageProcParams);

    //called when a parameter has changed
    void Event_ParameterChanged();

  protected:

    virtual void Init();

    //intialisation and update
    virtual void PaintAll();

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_RightDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftDouble(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_RightUp(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt);

  private:

    TTImageWinMode Mode = TTIMGWIN_NORMAL;
    int StartPointX = -1;
    int StartPointY = -1;
    int CurrPointX = -1;
    int CurrPointY = -1;

    imgproc::ImageProcessor* ImgProcessor = nullptr;

    HBITMAP BitmapShowHandle = 0;
    unsigned int BitmapWidth = 0;
    unsigned int BitmapHeight = 0;
    int BitmapOffsetX = 0;
    int BitmapOffsetY = 0;

    bool ShowProcessed = false;
    bool InProcess = false;
    bool Uptodate = false;

};

} // end of namespace ttwin

#endif
