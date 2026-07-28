#include "simplerenderer.h"


//************************************************************************
//*                                                                      *
//*                             WireRenderer                             *
//*                                                                      *
//*            this is the abstract renderer for wire objects            *
//************************************************************************

//****************************
// RenderScene
//
// renders the SimpleWireScene 
//****************************
void cgi::WireRenderer::RenderScene(WireScene& scene, const Projectiontype projection){
  
  //do nothing if there is nothing to do
  if (projection == NOTHING || scene.Empty()) return;
  
  scene.GetCamera().SetResolution(ScreenWidth, ScreenHeight);
  scene.InitRendering();
  
  Drawbackground(scene.GetBackColor());

  //we have to delete the internalt vectors later
  ColoredLine line;
  std::vector<ColoredLine*> lines;

  //get all lines of scene, store them, sort them by priority and render them
  int ret = 0;
  while ((ret = scene.GetNextScreenLine(line))){
    if (ret == 1){
      lines.push_back(line.DeepCopy());
    }
  }
  std::sort(lines.begin(), lines.end(), &CompareLines);
  for (size_t i = 0; i < lines.size(); ++i){
    SetLineColor(lines[i]->Color);
    MoveTo(lines[i]->Start->X, lines[i]->Start->Y);
    LineTo(lines[i]->End->X, lines[i]->End->Y);
    delete lines[i];
  }

  //delete line's points, we are the owner
  line.DeletePoints();
}
  
//****************************************
// compareLines
//
// global function for sorting by priority
//****************************************
bool cgi::CompareLines(const ColoredLine* line1, const ColoredLine* line2){
  return line1->Priority < line2->Priority;
} 

//************************************************************************
//*                                                                      *
//*                           ASCIIWireRenderer                          *
//*                                                                      *
//*               renders to a char array for ASCII graphic              *
//************************************************************************

//************
// constructor
//************
cgi::ASCIIWireRenderer::ASCIIWireRenderer(unsigned int width, unsigned int height)
                       :currPosX(0), 
                        currPosY(0),
                        ForegroundChar('*'),
                        BackgroundChar(' ')
{
  ScreenWidth = width;
  ScreenHeight = height;
  Screen = new unsigned char[ScreenWidth * ScreenHeight];
  for (unsigned int i = 0; i < (ScreenWidth * ScreenHeight); ++i) Screen[i] = 0;
}
    
//***********
// destructor
//***********
cgi::ASCIIWireRenderer::~ASCIIWireRenderer(){
  delete Screen;  
}
    
//***************
// Print
//
// print function
//***************
void cgi::ASCIIWireRenderer::Print(){
  for (unsigned int x = 0; x < ScreenWidth + 2; ++x) std::cout << "-";
  std::cout << std::endl;
  for (unsigned int y = 0; y < ScreenHeight; ++y){
    std::string line = "|";
    for (unsigned int x = 0; x < ScreenWidth; ++x){
      line += (Screen[x + y * ScreenWidth] > 0 ? Screen[x + y * ScreenWidth] : BackgroundChar);   
    }  
    std::cout << line << "|" << std::endl;
  }    
  for (unsigned int x = 0; x < ScreenWidth + 2; ++x) std::cout << "-";
  std::cout << std::endl;
}

//***************
// Drawbackground
//***************
void cgi::ASCIIWireRenderer::Drawbackground(const RGBColor& color){
  BackgroundChar = (0.3 * color.R + 0.59 * color.G + 0.11 * color.B) > 0.5 ? '*' : ' ';
  for (unsigned int i = 0; i < (ScreenWidth * ScreenHeight); ++i) Screen[i] = BackgroundChar;
}

//*************
// SetLineColor
//*************
void cgi::ASCIIWireRenderer::SetLineColor(const RGBColor& color){
  ForegroundChar = (0.3 * color.R + 0.59 * color.G + 0.11 * color.B) > 0.5 ? '*' : ' ';
}

//********
// MoveTo
//********
void cgi::ASCIIWireRenderer::MoveTo(const double x, const double y){
  currPosX = static_cast<int>(x + 0.5);
  currPosY = static_cast<int>(y + 0.5);
}

//********
// LineTo
//********
void cgi::ASCIIWireRenderer::LineTo(const double x, const double y){
  const int posX = static_cast<int>(x + 0.5);
  const int posY = static_cast<int>(y + 0.5);
  
  //@TODO: a better algorithm 
  int dx = posX - currPosX;
  int dy = posY - currPosY;
  
  if (dx == 0 && dy == 0){
    Screen[currPosX + currPosY * ScreenWidth] = ForegroundChar;
    return; 
  }
  
  int xg, yg;
  if (abs(dx) >= abs(dy)){
    const double ratio = static_cast<double>(dy) / static_cast<double>(dx); 
    const int increment = (dx >= 0) ? 1 : -1; 
    for (int cdx = 0; cdx  != dx; cdx  += increment){
      xg = currPosX + cdx;
      yg = currPosY + static_cast<int>(ratio * static_cast<double>(cdx) + 0.5);
      if (xg >= 0 && xg < static_cast<int>(ScreenWidth) && yg >= 0 && yg < static_cast<int>(ScreenHeight)){
        Screen[xg + yg * ScreenWidth] = ForegroundChar;
      }
    }
  }
  else{
    const double ratio = static_cast<double>(dx) / static_cast<double>(dy); 
    const int increment = (dy >= 0) ? 1 : -1; 
    for (int cdy = 0; cdy  != dy; cdy  += increment){
      xg = currPosX + static_cast<int>(ratio * static_cast<double>(cdy) + 0.5);
      yg = currPosY + cdy;
      if (xg >= 0 && xg < static_cast<int>(ScreenWidth) && yg >= 0 && yg < static_cast<int>(ScreenHeight)){
        Screen[xg + yg * ScreenWidth] = ForegroundChar;
      }
    }
  }
  
  currPosX = posX;
  currPosY = posY;
} 

