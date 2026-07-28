#pragma once

#include "TTAudioFile.h"
#include "TTWave.h"
#include <string>

namespace ttaudio {

//***********************************************************************
//*                          TTWaveFile                                 *
//*                                                                     *
//*   TTAudioFile implementation that reads from a .wav file on disk    *
//***********************************************************************
class TTWaveFile : public TTAudioFile {
  public:
    bool Open(const std::string& filename);

    bool         IsValid() const override;
    unsigned int GetLengthSamples() const override;
    unsigned int GetChannels() const override;

    void         CreateReadingBuffer(unsigned int samples, bool dontCreateIfExists) override;
    unsigned int ReadSamplesFromFile(const unsigned int samplesToRead) override;
    const U32*   GetConvertedSample(const unsigned int i, const WaveFormat format) override;

  private:
    TTWave Wave;
};

} // namespace ttaudio
