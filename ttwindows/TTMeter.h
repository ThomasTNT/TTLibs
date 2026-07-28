#ifndef TTMETER_H
#define TTMETER_H

#include <cmath>
#include "resource.h"

#include "LevelMeter.h"
#include "TTWin.h"

namespace ttwin{

//*****************************************************************
//*                           TTMeter                             *
//*                                                               *
//*                                                               *
//* A level meter window. Based on the Levelmeter class.          *
//*                                                               *
//*****************************************************************

const double METER_DB_NEG_INF = -99999999;

const int METER_BORDER = 17; //-> small fader
const int METER_WIDTH = 12;
const int METER_CHANNEL_SPACE = 1;

const unsigned int METER_SCALE_SPACE = 1;
const unsigned int METER_SCALE_LENGTH = 3;

const unsigned int METERTIMER_PERIODE = 10; //ms

enum MeterBarType {METERBARTYPE_FULL, METERBARTYPE_STRIPES };

class TTMeter : public TTWin {

  public:

    //contructor
    TTMeter();

    //configures the slider
    void Configure(double stepValue, double stepBase, double scaleMax, double scaleMin, const bool drawNumbers, const MeterBarType type);

    //configures the colors
    void ConfigureColor(const COLORREF over, const COLORREF head, const COLORREF normal, const COLORREF low);

    //sets Levelmeter
    void SetLevelMeter(audioplay::LevelMeter* const meter);

    //causes update
    virtual void UpdateContent();

  protected:

    // update timing event
    // return false for stop
    virtual bool TimerHandler1();

    //intialisation and update
    virtual void Created();
    virtual void Init();
    virtual void PaintAll();

    //gets the rectandle to update when meter has changed
    virtual RECT CalcUpdateRect(int pos1, int pos2, const RECT& controllerrect);

    void CalculateSteps();
    static COLORREF MeterGradientColor(const int pos, const int posOver);

    double DBByStep(double step);
    double StepBydB(double db);
    double DBbyPos(const int pos);
    int PosBydB(const double db);
    int YByPos(const int pos);

    //current physical position of meter
    std::vector<int> Pos;
    unsigned int Channels = 0;

    //more or less a constant 
    int ZeroOffset = 0;

    //maximal physical position of slider
    //minimal postion is zero of course
    int  MaxPos = 0;

    //value of the first step
    double StepValue = 0.0;

    //mulitplicator of nexts steps
    double StepBase = 0.0;

    //scale maximum
    double ScaleMax = 0.0;

    //scale minimum
    double ScaleMin = 0.0;

    //pos where value is 0
    int PosZero  = 0;

    double StepsPlus = 0.0;
    double StepsMinus = 0.0;
    double StepSize = 0.0;

    //the level meter
    audioplay::LevelMeter* Meter;

    COLORREF ColorOver;
    COLORREF ColorHead;
    COLORREF ColorNormal;
    COLORREF ColorLow;

    bool DrawScaleNumbers = false;
    MeterBarType Type = METERBARTYPE_FULL;
};



} // end of namespace ttwin



#endif
