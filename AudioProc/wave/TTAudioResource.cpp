#include "TTAudioResource.h"

//*************
// destructor
//
// deletes the temp file that was written for TTWave to read from
//*************
ttaudio::TTAudioResource::~TTAudioResource(){
  if (!TempFilePath.empty())
    ::DeleteFile(TempFilePath.c_str());
}

//******************
// OpenFromResource
//
// loads a WAV from a Windows RCDATA resource, writes it to a temp file,
// then opens the temp file with TTWave
//******************
bool ttaudio::TTAudioResource::OpenFromResource(const HINSTANCE hInst, const UINT resourceId){
  HRSRC hRes = ::FindResource(hInst, MAKEINTRESOURCE(resourceId), RT_RCDATA);
  if (!hRes) return false;

  HGLOBAL hGlobal = ::LoadResource(hInst, hRes);
  if (!hGlobal) return false;

  const DWORD    size  = ::SizeofResource(hInst, hRes);
  const void*    pData = ::LockResource(hGlobal);
  if (!pData || size == 0) return false;

  char tempDir[MAX_PATH];
  char tempFile[MAX_PATH];
  ::GetTempPath(MAX_PATH, tempDir);
  ::GetTempFileName(tempDir, "slp", 0, tempFile);
  TempFilePath = tempFile;

  HANDLE hFile = ::CreateFile(tempFile, GENERIC_WRITE, 0, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) return false;

  DWORD written = 0;
  ::WriteFile(hFile, pData, size, &written, NULL);
  ::CloseHandle(hFile);

  return Wave.Open(TempFilePath);
}

//**********
// IsValid
//**********
bool ttaudio::TTAudioResource::IsValid() const {
  return Wave.IsValid();
}

//******************
// GetLengthSamples
//******************
unsigned int ttaudio::TTAudioResource::GetLengthSamples() const {
  return Wave.GetLengthSamples();
}

//***************
// GetChannels
//***************
unsigned int ttaudio::TTAudioResource::GetChannels() const {
  return Wave.GetChannels();
}

//**********************
// CreateReadingBuffer
//**********************
void ttaudio::TTAudioResource::CreateReadingBuffer(const unsigned int samples, const bool dontCreateIfExists){
  Wave.CreateReadingBuffer(samples, dontCreateIfExists);
}

//**********************
// ReadSamplesFromFile
//**********************
unsigned int ttaudio::TTAudioResource::ReadSamplesFromFile(const unsigned int samplesToRead){
  return Wave.ReadSamplesFromFile(samplesToRead);
}

//*********************
// GetConvertedSample
//*********************
const U32* ttaudio::TTAudioResource::GetConvertedSample(const unsigned int i, const ttaudio::WaveFormat format){
  return Wave.GetConvertedSample(i, format);
}
