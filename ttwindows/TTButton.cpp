#include "TTButton.h"

#include "Logger.h"

//*****************************************************************
//*                          ButtonControl                        *
//*                                                               *
//*                                                               *
//*          class for check boxes for photoshop plugins          *
//*                                                               *
//*****************************************************************
//**********
//contructor
//**********
ttwin::TTButton::TTButton() : TTWin("TTButton")
{
}

//**********
//destructor
//**********
ttwin::TTButton::~TTButton(){}


//**********************
// Configure
//
// configures the button
//**********************
void ttwin::TTButton::Configure(const ButtonType type, const ButtonForm form, const bool enabledWhenInactive){
  Type = type;
  Form = form;
  EnabledWhenInactive = enabledWhenInactive;
}

//***********************
// ConfigureNumber
//
// sets the button number
//***********************
void ttwin::TTButton::ConfigureButtonNumber(const int number){
  ButtonNumber = number;
}

//***********************
// GetButtonNumber
//
// gets the button number
//***********************
int ttwin::TTButton::GetButtonNumber(){
  return ButtonNumber;
}

//*************************
// ConfigureColor
//
// sets the button color
// return true when changed
//*************************
bool ttwin::TTButton::ConfigureColor(const COLORREF c){
  return ConfigureColor(c, c, c);
}


//*************************
// ConfigureColor
//
// sets the button color 
// return true when changed
//*************************
bool ttwin::TTButton::ConfigureColor(const COLORREF c1, const COLORREF c2){
   return ConfigureColor(c1, c2, c2);
}

//*************************
// ConfigureColor
//
// sets the button color 
// return true when changed
//*************************
bool ttwin::TTButton::ConfigureColor(const COLORREF c1, const COLORREF c2, const COLORREF c3){
  unsigned int value;
  bool changed = false;
  value = GetRValue(c1);
  changed |= (value != R1);
  R1 = value; 
  value = GetGValue(c1);
  changed |= (value != G1);
  G1 = value; 
  value = GetBValue(c1);
  changed |= (value != B1);
  B1 = value; 
  value = GetRValue(c2);
  changed |= (value != R2);
  R2 = value; 
  value = GetGValue(c2);
  changed |= (value != G2);
  G2 = value;
  value = GetBValue(c2);
  changed |= (value != B2);
  B2 = value;
  value = GetRValue(c3);
  changed |= (value != R3);
  R3 = value; 
  value = GetGValue(c3);
  changed |= (value != G3);
  G3 = value;
  value = GetBValue(c3);
  changed |= (value != B3);
  B3 = value; 
  InitGraphic();
  return changed;
}

//******************************************************************
// ConfigureInactiveColor
//
// sets the button color for inactive state
// if disabledGrayBoarder is false, then inaktive is the same as all
// active states. 
// If it is true then only text will be colored in disabled mode.
// (this is the old behaviour) 
// return true when changed
//******************************************************************
bool ttwin::TTButton::ConfigureInactiveColor(const COLORREF c, const bool disabledGrayBoarder){
  DisabledGrayBoarder = disabledGrayBoarder;
  unsigned int value;
  bool changed = false;
  value = GetRValue(c);
  changed |= (value != R0);
  R0 = value; 
  value = GetGValue(c);
  changed |= (value != G0);
  G0 = value; 
  value = GetBValue(c);
  changed |= (value != B0);
  B0 = value; 
  if (!UseDisabledColors){
    Rd = R0;
    Gd = G0;
    Bd = B0;
  }
  InitGraphic();
  return changed;
}

//******************************************
//
//
// sets the special color for disabled state
//******************************************
bool ttwin::TTButton::ConfigureDisabledColor(const COLORREF c){
  bool changed = !UseDisabledColors;
  UseDisabledColors = true;
  unsigned int value;
  value = GetRValue(c);
  changed |= (value != Rd);
  Rd = value;
  value = GetGValue(c);
  changed |= (value != Gd);
  Gd = value;
  value = GetBValue(c);
  changed |= (value != Bd);
  Bd = value;
  InitGraphic();
  return changed;

}

//****************************************
// SetLabelInactive
// 
// sets an special text for inactive state
//****************************************
void ttwin::TTButton::SetLabelInactive(const std::string &label){
  LabelInactive = label;
}


//*************************
// SetLabel3rd
// 
// sets the additional text
//*************************
void ttwin::TTButton::SetLabel3rd(const std::string &label){
  Label3rd = label;
}

