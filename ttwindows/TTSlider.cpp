#include "TTSlider.h"

#include "Logger.h"

//*****************************************************************
//*                     AbstractSliderControl                     *
//*                                                               *
//*                                                               *
//*       base class for slider controls for photoshop plugins    *
//*                                                               *
//*****************************************************************

//**********
//contructor
//**********
ttwin::AbstractSlider::AbstractSlider() : TTWin("TTSlider"), 
                                          Pos(0), 
                                          MaxPos(0),
                                          RangeMin(0.0),
                                          RangeMax(1.0),
                                          SnapPos(-1),
                                          SnapValue(0.0),
                                          SnapRange(0),
                                          ScaleDiv(3),
                                          Illumination(ttwin::NONE),
                                          Type(ttwin::LINEAR),
                                          TypeFactor(0.5),
                                          SLIDER_ZEROOFFSET(0),
                                          VERTICAL(false)
{
}


//**********************
// Configure
//
// configures the slider
//**********************
void ttwin::AbstractSlider::Configure(double rangeMin, double rangeMax, unsigned int scaleDiv, 
                                      SliderIlluminationType illumination, SliderType type, SliderGridType grid, 
                                      double typeFactor, int snapRange)
{
  if (rangeMin <= rangeMax){
    RangeMin = rangeMin;
    RangeMax = rangeMax;
  } 
  else{
    RangeMin = rangeMax;
    RangeMax = rangeMin;
  } 
  
  ScaleDiv = scaleDiv % 2 == 1 ? scaleDiv : scaleDiv + 1;
  if (ScaleDiv < 3) ScaleDiv = 3;

  Illumination = illumination;
  Type         = type;
  TypeFactor   = typeFactor;
  Grid         = grid;
  SnapRange    = snapRange;
  switch (Type){
    case LINEAR_INT:
    case LINEAR:               Exponent = 1.0;
                               break;
    case EXPONENTIAL:          Exponent = log((TypeFactor - RangeMin) / (RangeMax - RangeMin)) / log(0.5);
                               break;
    case EXPONENTIAL_NEGATIVE: Exponent = TypeFactor;
                               break;
    //case LOGARITHMIC: 
  }

  switch (Grid){
    
    case SNAPTOCENTERPOS:      SnapPos = MaxPos / 2;
                               SnapValue = ValueFromPos(SnapPos);
                               break;
    case SNAPTOZEROPOS:        SnapValue = 0.0;
                               SnapPos = PosFromValue(SnapValue);
                               break;
    case NOGRID:               
    default:                   SnapPos = -1;
                               SnapValue = 0;
  }
}

//*********
// SetValue
//*********
void ttwin::AbstractSlider::SetValue(double value){
  int oldPos = Pos;
  Pos = PosFromValue(value); 
  if (Pos != oldPos){
    const RECT rect = GetClientRect();
    RECT updateRect = CalcUpdateRect(oldPos, Pos, rect);
    InvalidateRect(Hwnd, &updateRect, TRUE);
  }
}

//*********
// GetValue
//*********
double ttwin::AbstractSlider::GetValue(){
  return ValueFromPos(Pos);
}

//************************************************
// GetValueInt
//
// returns the value rounded to the neares integer
//************************************************
int ttwin::AbstractSlider::GetValueInt(){
  return static_cast<int>(GetValue() + 0.5);
}


//*********
// SetRangeMax
//*********
/*void pscontrols::AbstractSliderControl::SetRangeMax(double max){
  RangeMax = max;
}*/

//************************************
// Created 
//
// this is called after creation
// hack for calling Init()
// Why is ther no WM_CREATE Message?!?
//************************************
void ttwin::AbstractSlider::Created(){
  Init(); //just call init
}

//*******
// Init
//*******
void ttwin::AbstractSlider::Init(){
  const RECT rect = GetClientRect();
  const int width = rect.right - rect.left;
  MaxPos = width - 2 * SLIDER_ZEROOFFSET + 1; //slider way is shorter than width (0...MaxPos == slider way)
  LeftMouseDown = false;
}

//----------------------------------- event functions ----------------------------------------

