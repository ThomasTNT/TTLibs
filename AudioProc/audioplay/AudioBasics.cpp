#include "AudioBasics.h"

#include <cmath>

//*************************************************************************
//*                            AudioBasics                                *
//*                                                                       *
//*                  general audio processing formulas                    *
//*                                                                       *
//*************************************************************************


//****************************************************************
// StereoPanGainLeft
//
// returns the left channel gain factor for the given pan position
// pan: 0.0 = full left, 0.5 = center, 1.0 = full right
//****************************************************************
float ttaudio::StereoPanGainLeft(const float pan, const PanLaw law) {
  const float p = pan < 0.0f ? 0.0f : (pan > 1.0f ? 1.0f : pan);
  switch (law) {
    case PAN_CONST_POWER_3DB:
      return std::cos(p * (3.14159265358979323846f / 2.0f));
    case PAN_LINEAR_6DB:
    default:
      return 1.0f - p;
  }
}


//*****************************************************************
// StereoPanGainRight
//
// returns the right channel gain factor for the given pan position
// pan: 0.0 = full left, 0.5 = center, 1.0 = full right
//*****************************************************************
float ttaudio::StereoPanGainRight(const float pan, const PanLaw law) {
  const float p = pan < 0.0f ? 0.0f : (pan > 1.0f ? 1.0f : pan);
  switch (law) {
    case PAN_CONST_POWER_3DB:
      return std::sin(p * (3.14159265358979323846f / 2.0f));
    case PAN_LINEAR_6DB:
    default:
      return p;
  }
}


//***************************************************************************
// StereoPanGain (by channel index)
//
// returns the gain factor for the given stereo channel (0 = left, 1 = right)
// pan: 0.0 = full left, 0.5 = center, 1.0 = full right
//***************************************************************************
float ttaudio::StereoPanGain(const float pan, const unsigned int channel, const PanLaw law) {
  return (channel == 0) ? StereoPanGainLeft(pan, law) : StereoPanGainRight(pan, law);
}
