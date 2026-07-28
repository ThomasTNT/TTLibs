#include "TTCombobox.h"

#include "Logger.h"

//*****************************************************************
//*                          TTCombobox                           *
//*                                                               *
//*                                                               *
//*           it is a wrapper around an combobox control          *
//*                                                               *
//*****************************************************************

//***********************************************
// MainCallback
//
// static windows event callback function
// its adress is given by AddressOfMainCallback()
//***********************************************
LRESULT CALLBACK ttwin::TTCombobox::ComboCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  //forward to the actual window object
  TTWin* win = TTWinCallBackMap[hwnd];
  if (win) return win->Messagehandler(hwnd, msg, wParam, lParam);
  return TRUE;
}

//***************************************************
// AddressOfMainCallback
//
// static function to get the address of MainCallback
//***************************************************
WNDPROC ttwin::TTCombobox::AddressOfComboCallback(){ return &ComboCallback; }

//**********************************************
// constructor - doas nothing but initialisation
//**********************************************
ttwin::TTCombobox::TTCombobox() : TTWin("not used")
{
}


//************
// destructor
//************
ttwin::TTCombobox::~TTCombobox(){
 
}

//***************************************************************
// Messagehandler
// 
// edit control subclassing callback function
//
// we have to detect two important cases: 
// 1st this edit contral has actually changed
// 2nd this edit control has been activated after editing another
//***************************************************************
LRESULT ttwin::TTCombobox::Messagehandler(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam){
  const HWND hDlg = GetParent(hwnd);
  const int item  = GetDlgCtrlID(hwnd); 
  switch (wMsg){
    /*case WM_CTLCOLOR:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX: 
    
      //to set edit control to white/black instead standard windows colors
      ::SetTextColor((HDC)wParam, RGB(255, 0, 0)); // white
      ::SetBkMode((HDC)wParam, TRANSPARENT);
      return reinterpret_cast<BOOL>(GetStockObject(BLACK_BRUSH));
    */
    //events indicating that the current item was changed
    case WM_KEYDOWN:
    case WM_CHAR:
    case WM_PASTE:      break;
    //events indicating that the current item was really(!) activated, but not yet changed
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
                        break;

    case WM_COMMAND:
      {Logger::getInstance().println("ttwin::TTCombobox::Messagehandler 4"); //############
        const int item = LOWORD (wParam);
			  const int cmd = HIWORD (wParam);
        //switch(item){
        Logger::getInstance().println("ttwin::TTCombobox::Messagehandler WM_COMMAND");
        SendClickMessage();
      }
      break;
  }  
  
  //forward to original proc 
  return CallWindowProc(OrgComboProcPtr, hwnd, wMsg, wParam, lParam);
}

//***************************************************
// SendClickMessage
//
// send a WM_COMMAND message like standard comboboxes
//***************************************************
void ttwin::TTCombobox::SendClickMessage(){
  ::SendMessage(Parent,     //HWND to which the message should be sent, our parent of course 
                WM_COMMAND, //the message 
                MAKEWPARAM(ID, CBN_SELCHANGE), //wParam
                //reinterpret_cast<LPARAM>(Hwnd)); //the window handle is strored in lParam 
                0);
}

//*******************
// Create
// 
// create the window
//*******************
bool ttwin::TTCombobox::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, 
                           int x, int y, int width, int height, TTControlUnit unit)
{
 
  //convert coordinates into pixel
  if (unit == DIALOGUNIT) PixelsFromBaseUnits(x, y, width, height);
  Hwnd = ::CreateWindow("COMBOBOX", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_OWNERDRAWFIXED | WS_CHILD | WS_VISIBLE,
                         x, y, width, height, parent, NULL, hinst, NULL);
  if (Hwnd == 0){
    return false;
  }
  Hinst = hinst;
  ID = id;
  Parent = parent;
  TTWinCallBackMap[Hwnd] = this;
  Label = label;

  //subclassing 
  OrgComboProcPtr = reinterpret_cast<WNDPROC>(SetWindowLongPtr(Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG>(AddressOfComboCallback())));

  Created();
  return true;
}

//**************
// SetFont
//
// sets the font
//**************
void ttwin::TTCombobox::SetFont(HFONT font){
  ::SendMessage(Hwnd,  WM_SETFONT, reinterpret_cast<WPARAM>(font), MAKELPARAM(TRUE, 0));
}

//********************************
// SetItem
//
// adds an entry (counting from 1)
//********************************
void ttwin::TTCombobox::SetItem(const unsigned int pos, const std::string& itemtext){
  if (pos == 0) return;
  int count = ::SendMessage(Hwnd, CB_GETCOUNT, 0, 0);
  while (count != CB_ERR && count < (static_cast<int>(pos) - 1)){
    ::SendMessage(Hwnd, CB_INSERTSTRING, -1, reinterpret_cast<LPARAM>((LPCTSTR)"---"));
    count = ::SendMessage(Hwnd, CB_GETCOUNT, 0, 0);
  }
  if (count >= static_cast<int>(pos)) ::SendMessage(Hwnd, CB_DELETESTRING, pos - 1, 0);
  ::SendMessage(Hwnd, CB_INSERTSTRING, pos - 1, reinterpret_cast<LPARAM>(reinterpret_cast<LPCTSTR>(itemtext.c_str())) );
}

//************************************
// GetItem
//
// gets the entry at (counting from 1)
//************************************
std::string ttwin::TTCombobox::GetItem(const unsigned int pos){
  if (pos == 0) return "";
  const int length = ::SendMessage(Hwnd, CB_GETLBTEXT, static_cast<int>(pos) - 1, reinterpret_cast<LPARAM>(lpString));
  if (length == CB_ERR) return "";
  return std::string(lpString);
}

//*************************************
// SetSelectedItemPos
//
// sets selected index, counting from 1
//*************************************
void ttwin::TTCombobox::SetSelectedItemPos(const unsigned int pos){
  const int sel = ::SendMessage(Hwnd, CB_SETCURSEL, static_cast<int>(pos) - 1, 0);
  if (sel == CB_ERR){
    return;
  }
}

//***********************
// SetSelectedItemString
//
// sets selected item
//***********************
void ttwin::TTCombobox::SetSelectedItemString(const std::string& itemtext){
  const int sel = ::SendMessage(Hwnd, CB_SELECTSTRING, -1, reinterpret_cast<LPARAM>(reinterpret_cast<LPCTSTR>(itemtext.c_str())));
  if (sel == CB_ERR){
    return;
  }
}

//*************************************
// GetSelectedItemPos
//
// gets selected index, counting from 1
//*************************************
unsigned int ttwin::TTCombobox::GetSelectedItemPos(){
  const int sel = ::SendMessage(Hwnd, CB_GETCURSEL, 0, 0);
  return sel == CB_ERR ? 0 : sel + 1;
}


//**********************
// GetSelectedItemString
//
// gets selected item
//**********************
std::string ttwin::TTCombobox::GetSelectedItemString(){
  return GetItem(GetSelectedItemPos());
}


//*******************
// Clear
// 
// clears all entries
//*******************
void ttwin::TTCombobox::Clear(){
  ::SendMessage(Hwnd, CB_RESETCONTENT, 0, 0);
}



