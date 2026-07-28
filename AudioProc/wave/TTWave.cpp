#include "TTWave.h"

#include "Logger.h"

//***************************************************************
//*                         RIFFHeader                          *
//*                                                             *
//*   class represents a wavefile header                        *
//*                                                             *
//***************************************************************

//********
// IsValid
//********
bool ttaudio::RIFFHeader::IsValid() const {
  return    magicNumber[0] == 82  //R
         && magicNumber[1] == 73  //I
         && magicNumber[2] == 70  //F
         && magicNumber[3] == 70  //F
         && name[0] == 87  //W
         && name[1] == 65  //A
         && name[2] == 86  //V
         && name[3] == 69; //E
}

//***********************************************
// overwritten generic >> operator for RIFFHeader
//***********************************************
std::istream& ttaudio::operator >> (std::istream& is, ttaudio::RIFFHeader& riffHeader){
  is.read((char *)&riffHeader.magicNumber, sizeof(riffHeader.magicNumber));
  is.read((char *)&riffHeader.size,        sizeof(riffHeader.size));
  is.read((char *)&riffHeader.name,        sizeof(riffHeader.name));
  return is;
}

//***********************************************
// overwritten generic << operator for RIFFHeader
//***********************************************
std::ostream& ttaudio::operator << (std::ostream& os, const ttaudio::RIFFHeader& riffHeader){
  os.write((char *)&riffHeader.magicNumber, sizeof(riffHeader.magicNumber));
  os.write((char *)&riffHeader.size,        sizeof(riffHeader.size));
  os.write((char *)&riffHeader.name,        sizeof(riffHeader.name));
  return os;
}

//********************************
// print - help and debug function
//********************************
void ttaudio::RIFFHeader::print() const {
  std::cout << "magic Number: " << char4ToString(magicNumber) << std::endl;
  std::cout << "   file size: " << size << std::endl;
  std::cout << "        name: " << char4ToString(name) << std::endl;
}


//***************************************************************
//*                         Chunk                               *
//*                                                             *
//*                  class for chunk handling                   *
//*                                                             *
//***************************************************************

//-------------- CONSTRUCTORS / DESTRUCTORS / OPERATORS --------------

//************
// constructor
//************
ttaudio::Chunk::Chunk() : size(0), startOfData(std::ios::beg), restData(NULL), sizeOfRestData(0){
  for (int i=0; i<sizeof(name); i++) name[i] = 0;
}

//******************
// copy constructor
//******************
ttaudio::Chunk::Chunk(const ttaudio::Chunk& other)
           : size(other.size),
             startOfData(other.startOfData),
             restData(0){
  copyName(other);
  copyData(other);
}

//************
// destructor
//************
ttaudio::Chunk::~Chunk(){
  delete[] restData;
}

//*********************
// assignment operator
//*********************
ttaudio::Chunk& ttaudio::Chunk::operator=(const Chunk& other){
  if (this == &other) return *this;
  copyName(other);
  size = other.size;
  copyData(other);
  return *this;
}

//****************************************************************
// overwritten generic >> operator for Chunk
//
// Note that it reads only the chunk "header data"!
// You have to read user data by explizitely calling readPayload()
// after the creation of the right derived type.
//****************************************************************
std::istream& ttaudio::operator >> (std::istream& is, ttaudio::Chunk& chunk){
  chunk.readChunk(is);
  return is;
}

//******************************************
// overwritten generic << operator for Chunk
//******************************************
std::ostream& ttaudio::operator << (std::ostream& os, const ttaudio::Chunk& chunk){
  os.write((char *)&chunk.name, sizeof(chunk.name));
  os.write((char *)&chunk.size, sizeof(chunk.size));
  chunk.writeData(os);
  return os;
}

//**********
// copyName
//**********
void ttaudio::Chunk::copyName(const Chunk& other){
  for (unsigned int i=0; i < sizeof(name); i++) name[i] = other.name[i];
}

//*******************************
// getName
//
// getter for name as std::string
//*******************************
std::string ttaudio::Chunk::getName() const {
  return char4ToString(name);
}

