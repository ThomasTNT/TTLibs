#include "TTWin.h"

#include "Logger.h"

//*****************************************************************
//*                             TTWin                             *
//*                                                               *
//*                                                               *
//*                    base class for own windows                 *
//*                                                               *
//*****************************************************************

//**********************************************************************
// map to store the handels ansd the pointes to the real control objects
//**********************************************************************
std::map<HWND, ttwin::TTWin*> ttwin::TTWin::TTWinCallBackMap;

//***********************************************
// MainCallback
//
// static windows event callback function
// its adress is given by AddressOfMainCallback()
//***********************************************
LRESULT CALLBACK ttwin::TTWin::MainCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
   
  //Logger::getInstance().println("ttwin::TTWin::MainCallback " + WinMessageToString(msg));

  //forward to the actual window object
  auto iter = TTWinCallBackMap.find(hwnd);
  TTWin* win = iter != TTWinCallBackMap.end() ? iter->second : nullptr;
  if (!win){
    if (msg == WM_CREATE || msg == WM_INITDIALOG){
      const CREATESTRUCT* const cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
      win = reinterpret_cast<TTWin*>(cs->lpCreateParams);
      if (win){
        TTWinCallBackMap[hwnd] = win;
        //set Hwnd and Hinst. It is needed in child windows before CreateWinEx returns! 
        win->Hwnd = hwnd;
      }
    } 
  } 
  if (win){
    return win->Messagehandler(hwnd, msg, wParam, lParam);
  }
  else{
    return ::DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

//***************************************************
// AddressOfMainCallback
//
// static function to get the address of MainCallback
//***************************************************
WNDPROC ttwin::TTWin::AddressOfMainCallback(){ return &MainCallback; }

//******************
// LookupByHandle
//******************
ttwin::TTWin* ttwin::TTWin::LookupByHandle(HWND hwnd){
  const auto it = TTWinCallBackMap.find(hwnd);
  return (it != TTWinCallBackMap.end()) ? it->second : nullptr;
}

//**********************************************
// constructor - doas nothing but initialisation
//**********************************************
ttwin::TTWin::TTWin(const std::string& classname): Classname(classname)
{
}

//**********************************************
// constructor - doas nothing but initialisation
//**********************************************
/*ttwin::TTWin::TTWin(const std::wstring& classname){
  TTWin(WStringByString(classname));
}*/

//************
// destructor
//************
ttwin::TTWin::~TTWin(){
  Destroy();
}


//*******************
// Create
// 
// create the window
//*******************
bool ttwin::TTWin::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, 
                          int x, int y, int width, int height, TTControlUnit unit)
{
  Logger::getInstance().println("ttwin::TTWin::Create " + Classname);

  //register class in necessary 
  Register(hinst, Classname.c_str());

  //convert coordinates into pixel
  if (unit == DIALOGUNIT) PixelsFromBaseUnits(x, y, width, height);

  //must be set earlier, because it will be used in CreateWindowEx in the WM_CREATE handler method 
  Hinst = hinst;
  ID = id;
  Parent = parent;
  Label = label;

  //Hwnd will be set in CreateWindowEx before in WM_CREATE handler method 
  //so this is some kind of redundancy
  //TODO: check this behaviour. And set Hwnd only in WM_CREATE handler
  Hwnd = ::CreateWindowEx(0, Classname.c_str(), NULL, WS_CHILD | WS_VISIBLE | GetExtraWindowStyle(),
                          x, y, width, height, parent, (HMENU)id, hinst, 
                          reinterpret_cast<LPVOID>(this));
  if (Hwnd == 0){
    const std::string errorMessage = GetLastErrorString();
    return false;
  }
 
  Logger::getInstance().println("ttwin::TTWin::Create ende " + Classname + " " + ttutil::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(Hwnd)));

  //TODO: redundancy. Should be set in WM_CREATE handler already. 
  TTWinCallBackMap[Hwnd] = this;

  Created();
  return true;
}

//*************************************
// CreateAsChild
//
// create control as a child of control
//*************************************
bool ttwin::TTWin::CreateAsChild(TTWin& control, HINSTANCE hinst, int id, const std::string& label, 
                                 int x, int y, int width, int height, TTControlUnit unit)
{
  SetParentTTWin(&control);
  return Create(control.Hwnd, hinst, id, label, x, y, width, height, unit);
}

