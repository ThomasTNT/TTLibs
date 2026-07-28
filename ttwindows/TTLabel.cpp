#include "TTLabel.h"

//*****************************************************************
//*                             TTLabel                           *
//*                                                               *
//*                                                               *
//*                    windows class for labels                   *
//*                                                               *
//*****************************************************************

//**********
//contructor
//**********
ttwin::TTLabel::TTLabel() : TTWin("TTLabel"), AlignHor(TA_LEFT), AlignVer(TA_BOTTOM),
                            ColorText(RGB(255, 0, 0))
{}

//*********************
// Configure
//
// configures the label
//*********************
void ttwin::TTLabel::Configure(const UINT alignHor, const UINT alignVer){
  AlignHor = alignHor;
  AlignVer = alignVer;
} 

//*************
// SetColorText
//
// set color
//*************
void ttwin::TTLabel::SetColorText(const COLORREF colorText){
  ColorText = colorText;
}


//*********
// PaintAll
//*********
void ttwin::TTLabel::PaintAll(){

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  DrawBackground(hdc, false);

  HFONT labelFont = ::CreateFont(FontHeight,                 // height of font
                                  0,                         // average character width
                                  0,                         // angle of escapement
                                  0,                         // base-line orientation angle
                                  FontWeight,                // font weight
                                  FALSE,                     // italic attribute option
                                  FALSE,                     // underline attribute option
                                  FALSE,                     // strikeout attribute option
                                  ANSI_CHARSET,              // character set identifier
                                  OUT_DEFAULT_PRECIS,        // output precision
                                  CLIP_DEFAULT_PRECIS,       // clipping precision
                                  ANTIALIASED_QUALITY,       // output quality
                                  VARIABLE_PITCH | FF_SWISS, // pitch and family
                                  "MS Shell Dlg");           // typeface name
  ::SetBkMode(hdc, TRANSPARENT);
  OrgFont = ::SelectObject(hdc, labelFont);
  ::SetTextColor(hdc, ColorText);

  ::DrawText(hdc, Label.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

  ::SelectObject(hdc, OrgFont);
  ::DeleteObject(labelFont);

  ::EndPaint(Hwnd, &ps);

}

