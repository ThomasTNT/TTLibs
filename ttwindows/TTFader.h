#ifndef TTFADER_H
#define TTFADER_H

#include <cmath>
#include "resource.h"

#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                           TTFader                             *
//*                                                               *
//*                                                               *
//*                   control for a mixer fader                   *
//*                                                               *
//*****************************************************************

const double FADER_DB_NEG_INF = -99999999;

class TTFader : public TTWin {

  public:

    //contructor
    TTFader();

    //configures the slider
    void Configure(double stepValue, double stepBase, double scaleMax, double scaleMin, int snapRange);

    //sets the value
    void SetValue(double value);

    //returns the value
    double GetValue();

    //returns the value rounded to the nearest integer
    int GetValueInt();

   
  protected:

    //intialisation and update
    virtual void Created();
    virtual void Init();
    virtual void PaintAll();

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt);

    //gets the rectandle to update when slider has moved
    virtual RECT CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect);

    //send a WM_HSCROLL message like standard sliders
    virtual void SendHScrollMessage();  
  
    //conversion functions that indirectly gets and sets the postion of slider
    int    PosFromValue(double value);
    double ValueFromPos(int pos);

    void CalculateSteps();

    double DBByStep(double step);
    double StepBydB(double db);
    double DBbyPos(const int pos);
    int PosBydB(const double db);
    int PosByY(const int y);
    int YByPos(const int pos);

    //current physical position of slider
    int  Pos;

    //more or less a constant 
    int ZeroOffset;

    //maximal physical position of slider
    //minimal postion is zero of course
    int  MaxPos;

    //value of the first step
    double StepValue;

    //mulitplicator of nexts steps
    double StepBase;

    //scale maximum
    double ScaleMax;

    //scale minimum
    double ScaleMin;

    //pos where value is 0
    int PosZero;

    double StepsPlus;
    double StepsMinus;
    double StepSize;

    //true if left mouse is pressed
    bool LeftMouseDown;

    //bitmaps for drawing
    HBITMAP BmpFader;
    HBITMAP BmpSlit;
    int FaderBmpWidth; 
    int FaderBmpHeight; 
    int FaderSlitBorder;
    int FaderSlitWidth;
    int FaderSlitScale;
};



} // end of namespace ttwin



#endif
