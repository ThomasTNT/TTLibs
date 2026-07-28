#include "TTWinLayout.h"


//*****************************************************************
//*                        TTWinLayout                            *
//*                                                               *
//*                                                               *
//*                   abstract layout manager                     *
//*                                                               *
//*****************************************************************

std::map<ttwin::TTWin*, ttwin::TTLayout*> ttwin::TTLayout::LayoutMap;

//******************************
// constructor of WinLayoutSizes
//******************************
ttwin::WinLayoutSizes::WinLayoutSizes(TTWin * const win, const int width, const int height, const int rowSpace)
  : Win(win), Width(width), Height(height), RowSpace(rowSpace)
{
}


//********
// getter
//********
ttwin::TTLayoutPrefSizes ttwin::TTLayout::GetConfigPrefSizes() const {
  return ConfigPrefSizes;
}

//********
// getter
//********
ttwin::TTLayoutPrefSizes& ttwin::TTLayout::GetConfigPrefSizes() {
  return ConfigPrefSizes;
}

//********
// setter
//********
void ttwin::TTLayout::SetConfigPrefSizes(const TTLayoutPrefSizes& configPrefSizes) {
  ConfigPrefSizes = configPrefSizes;
}

//********
// getter
//********
std::string ttwin::TTLayout::GetName() const { 
  return Name;  
}

//********
// setter
//********
void ttwin::TTLayout::SetName(const std::string& name) { 
  Name = name; 
}

