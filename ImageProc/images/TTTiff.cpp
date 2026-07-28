#include "TTTiff.h"

//******************************************************************************
//*                                   TTTiff                                   *
//*                                                                            *
//*                           class for tiff handling                          *
//******************************************************************************


//***********************************************************************
//*                               Tag                                   *
//* PRIVATELY USED CLASS                                                *
//* this is a struct to strore TIFF-Tag data and some temporare runtime *
//* informations to read and write the Tags                             *
//***********************************************************************

//************
// constructor 
//************
image::Tag::Tag(const U16 id, const TagDataType type, const U32 length, const U32 dataPtr)
           :Id(id), Type(type), Length(length), DataPtr(dataPtr), Data(), ContainerType(TAGDATACONTAINERTYPE_SINGLE)
{

}

//**********************************
// sizeofType
//
// returns sizeof(datatype) in bytes
//**********************************
unsigned int image::Tag::SizeofType() const{
  switch (Type){
    case TAGDATATYPE_BYTE:     return 1;
    case TAGDATATYPE_ASCII:    return 1;
    case TAGDATATYPE_SHORT:    return 2;
    case TAGDATATYPE_LONG:     return 4;
    case TAGDATATYPE_RATIONAL: return 8;
    default: return 0;
  }
}

//********************************
// WritePtr
//
// write pointer or direct data ?
//********************************
bool image::Tag::WritePtr() const{
  return (SizeofType() * Length) > 4;
}

