#pragma once

#include <windows.h>
#include <string>
#include "TTAudioFile.h"
#include "TTWave.h"

namespace ttaudio {

//***********************************************************************
//*                       TTAudioResource                               *
//*                                                                     *
//*   TTAudioFile implementation that loads a WAV from a Windows        *
//*   RCDATA resource. Writes a temp file so TTWave can read it;        *
//*   temp file is deleted in the destructor.                           *
//***********************************************************************
class TTAudioResource : public TTAudioFile {
  public:
    ~TTAudioResource();

    bool OpenFromResource(const HINSTANCE hInst, const UINT resourceId);

    bool         IsValid() const override;
    unsigned int GetLengthSamples() const override;
    unsigned int GetChannels() const override;

    void         CreateReadingBuffer(unsigned int samples, bool dontCreateIfExists) override;
    unsigned int ReadSamplesFromFile(const unsigned int samplesToRead) override;
    const U32*   GetConvertedSample(const unsigned int i, const WaveFormat format) override;

  private:
    TTWave      Wave;
    std::string TempFilePath;
};

} // namespace ttaudio