//**********
// copyData
//**********
void ttaudio::Chunk::copyData(const Chunk& other){
  if (sizeOfRestData != other.sizeOfRestData){
    sizeOfRestData = other.sizeOfRestData;
    delete[] restData;
    restData = new U8[sizeOfRestData];
  }
  for (unsigned int i=0; i < sizeOfRestData; ++i) restData[i] = other.restData[i];
}

//**********
// readChunk
//**********
bool ttaudio::Chunk::readChunk(std::istream& is){
  clear();
  is.read((char *)&name, sizeof(name));
  is.read((char *)&size, sizeof(size));
  startOfData = is.tellg();
  is.seekg(size, std::ios::cur);
  return true;
}

//************
// readPayload
//************
bool ttaudio::Chunk::readPayload(std::istream& is){
  if (size <= 0 || char4ToString(name).empty()) return false;

  is.seekg(startOfData);
  if (!readDataUser(is)) return false;

  std::istream::pos_type currentPos = is.tellg();
  is.seekg(startOfData, std::ios::beg);
  is.seekg(size, std::ios::cur);
  std::istream::pos_type afterChunk = is.tellg();
  is.seekg(currentPos, std::ios::beg);
  sizeOfRestData = 0;
  char temp;
  while (is.tellg() != afterChunk && !is.eof()){
    is.read(&temp, 1);
    ++sizeOfRestData;
  }

  delete[] restData;
  restData = new U8[sizeOfRestData];
  is.seekg(currentPos, std::ios::beg);
  is.read((char*)restData, sizeOfRestData);
  return true;
}

//**********
// writeData
//**********
bool ttaudio::Chunk::writeData(std::ostream& os) const {
  if (size <= 0 || char4ToString(name).empty()) return false;
  writeDataUser(os);
  os.write((char*)restData, sizeOfRestData);
  return true;
}

//******
// clear
//******
void ttaudio::Chunk::clear(){
  for (int i=0; i<sizeof(name); i++) name[i] = 0;
  size = 0;
  sizeOfRestData = 0;
  delete[] restData;
  restData = NULL;
}

//***********
// removeData
//***********
void ttaudio::Chunk::removeRestData(){
  size -= sizeOfRestData;
  sizeOfRestData = 0;
  delete[] restData;
  restData = NULL;
}

//*******
// setRest
//*******
void ttaudio::Chunk::setRestData(U32 size, U8* dataToSet){
  size -= sizeOfRestData;
  delete[] restData;
  restData = NULL;
  sizeOfRestData = size;
  if (sizeOfRestData > 0){
    restData = new U8[sizeOfRestData];
    for (unsigned int i=0; i < sizeOfRestData; ++i) restData[i] = dataToSet[i];
  }
  size += sizeOfRestData;
}

//****************
// DEBUGGING print
//****************
void ttaudio::Chunk::print() const {
  const unsigned int MAX_DATABYTES_TO_PRINT = 30;
  std::cout << "----------- Chunk ------------" << std::endl;
  std::cout << "name: " << char4ToString(name) << std::endl;
  std::cout << "size: " << size << std::endl;
  if (sizeOfRestData > 0){
    std::cout << "data(" << sizeOfRestData << "): ";
    for (unsigned int i=0; i < sizeOfRestData && i < MAX_DATABYTES_TO_PRINT; ++i){
      std::cout << std::hex << (int)restData[i] << " ";
    }
    std::cout << std::dec << std::endl;
  }
}


//*******************************************************************
//*                      WaveFormatChunk                            *
//*                                                                 *
//*   holds all format data for any kind of Wave-Files.             *
//*   provides a more useful Format interface (enum) and            *
//*   directly represents a "fmt " Chunk of an RIFF-File            *
//*                                                                 *
//*******************************************************************

//-------------- CONSTRUCTORS / DESTRUCTORS / OPERATORS --------------

//************
// constructor
//************
ttaudio::WaveFormatChunk::WaveFormatChunk()
                      :Chunk(),
                       format(1),
                       channels(1),
                       sampleRate(44100),
                       bytesPerSecond(264600),
                       blockAlign(6),
                       bitsPerSample(24){
}