//*********************
// ToString
//
// method for printing
//*********************
std::string image::Tag::ToString() const{
  std::string idName = ttutil::StringUtil::int2string(Id);
  switch (Id){
    //baseline tags
    case 254: idName = "NewSubfileType"; break; //    A general indication of the kind of data contained in this subfile.
    case 255: idName = "SubfileType"; break;  //    A general indication of the kind of data contained in this subfile.
    case 256: idName = "ImageWidth"; break;  //   The number of columns in the image, i.e., the number of pixels per row.
    case 257: idName = "ImageLength"; break;  //    The number of rows of pixels in the image.
    case 258: idName = "BitsPerSample"; break;  //Number of bits per component.
    case 259: idName = "Compression"; break;  //    Compression scheme used on the image data.
    case 262: idName = "PhotometricInterpretation"; break;  //    The color space of the image data.
    case 263: idName = "Threshholding"; break;  //    For black and white TIFF files that represent shades of gray, the technique used to convert from gray to black and white pixels.
    case 264: idName = "CellWidth"; break;  //    The width of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
    case 265: idName = "CellLength"; break;  //   The length of the dithering or halftoning matrix used to create a dithered or halftoned bilevel file.
    case 266: idName = "FillOrder"; break;  //    The logical order of bits within a byte.
    case 270: idName = "ImageDescription"; break;  //   A string that describes the subject of the image.
    case 271: idName = "Make"; break;  //   The scanner manufacturer.
    case 272: idName = "Model"; break;  //    The scanner model name or number.
    case 273: idName = "StripOffset"; break;  //  s For each strip, the byte offset of that strip.
    case 274: idName = "Orientation"; break;  //    The orientation of the image with respect to the rows and columns.
    case 277: idName = "SamplesPerPixel"; break;  //    The number of components per pixel.
    case 278: idName = "RowsPerStrip"; break;  //   The number of rows per strip.
    case 279: idName = "StripByteCounts"; break;  //    For each strip, the number of bytes in the strip after compression.
    case 280: idName = "MinSampleValue"; break;  //   The minimum component value used.
    case 281: idName = "MaxSampleValue"; break;  //   The maximum component value used.
    case 282: idName = "XResolution"; break;  //    The number of pixels per ResolutionUnit in the ImageWidth direction.
    case 283: idName = "YResolution"; break;  //    The number of pixels per ResolutionUnit in the ImageLength direction.
    case 284: idName = "PlanarConfiguration"; break;  //    How the components of each pixel are stored.
    case 288: idName = "FreeOffsets"; break;  //    For each string of contiguous unused bytes in a TIFF file, the byte offset of the string.
    case 289: idName = "FreeByteCounts"; break;  // or each string of contiguous unused bytes in a TIFF file, the number of bytes in the string.
    case 290: idName = "GrayResponseUnit"; break;  // The precision of the information contained in the GrayResponseCurve.
    case 291: idName = "GrayResponseCurve"; break;  //    For grayscale data, the optical density of each possible pixel value.
    case 296: idName = "ResolutionUnit"; break;  //   The unit of measurement for XResolution and YResolution.
    case 305: idName = "Software"; break;  //   Name and version number of the software package(s) used to create the image.
    case 306: idName = "DateTime"; break;  //   Date and time of image creation.
    case 315: idName = "Artist"; break;  //   Person who created the image.
    case 316: idName = "HostComputer"; break;  //   The computer and/or operating system in use at the time of image creation.
    case 320: idName = "ColorMap"; break;  //   A color map for palette color images.
    case 338: idName = "ExtraSamples"; break;  //   Description of extra components.
    case 33432 - 65536: idName = "Copyright"; break;  //  Copyright notice.

    //extension tags
    case 269: idName = "DocumentName"; break;  // The name of the document from which this image was scanned.
    case 285: idName = "PageName"; break;  // The name of the page from which this image was scanned.
    case 286: idName = "XPosition"; break;  //  X position of the image.
    case 287: idName = "YPosition"; break;  //  Y position of the image.
    case 292: idName = "T4Options"; break;  //  Options for Group 3 Fax compression
    case 293: idName = "T6Options"; break;  //  Options for Group 4 Fax compression
    case 297: idName = "PageNumber"; break;  // The page number of the page from which this image was scanned.
    case 301: idName = "TransferFunction"; break;  // Describes a transfer function for the image in tabular style.
    case 317: idName = "Predictor"; break;  //  A mathematical operator that is applied to the image data before an encoding scheme is applied.
    case 318: idName = "WhitePoint"; break;  // The chromaticity of the white point of the image.
    case 319: idName = "PrimaryChromaticities"; break;  //  The chromaticities of the primaries of the image.
    case 321: idName = "HalftoneHints"; break;  //  Conveys to the halftone function the range of gray levels within a colorimetrically-specified image that should retain tonal detail.
    case 322: idName = "TileWidth"; break;  //  The tile width in pixels. This is the number of columns in each tile.
    case 323: idName = "TileLength"; break;  // The tile length (height) in pixels. This is the number of rows in each tile.
    case 324: idName = "TileOffsets"; break;  //  For each tile, the byte offset of that tile, as compressed and stored on disk.
    case 325: idName = "TileByteCounts"; break;  // For each tile, the number of (compressed) bytes in that tile.
    case 326: idName = "BadFaxLines"; break;  //  Used in the TIFF-F standard, denotes the number of 'bad' scan lines encountered by the facsimile device.
    case 327: idName = "CleanFaxData"; break;  // Used in the TIFF-F standard, indicates if 'bad' lines encountered during reception are stored in the data, or if 'bad' lines have been replaced by the receiver.
    case 328: idName = "ConsecutiveBadFaxLines"; break;  // Used in the TIFF-F standard, denotes the maximum number of consecutive 'bad' scanlines received.
    case 330: idName = "SubIFDs"; break;  //  Offset to child IFDs.
    case 332: idName = "InkSet"; break;  // The set of inks used in a separated (PhotometricInterpretation=5) image.
    case 333: idName = "InkNames"; break;  // The name of each ink used in a separated image.
    case 334: idName = "NumberOfInks"; break;  // The number of inks.
    case 336: idName = "DotRange"; break;  // The component values that correspond to a 0% dot and 100% dot.
    case 337: idName = "TargetPrinter"; break;  //  A description of the printing environment for which this separation is intended.
    case 339: idName = "SampleFormat"; break;  // Specifies how to interpret each data sample in a pixel.
    case 340: idName = "SMinSampleValue"; break;  //  Specifies the minimum sample value.
    case 341: idName = "SMaxSampleValue"; break;  //  Specifies the maximum sample value.
    case 342: idName = "TransferRange"; break;  //  Expands the range of the TransferFunction.
    case 343: idName = "ClipPath"; break;  // Mirrors the essentials of PostScript's path creation functionality.
    case 344: idName = "XClipPathUnits"; break;  // The number of units that span the width of the image, in terms of integer ClipPath coordinates.
    case 345: idName = "YClipPathUnits"; break;  // The number of units that span the height of the image, in terms of integer ClipPath coordinates.
    case 346: idName = "Indexed"; break;  //  Aims to broaden the support for indexed images to include support for any color space.
    case 347: idName = "JPEGTables"; break;  // JPEG quantization and/or Huffman tables.
    case 351: idName = "OPIProxy"; break;  // OPI-related.
    case 400: idName = "GlobalParametersIFD"; break;  //  Used in the TIFF-FX standard to point to an IFD containing tags that are globally applicable to the complete TIFF file.
    case 401: idName = "ProfileType"; break;  //  Used in the TIFF-FX standard, denotes the type of data stored in this file or IFD.
    case 402: idName = "FaxProfile"; break;  // Used in the TIFF-FX standard, denotes the 'profile' that applies to this file.
    case 403: idName = "CodingMethods"; break;  //  Used in the TIFF-FX standard, indicates which coding methods are used in the file.
    case 404: idName = "VersionYear"; break;  //  Used in the TIFF-FX standard, denotes the year of the standard specified by the FaxProfile field.
    case 405: idName = "ModeNumber"; break;  // Used in the TIFF-FX standard, denotes the mode of the standard specified by the FaxProfile field.
    case 433: idName = "Decode"; break;  // Used in the TIFF-F and TIFF-FX standards, holds information about the ITULAB (PhotometricInterpretation = 10) encoding.
    case 434: idName = "DefaultImageColor"; break;  //  Defined in the Mixed Raster Content part of RFC 2301, is the default color needed in areas where no image is available.
    case 512: idName = "JPEGProc"; break;  // Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 513: idName = "JPEGInterchangeFormat"; break;  //  Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 514: idName = "JPEGInterchangeFormatLength"; break;  //  Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 515: idName = "JPEGRestartInterval"; break;  //  Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 517: idName = "JPEGLosslessPredictors"; break;  // Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 518: idName = "JPEGPointTransforms"; break;  //  Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 519: idName = "JPEGQTables"; break;  //  Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 520: idName = "JPEGDCTables"; break;  // Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 521: idName = "JPEGACTables"; break;  // Old-style JPEG compression field. TechNote2 invalidates this part of the specification.
    case 529: idName = "YCbCrCoefficients"; break;  //  The transformation from RGB to YCbCr image data.
    case 530: idName = "YCbCrSubSampling"; break;  // Specifies the subsampling factors used for the chrominance components of a YCbCr image.
    case 531: idName = "YCbCrPositioning"; break;  // Specifies the positioning of subsampled chrominance components relative to luminance samples.
    case 532: idName = "ReferenceBlackWhite"; break;  //  Specifies a pair of headroom and footroom image data values (codes) for each pixel component.
    case 559: idName = "StripRowCounts"; break;  // Defined in the Mixed Raster Content part of RFC 2301, used to replace RowsPerStrip for IFDs with variable-sized strips.
    case 700: idName = "XMP"; break;  //  XML packet containing XMP metadata
    case 33723: idName = "IPTC"; break;
    case 34377: idName = "PhotoshopResources"; break;
    case 34665: idName = "ExifIFDPointer"; break;
    case 34675: idName = "GPSInfoIFDPointer"; break;
   
    case 32781 - 65536: idName = "ImageID"; break;  //  OPI-related. (note its a negative value!)
    case 34732 - 65536: idName = "ImageLayer"; break;  // Defined in the Mixed Raster Content part of RFC 2301, used to denote the particular function of this Image in the mixed raster scheme.
  }

  std::string typeName;
  switch (Type){
    case TAGDATATYPE_BYTE:      typeName = "BYTE";     break;
    case TAGDATATYPE_ASCII:     typeName = "ASCII";    break;
    case TAGDATATYPE_SHORT:     typeName = "SHORT";    break;
    case TAGDATATYPE_LONG:      typeName = "LONG";     break;
    case TAGDATATYPE_RATIONAL:  typeName = "RATIONAL"; break;
    case TAGDATATYPE_SBYTE:     typeName = "SBYTE"; break;
    case TAGDATATYPE_UNDEFINED: typeName = "UNDEFINED"; break;
    case TAGDATATYPE_SSHORT:    typeName = "SSHORT"; break;
    case TAGDATATYPE_SLONG:     typeName = "SLONG"; break;
    case TAGDATATYPE_SRATIONAL: typeName = "SRATIONAL"; break;
    case TAGDATATYPE_FLOAT:     typeName = "FLOAT"; break;
    case TAGDATATYPE_DOUBLE:    typeName = "DOUBLE"; break;
    case TAGDATATYPE_IFD:       typeName = "IFD"; break;
    default: typeName = "unknown";
  }

  std::string dataValue;

  //special interpretations
  if (Id == 700){ //XMP
    for (unsigned int i = 0; i < Data.IntArray.size() && i < 10; ++i){
      dataValue += static_cast<char>(Data.IntArray[i]);
    }
  }
  else if (Id == 34377){ //PhotoshopResources
    dataValue = "some binary data";
  }
  //all other
  else{
    if (ContainerType == TAGDATACONTAINERTYPE_SINGLE){
      switch (Type){
        case TAGDATATYPE_BYTE:     dataValue = ttutil::StringUtil::uint2hexstring(Data.ByteValue); break;
        case TAGDATATYPE_ASCII:    dataValue = Data.StringValue; break;
        case TAGDATATYPE_SHORT:    dataValue = ttutil::StringUtil::int2string(Data.ShortValue); break;
        case TAGDATATYPE_LONG:     dataValue = ttutil::StringUtil::int2string(Data.LongValue); break;
        case TAGDATATYPE_RATIONAL: dataValue = ttutil::StringUtil::int2string(Data.RationalEnumerator) + "/" + ttutil::StringUtil::int2string(Data.RationalDenominator); break;
        default: dataValue = "-----";
      }
    }
    else{
      switch (Type){
        case TAGDATATYPE_BYTE:
        case TAGDATATYPE_SHORT:
        case TAGDATATYPE_LONG:
          for (unsigned int i = 0; i < Data.IntArray.size(); ++i){
            if (i > 0) dataValue += ", ";
            dataValue += ttutil::StringUtil::int2string(Data.IntArray[i]);
          }
          break;
        default: dataValue = "possible invalid";
      }
    }
  }
  return "Tag(" + idName + ", " + typeName + ", " + ttutil::StringUtil::uint2string(Length) + ", " + dataValue + ")";
}

