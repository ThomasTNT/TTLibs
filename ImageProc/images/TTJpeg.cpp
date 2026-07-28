#include "TTJpeg.h"

//******************************************************************************
//*                                  TTJpeg                                    *
//*                                                                            *
//*                     class for baseline JPEG handling                       *
//******************************************************************************


//*******************
// GetJpegMarkerName
//
// method for printing
//*******************
std::string image::GetJpegMarkerName(const U16 marker){
  switch (marker){
    case MARKER_SOI:  return "SOI";
    case MARKER_EOI:  return "EOI";
    case MARKER_SOS:  return "SOS";
    case MARKER_DQT:  return "DQT";
    case MARKER_DHT:  return "DHT";
    case MARKER_DRI:  return "DRI";
    case MARKER_SOF0: return "SOF0 (baseline sequential)";
    case MARKER_SOF1: return "SOF1 (extended sequential)";
    case MARKER_SOF2: return "SOF2 (progressive)";
    case MARKER_TEM:  return "TEM";
    case MARKER_COM:  return "COM";
  }
  if (marker >= MARKER_APP0 && marker <= MARKER_APP15){
    return "APP" + ttutil::StringUtil::uint2string(marker - MARKER_APP0);
  }
  if (marker >= MARKER_RST0 && marker <= MARKER_RST7){
    return "RST" + ttutil::StringUtil::uint2string(marker - MARKER_RST0);
  }
  if (marker >= 0xFFC0 && marker <= 0xFFCF){
    return "SOF" + ttutil::StringUtil::uint2string(marker - 0xFFC0);
  }
  return "0x" + ttutil::StringUtil::uint2hexstring(marker);
}


//***********************************************************************
//*                           JpegBitReader                             *
//* PRIVATELY USED CLASS                                                *
//* reads single bits MSB first from the entropy coded scan data,       *
//* handles stuffed bytes (0xFF 0x00) and restart markers               *
//***********************************************************************

//************
// constructor
//************
image::JpegBitReader::JpegBitReader(const U8* const data, const unsigned int length)
                     :Data(data), Length(length)
{
}

//**********************************************
// NextByte
//
// returns the next data byte, skips stuffed bytes
//**********************************************
U8 image::JpegBitReader::NextByte(){
  if (BytePos >= Length){
    throw TTImageException("TTJpeg: unexpected end of scan data!");
  }
  const U8 value = Data[BytePos];
  ++BytePos;
  if (value == 0xFF){
    if (BytePos >= Length){
      throw TTImageException("TTJpeg: unexpected end of scan data!");
    }
    const U8 code = Data[BytePos];
    if (code == 0x00){
      //stuffed byte: the 0x00 is not data
      ++BytePos;
    }
    else{
      //a real marker inside the bit stream: restart markers must be consumed
      //via SyncToRestartMarker, everything else is a broken file
      throw TTImageException("TTJpeg: unexpected marker inside scan data!");
    }
  }
  return value;
}

//**************************
// NextBit
//
// returns the next single bit
//**************************
unsigned int image::JpegBitReader::NextBit(){
  if (BitCount == 0){
    CurrentByte = NextByte();
    BitCount = 8;
  }
  --BitCount;
  return (CurrentByte >> BitCount) & 1;
}

//**************************
// ReadBits
//
// reads count bits MSB first
//**************************
unsigned int image::JpegBitReader::ReadBits(const unsigned int count){
  unsigned int value = 0;
  for (unsigned int i = 0; i < count; ++i){
    value = (value << 1) | NextBit();
  }
  return value;
}

//************************************************************
// SyncToRestartMarker
//
// aligns to the next byte and consumes the expected restart
// marker; restartIndex counts the restarts, the marker code
// cycles through RST0 ... RST7
//************************************************************
void image::JpegBitReader::SyncToRestartMarker(const unsigned int restartIndex){
  //discard the remaining bits of the current byte
  BitCount = 0;
  if (BytePos + 1 >= Length){
    throw TTImageException("TTJpeg: unexpected end of scan data at restart marker!");
  }
  const U8 expected = 0xD0 + (restartIndex & 0x07);
  if (Data[BytePos] != 0xFF || Data[BytePos + 1] != expected){
    throw TTImageException("TTJpeg: restart marker expected but not found!");
  }
  BytePos += 2;
}

