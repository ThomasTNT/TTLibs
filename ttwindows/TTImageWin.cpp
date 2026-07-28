#include "TTImageWin.h"

//*****************************************************************************
//*                                 TTImageWin                                *
//*                                                                           *
//*                                                                           *
//*          TTWin to show an manipulate an image or image sequence           *
//*                                                                           *
//*****************************************************************************

//***********
//constructor
//***********
ttwin::TTImageWin::TTImageWin() : TTWin("TTImageWin"){

}

//**********
//destructor
//**********
ttwin::TTImageWin::~TTImageWin(){

}


//************************
// SetImgProcessor
//
// setter for imgProcessor
//************************
void ttwin::TTImageWin::SetImgProcessor(imgproc::ImageProcessor* const imgProcessor){
  ImgProcessor = imgProcessor;
}

//*********
// Init
//*********
void ttwin::TTImageWin::Init(){

  //sets call back object for progress bar etc.
  ImgProcessor->SetCallback(this);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  const int w = rect.right - rect.left;
  const int h = rect.bottom - rect.top;

  //the imageprocessor decides what to do with this information
  ImgProcessor->SetWindowDimensions(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
}


//*********
// PaintAll
//*********
void ttwin::TTImageWin::PaintAll(){

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  /*
  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  if (BackgroundImage) BackgroundImage->Init(hdc);
  DrawBackground(hdc, true);

  //used pens etc.
  HPEN outline = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 100));

  rect.bottom -= 1;
  rect.right -= 1;

  POINT point;
  HGDIOBJ orgpen = ::SelectObject(hdc, outline);
  MoveToEx(hdc, rect.left, rect.top, &point);
  LineTo(hdc, rect.right, rect.top);
  LineTo(hdc, rect.right, rect.bottom);
  LineTo(hdc, rect.left, rect.bottom);
  LineTo(hdc, rect.left, rect.top);*/

  if (BitmapShowHandle){

    const HDC hdcMem = ::CreateCompatibleDC(hdc);
    const HBITMAP hbmOld = reinterpret_cast<HBITMAP>(::SelectObject(hdcMem, BitmapShowHandle));

    const unsigned int imgW = ImgProcessor->GetWidth();
    const unsigned int imgH = ImgProcessor->GetHeight();

    ::BitBlt(hdc, BitmapOffsetX, BitmapOffsetY, BitmapWidth, BitmapHeight, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
  }

  //current selection 
  if (Mode == TTIMGWIN_SELCOLOR){
    POINT point;
    HPEN outline = ::CreatePen(PS_SOLID, 1, RGB(255, 200, 20));
    HGDIOBJ orgpen = ::SelectObject(hdc, outline);
    MoveToEx(hdc, StartPointX, StartPointY, &point);
    LineTo(hdc, CurrPointX, StartPointY);
    LineTo(hdc, CurrPointX, CurrPointY);
    LineTo(hdc, StartPointX, CurrPointY);
    LineTo(hdc, StartPointX, StartPointY);
    ::SelectObject(hdc, orgpen);
    ::DeleteObject(outline);
  }

  //other stuff
  POINT point;
  const std::vector<image::TTColorArea> areas = ImgProcessor->GetAreasToDraw();
  HPEN outline = ::CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
  HGDIOBJ orgpen = ::SelectObject(hdc, outline);
  for (const image::TTColorArea& area : areas){
    switch (area.GetType()){
      case image::COLORAREATYPE_RECT:
        MoveToEx(hdc, area.GetValue0(), area.GetValue1(), &point);
        LineTo(hdc,   area.GetValue2(), area.GetValue1());
        LineTo(hdc,   area.GetValue2(), area.GetValue3());
        LineTo(hdc,   area.GetValue0(), area.GetValue3());
        LineTo(hdc,   area.GetValue0(), area.GetValue1());
        break;
    }
  }
  ::SelectObject(hdc, orgpen);
  ::DeleteObject(outline);


  //free device context
  ::EndPaint(Hwnd, (LPPAINTSTRUCT)&ps);
}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  if (ImgProcessor && ImgProcessor->ReadyForAreaSelection()){
    Mode = TTIMGWIN_SELCOLOR;   
  }
 
  StartPointX = x;
  StartPointY = y;
  CurrPointX = x;
  CurrPointY = y;
}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_RightDown(int x, int y, bool shift, bool ctrl, bool alt){
  ::MessageBox(Hwnd, "CM_Mouse_RightDown", "MOUSE", 0);
}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_LeftDouble(int x, int y, bool shift, bool ctrl, bool alt){
  ::MessageBox(Hwnd, "CM_Mouse_LeftDouble", "MOUSE", 0);
}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){
  if (Mode == TTIMGWIN_SELCOLOR){
    //we wouln't be in this mode when we had no processor
    //so no check neccessary

    const int Ximg0 = (StartPointX < CurrPointX ? StartPointX : CurrPointX) - BitmapOffsetX;
    const int Ximg1 = (StartPointX > CurrPointX ? StartPointX : CurrPointX) - BitmapOffsetX;
    const int Yimg0 = (StartPointY < CurrPointY ? StartPointY : CurrPointY) - BitmapOffsetY;
    const int Yimg1 = (StartPointY > CurrPointY ? StartPointY : CurrPointY) - BitmapOffsetY;

    const unsigned int imgWidth = ImgProcessor->GetWidth();
    const unsigned int imgHeight = ImgProcessor->GetHeight();

    //must be in image
    if (   (Ximg0 >= 0 || Ximg1 >= 0)
        && (Ximg0 < imgWidth || Ximg1 < imgWidth)
        && (Yimg1 >= 0 && Yimg1 >= 0)
        && (Yimg0 < imgHeight || Yimg1 < imgHeight))
    { 
      ImgProcessor->AreaSelection(static_cast<unsigned int>(max(Ximg0, 0)), 
                                  static_cast<unsigned int>(max(Yimg0, 0)),
                                  min(static_cast<unsigned int>(Ximg1), imgWidth - 1),
                                  min(static_cast<unsigned int>(Yimg1), imgHeight - 1));

      //informing parent causes indiretly update or invalidate
      if (TTWinParent){
        TTWinParent->ChildWasClickedAndChanged(this, 0, 0);
      }
      else{
        SendChildWasClickedAndChangedMsg(0);
      }

    }
    
  }
  else if (Mode == TTIMGWIN_NORMAL){
    BitmapOffsetX += (CurrPointX - StartPointX);
    BitmapOffsetY += (CurrPointY - StartPointY);
    Invalidate();
  }

  //resets points
  StartPointX = -1;
  StartPointY = -1;
  CurrPointX = -1;
  CurrPointY = -1;
 
  //end mode
  Mode = TTIMGWIN_NORMAL;

}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_RightUp(int x, int y, bool shift, bool ctrl, bool alt){
  ::MessageBox(Hwnd, "CM_Mouse_RightUp", "MOUSE", 0);
  Mode = TTIMGWIN_NORMAL;
}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt){
  CurrPointX = x;
  CurrPointY = y;

  if (Mode == TTIMGWIN_SELCOLOR){
    Invalidate();
  }
  
}