//************************************************************************
//*                           ImageInfoTiff                              *
//* PRIVATELY USED CLASS                                                 *
//* this class capsulates the infos for one TIFF File                    *
//* in case of multi-TIFFs it's one internal TIFF                        *
//* it is an internal structure of TIFFHandler,                          *
//* is not a class to handle TIFFs itself                                *
//*                                                                      *
//* the functionality is capsulated in an extra class for better future  *
//* extension. E.g. if you need the image data (width etc.) directly for *
//* image processing without the need to loop over all tags              *
//************************************************************************

//************* 
// constructor
//************
image::ImageInfoTiff::ImageInfoTiff()
{
}

//************
// destructor
//************
image::ImageInfoTiff::~ImageInfoTiff(){
  for (unsigned int i = 0; i < Tags.size(); ++i) delete Tags[i];
  Tags.clear();
}


//******************************************************************************
//*                                  TTTiff                                    *
//*                                                                            *
//*                          class for tiff handling                           *
//******************************************************************************

//************
// constructor
//************
image::TTTiff::TTTiff() : TTMemoryImage()
{
}

//***********
// destructor
//***********
image::TTTiff::~TTTiff(){
  Close();
}


//******************
// Width
//
// returns the width
//******************
unsigned int  image::TTTiff::Width() const { 
  return ImageInfos.size() > 0 ? ImageInfos[0]->Width : 0;
}

//*******************
// Height
//
// returns the height
//*******************
unsigned int  image::TTTiff::Height() const { 
  return ImageInfos.size() > 0 ? ImageInfos[0]->Height : 0;
}


//***********************************************************************
// WholeChannels
//
// returns the number of whole channels (== color channel + alpha channel)
//************************************************************************
unsigned int image::TTTiff::WholeChannels() const { 
  return ImageInfos.size() > 0 ? (ImageInfos[0]->Channels + ImageInfos[0]->AlphaChannels) : 0;
};

//*************************************
// ColorChannels
//
// returns the number of color channels
//*************************************
unsigned int image::TTTiff::ColorChannels() const { 
  return ImageInfos.size() > 0 ? ImageInfos[0]->Channels : 0;
  
};

//*************************************
// AlphaChannels
//
// returns the number of alpha channels
//*************************************
unsigned int image::TTTiff::AlphaChannels() const { 
  return ImageInfos.size() > 0 ? ImageInfos[0]->AlphaChannels : 0; 
};

//*********************
// Colormode
//
// return the colormode
//*********************
image::ColormodeType image::TTTiff::Colormode() const { 
  if (ImageInfos.size() > 0){
    const unsigned int channels = ImageInfos[0]->Channels;
    const unsigned int alphas = ImageInfos[0]->AlphaChannels;
    const bool interlaced = ImageInfos[0]->Interlaced;
    if (channels == 1) return GRAYSCALE; //bit?!??!
    if (interlaced){
      if (channels == 3 && alphas == 0) return RGB; //Lab?  CMYK?
      if (channels == 3 && alphas > 0) return RGBA; //Lab?  CMYK?
    }
    else{
      if (channels == 3 && alphas == 0) return RRGGBB;
      if (channels == 3 && alphas > 0) return RRGGBBAA;
    }
    return OTHERMODE;
  }
  else{
    return UNKNOWN_COLORMODE;
  }
}


//*********************
// ColorSpace
//
// return the ColorSpace
//*********************
image::ColorSpaceType image::TTTiff::ColorSpace() const { 
  if (ImageInfos.size() > 0){
    //TODO
    //ImageInfos[0]->Photometric
    return COLORSPACE_UNKNOWN;
  }
  else{
    return COLORSPACE_UNKNOWN;
  }
};


//******************************************
// Datatype
//
// return the colormodebit depth per channel
//******************************************
image::Datatyp image::TTTiff::Datatype() const { 
  if (ImageInfos.size() > 0){
    const unsigned int bits = ImageInfos[0]->BitsPerSample;
    if      (bits == 0)  return OTHERTYPE;
    else if (bits == 8)  return INT8;
    else if (bits == 16) return INT16;
    else return OTHERTYPE;
  }
  else{
    return UNKNOWN_DATATYPE;
  }
}

//*************************
// Interlaced
//
// return interlaced or not 
//*************************
bool image::TTTiff::Interlaced() const { 
  return ImageInfos.size() > 0 ? ImageInfos[0]->Interlaced : true; //interlaced is standard
}

//***********************
// ResetFilePointer
//
// resets the filepointes
//***********************
bool image::TTTiff::ResetFilePointer(){
  if (!file) return false;
  //file->seekg(header.Win3xBitmapHead.ImageDataOffset, std::ios::beg);
  if (!file->good()) return false;
  return true;
}

//************************
// open
//
// opens a image from file
//************************
bool image::TTTiff::Open(const std::string& filename){
  return OpenInternal(filename, true);
}

//************************
// open
//
// opens a image from file
//************************
bool image::TTTiff::OpenFileInfo(const std::string& filename){
  return OpenInternal(filename, false);
}

//************************
// open
//
// opens a image from file
//************************
bool image::TTTiff::OpenInternal(const std::string& filename, const bool readImageData){

  //open file
  file = new std::fstream();
  file->open(filename.c_str(), std::ios::in | std::ios::binary);
  if (!file->good()){
    Close();
    return false;
  }

  //read header
  if (!ReadTIFFHeader()){
    Close();
    return false;
  }
  //read image meta datas
  do{
    ReadTagTable();
  } while (ToNextIFD);

  //extract the image valus from the tags
  //note we can have multiple datas
  //and even valide them
  if (!ExtractImageValuesFromTags()){
    return false;
  }

  //read the image data
  if (readImageData){
    ReadImageDatas();
  }

  return true;
}

