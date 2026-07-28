#include "TTFader.h"

#include "Logger.h"

//*****************************************************************
//*                           TTFader                             *
//*                                                               *
//*                                                               *
//*                   control for a mixer fader                   *
//*                                                               *
//*****************************************************************

//**********
//contructor
//**********
ttwin::TTFader::TTFader() : TTWin("TTFader"),
                            Pos(0), 
                            MaxPos(0),
                            LeftMouseDown(false),
                            ZeroOffset(0),
                            BmpFader(0),
                            BmpSlit(0),
                            FaderBmpWidth(0),
                            FaderBmpHeight(0),
                            FaderSlitBorder(0),
                            FaderSlitWidth(0),
                            FaderSlitScale(0),
                            StepValue(0.0),
                            StepBase(0.0),
                            ScaleMax(0.0),
                            ScaleMin(0.0),
                            StepsPlus(0.0),
                            StepsMinus(0.0),
                            StepSize(0.0),
                            PosZero(0)
                            
{
                                    
}


//**********************
// Configure
//
// configures the slider
//**********************
void ttwin::TTFader::Configure(const double stepValue, const double stepBase, const double scaleMax, const double scaleMin, const int snapRange)
{
  StepValue = stepValue;
  StepBase = stepBase;
  ScaleMax = scaleMax;
  ScaleMin = scaleMin;
  CalculateSteps();
}

//************************************
// Created 
//
// this is called after creation
// hack for calling Init()
// Why is ther no WM_CREATE Message?!?
//************************************
void ttwin::TTFader::Created(){
  Init(); //just call init
}

//*******
// Init
//*******
void ttwin::TTFader::Init(){
  const RECT rect = GetClientRect();
  const int height = rect.bottom - rect.top;
  LeftMouseDown = false;

  BmpFader = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_FADER_SMALL));
  FaderBmpWidth = 17; 
  FaderBmpHeight = 32; 
  BmpSlit  = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_FADERSLIT_NORMAL));
  FaderSlitWidth = 8;
  FaderSlitBorder = 2;
  FaderSlitScale = 182 - FaderSlitBorder - FaderSlitBorder;

  ZeroOffset = FaderBmpHeight / 2 + 1;
  MaxPos = height - 2 * ZeroOffset;
 
}