//*******************
// AtEnd
//
// all data consumed?
//*******************
bool image::JpegBitReader::AtEnd() const{
  return BytePos >= Length && BitCount == 0;
}


//***********************************************************************
//*                          JpegQuantTable                             *
//* PRIVATELY USED CLASS                                                *
//* one quantization table as read from a DQT segment                   *
//***********************************************************************

//*********************
// ToString
//
// method for printing
//*********************
std::string image::JpegQuantTable::ToString() const{
  std::string s;
  for (unsigned int i = 0; i < 64; ++i){
    if (i > 0){
      s += i % 8 == 0 ? "\n    " : ", ";
    }
    s += ttutil::StringUtil::uint2string(Values[i]);
  }
  return s;
}


//***********************************************************************
//*                         JpegHuffmanTable                            *
//* PRIVATELY USED CLASS                                                *
//* one huffman table as read from a DHT segment with the canonical     *
//* decode helpers as described in the JPEG spec (annex F.2.2.3)        *
//***********************************************************************

//*****************************************************************
// Build
//
// builds the canonical huffman decode helpers from CodeCounts and
// Symbols; codes of the same length are consecutive, each length
// starts with the doubled successor code of the previous length
//*****************************************************************
void image::JpegHuffmanTable::Build(){
  S32 code = 0;
  unsigned int symbolIndex = 0;
  for (unsigned int length = 1; length <= 16; ++length){
    if (CodeCounts[length] > 0){
      ValPtr[length] = symbolIndex;
      MinCode[length] = code;
      code += CodeCounts[length];
      MaxCode[length] = code - 1;
      symbolIndex += CodeCounts[length];
    }
    else{
      MaxCode[length] = -1;
    }
    code <<= 1;
  }
}

//*******************************************
// DecodeSymbol
//
// decodes the next symbol from the bit reader
//*******************************************
U8 image::JpegHuffmanTable::DecodeSymbol(JpegBitReader& reader) const{
  unsigned int length = 1;
  S32 code = reader.NextBit();
  while (code > MaxCode[length]){
    ++length;
    if (length > 16){
      throw TTImageException("TTJpeg: invalid huffman code in scan data!");
    }
    code = (code << 1) | reader.NextBit();
  }
  return Symbols[ValPtr[length] + code - MinCode[length]];
}

//*********************
// ToString
//
// method for printing
//*********************
std::string image::JpegHuffmanTable::ToString() const{
  std::string s = "counts ";
  for (unsigned int length = 1; length <= 16; ++length){
    if (length > 1) s += ",";
    s += ttutil::StringUtil::uint2string(CodeCounts[length]);
  }
  s += "; " + ttutil::StringUtil::uint2string(static_cast<unsigned int>(Symbols.size())) + " symbols";
  return s;
}


//***********************************************************************
//*                         JpegComponentInfo                           *
//* PRIVATELY USED CLASS                                                *
//* stores the frame and scan parameters of one color component         *
//* (Y, Cb or Cr) as read from the SOF and SOS segments                 *
//***********************************************************************

//*********************
// ToString
//
// method for printing
//*********************
std::string image::JpegComponentInfo::ToString() const{
  return "Component(id " + ttutil::StringUtil::uint2string(Id)
       + ", sampling " + ttutil::StringUtil::uint2string(SamplingHorizontal) + "x" + ttutil::StringUtil::uint2string(SamplingVertical)
       + ", quant table " + ttutil::StringUtil::uint2string(QuantTableId)
       + ", dc table " + ttutil::StringUtil::uint2string(DcTableId)
       + ", ac table " + ttutil::StringUtil::uint2string(AcTableId) + ")";
}


//******************************************************************************
//*                                  TTJpeg                                    *
//******************************************************************************

//************
// constructor
//************
image::TTJpeg::TTJpeg() : TTMemoryImage(), TTByteOrderFile(){
  //JPEG is always big endian
  ByteOrderIntel = false;
}

//***********
// destructor
//***********
image::TTJpeg::~TTJpeg(){
  Close();
}

//************************
// Open
//
// opens a image from file
//************************
bool image::TTJpeg::Open(const std::string& filename){
  return OpenInternal(filename, true);
}

