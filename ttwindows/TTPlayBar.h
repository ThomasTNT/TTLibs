#ifndef __TTPLAYBAR_h_
#define __TTPLAYBAR_h_

#include <vector>

#include "TTWinUtils.h"
#include "TTWin.h"
#include "resource.h"

namespace ttwin{

//*****************************************************************************
//*                                  TTPlayBar                                *
//*                                                                           *
//*                                                                           *
//*               class for showing a bar for audio or video playing          *
//*                                                                           *
//*****************************************************************************

//NOTE: bar sizes depend on the size of the images!
const int BAR_HEIGHT = 25;  
const int PLAY_HEIGHT = 8;  

const int BAR_WIDTH_LEFT  = 13; 
const int BAR_WIDTH_MID   = 1;  
const int BAR_WIDTH_RIGHT = 13; 

const int BAR_OFFSET_LEFT  = 5; 
const int BAR_OFFSET_RIGHT = 5; 

const int PLAY_WIDTH_LEFT  = 5;  
const int PLAY_WIDTH_MID   = 1;  
const int PLAY_WIDTH_RIGHT = 4; 

const int PLAYBAR_OFFSET_LEFT  = 0; 
const int PLAYBAR_OFFSET_RIGHT = 0; 



class TTPlayBar : public TTWin {

  public:

    //---------------------- construction and initialisation -----------------

    //constructor - does nothing but initialisation
    TTPlayBar();

    //destructor 
    virtual ~TTPlayBar();

    //configures the bar
    void Configure(const double min,     //the value representing physical pos == 0
                   const double max,     //the value representing physical pos == width (or such)
                   const double start,   //the start position if the song, video
                   const double length); //the length of the song, video etc.

    //sets the minimum scale value
    void SetMin(const double min);

    //sets the maximum scale value
    void SetMax(const double max);

    //sets the actual end value
    void SetStart(const double start);

    //sets the actual end value
    void SetLength(const double length);

    //set play position
    void SetPlayValue(double playvalue);

    //set marker position
    void SetMarkerValues(const std::vector<double>& marker);

    //set marker position
    void SetTransportMarkerValues(const std::vector<double>& transportMarker);

    //set selected marker
    void SetMarkerSelected(const unsigned int index);

  protected:

    //intialisation and update
    virtual void Created();
    virtual void Init();
    virtual void InitGraphic();
    virtual void PaintAll();
    //virtual void DrawBackground(HDC hdc, const bool drawDefaultBackground);

    //event functions
    virtual void CM_Mouse_LeftDown(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_LeftUp(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Move(int x, int y, bool shift, bool ctrl, bool alt);
    virtual void CM_Mouse_Leave(int x, int y, bool shift, bool ctrl, bool alt);
    
    //calculations
    int PhysicalPosByValue(const double value, const int posMin, const int posMax);
    double ValueByPhysicalPos(const int pos, const int posMin, const int posMax);
    virtual RECT CalcUpdateRect(const double value1, const double value2);

    //current physical position of play pos (-> last painting)
    int PlayPos;

    //values of bar
    double MaxValue;
    double MinValue;
    double Start;
    double Length;
    double PlayValue;
   
    std::vector<double> Marker;
    std::vector<double> TransportMarker;
    unsigned int MarkerSelected; //starting with 1

    //events fields
    bool LeftMousePressed;

    //images
    HBITMAP barDarkLeft;  // 13
    HBITMAP barDarkRight; // 13
    HBITMAP barDarkMid;   //  1 
    HBITMAP barBrightLeft;  // 13
    HBITMAP barBrightRight; // 13
    HBITMAP barBrightkMid;   //  1 
    HBITMAP barPlayLeft;  // 5
    HBITMAP barPlayRight; // 4
    HBITMAP barPlaykMid;

};

} // end of namespace ttwin

#endif
