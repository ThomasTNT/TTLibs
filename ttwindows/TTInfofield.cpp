#include "TTInfofield.h"

//*************************************************************************
//*                              TTInfofield                              *
//*                                                                       *
//*                          an area to show text                         *
//*                                                                       *
//*************************************************************************

//************
// constructor
//************
ttwin::TTInfofield::TTInfofield() : TTWin("TTInfofield") {}


//**********
//destructor
//**********
ttwin::TTInfofield::~TTInfofield(){}


//********************************
// UpdateText
//
// updates text and causes repaint
//********************************
void ttwin::TTInfofield::UpdateText(const std::string& text){
  if (Text == text) return;
  Text = text;
  Invalidate();
}

//********************************
// UpdateSubText
//
// updates sub text and causes repaint (shown only in classic layout)
//********************************
void ttwin::TTInfofield::UpdateSubText(const std::string& text){
  if (SubText == text) return;
  SubText = text;
  Invalidate();
}

//**********************
// SetSideBySideLayout
//**********************
void ttwin::TTInfofield::SetSideBySideLayout(const bool enable){
  SideBySideLayout = enable;
}

//**********************
// SetTextFontSize
//**********************
void ttwin::TTInfofield::SetTextFontSize(const int size){
  TextFontSize = size;
}

//**********************
// SetMainTextColor
//**********************
void ttwin::TTInfofield::SetMainTextColor(COLORREF color){
  if (MainTextColor == color) return;
  MainTextColor = color;
  Invalidate();
}

