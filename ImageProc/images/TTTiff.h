#ifndef _TTTIFF_H_
#define _TTTIFF_H_

#include <iostream>
#include <fstream>
#include <vector>

#include "TTImageFiles.h"
#include "stringutil.h"

namespace image{


//******************************************************************************
//*                                  TTTiff                                    *
//*                                                                            *
//*                          class for tiff handling                           *
//******************************************************************************


//***********************************************************************
//*                               Tag                                   *
//* PRIVATELY USED CLASS                                                *
//* this is a struct to strore TIFF-Tag data and some temporare runtime *
//* informations to read and write the Tags                             *
//***********************************************************************
enum TagDataType {
  TAGDATATYPE_UNKNOWN = 0,
  TAGDATATYPE_BYTE = 1,
  TAGDATATYPE_ASCII = 2,
  TAGDATATYPE_SHORT = 3,
  TAGDATATYPE_LONG = 4,
  TAGDATATYPE_RATIONAL = 5,
  TAGDATATYPE_SBYTE = 6,
  TAGDATATYPE_UNDEFINED = 7,
  TAGDATATYPE_SSHORT = 8,
  TAGDATATYPE_SLONG = 9,
  TAGDATATYPE_SRATIONAL = 10,
  TAGDATATYPE_FLOAT = 11,
  TAGDATATYPE_DOUBLE = 12,
  TAGDATATYPE_IFD = 13,
};

enum TagDataContainerType { TAGDATACONTAINERTYPE_SINGLE, TAGDATACONTAINERTYPE_ARRAY };

enum TiffCompression { COMPRESSION_NONE = 1, COMPRESSION_LZW = 5 };

enum TiffPhotometricInterpretation {
  PHOTOMETRIC_MINISWHITE = 0,
  PHOTOMETRIC_MINISBLACK = 1,
  PHOTOMETRIC_RGB = 2,
  PHOTOMETRIC_PALETTE = 3,
  PHOTOMETRIC_MASK = 4,
  PHOTOMETRIC_SEPARATED = 5,
  PHOTOMETRIC_YCBCR = 6,
  PHOTOMETRIC_CIELAB = 8,
  PHOTOMETRIC_ICCLAB = 9,
  PHOTOMETRIC_ITULAB = 10,
  PHOTOMETRIC_LOGL = 32844,
  PHOTOMETRIC_LOGLUV = 3284
};

enum TiffOrientation {
  ORIENTATION_TOPLEFT = 1,
  ORIENTATION_TOPRIGHT = 2,
  ORIENTATION_BOTRIGHT = 3,
  ORIENTATION_BOTLEFT = 4,
  ORIENTATION_LEFTTOP = 5,
  ORIENTATION_RIGHTTOP = 6,
  ORIENTATION_RIGHTBOT = 7,
  ORIENTATION_LEFTBOT = 8
};

enum TiffPlanarConfiguration{ PLANARCONFIG_CONTIG = 1, PLANARCONFIG_SEPARATE = 2 };

enum TiffResolutionUnit{ RESUNIT_NONE = 1, RESUNIT_INCH = 2, RESUNIT_CENTIMETER = 3};

//tag Ids
const U16 TAG_ID_NEWSUBFILETYPE            = 254;
const U16 TAG_ID_SUBFILETYPE               = 255;
const U16 TAG_ID_IMAGEWIDTH                = 256;
const U16 TAG_ID_IMAGELENGTH               = 257;
const U16 TAG_ID_BITSPERSAMPLE             = 258;
const U16 TAG_ID_COMPRESSION               = 259;
const U16 TAG_ID_PHOTOMETRICINTERPRETATION = 262;
const U16 TAG_ID_STRIPOFFSET               = 273;
const U16 TAG_ID_ORIENTATION               = 274;
const U16 TAG_ID_SAMPLESPERPIXEL           = 277;
const U16 TAG_ID_ROWSPERSTRIP              = 278;
const U16 TAG_ID_STRIPBYTECOUNT            = 279;
const U16 TAG_ID_XRESOLUTION               = 282;
const U16 TAG_ID_YRESOLUTION               = 283;
const U16 TAG_ID_PLANARCONFIGURATION       = 284;
const U16 TAG_ID_RESOLUTIONUNIT            = 296;
const U16 TAG_ID_SOFTWARE                  = 305;

//TagValue storage class.
//could be a union. But I use a class for further possibilities
class TagValue{
  public:
    char ByteValue;
    std::string StringValue;
    short ShortValue;
    int LongValue;
    int RationalEnumerator;
    int RationalDenominator;
    std::vector<int> IntArray;
};



class Tag{