//*******************
// CreateToolTip
// 
// creates a tool tip
//*******************
void ttwin::TTWin::CreateToolTip(const std::string& tooltip){
  if (Tooltip) delete Tooltip;
  Tooltip = new TTTooltip(Hwnd, Hinst, tooltip);
}

//*******************
// SetToolTip
//
// creates a tool tip
//*******************
void ttwin::TTWin::SetToolTip(const std::string& tooltip){
  if (Tooltip) Tooltip->SetToolTip(tooltip);
}


//************************
// Register
// 
// registers window class
//************************
bool ttwin::TTWin::Register(const HINSTANCE hinst, const LPCSTR classname){
  WNDCLASS rwc = {0};
  rwc.lpszClassName = classname;
  rwc.hInstance     = hinst;
  rwc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);//GetSysColorBrush(COLOR_BTNFACE);
  rwc.style         = CS_HREDRAW;
  rwc.lpfnWndProc   = AddressOfMainCallback();
  rwc.hCursor       = LoadCursor(0, IDC_ARROW);
  ATOM klasse = ::RegisterClass(&rwc);
  DWORD err = GetLastError();
  if(!klasse && err != ERROR_CLASS_ALREADY_EXISTS){
    return false;
  }
  return klasse != 0;
}

//*************************
// Unregister
// 
// unregisters window class
//*************************
bool ttwin::TTWin::Unregister(const HINSTANCE hinst, const LPCSTR classname){
  const BOOL success = ::UnregisterClass(classname, hinst);
  if ((success == FALSE)){
    const DWORD err = ::GetLastError();
    Logger::getInstance().println("Error adding string to combobox: " + ttutil::StringUtil::int2string(err));
  }
  return success != FALSE;
}

//*********************
// Destroy
// 
// called by WM_DESTROY
//*********************
void ttwin::TTWin::Destroy(){
  Logger::getInstance().println("ttwin::TTWin::Destroy: " + ttutil::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)) + " hwnd: " + ttutil::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(Hwnd)));
  if (Hwnd){ 
    TTWinCallBackMap.erase(Hwnd);
    ::DestroyWindow(Hwnd);
    Hwnd = 0;
  }
  //TODO causes crash!!!! 
  //delete Tooltip;
}

