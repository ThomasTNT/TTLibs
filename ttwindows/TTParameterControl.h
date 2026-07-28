#ifndef TTPARAMETERCONTROL_H
#define TTPARAMETERCONTROL_H

#include "TTWin.h"
#include "TTSlider.h"
#include "TTEdit.h"
#include "TTLabel.h"
#include "resource.h"

namespace ttwin{

//*****************************************************************************
//*                          TTParameterControl                               *
//*                                                                           *
//*                                                                           *
//*     complexer control window that handels manipulation of an parameter    *
//*                                                                           *
//* This class is abstract and should be derived for the specifiec type of    *
//* parameters. TTParameterControl is the base class.                         *
//*****************************************************************************
class TTParameterControl : public TTWin {

  public:

    //constructor
    TTParameterControl();

    //destructor
    ~TTParameterControl();

    //sets font
    void SetEditFont(const HFONT font);

  protected:

    //font
    HFONT EditFont = 0;

  private:

};

//*****************************************************************************
//*                          TTParameterControlSimple                         *
//*                                                                           *
//*                                                                           *
//*     complexer control window that handels manipulation of an parameter    *
//*                                                                           *
//* this class is abstract and should be derived for the specifiec type of    *
//* parameter.                                                                *
//*****************************************************************************

const int PARAMCONTROL_EDIT_WIDTH = 35;
const int PARAMCONTROL_EDIT_HEIGHT = 20;
const int PARAMCONTROL_SLIDER_WIDTH = 150;
const int PARAMCONTROL_SLIDER_HEIGHT = 30;

const int PARAMCONTROL_SPACE_BETWEEN = 3;

const int PARAMCONTROL_WIDTH = PARAMCONTROL_SLIDER_WIDTH + PARAMCONTROL_SPACE_BETWEEN + PARAMCONTROL_EDIT_WIDTH;
const int PARAMCONTROL_HEIGHT = max(PARAMCONTROL_SLIDER_HEIGHT, PARAMCONTROL_EDIT_HEIGHT);

class TTParameterControlSimple : public TTParameterControl {

  public:

    //constructor
    TTParameterControlSimple();

    //destructor
    ~TTParameterControlSimple();

    void Configure(double sliderRangeMin, double sliderrangeMax, unsigned int scaleDiv,
                   SliderIlluminationType illumination, SliderType type, SliderGridType grid,
                   double typeFactor, int snapRange, 
                   double editRangeMin, double editRangeMax, unsigned int editdigits);

    // creates a tool tip
    void CreateToolTip(const std::string& tooltip);

    //Enables or disbales the control
    virtual void Enable(const bool enable);

  protected:

    //inherited from TTWin
    virtual void Init();
    virtual void PaintAll();

    // windows message function
    virtual LRESULT Messagehandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void CreateOwnControls();

    virtual void CM_Slider() = 0;
    virtual void CM_Text() = 0;

    TTLabel LabelWin;
    TTSlider Slider;
    TTEdit  EditWin;
};

//*****************************************************************************
//*                          TTParameterControlFloat                          *
//*                                                                           *
//*                                                                           *
//* Implementation of TTParameterControl for simple float values              *
//*****************************************************************************
class TTParameterControlFloat : public TTParameterControlSimple {

  public:

    //constructor
    TTParameterControlFloat();

    //destructor
    ~TTParameterControlFloat();

    //configures the control
    void SetValuePointer(float * const ptr);

    //sets the value
    void SetValue(float value);

    //sets the value
    float GetValue();

    //configures the control
    //void Configure();



  protected:

    //inherited from TTWin
    virtual void Init();

    void CreateOwnControls();

    virtual void CM_Slider();
    virtual void CM_Text();

  private:

    float* PointerToValue = nullptr;

};



//*****************************************************************************
//*                           TTParameterControlInt                           *
//*                                                                           *
//*                                                                           *
//* Implementation of TTParameterControl for simple int values                *
//*****************************************************************************
class TTParameterControlInt : public TTParameterControlSimple {

public:

  //constructor
  TTParameterControlInt();

  //destructor
  ~TTParameterControlInt();

  //configures the control
  void SetValuePointer(int * const ptr);

  //configures the control
  //void Configure();

protected:

  //inherited from TTWin
  virtual void Init();

  void CreateOwnControls();

  virtual void CM_Slider();
  virtual void CM_Text();

private:

  int* PointerToValue = nullptr;

};

//*****************************************************************************
//*                           TTParameterControlUInt                          *
//*                                                                           *
//*                                                                           *
//* Implementation of TTParameterControl for simple unsigned int values       *
//*****************************************************************************
class TTParameterControlUInt : public TTParameterControlSimple {

public:

  //constructor
  TTParameterControlUInt();

  //destructor
  ~TTParameterControlUInt();

  //configures the control
  void SetValuePointer(unsigned int * const ptr);

  //configures the control
  //void Configure();

protected:

  //inherited from TTWin
  virtual void Init();

  void CreateOwnControls();

  virtual void CM_Slider();
  virtual void CM_Text();

private:

  unsigned int* PointerToValue = nullptr;

};

} // end of namespace ttwin

#endif
