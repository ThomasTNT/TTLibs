#ifndef IMAGE_INFO_STRCUTS_H
#define IMAGE_INFO_STRCUTS_H

#include "TTColor.h"

namespace image{

class ImageTileInfos;

//************************************************************************
//*                                                                      *
//*                         ImageProperties                              *
//*                                                                      *
//*                      global image properties                         *
//*                                                                      *
//************************************************************************
class ImageProperties{

  public:

    //default constructor
    ImageProperties();

    //constructor
    ImageProperties(const unsigned int width,
                    const int height,
                    const ColormodeType colorMode,
                    const Datatyp dataType,
                    const unsigned int colorChannels,
                    const unsigned int alphaChannels);

    //getter
    unsigned int Width() const;
    unsigned int Height() const;
    ColormodeType ColorMode() const;
    Datatyp DataType() const;
    unsigned int ColorChannels() const;
    unsigned int AlphaChannels() const;
    unsigned int WholeChannels() const;

    //informations
    bool IsInterlaced() const;
    ColormodeType SimpleColorMode() const;

    //builds tile info where te whole image is one tile
    ImageTileInfos GetTileInfosWholeImage() const;

  private:
    unsigned int width = 0;
    unsigned int height = 0;
    ColormodeType colorMode = RGB;
    Datatyp dataType = INT16;
    unsigned int colorChannels = 3;
    unsigned int alphaChannels = 0;

     
};


//************************************************************************
//*                                                                      *
//*                          ImageTileInfos                              *
//*                                                                      *
//*           information about the current tile of an image             *
//*           (can be of course also the whole image)
//*                                                                      *
//************************************************************************
class ImageTileInfos{

  public:

    //default constructor
    ImageTileInfos();

    //constructor
    ImageTileInfos(const unsigned int wholeImageWidth,
                   const unsigned int wholeImageHeight,
                   const unsigned int tileWidth,
                   const unsigned int tileHeight,
                   const int          posInImageX,
                   const int          posInImageY,
                   const unsigned int noOfTilesHorizontal,
                   const unsigned int noOfTilesVertical,
                   const unsigned int tileNumberH,
                   const unsigned int tileNumberV,
                   const unsigned int tileCorridorLow, 
                   const unsigned int tileCorridorHigh,
                   const unsigned int rowbytes,
                   const unsigned int planebytes
                   );

    //getter
    unsigned int WholeImageWidth() const;
    unsigned int WholeImageHeight() const;
    unsigned int TileWidth() const;
    unsigned int TileHeight() const;
    int          PosInImageX() const;
    int          PosInImageY() const;
    unsigned int NoOfTilesHorizontal() const;
    unsigned int NoOfTilesVertical() const;
    unsigned int TileNumberH() const;
    unsigned int TileNumberV() const;
    unsigned int TileCorridorLow() const;
    unsigned int TileCorridorHigh() const;
    unsigned int Rowbytes() const;
    unsigned int Planebytes() const;

    bool Valid() const;

    //calculations and stuff
    unsigned int TileNumber() const;

    //calculate rowOffset in pointer indizes depending on datatype instead of bytes
    unsigned int RowOffset(const Datatyp imgDatatype) const;

    //calculate plane size in pointer indizes depending on datatype instead of bytes
    unsigned int PlaneOffset(const Datatyp imgDatatype) const;

  private:

    unsigned int wholeImageWidth = 0;
    unsigned int wholeImageHeight = 0;
    unsigned int tileWidth = 0;
    unsigned int tileHeight = 0;
    int          posInImageX = 0;
    int          posInImageY = 0;
    unsigned int noOfTilesHorizontal = 0;
    unsigned int noOfTilesVertical = 0;
    unsigned int tileNumberH = 0;
    unsigned int tileNumberV = 0;
    unsigned int tileCorridorLow = 0;  //lowest y coordinate in this tile row
    unsigned int tileCorridorHigh = 0; //highest y coordinate in this tile row

    unsigned int rowbytes = 0;      //offset between two lines in bytes (there may be pad bytes...)
    unsigned int planebytes = 0;    //offset to the next channel on noninterlaced input image data in bytes
    
};

} //end of namespace image

#endif
