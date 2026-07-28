#pragma once

#include "TTImage.h"

namespace image{

//************************************************************************
//*                                                                      *
//*                              TTImageDraw                             *
//*                                                                      *
//*                     class for drawing on images                      *
//*                                                                      *
//************************************************************************
class TTImageDraw{

  public:

    //constructor
    TTImageDraw(TTImage * const image);

    //sets pen style and color
    void SetPen(const t_proc r, const t_proc g, const t_proc b);
    void SetPen(const image::TTColor& color);
    

    //move to 
    void MoveTo(const int x, const int y);

    //line to
    void LineTo(const int x, const int y);

    //draws a circle
    void DrawCircle(const int x, const int y, const int r);

    //draws a circle
    void DrawFullCircle(const int x, const int y, const int r);

    //draws a rechtangle
    void DrawRect(const int x0, const int x1, const int y0, const int y1);

    //draws a rechtangle filled
    void DrawFullRect(const int x0, const int x1, const int y0, const int y1);

    //draws a rechtangle
    void DrawSquareCenter(const int x, const int y, const unsigned int a);

    //sets a single pixel (does not move)
    void SetPixel(const int x, const int y);

    //smears a point into the image (antialiased) 
    void SmearPointIntoImage(const image::t_proc r, const image::t_proc g, const image::t_proc b,
                             const int x, const int y,
                             const float pointsize);


  private:

    //draws a line
    void DrawLine(int x0, int y0, const int x1, const int y1);

    TTImage * Image = nullptr;

    int CurrX = 0;
    int CurrY = 0;

    t_proc Rpen = 1.0f;
    t_proc Gpen = 1.0f;
    t_proc Bpen = 1.0f;
};




} //end of namespace images
