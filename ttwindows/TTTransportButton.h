#ifndef __TTTRANSPORT_BUTTON_h_
#define __TTTRANSPORT_BUTTON_h_

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                        TransportButton                        *
//*                                                               *
//*                                                               *
//*             class for big illumated transport buttons         *
//*                                                               *
//*****************************************************************

enum TransportButtonState { ENABLED,   //button is enabled and normal illuminated
                            SELECTED,  //button is enabled and normal illuminated with glow (mouse over)
                            ACTIVE,    //button is enabled and bright illuminated
                            DISABLED   //button is disables and dark
};

class TTTransportButton : public TTWin {

  public:

    //contructor
    TTTransportButton();

    //destructor
    virtual ~TTTransportButton();

    //configures the button
    void Configure(const int idbmpNormal, const int idbmpActive, const int idbmpSelected, const int idbmpDisabled);

    //sets the value
    void SetButtonState(const TransportButtonState state);

    //returns the value
    TransportButtonState GetButtonState();

  protected:

    //intialisation and update
    virtual void Created();
    virtual void Init();
    //virtual void InitGraphic();
    virtual void PaintAll();

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt);

  private:

    //send a WM_COMMAND message like standard sliders
    void SendClickMessage(); 

    //ButtonState
    TransportButtonState State;

    //true when pressed
    bool Pressed;
    
    HBITMAP BmpNormal;
    HBITMAP BmpActive;
    HBITMAP BmpSelected;
    HBITMAP BmpDisabled;
    

};

} // end of namespace ttwin

#endif