//**********************************************************
// OpenFileInfo
//
// opens a image from file but does not load the image data
//**********************************************************
bool image::TTJpeg::OpenFileInfo(const std::string& filename){
  return OpenInternal(filename, false);
}

//************************
// OpenInternal
//
// opens a image from file
//************************
bool image::TTJpeg::OpenInternal(const std::string& filename, const bool readImageData){

  //open file
  file = new std::fstream();
  file->open(filename.c_str(), std::ios::in | std::ios::binary);
  if (!file->good()){
    Close();
    return false;
  }

  //check start of image marker
  if (ReadU16() != MARKER_SOI){
    throw TTImageException("TTJpeg::Open: File is not a JPEG!");
  }
  SegmentLog.push_back(GetJpegMarkerName(MARKER_SOI));

  //walk over all segments until end of image
  bool endOfImage = false;
  while (!endOfImage){
    if (!file->good()){
      throw TTImageException("TTJpeg::Open: Unexpected end of file!");
    }
    const U16 marker = ReadNextMarker();
    switch (marker){
      case MARKER_SOF0:
      case MARKER_SOF1:
        ReadStartOfFrame(marker);
        break;
      case MARKER_SOF2:
        throw TTImageException("TTJpeg::Open: progressive JPEG is not supported!");
      case MARKER_DQT:
        ReadQuantTables();
        break;
      case MARKER_DHT:
        ReadHuffmanTables();
        break;
      case MARKER_DRI:
        ReadRestartInterval();
        break;
      case MARKER_SOS:
        ReadStartOfScan();
        SkipScanData();
        break;
      case MARKER_COM:
        ReadComment();
        break;
      case MARKER_EOI:
        SegmentLog.push_back(GetJpegMarkerName(MARKER_EOI));
        endOfImage = true;
        break;
      default:
        if (marker >= MARKER_APP0 && marker <= MARKER_APP15){
          ReadAppSegment(marker);
        }
        else if (marker >= 0xFFC0 && marker <= 0xFFCF){
          //all remaining SOF types (lossless, arithmetic coded, hierarchical, ...)
          throw TTImageException("TTJpeg::Open: unsupported JPEG type (" + GetJpegMarkerName(marker) + ")!");
        }
        else if (   (marker >= MARKER_RST0 && marker <= MARKER_RST7)
                 || marker == MARKER_TEM
                 || marker == MARKER_SOI)
        {
          throw TTImageException("TTJpeg::Open: unexpected marker " + GetJpegMarkerName(marker) + " outside of scan data!");
        }
        else{
          //unknown but harmless segment with a length field (e.g. DNL): skip it
          SkipSegment(marker);
        }
        break;
    }
  }

  //validate that we have seen a frame header and a scan
  if (ImageInfo.Width == 0 || ImageInfo.Height == 0 || ImageInfo.Components.empty()){
    throw TTImageException("TTJpeg::Open: no valid frame header (SOF) found!");
  }
  if (ScanDataStart == std::ios::pos_type(0)){
    throw TTImageException("TTJpeg::Open: no scan (SOS) found!");
  }

  //validate that all referenced tables are defined
  for (unsigned int c = 0; c < ImageInfo.Components.size(); ++c){
    const JpegComponentInfo& component = ImageInfo.Components[c];
    if (!QuantTables[component.QuantTableId].Defined){
      throw TTImageException("TTJpeg::Open: quantization table " + ttutil::StringUtil::uint2string(component.QuantTableId) + " is referenced but not defined!");
    }
    if (   !HuffmanTablesDc[component.DcTableId].Defined
        || !HuffmanTablesAc[component.AcTableId].Defined)
    {
      throw TTImageException("TTJpeg::Open: huffman table is referenced but not defined!");
    }
  }

  //read and decode the image data
  if (readImageData){
    ReadScanData();
    //TODO step 3: huffman decode, dequantize, IDCT, color conversion
    throw TTImageException("TTJpeg::Open: image data decoding not implemented yet!");
  }

  return true;
}

