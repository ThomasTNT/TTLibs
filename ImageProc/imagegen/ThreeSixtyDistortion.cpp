#include "ThreeSixtyDistortion.h"


//*****************************************************************************
//*                         ThreeSixtyDistortion                              *
//*                                                                           *
//*              transform an rectangular image into 360° space               *
//*****************************************************************************
 
//************
// constructor
//************
imagegen::ThreeSixtyDistortion::ThreeSixtyDistortion(){}

//**************************************************
// createDistortedImagePlane
//
// x axis is front - back pointing to the center
// y axis is left - right
// z axis is down - up 
//
// lambda rotates left right around the z axis (pan)
// phi rotates up down (tilt)
//
//
//***************************************************
std::unique_ptr<const image::TTImage> imagegen::ThreeSixtyDistortion::CreateDistortedImagePlane(const image::TTImage& inImage,
                                                                                                const unsigned int widthResult,
                                                                                                const unsigned int heightResult,
                                                                                                const float anglePan,
                                                                                                const float angleTilt,
                                                                                                const float viewAngleHor)
{

  //create interpolators
  //including alpha channel
  const unsigned int widthIn = inImage.Width();
  const unsigned int heightIn = inImage.Height();
  const size_t bufferSize = widthIn * heightIn;
  
  const std::unique_ptr<image::t_proc[]> inBufferR = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferG = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferB = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferA = std::make_unique<image::t_proc[]>(bufferSize);

  for (unsigned int yg = 0; yg < heightIn; yg++) {
    for (unsigned int xg = 0; xg < widthIn; xg++) {
      inBufferR[xg + yg * widthIn] = inImage.GetRed(xg, yg);
      inBufferG[xg + yg * widthIn] = inImage.GetGreen(xg, yg);
      inBufferB[xg + yg * widthIn] = inImage.GetBlue(xg, yg);
      inBufferA[xg + yg * widthIn] = inImage.AlphaChannels() > 0 ? inImage.GetAlpha(xg, yg) : 1.0f;
    }
  }
    
  imgproc::InterpolatorFloat interpolator(widthIn, heightIn, imgproc::BICUBIC);

  //create result
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(widthResult, heightResult, image::RGBA, image::FLOAT32);
  resultImg->SetImageZero();

  
  const float centralLambda0 = 0.0f;
  const float centralPhi0 = 0.0f;
  const float angleImageXdeg = viewAngleHor;
  const float angleImageYdeg = angleImageXdeg * static_cast<float>(heightIn) / static_cast<float>(widthIn);

  const float lambda0 = cgi::DegToRad(anglePan);
  const float phi0 = cgi::DegToRad(angleTilt);
  const float angleImageX = cgi::DegToRad(angleImageXdeg);
  const float angleImageY = cgi::DegToRad(angleImageYdeg);
  const float maxProjX = std::tan(angleImageX / 2.0f);
  const float maxProjY = std::tan(angleImageY / 2.0f);


  const mathutil::Matrix<float> worldToCamera = RotMatrixZ(-lambda0) * RotMatrixY(-phi0);
  
  std::cout << "worldToCamera: " << std::endl << worldToCamera.ToString() << std::endl;

  //loop through target pixel
  for (unsigned yout = 0; yout < heightResult; ++yout) {
    if (yout % 100 == 0) std::cout << "processing line " << yout << std::endl;
    for (unsigned xout = 0; xout < widthResult; ++xout) {

      const float lambda = (static_cast<float>(xout) / (widthResult - 1.0f)) * (2.0f * cgi::PI) - cgi::PI;
      const float phi = cgi::PI / 2.0f - (static_cast<float>(yout) / (heightResult - 1.0f)) * cgi::PI;

      const float xSphere = std::cos(phi) * std::cos(lambda);
      const float ySphere = std::cos(phi) * std::sin(lambda);
      const float zSphere = std::sin(phi);

      const mathutil::Vector<float> camPoint = mathutil::Vector<float>({xSphere, ySphere, zSphere }) * worldToCamera;

      if (yout == 960 && xout == 1920) {
        std::cout << "center!" << std::endl;
        std::cout << "camPoint: " << std::endl << camPoint.ToString() << std::endl;
      }

      if (camPoint.GetX() <= 0.0001f) {
        resultImg->SetRed(xout, yout, 0.0f);
        resultImg->SetGreen(xout, yout, 0.0f);
        resultImg->SetBlue(xout, yout, 0.0f);
        resultImg->SetAlpha(xout, yout, 0.0f);
        continue;
      }

      const float projectNormalizedX = camPoint.GetY() / camPoint.GetX();
      const float projectNormalizedY = camPoint.GetZ() / camPoint.GetX();

      const float xin = (projectNormalizedX / (2.0f * maxProjX) + 0.5f) * widthIn;
      const float yin = (-projectNormalizedY / (2.0f * maxProjY) + 0.5f) * heightIn; 

      resultImg->SetRed(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferR.get()));
      resultImg->SetGreen(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferG.get()));
      resultImg->SetBlue(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferB.get()));
      resultImg->SetAlpha(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferA.get()));

    }
  }

  return resultImg;
}

