#include "TTScrollArea.h"

static constexpr int TITLE_HEIGHT = 18;
static constexpr int BORDER       = 1;

//*************
// TTScrollArea
//*************
ttwin::TTScrollArea::TTScrollArea() : TTWin("TTScrollArea")
{
}

//***************
// ~TTScrollArea
//***************
ttwin::TTScrollArea::~TTScrollArea(){}

//*********************
// GetExtraWindowStyle
//*********************
DWORD ttwin::TTScrollArea::GetExtraWindowStyle() const {
  return WS_VSCROLL | WS_CLIPCHILDREN;
}

//***********
// SetTitle
//***********
void ttwin::TTScrollArea::SetTitle(const std::string& title){
  Label = title;
  if (Hwnd) Invalidate();
}

//*****************
// SetVirtualHeight
//*****************
void ttwin::TTScrollArea::SetVirtualHeight(const int height){
  VirtualHeight = height;
  if (Hwnd) UpdateScrollInfo();
}

//***********
// GetScrollY
//***********
int ttwin::TTScrollArea::GetScrollY() const {
  return ScrollY;
}

//***********
// GetInnerTop
//***********
int ttwin::TTScrollArea::GetInnerTop() const {
  return (Label.empty() ? 0 : TITLE_HEIGHT) + BORDER;
}

//***********
// GetInnerLeft
//***********
int ttwin::TTScrollArea::GetInnerLeft() const {
  return BORDER;
}

//************
// GetInnerWidth
//************
int ttwin::TTScrollArea::GetInnerWidth() const {
  RECT r;
  ::GetClientRect(Hwnd, &r);
  const int w = r.right - BORDER * 2;
  return w > 0 ? w : 0;
}

//*****************
// UpdateScrollInfo
//*****************
void ttwin::TTScrollArea::UpdateScrollInfo(){
  if (!Hwnd) return;
  RECT r;
  ::GetClientRect(Hwnd, &r);
  const int titleH    = Label.empty() ? 0 : TITLE_HEIGHT;
  const int contentH  = r.bottom - titleH - BORDER * 2;
  const int clampedH  = contentH > 0 ? contentH : 0;
  const int maxScroll = VirtualHeight - clampedH;
  if (ScrollY > maxScroll) ScrollY = maxScroll > 0 ? maxScroll : 0;
  if (ScrollY < 0) ScrollY = 0;
  SCROLLINFO si{};
  si.cbSize = sizeof(si);
  si.fMask  = SIF_RANGE | SIF_POS | SIF_PAGE;
  si.nMin   = 0;
  si.nMax   = VirtualHeight > 0 ? VirtualHeight - 1 : 0;
  si.nPage  = static_cast<UINT>(clampedH);
  si.nPos   = ScrollY;
  ::SetScrollInfo(Hwnd, SB_VERT, &si, TRUE);
}

//*******************
// NotifyParentScrolled
//*******************
void ttwin::TTScrollArea::NotifyParentScrolled(){
  if (Parent) ::SendMessage(Parent, WM_COMMAND, MAKEWPARAM(ID, BN_CLICKED), reinterpret_cast<LPARAM>(Hwnd));
}

//*********
// PaintAll
//*********
void ttwin::TTScrollArea::PaintAll(){
  PAINTSTRUCT ps;
  HDC hdc = ::BeginPaint(Hwnd, &ps);

  RECT rect;
  ::GetClientRect(Hwnd, &rect);

  DrawBackground(hdc, true);

  // 1px outer border
  HPEN   borderPen = ::CreatePen(PS_SOLID, 1, RGB(25, 25, 25));
  HPEN   oldPen    = static_cast<HPEN>  (::SelectObject(hdc, borderPen));
  HBRUSH oldBrush  = static_cast<HBRUSH>(::SelectObject(hdc, ::GetStockObject(NULL_BRUSH)));
  ::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
  ::SelectObject(hdc, oldPen);
  ::SelectObject(hdc, oldBrush);
  ::DeleteObject(borderPen);

  // title bar (only when title is set)
  if (!Label.empty()) {
    const RECT titleRect = { rect.left + BORDER, rect.top + BORDER, rect.right - BORDER, rect.top + TITLE_HEIGHT };
    HBRUSH titleBrush = ::CreateSolidBrush(RGB(50, 60, 70));
    ::FillRect(hdc, &titleRect, titleBrush);
    ::DeleteObject(titleBrush);

    HFONT font    = ::CreateFont(FontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                  ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  ANTIALIASED_QUALITY, VARIABLE_PITCH | FF_SWISS, "MS Shell Dlg");
    HFONT oldFont = static_cast<HFONT>(::SelectObject(hdc, font));
    ::SetBkMode(hdc, TRANSPARENT);
    ::SetTextColor(hdc, RGB(180, 180, 180));
    ::SetTextAlign(hdc, TA_LEFT | TA_TOP);
    const int textY = titleRect.top + (TITLE_HEIGHT - FontHeight) / 2;
    ::TextOut(hdc, titleRect.left + 4, textY, Label.c_str(), static_cast<int>(Label.size()));
    ::SelectObject(hdc, oldFont);
    ::DeleteObject(font);
  }

  ::EndPaint(Hwnd, &ps);
}

//**************
// ResizesFields
//**************
void ttwin::TTScrollArea::ResizesFields(){
  UpdateScrollInfo();
  NotifyParentScrolled();
}

//***********
// CM_VScroll
//***********
void ttwin::TTScrollArea::CM_VScroll(const int scrollCode, const int trackPos){
  const int oldY = ScrollY;
  switch (scrollCode) {
    case SB_LINEUP:        ScrollY -= 20;      break;
    case SB_LINEDOWN:      ScrollY += 20;      break;
    case SB_PAGEUP:        ScrollY -= 60;      break;
    case SB_PAGEDOWN:      ScrollY += 60;      break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION: ScrollY = trackPos; break;
  }
  if (ScrollY < 0) ScrollY = 0;
  if (ScrollY != oldY) {
    UpdateScrollInfo();
    NotifyParentScrolled();
    Invalidate();
  }
}

//***************
// CM_MouseWheel
//***************
void ttwin::TTScrollArea::CM_MouseWheel(const int delta){
  const int oldY = ScrollY;
  ScrollY -= (delta / WHEEL_DELTA) * 20;
  if (ScrollY < 0) ScrollY = 0;
  if (ScrollY != oldY) {
    UpdateScrollInfo();
    NotifyParentScrolled();
    Invalidate();
  }
}