//*************************
// SetLabel4th
//
// sets the additional text
//*************************
void ttwin::TTButton::SetLabel4th(const std::string &label){
  Label4th = label;
}

//***************************************************
// SetLabelLine2
//
// sets the second line label shown below main label
//***************************************************
void ttwin::TTButton::SetLabelLine2(const std::string &label){
  LabelLine2 = label;
}

//*******************************
// ChangeColor
//
// 
// changes the color "on the fly"
//*******************************
void ttwin::TTButton::ChangeColor(const COLORREF newcolor){
  ConfigureColor(newcolor);
  //force redrawing
  const RECT updateRect = GetClientRect();
  InvalidateRect(Hwnd, &updateRect, TRUE);
  Update();
}
 
//**********
// SetActive
//**********
void ttwin::TTButton::SetActive(unsigned int value){
  //a button can only set active when pressed
  //and in this case it IS already active //WRONG!
  //if (Type != BUTTON && value != Active){
  if (Active != value){
    Active = value;
    Invalidate();
    Update();
  }
  //}
}

//**********
// GetActive
//**********
unsigned int ttwin::TTButton::GetActive() const{
  return Active;
}

//************************************************
// SwitchTwoColorState
//
// switches between two state on CHECKBOX_TWOCOLOR
//************************************************
// s == 0 or 1 
void ttwin::TTButton::SwitchTwoColorState(const unsigned int s){
  if (Type == CHECKBOX_TWOCOLOR){
    switch (s){
      case 0:
        if (Active == 2) Active = 0;
        else if (Active == 3) Active = 1;
        break;
      case 1:
        if (Active == 0) Active = 2;
        else if (Active == 1) Active = 3;
        break;
    }
  }
}

//*************************************
// Created 
//
// this is called after creation
// hack for calling Init()
// Why is there no WM_CREATE Message?!?
//*************************************
void ttwin::TTButton::Created(){
  Init(); //just call init
}

//*******
// Init
//*******
void ttwin::TTButton::Init(){
  Active  = 0;
  Pressed = false;
}