//******************
// CM_Mouse_LeftDown
//******************
void ttwin::AbstractSlider::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  SendChildWasClickedMsg(0); //value is not of interest for now

  LeftMouseDown = true;
  int currPos = VERTICAL ? y : x;

  //make that WM_MOUSE_LEAVE messages will be sent
  TRACKMOUSEEVENT event;
  event.cbSize      = sizeof(event);
  event.dwFlags     = TME_LEAVE;
  event.hwndTrack   = Hwnd;
  event.dwHoverTime = HOVER_DEFAULT;
  _TrackMouseEvent(&event);

  ::SetCapture(Hwnd);

  const RECT rect = GetClientRect();
  currPos -= SLIDER_ZEROOFFSET;
  if (currPos > MaxPos) currPos = MaxPos;
  if (currPos < 0)      currPos = 0;
  if (currPos != Pos){
    currPos = SnapToGrid(Pos, currPos);
    RECT updateRect = CalcUpdateRect(currPos, Pos, rect);
    Pos = currPos;
    SendHScrollMessage();
    InvalidateRect(Hwnd, &updateRect, TRUE);
    //Update();
  }
  Logger::getInstance().println("CM_Mouse_LeftDown x: " + ttutil::StringUtil::int2string(x) + " currPos: " + ttutil::StringUtil::int2string(currPos));
}

//****************
// CM_Mouse_LeftUp
//****************
void ttwin::AbstractSlider::CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){
  LeftMouseDown = false;
  ::ReleaseCapture();
}

//**************
// CM_Mouse_Move
//**************
void ttwin::AbstractSlider::CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt){
  if (LeftMouseDown){
    int currPos = VERTICAL ? y : x;
    const RECT rect = GetClientRect();
    currPos -= SLIDER_ZEROOFFSET;
    if (currPos > MaxPos) currPos = MaxPos;
    if (currPos < 0)      currPos = 0;
    if (currPos != Pos){
      currPos = SnapToGrid(Pos, currPos);
      RECT updateRect = CalcUpdateRect(currPos, Pos, rect);
      Pos = currPos;
      SendHScrollMessage();
      InvalidateRect(Hwnd, &updateRect, TRUE);
      //Update();
    }
  }
}

//***************
// CM_Mouse_Leave
//***************
void ttwin::AbstractSlider::CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt){
  //we capture the mouse, to theres no need to check for mouse leave
  //LeftMouseDown = false;
}

//***************************************************
// CalcUpdateRect
//
// gets the rectandle to update when slider has moved
//***************************************************
RECT ttwin::AbstractSlider::CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect){
  RECT rect = controllerrect;
  return rect;
}


//************************************************
// SendHScrollMessage
//
// send a WM_HSCROLL message like standard sliders
//************************************************
void ttwin::AbstractSlider::SendHScrollMessage(){
  ::SendMessage(Parent,     //HWND to which the message should be sent, our parent of course 
                WM_HSCROLL, //the message 
                0,          //wParam is not used in our main window
                reinterpret_cast<LPARAM>(Hwnd)); //the window handle is strored in lParam 
}

//*************
// PosFromValue
//*************
int ttwin::AbstractSlider::PosFromValue(double value){
  switch (Type){
    case LINEAR_INT:
    case LINEAR:               return PosFromValueLin(value);
    case EXPONENTIAL:          return PosFromValueExp(value);
    case EXPONENTIAL_NEGATIVE: return PosFromValueExpNeg(value);
    case LINSCALE:             return PosFromValueLinScale(value);
    default: return 0;
  }
}

//*************
// ValueFromPos
//*************
double ttwin::AbstractSlider::ValueFromPos(int pos){

  //prevent rounding errors on grid position 
  if (Grid != NOGRID && pos == SnapPos) return SnapValue;

  switch (Type){
    case LINEAR_INT:
    case LINEAR:               return ValueFromPosLin(pos);
    case EXPONENTIAL:          return ValueFromPosExp(pos);
    case EXPONENTIAL_NEGATIVE: return ValueFromPosExpNeg(pos);
    case LINSCALE:             return ValueFromPosLinScale(pos);
    default: return 0.0;
  }
}

//****************
// PosFromValueLin
//****************
int ttwin::AbstractSlider::PosFromValueLin(double value){
  if (value == std::numeric_limits<double>::infinity() || value <= RangeMin){
    return 0;
  }
  else if (value >= RangeMax){
    return MaxPos;
  }
  else{
    return static_cast<int>(static_cast<double>(MaxPos) * (value - RangeMin) / (RangeMax - RangeMin) + 0.5);
  }  
}

