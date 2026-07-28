#ifndef __TTLABEL_H_
#define __TTLABEL_H_

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                             TTLabel                           *
//*                                                               *
//*                                                               *
//*                    windows class for labels                   *
//*                                                               *
//*****************************************************************
class TTLabel : public TTWin {

  public:

    //contructor
    TTLabel();

    //configuring the text alignment
    void Configure(const UINT alignHor, const UINT alignVer);

    //set color
    void SetColorText(const COLORREF colorText);


  protected:

    //intialisation and update
    virtual void PaintAll();

  private:

    //textalignment
    UINT AlignHor;
    UINT AlignVer;

    COLORREF ColorText;
};

} // end of namespace ttwin


#endif
