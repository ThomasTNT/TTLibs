#include "TTMeter.h"



//**********
//contructor
//**********
ttwin::TTMeter::TTMeter() : TTWin("TTMeter")                       
{
                                    
}


//**********************
// Configure
//
// configures the slider
//**********************
void ttwin::TTMeter::Configure(const double stepValue, const double stepBase, const double scaleMax, const double scaleMin, const bool drawNumbers, const MeterBarType type)
{
  StepValue = stepValue;
  StepBase = stepBase;
  ScaleMax = scaleMax;
  ScaleMin = scaleMin;

  DrawScaleNumbers = drawNumbers;
  Type = type;

  CalculateSteps();
}

//*************************
// ConfigureColor
//
// sets the button color 
// return true when changed
//*************************
void ttwin::TTMeter::ConfigureColor(const COLORREF over, const COLORREF head, const COLORREF normal, const COLORREF low){
  ColorOver = over;
  ColorHead = head;
  ColorNormal = normal;
  ColorLow = low;
}

//****************
// SetLevelMeter
//
// sets Levelmeter
//****************
void ttwin::TTMeter::SetLevelMeter(audioplay::LevelMeter* const meter){
  Meter = meter;
  Channels = Meter->GetNumberOfChannels();
  Pos.clear();
  for (unsigned int i = 0; i < Channels; ++i) {
    Pos.push_back(0);
  }
}

//*************************************************
// UpdateContent
// 
// refreshes the model or the content of the window 
// shall call Invalidate() or Update() then
// Used to refresh level meters etc. 
//*************************************************
void ttwin::TTMeter::UpdateContent(){
  for (unsigned int i = 0; i < Channels; ++i) {
    const double dB = Meter ? Meter->GetPeakOfChannelNo(i + 1) : -9999;
    const int oldPos = Pos[i];
    Pos[i] = PosBydB(dB);
  }
  //TODO better methods for update rect
  const RECT updateRect = CalcUpdateRect(0, 0, GetClientRect());
  Invalidate(updateRect.left, updateRect.top, updateRect.right, updateRect.bottom);
}

//**********************
// TimerHandler1
//
// update timing event
// return false for stop
//**********************
bool ttwin::TTMeter::TimerHandler1(){
  UpdateContent();
  return true;
}

//************************************
// Created 
//
// this is called after creation
// hack for calling Init()
// Why is ther no WM_CREATE Message?!?
//************************************
void ttwin::TTMeter::Created(){
  Init(); //just call init

  //starts the update timer
  //StartTimer1(METERTIMER_PERIODE);
}

//*******
// Init
//*******
void ttwin::TTMeter::Init(){
  const RECT rect = GetClientRect();
  const int height = rect.bottom - rect.top;
  ZeroOffset = METER_BORDER;
  MaxPos = height - 2 * ZeroOffset;

  //default colors
  ColorOver   = RGB(220, 10,  5);   //red
  ColorHead   = RGB(250, 200,  10); //yellow
  ColorNormal = RGB(20, 200,  10);  //green
  ColorLow    = RGB(10, 100,  250);   //blue
}