//*************************************************
// InitGraphic
//
// this function initializes all graphical elements
//*************************************************
void ttwin::TTButton::InitGraphic(){
 
  unsigned int hr;
  unsigned int sg;
  unsigned int bb;
  unsigned int HUE;
  unsigned int SAT;
  unsigned int BRG;

  //inactive
  Colors[0][0] = RGB( R0,  G0,  B0); //text
  if (DisabledGrayBoarder){
    Colors[0][1] = RGB( 75,  75,  75); 
    Colors[0][2] = RGB( 21,  21,  21);
    Colors[0][3] = RGB(184, 184, 184);
    Colors[0][4] = RGB( 75,  75,  75);
    Colors[0][5] = RGB( 21,  21,  21);
    Colors[0][6] = RGB(131, 131, 131);
  }
  else{
    hr = R0;
    sg = G0;
    bb = B0;
    HSBInt8FromRGBInt8(hr, sg, bb);
    HUE = hr;
    SAT = sg;
    BRG = bb;
    //sg = 171; //  67%  saturation
    //bb = 255; // 100%  brightness
    //RGBInt8FromHSBInt8(hr, sg, bb);
  
    hr = HUE;
    sg = static_cast<unsigned int>(0.13 * SAT); //  9%  saturation
    bb = static_cast<unsigned int>(50.0 + 0.03 * BRG); // 20% -> 32%  brightness
    if (sg > 255) sg = 255; 
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[0][1] = RGB(hr, sg, bb); //RGB( 81,  80,  74); 
  
    hr = HUE;
    sg = static_cast<unsigned int>(1.10 * SAT); //  74%  saturation
    bb = static_cast<unsigned int>(20.0 + 0.75 * BRG); // 8% -> 93%  brightness
    if (sg > 255) sg = 255; 
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[0][2] = RGB(hr, sg, bb); //RGB(238, 203,  62); 
  
    hr = HUE;
    sg = static_cast<unsigned int>(0.06 * SAT); //   4%  saturation
    bb = static_cast<unsigned int>(183.0 + 0.01 * BRG); // 72% -> 73%  brightness
    if (sg > 255) sg = 255; 
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[0][3] = RGB(hr, sg, bb); //RGB(187, 186, 180); 
  
    hr = HUE;
    sg = static_cast<unsigned int>(0 * SAT); //   0%  saturation
    bb = static_cast<unsigned int>(74.0 + 0 * BRG); // 29% -> 29%  brightness
    if (sg > 255) sg = 255; 
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[0][4] = RGB(hr, sg, bb); //RGB( 75,  75,  75); 
  
    hr = HUE;
    sg = static_cast<unsigned int>(0.36 * SAT); //  24%  saturation
    bb = static_cast<unsigned int>(20.0 + 0.03 * BRG); // 8% -> 11%  brightness
    if (sg > 255) sg = 255; 
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[0][5] = RGB(hr, sg, bb); //RGB( 29,  28,  22)); 
  
    hr = HUE;
    sg = static_cast<unsigned int>(0.19 * SAT);   //   5%  saturation
    bb = static_cast<unsigned int>(130.0 + 0.02 * BRG); // 51% -> 53%  brightness
    if (sg > 255) sg = 255; 
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[0][6] = RGB(hr, sg, bb); //RGB(135, 134,  12);
  }

  //active

  //0. text color
  Colors[1][0] = RGB(R1, G1, B1); //RGB(253, 227,  83); //text
  hr = R1;
  sg = G1;
  bb = B1;
  HSBInt8FromRGBInt8(hr, sg, bb);
  HUE = hr;
  SAT = sg;
  BRG = bb;
  //sg = 171; //  67%  saturation
  //bb = 255; // 100%  brightness
  //RGBInt8FromHSBInt8(hr, sg, bb);
  
  hr = HUE;
  sg =  static_cast<unsigned int>(0.13 * SAT); //  9%  saturation
  bb =  static_cast<unsigned int>(50.0 + 0.03 * BRG); // 20% -> 32%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[1][1] = RGB(hr, sg, bb); //RGB( 81,  80,  74); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(1.10 * SAT); //  74%  saturation
  bb = static_cast<unsigned int>(20.0 + 0.75 * BRG); // 8% -> 93%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[1][2] = RGB(hr, sg, bb); //RGB(238, 203,  62); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.06 * SAT); //   4%  saturation
  bb = static_cast<unsigned int>(183.0 + 0.01 * BRG); // 72% -> 73%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[1][3] = RGB(hr, sg, bb); //RGB(187, 186, 180); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0 * SAT); //   0%  saturation
  bb = static_cast<unsigned int>(74.0 + 0 * BRG); // 29% -> 29%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[1][4] = RGB(hr, sg, bb); //RGB( 75,  75,  75); 

  hr = HUE;
  sg = static_cast<unsigned int>(0.36 * SAT); //  24%  saturation
  bb = static_cast<unsigned int>(20.0 + 0.03 * BRG); // 8% -> 11%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[1][5] = RGB(hr, sg, bb); //RGB( 29,  28,  22)); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.19 * SAT);   //   5%  saturation
  bb = static_cast<unsigned int>(130.0 + 0.02 * BRG); // 51% -> 53%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[1][6] = RGB(hr, sg, bb); //RGB(135, 134,  12); 
  
  //0. text color
  Colors[2][0] = RGB(R2, G2, B2); //RGB(253, 227,  83); //text
  hr = R2;
  sg = G2;
  bb = B2;
  HSBInt8FromRGBInt8(hr, sg, bb);
  HUE = hr;
  SAT = sg;
  BRG = bb;
  //sg = 171; //  67%  saturation
  //bb = 255; // 100%  brightness
  //RGBInt8FromHSBInt8(hr, sg, bb);
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.13 * SAT); //  9%  saturation
  bb = static_cast<unsigned int>(50.0 + 0.03 * BRG); // 20% -> 32%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[2][1] = RGB(hr, sg, bb); //RGB( 81,  80,  74); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(1.10 * SAT); //  74%  saturation
  bb = static_cast<unsigned int>(20.0 + 0.75 * BRG); // 8% -> 93%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[2][2] = RGB(hr, sg, bb); //RGB(238, 203,  62); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.06 * SAT); //   4%  saturation
  bb = static_cast<unsigned int>(183.0 + 0.01 * BRG); // 72% -> 73%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[2][3] = RGB(hr, sg, bb); //RGB(187, 186, 180); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0 * SAT); //   0%  saturation
  bb = static_cast<unsigned int>(74.0 + 0 * BRG); // 29% -> 29%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[2][4] = RGB(hr, sg, bb); //RGB( 75,  75,  75); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.36 * SAT); //  24%  saturation
  bb = static_cast<unsigned int>(20.0 + 0.03 * BRG); // 8% -> 11%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[2][5] = RGB(hr, sg, bb); //RGB( 29,  28,  22)); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.19 * SAT);   //   5%  saturation
  bb = static_cast<unsigned int>(130.0 + 0.02 * BRG); // 51% -> 53%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[2][6] = RGB(hr, sg, bb); //RGB(135, 134,  12); 

  //0. text color
  Colors[3][0] = RGB(R2, G2, B2); //RGB(253, 227,  83); //text
  hr = R3;
  sg = G3;
  bb = B3;
  HSBInt8FromRGBInt8(hr, sg, bb);
  HUE = hr;
  SAT = sg;
  BRG = bb;
  //sg = 171; //  67%  saturation
  //bb = 255; // 100%  brightness
  //RGBInt8FromHSBInt8(hr, sg, bb);
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.13 * SAT); //  9%  saturation
  bb = static_cast<unsigned int>(50.0 + 0.03 * BRG); // 20% -> 32%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[3][1] = RGB(hr, sg, bb); //RGB( 81,  80,  74); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(1.10 * SAT); //  74%  saturation
  bb = static_cast<unsigned int>(20.0 + 0.75 * BRG); // 8% -> 93%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[3][2] = RGB(hr, sg, bb); //RGB(238, 203,  62); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.06 * SAT); //   4%  saturation
  bb = static_cast<unsigned int>(183.0 + 0.01 * BRG); // 72% -> 73%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[3][3] = RGB(hr, sg, bb); //RGB(187, 186, 180); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0 * SAT); //   0%  saturation
  bb = static_cast<unsigned int>(74.0 + 0 * BRG); // 29% -> 29%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[3][4] = RGB(hr, sg, bb); //RGB( 75,  75,  75); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.36 * SAT); //  24%  saturation
  bb = static_cast<unsigned int>(20.0 + 0.03 * BRG); // 8% -> 11%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[3][5] = RGB(hr, sg, bb); //RGB( 29,  28,  22)); 
  
  hr = HUE;
  sg = static_cast<unsigned int>(0.19 * SAT);   //   5%  saturation
  bb = static_cast<unsigned int>(130.0 + 0.02 * BRG); // 51% -> 53%  brightness
  if (sg > 255) sg = 255; 
  if (bb > 255) bb = 255;
  RGBInt8FromHSBInt8(hr, sg, bb);
  Colors[3][6] = RGB(hr, sg, bb); //RGB(135, 134,  12); 

  //disabled
  Colors[4][0] = RGB(Rd, Gd, Bd); //text
  if (DisabledGrayBoarder){
    Colors[4][1] = RGB(75, 75, 75);
    Colors[4][2] = RGB(21, 21, 21);
    Colors[4][3] = RGB(184, 184, 184);
    Colors[4][4] = RGB(75, 75, 75);
    Colors[4][5] = RGB(21, 21, 21);
    Colors[4][6] = RGB(131, 131, 131);
  }
  else{
    hr = Rd;
    sg = Gd;
    bb = Bd;
    HSBInt8FromRGBInt8(hr, sg, bb);
    HUE = hr;
    SAT = sg;
    BRG = bb;
    //sg = 171; //  67%  saturation
    //bb = 255; // 100%  brightness
    //RGBInt8FromHSBInt8(hr, sg, bb);

    hr = HUE;
    sg = static_cast<unsigned int>(0.13 * SAT); //  9%  saturation
    bb = static_cast<unsigned int>(50.0 + 0.03 * BRG); // 20% -> 32%  brightness
    if (sg > 255) sg = 255;
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[4][1] = RGB(hr, sg, bb); //RGB( 81,  80,  74); 

    hr = HUE;
    sg = static_cast<unsigned int>(1.10 * SAT); //  74%  saturation
    bb = static_cast<unsigned int>(20.0 + 0.75 * BRG); // 8% -> 93%  brightness
    if (sg > 255) sg = 255;
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[4][2] = RGB(hr, sg, bb); //RGB(238, 203,  62); 

    hr = HUE;
    sg = static_cast<unsigned int>(0.06 * SAT); //   4%  saturation
    bb = static_cast<unsigned int>(183.0 + 0.01 * BRG); // 72% -> 73%  brightness
    if (sg > 255) sg = 255;
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[4][3] = RGB(hr, sg, bb); //RGB(187, 186, 180); 

    hr = HUE;
    sg = static_cast<unsigned int>(0 * SAT); //   0%  saturation
    bb = static_cast<unsigned int>(74.0 + 0 * BRG); // 29% -> 29%  brightness
    if (sg > 255) sg = 255;
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[4][4] = RGB(hr, sg, bb); //RGB( 75,  75,  75); 

    hr = HUE;
    sg = static_cast<unsigned int>(0.36 * SAT); //  24%  saturation
    bb = static_cast<unsigned int>(20.0 + 0.03 * BRG); // 8% -> 11%  brightness
    if (sg > 255) sg = 255;
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[4][5] = RGB(hr, sg, bb); //RGB( 29,  28,  22)); 

    hr = HUE;
    sg = static_cast<unsigned int>(0.19 * SAT);   //   5%  saturation
    bb = static_cast<unsigned int>(130.0 + 0.02 * BRG); // 51% -> 53%  brightness
    if (sg > 255) sg = 255;
    if (bb > 255) bb = 255;
    RGBInt8FromHSBInt8(hr, sg, bb);
    Colors[4][6] = RGB(hr, sg, bb); //RGB(135, 134,  12);
  }

}