//*********
// SetValue
//*********
void ttwin::TTFader::SetValue(double value){
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
double ttwin::TTFader::GetValue(){
  return ValueFromPos(Pos);
}

//************************************************
// GetValueInt
//
// returns the value rounded to the neares integer
//************************************************
int ttwin::TTFader::GetValueInt(){
  return static_cast<int>(GetValue() + 0.5);
}


//*************
// PosFromValue
//*************
int ttwin::TTFader::PosFromValue(double value){
  return PosBydB(value);
}

//*************
// ValueFromPos
//*************
double ttwin::TTFader::ValueFromPos(int pos){
  return DBbyPos(pos);
}

//*********
// PaintAll
//*********
void ttwin::TTFader::PaintAll(){

  const bool isEnabled = IsWindowEnabled(Hwnd) != FALSE;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();
  const int width = rect.right - rect.left;
  const int Xmid = rect.left + width / 2;

  DrawBackground(hdc, true);    

  //scale
  HFONT scaleFont = CreateFont(10,                        // height of font
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

  ::SetBkMode(hdc, TRANSPARENT);
  OrgFont = SelectObject(hdc, scaleFont);
  ::SetTextAlign(hdc, TA_BASELINE);
  //::SetTextAlign(hdc, TA_CENTER);
  ::SetTextColor(hdc, RGB(255, 255, 255));
  
  HPEN penScale = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
  POINT point;
  ::SelectObject(hdc, penScale);

  for (unsigned int step = 0; step <= StepsPlus; ++step){
    const int y = YByPos(PosZero + static_cast<int>(step * StepSize + 0.5));
    ::MoveToEx(hdc, 10 + 4, y, &point);
    ::LineTo  (hdc, Xmid - FaderSlitWidth / 2 - 2, y); 
    ::MoveToEx(hdc, Xmid + FaderSlitWidth / 2 + 2, y, &point);
    ::LineTo  (hdc, Xmid + FaderSlitWidth / 2 + 2 + 10, y); 
    ::SelectObject(hdc, scaleFont);
    const double dB = DBByStep(step);
    std::string sl = step == 0 ? "0" : "+" + ttutil::StringUtil::double2string(dB, 0);
    sl = ttutil::StringUtil::formatRight(sl, 3, ' ');
    ::TextOut(hdc, 0, y + 4, sl.c_str(), static_cast<int>(sl.size()));
  }
  for (unsigned int step = 1; step <= StepsMinus; ++step){
    const int y = YByPos(PosZero - static_cast<int>(step * StepSize + 0.5));
    ::MoveToEx(hdc, 10 + 3, y, &point);
    ::LineTo  (hdc, Xmid - FaderSlitWidth / 2 - 2, y); 
    ::MoveToEx(hdc, Xmid + FaderSlitWidth / 2 + 2, y, &point);
    ::LineTo  (hdc, Xmid + FaderSlitWidth / 2 + 2 + 10, y); 
    ::SelectObject(hdc, scaleFont);
    const double dB = DBByStep(step);
    std::string sl = step >= StepsMinus ? "inf" : "-" + ttutil::StringUtil::double2string(dB, 0);
    sl = ttutil::StringUtil::formatRight(sl, 3, ' ');
    ::TextOut(hdc, 0, y + 4, sl.c_str(), static_cast<int>(sl.size()));
  }

  ::SelectObject(hdc, OrgFont);
  ::DeleteObject(scaleFont);

  ::DeleteObject(penScale);


  HDC hdcmem = ::CreateCompatibleDC(hdc);
  
  //fader
  ::SelectObject(hdcmem, BmpFader);
  const int yFader = rect.bottom - ZeroOffset - Pos - FaderBmpHeight / 2;
  ::BitBlt(hdc, Xmid - FaderBmpWidth / 2, yFader, FaderBmpWidth, FaderBmpHeight, hdcmem, 0, 0, SRCCOPY);

  //slit
  ::SelectObject(hdcmem, BmpSlit);
  const int yUpper0 = rect.bottom - ZeroOffset - MaxPos - FaderSlitBorder;
  const int yUpper1 = yFader;
  if (yUpper1 > yUpper0){
    const int upperLength = yUpper1 - yUpper0;
    ::BitBlt(hdc, Xmid - FaderSlitWidth / 2, yUpper0, FaderSlitWidth, min(upperLength, FaderSlitBorder), hdcmem, 0, 0, SRCCOPY);
    if (upperLength > FaderSlitBorder){
      const int lengthToDraw = upperLength - FaderSlitBorder;
      const int maxLengthToDraw = MaxPos - 2 * FaderSlitBorder;
      const int cutOut = static_cast<int>(static_cast<double>(FaderSlitScale) * static_cast<double>(lengthToDraw) / static_cast<double>(maxLengthToDraw) + 0.5);
      ::StretchBlt(hdc, Xmid - FaderSlitWidth / 2, yUpper0 + FaderSlitBorder, FaderSlitWidth, lengthToDraw, 
                   hdcmem, 0, FaderSlitBorder, FaderSlitWidth, cutOut, 
                   SRCCOPY);
    }
  }

  const int ylower0 = rect.bottom - ZeroOffset + FaderSlitBorder;
  const int ylower1 = yFader + FaderBmpHeight;
  if (ylower1 < ylower0){
    const int lowerLength = ylower0 - ylower1;
    const int borderToDraw = min(lowerLength, FaderSlitBorder);
    ::BitBlt(hdc, Xmid - FaderSlitWidth / 2, ylower0 - borderToDraw, FaderSlitWidth, borderToDraw, hdcmem, 0, FaderSlitScale + FaderSlitBorder, SRCCOPY);
    if (lowerLength > FaderSlitBorder){
      const int lengthToDraw = lowerLength - FaderSlitBorder;
      const int maxLengthToDraw = MaxPos - 2 * FaderSlitBorder;
      const int cutOut = static_cast<int>(static_cast<double>(FaderSlitScale) * static_cast<double>(lengthToDraw) / static_cast<double>(maxLengthToDraw) + 0.5);
      ::StretchBlt(hdc, Xmid - FaderSlitWidth / 2, ylower1, FaderSlitWidth, lengthToDraw, 
                     hdcmem, 0, FaderSlitScale + FaderSlitBorder - cutOut, FaderSlitWidth, cutOut, 
                     SRCCOPY);  
    }
  }

  DeleteDC(hdcmem);//??? destroy here everytime? 


  EndPaint(Hwnd, &ps);
}
//----------------------------------- event functions ----------------------------------------

//******************
// CM_Mouse_LeftDown
//******************
void ttwin::TTFader::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  LeftMouseDown = true;

  const RECT rect = GetClientRect();
  const int currPos = PosByY(y);
  
  //make that WM_MOUSE_LEAVE messages will be sent
  TRACKMOUSEEVENT event;
  event.cbSize      = sizeof(event);
  event.dwFlags     = TME_LEAVE;
  event.hwndTrack   = Hwnd;
  event.dwHoverTime = HOVER_DEFAULT;
  _TrackMouseEvent(&event);

  ::SetCapture(Hwnd);

  if (currPos != Pos){
    //currPos = SnapToGrid(Pos, currPos);
    RECT updateRect = CalcUpdateRect(currPos, Pos, rect);
    Pos = currPos;
    SendHScrollMessage();
    InvalidateRect(Hwnd, &updateRect, TRUE);
  }
  Logger::getInstance().println("CM_Mouse_LeftDown x: " + ttutil::StringUtil::int2string(x) + " currPos: " + ttutil::StringUtil::int2string(currPos));
}

//****************
// CM_Mouse_LeftUp
//****************
void ttwin::TTFader::CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){
  LeftMouseDown = false;
  ::ReleaseCapture();
}