//***************************************
// ReadNextMarker
//
// reads the next marker, skips fill bytes
//***************************************
U16 image::TTJpeg::ReadNextMarker(){
  const U8 first = ReadU8();
  if (first != 0xFF){
    throw TTImageException("TTJpeg::Open: marker expected but not found!");
  }
  //fill bytes 0xFF are allowed before a marker
  U8 code = ReadU8();
  while (code == 0xFF){
    code = ReadU8();
  }
  return 0xFF00 | code;
}

//*****************************************
// ReadStartOfFrame
//
// reads the frame header (SOF0 or SOF1):
// image size, bit depth and the components
// with their sampling factors
//*****************************************
void image::TTJpeg::ReadStartOfFrame(const U16 marker){
  const U16 length = ReadU16();
  ImageInfo.BitsPerSample = ReadU8();
  ImageInfo.Height = ReadU16();
  ImageInfo.Width = ReadU16();
  const U8 componentCount = ReadU8();

  if (ImageInfo.BitsPerSample != 8){
    throw TTImageException("TTJpeg::Open: only 8 bit per sample supported, file has " + ttutil::StringUtil::uint2string(ImageInfo.BitsPerSample) + "!");
  }
  if (componentCount != 1 && componentCount != 3){
    throw TTImageException("TTJpeg::Open: only grayscale and YCbCr supported, file has " + ttutil::StringUtil::uint2string(componentCount) + " components!");
  }
  if (length != 8 + componentCount * 3u){
    throw TTImageException("TTJpeg::Open: invalid SOF segment length!");
  }

  for (unsigned int i = 0; i < componentCount; ++i){
    JpegComponentInfo component;
    component.Id = ReadU8();
    const U8 sampling = ReadU8();
    component.SamplingHorizontal = sampling >> 4;
    component.SamplingVertical = sampling & 0x0F;
    component.QuantTableId = ReadU8();
    if (component.SamplingHorizontal < 1 || component.SamplingHorizontal > 4 || component.SamplingVertical < 1 || component.SamplingVertical > 4){
      throw TTImageException("TTJpeg::Open: invalid sampling factors!");
    }
    ImageInfo.Components.push_back(component);
  }

  SegmentLog.push_back(GetJpegMarkerName(marker) + " " + ttutil::StringUtil::uint2string(ImageInfo.Width) + " x " + ttutil::StringUtil::uint2string(ImageInfo.Height)
                     + ", " + ttutil::StringUtil::uint2string(componentCount) + " components");
}

//*******************************************************
// ReadStartOfScan
//
// reads the scan header: the huffman table assignment
// per component; the entropy coded data follows directly
//*******************************************************
void image::TTJpeg::ReadStartOfScan(){
  const U16 length = ReadU16();
  const U8 componentCount = ReadU8();

  if (ImageInfo.Components.empty()){
    throw TTImageException("TTJpeg::Open: SOS without frame header (SOF)!");
  }
  if (componentCount != ImageInfo.Components.size()){
    //baseline camera JPEGs have exactly one interleaved scan with all components
    throw TTImageException("TTJpeg::Open: only single interleaved scans are supported!");
  }
  if (length != 6 + componentCount * 2u){
    throw TTImageException("TTJpeg::Open: invalid SOS segment length!");
  }

  for (unsigned int i = 0; i < componentCount; ++i){
    const U8 componentId = ReadU8();
    const U8 tableIds = ReadU8();
    bool found = false;
    for (unsigned int c = 0; c < ImageInfo.Components.size(); ++c){
      if (ImageInfo.Components[c].Id == componentId){
        ImageInfo.Components[c].DcTableId = tableIds >> 4;
        ImageInfo.Components[c].AcTableId = tableIds & 0x0F;
        found = true;
        break;
      }
    }
    if (!found){
      throw TTImageException("TTJpeg::Open: SOS references unknown component id " + ttutil::StringUtil::uint2string(componentId) + "!");
    }
  }

  //spectral selection and successive approximation, fixed values for baseline
  ReadU8();  //spectral start, always 0
  ReadU8();  //spectral end, always 63
  ReadU8();  //approximation, always 0

  ScanDataStart = file->tellg();
  SegmentLog.push_back(GetJpegMarkerName(MARKER_SOS) + ", " + ttutil::StringUtil::uint2string(componentCount) + " components");
}

