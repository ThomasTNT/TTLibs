#include "TTNixi.h"
#include "NixiImages.h"

#include <cmath>

//*****************************************************************************
//*                               TTNixi                                      *
//*                                                                           *
//*   Multi-digit Nixie tube display. Renders embedded BMP images on a       *
//*   black background with a sunken 3D border.                              *
//*   Images are self-contained — no external resources needed.              *
//*                                                                           *
//*****************************************************************************

//************
// constructor
//************
ttwin::TTNixi::TTNixi() : TTWin("TTNixi"){
  for (int i = 0; i < 11; ++i) Images[i] = nullptr;
}

//**********
// destructor
//**********
ttwin::TTNixi::~TTNixi(){
  for (int i = 0; i < 11; ++i){
    if (Images[i]){
      ::DeleteObject(Images[i]);
      Images[i] = nullptr;
    }
  }
}

//*************
// Configure
//
// digit count, leading zeros, pixel spacing between digits, and decimal places;
// call before Create()
//*************
void ttwin::TTNixi::Configure(const unsigned int digits, const bool leadingZeros, const int spacing, const unsigned int decimalPlaces){
  DigitCount    = (digits > 0) ? digits : 1;
  LeadingZeros  = leadingZeros;
  Spacing       = spacing;
  DecimalPlaces = decimalPlaces;
}

//**********************
// GetCurrentTotalWidth
//
// total window width for a given window height, using current configuration
//**********************
int ttwin::TTNixi::GetCurrentTotalWidth(const int windowHeight) const{
  const int contentH  = windowHeight - 2 * NIXI_BORDER;
  const int digitW    = contentH * NIXI_NATIVE_W / NIXI_NATIVE_H;
  return static_cast<int>(DigitCount) * digitW + (static_cast<int>(DigitCount) - 1) * Spacing + 2 * NIXI_BORDER;
}

//***********
// SetValue
//***********
void ttwin::TTNixi::SetValue(const double value){
  if (HasValue && Value == value) return;
  HasValue = true;
  Value    = value;
  if (Hwnd) Invalidate();
}

//***********
// ClearValue
//***********
void ttwin::TTNixi::ClearValue(){
  if (!HasValue) return;
  HasValue = false;
  if (Hwnd) Invalidate();
}

//*********
// PaintAll
//*********
void ttwin::TTNixi::PaintAll(){
  LoadImages();

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  ::FillRect(hdc, &rect, static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)));

  const int contentH = (rect.bottom - rect.top) - 2 * NIXI_BORDER;
  const int digitW   = contentH * NIXI_NATIVE_W / NIXI_NATIVE_H;
  const int y        = rect.top + NIXI_BORDER;

  // scale value by 10^DecimalPlaces and round to get integer digits
  const long long scaled = HasValue
    ? static_cast<long long>(Value * pow(10.0, static_cast<double>(DecimalPlaces)) + 0.5)
    : 0LL;
  long long digitValues[16] = {};
  long long temp = HasValue ? scaled : 0LL;
  for (int i = static_cast<int>(DigitCount) - 1; i >= 0; --i){
    digitValues[i] = temp % 10;
    temp /= 10;
  }
  const bool overflow = HasValue && (temp > 0);

  int x = rect.left + NIXI_BORDER;
  for (unsigned int i = 0; i < DigitCount; ++i){
    int imgIndex;
    if (!HasValue || overflow){
      imgIndex = 10;
    }
    else{
      bool allHigherAreZero = true;
      for (unsigned int j = 0; j < i; ++j)
        if (digitValues[j] != 0){ allHigherAreZero = false; break; }
      const bool isLeadingZero =    !LeadingZeros
                                 && (digitValues[i] == 0)
                                 && (i < DigitCount - 1)
                                 && allHigherAreZero;
      imgIndex = isLeadingZero ? 10 : static_cast<int>(digitValues[i]);
    }
    DrawDigit(hdc, x, y, digitW, contentH, imgIndex);
    x += digitW + Spacing;
  }

  DrawBorder(hdc, rect);

  ::EndPaint(Hwnd, &ps);
}