//*********
// PaintAll
//*********
void ttwin::TTButton::PaintAll(){
  switch (Form){
    case FORM_ARROWUP:
    case FORM_ARROWDOWN:
    case FORM_ARROWLEFT:
    case FORM_ARROWRIGHT: PaintArrow(); break;
    default:              PaintRect();  break;
  }
}

//**********
// PaintRect
//**********
void ttwin::TTButton::PaintRect(){

  const int offset = Pressed ? BUTTON_OFFSET_PRESSED : 0;

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  DrawBackground(hdc, true); 

  HFONT labelFont = ::CreateFont(FontHeight,                 // height of font
                                  0,                         // average character width
                                  0,                         // angle of escapement
                                  0,                         // base-line orientation angle
                                  FontWeight,                // font weight
                                  FALSE,                     // italic attribute option
                                  FALSE,                     // underline attribute option
                                  FALSE,                     // strikeout attribute option
                                  ANSI_CHARSET,              // character set identifier
                                  OUT_DEFAULT_PRECIS,        // output precision
                                  CLIP_DEFAULT_PRECIS,       // clipping precision
                                  ANTIALIASED_QUALITY,       // output quality
                                  VARIABLE_PITCH | FF_SWISS, // pitch and family
                                  "MS Shell Dlg");           // typeface name
  ::SetBkMode(hdc, TRANSPARENT);
  OrgFont = ::SelectObject(hdc, labelFont);
  ::SetTextAlign(hdc, TA_CENTER);

  const unsigned int colorIndex = (UseDisabledColors && !IsEnabled()) ? 4 : Active;

  ::SetTextColor(hdc, Colors[colorIndex][0]);

  std::string labelToUse;
  if      (Active == 2 && !Label3rd.empty())      labelToUse = Label3rd;
  else if (Active == 0 && !LabelInactive.empty()) labelToUse = LabelInactive;
  else if (Active == 3 && !Label4th.empty())      labelToUse = Label4th;
  else                                             labelToUse = Label;

  const int centerX = rect.left + (rect.right - rect.left) / 2 + offset;
  if (LabelLine2.empty()){
    const int y = rect.bottom - (rect.bottom - rect.top + FontHeight) / 2 - 1 + offset;
    ::TextOut(hdc, centerX, y, labelToUse.c_str(), static_cast<int>(labelToUse.size()));
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(labelFont);
  }
  else {
    const int fontHeight2 = FontHeight * 2 / 3 > 10 ? FontHeight * 2 / 3 : 10;
    const int totalH = FontHeight + 4 + fontHeight2;
    const int line1Y = rect.top + (rect.bottom - rect.top - totalH) / 2;
    ::TextOut(hdc, centerX, line1Y + offset, labelToUse.c_str(), static_cast<int>(labelToUse.size()));
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(labelFont);
    HFONT font2 = ::CreateFont(fontHeight2,                // height of font
                                0,                         // average character width
                                0,                         // angle of escapement
                                0,                         // base-line orientation angle
                                FW_NORMAL,                 // font weight
                                FALSE,                     // italic attribute option
                                FALSE,                     // underline attribute option
                                FALSE,                     // strikeout attribute option
                                ANSI_CHARSET,              // character set identifier
                                OUT_DEFAULT_PRECIS,        // output precision
                                CLIP_DEFAULT_PRECIS,       // clipping precision
                                ANTIALIASED_QUALITY,       // output quality
                                VARIABLE_PITCH | FF_SWISS, // pitch and family
                                "MS Shell Dlg");           // typeface name
    OrgFont = ::SelectObject(hdc, font2);
    ::TextOut(hdc, centerX, line1Y + FontHeight + 4 + offset, LabelLine2.c_str(), static_cast<int>(LabelLine2.size()));
    ::SelectObject(hdc, OrgFont);
    ::DeleteObject(font2);
  }
  
  POINT point;
  HPEN pen1, pen2, pen3, pen4, pen5, pen6;

  pen1 = ::CreatePen(PS_SOLID, 1, Colors[colorIndex][1]);
  pen2 = ::CreatePen(PS_SOLID, 1, Colors[colorIndex][2]);
  pen3 = ::CreatePen(PS_SOLID, 1, Colors[colorIndex][3]);
  pen4 = ::CreatePen(PS_SOLID, 1, Colors[colorIndex][4]);
  pen5 = ::CreatePen(PS_SOLID, 1, Colors[colorIndex][5]);
  pen6 = ::CreatePen(PS_SOLID, 1, Colors[colorIndex][6]);
 
  if (Pressed){

    //color4
    OrgPen = SelectObject(hdc, pen4);
    ::MoveToEx(hdc, rect.left,  rect.bottom - 1, &point);
    ::LineTo  (hdc, rect.left,  rect.top);
    ::LineTo  (hdc, rect.right, rect.top);

    //color5
    SelectObject(hdc, pen5);
    MoveToEx(hdc, rect.left  + 1, rect.bottom - 2, &point);
    LineTo  (hdc, rect.left  + 1, rect.top + 1);
    LineTo  (hdc, rect.right - 1, rect.top + 1);
    MoveToEx(hdc, rect.left  + 2, rect.bottom - 2, &point);
    LineTo  (hdc, rect.left  + 2, rect.top + 2);
    LineTo  (hdc, rect.right - 1, rect.top + 2);

    //color6
    SelectObject(hdc, pen6);
    MoveToEx(hdc, rect.left  + 4, rect.bottom - 2, &point);
    LineTo  (hdc, rect.left  + 4, rect.top + 4);
    LineTo  (hdc, rect.right - 3, rect.top + 4);

    //color1
    SelectObject(hdc, pen1);
    MoveToEx(hdc, rect.left + 4,  rect.bottom - 3, &point);
    LineTo  (hdc, rect.right - 3,  rect.bottom - 3);
    LineTo  (hdc, rect.right - 3,  rect.top + 2);

    //color2
    SelectObject(hdc, pen2);
    MoveToEx(hdc, rect.left + 3, rect.top + 3, &point);
    LineTo  (hdc, rect.right - 2, rect.top + 3);
    LineTo  (hdc, rect.right - 2, rect.bottom - 2);
    LineTo  (hdc, rect.left  + 3, rect.bottom - 2);
    LineTo  (hdc, rect.left  + 3, rect.top + 3);

    //color3
    SelectObject(hdc, pen3);
    MoveToEx(hdc, rect.left + 1, rect.bottom - 1, &point);
    LineTo  (hdc, rect.right - 1, rect.bottom - 1);
    LineTo  (hdc, rect.right - 1, rect.top);
    
    ::SelectObject(hdc, OrgPen);
  }
  else{

    //color1
    OrgPen = ::SelectObject(hdc, pen1);
    ::MoveToEx(hdc, rect.left,  rect.bottom - 1, &point);
    ::LineTo  (hdc, rect.left,  rect.top);
    ::LineTo  (hdc, rect.right, rect.top);
    ::MoveToEx(hdc, rect.left + 3,  rect.bottom - 3, &point);
    ::LineTo  (hdc, rect.right - 3,  rect.bottom - 3);
    ::LineTo  (hdc, rect.right - 3,  rect.top + 1);

    //color2
    ::SelectObject(hdc, pen2);
    ::MoveToEx(hdc, rect.left + 1, rect.top + 1, &point);
    ::LineTo  (hdc, rect.right - 2, rect.top + 1);
    ::LineTo  (hdc, rect.right - 2, rect.bottom - 2);
    ::LineTo  (hdc, rect.left  + 1, rect.bottom - 2);
    ::LineTo  (hdc, rect.left  + 1, rect.top + 1);

    //color3
    ::SelectObject(hdc, pen3);
    ::MoveToEx(hdc, rect.left + 2,  rect.bottom - 3, &point);
    ::LineTo  (hdc, rect.left + 2,  rect.top + 2);
    ::LineTo  (hdc, rect.right - 3, rect.top + 2);
    ::MoveToEx(hdc, rect.left + 1, rect.bottom - 1, &point);
    ::LineTo  (hdc, rect.right - 1, rect.bottom - 1);
    ::LineTo  (hdc, rect.right - 1, rect.top);

    ::SelectObject(hdc, OrgPen);

  }
  
  ::DeleteObject(pen6);
  ::DeleteObject(pen5); 
  ::DeleteObject(pen4);
  ::DeleteObject(pen3);
  ::DeleteObject(pen2); 
  ::DeleteObject(pen1);

  ::EndPaint(Hwnd, &ps);
}


