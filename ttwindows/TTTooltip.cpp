#include "TTTooltip.h"

//*****************************************************************
//*                           TTTooltip                           *
//*                                                               *
//*                                                               *
//*            class for creating a tool tip for a window         *
//*                                                               *
//*****************************************************************

//************
// constructor
//************
ttwin::TTTooltip::TTTooltip(HWND parent, HINSTANCE hinst, const std::string& tooltiptext){

  Parent = parent;
  Inst = hinst;

  Text = tooltiptext;
  LpStringByStdString(Text, lpString, MAX_TOOLTIP_LENGTH);

  //make sure 
  LoadCommonControlsToolTips();

  HwndTip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
                             WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             parent, NULL, 
                             hinst, NULL);
  ::SetWindowPos(HwndTip, HWND_TOPMOST,0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  ::SendMessage(HwndTip, TTM_ACTIVATE, static_cast<WPARAM>(TRUE), 0);

  TOOLINFO ti = {0};
  ti.cbSize = sizeof(TOOLINFO);
  ti.uFlags = TTF_SUBCLASS;
  ti.hwnd = Parent;
  ti.hinst = Inst;
  ti.lpszText = lpString;

  ::GetClientRect (parent, &ti.rect);

  // Associate the tooltip with the "tool" window.
  ::SendMessage(HwndTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

  //::SetWindowText(parent, TEXT("~~New ToolTip text"));
}

//***********
// destructor
//***********
ttwin::TTTooltip::~TTTooltip(){
  if (HwndTip && Parent){
    TOOLINFO ti = {0};
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = Parent;
    ti.hinst = Inst;
    ti.lpszText = lpString;
    ::SendMessage(HwndTip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
  }
  ::DestroyWindow(HwndTip);
}

//***********************
// SetToolTip
//
// sets the tool tip text
//***********************
void ttwin::TTTooltip::SetToolTip(const std::string& tooltiptext){
  Text = tooltiptext;
  LpStringByStdString(Text, lpString, MAX_TOOLTIP_LENGTH);
  if (HwndTip && Parent){
    TOOLINFO ti = {0};
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = Parent;
    ti.hinst = Inst;
    ti.lpszText = lpString;
    ::SendMessage(HwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM) (LPTOOLINFO) &ti);
  }
}