//***********
// LoadImages
//
// loads HBITMAPs from embedded BMP byte arrays on first call
//***********
void ttwin::TTNixi::LoadImages(){
  if (ImagesLoaded) return;
  static const unsigned char* const imageData[11] = {
    nixidata::nixi0, nixidata::nixi1, nixidata::nixi2, nixidata::nixi3,
    nixidata::nixi4, nixidata::nixi5, nixidata::nixi6, nixidata::nixi7,
    nixidata::nixi8, nixidata::nixi9, nixidata::nixioff
  };
  HDC screenDC = ::GetDC(NULL);
  for (int i = 0; i < 11; ++i){
    const unsigned char* data = imageData[i];
    const DWORD pixelOffset             = *reinterpret_cast<const DWORD*>(data + 10);
    const BITMAPINFOHEADER* bmih        = reinterpret_cast<const BITMAPINFOHEADER*>(data + 14);
    const BITMAPINFO*       bmi         = reinterpret_cast<const BITMAPINFO*>(data + 14);
    const void*             bits        = data + pixelOffset;
    Images[i] = ::CreateDIBitmap(screenDC, bmih, CBM_INIT, bits, bmi, DIB_RGB_COLORS);
  }
  ::ReleaseDC(NULL, screenDC);
  ImagesLoaded = true;
}

//***********
// DrawBorder
//
// draws a 3-pixel sunken border; light from top-left means
// top and left edges are dark, bottom and right edges are bright
//***********
void ttwin::TTNixi::DrawBorder(HDC hdc, const RECT& r){
  POINT pt;

  HPEN penShadowOuter    = ::CreatePen(PS_SOLID, 1, NIXI_SHADOW_OUTER);
  HPEN penShadowMid      = ::CreatePen(PS_SOLID, 1, NIXI_SHADOW_MID);
  HPEN penShadowInner    = ::CreatePen(PS_SOLID, 1, NIXI_SHADOW_INNER);
  HPEN penHighlightInner = ::CreatePen(PS_SOLID, 1, NIXI_HIGHLIGHT_INNER);
  HPEN penHighlightMid   = ::CreatePen(PS_SOLID, 1, NIXI_HIGHLIGHT_MID);
  HPEN penHighlightOuter = ::CreatePen(PS_SOLID, 1, NIXI_HIGHLIGHT_OUTER);

  // outer ring (row 0)
  HGDIOBJ orgPen = ::SelectObject(hdc, penShadowOuter);
  ::MoveToEx(hdc, r.left,     r.bottom - 1, &pt);
  ::LineTo  (hdc, r.left,     r.top);
  ::LineTo  (hdc, r.right,    r.top);
  ::SelectObject(hdc, penHighlightOuter);
  ::MoveToEx(hdc, r.left + 1, r.bottom - 1, &pt);
  ::LineTo  (hdc, r.right - 1, r.bottom - 1);
  ::LineTo  (hdc, r.right - 1, r.top + 1);

  // middle ring (row 1)
  ::SelectObject(hdc, penShadowMid);
  ::MoveToEx(hdc, r.left + 1, r.bottom - 2, &pt);
  ::LineTo  (hdc, r.left + 1, r.top + 1);
  ::LineTo  (hdc, r.right - 1, r.top + 1);
  ::SelectObject(hdc, penHighlightMid);
  ::MoveToEx(hdc, r.left + 2, r.bottom - 2, &pt);
  ::LineTo  (hdc, r.right - 2, r.bottom - 2);
  ::LineTo  (hdc, r.right - 2, r.top + 2);

  // inner ring (row 2)
  ::SelectObject(hdc, penShadowInner);
  ::MoveToEx(hdc, r.left + 2, r.bottom - 3, &pt);
  ::LineTo  (hdc, r.left + 2, r.top + 2);
  ::LineTo  (hdc, r.right - 2, r.top + 2);
  ::SelectObject(hdc, penHighlightInner);
  ::MoveToEx(hdc, r.left + 3, r.bottom - 3, &pt);
  ::LineTo  (hdc, r.right - 3, r.bottom - 3);
  ::LineTo  (hdc, r.right - 3, r.top + 3);

  ::SelectObject(hdc, orgPen);
  ::DeleteObject(penHighlightOuter);
  ::DeleteObject(penHighlightMid);
  ::DeleteObject(penHighlightInner);
  ::DeleteObject(penShadowInner);
  ::DeleteObject(penShadowMid);
  ::DeleteObject(penShadowOuter);
}

//**********
// DrawDigit
//
// blits one digit image scaled to the target cell
//**********
void ttwin::TTNixi::DrawDigit(HDC hdc, const int x, const int y, const int w, const int h, const int imgIndex){
  if (imgIndex < 0 || imgIndex > 10 || Images[imgIndex] == nullptr) return;
  HDC memDC = ::CreateCompatibleDC(hdc);
  HGDIOBJ old = ::SelectObject(memDC, Images[imgIndex]);
  ::StretchBlt(hdc, x, y, w, h, memDC, 0, 0, NIXI_NATIVE_W, NIXI_NATIVE_H, SRCCOPY);
  ::SelectObject(memDC, old);
  ::DeleteDC(memDC);
}
