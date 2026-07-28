#include "WinWireRenderer.h"

//************************************************************************
//*                                                                      *
//*                            WinWireRenderer                           *
//*                                                                      *
//*                    simple wire renderer for window GDI               *
//************************************************************************

//************
// constructor
//************
cgi::WinWireRenderer::WinWireRenderer(const RECT rect) : hDC(0), Pen(0), ScreenRect(rect){
  ScreenWidth  = rect.right - rect.left;
  ScreenHeight = rect.bottom - rect.top;
}
    
//***********
// destructor
//***********
cgi::WinWireRenderer::~WinWireRenderer(){
}

//******************************************
// StartRendering
//
// sets the device contect for draw function
//******************************************
void cgi::WinWireRenderer::StartRendering(HDC hDC){
  this->hDC = hDC;
  Pen = CreatePen(PS_SOLID, 1,  RGB(0, 0, 0));
}
//*************************
// StopRendering
//
// clean up after rendering
//*************************
void cgi::WinWireRenderer::StopRendering(){
  DeleteObject(Pen);
}

//***************
// Drawbackground
//***************
void cgi::WinWireRenderer::Drawbackground(const RGBColor& color){
  const U8 R = ck::ColorServices::ConvDoubleToU8(color.R);
  const U8 G = ck::ColorServices::ConvDoubleToU8(color.G);
  const U8 B = ck::ColorServices::ConvDoubleToU8(color.B);
}

//*************
// SetLineColor
//*************
void cgi::WinWireRenderer::SetLineColor(const RGBColor& color){
  const U8 R = ck::ColorServices::ConvDoubleToU8(color.R);
  const U8 G = ck::ColorServices::ConvDoubleToU8(color.G);
  const U8 B = ck::ColorServices::ConvDoubleToU8(color.B);
  DeleteObject(Pen);
  Pen = CreatePen(PS_SOLID, 1,  RGB(R, G, B));
  SelectObject(hDC, Pen);
}


//********
// MoveTo
//********
void cgi::WinWireRenderer::MoveTo(const double x, const double y){
  const int currPosX = static_cast<int>(x + 0.5) + ScreenRect.left;
  const int currPosY = static_cast<int>(y + 0.5) + ScreenRect.top;
  POINT point;
  ::MoveToEx(hDC, currPosX, currPosY, &point);
}

//********
// LineTo
//********
void cgi::WinWireRenderer::LineTo(const double x, const double y){
  const int currPosX = static_cast<int>(x + 0.5) + ScreenRect.left;
  const int currPosY = static_cast<int>(y + 0.5) + ScreenRect.top;
  ::LineTo(hDC, currPosX, currPosY);
}
    
 

