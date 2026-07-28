#include "TTSevenSegment.h"

//*************************************************************************
//*                          TTSevenSegment                               *
//*                                                                       *
//*   Fixed-width display showing up to 5 alphanumeric characters.       *
//*   Renders red-orange text on black background as a 7-segment dummy.  *
//*                                                                       *
//*************************************************************************

//************
// constructor
//************
ttwin::TTSevenSegment::TTSevenSegment() : TTWin("TTSevenSegment")
{
}

//**********
// destructor
//**********
ttwin::TTSevenSegment::~TTSevenSegment(){
}

//***************
// UpdateText
//***************
void ttwin::TTSevenSegment::UpdateText(const std::string& text){
  if (DisplayText == text) return;
  DisplayText = text;
  Invalidate();
}

//************
// SetColor
//************
void ttwin::TTSevenSegment::SetColor(COLORREF color){
  if (Color == color) return;
  Color = color;
  Invalidate();
}

//*********
// PaintAll
//*********
void ttwin::TTSevenSegment::PaintAll(){
  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  ::FillRect(hdc, &rect, static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)));

  HFONT font = ::CreateFont(
    14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    ANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, "Courier New");
  const HGDIOBJ oldFont = ::SelectObject(hdc, font);

  ::SetTextColor(hdc, Color);
  ::SetBkMode(hdc, TRANSPARENT);
  RECT textRect = rect;
  textRect.right -= 3;
  ::DrawText(hdc, DisplayText.c_str(), -1, &textRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

  ::SelectObject(hdc, oldFont);
  ::DeleteObject(font);
  ::EndPaint(Hwnd, &ps);
}
