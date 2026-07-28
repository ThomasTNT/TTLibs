#pragma once

#include "TTWin.h"

namespace ttwin{

//default main text color for TTInfofield (white)
const COLORREF INFOFIELD_DEFAULT_TEXT_COLOR = RGB(255, 255, 255);

//*************************************************************************
//*                              TTInfofield                              *
//*                                                                       *
//*                          an area to show text                         *
//*                                                                       *
//*************************************************************************

class TTInfofield : public TTWin {

  public:

    TTInfofield();
    virtual ~TTInfofield();

    //updates main text
    virtual void UpdateText(const std::string& text);

    //updates footer text
    virtual void UpdateSubText(const std::string& text);

    //if true: label small top-left, main text large to its right on the same line
    //if false (default): classic stacked layout (label top, text below, subtext at bottom)
    void SetSideBySideLayout(const bool enable);

    //sets the main text font size for the classic layout (default 29)
    void SetTextFontSize(const int size);

    //sets the main text color; triggers repaint
    void SetMainTextColor(COLORREF color);

  protected:

    virtual void PaintAll();

  private:

    std::string Text             = "--- not set ---";
    std::string SubText;
    bool        SideBySideLayout = false;
    int         TextFontSize     = 29;
    COLORREF    MainTextColor    = INFOFIELD_DEFAULT_TEXT_COLOR;

};

} // end of namespace ttwin