//********
// setter
//********
void ttwin::TTLayout::SetParent(TTLayout * const parent) {
  Parent = parent;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTLayout::GetWinX(TTWin * const win) const {
  const TTLayout* const layout = GetLayoutOfWin(win);
  return layout ? layout->GetWinX(win) : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTLayout::GetWinY(TTWin * const win) const {
  const TTLayout* const layout = GetLayoutOfWin(win);
  return layout ? layout->GetWinY(win) : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTLayout::GetWinWidth(TTWin * const win) const {
  const TTLayout* const layout = GetLayoutOfWin(win);
  return layout ? layout->GetWinWidth(win) : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTLayout::GetWinHeight(TTWin * const win) const {
  const TTLayout* const layout = GetLayoutOfWin(win);
  return layout ? layout->GetWinHeight(win) : 0;
}


//***********************************
// DrawLayout
//
// draws layout border for debugging
//**********************************
void ttwin::TTLayout::DrawLayout(HDC hdc) {

  //draw children first 
  for (auto childLayout : Children) {
    if (childLayout) childLayout->DrawLayout(hdc);
  }


  HPEN pen = CreatePen(PS_SOLID, 2, RGB(200, 200, 0));
  HGDIOBJ orgPen = SelectObject(hdc, pen);
  POINT point;

  const int x = GetAbsoluteX();
  const int y = GetAbsoluteY();

  MoveToEx(hdc, x, y, &point);
  LineTo(hdc, x + Size.Width, y);
  LineTo(hdc, x + Size.Width, y + Size.Height);
  LineTo(hdc, x, y + Size.Height);
  LineTo(hdc, x, y);

  SelectObject(hdc, orgPen);
  DeleteObject(pen);
}

//*************
// GetAbsoluteX
//*************
int ttwin::TTLayout::GetAbsoluteX() const {
  if (Parent) {
    return Size.X + Parent->GetAbsoluteX();
  }
  else {
    return Size.X;
  }
}

//*************
// GetAbsoluteÝ
//*************
int ttwin::TTLayout::GetAbsoluteY() const {
  if (Parent) {
    return Size.Y + Parent->GetAbsoluteY();
  }
  else {
    return Size.Y;
  }
}


//***********************
// DrawHatchRect
//
// help for debug drawing
//***********************
void ttwin::TTLayout::DrawHatchRect(HDC hdc, const int x, const int y, const int w, const int h, const COLORREF color) {

  SetBkMode(hdc, TRANSPARENT);
  HBRUSH brush = CreateHatchBrush(HS_BDIAGONAL, color);
  RECT rect;
  rect.left = x;
  rect.right = x + w;
  rect.top = y;
  rect.bottom = y + h;
  FillRect(hdc, &rect, brush);
  DeleteObject(brush);

  /*HPEN pen = CreatePen(PS_SOLID, 2, color);
  HGDIOBJ orgPen = SelectObject(hdc, pen);
  POINT point;

  MoveToEx(hdc, Size.X, Size.Y, &point);
  LineTo(hdc, Size.X + Size.Width, Size.Y);
  LineTo(hdc, Size.X + Size.Width, Size.Y + Size.Height);
  LineTo(hdc, Size.X, Size.Y + Size.Height);
  LineTo(hdc, Size.X, Size.Y);

  SelectObject(hdc, orgPen);
  DeleteObject(pen);*/
}


//**********************************************
// GetLayoutOfWin
//
// gets the actual layout containing this window
//**********************************************
const ttwin::TTLayout* ttwin::TTLayout::GetLayoutOfWin(TTWin* const win) const {
  if (LayoutMap.find(win) != LayoutMap.end()) {
    return LayoutMap.at(win);
  }
  else {
    return nullptr;
  }
}


//*********************************************
// SetInitializeSize
//
// will be called to set the size of the layout
//*********************************************
void ttwin::TTLayout::SetInitializeSize(const int width, const int height) {
  Size.InitialWidth = width;
  Size.InitialHeight = height;
}


//*********************************************
// CalcInitialSizes
// 
// will be called to set the size of the layout
//*********************************************
void ttwin::TTLayout::CalcInitialSizes(){

  //get what we and our children want 
  const TTLayoutPrefSizes initdims = CalcTargetSizes();

  //set layout and also into children
  SetSizes(0, 0, Size.InitialWidth, Size.InitialHeight);
}

//*********************************************
// GetPreferedSize
//
// gets the size we prefer
//*********************************************
ttwin::TTLayoutPrefSizes ttwin::TTLayout::CalcTargetSizes() {
  //nothing to do here, we are a simple layout without children
  //we just use configured values
  TargetSizes = ConfigPrefSizes;
  return TargetSizes;
}

//*********************************************************
// GetTargetSizes
//
// simply returns target sizes, have to be calculated first
//*********************************************************
ttwin::TTLayoutPrefSizes ttwin::TTLayout::GetTargetSizes() const {
  return TargetSizes;
}

//********************************************************
// SetSizes
//
// actually set size and recursively those of the children
//********************************************************
void ttwin::TTLayout::SetSizes(const int x, const int y, const int width, const int height) {
  Size.X = x;
  Size.Y = y;
  Size.WidthOld = Size.Width;
  Size.HeightOld = Size.Height;
  Size.Width = width;
  Size.Height = height;
}

//*********************************************
// Resize
//
// will be called to set the size of the layout
//*********************************************
void ttwin::TTLayout::Resize(const int x, const int y, const int width, const int height) {
  Size.X = x;
  Size.Y = y;
  Size.WidthOld = Size.Width;
  Size.HeightOld = Size.Height;
  Size.Width = width;
  Size.Height = height;
  //ResizeChilden();

  SetSizes(0, 0, Size.Width, Size.Height);
}



//*****************************************************************
//*                        TTSplitLayout                          *
//*                                                               *
//*                                                               *
//*                   a simple layout manager                     *
//*                                                               *
//*****************************************************************

//********************
// default constructor
//********************
ttwin::TTSplitLayout::TTSplitLayout() {
  Children.push_back(nullptr);
  Children.push_back(nullptr);
}

//*************
// constructor
//*************
ttwin::TTSplitLayout::TTSplitLayout(const TTLayoutOrientation orientation,
                                    TTLayout * const layout1,
                                    TTLayout * const layout2,
                                    const float ratioInitialSize,
                                    const float increaseRatio)
  : Orientation(orientation), 
    RatioInitialSize(ratioInitialSize),
    IncreaseRatio(increaseRatio)
{
  Children.push_back(nullptr);
  Children.push_back(nullptr);
  SetFirst(layout1);
  SetSecond(layout2);
}

//************
// destructor 
//************
ttwin::TTSplitLayout::~TTSplitLayout() {
  delete Layout1;
  delete Layout2;
}

//****************
// SetOrientation 
//****************
void ttwin::TTSplitLayout::SetOrientation(const TTLayoutOrientation orientation) {
  Orientation = orientation;
}

//*********
// SetFirst 
//*********
void ttwin::TTSplitLayout::SetFirst(TTLayout * const layout1) {
  delete Layout1;
  Layout1 = layout1;
  Children[0] = Layout1;
  if (Layout1) Layout1->SetParent(this);
}

//**********
// SetSecond 
//**********
void ttwin::TTSplitLayout::SetSecond(TTLayout * const layout2) {
  delete Layout2;
  Layout2 = layout2;
  Children[1] = Layout2;
  if (Layout2) Layout2->SetParent(this);
}

//********************
// SetRatioInitialSize 
//********************
void ttwin::TTSplitLayout::SetRatioInitialSize(const float ratioInitialSize) {
  RatioInitialSize = ratioInitialSize;
} 

//*****************
// SetIncreaseRatio
//*****************
void ttwin::TTSplitLayout::SetIncreaseRatio(const float increaseRatio) {
  IncreaseRatio = increaseRatio;
}

//***********************************
// DrawLayout
//
// draws layout border for debugging
//**********************************
void ttwin::TTSplitLayout::DrawLayout(HDC hdc) {

  TTLayout::DrawLayout(hdc);

  //DrawMissingAreas(hdc);

}

//******************
// DrawMissingAreas
//******************
void ttwin::TTSplitLayout::DrawMissingAreas(HDC hdc) {

  /*if (Layout1 && !Layout2) {
    if (Orientation == TTLayoutOrientation::HORIZONTAL) {
      const int w1 = Size.Width / 2;
      const int w2 = Size.Width - w1;
      DrawHatchRect(hdc, Size.X, Size.Y, w1, Size.Height, RGB(200, 0, 0));
      DrawHatchRect(hdc, Size.X + w1, Size.Y, w2, Size.Height, RGB(0, 200, 200));
    }
    else if (Orientation == TTLayoutOrientation::VERTICAL) {
      const int h1 = Size.Height / 2;
      const int h2 = Size.Height - h1;
      DrawHatchRect(hdc, Size.X, Size.Y, Size.Width, h1, RGB(200, 0, 0));
      DrawHatchRect(hdc, Size.X, Size.Y + h1, Size.Width, h2, RGB(0, 200, 200));
    }
  }
  else if (Layout1 && !Layout2) {
    if (Orientation == TTLayoutOrientation::HORIZONTAL) {
      const int w1 = Layout1->GetSize().Width;
      const int w2 = Size.Width - w1;
      DrawHatchRect(hdc, Size.X + w1, Size.Y, w2, Size.Height, RGB(0, 200, 200));
    }
    else if (Orientation == TTLayoutOrientation::VERTICAL) {
      const int h1 = Layout1->GetSize().Height;
      const int h2 = Size.Height - h1;
      DrawHatchRect(hdc, Size.X, Size.Y, Size.Width, h2, RGB(0, 200, 200));
    }
  }
  else if (!Layout1 && Layout2) {
    if (Orientation == TTLayoutOrientation::HORIZONTAL) {
      const int w2 = Layout2->GetSize().Width;
      const int w1 = Size.Width - w2;
      DrawHatchRect(hdc, Size.X, Size.Y, w1, Size.Height, RGB(200, 0, 0));
    }
    else if (Orientation == TTLayoutOrientation::VERTICAL) {
      const int h2 = Layout2->GetSize().Height;
      const int h1 = Size.Height - h2;
      DrawHatchRect(hdc, Size.X, Size.Y, Size.Width, h1, RGB(200, 0, 0));
    }
  }*/
}

//***************
// GetInitialSize
//***************
ttwin::TTLayoutPrefSizes ttwin::TTSplitLayout::CalcTargetSizes(){

  if (!Layout1 && !Layout2) {
    //no sublayout -> use own Dimensions
  }
  else if ((Layout1 && !Layout2) || (!Layout1 && Layout2)) {
    //get valid child layout's target sizes
    TTLayoutPrefSizes targetChildren = Layout1 ? Layout1->CalcTargetSizes() : Layout2->CalcTargetSizes();
    
    //apply own dimensions additionally
    TargetSizes.MinimalWidth = max(targetChildren.MinimalWidth, ConfigPrefSizes.MinimalWidth);
    TargetSizes.MinimalHeight = max(targetChildren.MinimalHeight, ConfigPrefSizes.MinimalHeight);
    //maximum is infinite, because the empty side can be streched 
    TargetSizes.MaximalWidth = ConfigPrefSizes.MaximalWidth;
    TargetSizes.MaximalHeight = ConfigPrefSizes.MaximalHeight;

    if (targetChildren.PreferedWidth > 0){
      if (ConfigPrefSizes.PreferedWidth > 0) {
        TargetSizes.PreferedWidth = (targetChildren.PreferedWidth + ConfigPrefSizes.PreferedWidth) / 2;
      }
      else {
        TargetSizes.PreferedWidth = targetChildren.PreferedWidth;
      }
    }
    else {
      TargetSizes.PreferedWidth = ConfigPrefSizes.PreferedWidth;
    }
    if (TargetSizes.PreferedWidth < TargetSizes.MinimalWidth) TargetSizes.PreferedWidth = TargetSizes.MinimalWidth;
    if (TargetSizes.PreferedWidth > TargetSizes.MaximalWidth) TargetSizes.PreferedWidth = TargetSizes.MaximalWidth;

    if (targetChildren.PreferedHeight > 0) {
      if (ConfigPrefSizes.PreferedHeight > 0) {
        TargetSizes.PreferedHeight = (targetChildren.PreferedHeight + ConfigPrefSizes.PreferedHeight) / 2;
      }
      else {
        TargetSizes.PreferedHeight = targetChildren.PreferedHeight;
      }
    }
    else {
      TargetSizes.PreferedHeight = ConfigPrefSizes.PreferedHeight;
    }
    if (TargetSizes.PreferedHeight < TargetSizes.MinimalHeight) TargetSizes.PreferedHeight = TargetSizes.MinimalHeight;
    if (TargetSizes.PreferedHeight > TargetSizes.MaximalHeight) TargetSizes.PreferedHeight = TargetSizes.MaximalHeight;

  }
  else { //both
    TTLayoutPrefSizes target1 = Layout1->CalcTargetSizes();
    TTLayoutPrefSizes target2 = Layout2->CalcTargetSizes();
   
    if (Orientation == TTLayoutOrientation::HORIZONTAL) {
      
      //width will be divided, means added
      TargetSizes.MinimalWidth = 0;
      if (target1.MinimalWidth > 0) TargetSizes.MinimalWidth += target1.MinimalWidth;
      if (target2.MinimalWidth > 0) TargetSizes.MinimalWidth += target2.MinimalWidth;
      if (TargetSizes.MinimalWidth < ConfigPrefSizes.MinimalWidth) TargetSizes.MinimalWidth = ConfigPrefSizes.MinimalWidth;

      if (target1.MaximalWidth < MAX_INT && target2.MaximalWidth < MAX_INT) {
        TargetSizes.MaximalWidth = target1.MaximalWidth + target2.MaximalWidth;
        if (ConfigPrefSizes.MaximalWidth < TargetSizes.MaximalWidth) TargetSizes.MaximalWidth = ConfigPrefSizes.MaximalWidth;
      }
      else {
        TargetSizes.MaximalWidth = ConfigPrefSizes.MaximalHeight;
      }

      //calc prefered width
      if (target1.PreferedWidth > 0 && target2.PreferedWidth > 0) {
        TargetSizes.PreferedWidth = target1.PreferedWidth + target2.PreferedWidth;
      }
      else {
        if (target1.PreferedWidth > 0) {
          if (ConfigPrefSizes.PreferedWidth > 0) {
            TargetSizes.PreferedWidth = max(target1.PreferedWidth, ConfigPrefSizes.PreferedWidth);
          }
          else {
            TargetSizes.PreferedWidth = target1.PreferedWidth;
          }
        }
        else if (target2.PreferedWidth > 0) {
          if (ConfigPrefSizes.PreferedWidth > 0) {
            TargetSizes.PreferedWidth = max(target2.PreferedWidth, ConfigPrefSizes.PreferedWidth);
          }
          else {
            TargetSizes.PreferedWidth = target2.PreferedWidth;
          }
        }
        else {
          TargetSizes.PreferedWidth = ConfigPrefSizes.PreferedWidth;
        }
      }

      //both have same height
      TargetSizes.MinimalHeight = 0;
      if (target1.MinimalHeight > TargetSizes.MinimalHeight) TargetSizes.MinimalHeight = target1.MinimalHeight;
      if (target2.MinimalHeight > TargetSizes.MinimalHeight) TargetSizes.MinimalHeight = target2.MinimalHeight;
      if (ConfigPrefSizes.MinimalHeight > TargetSizes.MinimalHeight) TargetSizes.MinimalHeight = ConfigPrefSizes.MinimalHeight;

      TargetSizes.MaximalHeight = MAX_INT;
      if (target1.MaximalHeight < TargetSizes.MaximalHeight) TargetSizes.MaximalHeight = target1.MaximalHeight;
      if (target2.MaximalHeight < TargetSizes.MaximalHeight) TargetSizes.MaximalHeight = target2.MaximalHeight;
      if (ConfigPrefSizes.MaximalHeight < TargetSizes.MaximalHeight) TargetSizes.MaximalHeight = ConfigPrefSizes.MaximalHeight;
      if (TargetSizes.MaximalHeight < TargetSizes.MinimalHeight) {
        TargetSizes.MaximalHeight = TargetSizes.MinimalHeight;
      }

      if (target1.PreferedHeight > 0 && target2.PreferedHeight > 0) {
        TargetSizes.PreferedHeight = (target1.PreferedHeight + target2.PreferedHeight) / 2;
      }
      else if (target1.PreferedHeight > 0) {
        TargetSizes.PreferedHeight = target1.PreferedHeight;
      }
      else if (target2.PreferedHeight > 0) {
        TargetSizes.PreferedHeight = target2.PreferedHeight;
      }
      else {
        TargetSizes.PreferedHeight = ConfigPrefSizes.PreferedHeight;
      }
      

    }
    else { //vertical

      //height will be divided, means added
      TargetSizes.MinimalHeight = 0;
      if (target1.MinimalHeight > 0) TargetSizes.MinimalHeight += target1.MinimalHeight;
      if (target2.MinimalHeight > 0) TargetSizes.MinimalHeight += target2.MinimalHeight;
      if (TargetSizes.MinimalHeight < ConfigPrefSizes.MinimalHeight) TargetSizes.MinimalHeight = ConfigPrefSizes.MinimalHeight;

      if (target1.MaximalHeight < MAX_INT && target2.MaximalHeight < MAX_INT) {
        TargetSizes.MaximalHeight = target1.MaximalHeight + target2.MaximalHeight;
        if (ConfigPrefSizes.MaximalHeight < TargetSizes.MaximalHeight) TargetSizes.MaximalHeight = ConfigPrefSizes.MaximalHeight;
      }
      else {
        TargetSizes.MaximalHeight = ConfigPrefSizes.MaximalHeight;
      }

      //calc prefered height
      if (target1.PreferedHeight > 0 && target2.PreferedHeight > 0) {
        TargetSizes.PreferedHeight = target1.PreferedHeight + target2.PreferedHeight;
      }
      else {
        if (target1.PreferedHeight > 0) {
          if (ConfigPrefSizes.PreferedHeight > 0) {
            TargetSizes.PreferedHeight = max(target1.PreferedHeight, ConfigPrefSizes.PreferedHeight);
          }
          else {
            TargetSizes.PreferedHeight = target1.PreferedHeight;
          }
        }
        else if (target2.PreferedHeight > 0) {
          if (ConfigPrefSizes.PreferedHeight > 0) {
            TargetSizes.PreferedHeight = max(target2.PreferedHeight, ConfigPrefSizes.PreferedHeight);
          }
          else {
            TargetSizes.PreferedHeight = target2.PreferedHeight;
          }
        }
        else {
          TargetSizes.PreferedHeight = ConfigPrefSizes.PreferedHeight;
        }
      }

      //both have same width
      TargetSizes.MinimalWidth = 0;
      if (target1.MinimalWidth > TargetSizes.MinimalWidth) TargetSizes.MinimalWidth = target1.MinimalWidth;
      if (target2.MinimalWidth > TargetSizes.MinimalWidth) TargetSizes.MinimalWidth = target2.MinimalWidth;
      if (ConfigPrefSizes.MinimalWidth > TargetSizes.MinimalWidth) TargetSizes.MinimalWidth = ConfigPrefSizes.MinimalWidth;

      TargetSizes.MaximalWidth = MAX_INT;
      if (target1.MaximalWidth < TargetSizes.MaximalWidth) TargetSizes.MaximalWidth = target1.MaximalWidth;
      if (target2.MaximalWidth < TargetSizes.MaximalWidth) TargetSizes.MaximalWidth = target2.MaximalWidth;
      if (ConfigPrefSizes.MaximalWidth < TargetSizes.MaximalWidth) TargetSizes.MaximalWidth = ConfigPrefSizes.MaximalWidth;
      if (TargetSizes.MaximalWidth < TargetSizes.MinimalWidth) {
        TargetSizes.MaximalWidth = TargetSizes.MinimalWidth;
      }

      if (target1.PreferedWidth > 0 && target2.PreferedWidth > 0) {
        TargetSizes.PreferedWidth = (target1.PreferedWidth + target2.PreferedWidth) / 2;
      }
      else if (target1.PreferedWidth > 0) {
        TargetSizes.PreferedWidth = target1.PreferedWidth;
      }
      else if (target2.PreferedWidth > 0) {
        TargetSizes.PreferedWidth = target2.PreferedWidth;
      }
      else {
        TargetSizes.PreferedWidth = ConfigPrefSizes.PreferedWidth;
      }
    }
    
  }

  return TargetSizes;
}

//********************************************************
// SetSizes
//
// actually set size and recursively those of the children
//********************************************************
void ttwin::TTSplitLayout::SetSizes(const int x, const int y, const int width, const int height) {
  TTLayout::SetSizes(x, y, width, height);

  if (!Layout1 && !Layout2) {
    //no sublayout -> use own Dimensions
  }
  else if ((Layout1 && !Layout2) || (!Layout1 && Layout2)) {
    TTLayout* const layout = Layout1 ? Layout1 : Layout2;
    const TTLayoutPrefSizes targetsizes = layout->GetTargetSizes();

    if (Orientation == TTLayoutOrientation::HORIZONTAL) {
      const int w = min(targetsizes.MaximalWidth, Size.Width);
      const int h = min(targetsizes.MaximalHeight, Size.Height);
      int xo = 0;
      if (w < Size.Width) {
        if (Layout1) xo = 0;
        else xo = Size.Width - w;
      }
      layout->SetSizes(xo, 0, w, h);
    }
    else { //vertical
      const int w = min(targetsizes.MaximalWidth, Size.Width);
      const int h = min(targetsizes.MaximalHeight, Size.Height);
      int yo = 0;
      if (h < Size.Height) {
        if (Layout1) yo = 0;
        else yo = Size.Height - h;
      }
      layout->SetSizes(0, yo, w, h);
    }
  }
  else { //both
    TTLayoutPrefSizes target1 = Layout1->GetTargetSizes();
    TTLayoutPrefSizes target2 = Layout2->GetTargetSizes();

    if (Orientation == TTLayoutOrientation::HORIZONTAL) {

      //WIDTH
      int w1, w2;
      CalcSplit(width,
                target1.MinimalWidth, target1.MaximalWidth, target1.PreferedWidth,
                target2.MinimalWidth, target2.MaximalWidth, target2.PreferedWidth,
                RatioInitialSize,
                w1, w2);
    
      //HEIGHT
      const int h = height;

      Layout1->SetSizes(0, 0, w1, h);
      Layout2->SetSizes(w1, 0, w2, h);
    }
    else { //vertical
      
      //HEIGHT
      int h1, h2;
      CalcSplit(height,
                target1.MinimalHeight, target1.MaximalHeight, target1.PreferedHeight,
                target2.MinimalHeight, target2.MaximalHeight, target2.PreferedHeight,
                RatioInitialSize,
                h1, h2);
      //WIDTH
      const int w = width;

      Layout1->SetSizes(0, 0, w, h1);
      Layout2->SetSizes(0, h1, w, h2);
    }
  }
}

//**********************************************
// CalcSplit
//
// the algorithm to calculate the splittet sizes
//**********************************************
void ttwin::TTSplitLayout::CalcSplit(const int size,
                                     const int min1, const int max1, const int pref1, 
                                     const int min2, const int max2, const int pref2,
                                     const float ratio,
                                     int& size1, int& size2)
{
  //trivial cases
  if ((min1 + min2) > size) {
    size1 = min1;
    size2 = min2;
    return;
  }
  if ((max1 + max2) < size) {
    size1 = max1;
    size2 = max2;
    return;
  }
  //both have no pref size
  if (pref1 <= 0 && pref2 <= 0) {
    //TODO use old values maybe
    //initial sizes
    size1 = static_cast<int>(static_cast<float>(size) * ratio + 0.5f);
    size2 = size - size1;
    //optimize sizes according min and max
    OptimizeSizes(size, min1, max1, min2, max2, size1, size2);
  }
  //both have pref size
  else if (pref1 > 0 && pref2 > 0) {
    size1 = pref1;
    size2 = pref2;
    OptimizeSizes(size, min1, max1, min2, max2, size1, size2);
  }
  //first has pref size
  else if (pref1 > 0) {
    size1 = pref1;
    size2 = size - size1;
    OptimizeSizes(size, min1, max1, min2, max2, size1, size2);
  }
  //second has pref size
  else if (pref2 > 0) {
    size2 = pref2;
    size1 = size - size2;
    OptimizeSizes(size, min1, max1, min2, max2, size1, size2);
  }

}


//***************************************************************
// OptimizeSizes
//
// checks if sizes are between min and max and corrects it if not
// we know, we are between both, so its possible
//***************************************************************
void ttwin::TTSplitLayout::OptimizeSizes(const int size,
                                         const int min1, const int max1,
                                         const int min2, const int max2,
                                         int& size1, int& size2)
{
  if (size1 > min1 && size1 < max1 && size2 > min2 && size2 < max2) return;

  //simple clipping
  if (size1 < min1) size1 = min1;
  if (size2 < min2) size2 = min2;
  if (size1 > max1) size1 = max1;
  if (size2 > max2) size2 = max2;

  //we are now to small, see where we can add something
  if ((size1 + size2) < size) {
    const float factor = static_cast<float>(size1) / static_cast<float>(size1 + size2);
    const int rest = size - (size1 + size2);
    //step one, add it to one
    const int addto1 = static_cast<int>(static_cast<float>(rest) * factor + 0.5);
    if ((addto1 + size1) < max1) {
      //ok, we add to 1
      const int addto2 = size - (size1 + size2 + addto1);
      //can we add it to 2
      if ((size2 + addto2) < max2) {
        size1 += addto1;
        size2 = size - size1;
      }
      else {
        size2 = max2;
        size1 = size - size2;
      }
    }
    else {
      //we add what we can, and therefore the rest to 2
      size1 = max1;
      //we add the rest to size2
      size2 = size - size1;
    }

  }
  //or reduces it 
  else if ((size1 + size2) > size) {
    const float factor = static_cast<float>(size1) / static_cast<float>(size1 + size2);
    //note: values are negative
    const int rest = size - (size1 + size2);
    const int addto1 = static_cast<int>(static_cast<float>(rest) * factor + 0.5);
    if ((addto1 + size1) > min1) {
      const int addto2 = size - (size1 + size2 + addto1);
      if ((size2 + addto2) > min2) {
        size1 += addto1;
        size2 = size - size1;
      }
      else {
        size2 = min2;
        size1 = size - size2;
      }
    }
    else {
      size1 = min1;
      size2 = size - size1;
    }
  }
}



//*********************************************
// Resize
//
// will be called to set the size of the layout
//*********************************************
//void ttwin::TTSplitLayout::ResizeChilden() {
  //int wRest = Dimensions.Width;
  //int hRest = Dimensions.Height;

  //int w1 = 0;
  //int h1 = 0;

  //get size of layout1
  /*if (Layout1) {
    //if there is an window resize window
    if (Layout1->Win) {
    
    }
    //resize inner layout
    else if (Layout1->Layout){
      //just for test 50% of main
      if (Orientation == TTLayoutOrientation::HORIZONTAL) {
        w1 = Dimensions.Width / 2;
        Layout1->Layout->Resize(Dimensions.X, Dimensions.Y, w1, Dimensions.Height);
        wRest -= w1;
      }
      else {
        h1 = Dimensions.Height / 2;
        Layout1->Layout->Resize(Dimensions.X, Dimensions.Y, Dimensions.Width, h1);
        hRest -= h1;
      }
    }
  }
  //get size of layout2
  if (Layout2) {
    //if there is an window resize window
    if (Layout2->Win) {

    }
    //resize inner layout
    else if (Layout2->Layout) {
      if (Orientation == TTLayoutOrientation::HORIZONTAL) {
        Layout2->Layout->Resize(Dimensions.X + w1, Dimensions.Y, wRest, Dimensions.Height);
      }
      else {
        Layout2->Layout->Resize(Dimensions.X, Dimensions.Y + h1, Dimensions.Width, hRest);
      }
    }
  }*/

//}




//*****************************************************************
//*                       TTBlockLayout                           *
//*                                                               *
//*                                                               *
//*                a kind of flow layout manager                  *
//*                                                               *
//*****************************************************************

//************
// constructor
//************
ttwin::TTBlockLayout::TTBlockLayout(){
  WindowRows.push_back(std::vector<WinLayoutSizes>());
}

//************
// constructor
//************
ttwin::TTBlockLayout::TTBlockLayout(const TTLayoutOrientation orientation) : Orientation(orientation)
{
  WindowRows.push_back(std::vector<WinLayoutSizes>());
}

//************
// constructor
//************
ttwin::TTBlockLayout::TTBlockLayout(const TTLayoutOrientation orientation, const int w, const int h) 
                     : Orientation(orientation)
{
  //all is constant, layout will normally never changed, but it could when there is no choice
  ConfigPrefSizes.PreferedWidth = w;
  ConfigPrefSizes.PreferedHeight = h;
  ConfigPrefSizes.MaximalWidth = w;
  ConfigPrefSizes.MaximalHeight = h;
  ConfigPrefSizes.MinimalWidth = w;
  ConfigPrefSizes.MinimalHeight = h;
  WindowRows.push_back(std::vector<WinLayoutSizes>());
}

//***********
// AddWindow
//***********
void ttwin::TTBlockLayout::AddWindow(TTWin * const win, const int width, const int height) {
  WindowRows.back().emplace_back(win, width, height, NextRowSpace);
  LayoutMap[win] = this;
}

//***********
// NewRow
//***********
void ttwin::TTBlockLayout::NewRow(const int space) {
  WindowRows.push_back(std::vector<WinLayoutSizes>());
  NextRowSpace = space;
}

//*****************
// SetSpaceBetween
//****************
void ttwin::TTBlockLayout::SetSpaceBetween(const int spacebetween) {
  SpaceBetween = spacebetween;
}

//****************
// GetPreferedSize
//****************
ttwin::TTLayoutPrefSizes ttwin::TTBlockLayout::CalcTargetSizes(){
  //there's nothing to do, we are fix
  TargetSizes = ConfigPrefSizes;
  return TargetSizes;
}

//********************************************************
// SetSizes
//
// actually set size and recursively those of the children
//********************************************************
void ttwin::TTBlockLayout::SetSizes(const int x, const int y, const int width, const int height) {
  TTLayout::SetSizes(x, y, width, height);
  
  //add left border
  //add top border

  if (Orientation == TTLayoutOrientation::HORIZONTAL) {
    // calc size
    int w = 0; 
    int h = 0;
    std::vector<int> hs;
    
    for (unsigned int j = 0; j < WindowRows.size(); ++j) {
      const auto& row = WindowRows[j];
      int wcurr = 0;
      int h1 = 0;
      for (unsigned int i = 0; i < row.size(); ++i) {
        const WinLayoutSizes& wls = row[i];
        wcurr += wls.Width; 
        if (i == 0){
          const int hcurr = wls.Height;
          if (hcurr > h1) h1 = hcurr;
        }
        else {
          wcurr += SpaceBetween;
        }
      }
      if (wcurr > w) w = wcurr;
      if (j > 0 && (row.size() > 0)) {
        int& h0 = hs.back();
        h0 += row[0].RowSpace;
      }
      hs.push_back(h1);
      h += h1;
    }

    //TODO alight (left, center, right?) / (top, center, bottom)
    const int xoffset = 0;
    const int yoffset = 0;
    int posY = ConfigPrefSizes.MarginTop + yoffset;
    for (unsigned int j = 0; j < WindowRows.size(); ++j) {
      int posX = ConfigPrefSizes.MarginLeft + xoffset;
      auto& row = WindowRows[j];
      for (unsigned int i = 0; i < row.size(); ++i) {
        WinLayoutSizes& wls = row[i];
        wls.PosX = posX;
        wls.PosY = posY;
        posX += wls.Width;
        posX += SpaceBetween;
      }
      posY += hs[j];
    }
  }
  else { //vertical
    /*for (auto& Win : Windows) {
      Win.posX = posX;
      Win.posY = posY;
      posY += Win.Height;
      posY += SpaceBetween;
    }*/
  }

}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTBlockLayout::GetWinX(TTWin * const win) const {
  const ttwin::WinLayoutSizes* const wls = GetWinLayout(win);
  int ax = this->GetAbsoluteX();
  return wls ? wls->PosX + ax : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTBlockLayout::GetWinY(TTWin * const win) const {
  const ttwin::WinLayoutSizes* const wls = GetWinLayout(win);
  return wls ? wls->PosY + this->GetAbsoluteY() : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTBlockLayout::GetWinWidth(TTWin * const win) const {
  const ttwin::WinLayoutSizes* const wls = GetWinLayout(win);
  return wls ? wls->Width : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTBlockLayout::GetWinHeight(TTWin * const win) const {
  const ttwin::WinLayoutSizes* const wls = GetWinLayout(win);
  return wls ? wls->Height : 0;
}


//*********************************************
// Resize
//
// will be called to set the size of the layout
//*********************************************
void ttwin::TTBlockLayout::ResizeChilden() {

}

//***********
// DrawLayout
//***********
void ttwin::TTBlockLayout::DrawLayout(HDC hdc) {

  /*SetBkMode(hdc, TRANSPARENT);
  HBRUSH brush = CreateSolidBrush(RGB(0,0,150));
  RECT rect;
  rect.left = GetAbsoluteX();
  rect.right = rect.left + Size.Width;
  rect.top = GetAbsoluteY();
  rect.bottom = rect.top + Size.Height;
  FillRect(hdc, &rect, brush);
  DeleteObject(brush);
  */
}

//***********
// DrawLayout
//***********
const ttwin::WinLayoutSizes* ttwin::TTBlockLayout::GetWinLayout(TTWin* const win) const {
  if (!win) return nullptr;
  for (const std::vector<WinLayoutSizes>& row : WindowRows) {
    for (const WinLayoutSizes& wls : row) {
      if (wls.Win == win) {
        return &wls;
      }
    }
  }
  return nullptr;
}

//*****************************************************************
//*                          TTWinLayout                          *
//*                                                               *
//*                                                               *
//*             contains layout infos for one window              *
//*                                                               *
//*****************************************************************

//********************
// default constructor
//********************
ttwin::TTWinLayout::TTWinLayout() {}

//*************
// constructor
//*************
ttwin::TTWinLayout::TTWinLayout(TTWin * const win) {
  SetWindow(win);
}

//***********
// setter
//***********
void ttwin::TTWinLayout::SetWindow(TTWin * const win) {
  Win = win;
  LayoutMap[win] = this;
}

//***********
// DrawLayout
//***********
void ttwin::TTWinLayout::DrawLayout(HDC hdc) {
  SetBkMode(hdc, TRANSPARENT);
  HBRUSH brush = CreateSolidBrush(RGB(0, 150, 0));
  RECT rect;
  rect.left = GetAbsoluteX();
  rect.right = rect.left + Size.Width;
  rect.top = GetAbsoluteY();
  rect.bottom = rect.top + Size.Height;
  FillRect(hdc, &rect, brush);
  DeleteObject(brush);
}
  
//****************
// GetPreferedSize
//****************
ttwin::TTLayoutPrefSizes ttwin::TTWinLayout::CalcTargetSizes() {
  //there's nothing to do we are fix
  TargetSizes = ConfigPrefSizes;
  return TargetSizes;
}
 
//********************************************************
// SetSizes
//
// actually set size and recursively those of the children
//********************************************************
void ttwin::TTWinLayout::SetSizes(const int x, const int y, const int width, const int height) {
  TTLayout::SetSizes(x, y, width, height);
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinLayout::GetWinX(TTWin * const win) const {
  return (win && win == Win) ? GetAbsoluteX() : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinLayout::GetWinY(TTWin * const win) const {
  return (win && win == Win) ? GetAbsoluteY() : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinLayout::GetWinWidth(TTWin * const win) const {
  return (win && win == Win) ? Size.Width : 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinLayout::GetWinHeight(TTWin * const win) const {
  return (win && win == Win) ? Size.Height : 0;
}

//*****************************************************************
//*                       TTWinFlowLayout                         *
//*                                                               *
//*                                                               *
//*            contains layout infos for a list of windows        *
//*                                                               *
//*****************************************************************

//************
// constructor
//************
ttwin::TTWinFlowLayout::TTWinFlowLayout(const TTLayoutOrientation orientation) : Orientation(orientation)
{
}

//**********
// AddWindow
//**********
void ttwin::TTWinFlowLayout::AddWindow(TTWin * const win) {
  Wins.push_back(win);
  LayoutMap[win] = this;
}

//***********
// DrawLayout
//***********
void ttwin::TTWinFlowLayout::DrawLayout(HDC hdc) {
  SetBkMode(hdc, TRANSPARENT);
  HBRUSH brush = CreateSolidBrush(RGB(200, 200, 0));
  RECT rect;
  rect.left = GetAbsoluteX();
  rect.right = rect.left + Size.Width;
  rect.top = GetAbsoluteY();
  rect.bottom = rect.top + Size.Height;
  FillRect(hdc, &rect, brush);
  DeleteObject(brush);
}

//****************
// GetPreferedSize
//****************
ttwin::TTLayoutPrefSizes ttwin::TTWinFlowLayout::CalcTargetSizes() {
  TargetSizes = ConfigPrefSizes;
  //acutally it depends on number and size of subwindows
  return TargetSizes;
}

//********************************************************
// SetSizes
//
// actually set size and recursively those of the children
//********************************************************
void ttwin::TTWinFlowLayout::SetSizes(const int x, const int y, const int width, const int height) {
  TTLayout::SetSizes(x, y, width, height);
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinFlowLayout::GetWinX(TTWin * const win) const {
  return 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinFlowLayout::GetWinY(TTWin * const win) const {
  return 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinFlowLayout::GetWinWidth(TTWin * const win) const {
  return 0;
}

//******************************************
// GetWinX
//
// methods for creating or resizing a window
//******************************************
int ttwin::TTWinFlowLayout::GetWinHeight(TTWin * const win) const {
  return 0;
}