//*********
// PaintAll
//*********
void ttwin::TTInfofield::PaintAll(){

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  const int DRAW_TEXT_BORDER_HOR = 5;
  const int DRAW_TEXT_BORDER_VER = 15;

  //-------------- text area background -----------
  RECT inner = rect;
  inner.left   += DRAW_TEXT_BORDER_HOR;
  inner.right  -= DRAW_TEXT_BORDER_HOR;
  inner.top    += DRAW_TEXT_BORDER_VER;
  inner.bottom -= DRAW_TEXT_BORDER_VER;

  HBRUSH textgroundBrush = ::CreateSolidBrush(RGB(0, 0, 0));
  ::FillRect(hdc, &inner, textgroundBrush);
  ::DeleteObject(textgroundBrush);

  //--------------- text area border ---------------------
  HPEN penBorder1 = ::CreatePen(PS_SOLID, 1, RGB(129, 129, 129));
  HPEN penBorder2 = ::CreatePen(PS_SOLID, 1, RGB( 74,  74,  74));
  POINT point;
  OrgPen = ::SelectObject(hdc, penBorder1);
  ::MoveToEx(hdc, inner.left,      inner.bottom - 1, &point);
  ::LineTo  (hdc, inner.right - 1, inner.bottom - 1);
  ::LineTo  (hdc, inner.right - 1, inner.top    - 1);
  ::SelectObject(hdc, penBorder2);
  ::MoveToEx(hdc, inner.left,      inner.bottom - 2, &point);
  ::LineTo  (hdc, inner.left,      inner.top);
  ::LineTo  (hdc, inner.right - 1, inner.top);
  ::SelectObject(hdc, OrgPen);
  ::DeleteObject(penBorder2);
  ::DeleteObject(penBorder1);

  ::SetBkMode(hdc, TRANSPARENT);

  //------------------ label -----------------
  HFONT labelFont = ::CreateFont( 12,                        // height of font
                                   0,                         // average character width
                                   0,                         // angle of escapement
                                   0,                         // base-line orientation angle
                                   400,                       // font weight
                                   FALSE,                     // italic attribute option
                                   FALSE,                     // underline attribute option
                                   FALSE,                     // strikeout attribute option
                                   ANSI_CHARSET,              // character set identifier
                                   OUT_DEFAULT_PRECIS,        // output precision
                                   CLIP_DEFAULT_PRECIS,       // clipping precision
                                   ANTIALIASED_QUALITY,       // output quality
                                   VARIABLE_PITCH | FF_SWISS, // pitch and family
                                   "MS Shell Dlg");           // typeface name
  OrgFont = ::SelectObject(hdc, labelFont);
  ::SetTextColor(hdc, RGB(130, 130, 130));
  ::SetTextAlign(hdc, TA_LEFT | TA_BOTTOM);
  ::TextOut(hdc, inner.left + 6, inner.top + 20, Label.c_str(), static_cast<int>(Label.size()));

  if (SideBySideLayout){
    SIZE labelSize = {0, 0};
    ::GetTextExtentPoint32(hdc, Label.c_str(), static_cast<int>(Label.size()), &labelSize);
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(labelFont);

    //---------- main text: large, white, right of label, fills inner height ----------
    const int innerH  = inner.bottom - inner.top;
    const int textH   = innerH - 20; // slightly smaller than inner height, room for descenders
    const int textX   = inner.left + 6 + labelSize.cx + 10;

    HFONT textFont = ::CreateFont(-textH,                     // negative = character height
                                    0,                         // average character width
                                    0,                         // angle of escapement
                                    0,                         // base-line orientation angle
                                    400,                       // font weight
                                    FALSE,                     // italic attribute option
                                    FALSE,                     // underline attribute option
                                    FALSE,                     // strikeout attribute option
                                    ANSI_CHARSET,              // character set identifier
                                    OUT_DEFAULT_PRECIS,        // output precision
                                    CLIP_DEFAULT_PRECIS,       // clipping precision
                                    CLEARTYPE_QUALITY,         // output quality
                                    VARIABLE_PITCH | FF_SWISS, // pitch and family
                                    "MS Shell Dlg");           // typeface name
    OrgFont = ::SelectObject(hdc, textFont);
    ::SetTextColor(hdc, MainTextColor);
    ::SetTextAlign(hdc, TA_LEFT | TA_TOP);
    // clip so long titles don't overflow the field
    HRGN clipRgn = ::CreateRectRgn(textX, inner.top, inner.right - 4, inner.bottom);
    ::SelectClipRgn(hdc, clipRgn);
    ::TextOut(hdc, textX, inner.top + 2, Text.c_str(), static_cast<int>(Text.size()));
    ::SelectClipRgn(hdc, NULL);
    ::DeleteObject(clipRgn);
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(textFont);
  }
  else{
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(labelFont);

    //---------- text (classic layout: below label) ----------
    HFONT textFont = ::CreateFont(TextFontSize,               // height of font
                                   0,                         // average character width
                                   0,                         // angle of escapement
                                   0,                         // base-line orientation angle
                                   400,                       // font weight
                                   FALSE,                     // italic attribute option
                                   FALSE,                     // underline attribute option
                                   FALSE,                     // strikeout attribute option
                                   ANSI_CHARSET,              // character set identifier
                                   OUT_DEFAULT_PRECIS,        // output precision
                                   CLIP_DEFAULT_PRECIS,       // clipping precision
                                   ANTIALIASED_QUALITY,       // output quality
                                   VARIABLE_PITCH | FF_SWISS, // pitch and family
                                   "MS Shell Dlg");           // typeface name
    OrgFont = ::SelectObject(hdc, textFont);
    ::SetTextColor(hdc, MainTextColor);
    ::SetTextAlign(hdc, TA_LEFT | TA_BOTTOM);
    ::TextOut(hdc, inner.left + 6, inner.top + 50, Text.c_str(), static_cast<int>(Text.size()));
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(textFont);

    //---------- subtext (small, bottom) ----------
    if (!SubText.empty()){
      HFONT subtextFont = ::CreateFont( 12,                        // height of font
                                         0,                         // average character width
                                         0,                         // angle of escapement
                                         0,                         // base-line orientation angle
                                         400,                       // font weight
                                         FALSE,                     // italic attribute option
                                         FALSE,                     // underline attribute option
                                         FALSE,                     // strikeout attribute option
                                         ANSI_CHARSET,              // character set identifier
                                         OUT_DEFAULT_PRECIS,        // output precision
                                         CLIP_DEFAULT_PRECIS,       // clipping precision
                                         ANTIALIASED_QUALITY,       // output quality
                                         VARIABLE_PITCH | FF_SWISS, // pitch and family
                                         "MS Shell Dlg");           // typeface name
      OrgFont = ::SelectObject(hdc, subtextFont);
      ::SetTextColor(hdc, RGB(200, 200, 200));
      ::SetTextAlign(hdc, TA_LEFT | TA_BOTTOM);
      ::TextOut(hdc, inner.left + 6, inner.bottom - 3, SubText.c_str(), static_cast<int>(SubText.size()));
      ::SelectObject(hdc, OrgFont);
      ::DeleteObject(subtextFont);
    }
  }

  ::SelectObject(hdc, OrgFont);
  ::SelectObject(hdc, OrgPen);

  ::EndPaint(Hwnd, &ps);
}
