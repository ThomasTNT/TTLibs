#include "TTBackgroundImage.h"

#include "Notificator.h"

//*****************************************************************************
//*                            TTBackgroundImage                              *
//*                                                                           *
//*                                                                           *
//*         class used by TTWin and other to handle a background image        *
//*                                                                           *
//*****************************************************************************

//************
// constructor
//************
ttwin::TTBackgroundImage::TTBackgroundImage() : HDCMem(0), 
                                                Bitmap(0), 
                                                BitmapWidth(0), 
                                                BitmapHeight(0),
                                                MainWinWidth(0), 
                                                MainWinHeight(0),
                                                Scalefactor(1.0),
                                                PosType(BACKGROUNDPOS_CENTER)
{
}

//***********
// destructor
//***********
ttwin::TTBackgroundImage::~TTBackgroundImage(){
  Clear();
  if (!HDCMem) ::DeleteDC(HDCMem);
}

//***********************************************************
// Init
//
// initializes the image 
// can be called mulitple times without effect
// so it should be called in paint method of the owner window
//***********************************************************
void ttwin::TTBackgroundImage::Init(const HDC hdc){
  if (!HDCMem) HDCMem = CreateCompatibleDC(hdc);
}

//*******************************
// Configure
//
// configures the image behaviour
//*******************************
void ttwin::TTBackgroundImage::Configure(const BackGroundPosition pos){
  PosType = pos;
}

//*********************************
// SetMainDimensions
// 
// for determining the scale factor
//*********************************
void ttwin::TTBackgroundImage::SetMainDimensions(const int width, const int height){
  MainWinWidth = width;
  MainWinHeight = height;
  if (Valid() && MainWinWidth > 0 && MainWinHeight > 0){
    const double scaleW = abs(static_cast<double>(width) / static_cast<double>(BitmapWidth));
    const double scaleH = abs(static_cast<double>(height) / static_cast<double>(BitmapHeight));
    Scalefactor = scaleW > scaleH ? scaleW : scaleH;
  }
  else{
    Scalefactor = 1.0;
  }
}

//**********************************
// Valide
//
// method to check if image is valid
//**********************************
bool ttwin::TTBackgroundImage::Valid() const {
  return HDCMem && Bitmap && BitmapWidth > 0 && BitmapHeight > 0;
}

//******************************************
// CreateMemDC
//
// create a compatible memory device context
//******************************************
void ttwin::TTBackgroundImage::CreateMemDC(const HDC hdc){
  HDCMem = ::CreateCompatibleDC(hdc);
}

//*******************************
// LoadBitmapResource
//
// loads the bitmap from resource
//*******************************
bool ttwin::TTBackgroundImage::LoadBitmapResource(const int resourceId){
  const HBITMAP bitmap = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId));
  if (!bitmap){
    ttutil::Notificator::GetInstance().AddNotification(ttutil::Notification(ttutil::NORMAL_ERROR, "4000", "LoadBitmapResource", "Could not open bitmap resource: " + ttutil::StringUtil::int2string(resourceId) + "!"));
    return false;
  }
  if (Bitmap) ::DeleteObject(Bitmap);
  Bitmap = bitmap;
  Filename.clear();
  BITMAP bm;
  ::GetObject(Bitmap, sizeof(bm), &bm);
  BitmapWidth = bm.bmWidth;
  BitmapHeight = bm.bmHeight;
  return true;
}