//************************************************************
// ReadQuantTables
//
// reads the quantization tables from a DQT segment;
// one segment can contain multiple tables
//************************************************************
void image::TTJpeg::ReadQuantTables(){
  const U16 length = ReadU16();
  if (length < 2){
    throw TTImageException("TTJpeg::Open: invalid DQT segment length!");
  }
  unsigned int remaining = length - 2;
  while (remaining > 0){
    const U8 precisionAndId = ReadU8();
    const U8 precision = precisionAndId >> 4;  //0 == 8 bit, 1 == 16 bit values
    const U8 tableId = precisionAndId & 0x0F;
    if (tableId > 3 || precision > 1){
      throw TTImageException("TTJpeg::Open: invalid DQT table header!");
    }
    const unsigned int tableBytes = 1 + 64 * (precision == 1 ? 2u : 1u);
    if (remaining < tableBytes){
      throw TTImageException("TTJpeg::Open: invalid DQT segment length!");
    }
    JpegQuantTable& table = QuantTables[tableId];
    for (unsigned int i = 0; i < 64; ++i){
      table.Values[i] = precision == 1 ? ReadU16() : ReadU8();
    }
    table.Defined = true;
    remaining -= tableBytes;
    SegmentLog.push_back(GetJpegMarkerName(MARKER_DQT) + " table " + ttutil::StringUtil::uint2string(tableId) + (precision == 1 ? " (16 bit)" : " (8 bit)"));
  }
}

//************************************************************
// ReadHuffmanTables
//
// reads the huffman tables from a DHT segment;
// one segment can contain multiple tables
//************************************************************
void image::TTJpeg::ReadHuffmanTables(){
  const U16 length = ReadU16();
  if (length < 2){
    throw TTImageException("TTJpeg::Open: invalid DHT segment length!");
  }
  unsigned int remaining = length - 2;
  while (remaining > 0){
    if (remaining < 17){
      throw TTImageException("TTJpeg::Open: invalid DHT segment length!");
    }
    const U8 classAndId = ReadU8();
    const U8 tableClass = classAndId >> 4;  //0 == DC, 1 == AC
    const U8 tableId = classAndId & 0x0F;
    if (tableClass > 1 || tableId > 3){
      throw TTImageException("TTJpeg::Open: invalid DHT table header!");
    }
    JpegHuffmanTable& table = tableClass == 0 ? HuffmanTablesDc[tableId] : HuffmanTablesAc[tableId];
    //reset in case the file redefines a table
    table = JpegHuffmanTable();

    unsigned int symbolCount = 0;
    for (unsigned int i = 1; i <= 16; ++i){
      table.CodeCounts[i] = ReadU8();
      symbolCount += table.CodeCounts[i];
    }
    if (symbolCount > 256 || remaining < 17 + symbolCount){
      throw TTImageException("TTJpeg::Open: invalid DHT table data!");
    }
    table.Symbols.resize(symbolCount);
    for (unsigned int i = 0; i < symbolCount; ++i){
      table.Symbols[i] = ReadU8();
    }
    table.Build();
    table.Defined = true;
    remaining -= 17 + symbolCount;
    SegmentLog.push_back(GetJpegMarkerName(MARKER_DHT) + " table " + std::string(tableClass == 0 ? "DC" : "AC") + ttutil::StringUtil::uint2string(tableId)
                       + " (" + ttutil::StringUtil::uint2string(symbolCount) + " symbols)");
  }
}

//************************************************************
// ReadScanData
//
// reads the raw entropy coded scan data into the ScanData
// buffer (still with stuffed bytes and restart markers,
// the bit reader handles them)
//************************************************************
void image::TTJpeg::ReadScanData(){
  if (ScanDataLength == 0){
    throw TTImageException("TTJpeg::Open: no scan data available!");
  }
  delete[] ScanData;
  ScanData = new U8[ScanDataLength];
  file->clear();
  file->seekg(ScanDataStart);
  file->read(reinterpret_cast<char*>(ScanData), ScanDataLength);
  if (static_cast<unsigned int>(file->gcount()) != ScanDataLength){
    throw TTImageException("TTJpeg::Open: Can't read scan data!");
  }
}