  public:

    //constructor 
    Tag(const U16 id, const TagDataType type, const U32 length, const U32 dataPtr);

    //returns sizeof(datatype) in bytes
    unsigned int SizeofType() const;

    // write pointer or direct data ?
    bool WritePtr() const;

    //method for printing
    std::string ToString() const;

    //------------------- fields -----------------

    //the logical type(id) of the tag
    U16 Id;

    //data type
    TagDataType Type;

    //data container type
    TagDataContainerType ContainerType;

    //data length
    U32 Length;

    //the extracted data field
    TagValue Data;

    //original data or pointer to them (as stored in tag table)
    U32 DataPtr;

    /**
    * temporäre Variable für das Schreiben,
    * Pointer auf die Daten
    */
    //public long   ptr = 0;

    //temporare variable for writing
    //stores the position of the data pointer
    //for later completion of indirect values
    std::ios::pos_type PtrToDataPtr;

    /** wird gebraucht um Stripdata Tags zu schreiben */
    //public long   stripPtrIn = 0;

    //used to write stripdata tags
    //std::ios::pos_type StripPtrOut;

};

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
class ImageInfoTiff{

  public:

    //constructor
    ImageInfoTiff();

    //destructor 
    ~ImageInfoTiff();

    //image Width
    unsigned int Width = 0;

    //image height
    unsigned int  Height = 0;

    //number of color channels
    unsigned short Channels = 0;

    //number of alpha and mask channels
    unsigned short AlphaChannels = 0;

    //base color mode
    ColormodeType BaseColorMode = UNKNOWN_COLORMODE;

    //number of (maximum) bits per color channel
    unsigned int BitsPerSample = 0;

    //interlaced or not
    bool Interlaced = true; //interlaced is more or less standard; 

    /** unused at the moment */
    TiffCompression Compression = COMPRESSION_NONE;

    /** unused at the moment */
    TiffPhotometricInterpretation Photometric = PHOTOMETRIC_RGB;

    /** unused at the moment */
    TiffOrientation Orientation = ORIENTATION_TOPLEFT;

    //NOTE: StripsPerImage = floor ((ImageLength + RowsPerStrip - 1) / RowsPerStrip). 

    //numbers of rows per strips
    unsigned short RowsPerStrip = 0;

    //byte offsets to the strip
    std::vector<unsigned int> StripOffsets;

    //sizes of strips
    std::vector<unsigned int> StripByteCounts;

    //Tags
    std::vector<Tag*> Tags;

};

//***********************************************************************
//*                               Header                                *
//***********************************************************************
const U16 TiffMagicII = 0x4949;   //Windows (Intel)
const U16 TiffMagicMM = 0x4D4D;

typedef struct{
  U16 MagicNumber;
  U16 Version;        //always 42(decimal)
  U32 IFDOffset;      //offset to first IFD
} TIFFHEADER;

#pragma warning( disable : 4250 )
class TTTiff : public virtual TTMemoryImage, public virtual TTByteOrderFile {

  public:

    //constructor
    TTTiff();

    //destructor
    virtual ~TTTiff();


    //----------------- methods from TTFileImage -----------

    //opens a image from file
    virtual bool Open(const std::string& filename);

    //opens a image from file but does not load image data
    virtual bool OpenFileInfo(const std::string& filename);

    //saves the image
    virtual bool Save(const std::string& filename);

    //closes the image and frees all resources
    virtual void Close();