//**********
// PaintArrow
//**********
void ttwin::TTButton::PaintArrow(){
  const int offset = Pressed ? BUTTON_OFFSET_PRESSED : 0;

  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  DrawBackground(hdc, true);

  const unsigned int colorIndex = (UseDisabledColors && !IsEnabled()) ? 4 : Active;
  const int w = rect.right  - rect.left;
  const int h = rect.bottom - rect.top;
  const int margin = 2;

  // triangle vertices: tip is the single corner, baseA/B are the two base corners
  POINT tip, baseA, baseB;
  switch (Form){
    case FORM_ARROWUP:
      tip   = {rect.left + w/2 + offset,    rect.top    + margin + offset};
      baseA = {rect.left + margin + offset,  rect.bottom - margin + offset};
      baseB = {rect.right - margin + offset, rect.bottom - margin + offset};
      break;
    case FORM_ARROWDOWN:
      tip   = {rect.left + w/2 + offset,    rect.bottom - margin + offset};
      baseA = {rect.left + margin + offset,  rect.top    + margin + offset};
      baseB = {rect.right - margin + offset, rect.top    + margin + offset};
      break;
    case FORM_ARROWLEFT:
      tip   = {rect.left  + margin + offset, rect.top + h/2 + offset};
      baseA = {rect.right - margin + offset, rect.top    + margin + offset};
      baseB = {rect.right - margin + offset, rect.bottom - margin + offset};
      break;
    case FORM_ARROWRIGHT:
      tip   = {rect.right - margin + offset, rect.top + h/2 + offset};
      baseA = {rect.left  + margin + offset, rect.top    + margin + offset};
      baseB = {rect.left  + margin + offset, rect.bottom - margin + offset};
      break;
    default:
      ::EndPaint(Hwnd, &ps);
      return;
  }

  // filled triangle: face color = configured color [0], null pen so Polygon has no built-in border
  HBRUSH faceBrush = ::CreateSolidBrush(Colors[colorIndex][0]);
  HPEN   nullPen   = ::CreatePen(PS_NULL, 0, 0);
  HGDIOBJ orgBrush = ::SelectObject(hdc, faceBrush);
  HGDIOBJ orgPen   = ::SelectObject(hdc, nullPen);
  POINT triangle[3] = {tip, baseA, baseB};
  ::Polygon(hdc, triangle, 3);
  ::SelectObject(hdc, orgBrush);
  ::SelectObject(hdc, orgPen);
  ::DeleteObject(faceBrush);
  ::DeleteObject(nullPen);

  // 3D edge lines — light source from top-left, swap lit/shadow when pressed
  // [2] = bright accent (lit), [1] = dark shadow, [4] = mid-tone (base)
  const COLORREF litColor    = Pressed ? Colors[colorIndex][1] : Colors[colorIndex][2];
  const COLORREF shadowColor = Pressed ? Colors[colorIndex][2] : Colors[colorIndex][1];
  const COLORREF baseColor   = Colors[colorIndex][4];

  HPEN penLit    = ::CreatePen(PS_SOLID, 1, litColor);
  HPEN penShadow = ::CreatePen(PS_SOLID, 1, shadowColor);
  HPEN penBase   = ::CreatePen(PS_SOLID, 1, baseColor);

  POINT p;
  switch (Form){
    case FORM_ARROWUP:
      // left diagonal = lit, right diagonal = shadow, base = neutral
      ::SelectObject(hdc, penLit);
      ::MoveToEx(hdc, tip.x, tip.y, &p); ::LineTo(hdc, baseA.x, baseA.y);
      ::SelectObject(hdc, penShadow);
      ::MoveToEx(hdc, tip.x, tip.y, &p); ::LineTo(hdc, baseB.x, baseB.y);
      ::SelectObject(hdc, penBase);
      ::MoveToEx(hdc, baseA.x, baseA.y, &p); ::LineTo(hdc, baseB.x, baseB.y);
      break;
    case FORM_ARROWDOWN:
      // left diagonal = shadow, right diagonal = lit (tip is at bottom)
      ::SelectObject(hdc, penShadow);
      ::MoveToEx(hdc, tip.x, tip.y, &p); ::LineTo(hdc, baseA.x, baseA.y);
      ::SelectObject(hdc, penLit);
      ::MoveToEx(hdc, tip.x, tip.y, &p); ::LineTo(hdc, baseB.x, baseB.y);
      ::SelectObject(hdc, penBase);
      ::MoveToEx(hdc, baseA.x, baseA.y, &p); ::LineTo(hdc, baseB.x, baseB.y);
      break;
    case FORM_ARROWLEFT:
    case FORM_ARROWRIGHT:
      // top diagonal = lit, bottom diagonal = shadow, base = neutral
      ::SelectObject(hdc, penLit);
      ::MoveToEx(hdc, tip.x, tip.y, &p); ::LineTo(hdc, baseA.x, baseA.y);
      ::SelectObject(hdc, penShadow);
      ::MoveToEx(hdc, tip.x, tip.y, &p); ::LineTo(hdc, baseB.x, baseB.y);
      ::SelectObject(hdc, penBase);
      ::MoveToEx(hdc, baseA.x, baseA.y, &p); ::LineTo(hdc, baseB.x, baseB.y);
      break;
    default: break;
  }

  ::SelectObject(hdc, OrgPen);
  ::DeleteObject(penLit);
  ::DeleteObject(penShadow);
  ::DeleteObject(penBase);

  ::EndPaint(Hwnd, &ps);
}