//**************************************************
// CreateDistortedImageUpright
//
// x axis is front - back pointing to the center
// y axis is left - right
// z axis is down - up 
//
// lambda rotates left right around the z axis (pan)
// phi rotates up down (tilt)
//
//
//***************************************************
std::unique_ptr<const image::TTImage> imagegen::ThreeSixtyDistortion::CreateDistortedImageUpright(const image::TTImage& inImage,
                                                                                                  const unsigned int widthResult,
                                                                                                  const unsigned int heightResult,
                                                                                                  const float anglePan,
                                                                                                  const float angleTilt,
                                                                                                  const float viewAngleHor)
{

  //create interpolators
  //including alpha channel
  const unsigned int widthIn = inImage.Width();
  const unsigned int heightIn = inImage.Height();
  const size_t bufferSize = widthIn * heightIn;

  const std::unique_ptr<image::t_proc[]> inBufferR = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferG = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferB = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferA = std::make_unique<image::t_proc[]>(bufferSize);

  for (unsigned int yg = 0; yg < heightIn; yg++) {
    for (unsigned int xg = 0; xg < widthIn; xg++) {
      inBufferR[xg + yg * widthIn] = inImage.GetRed(xg, yg);
      inBufferG[xg + yg * widthIn] = inImage.GetGreen(xg, yg);
      inBufferB[xg + yg * widthIn] = inImage.GetBlue(xg, yg);
      inBufferA[xg + yg * widthIn] = inImage.AlphaChannels() > 0 ? inImage.GetAlpha(xg, yg) : 1.0f;
    }
  }


  //create result
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(widthResult, heightResult, image::RGBA, image::FLOAT32);
  resultImg->SetImageZero();

  //convert deg -> rad
  const float lambda0 = cgi::DegToRad(anglePan);
  const float phi0 = cgi::DegToRad(angleTilt);
  const float viewAngleIdeal = cgi::DegToRad(viewAngleHor);

  const float imageWidthRelative = 1.0f;
  const float imageHeightRelative = imageWidthRelative * static_cast<float>(heightIn) / static_cast<float>(widthIn);
  const float distToImagePlane = (imageWidthRelative / 2.0f) / std::tan(viewAngleIdeal / 2.0f);
  const float offsetImageY = distToImagePlane * std::tan(lambda0);
  const float offsetImageZ = distToImagePlane * std::tan(phi0);

  const mathutil::Vector<float> imageCenterWorld({ distToImagePlane, offsetImageY, offsetImageZ });
  const mathutil::Vector<float> imageNormalWorld({ -1.0f, 0.0f, 0.0f });
  const mathutil::Vector<float> imageUpWorld({ 0.0f, 0.0f, 1.0f });
  const mathutil::Vector<float> imageRightWorld = (imageNormalWorld % imageUpWorld).Normalize();
  
  //loop through target pixel
  imgproc::InterpolatorFloat interpolator(widthIn, heightIn, imgproc::BILINEAR);

  for (unsigned yout = 0; yout < heightResult; ++yout) {
    if (yout % 100 == 0) std::cout << "processing line " << yout << std::endl;
    for (unsigned xout = 0; xout < widthResult; ++xout) {

      //get direction of ray
      const float lambda = (static_cast<float>(xout) / (widthResult - 1.0f)) * (2.0f * cgi::PI) - cgi::PI;
      const float phi = cgi::PI / 2.0f - (static_cast<float>(yout) / (heightResult - 1.0f)) * cgi::PI;

      const mathutil::Vector<float> rayDirection = mathutil::Vector<float>({ std::cos(phi) * std::cos(lambda),
                                                                             std::cos(phi) * std::sin(lambda),
                                                                             std::sin(phi) }).Normalize();

      const float denominator = rayDirection * imageNormalWorld;
      if (denominator >= -0.0001f) {
        resultImg->SetRed(xout, yout, 0.0f);
        resultImg->SetGreen(xout, yout, 0.0f);
        resultImg->SetBlue(xout, yout, 0.0f);
        resultImg->SetAlpha(xout, yout, 0.0f);
        continue;
      }

      const float t = (imageCenterWorld * imageNormalWorld) / denominator;
      if (t <= 0.0001f) {
        resultImg->SetRed(xout, yout, 0.0f);
        resultImg->SetGreen(xout, yout, 0.0f);
        resultImg->SetBlue(xout, yout, 0.0f);
        resultImg->SetAlpha(xout, yout, 0.0f);
        continue;
      }

      const mathutil::Vector<float> intersectionPoint = rayDirection * t;
      mathutil::Vector<float> vectorFromCenter = intersectionPoint - imageCenterWorld;

      float xOffset = vectorFromCenter * imageRightWorld;
      float yOffset = vectorFromCenter * imageUpWorld;

      const float normalizedX = (xOffset / imageWidthRelative) + 0.5f;
      const float normalizedY = (yOffset / imageHeightRelative) + 0.5f;

      const float xin = normalizedX * widthIn;
      const float yin = (1.0f - normalizedY) * heightIn; // Invert Y for Top-Left origin (0,0)

      resultImg->SetRed(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferR.get()));
      resultImg->SetGreen(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferG.get()));
      resultImg->SetBlue(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferB.get()));
      resultImg->SetAlpha(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferA.get()));

    }
  }

  return resultImg;
}


