#ifndef __TTTOOLTIP_h_
#define __TTTOOLTIP_h_

#include <string>

#include "TTWinUtils.h"

namespace ttwin{

const unsigned int MAX_TOOLTIP_LENGTH = 256;

//*****************************************************************
//*                           TTTooltip                           *
//*                                                               *
//*                                                               *
//*            class for creating a tool tip for a window         *
//*                                                               *
//*****************************************************************
class TTTooltip{

  public:

    //constructor
    TTTooltip(HWND parent, HINSTANCE hinst, const std::string& tooltiptext);

    //destructor
    ~TTTooltip();

    //sets the tool tip text
    void SetToolTip(const std::string& tooltiptext);


  protected:

    std::string Text;

    //buffer for textvalues from windows
    char lpString[MAX_TOOLTIP_LENGTH];

    HWND HwndTip;
    HWND Parent;
    HINSTANCE Inst;

};

} // end of namespace ttwin

#endif

