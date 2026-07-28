#ifndef __TTSLIDER_h_
#define __TTSLIDER_h_

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                     AbstractSliderControl                     *
//*                                                               *
//*                                                               *
//*      base class for slider controls for photoshop plugins     *
//*                                                               *
//*****************************************************************
enum SliderType {LINEAR,               //simple linear
                 LINEAR_INT,           //simple linear but only int - values
                 EXPONENTIAL,          //exponential e.g. quadratic
                 EXPONENTIAL_NEGATIVE, //like EXPONENTIAL but even with negativ values
                 LINSCALE,             //for scaling 1/4 1/3 1/2 1 2 3 4  
                }; 

enum SliderIlluminationType {NONE, LEFT, RIGHT, CENTER, ZERO};

enum SliderGridType {NOGRID, SNAPTOCENTERPOS, SNAPTOZEROPOS};

class AbstractSlider : public TTWin {

  public:

    //contructor
    AbstractSlider();

    //configures the slider
    void Configure(double rangeMin, double rangeMax, unsigned int scaleDiv, 
                   SliderIlluminationType illumination, SliderType type, SliderGridType grid,
                   double typeFactor, int snapRange);

    //sets the value
    void SetValue(double value);

    //returns the value
    double GetValue();

    //returns the value rounded to the neares integer
    int GetValueInt();

    //set new RangeMax
    //void SetRangeMax(double max);

  protected:

    //intialisation and update
    virtual void Created();
    virtual void Init();
    virtual void PaintAll() = 0;

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt);

    //zero offset
    int SLIDER_ZEROOFFSET;

    //true if vertical
    bool VERTICAL; 

    //gets the rectandle to update when slider has moved
    virtual RECT CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect);

    //send a WM_HSCROLL message like standard sliders
    virtual void SendHScrollMessage();  

    //conversion functions that indirectly gets and sets the postion of slider
    int    PosFromValue(double value);
    double ValueFromPos(int pos);

    int    PosFromValueLin(double value);
    double ValueFromPosLin(int    pos);
    int    PosFromValueExp(double value);
    double ValueFromPosExp(int    pos);
    int    PosFromValueExpNeg(double value);
    double ValueFromPosExpNeg(int    pos);
    int    PosFromValueLinScale(double value);
    double ValueFromPosLinScale(int pos);

    // snaps to grid if it is not mode NOGRID
    int SnapToGrid(const int oldPos, int newPos);

    //classname for this control
    static std::string Classname;

    //true if left mouse is pressed
    bool LeftMouseDown;

    //current physical position of slider
    int  Pos;

    //maximal physical position of slider
    //minimal postion is zero of course
    int  MaxPos;

    //pos of of grid to snap to 
    int  SnapPos;

    //exact value on SnapPos
    double SnapValue;

    //range of grid snap 
    int  SnapRange;

    //range of slider
    double RangeMin;
    double RangeMax;

    //scale devision
    unsigned int ScaleDiv;

    //type of slider
    SliderType             Type;
    SliderIlluminationType Illumination;
    SliderGridType         Grid;

    //form factor of slider curve when type is not linear
    double TypeFactor;

    //exponent for exponential (or logarithmic curves
    double Exponent;
};

//*****************************************************************
//*                     AbstractDoubleSlider                      *
//*                                                               *
//*                                                               *
//*  base class for double slider controls for photoshop plugins  *
//*                                                               *
//*****************************************************************
class AbstractDoubleSlider : public AbstractSlider {

  public:

    //contructor
    AbstractDoubleSlider();

    //sets the value
    void SetValue2(double value);

    //returns the value
    double GetValue2();

    //returns the value rounded to the neares integer
    int GetValue2Int();

  protected: 

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt);
    
    //current physical position of slider
    int  Pos2;
    bool UpperSliderSelected;
};

//*****************************************************************
//*                           TTSlider                            *
//*                                                               *
//*                                                               *
//*                      lass for slider controls                 *
//*                                                               *
//*****************************************************************
class TTSlider : public AbstractSlider {
  
  public: 
    
    //contructor
    TTSlider();   

  protected: 

    //paint function
    virtual void PaintAll();

    //gets the rectandle to update when slider has moved
    virtual RECT CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect);

};

//*****************************************************************
//*                          TTTuner                              *
//*                                                               *
//*                                                               *
//*        class for slider controls like a tuner wheel           *
//*****************************************************************
class TTTuner : public AbstractSlider {
  
  public: 

    //contructor
    TTTuner();

  protected: 

    //paint function
    virtual void PaintAll();

    //gets the rectandle to update when slider has moved
    virtual RECT CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect);
};





} // end of namespace ttwin

#endif
