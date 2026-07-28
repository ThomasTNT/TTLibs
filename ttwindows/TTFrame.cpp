#include "TTFrame.h"

//*****************************************************************
//*                            TTFrame                            *
//*                                                               *
//*                                                               *
//*             class for a frame similar to a group box          *
//*             this is not a window class. It's only a           *
//*             structur that draws directly on the dialog        *
//*                                                               *
//*****************************************************************

//***********
// contructor
//***********..
ttwin::TTFrame::TTFrame() : show(true) {}

//********
// Create
//********
bool ttwin::TTFrame::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, int x, int y, int width, int height, TTControlUnit unit){
  
  Rect.left   = x;
  Rect.right  = x + width;
  Rect.top    = y;
  Rect.bottom = y + height;

  //convert coordinates into pixel
  if (unit == DIALOGUNIT){ 
    ::MapDialogRect(parent, &Rect);
  }

  Parent = parent;
  Label = label;
  return true;
}

//********************
// Resize
// 
// resizes the control
//********************
void ttwin::TTFrame::Resize(int x, int y, int width, int height, TTControlUnit unit){
  Rect.left   = x;
  Rect.right  = x + width;
  Rect.top    = y;
  Rect.bottom = y + height;
  //convert coordinates into pixel
  if (unit == DIALOGUNIT){ 
    ::MapDialogRect(Parent, &Rect);
  }
}

//****************************************************
// Paint
// 
// this function will be called in parent dialog paint
// it draws directly on the dialog 
//****************************************************
void ttwin::TTFrame::PaintAll(HDC hdc){

  if (show){

    const int FONT_HEIGHT = 12; 

    //---------------------------- draw label ------------------------------------

    HFONT labelFont = CreateFont( FONT_HEIGHT,               // height of font
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
    SetBkMode(hdc, TRANSPARENT);
    HGDIOBJ orgFont = SelectObject(hdc, labelFont);
    SetTextAlign(hdc, TA_LEFT);
    SetTextAlign(hdc, TA_TOP);
    SetTextColor(hdc, RGB(255, 255, 255));

    TextOut(hdc, Rect.left + FONT_HEIGHT, Rect.top, Label.c_str(), static_cast<int>(Label.size()));

    const int textlength = GetWidthOfLabel(hdc); //used for border drawing

    ::SelectObject(hdc, orgFont);
    ::DeleteObject(labelFont);

    //---------------------------- draw border ------------------------------------

    const int topspace  = FONT_HEIGHT / 2;
    const int leftspace = FONT_HEIGHT - FONT_HEIGHT / 4;
    const int rigthspace = FONT_HEIGHT - FONT_HEIGHT / 4;

    HPEN penBright = ::CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
    HPEN penDark   = ::CreatePen(PS_SOLID, 1, RGB( 75,  75,  75));

    POINT point;
    HGDIOBJ orgPen = SelectObject(hdc, penDark);
    MoveToEx(hdc, Rect.left, Rect.bottom, &point);
    LineTo  (hdc, Rect.left, Rect.top + topspace);
    LineTo  (hdc, Rect.left + leftspace, Rect.top + topspace);
    MoveToEx(hdc, Rect.left + leftspace + textlength + rigthspace, Rect.top + topspace, &point);
    LineTo  (hdc, Rect.right - 1, Rect.top + topspace);
    MoveToEx(hdc, Rect.left + 2, Rect.bottom - 2, &point);
    LineTo  (hdc, Rect.right - 2, Rect.bottom - 2);
    LineTo  (hdc, Rect.right - 2, Rect.top + topspace);

    SelectObject(hdc, penBright);
    MoveToEx(hdc, Rect.right - 3, Rect.top + 1 + topspace, &point);
    LineTo  (hdc, Rect.left + leftspace + textlength + rigthspace - 1, Rect.top + 1 + topspace);
    MoveToEx(hdc, Rect.left + leftspace - 1, Rect.top + 1 + topspace, &point);
    LineTo  (hdc, Rect.left + 1, Rect.top + 1 + topspace);
    LineTo  (hdc, Rect.left + 1, Rect.bottom - 1);
    LineTo  (hdc, Rect.right - 1, Rect.bottom - 1);
    LineTo  (hdc, Rect.right - 1, Rect.top - 1 + topspace);

    ::SelectObject(hdc, orgPen);
    ::DeleteObject(penDark);
    ::DeleteObject(penBright);
  }
}


//*******************************************
// GetWidthOfLabel
//
// calculates the width of the label in pixel
// we need this to draw the border correctly
//*******************************************
int ttwin::TTFrame::GetWidthOfLabel(HDC hdc){
  int result = 0;

  ABC abc;
  char c;
  for (size_t i = 0; i < Label.size(); i++){ 
    c = Label[i];
    GetCharABCWidths(hdc, c, c, &abc);  
	  result += ::abs(abc.abcA); //distance to add to the current position before drawing the character glyph
	  result +=       abc.abcB;  //width of the drawn portion of the character glyph
	  result += ::abs(abc.abcC); //distance to add to the current position to provide white space to the right 
  }
  return result;
}

//***************************
// Show
//
// shows or hides the control
//***************************
void ttwin::TTFrame::Show(const bool show){
  this->show = show;
  ::InvalidateRect(Parent, &Rect, TRUE);
}


//*****************************************************************
//*                         TTColorFrame                          *
//*                                                               *
//*                                                               *
//*             frame with inner color and boarder color          *
//*                                                               *
//*****************************************************************

//****************
// SetColors
//
// sets the colors
//****************
void ttwin::TTColorFrame::SetColors(const COLORREF border, const bool drawborder, const COLORREF inner, const bool drawinner){
  BorderColor = border;
  DrawBorder = drawborder;
  InnerColor = inner;
  DrawInner = drawinner;
}

//****************
// SetColors
//
// sets the colors
//****************
void ttwin::TTColorFrame::SetColorsInactive(const COLORREF inner, const COLORREF cross){
  InactiveInner = inner;
  InactiveCross = cross;
}

//**********
// SetActive
//**********
void ttwin::TTColorFrame::SetActive(const bool active){
  Active = active;
}

//****************************************************
// Paint
// 
// this function will be called in parent dialog paint
// it draws directly on the dialog 
//****************************************************
void ttwin::TTColorFrame::PaintAll(HDC hdc){

  if (show){

    if (Active){
      HBRUSH brush = CreateSolidBrush(InnerColor);
      HGDIOBJ orgBrush = ::SelectObject(hdc, brush);
      ::Rectangle(hdc, Rect.left, Rect.top, Rect.right, Rect.bottom);
      ::SelectObject(hdc, orgBrush);
      ::DeleteObject(brush);
    }
    else{
      HBRUSH brush = CreateSolidBrush(InactiveInner);
      HGDIOBJ orgBrush = ::SelectObject(hdc, brush);
      ::Rectangle(hdc, Rect.left, Rect.top, Rect.right, Rect.bottom);
      ::SelectObject(hdc, orgBrush);
      ::DeleteObject(brush);

      HPEN pen = ::CreatePen(PS_SOLID, 1, InactiveCross);
      POINT point;
      HGDIOBJ orgPen = SelectObject(hdc, pen);
      MoveToEx(hdc, Rect.left, Rect.top, &point);
      LineTo(hdc, Rect.right, Rect.bottom);
      MoveToEx(hdc, Rect.left, Rect.bottom, &point);
      LineTo(hdc, Rect.right, Rect.top);
      ::SelectObject(hdc, orgPen);
      ::DeleteObject(pen);
    }
    

  }
}
