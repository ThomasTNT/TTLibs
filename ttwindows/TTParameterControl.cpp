#include "TTParameterControl.h"



//*****************************************************************************
//*                          TTParameterControl                               *
//*                                                                           *
//*                                                                           *
//*     complexer control window that handels manipulation of an parameter    *
//*                                                                           *
//* This class is abstract and should be derived for the specifiec type of    *
//* parameters. TTParameterControl is the base class.                         *
//*****************************************************************************

//***********
//constructor
//***********
ttwin::TTParameterControl::TTParameterControl() : TTWin("TTParameterControl"){

}

//**********
//destructor
//**********
ttwin::TTParameterControl::~TTParameterControl(){

}

//************
// SetEditFont
//************
void ttwin::TTParameterControl::SetEditFont(const HFONT font){
  EditFont = font;
}


//*****************************************************************************
//*                          TTParameterControlSimple                         *
//*                                                                           *
//*                                                                           *
//*     complexer control window that handels manipulation of an parameter    *
//*                                                                           *
//* this class is abstract and should be derived for the specifiec type of    *
//* parameter.                                                                *
//*****************************************************************************

//***********
//constructor
//***********
ttwin::TTParameterControlSimple::TTParameterControlSimple() : TTParameterControl(){

}

//**********
//destructor
//**********
ttwin::TTParameterControlSimple::~TTParameterControlSimple(){

}

//**********
//Configure
//**********
void ttwin::TTParameterControlSimple::Configure(double sliderRangeMin, double sliderrangeMax, unsigned int scaleDiv,
                                                SliderIlluminationType illumination, SliderType type, SliderGridType grid,
                                                double typeFactor, int snapRange, 
                                                double editRangeMin, double editRangeMax, unsigned int editdigits)
{
  Slider.Configure(sliderRangeMin, sliderrangeMax, scaleDiv, illumination, type, grid, typeFactor, snapRange);
  EditWin.ConfigureNumeric(editRangeMin, editRangeMax, editdigits);
}

//*******************
// CreateToolTip
// 
// creates a tool tip
//*******************
void ttwin::TTParameterControlSimple::CreateToolTip(const std::string& tooltip){
  TTParameterControl::CreateToolTip(tooltip);
  Slider.CreateToolTip(tooltip);
  EditWin.CreateToolTip(tooltip);
}

//*************************
// Messagehandler
// 
// windows message function
//*************************
LRESULT ttwin::TTParameterControlSimple::Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
  int	item, cmd;
  HWND handle;
  switch (msg){
  case WM_HSCROLL:
    handle = reinterpret_cast<HWND>(lParam);
    if (handle == Slider.Handle()) CM_Slider();
    return 0;

  case WM_COMMAND:
    item = LOWORD(wParam);
    cmd = HIWORD(wParam);

    if (item == EditWin.GetID()){
      if (cmd == EN_CHANGE) CM_Text();
    }
  }

  return ttwin::TTWin::Messagehandler(hwnd, msg, wParam, lParam);
}

//************************
//Init
//
// called during WM_CREATE
//************************
void ttwin::TTParameterControlSimple::Init(){
  CreateOwnControls();
}


//********************************
// Enable
//
// Enables or disbales the control
//********************************
void ttwin::TTParameterControlSimple::Enable(const bool enable){
  Slider.Enable(enable);
  EditWin.Enable(enable);
  //LabelWin.Enable(enable);
  TTWin::Enable(enable);
}

