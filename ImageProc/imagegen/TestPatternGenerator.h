#pragma once

#include "TTImageDraw.h"

namespace imagegen {

//*****************************************************************************
//*                         TestPatternGenerator                              *
//*                                                                           *
//*                         creates test pattern                              *
//*****************************************************************************
class TestPatternGenerator {

  public:

    TestPatternGenerator();

    std::unique_ptr<image::TTImage> CreateTestPatternGrid(const unsigned int width,
                                                          const unsigned int height,
                                                          const unsigned int divideX,
                                                          const unsigned int divideY,
                                                          const image::TTColor& color,
                                                          const bool transparent = false);

    std::unique_ptr<image::TTImage> CreateDegreePattern(const unsigned int width, const unsigned int height, const bool transparent);

    std::unique_ptr<image::TTImage> CreateTestPatternGridCoordColor(const unsigned int width,
                                                                    const unsigned int height,
                                                                    const unsigned int divideX,
                                                                    const unsigned int divideY,
                                                                    const image::TTColor& colorX0,
                                                                    const image::TTColor& colorX1,
                                                                    const image::TTColor& colorY0,
                                                                    const image::TTColor& colorY1,
                                                                    const bool transparent = false);
    

  private:


    void DrawColoredLine(image::TTImageDraw& draw, const int x0, const int y0, const int x1, const int y1, const image::TTColor& color0, const image::TTColor& color1);
    image::TTColor GetInterpolatedColor(const int x0, const int y0, const int x, const int y, const float length, const image::TTColor& color0, const image::TTColor& color1);



};


}