//************************************************************************
//*                                                                      *
//*                           BMPWireRenderer                            *
//*                                                                      *
//*                        renders to a bitmap                           *
//************************************************************************

//************
// constructor
//************
cgi::BMPWireRenderer::BMPWireRenderer(void* data, unsigned int width, unsigned int height)
                     :Image(static_cast<U8*>(data)),
                      LineColorR(255), 
                      LineColorG(255), 
                      LineColorB(255)
{
  ScreenWidth = width;
  ScreenHeight = height;
  Reset();
}
    
//***********
// destructor
//***********
cgi::BMPWireRenderer::~BMPWireRenderer(){}
    
    
//***************
// Drawbackground
//***************
void cgi::BMPWireRenderer::Drawbackground(const RGBColor& color){
  for (unsigned int i = 0; i < (ScreenWidth * ScreenHeight * 3); i += 3){ 
    Image[i + 2] = static_cast<U8>(color.R * 255.0);
    Image[i + 1] = static_cast<U8>(color.G * 255.0);
    Image[i + 0] = static_cast<U8>(color.B * 255.0);
  }
}

//*************
// SetLineColor
//*************
void cgi::BMPWireRenderer::SetLineColor(const RGBColor& color){
  LineColorR = static_cast<U8>(color.R * 255.0);
  LineColorG = static_cast<U8>(color.G * 255.0);
  LineColorB = static_cast<U8>(color.B * 255.0);
}

//********
// MoveTo
//********
void cgi::BMPWireRenderer::MoveTo(const double x, const double y){
  currPosX = static_cast<int>(x + 0.5);
  currPosY = static_cast<int>(ScreenHeight) - static_cast<int>(y + 0.5) + 1;
}

//********
// LineTo
//********
void cgi::BMPWireRenderer::LineTo(const double x, const double y){
  const int posX = static_cast<int>(x + 0.5);
  const int posY = static_cast<int>(ScreenHeight) - static_cast<int>(y + 0.5) + 1;
  
  //@TODO: a better algorithm 
  int dx = posX - currPosX;
  int dy = posY - currPosY;
  
  if (dx == 0 && dy == 0){
    SetPixel(currPosX, currPosY, LineColorR, LineColorG, LineColorB);
    return; 
  }
  
  int xg, yg;
  if (abs(dx) >= abs(dy)){
    const double ratio = static_cast<double>(dy) / static_cast<double>(dx); 
    const int increment = (dx >= 0) ? 1 : -1; 
    for (int cdx = 0; cdx  != dx; cdx  += increment){
      xg = currPosX + cdx;
      yg = currPosY + static_cast<int>(ratio * static_cast<double>(cdx) + 0.5);
      if (xg >= 0 && xg < static_cast<int>(ScreenWidth) && yg >= 0 && yg < static_cast<int>(ScreenHeight)){
        SetPixel(xg, yg, LineColorR, LineColorG, LineColorB);
      }
    }
  }
  else{
    const double ratio = static_cast<double>(dx) / static_cast<double>(dy); 
    const int increment = (dy >= 0) ? 1 : -1; 
    for (int cdy = 0; cdy  != dy; cdy  += increment){
      xg = currPosX + static_cast<int>(ratio * static_cast<double>(cdy) + 0.5);
      yg = currPosY + cdy;
      if (xg >= 0 && xg < static_cast<int>(ScreenWidth) && yg >= 0 && yg < static_cast<int>(ScreenHeight)){
        SetPixel(xg, yg, LineColorR, LineColorG, LineColorB);
      }
    }
  }
  
  currPosX = posX;
  currPosY = posY;
} 

//*****************
// Reset
//
// clears the image
//*****************
void cgi::BMPWireRenderer::Reset(){
  for (unsigned int i = 0; i < (ScreenWidth * ScreenHeight * 3); ++i){ 
    Image[i] = 0;
  }
}

//**********
// SetPixel
//**********
inline void cgi::BMPWireRenderer::SetPixel(const unsigned int x, const unsigned int y, U8 r, U8 g, U8 b){
  Image[(x + y * ScreenWidth) * 3 + 2] = r;
  Image[(x + y * ScreenWidth) * 3 + 1] = g;
  Image[(x + y * ScreenWidth) * 3    ] = b;
}
    
  