//************************
// CreateOwnControls
//************************
void ttwin::TTParameterControlSimple::CreateOwnControls(){

  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  const int width = rect.right - rect.left;
  const int height = rect.bottom - rect.top;

  int posX = rect.left;
  int posY = 0;

  //sizes of the elements
  /*const int labelW = EDITINFO_MIN_WIDTH;
  const int labelH = EDITINFO_HEIGHT;
  const int editW = EDITINFO_MIN_WIDTH;
  const int editH = EDITINFO_HEIGHT;
  const int labelX = frameX + INFO_SPACE_LEFT;
  const int editX = frameX + INFO_SPACE_LEFT + editW + INFO_SPACE_BETWEEN;
  const int longcomboW = labelW + editW + INFO_SPACE_BETWEEN;
  const int distanceV = (labelH > editH ? labelH : editH) + INFO_SPACE_BETWEEN;
  const int sliderW = SLIDER_WIDTH;
  const int sliderEditW = longcomboW - SLIDER_WIDTH - INFO_SPACE_BETWEEN;*/

  //LabelWin
  //LabelDevice.Create(Hwnd, HInst, IDC_LABEL_DEVICE, "Device", labelX, posY, labelW, labelH, ttwin::PIXEL);
  //LabelDevice.SetColorText(COLOR_LABELTEXT);
  //LabelDevice.CreateToolTip("selected Device. Must be ASIO!");

  Slider.CreateAsChild(*this, Hinst, IDC_PARAMCONTROL_SLIDER, Label, posX, posY, PARAMCONTROL_SLIDER_WIDTH, PARAMCONTROL_SLIDER_HEIGHT, ttwin::PIXEL);
  Slider.Configure(-10, 10, 19, ttwin::CENTER, ttwin::LINEAR, ttwin::SNAPTOCENTERPOS, 0, 2);
  Slider.Enable(true);

  const int dy = (height - PARAMCONTROL_EDIT_HEIGHT) / 2;
  EditWin.CreateAsChild(*this, Hinst, IDC_PARAMCONTROL_EDIT, "", posX + PARAMCONTROL_SLIDER_WIDTH + PARAMCONTROL_SPACE_BETWEEN, posY + dy, PARAMCONTROL_EDIT_WIDTH, PARAMCONTROL_EDIT_HEIGHT, ttwin::PIXEL);
  EditWin.SetFont(EditFont);
  EditWin.Enable(true);
  
}


//********
//PaintAll
//********
void ttwin::TTParameterControlSimple::PaintAll(){
  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);
  if (BackgroundImage) BackgroundImage->Init(hdc);
  DrawBackground(hdc, true);

  //free device context
  ::EndPaint(Hwnd, (LPPAINTSTRUCT)&ps);
}



//*****************************************************************************
//*                          TTParameterControlFloat                          *
//*                                                                           *
//*                                                                           *
//* Implementation of TTParameterControl for simple float values              *
//*****************************************************************************

//************
// constructor
//************
ttwin::TTParameterControlFloat::TTParameterControlFloat() : TTParameterControlSimple(){

}

//***********
// destructor*
//**********
ttwin::TTParameterControlFloat::~TTParameterControlFloat(){

}