//***********************************
// copy constructor for generic chunk
//***********************************
ttaudio::WaveFormatChunk::WaveFormatChunk(const Chunk& chunk)
                      :Chunk(chunk),
                       format(1),
                       channels(1),
                       sampleRate(44100),
                       bytesPerSecond(264600),
                       blockAlign(6),
                       bitsPerSample(24){
}

//*****************
// copy constructor
//*****************
ttaudio::WaveFormatChunk::WaveFormatChunk(const WaveFormatChunk& other)
                      :Chunk(other),
                       format(other.format),
                       channels(other.channels),
                       sampleRate(other.sampleRate),
                       bytesPerSecond(other.bytesPerSecond),
                       blockAlign(other.blockAlign),
                       bitsPerSample(other.bitsPerSample){
}

//***********
// destructor
//***********
ttaudio::WaveFormatChunk::~WaveFormatChunk(){}

//********************
// assignment operator
//********************
ttaudio::WaveFormatChunk& ttaudio::WaveFormatChunk::operator=(const Chunk& other){
  Chunk::operator=(other);
  return *this;
}

//********************
// assignment operator
//********************
ttaudio::WaveFormatChunk& ttaudio::WaveFormatChunk::operator=(const WaveFormatChunk& other){
  if (this == &other) return *this;
  Chunk::operator=(other);
  format         = other.format;
  channels       = other.channels;
  sampleRate     = other.sampleRate;
  bytesPerSecond = other.bytesPerSecond;
  blockAlign     = other.blockAlign;
  bitsPerSample  = other.bitsPerSample;
  return *this;
}

//-------------------- METHODS OVERWRITING BASE METHODS ----------------------

//*************
// readDataUser
//*************
bool ttaudio::WaveFormatChunk::readDataUser(std::istream& is){
  is.read((char *)&format,         sizeof(format));
  is.read((char *)&channels,       sizeof(channels));
  is.read((char *)&sampleRate,     sizeof(sampleRate));
  is.read((char *)&bytesPerSecond, sizeof(bytesPerSecond));
  is.read((char *)&blockAlign,     sizeof(blockAlign));
  is.read((char *)&bitsPerSample,  sizeof(bitsPerSample));
  return true;
}

//**************
// writeDataUser
//**************
bool ttaudio::WaveFormatChunk::writeDataUser(std::ostream& os) const {
  os.write((char *)&format,         sizeof(format));
  os.write((char *)&channels,       sizeof(channels));
  os.write((char *)&sampleRate,     sizeof(sampleRate));
  os.write((char *)&bytesPerSecond, sizeof(bytesPerSecond));
  os.write((char *)&blockAlign,     sizeof(blockAlign));
  os.write((char *)&bitsPerSample,  sizeof(bitsPerSample));
  return true;
}

//--------------------------- PUBLIC METHODS-----------------------------

//*******
// getter
//*******
U16  ttaudio::WaveFormatChunk::getFormat() const { return format; }

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::setFormat(U16 fmt){
  if (fmt == 1){
    blockAlign = getNormalBlockAlign();
    format = fmt;
  }
  else if (fmt == 153){
    format = fmt;
  }
  else{
    std::cerr << "WaveFormatChunk::setFormat(): format " << fmt << " is not supported!" << std::endl;
    return false;
  }
  return true;
}

//*******
// getter
//*******
U16  ttaudio::WaveFormatChunk::getChannels() const { return channels; }

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::setChannels(U16 cn){
  if (channels < 1) return false;
  channels = cn;
  return true;
}

//*******
// getter
//*******
U32 ttaudio::WaveFormatChunk::getSampleRate() const { return sampleRate; }

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::setSampleRate(U32 sr){
  if (sr == 0) return false;
  sampleRate = sr;
  return true;
}

//*******
// getter
//*******
U32  ttaudio::WaveFormatChunk::getBytesPerSecond() const { return bytesPerSecond; }

bool ttaudio::WaveFormatChunk::setBytesPerSecond(U32 byteps){
  std::cerr << "WaveFormatChunk::setBytesPerSecond(): This function is not supported!" << std::endl;
  return false;
}

