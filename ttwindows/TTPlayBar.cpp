#include "TTPlayBar.h"


//*****************************************************************************
//*                                  TTPlayBar                                *
//*                                                                           *
//*                                                                           *
//*               class for showing a bar for audio or video playing          *
//*                                                                           *
//*****************************************************************************

//********************************
// constructor 
//
// does nothing but initialisation
//********************************
ttwin::TTPlayBar::TTPlayBar() : TTWin("TTPlayBar"), 
                                PlayPos(0), 
                                MaxValue(1.0), MinValue(0.0), Length(1.0), Start(0.0), 
                                PlayValue(0.0),
                                LeftMousePressed(false),
                                MarkerSelected(0)
{
}


//***********
// destructor 
//***********
ttwin::TTPlayBar::~TTPlayBar(){}

//*******************
// Configure
//
// configures the bar
//*******************
void ttwin::TTPlayBar::Configure(const double min, 
                                 const double max, 
                                 const double start, 
                                 const double length)
{
  SetMin(min);
  SetMax(max);
  SetStart(start);
  SetLength(length);
}

//*****************************
// SetMin
//
// sets the minimum scale value
//*****************************
void ttwin::TTPlayBar::SetMin(const double min){
  MinValue = min;
}

//*****************************
// SetMax
//
// sets the maximum scale value
//*****************************
void ttwin::TTPlayBar::SetMax(const double max){
  MaxValue = max;
}

//**************************
// SetLength
//
// sets the actual end value
//**************************
void ttwin::TTPlayBar::SetLength(const double length){
  Length = length;
}

//*******************************************
// StartValue
//
// sets indirectly the offset of video to min
//*******************************************
void ttwin::TTPlayBar::SetStart(const double start){
  Start = start;
}

//******************
// SetPlayValue
//
// set play position
//******************
void ttwin::TTPlayBar::SetPlayValue(double playvalue){
  if (playvalue < Start) playvalue = Start;
  if (playvalue > Start + Length) playvalue = Start + Length;
  RECT updateRect = CalcUpdateRect(PlayValue, playvalue);
  PlayValue = playvalue;
  ::InvalidateRect(Hwnd, &updateRect, TRUE);
}

//********************
//SetMarkerValues
//
// set marker position
//********************
void ttwin::TTPlayBar::SetMarkerValues(const std::vector<double>& marker){
  Marker = marker;
}

//********************
//SetMarkerValues
//
// set marker position
//********************
void ttwin::TTPlayBar::SetTransportMarkerValues(const std::vector<double>& transportMarker){
  TransportMarker = transportMarker;
}

//********************
//SetMarkerSelected
//
// set selected marker
//********************
void ttwin::TTPlayBar::SetMarkerSelected(const unsigned int index){
  MarkerSelected = index;
}

//************************************************************
// Created
//
// called after creation as a workaround for missing WM_CREATE
// where normally Init() is called
//************************************************************
void ttwin::TTPlayBar::Created(){
 Init();
 InitGraphic();
}


//******
// Init
//******
void ttwin::TTPlayBar::Init(){
  LeftMousePressed = false;
}

//************
// InitGraphic
//************
void ttwin::TTPlayBar::InitGraphic(){

  barBrightLeft  = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_BRIGHT_LEFT, IDB_BARMASK_BRIGHT_LEFT,  BAR_WIDTH_LEFT,  BAR_HEIGHT);
  barBrightRight = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_BRIGHT_RIGHT, IDB_BARMASK_BRIGHT_RIGHT, BAR_WIDTH_RIGHT, BAR_HEIGHT);
  barBrightkMid  = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_BRIGHT_MID,   IDB_BARMASK_DARK_MID,   BAR_WIDTH_MID,   BAR_HEIGHT);

  barDarkLeft  = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_DARK_LEFT,  IDB_BARMASK_DARK_LEFT,  BAR_WIDTH_LEFT,  BAR_HEIGHT);
  barDarkRight = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_DARK_RIGHT, IDB_BARMASK_DARK_RIGHT, BAR_WIDTH_RIGHT, BAR_HEIGHT);
  barDarkMid   = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_DARK_MID,   IDB_BARMASK_DARK_MID,   BAR_WIDTH_MID,   BAR_HEIGHT);

  barPlayLeft  = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_PLAY_LEFT,  IDB_BARMASK_PLAY_LEFT,  PLAY_WIDTH_LEFT,  PLAY_HEIGHT);
  barPlayRight = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_PLAY_RIGHT, IDB_BARMASK_PLAY_RIGHT, PLAY_WIDTH_RIGHT, PLAY_HEIGHT);
  barPlaykMid  = ttwin::GetAlphaDibImage(Hinst, GetDC(Hwnd), IDB_BAR_PLAY_MID,   IDB_BARMASK_PLAY_MID,   PLAY_WIDTH_MID,   PLAY_HEIGHT);
}


