#include "TTMainWin.h"


//*****************************************************************************
//*                                TTMainWin                                  *
//*                                                                           *
//*                                                                           *
//*                      class that represents a main window                  *
//*                                                                           *
//*****************************************************************************


//---------------------- construction and initialisation -----------------

//***********************************************
// constructor - does nothing but initialisation
//***********************************************
ttwin::TTMainWin::TTMainWin(const std::string classname) : TTWin(classname) {}

//************
// destructor 
//************
ttwin::TTMainWin::~TTMainWin(){}

//*******
//create
//*******
bool ttwin::TTMainWin::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label,
                              int x, int y, int width, int height, TTControlUnit unit)
{
  return Create(parent, hinst, id, label, x, y, width, height, unit, SW_SHOW);
}


//*******
//create
//*******
bool ttwin::TTMainWin::Create(HWND parent, HINSTANCE hinst, int id, const std::string& label,
                              int x, int y, int width, int height, TTControlUnit unit, int nCmdShow)
{
  //HWND         hwnd;
  WNDCLASSEX   wndclassex = { 0 };

  wndclassex.cbSize = sizeof(WNDCLASSEX);
  wndclassex.style = CS_HREDRAW | CS_VREDRAW;
  wndclassex.lpfnWndProc = AddressOfMainCallback();
  wndclassex.cbClsExtra = 0;
  wndclassex.cbWndExtra = 0;
  wndclassex.hInstance = hinst;
  wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wndclassex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDR_MENU1);
  wndclassex.lpszClassName = TEXT(Classname.c_str());
  wndclassex.hIconSm = wndclassex.hIcon;

  if (!::RegisterClassEx(&wndclassex)){
    ::MessageBox(NULL, TEXT("RegisterClassEx failed!"), Classname.c_str(), MB_ICONERROR);
    return 0;
  }

  Hinst = hinst;
  ID = id;
  Label = label;

  //??????????
  INITCOMMONCONTROLSEX commenControl = { 0 };
  InitCommonControlsEx(&commenControl);

  Hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                          TEXT(Classname.c_str()),
                          TEXT(label.c_str()),
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          width,
                          height,
                          NULL,
                          NULL,
                          Hinst,
                          reinterpret_cast<LPVOID>(this));

  ::ShowWindow(Hwnd, nCmdShow);
  ::UpdateWindow(Hwnd);

  return true;
}


//**********
// mainloop 
//**********
WPARAM ttwin::TTMainWin::MainLoop(){
  MSG msg;
  while (::GetMessage(&msg, NULL, 0, 0))
  {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
  return msg.wParam;
}

//***************
// SetTitle
// 
// sets the title
//***************
void ttwin::TTMainWin::SetTitle(const std::string& title){
  Label = title;
  SetWindowText(Hwnd, Label.c_str());
}


