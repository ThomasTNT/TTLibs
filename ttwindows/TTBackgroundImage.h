#ifndef __TTBACKGROUNDIMAGE_h_
#define __TTBACKGROUNDIMAGE_h_

#include <string>

#include <windows.h>

#include "stringutil.h"

namespace ttwin{

//*****************************************************************************
//*                            TTBackgroundImage                              *
//*                                                                           *
//*                                                                           *
//*         class used by TTWin and other to handle a background image        *
//*                                                                           *
//*****************************************************************************

enum BackGroundPosition {BACKGROUNDPOS_CENTER, BACKGROUNDPOS_UPPERLEFT};

class TTBackgroundImage{

  public:

    //constructor
    TTBackgroundImage();

    //destructor
    ~TTBackgroundImage();

    //initializes the image 
    //can be called mulitple times without effect
    //so it should be called in paint method of the owner window
    void Init(const HDC hdc);

    //configures the image behaviour
    void Configure(const BackGroundPosition pos);

    //for determining the scale factor
    void SetMainDimensions(const int width, const int height);

    //loads the bitmap from resource
    bool LoadBitmapResource(const int resourceId);

    //loads the bitmap from file
    bool LoadBitmapFromFile(const std::string& filename);

    //sets the image invalid
    void Clear();

    //draws the image to the windows device context
    void DrawImage(HDC hdc, const RECT& tile, const int offsetX, const int offsetY);

    //method to check if image is valid
    bool Valid() const;

    //returns the filename
    std::string GetFilename() const;

  private:

    //create a compatible memory device context
    void CreateMemDC(const HDC hdc);

    //handles for brackground image
    HDC     HDCMem;
    HBITMAP Bitmap; 
    int BitmapWidth;
    int BitmapHeight;
    int MainWinWidth;
    int MainWinHeight;
    double Scalefactor;
    BackGroundPosition PosType;

    //filename of the image
    //empty when from resource
    std::string Filename; 


};

} // end of namespace ttwin

#endif