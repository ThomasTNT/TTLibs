#ifndef _SIMPLE_RENDERER_H_
#define _SIMPLE_RENDERER_H_

#include "camera.h"
#include "scene.h"

//#ifndef  NOT_IN_PS_PLUGIN
  //#include "ColorServices.h" // for types 
  //#include "Logger.h"
//#else
  #include "TTImage.h"
//#endif
#
#include <cmath>
#include <algorithm>
#include <iostream>

namespace cgi{
          
//************************************************************************
//*                                                                      *
//*                             WireRenderer                             *
//*                                                                      *
//*            this is the abstract renderer for wire objects            *
//************************************************************************
class WireRenderer{
  
  public: 
    
    //renders the scene 
    void RenderScene(WireScene& scene, const Projectiontype projection);
  
  protected: 
    
    //drawing methods
    virtual void Drawbackground(const RGBColor& color) = 0;
    virtual void SetLineColor(const RGBColor& color) = 0;
    virtual void MoveTo(const double x, const double y) = 0; 
    virtual void LineTo(const double x, const double y) = 0; 
    
    //screen dimensions
    unsigned int ScreenWidth;
    unsigned int ScreenHeight;
    
}; 

// global function for sorting by priority
bool CompareLines(const ColoredLine* line1, const ColoredLine* line2);

//************************************************************************
//*                                                                      *
//*                           ASCIIWireRenderer                          *
//*                                                                      *
//*                renders to a char array for ASCII graphic             *
//************************************************************************
class ASCIIWireRenderer : public WireRenderer{
  
  public: 
    
    // constructor
    ASCIIWireRenderer(unsigned int width, unsigned int height);
    
    // destructor
    ~ASCIIWireRenderer();
    
    // print function
    void Print();
    
  protected: 
    
    //drawing methods
    virtual void Drawbackground(const RGBColor& color);
    virtual void SetLineColor(const RGBColor& color);
    virtual void MoveTo(const double x, const double y); 
    virtual void LineTo(const double x, const double y); 
    
  private: 
    
    // the char array to render to 
    unsigned char* Screen;
    
    //"colors"
    char BackgroundChar; 
    char ForegroundChar;

    //current position 
    int currPosX;
    int currPosY;
    
}; 


//************************************************************************
//*                                                                      *
//*                           BMPWireRenderer                            *
//*                                                                      *
//*                        renders to a bitmap                           *
//************************************************************************
class BMPWireRenderer : public WireRenderer{
  
  public: 
    
    // constructor
    BMPWireRenderer(void* data, unsigned int width, unsigned int height);
    
    // destructor
    ~BMPWireRenderer();
    
  protected: 
    
    //drawing methods
    virtual void Drawbackground(const RGBColor& color);
    virtual void SetLineColor(const RGBColor& color);
    virtual void MoveTo(const double x, const double y); 
    virtual void LineTo(const double x, const double y); 
    
  private: 
    
    // clears the image
    void Reset();
    
    // sets a pixel
    inline void SetPixel(const unsigned int x, const unsigned int y, U8 r, U8 g, U8 b);
    
    // image data
    U8* Image;
    
    //current position 
    int currPosX;
    int currPosY;

    //current line color
    U8 LineColorR, LineColorG, LineColorB;
    
}; 

} // end of namespace cgi


#endif
