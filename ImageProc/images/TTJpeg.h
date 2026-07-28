#pragma once

#include <vector>
#include <array>

#include "TTImageFiles.h"
#include "stringutil.h"

namespace image{


//******************************************************************************
//*                                  TTJpeg                                    *
//*                                                                            *
//*                     class for baseline JPEG handling                       *
//*                                                                            *
//* supported: baseline sequential DCT (SOF0/SOF1), 8 bit,                     *
//*            grayscale and YCbCr with subsampling, restart markers           *
//* not supported: progressive (SOF2), arithmetic coding, 12 bit, CMYK         *
//******************************************************************************


//***********************************************************************
//*                            marker codes                             *
//***********************************************************************
constexpr U16 MARKER_SOI   = 0xFFD8;  //start of image
constexpr U16 MARKER_EOI   = 0xFFD9;  //end of image
constexpr U16 MARKER_SOS   = 0xFFDA;  //start of scan
constexpr U16 MARKER_DQT   = 0xFFDB;  //define quantization table(s)
constexpr U16 MARKER_DHT   = 0xFFC4;  //define huffman table(s)
constexpr U16 MARKER_DRI   = 0xFFDD;  //define restart interval
constexpr U16 MARKER_SOF0  = 0xFFC0;  //start of frame: baseline sequential DCT
constexpr U16 MARKER_SOF1  = 0xFFC1;  //start of frame: extended sequential DCT (baseline compatible)
constexpr U16 MARKER_SOF2  = 0xFFC2;  //start of frame: progressive DCT (not supported)
constexpr U16 MARKER_TEM   = 0xFF01;  //temporary marker, standalone
constexpr U16 MARKER_RST0  = 0xFFD0;  //restart markers 0xFFD0 ... 0xFFD7
constexpr U16 MARKER_RST7  = 0xFFD7;
constexpr U16 MARKER_APP0  = 0xFFE0;  //application segments 0xFFE0 ... 0xFFEF (JFIF, EXIF, ...)
constexpr U16 MARKER_APP15 = 0xFFEF;
constexpr U16 MARKER_COM   = 0xFFFE;  //comment

//method for printing
std::string GetJpegMarkerName(const U16 marker);


//***********************************************************************
//*                           JpegBitReader                             *
//* PRIVATELY USED CLASS                                                *
//* reads single bits MSB first from the entropy coded scan data,       *
//* handles stuffed bytes (0xFF 0x00) and restart markers               *
//***********************************************************************
class JpegBitReader{

  public:

    //constructor
    JpegBitReader(const U8* const data, const unsigned int length);

    //returns the next single bit
    unsigned int NextBit();

    //reads count bits MSB first
    unsigned int ReadBits(const unsigned int count);

    //aligns to the next byte and consumes the expected restart marker
    void SyncToRestartMarker(const unsigned int restartIndex);

    //all data consumed?
    bool AtEnd() const;

  private:

    //returns the next data byte, skips stuffed bytes
    U8 NextByte();

    const U8* const Data;
    const unsigned int Length;
    unsigned int BytePos = 0;
    U8 CurrentByte = 0;
    unsigned int BitCount = 0;

};


//***********************************************************************
//*                          JpegQuantTable                             *
//* PRIVATELY USED CLASS                                                *
//* one quantization table as read from a DQT segment                   *
//***********************************************************************
class JpegQuantTable{

  public:

    //table was defined by a DQT segment
    bool Defined = false;

    //the 64 quantization values in zigzag order as stored in the file
    std::array<U16, 64> Values = {};

    //method for printing
    std::string ToString() const;

};


//***********************************************************************
//*                         JpegHuffmanTable                            *
//* PRIVATELY USED CLASS                                                *
//* one huffman table as read from a DHT segment with the canonical     *
//* decode helpers as described in the JPEG spec (annex F.2.2.3)        *
//***********************************************************************
class JpegHuffmanTable{

  public:

    //builds the canonical huffman decode helpers from CodeCounts and Symbols
    void Build();

    //decodes the next symbol from the bit reader
    U8 DecodeSymbol(JpegBitReader& reader) const;

    //method for printing
    std::string ToString() const;

    //table was defined by a DHT segment
    bool Defined = false;

    //number of codes per code length (index 1 ... 16, index 0 is unused)
    std::array<U8, 17> CodeCounts = {};

    //the symbols in code order
    std::vector<U8> Symbols;

  private:

    //canonical huffman decode helpers, built by Build()
    //index is the code length 1 ... 16, index 0 is unused
    std::array<S32, 17> MinCode = {};
    std::array<S32, 17> MaxCode = {};
    std::array<S32, 17> ValPtr = {};

};


//***********************************************************************
//*                         JpegComponentInfo                           *
//* PRIVATELY USED CLASS                                                *
//* stores the frame and scan parameters of one color component         *
//* (Y, Cb or Cr) as read from the SOF and SOS segments                 *
//***********************************************************************
class JpegComponentInfo{

  public:

    //component identifier as stored in the file (usually 1 == Y, 2 == Cb, 3 == Cr)
    U8 Id = 0;

    //sampling factors from SOF (e.g. Y == 2x2 and Cb/Cr == 1x1 for 4:2:0)
    U8 SamplingHorizontal = 1;
    U8 SamplingVertical = 1;