//***************************
// LoadBitmapResource
//
// loads the bitmap from file
//***************************
bool ttwin::TTBackgroundImage::LoadBitmapFromFile(const std::string& filename){
  const HBITMAP bitmap = static_cast<HBITMAP>(::LoadImage(NULL, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
  if (!bitmap){
    ttutil::Notificator::GetInstance().AddNotification(ttutil::Notification(ttutil::NORMAL_ERROR, "4000", "OpenBackImage", "Could not open bitmap: " + filename + "!"));
  }
  if (Bitmap) ::DeleteObject(Bitmap);
  Bitmap = bitmap;
  Filename = filename;
  BITMAP bm;
  ::GetObject(Bitmap, sizeof(bm), &bm);
  BitmapWidth = bm.bmWidth;
  BitmapHeight = bm.bmHeight;
  ttutil::Notificator::GetInstance().AddNotification(ttutil::Notification(ttutil::TRACE_INFO, "4000", "OpenBackImage", "Opened new background: " + filename + "!"));
  return true;
}

//***********************
// Clear
//
// sets the image invalid
//***********************
void ttwin::TTBackgroundImage::Clear(){
  if (Bitmap) ::DeleteObject(Bitmap);
  Bitmap = 0;
  Filename.clear();
  BitmapWidth = 0;
  BitmapHeight = 0;
}

//*********************
// GetFilename
//
// returns the filename
//*********************
std::string ttwin::TTBackgroundImage::GetFilename() const{
  return Filename;
}

//**********************************************
// DrawImage
//
// draws the image to the windows device context
// offset are in screen coodinates
//**********************************************
void ttwin::TTBackgroundImage::DrawImage(HDC hdc, const RECT& tile, const int offsetX, const int offsetY){
  if (!Valid()) return;

  //center but do not scale or use the upper left corner
  if (Scalefactor <= 1.0){

    //tile in bitmap coordinates
    int bx0 = (PosType == BACKGROUNDPOS_UPPERLEFT ? 0 : (BitmapWidth - MainWinWidth) / 2) + offsetX;
    int by0 = (PosType == BACKGROUNDPOS_UPPERLEFT ? 0 : (BitmapHeight - MainWinHeight) / 2) + offsetY;
    int bw  = tile.right - tile.left;
    int bh  = tile.bottom - tile.top;

    //tile (bitmap part) in screen coordinates
    int tx0 = tile.left;
    int ty0 = tile.top;
    int tw = tile.right - tile.left;
    int th = tile.bottom - tile.top;

    HBRUSH brushBlack    = CreateSolidBrush(RGB(0, 0, 0));
    HGDIOBJ orgBrush     = ::SelectObject(hdc, brushBlack);
    //fill left with black if tile starts further left that image
    if (bx0 < 0){
      ::Rectangle(hdc, tx0, ty0, tx0 - bx0, ty0 + th);
      tx0 -= bx0;
      tw += bx0;
      bx0 = 0;
    }
    //fill right side with black
    if ((BitmapWidth - bx0) < tw){
      ::Rectangle(hdc, tx0 + BitmapWidth - bx0, ty0, tx0 + tw, ty0 + th);
      tw = BitmapWidth - bx0;
    }
    //fill top with black
    if (by0 < 0){
      ::Rectangle(hdc, tx0, ty0, tx0 + tw, ty0 - by0);
      ty0 -= by0;
      th += by0;
      by0 = 0;
    }

    //fill bottom with black
    if ((BitmapHeight - by0) < th){
      ::Rectangle(hdc, tx0, ty0 + BitmapHeight - by0, tx0 + tw, ty0 + th);
      th = BitmapHeight - by0;
    }
    ::SelectObject(hdc, orgBrush);
    ::DeleteObject(brushBlack);

    if (tw > 0 && th > 0) {
      ::SelectObject(HDCMem, Bitmap);
      ::BitBlt(hdc, tx0, ty0, tw, th, HDCMem, bx0, by0, SRCCOPY);
    }
  }
  //scale it centered
  else{
    const double bx0s = (static_cast<double>(BitmapWidth) * Scalefactor - MainWinWidth) / 2.0 + offsetX;
    const double by0s = (static_cast<double>(BitmapHeight) * Scalefactor - MainWinHeight) / 2.0 + offsetY;
    const double bws  = tile.right - tile.left;
    const double bhs  = tile.bottom - tile.top;

    const int bx0 = static_cast<int>(bx0s / Scalefactor + 0.5);
    const int by0 = static_cast<int>(by0s / Scalefactor + 0.5);
    const int bw = static_cast<int>(bws / Scalefactor + 0.5);
    const int bh = static_cast<int>(bhs / Scalefactor + 0.5);

    ::SelectObject(HDCMem, Bitmap);
    const BOOL success = ::StretchBlt(hdc, 
                                     tile.left, tile.top, tile.right - tile.left, tile.bottom - tile.top,
                                     HDCMem, 
                                     bx0, by0, bw, bh, 
                                     SRCCOPY);
    if (!success){
      const int errocode = ::GetLastError();
      ttutil::Notificator::GetInstance().AddNotification(ttutil::Notification(ttutil::WARNING, "9000", "OpenBackImage", "Error scaling image: " + ttutil::StringUtil::int2string(errocode) + "!"));
    }
  }
}


    