//*************************
// Messagehandler
// 
// windows message function
//*************************
LRESULT ttwin::TTWin::Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  //Logger::getInstance().println("ttwin::TTWin::Messagehandler msg: " + util::StringUtil::uint2hexstring(msg) +  " this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)) + " label: " + Label);
  //Logger::getInstance().println("ttwin::TTWin::Messagehandler " + WinMessageToString(msg) + " in " + Classname + " " + Label);

  switch(msg){

    case WM_TIMER:
      if (wParam == TTWIN_TIMER_ID1) TimerEvent1();
      break;

    case TTM_CHILD_CLICKED:
      ChildWasClicked(reinterpret_cast<TTWin*>(wParam), static_cast<int>(lParam), 0);
      return 0;

    case TTM_CHILD_CLICKEDCHANGED:
      ChildWasClickedAndChanged(reinterpret_cast<TTWin*>(wParam), static_cast<int>(lParam), 0);
      return 0;

    case WM_COMMAND:
      if (HIWORD(wParam) == BN_CLICKED){
        ::SendMessage(Parent, WM_COMMAND, wParam, lParam);
      }
      break;

    case WM_NCCREATE:
      //Logger::getInstance().println("ttwin::TTWin::Messagehandler WM_NCCREATE this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
      break;

    case WM_CREATE:
      //Logger::getInstance().println("ttwin::TTWin::Messagehandler WM_CREATE this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
      Init();
      break;
	
    case WM_CLOSE:

      break;

    case WM_DESTROY:
      Destroy();
      break;

		case WM_PAINT:
      //Logger::getInstance().println("ttwin::TTWin::Messagehandler WM_PAINT this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
      PaintAll();
      return FALSE;

    case WM_ERASEBKGND:
      //this called before WM_PAINT to erase the background
      //due to avaoid flickering while drawing the images we do this
      //in PaintAll()
     return (LRESULT)1; // Say we handled it.

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
      return EditColor(wParam);

    case WM_MEASUREITEM:
      OnMeasureComboItem(lParam);
      return TRUE;

    case WM_DRAWITEM: {
      const DRAWITEMSTRUCT* const dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
      if (dis && dis->CtlType == ODT_LISTBOX) return OnDrawListboxItem(lParam);
      return OnDrawComboItem(lParam);
    }

    case WM_SIZE:
      ResizesFields();
      return FALSE;
      
    case WM_LBUTTONDOWN:
      CM_Mouse_LeftDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (wParam & MK_SHIFT) != FALSE, (wParam & MK_CONTROL) != FALSE, false);
      return 0;

    case WM_LBUTTONUP:
      CM_Mouse_LeftUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (wParam & MK_SHIFT) != FALSE, (wParam & MK_CONTROL) != FALSE, false);
      return 0;
      
    case WM_MOUSEMOVE:
      CM_Mouse_Move(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (wParam & MK_SHIFT) != FALSE, (wParam & MK_CONTROL) != FALSE, false);
      return 0;

    case WM_MOUSELEAVE:
    //case WM_NCMOUSELEAVE: //?????? H�?
      CM_Mouse_Leave(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (wParam & MK_SHIFT) != FALSE, (wParam & MK_CONTROL) != FALSE, false);
      return 0;

    case WM_RBUTTONDOWN:
      CM_Mouse_RightDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (wParam & MK_SHIFT) != FALSE, (wParam & MK_CONTROL) != FALSE, false);
      return 0;

    case WM_RBUTTONUP:
      CM_Mouse_RightUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (wParam & MK_SHIFT) != FALSE, (wParam & MK_CONTROL) != FALSE, false);
      break; // let DefWindowProc generate WM_CONTEXTMENU so it bubbles up to the parent

    case WM_VSCROLL: {
      SCROLLINFO si{};
      si.cbSize = sizeof(si);
      si.fMask  = SIF_TRACKPOS;
      ::GetScrollInfo(hwnd, SB_VERT, &si);
      CM_VScroll(static_cast<int>(LOWORD(wParam)), si.nTrackPos);
      return 0;
    }

    case WM_MOUSEWHEEL:
      CM_MouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
      return 0;

  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}


//********************
// Resize
// 
// resizes the control
// by calling win-api
//********************
void ttwin::TTWin::Resize(int x, int y, int width, int height, TTControlUnit unit){
  //convert coordinates into pixel
  if (unit == DIALOGUNIT) PixelsFromBaseUnits(x, y, width, height);
  ::MoveWindow(Hwnd, x, y, width, height, TRUE);
}

//**********************************
// Invalidate
//
// invalidates the whole client area
//**********************************
void ttwin::TTWin::Invalidate(){
  if (!Hwnd) return;
  ::InvalidateRect(Hwnd, NULL, FALSE);
}

//**********************************
// Invalidate
//
// invalidates the given client area
//**********************************
void ttwin::TTWin::Invalidate(int x1, int y1, int x2, int y2){
  if (!Hwnd) return;
  RECT rect;
  rect.left = x1;
  rect.right = x2;
  rect.top = y1;
  rect.bottom = y2;
  ::InvalidateRect(Hwnd, &rect, FALSE);
}

//*************************************
// CheckForInvalidate
// 
// invalidates if necessary
// call this when something has changed
//*************************************
void ttwin::TTWin::CheckForInvalidate(){ 
  //just simple default implementation
  Invalidate(); 
}

//*****************************
// Update
//
// updest the window immediately
//******************************
void ttwin::TTWin::Update(){
  ::UpdateWindow(Hwnd);
}

//***************************
// Show
//
// shows or hides the control
//***************************

//******************
// AddWindowStyle
//******************
void ttwin::TTWin::AddWindowStyle(const LONG style) {
  ::SetWindowLong(Hwnd, GWL_STYLE, ::GetWindowLong(Hwnd, GWL_STYLE) | style);
}

void ttwin::TTWin::Show(const bool show){
  ::ShowWindow(Hwnd, show ? SW_SHOW : SW_HIDE);
}

//*********************************
// Show
//
// /Enables or disbales the control
//*********************************
void ttwin::TTWin::Enable(const bool enable){
  ::EnableWindow(Hwnd, enable);
  Invalidate();
}