//****************
// ReadTIFFHeader
//****************
bool image::TTTiff::ReadTIFFHeader(){

  //read Header
  file->read((char *)&Header, sizeof(Header));
  if (file->gcount() != sizeof(Header)) {
    throw TTImageException("TTTiffImage::Open: Can't read header data!");
    return false;
  }
  //check ByteOrder
  if (Header.MagicNumber == TiffMagicII){
    ByteOrderIntel = true;
  }
  else if (Header.MagicNumber == TiffMagicMM){
    ByteOrderIntel = false;
  }
  else {
    throw TTImageException("TTTiffImage::Open: File is not a Tiff! ");
    return false;
  }
  //swap header data if necessary
  if (!ByteOrderIntel){
    Header.Version = SwapBytes(Header.Version);
    Header.IFDOffset = SwapBytes(Header.IFDOffset);
  }
  //check Version
  if (Header.Version != 42){
    throw TTImageException("TTTiffImage::Open: File is not a valid Tiff! ");
    return false;
  }

  ToNextIFD = Header.IFDOffset;

  return true;
}

/**
* read image meta data
* @return success
* @throws IOException wenn was schiefgeht
*/
bool image::TTTiff::ReadTagTable(){

  // //////// new IFD \\\\\\\\\
  //go to nextread image file directory
  file->seekg(ToNextIFD);

  //read Tags in this IFD
  const U16 lengthOfIFD = ReadU16();

  // readTags> + lengthOfIFD
  for (unsigned int i = 0; i < lengthOfIFD; i++){
    ReadTag();
  }
  //get pointer to next image file directory
  ToNextIFD = ReadU32();
  // toNextIFD> + toNextIFD

  return true;
}

//*********
// ReadTag
//*********
void image::TTTiff::ReadTag(){
  //read one tag
  const U16 id = ReadU16();
  const U16 type = ReadU16();
  const U32 length = ReadU32();
  const U32 dataPtr = ReadU32();

  Tag* const tag = new Tag(id, static_cast<TagDataType>(type), length, dataPtr);
  FillTagData(*tag);

  //std::cout << "Read Tag: " << tag->ToString() << std::endl;

  //start new subimage
  if (ImageInfos.size() <= 0 || tag->Id == TAG_ID_NEWSUBFILETYPE || TagAlreadyExists(tag->Id))
  {
    // ************* NEW SUBIMAGE *************
    ImageInfos.push_back(new ImageInfoTiff());
  }

  //get current imageInfo and set tag in it
  ImageInfos.back()->Tags.push_back(tag);
}


//***********************************************************************
// FillTagData
//
// actually read the tag data. Some of them will be read from the stream. 
// So this method did not belong into the Tag class, but here. 
//***********************************************************************
void image::TTTiff::FillTagData(Tag& tag){

  unsigned int sizeoftype = tag.SizeofType();
  unsigned int length = tag.Length;
  unsigned int size = sizeoftype * length;

  //directly from the data pointer field
  if (size <= 4) {
    switch (tag.Type){

    case TAGDATATYPE_BYTE:
      tag.Data.ByteValue = ByteFromDataPtr(tag.DataPtr);
      break;
    case TAGDATATYPE_SHORT:
      tag.Data.ShortValue = ShortFromDataPtr(tag.DataPtr);
      break;
    case TAGDATATYPE_LONG:
      tag.Data.LongValue = LongFromDataPtr(tag.DataPtr);
      break;
    case TAGDATATYPE_RATIONAL:
      //this cannot happen because it is at least 8 byte long
      break;
    case TAGDATATYPE_ASCII:
      tag.Data.StringValue.clear();
      for (unsigned int i = 0; i < length; ++i){
        char c = (char)((tag.DataPtr & (0x000000FF << i * 8)) >> i * 8);
        tag.Data.StringValue += c;
      }
      break;
    }
  }
  //indirectly. The data pointer field points to data in the file.  
  else{
    const std::streampos oldfilepointer = file->tellg();
    file->seekg(tag.DataPtr);

    if (tag.Type == TAGDATATYPE_RATIONAL){
      length *= 2;
      sizeoftype /= 2;
    }
    switch (tag.Type){
      case TAGDATATYPE_BYTE:
      case TAGDATATYPE_SHORT:
      case TAGDATATYPE_LONG:
      case TAGDATATYPE_RATIONAL:
        tag.Data.IntArray.clear();
        for (unsigned int i = 0; i < length; ++i){
          switch (tag.Type){
            case TAGDATATYPE_BYTE:
            {
              const U8 v8 = ReadU8();
              tag.Data.IntArray.push_back(static_cast<int>(v8));
              break;
            }
            case TAGDATATYPE_SHORT:
            {
              const U16 v16 = ReadU16();
              tag.Data.IntArray.push_back(static_cast<int>(v16));
              break;
            }
            case TAGDATATYPE_LONG:
            case TAGDATATYPE_RATIONAL:
            {
              const U32 v32 = ReadU32();
              tag.Data.IntArray.push_back(static_cast<int>(v32));
              break;
            }
          }
        }
        //copy rational type values from vector to rational values
        if (tag.Type == TAGDATATYPE_RATIONAL){
          if (tag.Data.IntArray.size() == 2){
            tag.Data.RationalEnumerator = tag.Data.IntArray[0];
            tag.Data.RationalDenominator = tag.Data.IntArray[1];
          }
          else{
            std::cerr << "Rational type but not exact 2 values!" << std::endl;
          }
        }
        //else mark all other as array type
        else{
          tag.ContainerType = TAGDATACONTAINERTYPE_ARRAY;
        }
        break;
      case TAGDATATYPE_ASCII:
        tag.Data.StringValue = ReadString(length);
        break;
    }
    file->seekg(oldfilepointer);
  }
}

//************************************************************************
// ByteFromDataPtr
//
// handles byte padding
// byte is the first (msb), not the last (lsb) in case non intel byteorder
//************************************************************************
unsigned char image::TTTiff::ByteFromDataPtr(const U32 data){
  return ByteOrderIntel ? static_cast<unsigned char>(data)
                        : static_cast<unsigned char>(data >> 24);
}

//**************************************************************************************
// ShortFromDataPtr
//
// handles byte padding
// short value is the first two bytes, not the last two byte in case non intel byteorder
//**************************************************************************************
unsigned short image::TTTiff::ShortFromDataPtr(const U32 data){
  return ByteOrderIntel ? static_cast<unsigned short>(data)
                        : static_cast<unsigned short>(data >> 16);
}

//**********************
// LongFromDataPtr
//
// handles byte padding
//**********************
unsigned long image::TTTiff::LongFromDataPtr(const U32 data){
  return static_cast<unsigned long>(data);
}