//*********
// PaintAll
//*********
void ttwin::TTPlayBar::PaintAll(){

  //local config for drawing
  const bool USE_BITMAPS = true;

  //selected or not
  const bool selected = TTWinParent->GetSelected();


  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  const int width  = rect.right - rect.left;
  const int height = rect.bottom - rect.top;

  DrawBackground(hdc, true);   
  POINT point;

  //used pens etc.
  HPEN outline = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255)); 

  HGDIOBJ orgpen = ::SelectObject(hdc, outline);

  const int posX = rect.left;
  const int barH = BAR_HEIGHT;
  const int playH = PLAY_HEIGHT;
  const int barY = (height - barH) / 2;
  const int playY = (height - playH) / 2;

  const int posMin = posX + 0;               //physical Min-position 
  const int posMax = posX + width;           //physical Max-position 
  const int startpos = PhysicalPosByValue(Start, posMin, posMax);
  const int endPos   = PhysicalPosByValue(Start + Length, posMin, posMax);
  const int currPlayPos = PhysicalPosByValue(PlayValue, posMin, posMax);

  if (!USE_BITMAPS){
    //---------------------- normal brushes ------------------------
    HBRUSH brushBar = CreateSolidBrush(RGB(0, 10, 255));
    HBRUSH brushPlay = CreateSolidBrush(RGB(255, 80, 10));

    //from start - play
    OrgBrush = ::SelectObject(hdc, brushBar);
    ::Rectangle(hdc, startpos, barY, currPlayPos, barY + barH);
    ::SelectObject(hdc, brushPlay);
    ::Rectangle(hdc, startpos, playY, currPlayPos, playY + playH);  
  
    //from play - end
    ::SelectObject(hdc, brushBar);
    ::Rectangle(hdc, currPlayPos, barY, endPos, barY + barH);

    ::DeleteObject(brushPlay); 
    ::DeleteObject(brushBar); 
    ::SelectObject(hdc, OrgBrush);
    //---------------------- end of normal brushes -----------------
  }
  else{
    //------------------------- bitmaps ----------------------------

    const HDC memHDC = CreateCompatibleDC(hdc); //TODO every time new or make it global?

    int posright, posmid;

    //BAR
    BlendAlphaDibImage(hdc, memHDC, 
                       selected ? barBrightLeft : barDarkLeft, 
                       startpos - BAR_OFFSET_LEFT, 0,
                       BAR_WIDTH_LEFT, barH,
                       BAR_WIDTH_LEFT, barH);

    BlendAlphaDibImage(hdc, memHDC, 
                       selected ? barBrightRight : barDarkRight, 
                       endPos - BAR_WIDTH_RIGHT + BAR_OFFSET_RIGHT, 0,
                       BAR_WIDTH_RIGHT, barH,
                       BAR_WIDTH_RIGHT, barH);

    BlendAlphaDibImage(hdc, memHDC, 
                       selected ? barBrightkMid : barDarkMid, 
                       startpos + BAR_WIDTH_LEFT - BAR_OFFSET_LEFT, 0, 
                       endPos - BAR_WIDTH_RIGHT + BAR_OFFSET_RIGHT - startpos - BAR_WIDTH_LEFT + BAR_OFFSET_LEFT, barH,
                       BAR_WIDTH_MID, barH);
    
    //PLAY BAR
    if (selected){
      BlendAlphaDibImage(hdc, memHDC, 
                         barPlayLeft, 
                         startpos, playY,
                         PLAY_WIDTH_LEFT, playH,
                         PLAY_WIDTH_LEFT, playH);

      posright = currPlayPos - PLAY_WIDTH_LEFT;
      if (posright < (startpos + PLAY_WIDTH_LEFT)) posright = startpos + PLAY_WIDTH_LEFT;
      BlendAlphaDibImage(hdc, memHDC, 
                         barPlayRight, 
                         posright, playY,
                         PLAY_WIDTH_RIGHT, playH,
                         PLAY_WIDTH_RIGHT, playH);

      posmid = startpos + PLAY_WIDTH_LEFT;
      if (posright > posmid){
        BlendAlphaDibImage(hdc, memHDC, 
                           barPlaykMid, 
                           posmid, playY, 
                           currPlayPos - startpos - PLAY_WIDTH_LEFT - PLAY_WIDTH_RIGHT, playH, 
                           BAR_WIDTH_MID, playH);
      }
    }
    ::DeleteDC(memHDC);

    //----------------------- end of bitmaps -----------------------
  }

  //---------------------------- name -----------------------------------------
  const int namePosX = posX + 10;
  const int namePosY = rect.top + height / 2 + 6;

  HFONT nameFont = CreateFont( 12,                        // height of font
                               0,                         // average character width
                               0,                         // angle of escapement
                               0,                         // base-line orientation angle
                               600,                       // font weight
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
  OrgFont = SelectObject(hdc, nameFont);
  SetTextAlign(hdc, TA_LEFT);
  SetTextAlign(hdc, TA_BOTTOM);
  SetTextColor(hdc, RGB(255, 255, 255));
  TextOut(hdc, namePosX, namePosY, Label.c_str(), static_cast<int>(Label.size()));
  SelectObject(hdc, OrgFont);
  DeleteObject(nameFont);

  //--------------------------------- marker ----------------------------------
  if (selected){
    HPEN markerPenSel = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    HPEN markerPenNor = ::CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
    
    int markerX;
    for (unsigned int im = 0; im < Marker.size(); ++im){
      if ((im + 1) == MarkerSelected){
        ::SelectObject(hdc, markerPenSel);
      }
      else{
        ::SelectObject(hdc, markerPenNor);
      }
      markerX = PhysicalPosByValue(Marker[im], posMin, posMax);
      //::MoveToEx(hdc, markerX, barY, &point);
      //::LineTo  (hdc, markerX, barY + barH);
      ::MoveToEx(hdc, markerX - 2, barY, &point);
      ::LineTo  (hdc, markerX + 2, barY);
      ::LineTo  (hdc, markerX, barY + 5);
      ::LineTo  (hdc, markerX - 2, barY);
    }

    HPEN markerPenStop = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    ::SelectObject(hdc, markerPenStop);
    for (unsigned int im = 0; im < TransportMarker.size(); ++im){
      markerX = PhysicalPosByValue(TransportMarker[im], posMin, posMax);
      ::MoveToEx(hdc, markerX - 2, barY, &point);
      ::LineTo  (hdc, markerX + 2, barY);
      ::LineTo  (hdc, markerX, barY + 5);
      ::LineTo  (hdc, markerX - 2, barY);
    }
    
    ::DeleteObject(markerPenStop);
    ::DeleteObject(markerPenNor); 
    ::DeleteObject(markerPenSel); 
  }

  //select the original pens
  ::SelectObject(hdc, orgpen);

  //delete all objects after use
  ::DeleteObject(outline);  

  //free device context
  ::EndPaint(Hwnd, (LPPAINTSTRUCT)&ps);
}