//***********************************
// Focus
//
// sets keyboard focus to this window
//***********************************
void ttwin::TTWin::Focus(){
  ::SetFocus(Hwnd);
}

//******************************************
// ToTop
//
// brings this window (or its parent) to top
//******************************************
void ttwin::TTWin::ToTop(){
  ::BringWindowToTop(Hwnd);
}

//*********************************************
// IsVisible
//
// returns true if window exists and is visible
//*********************************************
bool ttwin::TTWin::IsVisible(){
  if (Hwnd == 0) return false;
  WINDOWPLACEMENT placementInfo;
  ::GetWindowPlacement(Hwnd, &placementInfo);
  return placementInfo.showCmd == SW_SHOWNORMAL || placementInfo.showCmd == SW_SHOWMAXIMIZED;
}

//***********************************
// IsEnabled
// 
// returns true if window is enabled
//***********************************
bool ttwin::TTWin::IsEnabled(){
  return ::IsWindowEnabled(Hwnd) != FALSE;
}


//*************
// SetBackImage
//*************
void ttwin::TTWin::SetBackImage(TTBackgroundImage * const backgroundImage){
  BackgroundImage = backgroundImage;
  SetBackImageToChildren();
}

//****************************************************************
// SetBackImageUpperLeft
//
// if upperRight is true, the back image will be arranged to (0,0) 
// not relative to the parent
//****************************************************************
void ttwin::TTWin::SetBackImageUpperLeft(bool upperLeft){
  BackgroundImageUpperLeft = upperLeft;
}

//*************
// SetBackColor
//*************
void ttwin::TTWin::SetBackColor(const COLORREF color){
  BackColor = color;
}

//****************************************************************
// ChildWasClicked
// 
// can be called directly or via sending TTM_CHILD_CLICKED message
//****************************************************************
void ttwin::TTWin::ChildWasClicked(TTWin* child, const int value1, const int value2){
  if (TTWinParent) TTWinParent->ChildWasClicked(child, value1, value2);
}

//***********************************************************************
// ChildWasClickedAndChanged
// 
// can be called directly or via sending TTM_CHILD_CLICKEDCHANGED message
//***********************************************************************
void ttwin::TTWin::ChildWasClickedAndChanged(TTWin* child, const int value1, const int value2){
  if (TTWinParent) TTWinParent->ChildWasClicked(child, value1, value2);
}

//************************************
// SendChildWasClickedMsg
// 
// sends the child was clicked message
//************************************
void ttwin::TTWin::SendChildWasClickedMsg(const int value){
  ::SendMessage(Parent,                          //HWND to which the message should be sent, our parent of course 
                TTM_CHILD_CLICKED,               //the message 
                reinterpret_cast<WPARAM>(this) , //wParam
                static_cast<LPARAM>(value)); 
}

//*******************************************************************************
// SendChildWasClickedAndChangedMsg
// 
// sends the child was clicked message
// the difference to SendChildWasClickedMsg is that the child calles for changing
// e.g. after mouse input. Where as SendChildWasClickedMsg is only a click.
//*******************************************************************************
void ttwin::TTWin::SendChildWasClickedAndChangedMsg(const int value){
  ::SendMessage(Parent,                          //HWND to which the message should be sent, our parent of course 
                TTM_CHILD_CLICKEDCHANGED,        //the message 
                reinterpret_cast<WPARAM>(this) , //wParam
                static_cast<LPARAM>(value)); 
}

//*************************************************
// SetParentTTWin
//
// adds this window as parent, for direct messaging
//*************************************************
void ttwin::TTWin::SetParentTTWin(TTWin* parent){
  TTWinParent = parent;
  Parent = TTWinParent->Hwnd;
}

//*************************************************
// GetParentTTWin

// returns the parent win as const 
//*************************************************
const ttwin::TTWin* ttwin::TTWin::GetParentTTWin() const {
  return TTWinParent;
}

//************************************
// GetGrandParentTTWin
//
// return the most upper parent window
//************************************
const ttwin::TTWin* ttwin::TTWin::GetGrandParentTTWin() const {
  if (!TTWinParent) return nullptr;
  const ttwin::TTWin * parent = TTWinParent;
  const ttwin::TTWin * lastParent = nullptr;
  while (parent) {
    lastParent = parent;
    parent = parent->GetParentTTWin();
  }
  return lastParent;
}