//***************************************************
// DrawBackground
//
// for drawing the background of the control
// if no background is set a dummy color will be used
//***************************************************
void ttwin::TTButton::DrawBackground(HDC hdc, const bool drawDefaultBackground) {

  const int pressedOffset = Pressed ? BUTTON_OFFSET_PRESSED : 0;

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  POINT offset;
  if (BackgroundImageUpperLeft) {
    offset.x = 0;
    offset.y = 0;
  }
  else {
    offset = CalcOffsetToMainWin();
  }

  offset.x -= pressedOffset;
  offset.y -= pressedOffset;

  if (BackgroundImage && BackgroundImage->Valid()) {
    BackgroundImage->DrawImage(hdc, rect, offset.x, offset.y);
  }
  else if (drawDefaultBackground) {
    HBRUSH brushBackgroundDefault = CreateSolidBrush(BackColor);
    ::SelectObject(hdc, brushBackgroundDefault);
    ::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    ::DeleteObject(brushBackgroundDefault);
  }
}


//******************
// CM_Mouse_LeftDown
//******************
void ttwin::TTButton::CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt){
  if (EnabledWhenInactive || Active != 0){
    Pressed = true;
    if      (Type == BUTTON)     Active = 1;
    else if (Type == BUTTON_INV) Active = 0;
    ::InvalidateRect(Hwnd, NULL, TRUE);
  }
}