//**********************
// LongFromDataPtr
//
// handles byte padding
//**********************
U32 image::TTTiff::DataPtrFromByte(unsigned char v){
  return ByteOrderIntel ? static_cast<U32>(v)
                        : static_cast<U32>(v << 24);
}

//**********************
// LongFromDataPtr
//
// handles byte padding
//**********************
U32 image::TTTiff::DataPtrFromShort(unsigned short v){
  return ByteOrderIntel ? static_cast<U32>(v)
                        : static_cast<U32>(v << 16);
}

//**********************
// LongFromDataPtr
//
// handles byte padding
//**********************
U32 image::TTTiff::DataPtrFromLong(unsigned long v){
  return static_cast<U32>(v);
}

//************************************************
// TagAlreadyExists
//
// checks if a tag of the same type exists already
// in the current image infos
//************************************************
bool image::TTTiff::TagAlreadyExists(const U16 tagid){
  const ImageInfoTiff* const currImageInfo = ImageInfos.back();
  for (unsigned int i = 0; i < currImageInfo->Tags.size(); ++i){
    if (currImageInfo->Tags[i]->Id == tagid){
      return true;
    }
  }
  return false;
}

//*********************************************
// ExtractImageValuesFromTags
//
// reads the image vales from the tags
// at the moment only from the first ImageInfo
//********************************************
bool image::TTTiff::ExtractImageValuesFromTags(){
  //std::cout << "We have " << ImageInfos.size() << " ImageInfos. Using the first one only." << std::endl;
  ImageInfoTiff* const currImageInfo = ImageInfos[0];
  for (const Tag* const tag : currImageInfo->Tags){
    switch (tag->Id){
      case TAG_ID_IMAGEWIDTH: if (tag->Type == TAGDATATYPE_LONG) currImageInfo->Width = tag->Data.LongValue; 
                              else if (tag->Type == TAGDATATYPE_SHORT) currImageInfo->Width = tag->Data.ShortValue;
                              break; 
      case TAG_ID_IMAGELENGTH: if (tag->Type == TAGDATATYPE_LONG) currImageInfo->Height = tag->Data.LongValue;
                               else if (tag->Type == TAGDATATYPE_SHORT) currImageInfo->Height = tag->Data.ShortValue;
                               break;
      case TAG_ID_SAMPLESPERPIXEL: currImageInfo->Channels = tag->Data.ShortValue; break; 
      case TAG_ID_COMPRESSION: currImageInfo->Compression = static_cast<TiffCompression>(tag->Data.ShortValue); break; 
      case TAG_ID_BITSPERSAMPLE: 
        {
          unsigned int maxBits = 0;
          for (unsigned int i = 0; i < tag->Data.IntArray.size(); ++i){
            const unsigned int channelBits = tag->Data.IntArray[i];
            if (channelBits > maxBits) maxBits = channelBits;
          }
          currImageInfo->BitsPerSample = maxBits;
        }
        break;
      case TAG_ID_PHOTOMETRICINTERPRETATION: 
        {
          const unsigned int photometric = tag->Data.ShortValue;
          switch (photometric){
            case PHOTOMETRIC_MINISWHITE: currImageInfo->BaseColorMode = GRAY_INVERTED; break;
            case PHOTOMETRIC_MINISBLACK: currImageInfo->BaseColorMode = GRAYSCALE; break;
            case PHOTOMETRIC_RGB:        currImageInfo->BaseColorMode = RGB; break;
            case PHOTOMETRIC_YCBCR:      currImageInfo->BaseColorMode = YUV; break;
            case PHOTOMETRIC_CIELAB: 
            case PHOTOMETRIC_ICCLAB:
            case PHOTOMETRIC_ITULAB: currImageInfo->BaseColorMode = LAB; break;
            
            default: std::cerr << "Invalid photometric color mode: " << photometric << std::endl; return false;
          }
        }
        break;
  
      case TAG_ID_PLANARCONFIGURATION: currImageInfo->Interlaced = tag->Data.ShortValue == PLANARCONFIG_CONTIG ? true : false; break;
      case TAG_ID_STRIPOFFSET:
        {
          if (tag->ContainerType == TAGDATACONTAINERTYPE_SINGLE){
            currImageInfo->StripOffsets.push_back(tag->Data.LongValue);
          }
          else{
            for (unsigned int i = 0; i < tag->Data.IntArray.size(); ++i){
              currImageInfo->StripOffsets.push_back(tag->Data.IntArray[i]);
            }
          }
        }
        break;
      case TAG_ID_ROWSPERSTRIP: currImageInfo->RowsPerStrip = tag->Data.ShortValue; break; 
      case TAG_ID_STRIPBYTECOUNT: 
        {
          if (tag->ContainerType == TAGDATACONTAINERTYPE_SINGLE){
            currImageInfo->StripByteCounts.push_back(tag->Data.LongValue);
          }
          else{
            for (unsigned int i = 0; i < tag->Data.IntArray.size(); ++i){
              currImageInfo->StripByteCounts.push_back(tag->Data.IntArray[i]);
            }
          }
        }
        break;
      case TAG_ID_ORIENTATION: currImageInfo->Orientation = static_cast<TiffOrientation>(tag->Data.ShortValue); break;
    }
  }

  //correct channels and alphachannels
  switch (currImageInfo->BaseColorMode){
    case BIT:
    case BIT_INVERTED:
    case GRAYSCALE:
    case GRAY_INVERTED:  if (currImageInfo->Channels > 1){
                           currImageInfo->AlphaChannels = currImageInfo->Channels - 1;
                           currImageInfo->Channels = 1;
                         }
                         break;
    case RGB:
    case YUV:
    case LAB:            if (currImageInfo->Channels > 3){
                           currImageInfo->AlphaChannels = currImageInfo->Channels - 3;
                           currImageInfo->Channels = 3;
                         }
                         break;
  }

  //validate the values
  if (currImageInfo->Width == 0){
    throw TTImageException("Width is 0!");
    return false;
  }
  if (currImageInfo->Height == 0){
    throw TTImageException("Height is 0!");
    return false;
  }
  if (currImageInfo->BitsPerSample != 8 && currImageInfo->BitsPerSample != 16){
    throw TTImageException("Only accept 8 bit or 16 bit images!");
    return false;
  }
  if (currImageInfo->BaseColorMode != RGB){
    throw TTImageException("Only accept RGB images!");
    return false;
  }
  if (!currImageInfo->Interlaced){
    throw TTImageException("Only accept interlaced data order!");
    return false;
  }
  if (currImageInfo->Compression != COMPRESSION_NONE){
    throw TTImageException("Only non compressed tiff are allowed");
    return false;
  }
  
  if (ImageInfos.size() > 1){
    throw TTImageException("TODO: Multi TIFFs not supported at the moment! Needs further investigation!");
    return false;
  }

  return true;

}