//*********
// PaintAll
//*********
void ttwin::TTMeter::PaintAll(){

  const bool isEnabled = IsWindowEnabled(Hwnd) != FALSE;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(Hwnd, &ps);

  const RECT rect = GetClientRect();
  const int width = rect.right - rect.left;
  const int Xmid = rect.left + width / 2;

  DrawBackground(hdc, true);    

  //scale
  HFONT scaleFont = CreateFont(10,                        // height of font
                               0,                         // average character width
                               0,                         // angle of escapement
                               0,                         // base-line orientation angle
                               400,                       // font weight
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
  OrgFont = SelectObject(hdc, scaleFont);
  ::SetTextAlign(hdc, TA_BASELINE);
  //::SetTextAlign(hdc, TA_CENTER);
  ::SetTextColor(hdc, RGB(255, 255, 255));
  
  HPEN penScale = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
  POINT point; 
  OrgPen = ::SelectObject(hdc, penScale);
 
  for (unsigned int step = 0; step <= StepsPlus; ++step){
    const int y = YByPos(PosZero + static_cast<int>(step * StepSize + 0.5));
    ::MoveToEx(hdc, Xmid - METER_WIDTH / 2 - METER_SCALE_SPACE - METER_SCALE_LENGTH, y, &point);
    ::LineTo  (hdc, Xmid - METER_WIDTH / 2 - METER_SCALE_SPACE, y);
    ::MoveToEx(hdc, Xmid + METER_WIDTH / 2 + METER_SCALE_SPACE, y, &point);
    ::LineTo  (hdc, Xmid + METER_WIDTH / 2 + METER_SCALE_SPACE + METER_SCALE_LENGTH, y);
    if (DrawScaleNumbers){
      ::SelectObject(hdc, scaleFont);
      const double dB = DBByStep(step);
      std::string sl = step == 0 ? "0" : "+" + ttutil::StringUtil::double2string(dB, 0);
      sl = ttutil::StringUtil::formatRight(sl, 3, ' ');
      ::TextOut(hdc, 0, y + 4, sl.c_str(), static_cast<int>(sl.size()));
    }
  }
  for (unsigned int step = 1; step <= StepsMinus; ++step){
    const int y = YByPos(PosZero - static_cast<int>(step * StepSize + 0.5));
    ::MoveToEx(hdc, Xmid - METER_WIDTH / 2 - METER_SCALE_SPACE - METER_SCALE_LENGTH, y, &point);
    ::LineTo  (hdc, Xmid - METER_WIDTH / 2 - METER_SCALE_SPACE, y);
    ::MoveToEx(hdc, Xmid + METER_WIDTH / 2 + METER_SCALE_SPACE, y, &point);
    ::LineTo  (hdc, Xmid + METER_WIDTH / 2 + METER_SCALE_SPACE + METER_SCALE_LENGTH, y);
    if (DrawScaleNumbers){
      ::SelectObject(hdc, scaleFont);
      const double dB = DBByStep(step);
      std::string sl = step >= StepsMinus ? "inf" : "-" + ttutil::StringUtil::double2string(dB, 0);
      sl = ttutil::StringUtil::formatRight(sl, 3, ' ');
      ::TextOut(hdc, 0, y + 4, sl.c_str(), static_cast<int>(sl.size()));
    }
  }

  ::SelectObject(hdc, OrgFont);
  ::DeleteObject(scaleFont);

  ::SelectObject(hdc, OrgPen);   // deselects penScale before deletion
  ::DeleteObject(penScale);

  //slit and bars
  if (Channels > 0) {

    int MeterWidthActual = (METER_WIDTH - ((Channels - 1) * METER_CHANNEL_SPACE)) / Channels;
    if (MeterWidthActual < 1) MeterWidthActual = 1;

    HBRUSH blackbrush = ::CreateSolidBrush(RGB(0, 0, 0));
    HPEN nullPen = CreatePen(PS_NULL,  1, RGB(0, 0, 0));

    HPEN penBorderBright = ::CreatePen(PS_SOLID, 1, RGB(150, 150, 150));
    HPEN penBorderDark = ::CreatePen(PS_SOLID, 1, RGB(90, 90, 90));

    const int posOver = PosZero;

    HPEN dcPen = static_cast<HPEN>(::GetStockObject(DC_PEN));

    HGDIOBJ orgBrush = ::SelectObject(hdc, blackbrush);

    for (unsigned int channel = 0; channel < Channels; ++channel) {

      const int x0 = Xmid - METER_WIDTH / 2 + channel * (MeterWidthActual + METER_CHANNEL_SPACE);
      const int x1 = x0 + MeterWidthActual;
      const int y0 = YByPos(0);
      const int y1 = YByPos(MaxPos);

      ::SelectObject(hdc, blackbrush);
      ::SelectObject(hdc, nullPen);
      ::Rectangle(hdc, x0, y0, x1, y1);

      ::SelectObject(hdc, penBorderBright);
      ::MoveToEx(hdc, x0 - 1, y0 + 1, &point);
      ::LineTo(hdc, x1 - 1, y0 + 1);
      ::LineTo(hdc, x1 - 1, y1 - 1);

      ::SelectObject(hdc, penBorderDark);
      ::MoveToEx(hdc, x1 - 1, y1 - 1, &point);
      ::LineTo(hdc, x0 - 1, y1 - 1);
      ::LineTo(hdc, x0 - 1, y0 - 1);

      ::SelectObject(hdc, dcPen);
      const int step = (Type == METERBARTYPE_STRIPES) ? 2 : 1;
      for (int p = Pos[channel]; p >= 0; p -= step) {
        const COLORREF clr = p > posOver ? ColorOver : MeterGradientColor(p, posOver);
        ::SetDCPenColor(hdc, clr);
        const int y = YByPos(p);
        ::MoveToEx(hdc, x0, y, &point);
        ::LineTo(hdc, x1, y);
      }
    }

    ::SelectObject(hdc, orgBrush);    // deselects blackbrush before deletion
    ::DeleteObject(penBorderDark);
    ::DeleteObject(penBorderBright);
    ::DeleteObject(nullPen);
    ::DeleteObject(blackbrush);
  }

  ::SelectObject(hdc, OrgPen);

  EndPaint(Hwnd, &ps);
}

//***************************************************
// CalcUpdateRect
//
// gets the rectandle to update when slider has moved
//***************************************************
RECT ttwin::TTMeter::CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect){
  
  if (pos1 > pos2){
     int temp = pos2;
     pos2 = pos1;
     pos1 = temp;
  }
  RECT rect = controllerrect;
  rect.left   = rect.left + METER_BORDER;
  rect.right  = rect.right + METER_BORDER;
  //rect.top    = controllerrect.bottom - pos1 - ZeroOffset - FaderBmpHeight / 2 - 2;
  //rect.bottom = controllerrect.bottom - pos2 - ZeroOffset + FaderBmpHeight / 2 + 2;
  return rect;
}