//****************************************
// ReadRestartInterval
//
// reads the restart interval in MCUs (DRI)
//****************************************
void image::TTJpeg::ReadRestartInterval(){
  const U16 length = ReadU16();
  if (length != 4){
    throw TTImageException("TTJpeg::Open: invalid DRI segment length!");
  }
  ImageInfo.RestartInterval = ReadU16();
  SegmentLog.push_back(GetJpegMarkerName(MARKER_DRI) + ", interval " + ttutil::StringUtil::uint2string(ImageInfo.RestartInterval) + " MCUs");
}

//**************************
// ReadComment
//
// reads a comment segment
//**************************
void image::TTJpeg::ReadComment(){
  const U16 length = ReadU16();
  if (length < 2){
    throw TTImageException("TTJpeg::Open: invalid COM segment length!");
  }
  const std::string comment = ReadString(length - 2);
  SegmentLog.push_back(GetJpegMarkerName(MARKER_COM) + " \"" + comment + "\"");
}

//***********************************************************
// ReadAppSegment
//
// skips an application segment but extracts the identifier
// string at its start (e.g. "JFIF", "Exif") for the log
//***********************************************************
void image::TTJpeg::ReadAppSegment(const U16 marker){
  const U16 length = ReadU16();
  if (length < 2){
    throw TTImageException("TTJpeg::Open: invalid APP segment length!");
  }
  const unsigned int payload = length - 2;

  std::string identifier;
  unsigned int consumed = 0;
  while (consumed < payload && identifier.length() < 15){
    const U8 c = ReadU8();
    ++consumed;
    if (c < 32 || c >= 127) break;
    identifier += static_cast<char>(c);
  }
  file->seekg(payload - consumed, std::ios::cur);

  SegmentLog.push_back(GetJpegMarkerName(marker) + " (" + identifier + ", " + ttutil::StringUtil::uint2string(payload) + " bytes)");
}

//***********************************************************
// SkipSegment
//
// skips a segment with length field without parsing its content
//***********************************************************
void image::TTJpeg::SkipSegment(const U16 marker){
  const U16 length = ReadU16();
  if (length < 2){
    throw TTImageException("TTJpeg::Open: invalid segment length!");
  }
  file->seekg(length - 2, std::ios::cur);
  SegmentLog.push_back(GetJpegMarkerName(marker) + " (" + ttutil::StringUtil::uint2string(length - 2) + " bytes)");
}

//*********************************************************
// SkipScanData
//
// skips the entropy coded scan data until the next real
// marker; stuffed bytes (0xFF 0x00) and restart markers
// belong to the scan data
//*********************************************************
void image::TTJpeg::SkipScanData(){
  unsigned int byteCount = 0;
  while (true){
    if (!file->good()){
      throw TTImageException("TTJpeg::Open: unexpected end of file in scan data!");
    }
    const int c = file->get();
    if (c == EOF){
      throw TTImageException("TTJpeg::Open: unexpected end of file in scan data!");
    }
    if (c != 0xFF){
      ++byteCount;
      continue;
    }
    const int code = file->get();
    if (code == EOF){
      throw TTImageException("TTJpeg::Open: unexpected end of file in scan data!");
    }
    if (code == 0x00 || (code >= 0xD0 && code <= 0xD7)){
      //stuffed byte or restart marker
      byteCount += 2;
      continue;
    }
    //a real marker: put it back for the segment loop
    file->seekg(-2, std::ios::cur);
    break;
  }
  ScanDataLength = byteCount;
  SegmentLog.push_back("scan data (" + ttutil::StringUtil::uint2string(byteCount) + " bytes)");
}

//*****************************************
// Save
//
// saves the image
//*****************************************
bool image::TTJpeg::Save(const std::string& filename){
  throw TTImageException("TTJpeg::Save: JPEG writing not implemented yet!");
}

//*****************************************
// Close
//
// closes the image and frees all resources
//*****************************************
void image::TTJpeg::Close(){
  delete[] ImageDataU8;
  ImageDataU8 = nullptr;
  delete[] ScanData;
  ScanData = nullptr;
  imageData = nullptr;
  delete file;
  file = nullptr;
  ImageInfo = ImageInfoJpeg();
  QuantTables.fill(JpegQuantTable());
  HuffmanTablesDc.fill(JpegHuffmanTable());
  HuffmanTablesAc.fill(JpegHuffmanTable());
  SegmentLog.clear();
  ScanDataStart = 0;
  ScanDataLength = 0;
}

