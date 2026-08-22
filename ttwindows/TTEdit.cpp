#include "TTEdit.h"


//*****************************************************************
//*                            TTEdit                             *
//*                                                               *
//*                                                               *
//*               it is a wrapper around an edit control          *
//*                                                               *
//*****************************************************************

//***********************************************
// MainCallback
//
// static windows event callback function
// its adress is given by AddressOfMainCallback()
//***********************************************
LRESULT CALLBACK ttwin::TTEdit::EditCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
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
WNDPROC ttwin::TTEdit::AddressOfEditCallback(){ return &EditCallback; }

//**********************************************
// constructor - doas nothing but initialisation
//**********************************************
ttwin::TTEdit::TTEdit() : TTWin("not used")
{
}


//************
// destructor
//************
ttwin::TTEdit::~TTEdit(){
 
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
LRESULT ttwin::TTEdit::Messagehandler(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam){
 
  const HWND hDlg = GetParent(hwnd);
  const int item  = GetDlgCtrlID(hwnd); 

  
  switch (wMsg){
    //events indicating that the current item was changed 
    case WM_KEYDOWN:
    case WM_CHAR:
    case WM_PASTE:      //CheckLastEditField(hDlg, item);
                        //EditFieldHasChanged(item);
                        break;
    //events indicating that the current item was really(!) activated, but not yet changed
    case WM_SETFOCUS:   SendChildWasClickedMsg(0);
    case WM_KILLFOCUS:
                        //CheckLastEditField(hDlg, item);
                        break;
    case WM_CTLCOLOREDIT:
      SetTextColor((HDC)wParam, RGB(255, 255, 255)); // white
      SetBkMode((HDC)wParam, TRANSPARENT);
      return reinterpret_cast<BOOL>(GetStockObject(BLACK_BRUSH));
  }

  //forward to original proc 
  return CallWindowProc(OrgEditProcPtr, hwnd, wMsg, wParam, lParam);
}


//*******************
// Create
// 
// create the window
//*******************
bool ttwin::TTEdit::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, 
                           int x, int y, int width, int height, TTControlUnit unit)
{
 
  //convert coordinates into pixel
  if (unit == DIALOGUNIT) PixelsFromBaseUnits(x, y, width, height);
  Hwnd = ::CreateWindowEx(WS_EX_STATICEDGE, "edit", label.c_str(), WS_CHILD | WS_VISIBLE,//| WS_CLIPCHILDREN,
                          x, y, width, height, parent, reinterpret_cast<HMENU>(static_cast<UINT_PTR>(id)), hinst, NULL);
  if (Hwnd == 0){
    return false;
  }
  Hinst = hinst;
  ID = id;
  Parent = parent;
  TTWinCallBackMap[Hwnd] = this;
  Label = label;

  //subclassing 
  OrgEditProcPtr = reinterpret_cast<WNDPROC>(SetWindowLongPtr(Hwnd, GWLP_WNDPROC, reinterpret_cast<LONG>(AddressOfEditCallback())));

  Created();
  return true;
}

//**************
// SetFont
//
// sets the font
//**************
void ttwin::TTEdit::SetFont(HFONT font){
  ::SendMessage(Hwnd,  WM_SETFONT, reinterpret_cast<WPARAM>(font), MAKELPARAM(TRUE, 0));
}

//***************************************
// ConfigureNumeric
//
// configures the edit for numeric values
//***************************************
void ttwin::TTEdit::ConfigureNumeric(double editRangeMin, double editRangeMax, unsigned int editdigits){
  IsNumeric = true;
  EditRangeMin = editRangeMin;
  EditRangeMax = editRangeMax;
  Editdigits = editdigits;
}



//********************
// SetText
//
// sets the label text
//********************
void ttwin::TTEdit::SetText(const std::string& label){
  Label = label;
  ::SendMessage(Hwnd, WM_SETTEXT, 0, (LPARAM)label.c_str());
  Invalidate();
}

//********************
// GetText
//
// gets the label text
//********************
std::string ttwin::TTEdit::GetText() const{
  ::GetWindowText(Hwnd, lpString, MAX_TEXTVALUE_LENGTH);
  Label = lpString;
  return Label;
}


//****************
// SetNumericValue
//****************
void ttwin::TTEdit::SetNumericValue(const double v){
  if (IsNumeric){
    SetText(ttutil::StringUtil::double2string(v, Editdigits));
  }
}

//****************
// GetNumericValue
//****************
double ttwin::TTEdit::GetNumericValue() const{
  if (IsNumeric){
    const std::string text = GetText();
    double v = ttutil::StringUtil::string2double(text);
    if (v < EditRangeMin){
      v = EditRangeMin;
    }
    if (v > EditRangeMax){
      v = EditRangeMax;
    }
    return v;
  }
  else{
    return 0.0;
  }
}