//****************
// ValueFromPosLin
//****************
double ttwin::AbstractSlider::ValueFromPosLin(int pos){
  return (RangeMax - RangeMin) * static_cast<double>(pos) / static_cast<double>(MaxPos) + RangeMin;
}

//****************
// PosFromValueExp
//****************
int ttwin::AbstractSlider::PosFromValueExp(double value){
  if (value == std::numeric_limits<double>::infinity()) return 0;
  return static_cast<int>(static_cast<double>(MaxPos) * pow((value - RangeMin) / (RangeMax - RangeMin), 1.0 / Exponent) + 0.5);
}

//****************
// ValueFromPosExp
//****************
double ttwin::AbstractSlider::ValueFromPosExp(int pos){
  return pow((static_cast<double>(pos) / static_cast<double>(MaxPos)), Exponent) * (RangeMax - RangeMin) + RangeMin;  
}

//*******************
// PosFromValueExpNeg
//*******************
int ttwin::AbstractSlider::PosFromValueExpNeg(double value){
  if (value == std::numeric_limits<double>::infinity()) return 0;
  return   value >= 0
         ? MaxPos / 2 + static_cast<int>(pow((value / RangeMax), 1.0 / Exponent) * static_cast<double>(MaxPos / 2) + 0.5)
         : MaxPos / 2 - static_cast<int>(pow((value / RangeMin), 1.0 / Exponent) * static_cast<double>(MaxPos / 2) + 0.5);
}

//*******************
// ValueFromPosExpNeg
//*******************
double ttwin::AbstractSlider::ValueFromPosExpNeg(int pos){
  pos -= MaxPos / 2;
  return   pos >= 0 
         ? pow((static_cast<double>(pos) * 2.0 / static_cast<double>(MaxPos)), Exponent) * RangeMax
         : pow((static_cast<double>(-pos) * 2.0 / static_cast<double>(MaxPos)), Exponent) * RangeMin;
}

//*********************
// PosFromValueLinScale
//*********************
int ttwin::AbstractSlider::PosFromValueLinScale(double value){
  if (value == std::numeric_limits<double>::infinity()) return 0;
  return   (value >= 1.0)
         ? static_cast<int>(((value - 1.0) / (RangeMax - 1.0)) * (static_cast<double>(MaxPos) / 2.0) + (static_cast<double>(MaxPos) / 2.0) + 0.5)
         : static_cast<int>( static_cast<double>(MaxPos) * 0.5 * (value - RangeMin) / (1.0 - RangeMin) + 0.5);
}

//*********************
// ValueFromPosLinScale
//*********************
double ttwin::AbstractSlider::ValueFromPosLinScale(int pos){
  double ratio = (static_cast<double>(pos) / static_cast<double>(MaxPos)) - 0.5; 
  return   (ratio >= 0.0) 
         ? (RangeMax - 1.0) * ratio / 0.5 + 1.0 
         : ratio * (1.0 - RangeMin) / 0.5 + 1.0;
}

//***************************************
// SnapToGrid
//
// snaps to grid if it is not mode NOGRID
// returns the new position 
//***************************************
int ttwin::AbstractSlider::SnapToGrid(const int oldPos, const int newPos){
  if (Grid == NOGRID){
    return newPos;
  }
  else{
    const int dist = abs(SnapPos - newPos);
    if (dist <= SnapRange){
      const int move = newPos - oldPos;
      if (move > 0 && newPos < SnapPos || move < 0 && newPos > SnapPos){
        return SnapPos; 
      } 
    }
  }
  return newPos;
}

//*****************************************************************
//*                     AbstractDoubleSlider                      *
//*                                                               *
//*                                                               *
//*  base class for double slider controls for photoshop plugins  *
//*                                                               *
//*****************************************************************

//**********
//contructor
//**********
ttwin::AbstractDoubleSlider::AbstractDoubleSlider() : AbstractSlider(), Pos2(0), UpperSliderSelected(false) {}