//***************************************************
// DrawBackground
//
// for drawing the background of the control
// if no background is set a dummy color will be used
//***************************************************
/*void ttwin::TTPlayBar::DrawBackground(HDC hdc, const bool drawDefaultBackground){
  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  const POINT offset = CalcOffsetToMainWin();
  if (HDCMem && BackgroundBitmap){
    ::SelectObject(HDCMem, BackgroundBitmap);
    ::BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, HDCMem, offset.x, offset.y, SRCCOPY);
  }
  else if (drawDefaultBackground){
    HBRUSH brushBackgroundDefault = CreateSolidBrush(RGB(255, 110, 110));
    ::SelectObject(hdc, brushBackgroundDefault);
    ::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    ::DeleteObject(brushBackgroundDefault);    
  }
}*/

//******************
// CM_Mouse_LeftDown
//******************
void ttwin::TTPlayBar::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  
  LeftMousePressed = true;

  //make that WM_MOUSE_LEAVE messages will be sent
  TRACKMOUSEEVENT event;
  event.cbSize      = sizeof(event);
  event.dwFlags     = TME_LEAVE;
  event.hwndTrack   = Hwnd;
  event.dwHoverTime = HOVER_DEFAULT;
  _TrackMouseEvent(&event);

  if (shift){
    ::SetCapture(Hwnd);
    RECT rect;
    ::GetClientRect(Hwnd, &rect);
    const int width  = rect.right - rect.left;
    const int posX   = rect.left;
    const int posMin = posX + 0;               //physical Min-position 
    const int posMax = posX + width;           //physical Max-position 
    const double newValue = ValueByPhysicalPos(x, posMin, posMax);
    const int newValueInt = static_cast<int>(newValue + 0.5);
    SendChildWasClickedAndChangedMsg(newValueInt);
    //SetPlayValue(newValue);
  }
  else{
    SendChildWasClickedMsg(0);
  }
}