//**************************************************
// CreateDistortedImageCinema
//
// x axis is front - back pointing to the center
// y axis is left - right
// z axis is down - up 
//
// lambda rotates left right around the z axis (pan)
// phi rotates up down (tilt)
//
//
//***************************************************
std::unique_ptr<const image::TTImage> imagegen::ThreeSixtyDistortion::CreateDistortedImageCinema(const image::TTImage& inImage,
                                                                                                 const unsigned int widthResult,
                                                                                                 const unsigned int heightResult,
                                                                                                 const float anglePan,
                                                                                                 const float angleTilt,
                                                                                                 const float viewAngleHor)

{
  const unsigned int widthIn = inImage.Width();
  const unsigned int heightIn = inImage.Height();
  const size_t bufferSize = widthIn * heightIn;

  //convert deg -> rad
  const float lambda0 = cgi::DegToRad(anglePan);
  const float phi0 = cgi::DegToRad(angleTilt);
  const float viewAngleIdeal = cgi::DegToRad(viewAngleHor);
  const float imageWidthRelative = viewAngleIdeal;
  const float imageHeightRelative = imageWidthRelative * static_cast<float>(heightIn) / static_cast<float>(widthIn);
  
  const float dy = std::tan(phi0);
  const float phiMax = std::atan(imageHeightRelative / 2.0f);

  const std::unique_ptr<image::t_proc[]> inBufferR = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferG = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferB = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferA = std::make_unique<image::t_proc[]>(bufferSize);

  for (unsigned int yg = 0; yg < heightIn; yg++) {
    for (unsigned int xg = 0; xg < widthIn; xg++) {
      inBufferR[xg + yg * widthIn] = inImage.GetRed(xg, yg);
      inBufferG[xg + yg * widthIn] = inImage.GetGreen(xg, yg);
      inBufferB[xg + yg * widthIn] = inImage.GetBlue(xg, yg);
      inBufferA[xg + yg * widthIn] = inImage.AlphaChannels() > 0 ? inImage.GetAlpha(xg, yg) : 1.0f;
    }
  }

  //create result
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(widthResult, heightResult, image::RGBA, image::FLOAT32);
  resultImg->SetImageZero();

  //loop through target pixel
  imgproc::InterpolatorFloat interpolator(widthIn, heightIn, imgproc::BILINEAR);

  for (unsigned yout = 0; yout < heightResult; ++yout) {
    if (yout % 100 == 0) std::cout << "processing line " << yout << std::endl;
    for (unsigned xout = 0; xout < widthResult; ++xout) {

      //get direction of ray
      const float lambda = (static_cast<float>(xout) / (widthResult - 1.0f)) * (2.0f * cgi::PI) - cgi::PI;
      const float phi = cgi::PI / 2.0f - (static_cast<float>(yout) / (heightResult - 1.0f)) * cgi::PI;
      
      const float xin = ((lambda - lambda0 + viewAngleIdeal / 2.0f) / viewAngleIdeal) * widthIn;
      const float yin = (1.0f - ((imageHeightRelative / 2.0f) * (1.0f + std::tan(phi) / std::tan(phiMax) - dy))) * heightIn;
      
      resultImg->SetRed(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferR.get()));
      resultImg->SetGreen(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferG.get()));
      resultImg->SetBlue(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferB.get()));
      resultImg->SetAlpha(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferA.get()));
    }
  }

  return resultImg;
}