//*******
// getter
//*******
U16  ttaudio::WaveFormatChunk::getBlockAlign() const { return blockAlign; }

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::setBlockAlign(U16 ba){
  int min = getNormalBlockAlign();
  if (ba < min){
    std::cerr << "WaveFormatChunk::setBlockAlign(): " << ba;
    std::cerr << " is too less! Must be at least " << min << "." << std::endl;
    return false;
  }
  return true;
}

//*******
// getter
//*******
U16  ttaudio::WaveFormatChunk::getBitsPerSample() const { return bitsPerSample; }

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::setBitsPerSample(U16 bitps){
  if (bitps < 1 || bitps > 32){
    std::cerr << "WaveFormatChunk::setBitsPerSample(): ";
    std::cerr << " bits per sample must be in range (1...32)!" << std::endl;
    return false;
  }
  bitsPerSample = bitps;
  if (format == 153 && bitsPerSample == 20){
    blockAlign = 5;
  }
  else{
    int blockAlign = bitsPerSample / 8;
    if (blockAlign % 8 > 0) ++blockAlign;
  }
  return true;
}

//*******
// getter
//*******
ttaudio::WaveFormat ttaudio::WaveFormatChunk::getWaveFormat() const {
  if (format == 1){
    if      (bitsPerSample == 16) return NORMAL16;
    else if (bitsPerSample == 20) return NORMAL24;
    else if (bitsPerSample == 24) return NORMAL24;
    else if (bitsPerSample == 32) return STUDER24L;
    else return UNKNOWN;
  }
  else if (format == 153){
    if      (bitsPerSample == 20 && channels == 2) return STUDER20;
    else if (bitsPerSample == 24 && channels == 2) return STUDER24;
    else return UNKNOWN;
  }
  else if (format == 3){
    if (bitsPerSample == 32) return FLOAT32;
  }
  return INVALID;
}

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::setWaveFormat(WaveFormat wf){
  if (wf == INVALID){
    std::cerr << "WaveFormatChunk::setWaveFormat(): Refuse to set format to 'INVALID'!" << std::endl;
    return false;
  }
  if (wf == UNKNOWN){
    std::cerr << "WaveFormatChunk::setWaveFormat(): Refuse to set format to 'UNKNOWN'!" << std::endl;
    return false;
  }
  else if (wf == NORMAL16){
    format = 1;
    bitsPerSample = 16;
    blockAlign = getNormalBlockAlign();
    removeRestData();
  }
  else if (wf == NORMAL24){
    format = 1;
    bitsPerSample = 24;
    blockAlign = getNormalBlockAlign();
    removeRestData();
  }
  else if (wf == STUDER20){
    format = 153;
    bitsPerSample = 20;
    blockAlign = 5;
    U8 data[6] = { 4, 0, 0, 0, 0, 0 };
    setRestData(6, data);
  }
  else if (wf == STUDER24){
    format = 153;
    bitsPerSample = 24;
    blockAlign = getNormalBlockAlign();
    U8 data[4] = { 2, 0, 0, 0 };
    setRestData(4, data);
  }
  else if (wf == STUDER24L){
    format = 1;
    bitsPerSample = 32;
    blockAlign = getNormalBlockAlign();
    removeRestData();
  }
  else if (wf == FLOAT32){
    format = 3;
    bitsPerSample = 32;
    blockAlign = getNormalBlockAlign();
    removeRestData();
  }
  bytesPerSecond = sampleRate * blockAlign;
  return true;
}

//*******
// setter
//*******
bool ttaudio::WaveFormatChunk::isValid() const {
  WaveFormat wf = getWaveFormat();
  return ( wf != INVALID && wf != UNKNOWN );
}

//*********
// validize
//*********
bool ttaudio::WaveFormatChunk::validize(){
  //@todo
  return true;
}

//*******
// print
//*******
void ttaudio::WaveFormatChunk::print() const {
  Chunk::print();
  std::cout << "        format: " << format         << std::endl;
  std::cout << "      channels: " << channels       << std::endl;
  std::cout << "    sampleRate: " << sampleRate     << std::endl;
  std::cout << "bytesPerSecond: " << bytesPerSecond << std::endl;
  std::cout << "    blockAlign: " << blockAlign     << std::endl;
  std::cout << " bitsPerSample: " << bitsPerSample  << std::endl;
}

