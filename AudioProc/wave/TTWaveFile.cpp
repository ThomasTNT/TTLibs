#include "TTWaveFile.h"

//********
// Open
//********
bool ttaudio::TTWaveFile::Open(const std::string& filename){
  return Wave.Open(filename);
}

//**********
// IsValid
//**********
bool ttaudio::TTWaveFile::IsValid() const {
  return Wave.IsValid();
}

//******************
// GetLengthSamples
//******************
unsigned int ttaudio::TTWaveFile::GetLengthSamples() const {
  return Wave.GetLengthSamples();
}

//***************
// GetChannels
//***************
unsigned int ttaudio::TTWaveFile::GetChannels() const {
  return Wave.GetChannels();
}

//**********************
// CreateReadingBuffer
//**********************
void ttaudio::TTWaveFile::CreateReadingBuffer(const unsigned int samples, const bool dontCreateIfExists){
  Wave.CreateReadingBuffer(samples, dontCreateIfExists);
}

//**********************
// ReadSamplesFromFile
//**********************
unsigned int ttaudio::TTWaveFile::ReadSamplesFromFile(const unsigned int samplesToRead){
  return Wave.ReadSamplesFromFile(samplesToRead);
}

//*********************
// GetConvertedSample
//*********************
const U32* ttaudio::TTWaveFile::GetConvertedSample(const unsigned int i, const ttaudio::WaveFormat format){
  return Wave.GetConvertedSample(i, format);
}