//**********************************************
// ReadImageDatas
//
// reads the image data into the internal buffer
//**********************************************
void image::TTTiff::ReadImageDatas(){

  //read image by image infos
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];

  //allocate memory
  if (!AllocateMemory(*currImageInfo)){
    return;
  }

  //loop through the strips
  char* readBuffer = static_cast<char*>(imageData);
  for (unsigned int i = 0; i < currImageInfo->StripOffsets.size(); ++i){
    const unsigned int offset = currImageInfo->StripOffsets[i];
    const unsigned int bytesToRead = currImageInfo->StripByteCounts[i];
    file->seekg(offset);
    file->read(readBuffer, bytesToRead);
    if (file->gcount() != bytesToRead){
      throw TTImageException("ReadImageDatas: error while reading image data ! offset: " + ttutil::StringUtil::uint2string(offset) + " length: " + ttutil::StringUtil::uint2string(bytesToRead) + " could read only: " + ttutil::StringUtil::uint2string(static_cast<unsigned long>(file->gcount())) + "!");
      return;
    }
    readBuffer += bytesToRead; //increment buffer
  }

  //std::cerr << "Sucessfully read image data!" << std::endl;
}

//****************************
// Create
// 
// creates the image in memory
//****************************
bool image::TTTiff::Create(unsigned int w, unsigned int h){
  return Create(w, h, RGB, INT8);
}

//***************************
// Create
//
//creates the image in memory
//with given properties
//***************************
bool image::TTTiff::Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype){
  //validate
  if (w == 0){
    std::cerr << "Width must be greater than null!" << std::endl;
    return false;
  }
  if (h == 0){
    std::cerr << "Height must be greater than null!" << std::endl;
    return false;
  }
  if (datatype != INT8 && datatype != INT16){
    std::cerr << "Only 8 bit or 16 bit images allowed!" << std::endl;
    return false;
  }
  if (colormode == BGR || GetSimpleColormodeType(colormode) != RGB){
    std::cerr << "Only RGB colormodes allowed!" << std::endl;
    return false;
  }

  //create image header
  Header.MagicNumber = TiffMagicII; //windows byte order only, should we make this configurable?
  Header.Version = 42;        //always 42(decimal)
  Header.IFDOffset = sizeof(Header); //should be 8      

  //create image info
  ImageInfoTiff* const currImageInfo = new ImageInfoTiff;
  currImageInfo->Width = w;
  currImageInfo->Height = h;
  currImageInfo->RowsPerStrip = w;

  currImageInfo->BitsPerSample = datatype == INT16 ? 16 : 8;

  currImageInfo->BaseColorMode = GetSimpleColormodeType(colormode);
  currImageInfo->Channels = GetChannelsByColormodeType(colormode);
  currImageInfo->AlphaChannels = GetAlphaChannelsByColormodeType(colormode);
  currImageInfo->Photometric = PHOTOMETRIC_RGB;
  currImageInfo->Interlaced = GetInterlacedByColormodeType(colormode);
  currImageInfo->Compression = COMPRESSION_NONE;
  ImageInfos.push_back(currImageInfo);

  return AllocateMemory(*currImageInfo);

}

//****************
// save
//
// saves the image
//****************
bool image::TTTiff::Save(const std::string& filename){

  //at the moment we save only with windows byte order 
  //TODO later ...

  //open the file
  //open file
  file = new std::fstream();
  file->open(filename.c_str(), std::ios::out | std::ios::binary);
  if (!file->good()){
    return false;
  }

  //prepare tags for writing
  //using only first ImageInfo
  ImageInfoTiff* const currImageInfo = ImageInfos[0];
  CreateTagsForWriting(*currImageInfo);

  //write header
  file->write(reinterpret_cast<char *>(&Header), sizeof(Header));

  //-------------------- all tags in one table -------------------

  //write tag table
  unsigned short lengthOfIFD = static_cast<unsigned short>(currImageInfo->Tags.size());
  WriteU16(lengthOfIFD);

  //write tags ifselves
  //with placeholders
  //for indirect datas the pointer to the data pointer (the datafield ...) will be stored in tag->Ptr
  std::vector<Tag*> tagsWithPointer;
  std::vector<Tag*> stripOffsets; 
  std::vector<Tag*> stripCounts;
  for (unsigned int i = 0; i < lengthOfIFD; i++){
    SaveTag(currImageInfo->Tags[i], tagsWithPointer, stripOffsets, stripCounts);
  }

  //write "placeholder" for next IFD
  //it will not be overwritten if there is no further IFD
  //and since we have only one IFD, it stays to "00 00 00 00"
  WriteU32(0);

  //----------------------- pointer values -------------------

  //write additional data where the pointers point to
  //in case of strip-tag the old values will be written at the moment
  for (Tag* tag : tagsWithPointer){

    //ptr points to the beginn of the data field
    const std::ios::pos_type dataStart = file->tellp();

    //write the datas on the position
    WriteIndirectTagData(tag);

    //write data pointer in tag in file
    file->seekp(tag->PtrToDataPtr);
    WriteU32(static_cast<U32>(dataStart));
    //set write pointer to end of file again
    file->seekp(0, std::ios_base::end);
  }

  //----------------------- image data ----------------------

  //we could have serveral strips so we loop over them
  for (unsigned int i = 0; i < stripOffsets.size(); ++i){

    Tag* const offsetTag = stripOffsets[i];
    Tag* const countTag = stripCounts[i];

    const std::ios::pos_type imageDataStart = file->tellp();
    const unsigned int numberOfBytesToWrite = countTag->DataPtr;
    const char* writeImagePointer = static_cast<const char*>(imageData);
    file->write(writeImagePointer, numberOfBytesToWrite);
    //update stripoffsettag
    offsetTag->DataPtr = static_cast<U32>(imageDataStart); //just to be sure, important is only the data in file
    file->seekg(offsetTag->PtrToDataPtr);
    WriteU32(static_cast<U32>(imageDataStart));
    file->seekp(0, std::ios_base::end);
    writeImagePointer += numberOfBytesToWrite;
  }

  file->close();
  delete file;
  file = NULL;
  //std::cout << "image saved: " << filename << std::endl;
  return true;
}