//***********************************************
// MeterGradientColor
//
// pos 0..posOver mapped to hue 240 (blue) -> 15 (deep orange)
// using non-linear key stops: short green zone, wide orange zone
//***********************************************
COLORREF ttwin::TTMeter::MeterGradientColor(const int pos, const int posOver) {
  struct Stop { float t; float hue; };
  static const Stop stops[] = {
    {0.00f, 240.0f}, // blue
    {0.20f, 180.0f}, // cyan
    {0.40f, 120.0f}, // green
    {0.55f,  80.0f}, // green ends quickly
    {0.68f,  55.0f}, // yellow
    {0.82f,  28.0f}, // orange
    {1.00f,  12.0f}, // deep orange, just before red
  };
  static const int nStops = 7;

  const float t = posOver > 0 ? static_cast<float>(pos) / static_cast<float>(posOver) : 0.0f;

  float hue = stops[0].hue;
  for (int i = 0; i < nStops - 1; ++i) {
    if (t <= stops[i + 1].t) {
      const float f = (t - stops[i].t) / (stops[i + 1].t - stops[i].t);
      hue = stops[i].hue + f * (stops[i + 1].hue - stops[i].hue);
      break;
    }
    hue = stops[nStops - 1].hue;
  }

  const float hh = hue / 60.0f;
  const float x = 1.0f - std::abs(std::fmod(hh, 2.0f) - 1.0f);
  float r, g, b;
  if (hh < 1.0f) { r = 1.0f; g = x;    b = 0.0f; }
  else if (hh < 2.0f) { r = x;    g = 1.0f; b = 0.0f; }
  else if (hh < 3.0f) { r = 0.0f; g = 1.0f; b = x; }
  else if (hh < 4.0f) { r = 0.0f; g = x;    b = 1.0f; }
  else { r = x;    g = 0.0f; b = 1.0f; }
  return RGB(static_cast<int>(r * 255.0f + 0.5f), static_cast<int>(g * 255.0f + 0.5f), static_cast<int>(b * 255.0f + 0.5f));
}

//*********************
// HELP: CalculateSteps
//*********************
void ttwin::TTMeter::CalculateSteps(){

  if (ScaleMax < 0.0) ScaleMax = 0.0;
  if (ScaleMax == 0.0) {
    StepsPlus = 0.0;
  }
  else{
    StepsPlus = StepBydB(ScaleMax);
  }

  if (ScaleMin > 0.0) ScaleMin = 0.0;
  if (ScaleMin == 0.0) {
    StepsMinus = 0.0;
  }
  else{
    StepsMinus = StepBydB(-ScaleMin);
  }

  StepSize = static_cast<double>(MaxPos) / static_cast<double>(StepsMinus + StepsPlus);
  PosZero = MaxPos - static_cast<int>(StepsPlus * StepSize + 0.5);
}

//*******************
// Step -> dB
//
// taken from TTFader
//*******************
double ttwin::TTMeter::DBByStep(double step){
  const bool signPositiv = step >= 0.0;
  if (!signPositiv) step = -step;
  const double db = step > 1.0 ? StepValue * pow(StepBase, step - 1.0) : StepValue * step;
  return signPositiv ? db : -db;
}


//*******************
// dB -> Step
//
// taken from TTFader
//*******************
double ttwin::TTMeter::StepBydB(double db){
  const bool signPositiv = db >= 0.0;
  if (!signPositiv) db = -db;
  const double steps = db > StepValue ? 1.0 + (log(db) - log(StepValue)) / log(StepBase) : db / StepValue;
  return signPositiv ? steps : -steps;
}

//***********
// Pos -> dB
//***********
double ttwin::TTMeter::DBbyPos(const int pos){
  if (pos == 0){
    return -99999999;
  }
  else if (pos == PosZero){
    return 0.0;
  }
  else if (pos > PosZero){
    const double steps = static_cast<double>(pos - PosZero) / StepSize;
    const double dB = DBByStep(steps);
    return dB;
  }
  else{
    const double steps = static_cast<double>(PosZero - pos) / StepSize;
    const double dB = -DBByStep(steps);
    return dB;
  }
}

//********************
// dB -> Pos 
//
// taken from TTFader
//*******************
int ttwin::TTMeter::PosBydB(const double db){
  int pos;
  if (db == 0.0){
    pos = PosZero;
  }
  else if (db > 0.0){
    const double steps = StepBydB(db);
    pos = PosZero + static_cast<int>(steps * StepSize + 0.5);
  }
  else{
    const double steps = StepBydB(-db);
    pos = PosZero - static_cast<int>(steps * StepSize + 0.5);
  }
  if (pos > MaxPos) pos = MaxPos;
  if (pos < 0)      pos = 0;
  return pos;
}

//*******************
// Pos -> y
//
// taken from TTFader
//*******************
int ttwin::TTMeter::YByPos(const int pos){
  return MaxPos + ZeroOffset - pos;
}