    //resets the filepointer
    virtual bool ResetFilePointer();

    //--------------- methods from TTMemoryImage -----------

    //------------------ methods from TTImage --------------

    //creates the image in memory
    virtual bool Create(unsigned int w, unsigned int h);

    //creates the image in memory
    //with given properties
    virtual bool Create(unsigned int w, unsigned int h, const ColormodeType colormode, const Datatyp datatype);

    //returns the width
    virtual unsigned int Width() const;

    //returns the height
    virtual unsigned int Height() const;

    //returns the number of whole channels (== color channel + alpha channel)
    virtual unsigned int WholeChannels() const;

    //returns the number of color channels
    virtual unsigned int ColorChannels() const;

    //returns the number of alpha channels
    virtual unsigned int AlphaChannels() const;

    //return the colormode
    virtual ColormodeType Colormode() const;

    //return the ColorSpace
    virtual ColorSpaceType ColorSpace() const;

    //return the colormodebit depth per channel
    virtual Datatyp Datatype() const;

    //return interlaced or not 
    virtual bool Interlaced() const;

    //getter for pixel data
    virtual t_proc GetRed(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetGreen(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetBlue(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetGray(const unsigned int x, const unsigned int y) const;
    virtual t_proc GetAlpha(const unsigned int x, const unsigned int y) const;

    //setter for pixel data
    virtual void SetZero(unsigned int x, unsigned int y);

    virtual void SetRed(const unsigned int x, const unsigned int y, const t_proc r);
    virtual void SetGreen(const unsigned int x, const unsigned int y, const t_proc g);
    virtual void SetBlue(const unsigned int x, const unsigned int y, const t_proc b);
    virtual void SetGray(const unsigned int x, const unsigned int y, const t_proc g);
    virtual void SetAlpha(const unsigned int x, const unsigned int y, const t_proc a);

    //------------------------------- tiff methods ----------------------------------

    //void SetPixelU8Direct(unsigned int x, unsigned int y, const U8 r, const U8 g, const U8 b);
    //void SetPixelU16Direct(unsigned int x, unsigned int y, const U16 r, const U16 g, const U16 b);

    // multitiff ?
    bool IsMultiImage();

    //for information
    std::string GetImageInfos();

  private:

    //opens a image from file
    bool OpenInternal(const std::string& filename, const bool readImageData);

    //methods for reading / opening
    bool ReadTIFFHeader();
    bool ReadTagTable();
    void ReadTag();
    void FillTagData(Tag& tag);

    //handles byte padding
    unsigned char ByteFromDataPtr(const U32 data);
    unsigned short ShortFromDataPtr(const U32 data);
    unsigned long LongFromDataPtr(const U32 data);
    U32 DataPtrFromByte(unsigned char v);
    U32 DataPtrFromShort(unsigned short v);
    U32 DataPtrFromLong(unsigned long v);

    bool TagAlreadyExists(const U16 tagid);

    // reads the image vales from the tags
    // at the moment only from the first ImageInfo
    bool ExtractImageValuesFromTags();

    // reads the image data into the internal buffer
    void ReadImageDatas();

    // allocates the needed memory
    bool AllocateMemory(const ImageInfoTiff& currImageInfo);
    unsigned int CalcImageBufferSize(const ImageInfoTiff& currImageInfo);

    //methods for writing
    void CreateTagsForWriting(ImageInfoTiff& currImageInfo);
    void SaveTag(Tag* const tag, std::vector<Tag*>& tagsWithPointer, std::vector<Tag*>& stripOffsets, std::vector<Tag*>& stripCounts);
    void WriteIndirectTagData(Tag* const tag);

    //private fields 
    TIFFHEADER Header;
    std::vector<ImageInfoTiff*> ImageInfos;
    int ToNextIFD = 0;

    //image data
    //(should we pack this in imageInfo? But what is then with global values?)
    U8* ImageDataU8 = nullptr;
    U16* ImageDataU16 = nullptr;

  };

} //end of namespace images
#endif