//***********************
// ResetFilePointer
//
// resets the filepointer
//***********************
bool image::TTJpeg::ResetFilePointer(){
  if (!file) return false;
  file->seekg(ScanDataStart);
  return file->good();
}

//**************************************
// Create
//
// creates the image in memory
// with default properties
//**************************************
bool image::TTJpeg::Create(unsigned int w, unsigned int h){
  return Create(w, h, RGB, INT8);
}

//**************************************
// Create
//
// creates the image in memory
// with given properties
//**************************************
bool image::TTJpeg::Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype){
  //TODO step 5: needed for JPEG writing
  throw TTImageException("TTJpeg::Create: not implemented yet!");
}

//******************
// Width
//
// returns the width
//******************
unsigned int image::TTJpeg::Width() const{
  return ImageInfo.Width;
}

//*******************
// Height
//
// returns the height
//*******************
unsigned int image::TTJpeg::Height() const{
  return ImageInfo.Height;
}

//************************************************************************
// WholeChannels
//
// returns the number of whole channels (== color channel + alpha channel)
//************************************************************************
unsigned int image::TTJpeg::WholeChannels() const{
  return ColorChannels();
}

//*************************************
// ColorChannels
//
// returns the number of color channels
//*************************************
unsigned int image::TTJpeg::ColorChannels() const{
  return static_cast<unsigned int>(ImageInfo.Components.size());
}

//*******************************************************
// AlphaChannels
//
// returns the number of alpha channels
// always 0, JPEG has no alpha
//*******************************************************
unsigned int image::TTJpeg::AlphaChannels() const{
  return 0;
}

//*********************
// Colormode
//
// return the colormode
//*********************
image::ColormodeType image::TTJpeg::Colormode() const{
  //the decoder converts YCbCr to RGB, so 3 components mean RGB here
  if (ImageInfo.Components.size() == 1) return GRAYSCALE;
  if (ImageInfo.Components.size() == 3) return RGB;
  return UNKNOWN_COLORMODE;
}

//**********************
// ColorSpace
//
// return the ColorSpace
//**********************
image::ColorSpaceType image::TTJpeg::ColorSpace() const{
  //JFIF images are sRGB by convention
  return ImageInfo.Components.empty() ? COLORSPACE_UNKNOWN : COLORSPACE_SRGB;
}

//******************************************
// Datatype
//
// return the colormodebit depth per channel
//******************************************
image::Datatyp image::TTJpeg::Datatype() const{
  return ImageInfo.BitsPerSample == 8 ? INT8 : UNKNOWN_DATATYPE;
}

//*****************************************************
// Interlaced
//
// return interlaced or not
// decoded data is always interlaced RGB or gray
//*****************************************************
bool image::TTJpeg::Interlaced() const{
  return true;
}

//**********************
// GetRed
//
// getter for pixel data
//**********************
image::t_proc image::TTJpeg::GetRed(const unsigned int x, const unsigned int y) const{
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels;
  return TTColorConv::ProcFromU8(ImageDataU8[index]);
}

//**********************
// GetGreen
//
// getter for pixel data
//**********************
image::t_proc image::TTJpeg::GetGreen(const unsigned int x, const unsigned int y) const{
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels + (channels == 3 ? 1 : 0);
  return TTColorConv::ProcFromU8(ImageDataU8[index]);
}

//**********************
// GetBlue
//
// getter for pixel data
//**********************
image::t_proc image::TTJpeg::GetBlue(const unsigned int x, const unsigned int y) const{
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels + (channels == 3 ? 2 : 0);
  return TTColorConv::ProcFromU8(ImageDataU8[index]);
}

//**********************
// GetGray
//
// getter for pixel data
//**********************
image::t_proc image::TTJpeg::GetGray(const unsigned int x, const unsigned int y) const{
  if (ImageInfo.Components.size() == 1){
    return GetRed(x, y);
  }
  return TTColorConv::GetSimpleGrayByRGB(GetRed(x, y), GetGreen(x, y), GetBlue(x, y));
}

