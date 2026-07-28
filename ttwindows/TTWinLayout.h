#pragma once

#include "TTWin.h"

namespace ttwin {

enum TTLayoutOrientation { VERTICAL, HORIZONTAL };

// This is a different approach than layout managers in swing and the like.
// Here the layout manager calculates the values outside of the window.
const int MAX_INT = 999999999;

// help struct for layouts containing windows
class WinLayoutSizes {
  public:
    WinLayoutSizes(TTWin * const win, const int width, const int height, const int rowSpace = 0);
    TTWin * Win; 
    int Width = 0;
    int Height = 0;
    int PosX = 0;
    int PosY = 0;
    int RowSpace = 0;
};


class TTLayoutPrefSizes {

  public:
    //tells layout manager, the optimal size
    int PreferedWidth = -1;
    int PreferedHeight = -1;

    //minmal dimension
    int MinimalWidth = 0;
    int MinimalHeight = 0;

    //maximal dimension
    int MaximalWidth = MAX_INT;
    int MaximalHeight = MAX_INT;

    //this will be used layouts that contains layouts with prefered sizes, but could stredge even more
    //because it contains other sizes 
    int PreferedWidthMin = -1;
    int PreferedWidthMax = MAX_INT;
    int PreferedHeightMin = -1;
    int PreferedHeightMax = MAX_INT;
  
    //unit
    TTControlUnit Unit = PIXEL;

    //margin 
    int MarginLeft = 0;
    int MarginRight = 0;
    int MarginTop = 0;
    int MargingBottom = 0;
};


class TTLayoutSizes {
  public:

    //current dimensioms
    //these are relative to the parent layout
    int X = 0;
    int Y = 0;
    int Width = 0;
    int Height = 0;

    //used to calculate resizing
    int WidthOld = 0;
    int HeightOld = 0;

    //initial size
    //used to calculate resizing
    int InitialWidth = -1;
    int InitialHeight = -1;

};

//*****************************************************************
//*                        TTWinLayout                            *
//*                                                               *
//*                                                               *
//*                   abstract layout manager                     *
//*                                                               *
//*****************************************************************


class TTLayout{

  public:

    static std::map<TTWin*, TTLayout*> LayoutMap;

    //------------------------------ initialisation ---------------------------------

    //virtual destructor
    virtual ~TTLayout() {};

    //setter and getter for dimensions
    void SetConfigPrefSizes(const TTLayoutPrefSizes& configPrefSizes);
    TTLayoutPrefSizes GetConfigPrefSizes() const;
    TTLayoutPrefSizes& GetConfigPrefSizes();

    //will be called to set the size of the layout initially
    void SetInitializeSize(const int width, const int height);

    //will be called to set the size of the layout
    void CalcInitialSizes();

    //is called in CalcInitialSizes
    virtual TTLayoutPrefSizes CalcTargetSizes();
    TTLayoutPrefSizes GetTargetSizes() const;

    //actually set size and recursively those of the children
    virtual void SetSizes(const int x, const int y, const int width, const int height);

    //will be called to set the size of the layout
    void Resize(const int x, const int y, const int width, const int height);

    //debug
    virtual void DrawLayout(HDC hdc);

    //setter and getter for name
    std::string GetName() const;
    void SetName(const std::string& name);

    void SetParent(TTLayout * const parent);

    //methods for creating or resizing a window
    virtual int GetWinX(TTWin * const win) const;
    virtual int GetWinY(TTWin * const win) const;
    virtual int GetWinWidth(TTWin * const win) const;
    virtual int GetWinHeight(TTWin * const win) const;


  protected:

    // is called in Resize
    //virtual void ResizeChilden() = 0;

    int GetAbsoluteX() const;
    int GetAbsoluteY() const;

    // help for debuf drawing
    void DrawHatchRect(HDC hdc, const int x, const int y, const int w, const int h, const COLORREF color);

    // gets the actual layout containing this window
    const TTLayout* GetLayoutOfWin(TTWin* const win) const;


    std::vector<TTLayout*> Children;
    TTLayout* Parent = nullptr;

    TTLayoutPrefSizes ConfigPrefSizes;
    TTLayoutPrefSizes TargetSizes;
    TTLayoutSizes Size;

    std::string Name;


};



//*****************************************************************
//*                        TTSplitLayout                          *
//*                                                               *
//*                                                               *
//*                   a simple layout manager                     *
//*                                                               *
//*****************************************************************
class TTSplitLayout : public TTLayout {

  public:

    TTSplitLayout();
    TTSplitLayout(const TTLayoutOrientation orientation,
                  TTLayout * const layout1,
                  TTLayout * const layout2,
                  const float ratioInitialSize = 0.5f,
                  const float increaseRatio = 0.5f);

    virtual ~TTSplitLayout();

    void SetOrientation(const TTLayoutOrientation orientation);
    
    void SetFirst(TTLayout * const layout1);

    void SetSecond(TTLayout * const layout2);

    void SetRatioInitialSize(const float ratioInitialSize);

