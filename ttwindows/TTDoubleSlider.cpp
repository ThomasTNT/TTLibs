#include "TTDoubleSlider.h"

//***************
// TTDoubleSlider
//***************
ttwin::TTDoubleSlider::TTDoubleSlider(){
  SLIDER_ZEROOFFSET = 4;
}

//*********
// PaintAll
//*********
void ttwin::TTDoubleSlider::PaintAll(){
  const bool isEnabled = ::IsWindowEnabled(Hwnd) != FALSE;

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();

  DrawBackground(hdc, true);

  HPEN penSlot1  = ::CreatePen(PS_SOLID, 1, RGB( 75,  75,  75));
  HPEN penSlot2  = ::CreatePen(PS_SOLID, 1, RGB( 21,  21,  21));
  HPEN penSlot2a = ::CreatePen(PS_SOLID, 1, RGB(148, 119,  29));
  HPEN penSlot3  = ::CreatePen(PS_SOLID, 1, RGB( 45,  45,  45));
  HPEN penSlot3a = ::CreatePen(PS_SOLID, 1, RGB(193, 155,  37));
  HPEN penSlot4  = ::CreatePen(PS_SOLID, 1, RGB(184, 184, 184));

  HPEN   penKnob1  = ::CreatePen(PS_SOLID, 1, RGB(172, 172, 172));
  HPEN   penKnob2  = ::CreatePen(PS_SOLID, 1, RGB( 74,  74,  74));
  HBRUSH brushKnob = ::CreateSolidBrush(RGB( 74,  74,  74));
  HPEN   penKnob3  = ::CreatePen(PS_SOLID, 1, RGB( 19,  19,  19));
  HPEN   penKnob4  = ::CreatePen(PS_SOLID, 1, RGB( 37,  37,  37));

  HPEN penScale = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

  POINT point;
  const int centerY   = rect.bottom - 10;
  const int leftStop  = Pos  + 3;
  const int rightStop = Pos2 + 3;

  // slot frame
  OrgPen = ::SelectObject(hdc, penSlot1);
  ::MoveToEx(hdc, rect.right - 5, centerY - 2, &point);
  ::LineTo  (hdc, rect.left  + 3, centerY - 2);
  ::LineTo  (hdc, rect.left  + 3, centerY + 1);

  ::SelectObject(hdc, penSlot4);
  ::MoveToEx(hdc, rect.left  + 3, centerY + 1, &point);
  ::LineTo  (hdc, rect.right - 4, centerY + 1);
  ::LineTo  (hdc, rect.right - 4, centerY - 3);

  // dark zone left of lower handle
  ::SelectObject(hdc, penSlot2);
  ::MoveToEx(hdc, leftStop,    centerY - 1, &point);
  ::LineTo  (hdc, rect.left + 3, centerY - 1);
  ::SelectObject(hdc, penSlot3);
  ::MoveToEx(hdc, rect.left + 4, centerY, &point);
  ::LineTo  (hdc, leftStop,    centerY);

  // dark zone right of upper handle
  ::SelectObject(hdc, penSlot2);
  ::MoveToEx(hdc, rect.right - 6, centerY - 1, &point);
  ::LineTo  (hdc, rightStop,      centerY - 1);
  ::SelectObject(hdc, penSlot3);
  ::MoveToEx(hdc, rightStop,      centerY, &point);
  ::LineTo  (hdc, rect.right - 5, centerY);
  ::LineTo  (hdc, rect.right - 5, centerY - 2);

  // highlighted zone between the two handles
  if (leftStop < rightStop){
    ::SelectObject(hdc, penSlot2a);
    ::MoveToEx(hdc, rightStop, centerY - 1, &point);
    ::LineTo  (hdc, leftStop,  centerY - 1);
    ::SelectObject(hdc, penSlot3a);
    ::MoveToEx(hdc, rightStop, centerY, &point);
    ::LineTo  (hdc, leftStop,  centerY);
  }

  // scale ticks
  const int scaleLong    = 4;
  const int scaleShort   = 2;
  const int distToCenter = 3;
  if (ScaleDiv >= 3){
    ::SelectObject(hdc, penScale);
    for (unsigned int i = 0; i < ScaleDiv; ++i){
      const double value  = RangeMin + static_cast<double>(i) * ((RangeMax - RangeMin) / static_cast<double>(ScaleDiv - 1));
      const int    pos    = PosFromValue(value);
      const int    length = (i == 0 || i == ScaleDiv - 1 || i == ScaleDiv / 2) ? scaleLong : scaleShort;
      ::MoveToEx(hdc, rect.left + SLIDER_ZEROOFFSET + pos, centerY + distToCenter,          &point);
      ::LineTo  (hdc, rect.left + SLIDER_ZEROOFFSET + pos, centerY + distToCenter + length);
    }
  }

  // knob at Pos (lower handle / ThresholdOff)
  ::SelectObject(hdc, penKnob1);
  ::MoveToEx(hdc, rect.left + 1 + Pos, centerY + 3, &point);
  ::LineTo  (hdc, rect.left + 1 + Pos, centerY - 4);
  ::LineTo  (hdc, rect.left + 7 + Pos, centerY - 4);
  ::SelectObject(hdc, penKnob2);
  OrgBrush = ::SelectObject(hdc, brushKnob);
  ::Rectangle(hdc, rect.left + 2 + Pos, centerY - 3, rect.left + 6 + Pos, centerY + 3);
  ::SelectObject(hdc, penKnob3);
  ::MoveToEx(hdc, rect.left + 2 + Pos, centerY + 3, &point);
  ::LineTo  (hdc, rect.left + 6 + Pos, centerY + 3);
  ::LineTo  (hdc, rect.left + 6 + Pos, centerY - 4);
  ::SelectObject(hdc, penKnob4);
  ::MoveToEx(hdc, rect.left + 7 + Pos, centerY + 3, &point);
  ::LineTo  (hdc, rect.left + 7 + Pos, centerY - 4);
  if (isEnabled){
    HPEN redLED = ::CreatePen(PS_SOLID, 1, RGB(175, 36, 32));
    ::SelectObject(hdc, redLED);
    ::MoveToEx(hdc, rect.left + 3 + Pos, centerY - 2, &point);
    ::LineTo  (hdc, rect.left + 5 + Pos, centerY - 2);
    ::MoveToEx(hdc, rect.left + 2 + Pos, centerY - 1, &point);
    ::LineTo  (hdc, rect.left + 2 + Pos, centerY + 1);
    ::DeleteObject(redLED);
    redLED = ::CreatePen(PS_SOLID, 1, RGB(186, 32, 27));
    ::SelectObject(hdc, redLED);
    ::MoveToEx(hdc, rect.left + 3 + Pos, centerY + 1, &point);
    ::LineTo  (hdc, rect.left + 5 + Pos, centerY + 1);
    ::MoveToEx(hdc, rect.left + 5 + Pos, centerY - 1, &point);
    ::LineTo  (hdc, rect.left + 5 + Pos, centerY + 1);
    ::DeleteObject(redLED);
    ::SetPixel(hdc, rect.left + 3 + Pos, centerY,     RGB(252,  60,  54));
    ::SetPixel(hdc, rect.left + 4 + Pos, centerY - 1, RGB(252,  60,  54));
    ::SetPixel(hdc, rect.left + 3 + Pos, centerY - 1, RGB(253, 125, 121));
    ::SetPixel(hdc, rect.left + 4 + Pos, centerY,     RGB(252,  60,  54));
  }

  // knob at Pos2 (upper handle / ThresholdOn) — drawn on top
  ::SelectObject(hdc, penKnob1);
  ::MoveToEx(hdc, rect.left + 1 + Pos2, centerY + 3, &point);
  ::LineTo  (hdc, rect.left + 1 + Pos2, centerY - 4);
  ::LineTo  (hdc, rect.left + 7 + Pos2, centerY - 4);
  ::SelectObject(hdc, penKnob2);
  ::SelectObject(hdc, brushKnob);
  ::Rectangle(hdc, rect.left + 2 + Pos2, centerY - 3, rect.left + 6 + Pos2, centerY + 3);
  ::SelectObject(hdc, penKnob3);
  ::MoveToEx(hdc, rect.left + 2 + Pos2, centerY + 3, &point);
  ::LineTo  (hdc, rect.left + 6 + Pos2, centerY + 3);
  ::LineTo  (hdc, rect.left + 6 + Pos2, centerY - 4);
  ::SelectObject(hdc, penKnob4);
  ::MoveToEx(hdc, rect.left + 7 + Pos2, centerY + 3, &point);
  ::LineTo  (hdc, rect.left + 7 + Pos2, centerY - 4);
  if (isEnabled){
    HPEN redLED = ::CreatePen(PS_SOLID, 1, RGB(175, 36, 32));
    ::SelectObject(hdc, redLED);
    ::MoveToEx(hdc, rect.left + 3 + Pos2, centerY - 2, &point);
    ::LineTo  (hdc, rect.left + 5 + Pos2, centerY - 2);
    ::MoveToEx(hdc, rect.left + 2 + Pos2, centerY - 1, &point);
    ::LineTo  (hdc, rect.left + 2 + Pos2, centerY + 1);
    ::DeleteObject(redLED);
    redLED = ::CreatePen(PS_SOLID, 1, RGB(186, 32, 27));
    ::SelectObject(hdc, redLED);
    ::MoveToEx(hdc, rect.left + 3 + Pos2, centerY + 1, &point);
    ::LineTo  (hdc, rect.left + 5 + Pos2, centerY + 1);
    ::MoveToEx(hdc, rect.left + 5 + Pos2, centerY - 1, &point);
    ::LineTo  (hdc, rect.left + 5 + Pos2, centerY + 1);
    ::DeleteObject(redLED);
    ::SetPixel(hdc, rect.left + 3 + Pos2, centerY,     RGB(252,  60,  54));
    ::SetPixel(hdc, rect.left + 4 + Pos2, centerY - 1, RGB(252,  60,  54));
    ::SetPixel(hdc, rect.left + 3 + Pos2, centerY - 1, RGB(253, 125, 121));
    ::SetPixel(hdc, rect.left + 4 + Pos2, centerY,     RGB(252,  60,  54));
  }

  ::SelectObject(hdc, OrgPen);
  ::SelectObject(hdc, OrgBrush);

  ::DeleteObject(penScale);
  ::DeleteObject(penKnob4);
  ::DeleteObject(penKnob3);
  ::DeleteObject(brushKnob);
  ::DeleteObject(penKnob2);
  ::DeleteObject(penKnob1);
  ::DeleteObject(penSlot4);
  ::DeleteObject(penSlot3a);
  ::DeleteObject(penSlot3);
  ::DeleteObject(penSlot2a);
  ::DeleteObject(penSlot2);
  ::DeleteObject(penSlot1);

  ::EndPaint(Hwnd, &ps);
}