//**********************************
// Handle
//
// returns the handle of this window
//**********************************
HWND ttwin::TTWin::Handle() const {
  return Hwnd;
}

//*********************
// GetClientRect
// 
// gets the client rect
//*********************
RECT ttwin::TTWin::GetClientRect() const {
  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  return rect;
}

//*********************
// GetWindowRect
// 
// gets the window rect
//*********************
RECT ttwin::TTWin::GetWindowRect() const {
  RECT rect;
  ::GetWindowRect(Hwnd, &rect);
  return rect;
}

//******************
// OffsetToOtherWinX 
//
// gets offset x
//******************
int ttwin::TTWin::OffsetToOtherWinX(const TTWin* const win) const {
  const RECT ownRect = GetWindowRect();
  POINT point;
  point.x = ownRect.left;
  point.y = ownRect.top;
  ScreenToClient(win->Handle(), &point);
  const int offsetX = point.x - ownRect.left;
  return offsetX;
}

//******************
// OffsetToOtherWinY 
//
// gets offset y
//******************
int ttwin::TTWin::OffsetToOtherWinY(const TTWin* const win) const {
  const RECT ownRect = GetWindowRect();
  POINT point;
  point.x = ownRect.left;
  point.y = ownRect.top;
  ScreenToClient(win->Handle(), &point);
  const int offsetY = point.y - ownRect.top;
  return offsetY;
}


//******************************************
// GetPreferedWindowRect
//
// gets the prefered window rect
// normally calculated before calling create
// default is (0, 0, 0, 0);
//******************************************
RECT ttwin::TTWin::GetPreferedWindowRect() const {
  RECT rect;
  rect.bottom = 0;
  rect.left = 0;
  rect.right = 0;
  rect.top = 0;
  return rect;
}

//********************
// SetText
//
// sets the label text
//********************
void ttwin::TTWin::SetText(const std::string& label){
  Label = label;
  Invalidate();
}

//********************
// GetText
//
// gets the label text
//********************
std::string ttwin::TTWin::GetText(){
  return Label;
}

//*************
// GetID
//
// gets the id
//*************
unsigned int ttwin::TTWin::GetID(){
  return ID;
}

//***********************
// SetSelected
//
// sets the select status
//***********************
void ttwin::TTWin::SetSelected(const bool selected){
  Selected = selected;
}

//***********************
// GetSelected
// 
// gets the select status
//***********************
bool ttwin::TTWin::GetSelected(){
  return Selected;
}

//*************************************************
// UpdateContent
//
// refreshes the model or the content of the window
// shall call Invalidate() or Update() then
// Used to refresh level meters etc.
//*************************************************
void ttwin::TTWin::UpdateContent(){
  //empty implementation
}

//**********************************************
// SetFontDimensions
//
// sets font height and weight used in PaintAll
//**********************************************
void ttwin::TTWin::SetFontDimensions(const int height, const int weight){
  FontHeight = height;
  FontWeight = weight;
}

//*******************
// StartTimer1
// 
// starts the timer 1
//*******************
void ttwin::TTWin::StartTimer1(const unsigned int milliseconds){
  TimerTime1 = milliseconds;
  ::SetTimer(Hwnd, TTWIN_TIMER_ID1, TimerTime1, NULL);
}

//*****************
// StopTimer1
//
// stops the time 1
//*****************
void ttwin::TTWin::StopTimer1(){
  ::KillTimer(Hwnd, TTWIN_TIMER_ID1);
  TimerTime1 = 0;
}

//*******************
// TimerEvent1
//
// called for timer 1 
//*******************
void ttwin::TTWin::TimerEvent1(){
  const bool run = TimerHandler1();
  if (run){
    StartTimer1(TimerTime1);
  }
  else{
    StopTimer1();
  }
}

//***********************************
// EditColor
//
// to set edit control to white/black
// instead standard windows colors
//***********************************
BOOL ttwin::TTWin::EditColor(WPARAM wParam){
  ::SetTextColor((HDC)wParam, TTWIN_EDIT_TEXT_COLOR);
  ::SetBkMode((HDC)wParam, TRANSPARENT);
  return reinterpret_cast<BOOL>(GetStockObject(BLACK_BRUSH));
}

