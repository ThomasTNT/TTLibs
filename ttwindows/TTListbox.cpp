#include "TTListbox.h"

//*****************************************************************
//*                          TTListbox                            *
//*                                                               *
//*           wrapper around a Windows LISTBOX control            *
//*                                                               *
//*****************************************************************

//***********************************************
// ListboxCallback
//***********************************************
LRESULT CALLBACK ttwin::TTListbox::ListboxCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  TTWin* win = TTWinCallBackMap[hwnd];
  if (win) return win->Messagehandler(hwnd, msg, wParam, lParam);
  return TRUE;
}

//***********************************************
// AddressOfListboxCallback
//***********************************************
WNDPROC ttwin::TTListbox::AddressOfListboxCallback(){ return &ListboxCallback; }

//************
// constructor
//************
ttwin::TTListbox::TTListbox() : TTWin("not used")
{
}

//************
// destructor
//************
ttwin::TTListbox::~TTListbox(){
}

//***********************
// Messagehandler
//***********************
LRESULT ttwin::TTListbox::Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  return CallWindowProc(OrgListboxProcPtr, hwnd, msg, wParam, lParam);
}

//*******************
// Create
//*******************
bool ttwin::TTListbox::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label,
                              int x, int y, int width, int height, TTControlUnit unit)
{
  if (unit == DIALOGUNIT) PixelsFromBaseUnits(x, y, width, height);

  Hwnd = ::CreateWindow("LISTBOX", NULL,
    LBS_NOTIFY | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER,
    x, y, width, height, parent, reinterpret_cast<HMENU>(id), hinst, NULL);
  if (Hwnd == 0) return false;

  Hinst  = hinst;
  ID     = id;
  Parent = parent;
  Label  = label;
  TTWinCallBackMap[Hwnd] = this;

  OrgListboxProcPtr = reinterpret_cast<WNDPROC>(
    SetWindowLongPtr(Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG>(AddressOfListboxCallback())));

  Created();
  return true;
}

//**************
// SetFont
//**************
void ttwin::TTListbox::SetFont(HFONT font){
  ::SendMessage(Hwnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), MAKELPARAM(TRUE, 0));
}

//**************
// AddItem
//**************
void ttwin::TTListbox::AddItem(const std::string& text, COLORREF color){
  const LRESULT pos = ::SendMessage(Hwnd, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
  if (color != 0 && pos != LB_ERR)
    ::SendMessage(Hwnd, LB_SETITEMDATA, static_cast<WPARAM>(pos), static_cast<LPARAM>(color));
}

//***************
// InsertItem
//***************
void ttwin::TTListbox::InsertItem(int pos, const std::string& text, COLORREF color){
  const LRESULT inserted = ::SendMessage(Hwnd, LB_INSERTSTRING, static_cast<WPARAM>(pos), reinterpret_cast<LPARAM>(text.c_str()));
  if (color != 0 && inserted != LB_ERR)
    ::SendMessage(Hwnd, LB_SETITEMDATA, static_cast<WPARAM>(inserted), static_cast<LPARAM>(color));
}

//***************
// SetItemColor
//***************
void ttwin::TTListbox::SetItemColor(int pos, COLORREF color){
  ::SendMessage(Hwnd, LB_SETITEMDATA, static_cast<WPARAM>(pos), static_cast<LPARAM>(color));
}

//***************
// RemoveItem
//***************
void ttwin::TTListbox::RemoveItem(int pos){
  ::SendMessage(Hwnd, LB_DELETESTRING, static_cast<WPARAM>(pos), 0);
}

//*********
// Clear
//*********
void ttwin::TTListbox::Clear(){
  ::SendMessage(Hwnd, LB_RESETCONTENT, 0, 0);
}

//*************
// GetCount
//*************
int ttwin::TTListbox::GetCount() const {
  const LRESULT count = ::SendMessage(Hwnd, LB_GETCOUNT, 0, 0);
  return count == LB_ERR ? 0 : static_cast<int>(count);
}

//******************
// GetSelectedPos
//******************
int ttwin::TTListbox::GetSelectedPos() const {
  const LRESULT sel = ::SendMessage(Hwnd, LB_GETCURSEL, 0, 0);
  return sel == LB_ERR ? -1 : static_cast<int>(sel);
}

//***********************
// GetSelectedItemString
//***********************
std::string ttwin::TTListbox::GetSelectedItemString() const {
  const int pos = GetSelectedPos();
  return pos >= 0 ? GetItem(pos) : "";
}

//*************
// GetItem
//*************
std::string ttwin::TTListbox::GetItem(int pos) const {
  const LRESULT length = ::SendMessage(Hwnd, LB_GETTEXTLEN, static_cast<WPARAM>(pos), 0);
  if (length == LB_ERR || length <= 0) return "";
  ::SendMessage(Hwnd, LB_GETTEXT, static_cast<WPARAM>(pos),
    reinterpret_cast<LPARAM>(const_cast<char*>(lpString)));
  return std::string(lpString);
}

//******************
// SetSelectedPos
//******************
void ttwin::TTListbox::SetSelectedPos(int pos){
  ::SendMessage(Hwnd, LB_SETCURSEL, static_cast<WPARAM>(pos), 0);
}
