#pragma once

#include "TTWin.h"
#include "TTWinUtils.h"

namespace ttwin{

//*****************************************************************
//*                           TTButton                            *
//*                                                               *
//*                                                               *
//*               class for check boxes and buttons               *
//*                                                               *
//*****************************************************************
// ButtonType controls click BEHAVIOUR
enum ButtonType {CHECKBOX,          // switches between off and on (Active==0, Active==1)
                 BUTTON,            // Active when pressed, releases to inactive
                 BUTTON_INV,        // inverse BUTTON: idle=Active(1), pressed=inactive(0), releases to Active(1)
                 THREESTATE,        // cycles Active 0, 1, 2 on each press
                 ONBUTTON,          // switches to on when pressed (radio button style)
                 CHECKBOX_TWOCOLOR};// switches between Active 0/1 and 2/3

// ButtonForm controls VISUAL SHAPE (orthogonal to type)
enum ButtonForm {FORM_RECT,         // default: rounded rectangle with text label
                 FORM_ARROWUP,      // filled triangle pointing up
                 FORM_ARROWDOWN,    // filled triangle pointing down
                 FORM_ARROWLEFT,    // filled triangle pointing left
                 FORM_ARROWRIGHT};  // filled triangle pointing right

const int BUTTON_OFFSET_PRESSED = 2; //x and y

class TTButton : public TTWin {

  public:

    //constructor
    TTButton();

    //destructor
    virtual ~TTButton();

    //configures the button
    void Configure(const ButtonType type, const ButtonForm form, const bool enabledWhenInactive);

    //sets the button number
    void ConfigureButtonNumber(const int number);

    //gets the button number
    int GetButtonNumber();

    //sets the color - return true when changed
    bool ConfigureColor(const COLORREF c);

    //sets the color - return true when changed
    bool ConfigureColor(const COLORREF c1, const COLORREF c2);

    //sets the color - return true when changed
    bool ConfigureColor(const COLORREF c1, const COLORREF c2, const COLORREF c3);

    //sets the color - return true when changed
    //if disabledGrayBoarder is false, then inaktive is the same as all
    //active states, if it is true then only text will be colored in disabled mode
    //(this is the old behaviour) 
    bool ConfigureInactiveColor(const COLORREF c, const bool disabledGrayBoarder);

    //sets the special color for disabled state
    bool ConfigureDisabledColor(const COLORREF c);

    //sets an special text for inactive state
    void SetLabelInactive(const std::string &label);

    //sets the additional text
    void SetLabel3rd(const std::string &label);

    //sets the additional text
    void SetLabel4th(const std::string &label);

    //sets the second line label (shown below main label)
    void SetLabelLine2(const std::string &label);

    //changes the color "on the fly" ??????
    void ChangeColor(const COLORREF newcolor);

    //sets the value
    void SetActive(unsigned int value);

    //returns the value
    unsigned int GetActive() const;

    //switches between two state on CHECKBOX_TWOCOLOR
    //s == 0 or 1
    void SwitchTwoColorState(const unsigned int s);

  protected:

    //intialisation and update
    virtual void Created();
    virtual void Init();
    virtual void InitGraphic();
    virtual void PaintAll();
    virtual void DrawBackground(HDC hdc, const bool drawDefaultBackground);

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt);

  private:

    void PaintRect();
    void PaintArrow();

    //send a WM_COMMAND message like standard sliders
    void SendClickMessage(const bool shift);

    ButtonType Type = ButtonType::CHECKBOX;
    ButtonForm Form = ButtonForm::FORM_RECT;
    bool EnabledWhenInactive = true;
    unsigned int Active = 0;
    bool Pressed = false;

    //number of button (some may have the same id!) 
    int ButtonNumber = 0;

    //colors for painting
    COLORREF Colors[5][7];  //4 is special disabled

    //label for inactive state
    std::string LabelInactive;

    //label for third state
    std::string Label3rd;

    //label for third state
    std::string Label4th;

    //second line label
    std::string LabelLine2;

    //color
    unsigned int R0 = 50, G0 = 50, B0 = 50; //color for disabled or inactive mode
    unsigned int R1 = 255, G1 = 0, B1 = 0;
    unsigned int R2 = 0, G2 = 255, B2 = 0;
    unsigned int R3 = 255, G3 = 0, B3 = 0;
    unsigned int Rd = 50, Gd = 50, Bd = 50; //special disabled color

    bool UseDisabledColors = false;
    bool DisabledGrayBoarder = true; 
};


} // end of namespace ttwin