//**********************
// OnDrawComboItem
//
// WM_DRAWITEM handler for CBS_OWNERDRAWFIXED combo boxes:
// black background, white text, dark-gray highlight when item is selected in list
//**********************
LRESULT ttwin::TTWin::OnDrawComboItem(LPARAM lParam){
  const DRAWITEMSTRUCT* const dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
  if (!dis || dis->CtlType != ODT_COMBOBOX) return FALSE;

  const bool highlighted = (dis->itemState & ODS_SELECTED) != 0;
  ::FillRect(dis->hDC, &dis->rcItem,
    static_cast<HBRUSH>(::GetStockObject(highlighted ? DKGRAY_BRUSH : BLACK_BRUSH)));

  if (dis->itemID != static_cast<UINT>(-1)){
    char text[256] = "";
    ::SendMessage(dis->hwndItem, CB_GETLBTEXT, dis->itemID, reinterpret_cast<LPARAM>(text));
    ::SetTextColor(dis->hDC, TTWIN_EDIT_TEXT_COLOR);
    ::SetBkMode(dis->hDC, TRANSPARENT);
    RECT textRect = dis->rcItem;
    textRect.left += 3;
    ::DrawText(dis->hDC, text, -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
  }
  return TRUE;
}

//***********************
// OnMeasureComboItem
//
// WM_MEASUREITEM handler for owner-drawn combo boxes and listboxes
//***********************
void ttwin::TTWin::OnMeasureComboItem(LPARAM lParam){
  MEASUREITEMSTRUCT* const mis = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
  if (!mis) return;
  if (mis->CtlType == ODT_COMBOBOX) mis->itemHeight = 18;
  else if (mis->CtlType == ODT_LISTBOX) mis->itemHeight = 16;
}

//**********************
// OnDrawListboxItem
//
// WM_DRAWITEM handler for LBS_OWNERDRAWFIXED listboxes:
// item color is stored per-item via LB_SETITEMDATA (0 or LB_ERR = default white)
//**********************
LRESULT ttwin::TTWin::OnDrawListboxItem(LPARAM lParam){
  const DRAWITEMSTRUCT* const dis = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
  if (!dis || dis->CtlType != ODT_LISTBOX) return FALSE;
  if (dis->itemID == static_cast<UINT>(-1)) return TRUE;

  const bool highlighted = (dis->itemState & ODS_SELECTED) != 0;
  ::FillRect(dis->hDC, &dis->rcItem, static_cast<HBRUSH>(::GetStockObject(highlighted ? DKGRAY_BRUSH : BLACK_BRUSH)));

  char text[512] = "";
  ::SendMessage(dis->hwndItem, LB_GETTEXT, dis->itemID, reinterpret_cast<LPARAM>(text));

  const LRESULT itemData = ::SendMessage(dis->hwndItem, LB_GETITEMDATA, dis->itemID, 0);
  const COLORREF textColor = (itemData != 0 && itemData != LB_ERR) ? static_cast<COLORREF>(itemData) : TTWIN_EDIT_TEXT_COLOR;

  ::SetTextColor(dis->hDC, textColor);
  ::SetBkMode(dis->hDC, TRANSPARENT);
  RECT textRect = dis->rcItem;
  textRect.left += 3;
  ::DrawText(dis->hDC, text, -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
  return TRUE;
}

//*********
// PaintAll
//*********
void ttwin::TTWin::PaintAll(){

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  if (BackgroundImage) BackgroundImage->Init(hdc);
  DrawBackground(hdc, true);   

  if (DrawOutline) {
    //used pens etc.
    HPEN outline = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

    rect.bottom -= 1;
    rect.right -= 1;

    POINT point;
    HGDIOBJ orgpen = ::SelectObject(hdc, outline);
    MoveToEx(hdc, rect.left, rect.top, &point);
    LineTo(hdc, rect.right, rect.top);
    LineTo(hdc, rect.right, rect.bottom);
    LineTo(hdc, rect.left, rect.bottom);
    LineTo(hdc, rect.left, rect.top);

    //select the original pens
    ::SelectObject(hdc, orgpen);

    //delete all objects after use
    ::DeleteObject(outline);
  }

  //free device context
  ::EndPaint(Hwnd, (LPPAINTSTRUCT)&ps);
}


//***************************************************
// DrawBackground
//
// for drawing the background of the control
// if no background is set a dummy color will be used
//***************************************************
void ttwin::TTWin::DrawBackground(HDC hdc, const bool drawDefaultBackground){
  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  POINT offset;
  if (BackgroundImageUpperLeft){
    offset.x = 0; 
    offset.y = 0;
  }
  else{
    offset = CalcOffsetToMainWin();
  }

  if (BackgroundImage && BackgroundImage->Valid()){
    BackgroundImage->DrawImage(hdc, rect, offset.x, offset.y);
  }
  else if (drawDefaultBackground){
    HBRUSH brushBackgroundDefault  = CreateSolidBrush(BackColor);
    HGDIOBJ orgBrush               = ::SelectObject(hdc, brushBackgroundDefault);
    ::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    ::SelectObject(hdc, orgBrush);
    ::DeleteObject(brushBackgroundDefault);
  }
}

//****************************************************
// CreateBackDrawDC
//
// you can call this to create a memory device context
// for drawing in background
// call this on PaintAll() if you need such a DC
//****************************************************
void ttwin::TTWin::CreateBackDrawDC(HDC hdc, const RECT winRect){
  if (BackDrawDC == 0){
    BackDrawDC = ::CreateCompatibleDC(hdc);
    BackDrawBitmap = ::CreateCompatibleBitmap(hdc, winRect.right - winRect.left + 1, winRect.bottom - winRect.top + 1);
    BackDrawOld = ::SelectObject(BackDrawDC, BackDrawBitmap);
  }
}

//********************
// PixelsFromBaseUnits
//********************
void ttwin::TTWin::PixelsFromBaseUnits(int &x, int &y, int &width, int &height){
  RECT rect; 
  rect.left   = x; 
  rect.top    = y;
  rect.right  = x + width;
  rect.bottom = y + height;
  ::MapDialogRect(Parent, &rect);
  x      = rect.left;
  y      = rect.top;
  width  = rect.right - x;
  height = rect.bottom - y;
}

//*******************************************************
// CalcOffsetToMainWin
//
// this is used in DrawBackground to calculate the offset
//*******************************************************
POINT ttwin::TTWin::CalcOffsetToMainWin() const{

  POINT points[1];
  points[0].x = 0;
  points[0].y = 0;

  //no parent -> offset is 0,0
  if (!Parent){
    return points[0];
  }

  HWND hwndMain; 
  if (!TTWinParent){
    //not ttwin parent -> just use the win parent 
    hwndMain = Parent;
  }
  else{
    // look for the highes grant parent
    TTWin* parent = TTWinParent;
    TTWin* grandparent = parent->TTWinParent;
    while (grandparent){
      parent = grandparent;
      grandparent = parent->TTWinParent;
    }
    //take its parent (handle) or the window (handle) itself 
    hwndMain = parent->Parent ? parent->Parent : parent->Hwnd;
  }
  ::MapWindowPoints(Hwnd, hwndMain, points, 1);
  return points[0];
}

//***********************
// GetLastErrorString
// 
// gets last error string
//***********************
std::string ttwin::TTWin::GetLastErrorString(){

  const DWORD errorcode = ::GetLastError();

  LPVOID lpMsgBuf;
  ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  errorcode,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  reinterpret_cast<LPTSTR>(&lpMsgBuf),
                  0,
                  NULL);

  std::string errorMessage(reinterpret_cast<char*>(lpMsgBuf));

  ::LocalFree(lpMsgBuf);

  return errorMessage;
}

//*********************************************
// WinMessageToString
// 
// gets the printable string of an WM_* message
//*********************************************
std::string ttwin::TTWin::WinMessageToString(const UINT msg){
  switch (msg){
    case WM_CREATE:        return std::string("WM_CREATE");
    case WM_INITDIALOG:    return std::string("WM_INITDIALOG");
    case WM_PAINT:         return std::string("WM_PAINT");
    case WM_SIZE:          return std::string("WM_SIZE");
    case WM_MOVE:          return std::string("WM_MOVE");
    case WM_PARENTNOTIFY:  return std::string("WM_PARENTNOTIFY");
    case WM_SHOWWINDOW:    return std::string("WM_SHOWWINDOW");
    case WM_COMMAND:       return std::string("WM_COMMAND");
    default: return ttutil::StringUtil::uint2hexstring(msg);
  }
}