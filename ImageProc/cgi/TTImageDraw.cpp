#include "TTImageDraw.h"


//************************************************************************
//*                                                                      *
//*                              TTImageDraw                             *
//*                                                                      *
//*                     class for drawing on images                      *
//*                                                                      *
//************************************************************************

//************
// constructor
//************
image::TTImageDraw::TTImageDraw(TTImage * const image) : Image(image){}

//*************************
// SetPen
//
// sets pen style and color
//*************************
void image::TTImageDraw::SetPen(const t_proc r, const t_proc g, const t_proc b){
  Rpen = r;
  Gpen = g;
  Bpen = b;
}

//*************************
// SetPen
//
// sets pen style and color
//*************************
void image::TTImageDraw::SetPen(const image::TTColor& color) {
  Rpen = color.GetRed();
  Gpen = color.GetGreen();
  Bpen = color.GetBlue();
}

//*******
// MoveTo
//*******
void image::TTImageDraw::MoveTo(const int x, const int y){
  CurrX = x;
  CurrY = y;
}

//*******
// LineTo
//*******
void image::TTImageDraw::LineTo(const int x, const int y){
  DrawLine(CurrX, CurrY, x, y);
  MoveTo(x, y);
}

//*********
// DrawRect
//*********
void image::TTImageDraw::DrawRect(const int x0, const int x1, const int y0, const int y1){
  DrawLine(x0, y0, x1, y0);
  DrawLine(x1, y0, x1, y1);
  DrawLine(x1, y1, x0, y1);
  DrawLine(x0, y1, x0, y0);
  MoveTo(x0, y0);
}

//*************
// DrawFullRect
//*************
void image::TTImageDraw::DrawFullRect(const int x0, const int x1, const int y0, const int y1) {
  for (int x = x0; x <= x1; ++x) {
    DrawLine(x, y0, x, y1);
  }
  MoveTo(x0, y0);
}


//*****************
// DrawSquareCenter
//*****************
void image::TTImageDraw::DrawSquareCenter(const int x, const int y, const unsigned int a){
  const int x0 = x - a;
  const int x1 = x + a;
  const int y0 = y - a;
  const int y1 = y + a;
  DrawLine(x0, y0, x1, y0);
  DrawLine(x1, y0, x1, y1);
  DrawLine(x1, y1, x0, y1);
  DrawLine(x0, y1, x0, y0);
  MoveTo(x, y);
}

//************************************
// SetPixel
//
// sets a single pixel (does not move)
//************************************
void image::TTImageDraw::SetPixel(const int x, const int y){
  if (x >= 0 && static_cast<unsigned int>(x) < Image->Width() && y >= 0 && static_cast<unsigned int>(y) < Image->Height()){
    Image->SetRed(x, y, Rpen);
    Image->SetGreen(x, y, Gpen);
    Image->SetBlue(x, y, Bpen);
    if (Image->AlphaChannels() > 0) Image->SetAlpha(x, y, 1);
  }
}


//***********
// DrawCircle
//***********
void image::TTImageDraw::DrawCircle(const int x, const int y, const int r){

  //Bresenham from Wikipedia ;-)
  int f = 1 - r;
  int ddF_x = 0;
  int ddF_y = -2 * r;
  int x1 = 0;
  int y1 = r;

  SetPixel(x, y + r);
  SetPixel(x, y - r);
  SetPixel(x + r, y);
  SetPixel(x - r, y);

  while (x1 < y1){
    if (f >= 0){
      y1--;
      ddF_y += 2;
      f += ddF_y;
    }
    x1++;
    ddF_x += 2;
    f += ddF_x + 1;

    SetPixel(x + x1, y + y1);
    SetPixel(x - x1, y + y1);
    SetPixel(x + x1, y - y1);
    SetPixel(x - x1, y - y1);
    SetPixel(x + y1, y + x1);
    SetPixel(x - y1, y + x1);
    SetPixel(x + y1, y - x1);
    SetPixel(x - y1, y - x1);
  }

  MoveTo(x, y);
}


//***************
// DrawFullCircle
//***************
void image::TTImageDraw::DrawFullCircle(const int x, const int y, const int r) {
  for (unsigned int i = 1; i <= static_cast<unsigned int>(abs(r)); ++i) {
    DrawCircle(x, y, i);
  }
}


//*********
// DrawLine
//*********
void image::TTImageDraw::DrawLine(int x0, int y0, const int x1, const int y1){

  //Bresenham from Wikipedia ;-)
  int dx = abs(x1 - x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0<y1 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */

  for (;;){  
    SetPixel(x0, y0);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 > dy) { err += dy; x0 += sx; } // e_xy+e_x > 0
    if (e2 < dx) { err += dx; y0 += sy; } // e_xy+e_y < 0
  }
}



//******************************************
// SmearPointIntoImage
//
//smears a pixel into the image(antialiased)
//******************************************
void image::TTImageDraw::SmearPointIntoImage(const image::t_proc r, const image::t_proc g, const image::t_proc b,
                                             const int x, const int y,
                                             const float pointsize)
{
  //simple implementation
  //TODO use gauss etc. or an extra class 

  const unsigned int intSize = (pointsize - static_cast<int>(pointsize)) > 0.2f ? static_cast<unsigned int>(pointsize)+1 : static_cast<unsigned int>(pointsize);
  int xg, yg;
  const unsigned int w = Image->Width();
  const unsigned int h = Image->Height();
  for (int iy = 0; iy < static_cast<int>(intSize); ++iy){
    yg = y + iy;
    if (yg >= 0 && static_cast<unsigned int>(yg) < h){
      for (int ix = 0; ix < static_cast<int>(intSize); ++ix){
        xg = x + ix;
        if (xg >= 0 && static_cast<unsigned int>(xg) < w){
          Image->AddRed(xg, yg, r);
          Image->AddGreen(xg, yg, g);
          Image->AddBlue(xg, yg, b);
        }
      }
    }
  }

}