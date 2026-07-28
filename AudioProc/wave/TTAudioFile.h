#pragma once

#include "WaveBasics.h"

namespace ttaudio {

//***********************************************************************
//*                          TTAudioFile                                *
//*                                                                     *
//*   abstract base class for audio sample sources                      *
//*   concrete subclasses: TTWaveFile (from disk),                      *
//*                        TTAudioResource (from embedded resource)     *
//***********************************************************************
class TTAudioFile {
  public:
    virtual ~TTAudioFile() {}

    virtual bool         IsValid() const = 0;
    virtual unsigned int GetLengthSamples() const = 0;
    virtual unsigned int GetChannels() const = 0;

    virtual void         CreateReadingBuffer(unsigned int samples, bool dontCreateIfExists) = 0;
    virtual unsigned int ReadSamplesFromFile(const unsigned int samplesToRead) = 0;
    virtual const U32*   GetConvertedSample(const unsigned int i, const WaveFormat format) = 0;
};

} // namespace ttaudio