//****************
// SetValuePointer
//****************
void ttwin::TTParameterControlFloat::SetValuePointer(float * const ptr){
  PointerToValue = ptr;
  if (PointerToValue){
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}

//*********
// SetValue
//*********
void ttwin::TTParameterControlFloat::SetValue(float value){
  if (PointerToValue){
    *PointerToValue = value;
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}

//*********
// GetValue
//*********
float ttwin::TTParameterControlFloat::GetValue(){
  return PointerToValue ? *PointerToValue : 0.0;
}

//***********************
// Configure
//
// configures the control
//***********************
//void ttwin::TTParameterControlFloat::Configure(){

//}


//************************
// Init
//
// called during WM_CREATE
//************************
void ttwin::TTParameterControlFloat::Init(){
  CreateOwnControls();
}


//************************
// CreateOwnControls
//************************
void ttwin::TTParameterControlFloat::CreateOwnControls(){
  TTParameterControlSimple::CreateOwnControls();
  if (PointerToValue){
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}


//****************
// EVENT CM_Slider
//****************
void ttwin::TTParameterControlFloat::CM_Slider(){
  if (!SkipControlFunction){
    SkipControlFunction = true;
    const double v = Slider.GetValue();
    EditWin.SetNumericValue(v);
    if (PointerToValue) *PointerToValue = static_cast<float>(v);
    SkipControlFunction = false;
    SendChildWasClickedAndChangedMsg(0);
  }
}

//***********
// CM_CM_Text
//***********
void ttwin::TTParameterControlFloat::CM_Text(){
  if (!SkipControlFunction){
    SkipControlFunction = true;
    const double v = EditWin.GetNumericValue();
    Slider.SetValue(v);
    if (PointerToValue) *PointerToValue = static_cast<float>(v);
    SkipControlFunction = false;
    SendChildWasClickedAndChangedMsg(0);
  }
}

//*****************************************************************************
//*                          TTParameterControlInt                            *
//*                                                                           *
//*                                                                           *
//* Implementation of TTParameterControl for simple int values                *
//*****************************************************************************

//************
// constructor
//************
ttwin::TTParameterControlInt::TTParameterControlInt() : TTParameterControlSimple(){

}

//***********
// destructor*
//**********
ttwin::TTParameterControlInt::~TTParameterControlInt(){

}

//****************
// SetValuePointer
//****************
void ttwin::TTParameterControlInt::SetValuePointer(int * const ptr){
  PointerToValue = ptr;
  if (PointerToValue){
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}

//************************
// Init
//
// called during WM_CREATE
//************************
void ttwin::TTParameterControlInt::Init(){
  CreateOwnControls();
}


//************************
// CreateOwnControls
//************************
void ttwin::TTParameterControlInt::CreateOwnControls(){
  TTParameterControlSimple::CreateOwnControls();
  if (PointerToValue){
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}


//****************
// EVENT CM_Slider
//****************
void ttwin::TTParameterControlInt::CM_Slider(){
  if (!SkipControlFunction){
    SkipControlFunction = true;
    const int v = static_cast<int>(Slider.GetValue() + 0.5);
    EditWin.SetNumericValue(v);
    if (PointerToValue) *PointerToValue = v;
    SkipControlFunction = false;
    SendChildWasClickedAndChangedMsg(0);
  }
}

//***********
// CM_CM_Text
//***********
void ttwin::TTParameterControlInt::CM_Text(){
  if (!SkipControlFunction){
    SkipControlFunction = true;
    const int v = static_cast<int>(EditWin.GetNumericValue() + 0.5);
    Slider.SetValue(v);
    if (PointerToValue) *PointerToValue = v;
    SkipControlFunction = false;
    SendChildWasClickedAndChangedMsg(0);
  }
}



//*****************************************************************************
//*                          TTParameterControlUInt                           *
//*                                                                           *
//*                                                                           *
//* Implementation of TTParameterControl for simple unisgned int values       *
//*****************************************************************************

//************
// constructor
//************
ttwin::TTParameterControlUInt::TTParameterControlUInt() : TTParameterControlSimple(){

}

//***********
// destructor*
//**********
ttwin::TTParameterControlUInt::~TTParameterControlUInt(){

}

//****************
// SetValuePointer
//****************
void ttwin::TTParameterControlUInt::SetValuePointer(unsigned int * const ptr){
  PointerToValue = ptr;
  if (PointerToValue){
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}

//************************
// Init
//
// called during WM_CREATE
//************************
void ttwin::TTParameterControlUInt::Init(){
  CreateOwnControls();
}


//************************
// CreateOwnControls
//************************
void ttwin::TTParameterControlUInt::CreateOwnControls(){
  TTParameterControlSimple::CreateOwnControls();
  if (PointerToValue){
    Slider.SetValue(*PointerToValue);
    EditWin.SetNumericValue(*PointerToValue);
  }
}


//****************
// EVENT CM_Slider
//****************
void ttwin::TTParameterControlUInt::CM_Slider(){
  if (!SkipControlFunction){
    SkipControlFunction = true;
    const int v = static_cast<int>(Slider.GetValue() + 0.5);
    EditWin.SetNumericValue(v);
    if (PointerToValue) *PointerToValue = v;
    SkipControlFunction = false;
    SendChildWasClickedAndChangedMsg(0);
  }
}

//***********
// CM_CM_Text
//***********
void ttwin::TTParameterControlUInt::CM_Text(){
  if (!SkipControlFunction){
    SkipControlFunction = true;
    const int v = static_cast<int>(EditWin.GetNumericValue() + 0.5);
    Slider.SetValue(v);
    if (PointerToValue) *PointerToValue = v;
    SkipControlFunction = false;
    SendChildWasClickedAndChangedMsg(0);
  }
}