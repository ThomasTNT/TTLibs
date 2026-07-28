#pragma once

#include <iostream>
#include <iomanip>

#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "WaveBasics.h"

namespace ttaudio {


//***************************************************************
//*                         RIFFHeader                          *
//*                                                             *
//*   class represents a wavefile header                        *
//*                                                             *
//***************************************************************
class RIFFHeader {
  public:

    char magicNumber[4];
    U32  size;
    char name[4];

    bool IsValid() const;
    void print() const;
};

std::istream& operator >> (std::istream& is, RIFFHeader& riffHeader);
std::ostream& operator << (std::ostream& os, const RIFFHeader& riffHeader);


//***************************************************************
//*                         Chunk                               *
//*                                                             *
//*                  class for chunk handling                   *
//*                                                             *
//***************************************************************
class Chunk {
  public:

    char                   name[4];
    U32                    size;
    U8*                    restData;
    U32                    sizeOfRestData;
    std::istream::pos_type startOfData;

    Chunk();
    Chunk(const Chunk& other);
    virtual ~Chunk();
    Chunk& operator=(const Chunk& other);
    void clear();

    std::string getName() const;

    bool readChunk(std::istream& is);
    bool readPayload(std::istream& is);

    bool writeData(std::ostream& os) const;
    void removeRestData();
    void setRestData(U32 size, U8* dataToSet);

    virtual void print() const;

  protected:
    virtual bool readDataUser(std::istream& is) { return true; }
    virtual bool writeDataUser(std::ostream& os) const { return true; }

  private:
    void copyName(const Chunk& other);
    void copyData(const Chunk& other);
};

std::istream& operator >> (std::istream& is, Chunk& chunk);
std::ostream& operator << (std::ostream& os, const Chunk& chunk);


//*******************************************************************
//*                      WaveFormatChunk                            *
//*                                                                 *
//*   holds all format data for any kind of Wave-Files.             *
//*   provides a more useful Format interface (enum) and            *
//*   directly represents a "fmt " Chunk of an RIFF-File            *
//*                                                                 *
//*******************************************************************
const U16 FORMAT_PCM      = 1;
const U16 FORMAT_DUALMONO = 3;
const U16 FORMAT_STUDER   = 153;

class WaveFormatChunk : public Chunk {
  public:

    WaveFormatChunk();
    WaveFormatChunk(const Chunk& chunk);
    WaveFormatChunk(const WaveFormatChunk& other);
    ~WaveFormatChunk();

    WaveFormatChunk& operator=(const Chunk& chunk);
    WaveFormatChunk& operator=(const WaveFormatChunk& other);

    U16  getFormat() const;
    bool setFormat(U16 fmt);

    U16  getChannels() const;
    bool setChannels(U16 cn);

    U32  getSampleRate() const;
    bool setSampleRate(U32 sr);

    U32  getBytesPerSecond() const;
    bool setBytesPerSecond(U32 byteps);

    U16  getBlockAlign() const;
    bool setBlockAlign(U16 ba);

    U16  getBitsPerSample() const;
    bool setBitsPerSample(U16 bitps);

    WaveFormat getWaveFormat() const;
    bool       setWaveFormat(WaveFormat wf);

    bool isValid() const;
    bool validize();

    virtual void print() const;

  protected:
    virtual bool readDataUser(std::istream& is);
    virtual bool writeDataUser(std::ostream& os) const;

  private:
    U16 format;
    U16 channels;
    U32 sampleRate;
    U32 bytesPerSecond;
    U16 blockAlign;
    U16 bitsPerSample;

    U16 getNormalBlockAlign() const;
};


//*******************************************************************
//*                      AudioDataChunk                             *
//*                                                                 *
//*   holds the actual audio data                                   *
//*                                                                 *
//*******************************************************************
class AudioDataChunk : public Chunk {
  public:

    AudioDataChunk(const Chunk& chunk);
    AudioDataChunk(const AudioDataChunk& other);
    ~AudioDataChunk();

    AudioDataChunk& operator=(const Chunk& chunk);
    AudioDataChunk& operator=(const AudioDataChunk& other);

    virtual void print();

  protected:
    virtual bool readDataUser(std::istream& is);
};


//***************************************************************
//*                          TTWave                             *
//*                                                             *
//*   class represents a wave file and encapsulates the access  *
//*                                                             *
//***************************************************************
class TTWave {
  public:

    //----------- CONSTRUCTION, INITIALIZATION AND CO. ---------------

    TTWave();
    ~TTWave();

    bool IsValid() const;

    bool Save(const std::string& filename);
    bool Open(const std::string& filename);

    //----------------------- PUBLIC AUDIO METHODS ---------------------------

    WaveFormatChunk GetInputFormat() const;
    WaveFormatChunk GetOutputFormat() const;
    void            SetOutputFormat(const WaveFormatChunk& outputFormat);

    std::string  GetName() const;
    unsigned int GetLengthSamples() const;
    unsigned int GetChannels() const;

    void CreateReadingBuffer(const unsigned int samples, const bool dontCreateIfExists);
    void SetReadingBuffer(char* readingBuffer);

    unsigned int ReadSamplesFromFile(const unsigned int samplesToRead);
    const U32*   GetConvertedSample(const unsigned int i, const WaveFormat format);

    void SeekToSample(const unsigned int samplePos);
    void ResetToStart();

  private:

    //----------------- processing -----------------

    void         SetDataFilePointer(const unsigned int samplePos);
    unsigned int ReadBytesFromFile(const unsigned int bytesToRead);
    void         ConvertSampleInput(const unsigned int samplePos);
    void         ConvertSampleOutput(const WaveFormat format);

    //----------------- help methods ---------------

    void PrintSample(U32 value, int size);

    //------------------- fields --------------------

    RIFFHeader       Header;
    WaveFormatChunk* InputFormat          = nullptr;
    WaveFormatChunk* OutputFormat         = nullptr;
    AudioDataChunk*  Audio                = nullptr;
    std::vector<Chunk*> Chunks;

    std::string  Filename;
    std::fstream File;

    unsigned int LengthSamples            = 0;
    bool         ReadingBufferFromOutside = false;
    char*        ReadingBuffer            = nullptr;
    unsigned int ReadingBufferSize        = 0;
    U32*         SampleTempBuffer         = nullptr;
};


} //end of namespace ttaudio