    void SetIncreaseRatio(const float increaseRatio);

    //debug
    virtual void DrawLayout(HDC hdc);

    virtual TTLayoutPrefSizes CalcTargetSizes();

    //actually set size and recursively those of the children
    virtual void SetSizes(const int x, const int y, const int width, const int height);

  protected:

    //will be called to set the size of the layout
    //virtual void ResizeChilden();

    void DrawMissingAreas(HDC hdc);

    //HELP:
    //the algorithm to calculate the splittet sizes
    void CalcSplit(const int size,
                   const int min1, const int max1, const int pref1,
                   const int min2, const int max2, const int pref2,
                   const float factor,
                   int& size1, int& size2);

    void OptimizeSizes(const int size, 
                       const int min1, const int max1, 
                       const int min2, const int max2, 
                       int& size1, int& size2);

    TTLayoutOrientation Orientation = TTLayoutOrientation::HORIZONTAL;
    TTLayout * Layout1 = nullptr;
    TTLayout * Layout2 = nullptr;
    float RatioInitialSize = 0.5f;
    float IncreaseRatio = 0.5f;
};


//*****************************************************************
//*                        TTBlockLayout                          *
//*                                                               *
//*                                                               *
//* Blocklayout contains several windows with fix size.           *
//* Spaces between them will be set. The whole "block" in         *
//* the window is fix but can be moved together                   *
//*                                                               *
//*****************************************************************
class TTBlockLayout : public TTLayout {

  public:

    TTBlockLayout();
    TTBlockLayout(const TTLayoutOrientation orientation);
    TTBlockLayout(const TTLayoutOrientation orientation, const int w, const int h);

    void AddWindow(TTWin * const win, const int width, const int height);
    void NewRow(const int space);

    void SetSpaceBetween(const int spacebetween);

    //will be called to set the size of the layout
    virtual void ResizeChilden();

    //debug
    virtual void DrawLayout(HDC hdc);

    virtual TTLayoutPrefSizes CalcTargetSizes();

    //actually set size and recursively those of the children
    virtual void SetSizes(const int x, const int y, const int width, const int height);

    //methods for creating or resizing a window
    virtual int GetWinX(TTWin * const win) const;
    virtual int GetWinY(TTWin * const win) const;
    virtual int GetWinWidth(TTWin * const win) const;
    virtual int GetWinHeight(TTWin * const win) const;

  protected: 
    
    const WinLayoutSizes* GetWinLayout(TTWin* const win) const;

    TTLayoutOrientation Orientation = TTLayoutOrientation::HORIZONTAL;

    std::vector<std::vector<WinLayoutSizes> > WindowRows;

    int SpaceBetween = 0;

    int NextRowSpace = 0;
};

//*****************************************************************
//*                          TTWinLayout                          *
//*                                                               *
//*                                                               *
//*             contains layout infos for one window              *
//*                                                               *
//*****************************************************************
class TTWinLayout : public TTLayout {

  public:

    TTWinLayout();
    TTWinLayout(TTWin * const win);

    void SetWindow(TTWin * const win);

    //debug
    virtual void DrawLayout(HDC hdc);

    virtual TTLayoutPrefSizes CalcTargetSizes();

    //actually set size and recursively those of the children
    virtual void SetSizes(const int x, const int y, const int width, const int height);

    //methods for creating or resizing a window
    virtual int GetWinX(TTWin * const win) const;
    virtual int GetWinY(TTWin * const win) const;
    virtual int GetWinWidth(TTWin * const win) const;
    virtual int GetWinHeight(TTWin * const win) const;

  protected:

    TTWin* Win = nullptr;
};



//*****************************************************************
//*                       TTWinFlowLayout                         *
//*                                                               *
//*                                                               *
//*          contains layout infos for a list of windows          *
//*                                                               *
//*****************************************************************
class TTWinFlowLayout : public TTLayout {

  public:

    TTWinFlowLayout(TTLayoutOrientation orientation);

    void AddWindow(TTWin * const win);

    //debug
    virtual void DrawLayout(HDC hdc);

    virtual TTLayoutPrefSizes CalcTargetSizes();

    //actually set size and recursively those of the children
    virtual void SetSizes(const int x, const int y, const int width, const int height);

    //methods for creating or resizing a window
    virtual int GetWinX(TTWin * const win) const;
    virtual int GetWinY(TTWin * const win) const;
    virtual int GetWinWidth(TTWin * const win) const;
    virtual int GetWinHeight(TTWin * const win) const;

  protected:

    std::vector<TTWin*> Wins;

    TTLayoutOrientation Orientation = TTLayoutOrientation::HORIZONTAL;
};

//*****************************************************************
//*                       TTWinFixLayout                          *
//*                                                               *
//*                                                               *
//* contains layout infos some window with fix relative positions *
//*                                                               *
//*****************************************************************
class TTWinFixLayout : public TTLayout {

  public:


  protected:

    std::vector<WinLayoutSizes> Windows;
};



} // end of namespace ttwin