//**********
// SetValue2
//**********
void ttwin::AbstractDoubleSlider::SetValue2(double value){
  int oldPos = Pos2;
  Pos2 = PosFromValue(value); 
  if (Pos2 != oldPos){
    RECT rect;
    ::GetClientRect(Hwnd, &rect);
    RECT updateRect = CalcUpdateRect(oldPos, Pos2, rect);
    ::InvalidateRect(Hwnd, &updateRect, TRUE);
  }
}

//**********
// GetValue2
//**********
double ttwin::AbstractDoubleSlider::GetValue2(){
  return ValueFromPos(Pos2);
}

//************************************************
// GetValue2Int
//
// returns the value rounded to the neares integer
//************************************************
int ttwin::AbstractDoubleSlider::GetValue2Int(){
  return static_cast<int>(GetValue2() + 0.5);
}

//----------------------------------- event functions ----------------------------------------

//******************
// CM_Mouse_LeftDown
//******************
void ttwin::AbstractDoubleSlider::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  LeftMouseDown = true;
  int currPos = VERTICAL ? y : x;

  //make that WM_MOUSE_LEAVE messages will be sent
  TRACKMOUSEEVENT event;
  event.cbSize      = sizeof(event);
  event.dwFlags     = TME_LEAVE;
  event.hwndTrack   = Hwnd;
  event.dwHoverTime = HOVER_DEFAULT;
  ::_TrackMouseEvent(&event);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  currPos -= SLIDER_ZEROOFFSET;
  if (currPos > MaxPos) currPos = MaxPos;
  if (currPos < 0)      currPos = 0;
  if (VERTICAL) currPos = MaxPos - currPos;
  UpperSliderSelected = abs(Pos - currPos) > abs(Pos2 - currPos);
 
  if (UpperSliderSelected){
    if (currPos != Pos2){
      RECT updateRect = CalcUpdateRect(currPos, Pos2, rect);
      Pos2 = currPos;
      if (Pos2 < Pos) Pos = Pos2;
      SendHScrollMessage();
      ::InvalidateRect(Hwnd, &updateRect, TRUE);
    }
  }
  else{
    if (currPos != Pos){
      RECT updateRect = CalcUpdateRect(currPos, Pos, rect);
      Pos = currPos;
      if (Pos > Pos2) Pos2 = Pos;
      SendHScrollMessage();
      ::InvalidateRect(Hwnd, &updateRect, TRUE);
    }
  }
}

//**************
// CM_Mouse_Move
//**************
void ttwin::AbstractDoubleSlider::CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt){
  if (LeftMouseDown){
    int currPos = VERTICAL ? y : x;
    RECT rect;
    ::GetClientRect(Hwnd, &rect);
    currPos -= SLIDER_ZEROOFFSET;
    if (currPos > MaxPos) currPos = MaxPos;
    if (currPos < 0)      currPos = 0;
    if (VERTICAL) currPos = MaxPos - currPos;
    if (UpperSliderSelected){
      if (currPos != Pos2){
        RECT updateRect = CalcUpdateRect(currPos, Pos2, rect);
        Pos2 = currPos;
        if (Pos2 < Pos) Pos = Pos2;
        SendHScrollMessage();
        ::InvalidateRect(Hwnd, &updateRect, TRUE);
      }
    }
    else{
      if (currPos != Pos){
        RECT updateRect = CalcUpdateRect(currPos, Pos, rect);
        Pos = currPos;
        if (Pos > Pos2) Pos2 = Pos;
        SendHScrollMessage();
        ::InvalidateRect(Hwnd, &updateRect, TRUE);
      }
    }
  }
}

//*****************************************************************
//*                         SliderControl                         *
//*                                                               *
//*                                                               *
//*        class for slider controls for photoshop plugins        *
//*                                                               *
//*****************************************************************

//**********
//contructor
//**********
ttwin::TTSlider::TTSlider() {
  SLIDER_ZEROOFFSET = 4; //?????
}