//****************
// CM_Mouse_LeftUp
//****************
void ttwin::TTPlayBar::CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){
  LeftMousePressed = false;
  ::ReleaseCapture();
  if (shift){
    RECT rect;
    ::GetClientRect(Hwnd, &rect);
    const int width  = rect.right - rect.left;
    const int posX   = rect.left;
    const int posMin = posX + 0;               //physical Min-position 
    const int posMax = posX + width;           //physical Max-position 
    const double newValue = ValueByPhysicalPos(x, posMin, posMax);
    const int newValueInt = static_cast<int>(newValue + 0.5);
    SendChildWasClickedAndChangedMsg(newValueInt);
    //SetPlayValue(newValue);
  }
  else{
    SendChildWasClickedMsg(0);
  }
}

//******************
// CM_Mouse_LeftDown
//******************
void ttwin::TTPlayBar::CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt){
  if (LeftMousePressed){
    if (shift){
      RECT rect;
      ::GetClientRect(Hwnd, &rect);
      const int width  = rect.right - rect.left;
      const int posX   = rect.left;
      const int posMin = posX + 0;               //physical Min-position 
      const int posMax = posX + width;           //physical Max-position 
      const double newValue = ValueByPhysicalPos(x, posMin, posMax);
      const int newValueInt = static_cast<int>(newValue + 0.5);
      SendChildWasClickedAndChangedMsg(newValueInt);
      //SetPlayValue(newValue);
    }
    else{
      SendChildWasClickedMsg(0);
    }
  }
}

//***************
// CM_Mouse_Leave
//***************
void ttwin::TTPlayBar::CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt){
  LeftMousePressed = false;
  //complete the setting and call for update
}


//*******************
// PhysicalPosByValue
//*******************
int ttwin::TTPlayBar::PhysicalPosByValue(const double value, const int posMin, const int posMax){
  return static_cast<int>(  (value - MinValue) * static_cast<double>(posMax - posMin) / (MaxValue - MinValue)   
                           + static_cast<double>(posMin) + 0.5);
}

//*******************
// PhysicalPosByValue
//*******************
double ttwin::TTPlayBar::ValueByPhysicalPos(const int pos, const int posMin, const int posMax){
  return static_cast<double>(pos - posMin) * (MaxValue - MinValue) / static_cast<double>(posMax - posMin) + MinValue;
}

//***************************************************
// CalcUpdateRect
//
// gets the rectandle to update when slider has moved
//***************************************************
RECT ttwin::TTPlayBar::CalcUpdateRect(const double value1, const double value2){
  
  RECT controllerrect;
  ::GetClientRect(Hwnd, &controllerrect);
  const int width  = controllerrect.right - controllerrect.left;
  const int posX   = controllerrect.left;
  const int posMin = posX + 0;               //physical Min-position 
  const int posMax = posX + width;           //physical Max-position 

  int pos1 = PhysicalPosByValue(value1, posMin, posMax);
  int pos2 = PhysicalPosByValue(value2, posMin, posMax);

  RECT rect;
  if (pos1 > pos2){
     int temp = pos2;
     pos2 = pos1;
     pos1 = temp;
  }
  //values empirical estimated (more or less...)
  rect.left   = controllerrect.left + pos1 - 2;
  rect.right  = controllerrect.left + pos2 + 1;
  rect.top    = controllerrect.top;
  rect.bottom = controllerrect.bottom;
  return rect;
}