//------------------------ PRIVATE METHODS --------------------------

//********************
// getNormalBlockAlign
//********************
U16 ttaudio::WaveFormatChunk::getNormalBlockAlign() const {
  U16 ba = bitsPerSample / 8;
  if (bitsPerSample % 8 > 0) ++ba;
  ba *= channels;
  return ba;
}


//*******************************************************************
//*                      AudioDataChunk                             *
//*                                                                 *
//*   holds the actual audio data                                   *
//*                                                                 *
//*******************************************************************

//-------------- CONSTRUCTORS / DESTRUCTORS / OPERATORS --------------

//************
// constructor
//************
ttaudio::AudioDataChunk::AudioDataChunk(const Chunk& chunk)
                     :Chunk(chunk){
}

//*****************
// copy constructor
//*****************
ttaudio::AudioDataChunk::AudioDataChunk(const AudioDataChunk& other)
                     :Chunk(other){
}

//***********
// destructor
//***********
ttaudio::AudioDataChunk::~AudioDataChunk(){
}

//**************************************
// assignment operator for generic chunk
//**************************************
ttaudio::AudioDataChunk& ttaudio::AudioDataChunk::operator=(const Chunk& other){
  Chunk::operator=(other);
  return *this;
}

//********************
// assignment operator
//********************
ttaudio::AudioDataChunk& ttaudio::AudioDataChunk::operator=(const AudioDataChunk& other){
  if (this == &other) return *this;
  Chunk::operator=(other);
  return *this;
}

//*******
// print
//*******
void ttaudio::AudioDataChunk::print(){
  Chunk::print();
  std::cout << "audio data" << std::endl;
}

//-------------------- METHODS OVERWRITING BASE METHODS ----------------------

//*************
// readDataUser
//*************
bool ttaudio::AudioDataChunk::readDataUser(std::istream& is){
  is.seekg(size, std::ios::cur);
  return false;
}


//***************************************************************
//*                          TTWave                             *
//*                                                             *
//*   class represents a wave file and encapsulates the access  *
//*                                                             *
//***************************************************************

//---------------- CONSTRUCTORS / DESTRUCTORS / OPERATORS --------------

//************
// constructor
//************
ttaudio::TTWave::TTWave(){
}

//***********
// destructor
//***********
ttaudio::TTWave::~TTWave(){
  if (!ReadingBufferFromOutside) delete[] ReadingBuffer;
  delete[] SampleTempBuffer;
}

//********
// IsValid
//********
bool ttaudio::TTWave::IsValid() const {
  return Audio != nullptr && InputFormat != nullptr;
}

// ------------------------ PUBLIC METHODS -------------------

//*****
// Save
//*****
bool ttaudio::TTWave::Save(const std::string& filename){
  std::ofstream outfile(filename.c_str(), std::ios::out|std::ios::binary);
  if (!outfile){
    std::cerr << "could not create " << filename << std::endl;
    return false;
  }
  outfile << Header;

  WaveFormatChunk outform(*InputFormat);
  outform.setWaveFormat(NORMAL24);

  Chunk* currChunk = NULL;
  for (unsigned int i=0; i < Chunks.size(); ++i){
    currChunk = Chunks[i];
    if (currChunk->getName() == "fmt "){
      outfile << outform;
    }
    else{
      outfile << *currChunk;
    }

    if (currChunk->getName() == "data"){
      SetDataFilePointer(0);
      // audio conversion not yet implemented
    }
  }

  outfile.close();
  return true;
}

