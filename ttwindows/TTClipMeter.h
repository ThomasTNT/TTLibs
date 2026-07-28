#pragma once

//*************************************************************************
//*                           TTClipMeter                                 *
//*                                                                        *
//*  Horizontal pseudo-LED strip for limiter feedback on output channels. *
//*  - if clipping: full red bar with "CLIPPING" label                    *
//*  - else: configurable-color LED strip showing gain reduction amount   *
//*                                                                        *
//*  Set values via SetGainReduction() / SetClipping().                   *
//*  Configure max dB range and reduction color via Configure().           *
//*                                                                        *
//*************************************************************************

#include "TTWin.h"

namespace ttwin {
  
  class TTClipMeter : public TTWin {
  
  public:
  
    TTClipMeter();
  
    // maxReductionDb: positive dB value = full bar (e.g. SongGain + FaderGain headroom)
    // reductionColor: color for gain reduction LEDs (default: orange)
    void Configure(const float maxReductionDb, const COLORREF reductionColor = RGB(255, 140, 0));
  
    // gain reduction in dB (positive = reduction, 0 = no reduction)
    void SetGainReduction(const float dB);
  
    // set true when the output clipped
    void SetClipping(const bool clipping);
  
  protected:
  
    virtual void Created();
    virtual void Init();
    virtual void PaintAll();
  
  private:
  
    float    MaxReductionDb      = 24.0f;
    float    CurrentReductionDb  = 0.0f;
    bool     Clipping            = false;
    COLORREF ReductionColor      = RGB(255, 140, 0);
    int      MaxPos              = 0;
};

} // end of namespace ttwin
