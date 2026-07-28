#include "TTImage.h"


//***********************
// GetTTFileImageByEnding
//***********************
std::unique_ptr<image::TTFileImage> image::GetTTFileImageByEnding(const std::string& filename){
  const std::string ENDING_TIF = ".tif";
  const std::string ENDING_BMP = ".bmp";

  std::string filenameLower = filename;
  std::transform(filenameLower.begin(), filenameLower.end(), filenameLower.begin(), ::tolower);

  //checks if the lowercase filename ends with the given ending
  const auto hasEnding = [&filenameLower](const std::string& ending){
    return filenameLower.length() >= ending.length()
        && filenameLower.compare(filenameLower.length() - ending.length(), ending.length(), ending) == 0;
  };

  if (hasEnding(ENDING_TIF)){
    return std::make_unique<image::TTTiff>();
  }
  else if (hasEnding(ENDING_BMP)){
    return std::make_unique<image::TTBitmap>();
  }

  return nullptr;
}



//**********
// SaveImage
//**********
void image::SaveImage(const image::TTImage& image, const std::string& filename) {

  std::unique_ptr<image::TTFileImage> outImage = image::GetTTFileImageByEnding(filename);
  const image::Datatyp dataType = image.Datatype();
  outImage->Create(image.Width(), image.Height(), image.Colormode(), dataType == image::FLOAT32 ? image::INT8 : dataType);
  outImage->CopyImage(image);
  outImage->Save(filename);

  std::cout << "Wrote " << filename << std::endl;
}

