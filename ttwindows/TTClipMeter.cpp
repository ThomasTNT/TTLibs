#include "TTClipMeter.h"

//**********
// constructor
//**********
ttwin::TTClipMeter::TTClipMeter() : TTWin("TTClipMeter")
{
}

//*****************************
// Configure
//
// sets max reduction range and LED color
//*****************************
void ttwin::TTClipMeter::Configure(const float maxReductionDb, const COLORREF reductionColor) {
  MaxReductionDb  = maxReductionDb > 0.0f ? maxReductionDb : 1.0f;
  ReductionColor  = reductionColor;
}

//***********************
// SetGainReduction
//
// gain reduction in dB (positive = reduction)
//***********************
void ttwin::TTClipMeter::SetGainReduction(const float dB) {
  const float clamped = dB < 0.0f ? 0.0f : (dB > MaxReductionDb ? MaxReductionDb : dB);
  if (clamped != CurrentReductionDb) {
    CurrentReductionDb = clamped;
    ::InvalidateRect(Hwnd, nullptr, FALSE);
  }
}

//***************
// SetClipping
//***************
void ttwin::TTClipMeter::SetClipping(const bool clipping) {
  if (clipping != Clipping) {
    Clipping = clipping;
    ::InvalidateRect(Hwnd, nullptr, FALSE);
  }
}

//************************************
// Created
//
// called after window creation
//************************************
void ttwin::TTClipMeter::Created() {
  Init();
}

//*******
// Init
//*******
void ttwin::TTClipMeter::Init() {
  const RECT rect = GetClientRect();
  MaxPos = (rect.right - rect.left) - 4; // 2px border on each side
  if (MaxPos < 0) MaxPos = 0;
}

//*********
// PaintAll
//*********
void ttwin::TTClipMeter::PaintAll() {
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();
  DrawBackground(hdc, true);

  const int x0 = rect.left  + 2;
  const int x1 = rect.right - 2;
  const int y0 = rect.top   + 2;
  const int y1 = rect.bottom - 2;

  // black background for LED area
  HPEN   nullPen    = ::CreatePen(PS_NULL, 1, RGB(0, 0, 0));
  HBRUSH blackBrush = ::CreateSolidBrush(RGB(0, 0, 0));
  OrgPen            = ::SelectObject(hdc, nullPen);
  HGDIOBJ orgBrush  = ::SelectObject(hdc, blackBrush);
  ::Rectangle(hdc, x0, y0, x1 + 1, y1 + 1);

  POINT point;
  HPEN dcPen = static_cast<HPEN>(::GetStockObject(DC_PEN));
  ::SelectObject(hdc, dcPen);

  if (Clipping) {
    // dark red solid background
    HBRUSH darkRedBrush = ::CreateSolidBrush(RGB(80, 0, 0));
    HGDIOBJ prevBrush   = ::SelectObject(hdc, darkRedBrush);
    ::Rectangle(hdc, x0, y0, x1 + 1, y1 + 1);
    ::SelectObject(hdc, prevBrush);
    ::DeleteObject(darkRedBrush);
    // "CLIPPING" text in red
    const int fontH = (y1 - y0 - 2) * 7 / 10;
    HFONT clipFont = ::CreateFont(-fontH, 0, 0, 0, 700, FALSE, FALSE, FALSE,
                                  ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  ANTIALIASED_QUALITY, VARIABLE_PITCH | FF_SWISS, "MS Shell Dlg");
    HGDIOBJ orgFont = ::SelectObject(hdc, clipFont);
    ::SetBkMode(hdc, TRANSPARENT);
    ::SetTextColor(hdc, RGB(210, 40, 40));
    ::SetTextAlign(hdc, TA_CENTER | TA_TOP);
    ::TextOut(hdc, (x0 + x1) / 2, (y0 + y1 - fontH) / 2, "CLIPPING", 8);
    ::SelectObject(hdc, orgFont);
    ::DeleteObject(clipFont);
  }
  else if (CurrentReductionDb > 0.0f) {
    const int barWidth = static_cast<int>(CurrentReductionDb / MaxReductionDb * static_cast<float>(MaxPos) + 0.5f);
    const int startX   = x1 - barWidth > x0 ? x1 - barWidth : x0;
    ::SetDCPenColor(hdc, ReductionColor);
    for (int x = startX; x < x1; x += 2) {
      ::MoveToEx(hdc, x, y0, &point);
      ::LineTo(hdc, x, y1);
    }
  }

  // border
  HPEN penBright = ::CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
  HPEN penDark   = ::CreatePen(PS_SOLID, 1, RGB(90,  90,  90));
  ::SelectObject(hdc, penBright);
  ::MoveToEx(hdc, x0 - 1, y1,     &point); ::LineTo(hdc, x0 - 1, y0 - 1);
  ::MoveToEx(hdc, x0 - 1, y0 - 1, &point); ::LineTo(hdc, x1 + 1, y0 - 1);
  ::SelectObject(hdc, penDark);
  ::MoveToEx(hdc, x1,     y0 - 1, &point); ::LineTo(hdc, x1,     y1 + 1);
  ::MoveToEx(hdc, x1,     y1 + 1, &point); ::LineTo(hdc, x0 - 2, y1 + 1);
  ::DeleteObject(penBright);   // penBright is deselected (penDark is current) — safe

  ::SelectObject(hdc, blackBrush);
  ::SelectObject(hdc, OrgPen);  // deselects penDark before we delete it
  ::SelectObject(hdc, orgBrush);
  ::DeleteObject(penDark);      // now safe: no longer selected
  ::DeleteObject(nullPen);
  ::DeleteObject(blackBrush);

  EndPaint(Hwnd, &ps);
}
