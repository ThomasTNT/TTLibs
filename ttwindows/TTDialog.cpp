#include "TTDialog.h"

#include "Logger.h"

//*****************************************************************
//*                           TTDialog                            *
//*                                                               *
//*                                                               *
//*                     base class for dialogs                    *
//*                                                               *
//*****************************************************************

//**********************************************
// constructor - doas nothing but initialisation
//**********************************************
ttwin::TTDialog::TTDialog(const std::string classname, const int width, const int height)
                : TTWin(classname), 
                Modal(false), Width(width), Height(height)
{
  SetBackColor(RGB(10, 10, 10));
}

//************
// destructor
//************
ttwin::TTDialog::~TTDialog(){
}


//******************************************
// GetPreferedWindowRect
//
// gets the prefered window rect
// normally calculated before calling create
// returns default sizes
//******************************************
RECT ttwin::TTDialog::GetPreferedWindowRect(){
  RECT rect;
  rect.left = 0;
  rect.right = rect.left + Width;
  rect.top = 0;
  rect.bottom = rect.top + Height;
  return rect;
}

//*******************
// Create
// 
// create the window
//*******************
bool ttwin::TTDialog::Create(HWND parent, HINSTANCE hinst, int id, int x, int y, const std::string& label, bool modal)
{
  Hinst = hinst;
  ID = id;
  Parent = parent;
  Label = label;
  Modal = modal;

  //calculate size (again)
  const RECT prefRect = GetPreferedWindowRect();
  Width = prefRect.right - prefRect.left + 15; //plus for real client window
  Height = prefRect.bottom - prefRect.top + 15; //plus for real client window

  //register class in necessary 
  Register(Hinst, Classname.c_str());
  
  Hwnd = CreateWindowEx(WS_EX_DLGMODALFRAME, TEXT(Classname.c_str()), TEXT(Label.c_str()), WS_CAPTION|WS_SYSMENU,
                        x, y, Width, Height, 
                        NULL, NULL, Hinst, reinterpret_cast<LPVOID>(this));

  if (Modal) ::EnableWindow(Parent, FALSE);

  TTWinCallBackMap[Hwnd] = this;
  if (Hwnd == 0){
    const DWORD errorcode = ::GetLastError();
    return false;
  }
  
  Created();

  return true;
}


//************************
// Register
// 
// registers window class
//************************
bool ttwin::TTDialog::Register(const HINSTANCE hinst, const LPCSTR classname){
  WNDCLASSEX wc = {0};
  wc.cbSize           = sizeof(WNDCLASSEX);
  wc.lpfnWndProc      = AddressOfMainCallback();
  wc.hInstance        = hinst;
  wc.hbrBackground    = GetSysColorBrush(COLOR_3DFACE);
  wc.lpszClassName    = classname;
  ATOM klasse = ::RegisterClassEx(&wc);

  return klasse != 0;
}


//*********************
// Init
// 
// called by WM_CREATE
//*********************
void ttwin::TTDialog::Init(){
  if (Modal) ::EnableWindow(Parent, FALSE);
}

//*********************
// Destroy
// 
// called by WM_DESTROY
//*********************
void ttwin::TTDialog::Destroy(){
  TTWin::Destroy();
  if (Modal) ::EnableWindow(Parent, TRUE);
}

//*************************
// Messagehandler
// 
// windows message function
//*************************
LRESULT ttwin::TTDialog::Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  //Logger::getInstance().println("ttwin::TTWin::Messagehandler msg: " + util::StringUtil::uint2hexstring(msg) +  " this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
  Logger::getInstance().println("ttwin::TTDialog::Messagehandler " + WinMessageToString(msg) + " in " + Classname + " " + Label);

  int	item, cmd;
  HWND handle;

  switch(msg){

    case WM_TIMER:
      
      break;

    case TTM_CHILD_CLICKED:
      ChildWasClicked(reinterpret_cast<TTWin*>(wParam), static_cast<int>(lParam), 0);
      return 0;

    case TTM_CHILD_CLICKEDCHANGED:
      ChildWasClickedAndChanged(reinterpret_cast<TTWin*>(wParam), static_cast<int>(lParam), 0);
      return 0;

    case WM_NCCREATE:
      //Logger::getInstance().println("ttwin::TTWin::Messagehandler WM_NCCREATE this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
      break;

    case WM_CREATE:
      //Logger::getInstance().println("ttwin::TTWin::Messagehandler WM_CREATE this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
      Init();
      break;

    case WM_INITDIALOG:
      //Logger::getInstance().println("ttwin::TTWin::Messagehandler WM_INITDIALOG this: " + util::StringUtil::uint2hexstring(reinterpret_cast<unsigned int>(this)));
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

    case WM_HSCROLL:
      handle = reinterpret_cast<HWND>(lParam);
      ChildScroll(handle);
      return 0;

    case WM_VSCROLL:
      handle = reinterpret_cast<HWND>(lParam);
      ChildScroll(handle);
      return 0;

    case WM_COMMAND:
      item = LOWORD(wParam);
      cmd  = HIWORD(wParam);
      switch (cmd){
        case BN_CLICKED:  ChildCommandBnClicked(item); break;
        case EN_CHANGE:   ChildEnChanged(item);        break;
        case LBN_SELCHANGE: ChildListboxSelectionChanged(item); break;
        case LBN_DBLCLK:    ChildListboxDoubleClicked(item);   break;
      }
      break;

  }
  return ::DefWindowProc(hwnd, msg, wParam, lParam);
}







//*********
// PaintAll
//*********
void ttwin::TTDialog::PaintAll(){

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  if (BackgroundImage) BackgroundImage->Init(hdc);
  DrawBackground(hdc, true);   

  //used pens etc.
  /*HPEN outline = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 0)); 

  rect.bottom -= 1;
  rect.right -=1;

  POINT point;
  HGDIOBJ orgpen = ::SelectObject(hdc, outline);
  MoveToEx(hdc, rect.left,  rect.top, &point);
  LineTo  (hdc, rect.right, rect.top); 
  LineTo  (hdc, rect.right, rect.bottom); 
  LineTo  (hdc, rect.left,  rect.bottom); 
  LineTo  (hdc, rect.left,  rect.top); 

  //select the original pens
  ::SelectObject(hdc, orgpen);

  //delete all objects after use
  ::DeleteObject(outline);  */

  //free device context
  ::EndPaint(Hwnd, (LPPAINTSTRUCT)&ps);
}





