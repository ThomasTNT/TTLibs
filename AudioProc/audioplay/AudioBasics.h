#pragma once

//*************************************************************************
//*                            AudioBasics                                *
//*                                                                       *
//*    general audio processing formulas                                  *
//*    (format-independent, suitable for float pipeline)                  *
//*                                                                       *
//*    Pan convention: 0.0 = full left, 0.5 = center, 1.0 = full right   *
//*                                                                       *
//*************************************************************************

namespace ttaudio {

//***********************************************************
//*                        PanLaw                           *
//*                                                         *
//*  determines the gain curve when panning stereo signals  *
//***********************************************************
enum PanLaw {
  PAN_LINEAR_6DB,      // linear crossfade: -6 dB at center (left = 1-pan, right = pan)
  PAN_CONST_POWER_3DB  // constant power: -3 dB at center (left = cos(pan*pi/2), right = sin(pan*pi/2))
};

//****************************************************************
// StereoPanGainLeft
//
// returns the left channel gain factor for the given pan position
// pan: 0.0 = full left, 0.5 = center, 1.0 = full right
//****************************************************************
float StereoPanGainLeft(float pan, PanLaw law = PAN_CONST_POWER_3DB);

//*****************************************************************
// StereoPanGainRight
//
// returns the right channel gain factor for the given pan position
// pan: 0.0 = full left, 0.5 = center, 1.0 = full right
//*****************************************************************
float StereoPanGainRight(float pan, PanLaw law = PAN_CONST_POWER_3DB);

//***************************************************************************
// StereoPanGain (by channel index)
//
// returns the gain factor for the given stereo channel (0 = left, 1 = right)
// pan: 0.0 = full left, 0.5 = center, 1.0 = full right
//***************************************************************************
float StereoPanGain(float pan, unsigned int channel, PanLaw law = PAN_CONST_POWER_3DB);


} // end of namespace ttaudio