//**************************************************
// PlaceImageUnderAngle
//
// x axis is front - back pointing to the center
// y axis is left - right
// z axis is down - up 
//
//***************************************************
std::unique_ptr<const image::TTImage> imagegen::ThreeSixtyDistortion::PlaceImageUnderAngle(const image::TTImage& inImage,
                                                                                           const unsigned int widthResult,
                                                                                           const unsigned int heightResult,
                                                                                           const float anglePan,
                                                                                           const float angleTilt,
                                                                                           const float viewAngleHor,
                                                                                           const float viewAngleVer)
{
  const unsigned int widthIn = inImage.Width();
  const unsigned int heightIn = inImage.Height();
  const size_t bufferSize = widthIn * heightIn;

  //convert deg -> rad
  const float lambda0 = cgi::DegToRad(anglePan);
  const float phi0 = cgi::DegToRad(angleTilt);
  const float viewAngleX = cgi::DegToRad(viewAngleHor);
  const float viewAngleY = cgi::DegToRad(viewAngleVer);
  const float imageWidthRelative = viewAngleX;
  const float imageHeightRelative = viewAngleY;

  const float dy = std::tan(phi0);
  const float phiMax = std::atan(imageHeightRelative / 2.0f);

  const std::unique_ptr<image::t_proc[]> inBufferR = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferG = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferB = std::make_unique<image::t_proc[]>(bufferSize);
  const std::unique_ptr<image::t_proc[]> inBufferA = std::make_unique<image::t_proc[]>(bufferSize);

  for (unsigned int yg = 0; yg < heightIn; yg++) {
    for (unsigned int xg = 0; xg < widthIn; xg++) {
      inBufferR[xg + yg * widthIn] = inImage.GetRed(xg, yg);
      inBufferG[xg + yg * widthIn] = inImage.GetGreen(xg, yg);
      inBufferB[xg + yg * widthIn] = inImage.GetBlue(xg, yg);
      inBufferA[xg + yg * widthIn] = inImage.AlphaChannels() > 0 ? inImage.GetAlpha(xg, yg) : 1.0f;
    }
  }

  //create result
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(widthResult, heightResult, image::RGBA, image::FLOAT32);
  resultImg->SetImageZero();

  //loop through target pixel
  imgproc::InterpolatorFloat interpolator(widthIn, heightIn, imgproc::BILINEAR);

  for (unsigned yout = 0; yout < heightResult; ++yout) {
    if (yout % 100 == 0) std::cout << "processing line " << yout << std::endl;
    for (unsigned xout = 0; xout < widthResult; ++xout) {

      //get direction of ray
      const float lambda = (static_cast<float>(xout) / (widthResult - 1.0f)) * (2.0f * cgi::PI) - cgi::PI;
      const float phi = cgi::PI / 2.0f - (static_cast<float>(yout) / (heightResult - 1.0f)) * cgi::PI;

      const float xin = ((lambda - lambda0 + viewAngleX / 2.0f) / viewAngleX) * widthIn;
      const float yin = (1.0f - ((phi - phi0 + viewAngleY / 2.0f) / viewAngleY)) * heightIn;

      resultImg->SetRed(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferR.get()));
      resultImg->SetGreen(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferG.get()));
      resultImg->SetBlue(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferB.get()));
      resultImg->SetAlpha(xout, yout, interpolator.GetGrayPixel(xin, yin, inBufferA.get()));
    }
  }

  return resultImg;
}












//************
// RotMatrixX
//************
mathutil::Matrix<float> imagegen::ThreeSixtyDistortion::RotMatrixX(const float phi) {
  mathutil::Matrix<float> m(3, 3);
  m.Set(0, 0, 1.0f);
  m.Set(1, 1, std::cos(phi));
  m.Set(1, 2, -std::sin(phi));
  m.Set(2, 1, std::sin(phi));
  m.Set(2, 2, std::cos(phi));
  //std::cout << m.ToString() << std::endl;
  return m;
}

//************
// RotMatrixY
//************
mathutil::Matrix<float> imagegen::ThreeSixtyDistortion::RotMatrixY(const float phi) {
  mathutil::Matrix<float> m(3, 3);
  m.Set(0, 0, std::cos(phi));
  m.Set(0, 2, std::sin(phi));
  m.Set(1, 1, 1.0f);
  m.Set(2, 0, -std::sin(phi));
  m.Set(2, 2, std::cos(phi));
  //std::cout << m.ToString() << std::endl;
  return m;
}

//************
// RotMatrixZ
//************
mathutil::Matrix<float> imagegen::ThreeSixtyDistortion::RotMatrixZ(const float theta) {

  const float cosTheta = std::cos(theta);
  const float sinTheta = std::sin(theta);

  mathutil::Matrix<float> m(3, 3);
  m.Set(0, 0, cosTheta);
  m.Set(0, 1, -sinTheta);
  m.Set(0, 2, 0);
  m.Set(1, 0, sinTheta);
  m.Set(1, 1, cosTheta);
  m.Set(1, 2, 0);
  m.Set(2, 0, 0);
  m.Set(2, 1, 0);
  m.Set(2, 2, 1);
  //std::cout << m.ToString() << std::endl;
  return m;
}

