#include "TestPatternGenerator.h"


//*****************************************************************************
//*                         TestPatternGenerator                              *
//*                                                                           *
//*                         creates test pattern                              *
//*****************************************************************************

//************
// constructor
//************
imagegen::TestPatternGenerator::TestPatternGenerator(){}


//*****************************
// CreateTestPatternGrid
//
// creates a test pattern grid
//*****************************
std::unique_ptr<image::TTImage> imagegen::TestPatternGenerator::CreateTestPatternGrid(const unsigned int width,
                                                                                      const unsigned int height,
                                                                                      const unsigned int divideX,
                                                                                      const unsigned int divideY,
                                                                                      const image::TTColor& color,
                                                                                      const bool transparent)
{
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(width, height, transparent ? image::RGBA :image::RGB, image::FLOAT32);
  resultImg->SetImageZero();

  image::TTImageDraw draw(resultImg.get());
  draw.SetPen(color);

  const unsigned int dx = width / divideX;
  for (unsigned int x = 0; x < width; x += dx) {
    draw.MoveTo(x, 0);
    draw.LineTo(x, height - 1);
  }
  draw.MoveTo(width - 1, 0);
  draw.LineTo(width - 1, height - 1);
  const unsigned int dy = height / divideY;
  for (unsigned int y = 0; y < height; y += dy) {
    draw.MoveTo(0, y);
    draw.LineTo(width - 1, y);
  }
  draw.MoveTo(0, height - 1);
  draw.LineTo(width - 1, height - 1);

  return resultImg;
}

//*****************************
// CreateDegreePattern
//
// creates a test pattern grid
//*****************************
std::unique_ptr<image::TTImage> imagegen::TestPatternGenerator::CreateDegreePattern(const unsigned int width, const unsigned int height, const bool transparent)
{
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(width, height, transparent ? image::RGBA : image::RGB, image::FLOAT32);
  resultImg->SetImageZero();

  image::TTImageDraw draw(resultImg.get());

  // Define colors for the lines
  image::TTColor gray(0.5f, 0.5f, 0.5f);
  image::TTColor white(1.0f, 1.0f, 1.0f);
  const int thickness = 5;

  // ---- Draw vertical lines (longitude) ----
  for (int deg = -180; deg <= 180; deg += 10) {
    const int x = static_cast<int>((deg / 360.0f) * width + width / 2.0f);
    if (deg % 45 == 0) {
      draw.SetPen(white);
    }
    else {
      draw.SetPen(gray);
    }
    for (int i = 0; i < thickness; ++i) {
      const int xu = x + i - thickness / 2;
      if (xu >= 0 && xu < width) {
        draw.MoveTo(xu, 0);
        draw.LineTo(xu, height - 1);
      }
    }
  }
  // ---- Draw horizontal lines (latitude) ----
  for (int deg = -90; deg <= 90; deg += 10) {
    int y = static_cast<int>((deg / 180.0f) * height + height / 2.0f);
    y = height - 1 - y;

    if (deg % 45 == 0) {
      draw.SetPen(white);
    }
    else {
      draw.SetPen(gray);
    }
    for (int i = 0; i < thickness; ++i) {
      const int yu = y + i - thickness / 2;
      if (yu >= 0 && yu < height) {
        draw.MoveTo(0, yu);
        draw.LineTo(width - 1, yu);
      }
    }
  }


  return resultImg;
}

//********************************
// CreateTestPatternGridCoordColor
//
// creates a test pattern grid
//********************************
std::unique_ptr<image::TTImage> imagegen::TestPatternGenerator::CreateTestPatternGridCoordColor(const unsigned int width,
                                                                                                const unsigned int height,
                                                                                                const unsigned int divideX,
                                                                                                const unsigned int divideY,
                                                                                                const image::TTColor& colorX0,
                                                                                                const image::TTColor& colorX1,
                                                                                                const image::TTColor& colorY0,
                                                                                                const image::TTColor& colorY1,
                                                                                                const bool transparent)
{
  std::unique_ptr<image::TTSimpleMemoryImage> resultImg = std::make_unique<image::TTSimpleMemoryImage>();
  resultImg->Create(width, height, transparent ? image::RGBA : image::RGB, image::FLOAT32);
  resultImg->SetImageZero();

  image::TTImageDraw draw(resultImg.get());

  const unsigned int dx = width / divideX;
  for (unsigned int x = 0; x < width; x += dx) {
    DrawColoredLine(draw, x, 0, x, height - 1, colorY0, colorY1);
  }
  DrawColoredLine(draw, width - 1, 0, width - 1, height - 1, colorY0, colorY1);
  const unsigned int dy = height / divideY;
  for (unsigned int y = 0; y < height; y += dy) {
    DrawColoredLine(draw, 0, y, width - 1, y, colorX0, colorX1);
  }
  DrawColoredLine(draw, 0, height - 1, width - 1, height - 1, colorX0, colorX1);

  return resultImg;
}



//****************
// DrawColoredLine
//****************
void imagegen::TestPatternGenerator::DrawColoredLine(image::TTImageDraw& draw, 
                                                     const int x0, const int y0, const int x1, const int y1, 
                                                     const image::TTColor& color0, const image::TTColor& color1) 
{

  //Bresenham from Wikipedia ;-)
  const int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  const int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */

  int x = x0;
  int y = y0;

  const float l = sqrtf(static_cast<float>(dx * dx + dy * dy));

  for (;;) {
    draw.SetPen(GetInterpolatedColor(x0, y0, x, y,l, color0, color1));
    draw.SetPixel(x, y);
    if (x == x1 && y == y1) break;
    e2 = 2 * err;
    if (e2 > dy) { err += dy; x += sx; } // e_xy+e_x > 0
    if (e2 < dx) { err += dx; y += sy; } // e_xy+e_y < 0
  }
}


//*********************
// GetInterpolatedColor
//*********************
image::TTColor imagegen::TestPatternGenerator::GetInterpolatedColor(const int x0, const int y0, const int x, const int y, const float length,
                                                                    const image::TTColor& color0, const image::TTColor& color1)
{
  const int dx = x - x0;
  const int dy = y - y0;
  const float l = sqrtf(static_cast<float>(dx * dx + dy * dy));
  const float alpha = l / length;

  return color1 * alpha +  color0 * (1.0f - alpha);

}
