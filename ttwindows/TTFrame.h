#ifndef __TTFRAME_h_
#define __TTFRAME_h_

#include "TTWin.h"

namespace ttwin{


//*****************************************************************
//*                           TTFrame                             *
//*                                                               *
//*                                                               *
//* class for a frame similar to a group box                      *
//* Frame is not a windows class!                                 *
//* It's paint method has to be called in main window PaintAll()  *
//*                                                               *
//*****************************************************************
class TTFrame{

  public:

    //contructor
    TTFrame();

    //create control
    bool Create(HWND parent, HINSTANCE hinst, int id, const std::string& label, int x, int y, int width, int height, TTControlUnit unit);

    //resize
    virtual void Resize(int x, int y, int width, int height, TTControlUnit unit);

    //intialisation and update
    void PaintAll(HDC hdc);

    // shows or hides the control
    void Show(const bool show);

  protected:

    //calculates the width of the text in pixel
    int GetWidthOfLabel(HDC hdc);

    //the label 
    std::string Label;

    //the actual client rect
    RECT Rect;

    //handle for parent window
    HWND Parent; 

    //if true the frame will be drawn else not
    bool show; 
};

//*****************************************************************
//*                         TTColorFrame                          *
//*                                                               *
//*                                                               *
//*             frame with inner color and boarder color          *
//*                                                               *
//*****************************************************************
class TTColorFrame : public TTFrame{

  public:

    //sets the colors
    void SetColors(const COLORREF border, const bool drawborder, const COLORREF inner, const bool drawinner);

    //set color invalid
    void SetColorsInactive(const COLORREF inner, const COLORREF cross);

    //intialisation and update
    void PaintAll(HDC hdc);

    //setter
    void SetActive(const bool active);

  private:

    COLORREF BorderColor = RGB(0, 0, 0);
    bool DrawBorder = true;
    COLORREF InnerColor = RGB(150, 150, 150);
    bool DrawInner = true;

    COLORREF InactiveInner = RGB(200, 200, 200);
    COLORREF InactiveCross = RGB(150, 150, 150);

    bool Active = true;

};

} // end of namespace ttwin

#endif
