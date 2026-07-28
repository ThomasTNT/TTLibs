#include "TTTransportButton.h"

#include "Logger.h"

//*****************************************************************
//*                        TransportButton                        *
//*                                                               *
//*                                                               *
//*             class for big illumated transport buttons         *
//*                                                               *
//*****************************************************************


//**********
//contructor
//**********
ttwin::TTTransportButton::TTTransportButton() : TTWin("TransportButton"), 
                                                State(ENABLED), 
                                                Pressed(false),
                                                BmpNormal(0),
                                                BmpActive(0),
                                                BmpSelected(0),
                                                BmpDisabled(0)
{
                                             
}

//**********
//destructor
//**********
ttwin::TTTransportButton::~TTTransportButton(){}

//**********************
// Configure
//
// configures the button
//**********************
void ttwin::TTTransportButton::Configure(const int idbmpNormal, const int idbmpActive, const int idbmpSelected, const int idbmpDisabled){
  BmpNormal   = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(idbmpNormal));
  BmpActive   = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(idbmpActive));
  BmpSelected = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(idbmpSelected));
  BmpDisabled = ::LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(idbmpDisabled));;
}

//**********
// SetStar
//**********
void ttwin::TTTransportButton::SetButtonState(const TransportButtonState state){
  if (State != state){
    State = state;
    Invalidate();
    //Update(); //update causes a deadlock when FillAudioBuffer is running 
                // (it calls SetButtonState indirectyl via call back.)
                //and LoadNewSong runs at the same time and want's to 
                //enter the Critical Section update causes a deadlock ... 
  }
}

//**********
// GetActive
//**********
ttwin::TransportButtonState ttwin::TTTransportButton::GetButtonState(){
  return State;
}


//*************************************
// Created 
//
// this is called after creation
// hack for calling Init()
// Why is there no WM_CREATE Message?!?
//*************************************
void ttwin::TTTransportButton::Created(){
  Init(); //just call init
}

//*******
// Init
//*******
void ttwin::TTTransportButton::Init(){
  State  = ENABLED;
  Pressed = false;
}


//*************************************************
// InitGraphic
//
// this function initializes all graphical elements
//*************************************************
//void TransportButton::InitGraphic(){
 
  
//}


//*********
// PaintAll
//*********
void ttwin::TTTransportButton::PaintAll(){

  const int offset = Pressed ? 2 : 0;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();

  DrawBackground(hdc, false);

  HBITMAP bmpToDraw;
  switch(State) {
    case ENABLED: bmpToDraw = BmpNormal; break; 
    case SELECTED: bmpToDraw = BmpSelected; break;
    case ACTIVE: bmpToDraw = BmpActive; break;
    default: bmpToDraw = BmpDisabled; break;
  }
  if (Pressed) bmpToDraw = BmpActive;

  HDC hdcmem = ::CreateCompatibleDC(hdc);
  HBITMAP hbmOld = reinterpret_cast<HBITMAP>(::SelectObject(hdcmem, bmpToDraw));

  ::BitBlt(hdc, rect.left + offset, rect.top + offset, rect.right - rect.left, rect.bottom - rect.top, hdcmem, rect.left, rect.top, SRCCOPY);
  
  SelectObject(hdcmem, hbmOld);
  DeleteDC(hdcmem);//??? destroy here everytime? 

  POINT point;
  HPEN pen1;

  pen1 = CreatePen(PS_SOLID, 1, RGB(129, 129, 129));
  
  OrgPen = SelectObject(hdc, pen1);
  MoveToEx(hdc, rect.left,   rect.top, &point);
  LineTo  (hdc, rect.right,  rect.top); 
  LineTo  (hdc, rect.right,  rect.bottom); 
  LineTo  (hdc, rect.left,   rect.bottom); 
  LineTo  (hdc, rect.left,   rect.top); 
  
  DeleteObject(pen1);

  EndPaint(Hwnd, &ps);
}


//******************
// CM_Mouse_LeftDown
//******************
void ttwin::TTTransportButton::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  //if (EnabledWhenInactive || Active != 0){
  if (State != ttwin::DISABLED){
    Pressed = true;
    InvalidateRect(Hwnd, NULL, TRUE);
  }
}


//****************
// CM_Mouse_LeftUp
//****************
void ttwin::TTTransportButton::CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){
  //if (EnabledWhenInactive || Active != 0){
  if (State != ttwin::DISABLED){
    Pressed = false;
    SendClickMessage();
    InvalidateRect(Hwnd, NULL, TRUE);
  }
}

//************************************************
// SendClickMessage
//
// send a WM_COMMAND message like standard sliders
//************************************************
void ttwin::TTTransportButton::SendClickMessage(){
  SendMessage(Parent,     //HWND to which the message should be sent, our parent of course 
              WM_COMMAND, //the message 
              MAKEWPARAM(ID, BN_CLICKED), //wParam
              reinterpret_cast<LPARAM>(Hwnd)); //the window handle is strored in lParam 
}