//*******************************************
// GetAlpha
//
// getter for pixel data
// JPEG has no alpha, so it is always opaque
//*******************************************
image::t_proc image::TTJpeg::GetAlpha(const unsigned int x, const unsigned int y) const{
  return TTColorConv::ProcAlphaOpaque();
}

//***************************************
// SetZero
//
// sets the pixel to black
//***************************************
void image::TTJpeg::SetZero(unsigned int x, unsigned int y){
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels;
  for (unsigned int c = 0; c < channels; ++c){
    ImageDataU8[index + c] = 0;
  }
}

//**********************
// SetRed
//
// setter for pixel data
//**********************
void image::TTJpeg::SetRed(const unsigned int x, const unsigned int y, const t_proc r){
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels;
  ImageDataU8[index] = static_cast<U8>(TTColorConv::U8FromProc(r));
}

//**********************
// SetGreen
//
// setter for pixel data
//**********************
void image::TTJpeg::SetGreen(const unsigned int x, const unsigned int y, const t_proc g){
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels + (channels == 3 ? 1 : 0);
  ImageDataU8[index] = static_cast<U8>(TTColorConv::U8FromProc(g));
}

//**********************
// SetBlue
//
// setter for pixel data
//**********************
void image::TTJpeg::SetBlue(const unsigned int x, const unsigned int y, const t_proc b){
  if (!ImageDataU8){
    throw TTImageException("Image Buffers not available!");
  }
  const unsigned int channels = static_cast<unsigned int>(ImageInfo.Components.size());
  const unsigned int index = (x + y * ImageInfo.Width) * channels + (channels == 3 ? 2 : 0);
  ImageDataU8[index] = static_cast<U8>(TTColorConv::U8FromProc(b));
}

//**********************
// SetGray
//
// setter for pixel data
//**********************
void image::TTJpeg::SetGray(const unsigned int x, const unsigned int y, const t_proc g){
  if (ImageInfo.Components.size() == 1){
    SetRed(x, y, g);
  }
  else{
    SetRed(x, y, g);
    SetGreen(x, y, g);
    SetBlue(x, y, g);
  }
}

//*************************************************
// SetAlpha
//
// setter for pixel data
// JPEG has no alpha, so this is silently ignored
//*************************************************
void image::TTJpeg::SetAlpha(const unsigned int x, const unsigned int y, const t_proc a){
}

//****************
// GetImageInfos
//
// for information
//****************
std::string image::TTJpeg::GetImageInfos(){
  std::string s;
  s += "TTJpeg: " + ttutil::StringUtil::uint2string(Width()) + " x " + ttutil::StringUtil::uint2string(Height()) + "\n";
  s += "color channels:  " + ttutil::StringUtil::uint2string(ColorChannels()) + "\n";
  s += "alpha channels:  " + ttutil::StringUtil::uint2string(AlphaChannels()) + "\n";
  s += "color type: " + GetColormodeTypeName(Colormode()) + "\n";
  s += "date type: " + GetDatatypName(Datatype()) + "\n";
  for (unsigned int i = 0; i < ImageInfo.Components.size(); ++i){
    s += ImageInfo.Components[i].ToString() + "\n";
  }
  if (ImageInfo.RestartInterval > 0){
    s += "restart interval: " + ttutil::StringUtil::uint2string(ImageInfo.RestartInterval) + " MCUs\n";
  }
  for (unsigned int i = 0; i < 4; ++i){
    if (QuantTables[i].Defined){
      s += "quant table " + ttutil::StringUtil::uint2string(i) + ":\n    " + QuantTables[i].ToString() + "\n";
    }
  }
  for (unsigned int i = 0; i < 4; ++i){
    if (HuffmanTablesDc[i].Defined){
      s += "huffman table DC" + ttutil::StringUtil::uint2string(i) + ": " + HuffmanTablesDc[i].ToString() + "\n";
    }
  }
  for (unsigned int i = 0; i < 4; ++i){
    if (HuffmanTablesAc[i].Defined){
      s += "huffman table AC" + ttutil::StringUtil::uint2string(i) + ": " + HuffmanTablesAc[i].ToString() + "\n";
    }
  }
  s += "segments:\n";
  for (unsigned int i = 0; i < SegmentLog.size(); ++i){
    s += "  " + SegmentLog[i] + "\n";
  }
  return s;
}