//*********
// PaintAll
//*********
void ttwin::TTSlider::PaintAll(){

  const bool isEnabled = IsWindowEnabled(Hwnd) != FALSE;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();

  DrawBackground(hdc, true);    

  HPEN graysliderslot1  = CreatePen(PS_SOLID, 1, RGB( 75,  75,  75));
  HPEN graysliderslot2  = CreatePen(PS_SOLID, 1, RGB( 21,  21,  21));
  HPEN graysliderslot2a = CreatePen(PS_SOLID, 1, RGB(148, 119, 29));
  HPEN graysliderslot3  = CreatePen(PS_SOLID, 1, RGB( 45,  45,  45));
  HPEN graysliderslot3a = CreatePen(PS_SOLID, 1, RGB(193, 155, 37));
  HPEN graysliderslot4  = CreatePen(PS_SOLID, 1, RGB(184, 184, 184));

  HPEN grayslider1 = CreatePen(PS_SOLID, 1, RGB(172, 172, 172));
  HPEN grayslider2 = CreatePen(PS_SOLID, 1, RGB( 74,  74,  74));
  HBRUSH graysliderb2 = CreateSolidBrush(RGB( 74,  74,  74));
  HPEN grayslider3 = CreatePen(PS_SOLID, 1, RGB( 19,  19,  19));
  HPEN grayslider4 = CreatePen(PS_SOLID, 1, RGB( 37,  37,  37));

  HPEN penScale = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

  POINT point;

  const int centerY = rect.bottom - 10;

  //------------------------ draw slider slot ------------------------
  //color1 (does not change)
  OrgPen = SelectObject(hdc, graysliderslot1);
  MoveToEx(hdc, rect.right - 5, centerY - 2, &point);
  LineTo  (hdc, rect.left + 3, centerY - 2); 
  LineTo  (hdc, rect.left + 3, centerY + 1);

  //color4 (does not change)
  SelectObject(hdc, graysliderslot4);
  MoveToEx(hdc, rect.left + 3, centerY + 1, &point);
  LineTo  (hdc, rect.right - 4, centerY + 1); 
  LineTo  (hdc, rect.right - 4, centerY - 3);

  HPEN color2left, color2right, color3left, color3right;
  int  leftStop, rightStop; 
  int  posCenter;

  switch (isEnabled ? Illumination : NONE){
    case NONE:   //ok, its drawn in twice steps but with the same color - this shouldn't hurt
                 color2left  = graysliderslot2;
                 color2right = graysliderslot2;
                 color3left  = graysliderslot3;
                 color3right = graysliderslot3;
                 leftStop  = Pos + 3;
                 rightStop = Pos + 3;
                 break;
    case LEFT:   color2left  = graysliderslot2a;
                 color2right = graysliderslot2;
                 color3left  = graysliderslot3a;
                 color3right = graysliderslot3;
                 leftStop  = Pos + 3;
                 rightStop = Pos + 3;
                 break;
    case RIGHT:  color2left  = graysliderslot2;
                 color2right = graysliderslot2a;
                 color3left  = graysliderslot3;
                 color3right = graysliderslot3a;
                 leftStop  = Pos + 3;
                 rightStop = Pos + 3;
                 break;
    case CENTER: color2left  = graysliderslot2;
                 color2right = graysliderslot2;
                 color3left  = graysliderslot3;
                 color3right = graysliderslot3;
                 posCenter = MaxPos / 2;
                 leftStop  = min(Pos, posCenter) + 3;
                 rightStop = max(Pos, posCenter) + 3;
                 break;
    case ZERO:   color2left  = graysliderslot2;
                 color2right = graysliderslot2;
                 color3left  = graysliderslot3;
                 color3right = graysliderslot3;
                 posCenter = PosFromValue(0.0);
                 leftStop  = min(Pos, posCenter) + 3;
                 rightStop = max(Pos, posCenter) + 3;
                 break;
  }


  //color2 left side
  SelectObject(hdc, color2left);
  MoveToEx(hdc, leftStop, centerY - 1, &point);
  LineTo  (hdc, rect.left + 3, centerY - 1); 

  //color3 left side
  SelectObject(hdc, color3left);
  MoveToEx(hdc, rect.left + 4, centerY, &point);
  LineTo  (hdc, leftStop, centerY); 

  //color2 right side
  SelectObject(hdc, color2right);
  MoveToEx(hdc, rect.right - 6, centerY - 1, &point);
  LineTo  (hdc, rightStop, centerY - 1); 

  //color3 right side
  SelectObject(hdc, color3right);
  MoveToEx(hdc, rightStop, centerY, &point);
  LineTo  (hdc, rect.right - 5, centerY); 
  LineTo  (hdc, rect.right - 5, centerY - 2);

  if (leftStop != rightStop){
    //color 2 center
    SelectObject(hdc, graysliderslot2a);
    MoveToEx(hdc, rightStop, centerY - 1, &point);
    LineTo  (hdc, leftStop, centerY - 1); 

    //color 3 center
    SelectObject(hdc, graysliderslot3a);
    MoveToEx(hdc, rightStop, centerY, &point);
    LineTo  (hdc, leftStop, centerY); 
  }

  //-------------------------- draw label ---------------------------
  HFONT labelFont = CreateFont(12,                        // height of font
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
                               TEXT("MS Shell Dlg"));     // typeface name

  SetBkMode(hdc, TRANSPARENT);
  OrgFont = SelectObject(hdc, labelFont);
  SetTextAlign(hdc, TA_LEFT);
  SetTextAlign(hdc, TA_BOTTOM);
  SetTextColor(hdc, RGB(255, 255, 255));
  TextOut(hdc, rect.left + 3, centerY - 4, TEXT(Label.c_str()), static_cast<int>(Label.size()));
  SelectObject(hdc, OrgFont);
  DeleteObject(labelFont);

  //----------------------- draw scale ------------------------------
  const int scalelong  = 4;
  const int scaleshort = 2;
  const int distToCenter = 3;

  if (ScaleDiv >= 3){
    SelectObject(hdc, penScale);

    double value;
    int pos;
    int length; 
    for (unsigned int i = 0; i < ScaleDiv; i++){
      value = RangeMin + static_cast<double>(i) * ((RangeMax - RangeMin) / static_cast<double>(ScaleDiv - 1));
      pos = PosFromValue(value); 
      length = i == 0 || i == (ScaleDiv - 1) || i == (ScaleDiv / 2) ? scalelong : scaleshort;
      MoveToEx(hdc, rect.left + SLIDER_ZEROOFFSET + pos, centerY + distToCenter, &point);
      LineTo  (hdc, rect.left + SLIDER_ZEROOFFSET + pos, centerY + distToCenter + length); 
    }
  }

  //----------------------- draw slider knop ------------------------
  SelectObject(hdc, grayslider1);
  MoveToEx(hdc, rect.left + 1 + Pos, centerY + 3, &point);
  LineTo  (hdc, rect.left + 1 + Pos, centerY - 4); 
  LineTo  (hdc, rect.left + 7 + Pos, centerY - 4);

  SelectObject(hdc, grayslider2);
  OrgBrush = SelectObject(hdc, graysliderb2);
  Rectangle(hdc, rect.left + 2 + Pos, centerY - 3, rect.left + 6 + Pos, centerY + 3);

  SelectObject(hdc, grayslider3);
  MoveToEx(hdc, rect.left + 2 + Pos, centerY + 3, &point);
  LineTo  (hdc, rect.left + 6 + Pos, centerY + 3); 
  LineTo  (hdc, rect.left + 6 + Pos, centerY - 4);

  SelectObject(hdc, grayslider4);
  MoveToEx(hdc, rect.left + 7 + Pos, centerY + 3, &point);
  LineTo  (hdc, rect.left + 7 + Pos, centerY - 4); 

  //--------------------- draw LED of slider knop ---------------------

  if (isEnabled){
    HPEN redLED = CreatePen(PS_SOLID, 1, RGB(175, 36, 32));
    SelectObject(hdc, redLED);
    MoveToEx(hdc, rect.left + 3 + Pos, centerY - 2, &point);
    LineTo  (hdc, rect.left + 5 + Pos, centerY - 2); 
    MoveToEx(hdc, rect.left + 2 + Pos, centerY - 1, &point);
    LineTo  (hdc, rect.left + 2 + Pos, centerY + 1); 
    DeleteObject(redLED); 
    redLED = CreatePen(PS_SOLID, 1, RGB(186, 32, 27));
    SelectObject(hdc, redLED);
    MoveToEx(hdc, rect.left + 3 + Pos, centerY + 1, &point);
    LineTo  (hdc, rect.left + 5 + Pos, centerY + 1); 
    MoveToEx(hdc, rect.left + 5 + Pos, centerY - 1, &point);
    LineTo  (hdc, rect.left + 5 + Pos, centerY + 1); 
    DeleteObject(redLED); 
    SetPixel(hdc, rect.left + 3 + Pos, centerY,     RGB(252, 60, 54));
    SetPixel(hdc, rect.left + 4 + Pos, centerY - 1, RGB(252, 60, 54));
    SetPixel(hdc, rect.left + 3 + Pos, centerY - 1, RGB(253, 125, 121));
    SetPixel(hdc, rect.left + 4 + Pos, centerY,     RGB(252, 60, 54));
    DeleteObject(redLED);  
  }

  SelectObject(hdc, OrgPen);
  SelectObject(hdc, OrgBrush);

  DeleteObject(penScale);
  DeleteObject(grayslider4); 
  DeleteObject(grayslider3); 
  DeleteObject(graysliderb2);
  DeleteObject(grayslider2); 
  DeleteObject(grayslider1); 

  DeleteObject(graysliderslot4); 
  DeleteObject(graysliderslot3a); 
  DeleteObject(graysliderslot3); 
  DeleteObject(graysliderslot2a); 
  DeleteObject(graysliderslot2); 
  DeleteObject(graysliderslot1); 
  
  EndPaint(Hwnd, &ps);
}

