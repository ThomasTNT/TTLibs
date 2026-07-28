#pragma once

#include <algorithm>

#include "ImageInfoStructs.h"
#include "TTImageFiles.h"
#include "TTSimpleMemoryImage.h"
#include "TTBitmap.h"
#include "TTTiff.h"

namespace image{


//***********************
// GetTTFileImageByEnding
//***********************
std::unique_ptr<image::TTFileImage> GetTTFileImageByEnding(const std::string& filename);


//**********
// SaveImage
//**********
void SaveImage(const image::TTImage& image, const std::string& filename);


}