//**************
// CM_Mouse_Move
//**************
void ttwin::TTFader::CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt){
  if (LeftMouseDown){

    const RECT rect = GetClientRect();
    const int currPos = PosByY(y);
    if (currPos != Pos){
      //currPos = SnapToGrid(Pos, currPos);
      RECT updateRect = CalcUpdateRect(currPos, Pos, rect);
      Pos = currPos;
      SendHScrollMessage();
      InvalidateRect(Hwnd, &updateRect, TRUE);
    }
  }
}

//***************
// CM_Mouse_Leave
//***************
void ttwin::TTFader::CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt){
  //we capture the mouse, to theres no need to check for mouse leave
  //LeftMouseDown = false;
}

//***************************************************
// CalcUpdateRect
//
// gets the rectandle to update when slider has moved
//***************************************************
RECT ttwin::TTFader::CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect){
  
  if (pos1 > pos2){
     int temp = pos2;
     pos2 = pos1;
     pos1 = temp;
  }
  const int width = controllerrect.right - controllerrect.left;
  const int Xmid = controllerrect.left + width / 2;
  RECT rect = controllerrect;
  rect.left   = Xmid - FaderBmpWidth / 2;
  rect.right  = rect.left + FaderBmpWidth;
  //rect.top    = controllerrect.bottom - pos1 - ZeroOffset - FaderBmpHeight / 2 - 2;
  //rect.bottom = controllerrect.bottom - pos2 - ZeroOffset + FaderBmpHeight / 2 + 2;
  return rect;
}


//************************************************
// SendHScrollMessage
//
// send a WM_HSCROLL message like standard sliders
//************************************************
void ttwin::TTFader::SendHScrollMessage(){
  ::SendMessage(Parent,     //HWND to which the message should be sent, our parent of course 
                WM_HSCROLL, //the message 
                0,          //wParam is not used in our main window
                reinterpret_cast<LPARAM>(Hwnd)); //the window handle is strored in lParam 
}

//*********************
// HELP: CalculateSteps
//*********************
void ttwin::TTFader::CalculateSteps(){

  if (ScaleMax < 0.0) ScaleMax = 0.0;
  if (ScaleMax == 0.0) {
    StepsPlus = 0.0;
  }
  else{
    StepsPlus = StepBydB(ScaleMax);
  }

  if (ScaleMin > 0.0) ScaleMin = 0.0;
  if (ScaleMin == 0.0) {
    StepsMinus = 0.0;
  }
  else{
    StepsMinus = StepBydB(-ScaleMin);
  }

  StepSize = static_cast<double>(MaxPos) / static_cast<double>(StepsMinus + StepsPlus);
  PosZero = MaxPos - static_cast<int>(StepsPlus * StepSize + 0.5);
}

//***********
// Step -> dB
//***********
double ttwin::TTFader::DBByStep(double step){
  const bool signPositiv = step >= 0.0;
  if (!signPositiv) step = -step;
  const double db = step > 1.0 ? StepValue * pow(StepBase, step - 1.0) : StepValue * step;
  return signPositiv ? db : -db;
}

//***********
// dB -> Step
//***********
double ttwin::TTFader::StepBydB(double db){
  const bool signPositiv = db >= 0.0;
  if (!signPositiv) db = -db;
  const double steps = db > StepValue ? 1.0 + (log(db) - log(StepValue)) / log(StepBase) : db / StepValue;
  return signPositiv ? steps : -steps;
}

//***********
// Pos -> dB
//***********
double ttwin::TTFader::DBbyPos(const int pos){
  if (pos == 0){
    return FADER_DB_NEG_INF;
  }
  else if (pos == PosZero){
    return 0.0;
  }
  else if (pos > PosZero){
    const double steps = static_cast<double>(pos - PosZero) / StepSize;
    const double dB = DBByStep(steps);
    return dB;
  }
  else{
    const double steps = static_cast<double>(PosZero - pos) / StepSize;
    const double dB = -DBByStep(steps);
    return dB;
  }

}

//***********
// dB -> Pos
//***********
int ttwin::TTFader::PosBydB(const double db){
  if (db == 0.0){
    return PosZero;
  }
  else if (db > 0.0){
    const double steps = StepBydB(db);
    return PosZero + static_cast<int>(steps * StepSize + 0.5);
  }
  else{
    const double steps = StepBydB(-db);
    return PosZero - static_cast<int>(steps * StepSize + 0.5);
  }
}

//*************************************************************************
// y -> Pos
// NOTE: this works because ZeroOffset is even on top, not only from bottom
//*************************************************************************
int ttwin::TTFader::PosByY(const int y){
  int pos = MaxPos + ZeroOffset - y;
  if (pos > MaxPos) pos = MaxPos;
  if (pos < 0) pos = 0;
  return pos; 
}

//***********
// Pos -> y
//***********
int ttwin::TTFader::YByPos(const int pos){
  return MaxPos + ZeroOffset - pos;
}