//***************************************************
// CalcUpdateRect
//
// gets the rectandle to update when slider has moved
//***************************************************
RECT ttwin::TTSlider::CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect){
  RECT rect;
  if (pos1 > pos2){
     int temp = pos2;
     pos2 = pos1;
     pos1 = temp;
  }
  //values empirical estimated (more or less...)
  rect.left   = controllerrect.left + pos1 - 4;
  rect.right  = controllerrect.left + pos2 + 8;
  rect.top    = controllerrect.bottom - 19;
  rect.bottom = controllerrect.bottom - 5;
  return rect;
}

//*****************************************************************
//*                          TunerControl                         *
//*                                                               *
//*                                                               *
//*        class for slider controls for photoshop plugins        *
//*        like a tuner wheel                                     *
//*****************************************************************

//**********
//contructor
//**********
ttwin::TTTuner::TTTuner() {
  SLIDER_ZEROOFFSET = 2;
}

//*********
// PaintAll
//*********
void ttwin::TTTuner::PaintAll(){

  POINT point; 

  const bool isEnabled = IsWindowEnabled(Hwnd) != FALSE;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();

  //creates the memory dc if it does not exist yet 
  CreateBackDrawDC(hdc, rect);

  DrawBackground(hdc, true);

  HPEN penSlot1  = CreatePen(PS_SOLID, 1, RGB(129, 129, 129));
  HPEN penSlot2  = CreatePen(PS_SOLID, 1, RGB( 74,  74,  74));
  HPEN penSlot3  = CreatePen(PS_SOLID, 1, RGB(  0,   0,   0));

  HPEN penMark1  = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
  HPEN penScale  = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

  const int centerY = rect.bottom - 10;

  //--------------------------- draw scale ------------------------------
  //need not to be drawn every time
  const int scalelong  = 4;
  const int scaleshort = 2;
  const int baseline   = scalelong + rect.top + 3;

  if (ScaleDiv >= 3){
    OrgPen = SelectObject(hdc, penScale);
    double value;
    int pos;
    int length; 

    //linear scale has different scaling behaviour
    if (Type == LINSCALE){
      const unsigned int start = static_cast<unsigned int>((1.0 / RangeMin) + 0.5);
      for (unsigned int i = start; i > 1; --i){
        pos = PosFromValue(1.0 / static_cast<double>(i));
        length = i == start ? scalelong : scaleshort;
        MoveToEx(hdc, rect.left + SLIDER_ZEROOFFSET + pos, baseline, &point);
        LineTo  (hdc, rect.left + SLIDER_ZEROOFFSET + pos, baseline - length); 
      }
      for (unsigned int i = 1; i <= RangeMax; ++i){
        pos = PosFromValue(static_cast<double>(i));
        length = i == 1 || i == RangeMax? scalelong : scaleshort;
        MoveToEx(hdc, rect.left + SLIDER_ZEROOFFSET + pos, baseline, &point);
        LineTo  (hdc, rect.left + SLIDER_ZEROOFFSET + pos, baseline - length); 
      }
    }
    //draw scaling for all other
    else{
      for (unsigned int i = 0; i < ScaleDiv; i++){
        value = RangeMin + static_cast<double>(i) * ((RangeMax - RangeMin) / static_cast<double>(ScaleDiv - 1));
        pos = PosFromValue(value); 
        length = i == 0 || i == (ScaleDiv - 1) || i == (ScaleDiv / 2) ? scalelong : scaleshort;
        MoveToEx(hdc, rect.left + SLIDER_ZEROOFFSET + pos, baseline, &point);
        LineTo  (hdc, rect.left + SLIDER_ZEROOFFSET + pos, baseline - length); 
      }
    }
    SelectObject(hdc, OrgPen);
  }

  //-------------------------- draw wheelslot ------------------------
  //need not to be drawn every time
  const int height = rect.bottom - baseline - 3;

  OrgPen = SelectObject(hdc, penSlot3);
  HGDIOBJ orgBrush = SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)); 
  Rectangle(hdc, rect.left + 1, rect.bottom - height + 1, rect.right - 1, rect.bottom - 1);
  SelectObject(hdc, orgBrush);
  SelectObject(hdc, penSlot1);
  MoveToEx(hdc, rect.left, rect.bottom - 1, &point);
  LineTo  (hdc, rect.right - 1, rect.bottom - 1); 
  LineTo  (hdc, rect.right - 1, rect.bottom - height - 1); 
  SelectObject(hdc, penSlot2);
  MoveToEx(hdc, rect.left, rect.bottom - 2, &point);
  LineTo  (hdc, rect.left, rect.bottom - height); 
  LineTo  (hdc, rect.right - 1, rect.bottom - height); 

  //restore orgPen in hdc at last
  SelectObject(hdc, OrgPen);

  HDC currentDC = BackDrawDC;

  //--------------------------- draw wheel ---------------------------
  //has to be repainted every time

  const unsigned int PERIODE = 34; //3.4 * 10 (to have integer values)
  const unsigned int periode_Half = PERIODE / 2;
  const int maxPosHalf = MaxPos / 2;
   
  int d; 
  unsigned int Y;
  double round; 
  for (int x = 0; x < MaxPos - 1; x++){
    round = 1.0 - static_cast<double>(abs(maxPosHalf - x)) / static_cast<double>(maxPosHalf);
    d = (abs(Pos - x) * 10) % PERIODE;
    if (d > periode_Half) d = PERIODE - d;
    Y = static_cast<unsigned int>(180.0 * round * static_cast<double>(d) / static_cast<double>(periode_Half));
    
    if (Y > 255) Y = 255;
    HPEN penWheel  = CreatePen(PS_SOLID, 1, RGB(Y, Y, Y));
    OrgPen = SelectObject(currentDC, penWheel);
    MoveToEx(currentDC, x + SLIDER_ZEROOFFSET, rect.bottom - 3, &point);
    LineTo  (currentDC, x + SLIDER_ZEROOFFSET, rect.bottom - height + 1); 
    SelectObject(currentDC, OrgPen);
    DeleteObject(penWheel); 
  }

  //--------------------------- draw mark ----------------------------
  //has to be repainted every time

  OrgPen = SelectObject(currentDC, penMark1);
  MoveToEx(currentDC, rect.left + 2 + Pos, rect.bottom - height + 2, &point);
  LineTo  (currentDC, rect.left + 2 + Pos, rect.bottom - 2); 
  SelectObject(currentDC, OrgPen);

  //----------------- copy background drawing to normal DC ---------------
  const int left   = SLIDER_ZEROOFFSET;
  const int right  = left + MaxPos;
  const int bottom = rect.bottom - 2;
  const int top    = bottom - height + 4;
  BitBlt(hdc, left, top, right - left, bottom - top, BackDrawDC, left, top, SRCCOPY);
  
  DeleteObject(penScale);
  DeleteObject(penMark1);
  DeleteObject(penSlot3);
  DeleteObject(penSlot2);
  DeleteObject(penSlot1);
 
  EndPaint(Hwnd, &ps);

}

//***************************************************
// CalcUpdateRect
//
// gets the rectandle to update when slider has moved
//***************************************************
RECT ttwin::TTTuner::CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect){
  RECT rect;
  if (pos1 > pos2){
     int temp = pos2;
     pos2 = pos1;
     pos1 = temp;
  }
 
  rect.left   = controllerrect.left;
  rect.right  = controllerrect.right;
  rect.top    = controllerrect.top;
  rect.bottom = controllerrect.bottom;
  return rect;
}