    //quantization table id from SOF
    U8 QuantTableId = 0;

    //huffman table ids from SOS
    U8 DcTableId = 0;
    U8 AcTableId = 0;

    //method for printing
    std::string ToString() const;

};


//***********************************************************************
//*                           ImageInfoJpeg                             *
//* PRIVATELY USED CLASS                                                *
//* this class capsulates the frame infos of one JPEG file              *
//* it is an internal structure of TTJpeg,                              *
//* not a class to handle JPEGs itself                                  *
//***********************************************************************
class ImageInfoJpeg{

  public:

    //image width
    unsigned int Width = 0;

    //image height
    unsigned int Height = 0;

    //number of bits per color channel (always 8 for baseline)
    unsigned int BitsPerSample = 0;

    //the color components (1 == grayscale, 3 == YCbCr)
    std::vector<JpegComponentInfo> Components;

    //restart interval in MCUs from DRI (0 == no restart markers)
    unsigned int RestartInterval = 0;

};


#pragma warning( disable : 4250 )
class TTJpeg : public virtual TTMemoryImage, public virtual TTByteOrderFile{

  public:

    //constructor
    TTJpeg();

    //destructor
    ~TTJpeg() override;


    //----------------- methods from TTFileImage -----------

    //opens a image from file
    bool Open(const std::string& filename) override;

    //opens a image from file but does not load image data
    bool OpenFileInfo(const std::string& filename) override;

    //saves the image
    bool Save(const std::string& filename) override;

    //closes the image and frees all resources
    void Close() override;

    //resets the filepointer
    bool ResetFilePointer() override;

    //--------------- methods from TTMemoryImage -----------

    //------------------ methods from TTImage --------------

    //creates the image in memory
    //with default properties
    bool Create(unsigned int w, unsigned int h) override;

    //creates the image in memory
    //with given properties
    bool Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype) override;

    //returns the width
    unsigned int Width() const override;

    //returns the height
    unsigned int Height() const override;

    //returns the number of whole channels (== color channel + alpha channel)
    unsigned int WholeChannels() const override;

    //returns the number of color channels
    unsigned int ColorChannels() const override;

    //returns the number of alpha channels (always 0, JPEG has no alpha)
    unsigned int AlphaChannels() const override;

    //return the colormode
    ColormodeType Colormode() const override;

    //return the ColorSpace
    ColorSpaceType ColorSpace() const override;

    //return the colormodebit depth per channel
    Datatyp Datatype() const override;

    //return interlaced or not (decoded data is always interlaced RGB or gray)
    bool Interlaced() const override;

    //getter for pixel data
    t_proc GetRed(const unsigned int x, const unsigned int y) const override;
    t_proc GetGreen(const unsigned int x, const unsigned int y) const override;
    t_proc GetBlue(const unsigned int x, const unsigned int y) const override;
    t_proc GetGray(const unsigned int x, const unsigned int y) const override;
    t_proc GetAlpha(const unsigned int x, const unsigned int y) const override;

    //setter for pixel data
    void SetZero(unsigned int x, unsigned int y) override;

    void SetRed(const unsigned int x, const unsigned int y, const t_proc r) override;
    void SetGreen(const unsigned int x, const unsigned int y, const t_proc g) override;
    void SetBlue(const unsigned int x, const unsigned int y, const t_proc b) override;
    void SetGray(const unsigned int x, const unsigned int y, const t_proc g) override;
    void SetAlpha(const unsigned int x, const unsigned int y, const t_proc a) override;

    //for information
    std::string GetImageInfos() override;

  private:

    //opens a image from file
    bool OpenInternal(const std::string& filename, const bool readImageData);

    //reads the next marker, skips fill bytes
    U16 ReadNextMarker();

    //methods for reading the segments
    void ReadStartOfFrame(const U16 marker);
    void ReadStartOfScan();
    void ReadQuantTables();
    void ReadHuffmanTables();
    void ReadRestartInterval();
    void ReadComment();
    void ReadAppSegment(const U16 marker);

    //reads the raw entropy coded scan data into the ScanData buffer
    void ReadScanData();

    //skips a segment with length field without parsing its content
    void SkipSegment(const U16 marker);

    //skips the entropy coded scan data until the next real marker
    void SkipScanData();

    //private fields
    ImageInfoJpeg ImageInfo;

    //quantization tables from DQT (index == table id)
    std::array<JpegQuantTable, 4> QuantTables;

    //huffman tables from DHT (index == table id)
    std::array<JpegHuffmanTable, 4> HuffmanTablesDc;
    std::array<JpegHuffmanTable, 4> HuffmanTablesAc;

    //protocol of all read segments for GetImageInfos
    std::vector<std::string> SegmentLog;

    //position and size of the entropy coded scan data
    std::ios::pos_type ScanDataStart = 0;
    unsigned int ScanDataLength = 0;

    //raw entropy coded scan data (still with stuffed bytes and restart markers)
    U8* ScanData = nullptr;

    //decoded image data (interlaced RGB or gray, filled by the decoder)
    U8* ImageDataU8 = nullptr;

};

} //end of namespace image