//*****************
//CM_Mouse_LeftDown
//*****************
void ttwin::TTImageWin::CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt){
  //???? do nothing
}



//*******************************
// OpenImages
//
// opens image or image sequence
//*******************************
void ttwin::TTImageWin::OpenImages(const std::string& directory, const std::vector<std::string>& filenames, const unsigned int before, const unsigned int after){
  ImgProcessor->OpenImages(directory, filenames, before, after);
  ImgProcessor->SetSequencePos(0);
  Uptodate = false;
}

//**************************
// UpdateImage
//
// reloads the image to show
//**************************
void ttwin::TTImageWin::UpdateImage(){
  const bool isSequence = ImgProcessor->IsSequence();
  const unsigned int frameNumber = ImgProcessor->GetSequencePos();

  if (ImgProcessor->IsLoaded(frameNumber)){
    const image::TTImage * const image = ImgProcessor->GetImageToShow(frameNumber);
    const unsigned int imgWidth = image->Width();
    const unsigned int imgHeight = image->Height();

    char* buffer = new char[imgWidth * imgHeight * 4];
    
    for (unsigned int y = 0; y < imgHeight; ++y){
      for (unsigned int x = 0; x < imgWidth; ++x){
        buffer[(x + imgWidth * y) * 4 + 2] = image::TTColorConv::U8FromProc(image->GetRed(x, y));
        buffer[(x + imgWidth * y) * 4 + 1] = image::TTColorConv::U8FromProc(image->GetGreen(x, y));
        buffer[(x + imgWidth * y) * 4 + 0] = image::TTColorConv::U8FromProc(image->GetBlue(x, y));
      }
    }

    BitmapWidth = imgWidth;
    BitmapHeight = imgHeight;
    BitmapShowHandle = CreateBitmap(BitmapWidth, BitmapHeight, 1, 32, buffer);
  }

  Invalidate();
}

//***********************************
// IsImageLoaded
//
// returns true if an image is loaded
//***********************************
bool ttwin::TTImageWin::IsImageLoaded(){
  const unsigned int framenumber = ImgProcessor->GetSequencePos();
  return ImgProcessor->GetImageOriginal(framenumber) != nullptr;
}

//***********************************************
// IsImageLoaded
//
// returns true if the processed image is to show 
//***********************************************
bool ttwin::TTImageWin::IsShowProcessed(){
  return ShowProcessed;
}

//********************************************************
// IsImageLoaded
//
// returns true if the image processing is still processed 
//********************************************************
bool ttwin::TTImageWin::IsInProcess(){
  return InProcess;
}

//*************************************************
// IsUptodate
//
// returns true if the processed image is upto date
//*************************************************
bool ttwin::TTImageWin::IsUptodate(){
  return Uptodate;
}

//*******************
// IsImageLoaded
//
// sets showProcessed
//*******************
void ttwin::TTImageWin::SetShowProcessed(const bool showProcessed){
  if (!showProcessed){
    ShowProcessed = showProcessed;
  }
  else{
    const unsigned int framenumber = ImgProcessor->GetSequencePos();
    if (ImgProcessor->GetImageProcessed(framenumber) != nullptr){
      ShowProcessed = true;
    }
    else{
      ShowProcessed = false;
    }
  }
}

//************************
//ProcessImage
//
// processes current image
//************************
void ttwin::TTImageWin::ProcessImage(const imgproc::ImageProcessParameters& imageProcParams){
  InProcess = true;
  ImgProcessor->ProcessImage(imageProcParams);
  Uptodate = true;
  UpdateImage();
  InProcess = false;
}

//***********************
// Event_ParameterChanged
//***********************
void ttwin::TTImageWin::Event_ParameterChanged(){
  Uptodate = false;
}