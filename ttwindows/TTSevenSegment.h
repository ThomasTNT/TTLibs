#pragma once

#include "TTWin.h"

namespace ttwin{

const COLORREF SEVENSEG_DEFAULT_COLOR = RGB(255, 110, 30);

//*************************************************************************
//*                          TTSevenSegment                               *
//*                                                                       *
//*   Fixed-width display showing up to 5 alphanumeric characters.       *
//*   Renders red-orange text on black background as a 7-segment dummy.  *
//*                                                                       *
//*************************************************************************
class TTSevenSegment : public TTWin{

  public:

    TTSevenSegment();
    virtual ~TTSevenSegment();

    // replaces displayed text and repaints
    void UpdateText(const std::string& text);

    // sets the text color; default is red-orange
    void SetColor(COLORREF color);

  protected:

    virtual void PaintAll() override;

  private:

    std::string DisplayText  = "-----";
    COLORREF    Color        = SEVENSEG_DEFAULT_COLOR;

};

} // end of namespace ttwin