//*****
// Open
//*****
bool ttaudio::TTWave::Open(const std::string& filename){
  Filename = filename;

  File.clear();
  if (File.is_open()) File.close();

  File.open(filename.c_str(), std::ios::in|std::ios::binary);
  if (!File){
    Logger::getInstance().println("ERROR: could not open " + filename + "!");
    return false;
  }

  File >> Header;
  if (!Header.IsValid()){
    Logger::getInstance().println("ERROR: File " + filename + " is not a wave file!");
    return false;
  }

  Chunk  chunk;
  Chunk* currChunk = NULL;
  bool   dataReached = false;
  while (!dataReached && !File.eof()){
    File >> chunk;
    if (chunk.getName() == "fmt "){
      InputFormat  = new WaveFormatChunk(chunk);
      OutputFormat = new WaveFormatChunk(*InputFormat);
      currChunk    = InputFormat;
    }
    else if (chunk.getName() == "data"){
      Audio       = new AudioDataChunk(chunk);
      currChunk   = Audio;
      dataReached = true;
    }
    else{
      currChunk = new Chunk(chunk);
    }
    Chunks.push_back(currChunk);
    currChunk->readPayload(File);
  }

  SetDataFilePointer(0);
  const unsigned int channelsOfWave = InputFormat->getChannels();
  SampleTempBuffer = new U32[channelsOfWave];

  LengthSamples = Audio->size / InputFormat->getBlockAlign();
  return true;
}


//----------------------- PUBLIC AUDIO METHODS ---------------------------

//***************
// GetInputFormat
//***************
ttaudio::WaveFormatChunk ttaudio::TTWave::GetInputFormat() const {
  return *InputFormat;
}

//****************
// GetOutputFormat
//****************
ttaudio::WaveFormatChunk ttaudio::TTWave::GetOutputFormat() const {
  return *OutputFormat;
}

//***************
// SetOutputFormat
//***************
void ttaudio::TTWave::SetOutputFormat(const WaveFormatChunk& outputFormat){
  *OutputFormat = outputFormat;
}

//*******
// GetName
//*******
std::string ttaudio::TTWave::GetName() const {
  return Filename;
}

//*********************
// GetLengthSamples
//*********************
unsigned int ttaudio::TTWave::GetLengthSamples() const {
  return LengthSamples;
}

//***************
// GetChannels
//***************
unsigned int ttaudio::TTWave::GetChannels() const {
  return InputFormat->getChannels();
}

//*********************
// CreateReadingBuffer
//*********************
void ttaudio::TTWave::CreateReadingBuffer(const unsigned int samples, const bool dontCreateIfExists){
  if (InputFormat){
    if (!ReadingBufferFromOutside || !ReadingBuffer){
      const unsigned int channelOfWave = InputFormat->getChannels();
      const unsigned int bytesOfSample = InputFormat->getBlockAlign();
      const unsigned int bufSize = channelOfWave * bytesOfSample * samples;
      if (!dontCreateIfExists || bufSize > ReadingBufferSize){
        ReadingBufferSize = bufSize;
        ReadingBuffer = new char[ReadingBufferSize];
        ReadingBufferFromOutside = false;
      }
    }
  }
}

//**********************
// SetReadingBuffer
//**********************
void ttaudio::TTWave::SetReadingBuffer(char* readingBuffer){
  ReadingBuffer = readingBuffer;
  ReadingBufferFromOutside = true;
}

//*********************
// ReadSamplesFromFile
//*********************
unsigned int ttaudio::TTWave::ReadSamplesFromFile(const unsigned int samplesToRead){
  const unsigned int bytesOfSample = InputFormat->getBlockAlign();
  const unsigned int bytesToRead = bytesOfSample * samplesToRead;
  const unsigned int readBytes = ReadBytesFromFile(bytesToRead);
  const unsigned int actualReadSamples = readBytes / bytesOfSample;
  return actualReadSamples;
}

//*********************
// GetConvertedSample
//*********************
const U32* ttaudio::TTWave::GetConvertedSample(const unsigned int i, const ttaudio::WaveFormat format){
  ConvertSampleInput(i);
  ConvertSampleOutput(format);
  return SampleTempBuffer;
}

//**************
// SeekToSample
//**************
void ttaudio::TTWave::SeekToSample(const unsigned int samplePos){
  SetDataFilePointer(samplePos);
}

//***************
// ResetToStart
//***************
void ttaudio::TTWave::ResetToStart(){
  SetDataFilePointer(0);
}

