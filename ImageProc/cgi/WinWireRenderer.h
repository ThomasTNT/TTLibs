#ifndef _WIN_WIRE_RENDERER_H_
#define _WIN_WIRE_RENDERER_H_

#include <windows.h>
#include "ColorServices.h"
#include "simplerenderer.h"

namespace cgi{

//************************************************************************
//*                                                                      *
//*                            WinWireRenderer                           *
//*                                                                      *
//*                    simple wire renderer for window GDI               *
//************************************************************************
class WinWireRenderer : public WireRenderer{
  
  
  public: 
    
    // constructor
    WinWireRenderer(const RECT rect);
    
    // destructor
    ~WinWireRenderer();

    // init rendering
    void StartRendering(HDC hDC);

    // stops rendering
    void StopRendering();

  protected: 
    
    // drawing methods
    virtual void Drawbackground(const RGBColor& color);
    virtual void SetLineColor(const RGBColor& color);
    virtual void MoveTo(const double x, const double y); 
    virtual void LineTo(const double x, const double y); 
    
  private:

    // the windows device context
    HDC hDC;

    // the current pen
    HPEN Pen; 

    // the screen rectangle
    RECT ScreenRect; 
}; 

} // end of namespace cgi

#endif