//*****************************************************
// CreateTagsForWriting
//
// creates the neccessary tags of the image for writing
//*****************************************************
void image::TTTiff::CreateTagsForWriting(ImageInfoTiff& currImageInfo){
  Tag* tag;

  tag = new Tag(TAG_ID_NEWSUBFILETYPE, TAGDATATYPE_LONG, 1, DataPtrFromLong(0));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_IMAGEWIDTH, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Width));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_IMAGELENGTH, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Height));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_BITSPERSAMPLE, TAGDATATYPE_SHORT, DataPtrFromShort(currImageInfo.Channels + currImageInfo.AlphaChannels), 0);
  tag->ContainerType = TAGDATACONTAINERTYPE_ARRAY;
  for (unsigned short i = 0; i < currImageInfo.Channels + currImageInfo.AlphaChannels; ++i){
    tag->Data.IntArray.push_back(currImageInfo.BitsPerSample);
  }
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_COMPRESSION, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Compression));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_PHOTOMETRICINTERPRETATION, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Photometric));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_STRIPOFFSET, TAGDATATYPE_LONG, 1, DataPtrFromLong(0)); //will be filled later
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_ORIENTATION, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Orientation));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_SAMPLESPERPIXEL, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Channels + currImageInfo.AlphaChannels));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_ROWSPERSTRIP, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Height)); //the whole image in one big strip
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_STRIPBYTECOUNT, TAGDATATYPE_LONG, 1, DataPtrFromLong(CalcImageBufferSize(currImageInfo)));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_XRESOLUTION, TAGDATATYPE_RATIONAL, 1, 0);
  tag->Data.RationalEnumerator = 720000;
  tag->Data.RationalDenominator = 10000;
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_YRESOLUTION, TAGDATATYPE_RATIONAL, 1, 0);
  tag->Data.RationalEnumerator = 720000;
  tag->Data.RationalDenominator = 10000;
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_PLANARCONFIGURATION, TAGDATATYPE_SHORT, 1, DataPtrFromShort(currImageInfo.Interlaced ? PLANARCONFIG_CONTIG : PLANARCONFIG_SEPARATE));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_RESOLUTIONUNIT, TAGDATATYPE_SHORT, 1, DataPtrFromShort(RESUNIT_INCH));
  currImageInfo.Tags.push_back(tag);
  tag = new Tag(TAG_ID_SOFTWARE, TAGDATATYPE_ASCII, Software.length() + 1, 0);
  tag->Data.StringValue = Software;
  currImageInfo.Tags.push_back(tag);
}


//***************************************************************
// SaveTag
//
// stores tag with indirect values 
// and the tags containing the strip values for later completion
//**************************************************************
void image::TTTiff::SaveTag(Tag* const tag, std::vector<Tag*>& tagsWithPointer, std::vector<Tag*>& stripOffsets, std::vector<Tag*>& stripCounts){
  
  //write tag data
  WriteU16(tag->Id);
  WriteU16(tag->Type);
  WriteU32(tag->Length);

  //store pointer to datafield for later use
  //actually its only used for indirect data and strip datas
  //but it does not hurt to set it anyway
  const std::ios::pos_type currdataptr = file->tellp();
  tag->PtrToDataPtr = currdataptr;

  //strip data (offsets and counts)
  if (tag->Id == TAG_ID_STRIPOFFSET){
    stripOffsets.push_back(tag);
    //storing dummy value
    WriteU32(0xDDDDDDDD);
  }
  else if (tag->Id == TAG_ID_STRIPBYTECOUNT){
    stripCounts.push_back(tag);
    WriteU32(tag->DataPtr);
  }
  else if (tag->WritePtr()){
    tagsWithPointer.push_back(tag);
    //storing dummy values instead of pointer
    WriteU32(0xCCCCCCCC);
  }
  else{
    //simply write the original data then
    WriteU32(tag->DataPtr);
  }

}

//**************************
// WriteIndirectTagData
//
// stores the indirect datas 
//**************************
void image::TTTiff::WriteIndirectTagData(Tag* const tag){
  switch (tag->Type){
    case TAGDATATYPE_ASCII:
      {
         const std::string s = tag->Data.StringValue;
         for (unsigned int i = 0; i < s.length(); ++i) WriteU8(s[i]);
         WriteU8(0);
      }
      break;
    case TAGDATATYPE_SHORT:
      {
         const std::vector<int> v = tag->Data.IntArray;
         for (unsigned int i = 0; i < v.size(); ++i) WriteU16(v[i]);
      }
      break;
    case TAGDATATYPE_LONG:
      {
         const std::vector<int> v = tag->Data.IntArray;
         for (unsigned int i = 0; i < v.size(); ++i) WriteU32(v[i]);
      }
      break;
    case TAGDATATYPE_RATIONAL:
      {
         WriteU32(tag->Data.RationalEnumerator);
         WriteU32(tag->Data.RationalDenominator);
      }
      break;

    default: std::cerr << "Unknown indrect data type: " << tag->Type << "! File may be corrupted then." << std::endl;
  }
}


//*****************************************
// Close
//
// closes the image and frees all resources
//*****************************************
void image::TTTiff::Close(){
  for (unsigned int i = 0; i < ImageInfos.size(); ++i) delete ImageInfos[i];
  ImageInfos.clear();
  delete[] ImageDataU8;
  ImageDataU8 = nullptr;
  delete[] ImageDataU16;
  ImageDataU16 = nullptr;
  imageData = nullptr;
  delete file;
  file = nullptr;
}

//**************
// IsMultiImage
//
// multitiff ?
//*************+
bool image::TTTiff::IsMultiImage(){
  return ImageInfos.size() > 1;
}

//**********************
// GetRed
//
// getter for pixel data 
//**********************
image::t_proc image::TTTiff::GetRed(const unsigned int x, const unsigned int y) const{
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + 0; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      return TTColorConv::ProcFromU16(ImageDataU16[index]);
    }
    else{
      return TTColorConv::ProcFromU16(SwapBytes(ImageDataU16[index]));
    }
  }
  //8 bit
  else if (ImageDataU8){
    return TTColorConv::ProcFromU8(ImageDataU8[index]);
  }
  else{
    throw TTImageException("Image Buffers not available!");
    return 0;
  }
}

//**********************
// GetRed
//
// getter for pixel data 
//**********************
image::t_proc image::TTTiff::GetGreen(const unsigned int x, const unsigned int y) const{
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + 1; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      return TTColorConv::ProcFromU16(ImageDataU16[index]);
    }
    else{
      return TTColorConv::ProcFromU16(SwapBytes(ImageDataU16[index]));
    }
  }
  //8 bit
  else if (ImageDataU8){
    return TTColorConv::ProcFromU8(ImageDataU8[index]);
  }
  else{
    throw TTImageException("Image Buffers not available!");
    return 0;
  }
}

//**********************
// GetRed
//
// getter for pixel data 
//**********************
image::t_proc image::TTTiff::GetBlue(const unsigned int x, const unsigned int y) const{
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + 2; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      return TTColorConv::ProcFromU16(ImageDataU16[index]);
    }
    else{
      return TTColorConv::ProcFromU16(SwapBytes(ImageDataU16[index]));
    }
  }
  //8 bit
  else if (ImageDataU8){
    return TTColorConv::ProcFromU8(ImageDataU8[index]);
  }
  else{
    throw TTImageException("Image Buffers not available!");
    return 0;
  }
}