//--------------------------- PRIVATE METHODS ----------------------------

//*******************
// SetDataFilePointer
//*******************
void ttaudio::TTWave::SetDataFilePointer(const unsigned int samplePos){
  if (File.good() && InputFormat){
    const unsigned int bytesOfSample = InputFormat->getBlockAlign();
    const std::istream::pos_type pos = Audio->startOfData + static_cast<std::istream::pos_type>(bytesOfSample * samplePos);
    File.seekg(pos, std::ios::beg);
  }
}

//******************
// ReadBytesFromFile
//******************
unsigned int ttaudio::TTWave::ReadBytesFromFile(const unsigned int bytesToRead){
  File.read(ReadingBuffer, bytesToRead);
  if (File.bad()){
    Logger::getInstance().println("ERROR while reading wave samples!");
    return static_cast<unsigned int>(File.gcount());
  }
  return static_cast<unsigned int>(File.gcount());
}

//**********************
// ConvertSampleInput
//
// copy and convert a sample on samplePos from ReadingBuffer to SampleTempBuffer
//**********************
void ttaudio::TTWave::ConvertSampleInput(const unsigned int samplePos){
  const unsigned int bytesOfSample = InputFormat->getBlockAlign();
  const unsigned int indexOfSample = samplePos * bytesOfSample;

  if (InputFormat->getWaveFormat() == STUDER20){
    SampleTempBuffer[0] = ( ReadingBuffer[indexOfSample + 0] << 12)  + (ReadingBuffer[indexOfSample + 1] << 4) + ((ReadingBuffer[indexOfSample + 2] | 0xF0) >> 4);
    SampleTempBuffer[1] = ((ReadingBuffer[indexOfSample + 2] | 0x0F) << 16) + (ReadingBuffer[indexOfSample + 3] << 8) + (ReadingBuffer[indexOfSample + 4]);
  }
  else if (InputFormat->getWaveFormat() == STUDER24){
    SampleTempBuffer[0] = (ReadingBuffer[indexOfSample + 1] << 16) + (ReadingBuffer[indexOfSample + 0] << 8) + (ReadingBuffer[indexOfSample + 3]);
    SampleTempBuffer[1] = (ReadingBuffer[indexOfSample + 2] << 16) + (ReadingBuffer[indexOfSample + 5] << 8) + (ReadingBuffer[indexOfSample + 4]);
  }
  else if (InputFormat->getWaveFormat() == FLOAT32){
    //TODO
  }
  else{
    const unsigned int channelsOfWave = InputFormat->getChannels();
    const unsigned int bytesPerSample = InputFormat->getBlockAlign() / channelsOfWave;
    for (unsigned int channel = 0; channel < channelsOfWave; ++channel){
      SampleTempBuffer[channel] = 0;
      for (unsigned int byteNr = 0; byteNr < bytesPerSample; ++byteNr){
        const U8 byte = ReadingBuffer[indexOfSample + bytesPerSample * channel + byteNr];
        SampleTempBuffer[channel] += byte << (byteNr * 8);
      }
    }
  }
}

//***************************
// ConvertSampleOutput
//
// convert a sample in SampleTempBuffer to outformat
//***************************
void ttaudio::TTWave::ConvertSampleOutput(const ttaudio::WaveFormat format){
  const unsigned int channelsOfWave = InputFormat->getChannels();
  const WaveFormat formatIn = InputFormat->getWaveFormat();
  for (unsigned int channel = 0; channel < channelsOfWave; ++channel){
    SampleTempBuffer[channel] = Convert(SampleTempBuffer[channel], formatIn, format);
  }
}

//************
// PrintSample
//************
void ttaudio::TTWave::PrintSample(U32 value, int size){
  const unsigned int WIDTH = 10;
  const unsigned int maxvalue = (1 << size);
  int realvalue = (value >= maxvalue/2) ? (value - maxvalue) : value;
  std::string line;
  int pos = (WIDTH * (realvalue + maxvalue/2))/maxvalue;
  for (int i=0; i<WIDTH; i++){
    if (i != pos) line += " "; else line += "*";
  }
  std::cout << line << std::endl;
}
