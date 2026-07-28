#pragma once

#include "TTImage.h"
#include "InterpolatorFloat.h"
#include "GeometryAlgorithms.h"

namespace imagegen {



//*****************************************************************************
//*                         ThreeSixtyDistortion                              *
//*                                                                           *
//*              transform an rectangular image into 360° space               *
//*****************************************************************************
class ThreeSixtyDistortion {

  public:
    
    ThreeSixtyDistortion();

    std::unique_ptr<const image::TTImage> CreateDistortedImagePlane(const image::TTImage& inImage, 
                                                                    const unsigned int widthResult, 
                                                                    const unsigned int heightResult, 
                                                                    const float anglePan,
                                                                    const float angleTilt,
                                                                    const float viewAngleHor);

    std::unique_ptr<const image::TTImage> CreateDistortedImageUpright(const image::TTImage& inImage,
                                                                      const unsigned int widthResult,
                                                                      const unsigned int heightResult,
                                                                      const float anglePan,
                                                                      const float angleTilt,
                                                                      const float viewAngleHor);

    std::unique_ptr<const image::TTImage> CreateDistortedImageCinema(const image::TTImage& inImage,
                                                                     const unsigned int widthResult,
                                                                     const unsigned int heightResult,
                                                                     const float anglePan,
                                                                     const float angleTilt,
                                                                     const float viewAngleHor);

    std::unique_ptr<const image::TTImage> PlaceImageUnderAngle(const image::TTImage& inImage,
                                                               const unsigned int widthResult,
                                                               const unsigned int heightResult,
                                                               const float anglePan,
                                                               const float angleTilt,
                                                               const float viewAngleHor,
                                                               const float viewAngleVer);


  private: 

    mathutil::Matrix<float> RotMatrixX(const float phi);
    mathutil::Matrix<float> RotMatrixY(const float lambda);
    mathutil::Matrix<float> RotMatrixZ(const float lambda);
};


}