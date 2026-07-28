#include "ImageInfoStructs.h"

//************************************************************************
//*                                                                      *
//*                         ImageProperties                              *
//*                                                                      *
//*                      global image properties                         *
//*                                                                      *
//************************************************************************

//********************
// default constructor
//********************
image::ImageProperties::ImageProperties(){}


//************
// constructor
//************
image::ImageProperties::ImageProperties(const unsigned int width,
                                        const int height,
                                        const ColormodeType colorMode,
                                        const Datatyp dataType,
                                        const unsigned int colorChannels,
                                        const unsigned int alphaChannels)
                                        :width(width), height(height), colorMode(colorMode), dataType(dataType), 
                                         colorChannels(colorChannels), alphaChannels(alphaChannels)
{

}

//********
// getter
//********
unsigned int image::ImageProperties::Width() const { return width; }

//********
// getter
//********
unsigned int image::ImageProperties::Height() const { return height; }

//********
// getter
//********
image::ColormodeType image::ImageProperties::ColorMode() const { return colorMode; }

//********
// getter
//********
image::Datatyp image::ImageProperties::DataType() const { return dataType; }

//********
// getter
//********
unsigned int image::ImageProperties::ColorChannels() const { return colorChannels; }

//********
// getter
//********
unsigned int image::ImageProperties::AlphaChannels() const { return alphaChannels; }

//********
// getter
//********
unsigned int image::ImageProperties::WholeChannels() const { return colorChannels + alphaChannels; }

//*************
// IsInterlaced
//*************
bool image::ImageProperties::IsInterlaced() const{
  return image::GetInterlacedByColormodeType(colorMode);
}

//****************
// SimpleColorMode
//****************
image::ColormodeType image::ImageProperties::SimpleColorMode() const{
  return image::GetSimpleColormodeType(colorMode);
}

//**************************************************
// GetTileInfosWholeImage
//
// builds tile info where te whole image is one tile
//**************************************************
image::ImageTileInfos image::ImageProperties::GetTileInfosWholeImage() const{
  //rowbytes are the length of a row in bytes
  unsigned int rowbytes;
  unsigned int planebytes;
  if (IsInterlaced()){
    rowbytes = WholeChannels() * GetBytesByDatatyp(dataType) * width;
    planebytes = GetBytesByDatatyp(dataType); //does not really make sense, because next plane is interlaced 
  }
  else{
    rowbytes = GetBytesByDatatyp(dataType) * width;
    planebytes = GetBytesByDatatyp(dataType) * width * height;
  }
  
  return ImageTileInfos(width,
                        height,
                        width,
                        height,
                        0,
                        0,
                        1,
                        1,
                        0,
                        0,
                        0,
                        height - 1,
                        rowbytes,
                        planebytes
                       ); 
}


//************************************************************************
//*                                                                      *
//*                          ImageTileInfos                              *
//*                                                                      *
//*           information about the current tile of an image             *
//*           (can be of course also the whole image)
//*                                                                      *
//************************************************************************


//********************
// default constructor
//********************
image::ImageTileInfos::ImageTileInfos(){}


//************
// constructor
//************  
image::ImageTileInfos::ImageTileInfos(const unsigned int wholeImageWidth,
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
                                      )
                      :wholeImageWidth(wholeImageWidth), wholeImageHeight(wholeImageHeight), 
                       tileWidth(tileWidth), tileHeight(tileHeight), 
                       posInImageX(posInImageX), posInImageY(posInImageY),
                       noOfTilesHorizontal(noOfTilesHorizontal), noOfTilesVertical(noOfTilesVertical),
                       tileNumberH(tileNumberH), tileNumberV(tileNumberV),
                       tileCorridorLow(tileCorridorLow), tileCorridorHigh(tileCorridorHigh),
                       rowbytes(rowbytes), planebytes(planebytes)
{

}

//********
// getter
//********
unsigned int image::ImageTileInfos::WholeImageWidth() const { return wholeImageWidth; }

//********
// getter
//********
unsigned int image::ImageTileInfos::WholeImageHeight() const { return wholeImageHeight; }

//********
// getter
//********
unsigned int image::ImageTileInfos::TileWidth() const { return tileWidth; }

//********
// getter
//********
unsigned int image::ImageTileInfos::TileHeight() const { return tileHeight; }

//********
// getter
//********
int image::ImageTileInfos::PosInImageX() const { return posInImageX; }

//********
// getter
//********
int image::ImageTileInfos::PosInImageY() const { return posInImageY; }

//********
// getter
//********
unsigned int image::ImageTileInfos::NoOfTilesHorizontal() const { return noOfTilesHorizontal; }

//********
// getter
//********
unsigned int image::ImageTileInfos::NoOfTilesVertical() const { return noOfTilesVertical; }

//********
// getter
//********
unsigned int image::ImageTileInfos::TileNumberH() const { return tileNumberH; }

//********
// getter
//********
unsigned int image::ImageTileInfos::TileNumberV() const { return tileNumberV; }

//********
// getter
//********
unsigned int image::ImageTileInfos::Rowbytes() const { return rowbytes; }

//********
// getter
//********
unsigned int image::ImageTileInfos::Planebytes() const { return planebytes; }

//********
// Valid
//********
bool image::ImageTileInfos::Valid() const{
  return tileWidth > 0 && tileHeight > 0;
}

//***********
// TileNumber
//***********
unsigned int image::ImageTileInfos::TileNumber() const{
  return tileNumberV * noOfTilesHorizontal + tileNumberH;
}

//******************************************************************************
// RowOffset
//
// calculate rowOffset in pointer indizes depending on datatype instead of bytes
//******************************************************************************
unsigned int image::ImageTileInfos::RowOffset(const Datatyp imgDatatype) const {
  unsigned int rowOffset = rowbytes;
  if (imgDatatype == image::INT16) rowOffset /= 2;
  else if (imgDatatype == image::INT32 || imgDatatype == image::FLOAT32) rowOffset /= 4;
  return rowOffset;
}

//*******************************************************************************
// RowOffset
// 
// calculate plane size in pointer indizes depending on datatype instead of bytes
//*******************************************************************************
unsigned int image::ImageTileInfos::PlaneOffset(const Datatyp imgDatatype) const {
  unsigned int planeSize = planebytes;
  if (imgDatatype == image::INT16) planeSize /= 2;
  else if (imgDatatype == image::INT32 || imgDatatype == image::FLOAT32) planeSize /= 4;
  return planeSize;
}