//****************
// CM_Mouse_LeftUp
//****************
void ttwin::TTButton::CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt){
  const bool inactive = Type == CHECKBOX_TWOCOLOR ? Active % 2 == 0 : Active == 0;
  if (EnabledWhenInactive || !inactive){
    Pressed = false;
    if (Type == BUTTON){
      Active = 0;
    }
    else if (Type == BUTTON_INV){
      Active = 1;
    }
    else if (Type == CHECKBOX_TWOCOLOR){ 
      switch (Active){
        case 0: Active = 1; break;
        case 1: Active = 0; break;
        case 2: Active = 3; break;
        case 3: Active = 2; break;
      }
    }
    else if (Type == THREESTATE){ 
      ++Active;
      if (Active >= 3) Active = 0;
    }
    else if (Type == CHECKBOX){ 
      Active = Active == 0 ? 1 : 0;
    }
    else if (Type == ONBUTTON){ 
      Active = 1; 
    }
    SendClickMessage(shift);
    InvalidateRect(Hwnd, NULL, TRUE);
  }
}

//************************************************
// SendClickMessage
//
// send a WM_COMMAND message like standard sliders
//************************************************
void ttwin::TTButton::SendClickMessage(const bool shift){
  ::SendMessage(Parent,     //HWND to which the message should be sent, our parent of course 
                WM_COMMAND, //the message 
                MAKEWPARAM(ID, BN_CLICKED), //wParam
                //reinterpret_cast<LPARAM>(Hwnd)); //the window handle is strored in lParam 
                static_cast<LPARAM>(ButtonNumber + (shift ? 0x8000 : 0))); //button number
}