//**********************
// SetGray
//
// setter for pixel data 
//**********************
image::t_proc image::TTTiff::GetGray(const unsigned int x, const unsigned int y) const{
  return TTColorConv::GetSimpleGrayByRGB(GetRed(x, y), GetGreen(x, y), GetBlue(x, y));
}

//**********************
// GetAlpha
//
// getter for pixel data 
//**********************
image::t_proc image::TTTiff::GetAlpha(const unsigned int x, const unsigned int y) const{
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  if (currImageInfo->AlphaChannels == 0) return TTColorConv::ProcMax();  //max == opaque
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + currImageInfo->Channels; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      return TTColorConv::ProcFromU16(ImageDataU16[index]);
    }
    else{
      return TTColorConv::ProcFromU16(SwapBytes(ImageDataU16[index]));
    }
  }
  //8 bit
  else if (ImageDataU8){
    return TTColorConv::ProcFromU8(ImageDataU8[index]);
  }
  else{
    throw TTImageException("Image Buffers not available!");
    return 0;
  }
}


//**********************
// SetZero
//
// set all to 0 (black)
//**********************
void image::TTTiff::SetZero(unsigned int x, unsigned int y){
  SetRed(x, y, 0);
  SetGreen(x, y, 0);
  SetBlue(x, y, 0);
}

//**********************
// SetRed
//
// setter for pixel data 
//**********************
void image::TTTiff::SetRed(const unsigned int x, const unsigned int y, const t_proc r){
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + 0; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      ImageDataU16[index] = TTColorConv::U16FromProc(r);
    }
    else{
      ImageDataU16[index] = SwapBytes(TTColorConv::U16FromProc(r));
    }
  }
  //8 bit
  else if (ImageDataU8){
    ImageDataU8[index] = TTColorConv::U8FromProc(r);
  }
}

//**********************
// SetGreen
//
// setter for pixel data 
//**********************
void image::TTTiff::SetGreen(const unsigned int x, const unsigned int y, const t_proc g){
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + 1; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      ImageDataU16[index] = TTColorConv::U16FromProc(g);
    }
    else{
      ImageDataU16[index] = SwapBytes(TTColorConv::U16FromProc(g));
    }
  }
  //8 bit
  else if (ImageDataU8){
    ImageDataU8[index] = TTColorConv::U8FromProc(g);
  }
}

//**********************
// SetBlue
//
// setter for pixel data 
//**********************
void image::TTTiff::SetBlue(const unsigned int x, const unsigned int y, const t_proc b){
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + 2; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      ImageDataU16[index] = TTColorConv::U16FromProc(b);
    }
    else{
      ImageDataU16[index] = SwapBytes(TTColorConv::U16FromProc(b));
    }
  }
  //8 bit
  else if (ImageDataU8){
    ImageDataU8[index] = TTColorConv::U8FromProc(b);
  }
}

//**********************
// SetGray
//
// setter for pixel data 
//**********************
void image::TTTiff::SetGray(const unsigned int x, const unsigned int y, const t_proc g){
  //we have only RGB images at the moment
  SetRed(x, y, g);
  SetGreen(x, y, g);
  SetBlue(x, y, g);
}

//**********************
// SetAlpha
//
// setter for pixel data 
//**********************
void image::TTTiff::SetAlpha(const unsigned int x, const unsigned int y, const t_proc a){
  const ImageInfoTiff* const currImageInfo = ImageInfos[0];
  if (currImageInfo->AlphaChannels == 0) return;
  const unsigned int channels = currImageInfo->Channels + currImageInfo->AlphaChannels;
  const bool interlaced = currImageInfo->Interlaced;
  const unsigned int index = (x + y *  currImageInfo->Width) * channels + currImageInfo->Channels; //only interlaced!
  //16 bit
  if (ImageDataU16){
    if (ByteOrderIntel){
      ImageDataU16[index] = TTColorConv::U16FromProc(a);
    }
    else{
      ImageDataU16[index] = SwapBytes(TTColorConv::U16FromProc(a));
    }
  }
  //8 bit
  else if (ImageDataU8){
    ImageDataU8[index] = TTColorConv::U8FromProc(a);
  }
}

//**************************************
// SetPixelU8Direct
//
// direct data access to the 8bit values
//**************************************
/*void image::TTTiff::SetPixelU8Direct(unsigned int x, unsigned int y, const U8 r, const U8 g, const U8 b){
  
}*/

//**************************************
// SetPixelU16Direct
//
// direct data access to the 8bit values
//**************************************
/*void image::TTTiff::SetPixelU16Direct(unsigned int x, unsigned int y, const U16 r, const U16 g, const U16 b){

}*/

//****************************
// AllocateMemory
//
// allocates the needed memory
//****************************
bool image::TTTiff::AllocateMemory(const ImageInfoTiff& currImageInfo){
  const unsigned int allchannels = currImageInfo.Channels + currImageInfo.AlphaChannels;
  const unsigned int size = currImageInfo.Width * currImageInfo.Height * allchannels;
  if (currImageInfo.BitsPerSample == 8){
    ImageDataU8 = new U8[size];
    ImageDataU16 = nullptr;
    imageData = static_cast<void*>(ImageDataU8);
    return true;
  }
  else if (currImageInfo.BitsPerSample == 16){
    ImageDataU8 = nullptr;
    ImageDataU16 = new U16[size];
    imageData = static_cast<void*>(ImageDataU16);
    return true;
  }
  else{
    ImageDataU8 = nullptr;
    ImageDataU16 = nullptr;
    imageData = nullptr;
    throw TTImageException("Unknown data size (allowed only 8 or 16bit)!");
    return false;
  }
}

//********************
// CalcImageBufferSize
//********************
unsigned int image::TTTiff::CalcImageBufferSize(const ImageInfoTiff& currImageInfo){
  const unsigned int allchannels = currImageInfo.Channels + currImageInfo.AlphaChannels;
  const unsigned int size = currImageInfo.Width * currImageInfo.Height * allchannels;
  if (currImageInfo.BitsPerSample == 8){
    return sizeof(U8) * size;
  }
  else if (currImageInfo.BitsPerSample == 16){
    return sizeof(U16) * size;
  }
  else{
    throw TTImageException("Unknown data size (allowed only 8 or 16bit)!");
    return 0;
  }
}



//****************
// GetImageInfos
//
// for information
//****************
std::string image::TTTiff::GetImageInfos(){
  std::string s;
  s += "TTTiff: " + ttutil::StringUtil::uint2string(Width()) + " x " + ttutil::StringUtil::uint2string(Height()) + "\n";
  s += "color channels:  " + ttutil::StringUtil::uint2string(ColorChannels()) + "\n";
  s += "alpha channels:  " + ttutil::StringUtil::uint2string(AlphaChannels()) + "\n";
  s += "color type: " + GetColormodeTypeName(Colormode()) + "\n";
  s += "date type: " + GetDatatypName(Datatype()) + "\n";
  return s;
}