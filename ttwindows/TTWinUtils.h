#ifndef __TTWIUTILS_h_
#define __TTWIUTILS_h_

#include <string>
#include <vector>
#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>

namespace ttwin{

//*****************************************************************************
//*       here are some utility functions and classes used by TTWin           *
//*                                                                           *
//*****************************************************************************

//some color conversion (taken from our photoshop plugins ColorServices class)
void HSBInt8FromRGBInt8(unsigned int &HR, unsigned int &SG, unsigned int &BB);
void RGBInt8FromHSBInt8(unsigned int &RH, unsigned int &GS, unsigned int &BB);



//read image resources and combines them to one 4 channel DIB
HBITMAP GetAlphaDibImage(const HINSTANCE Hinst,  //hinstance 
                         const HDC winHDC,       //device context of window
                         const WORD resIdImage,  //resourceId of image (RGB bitmap)
                         const WORD resIdMask,   //resourceId of mask  (even a RGB bitmap but only blue is used for alpha)
                         const int width,        //the width of image
                         const int height);      //the height of image

// blends the 4 channel DIB to the windows device context
void BlendAlphaDibImage(const HDC winHDC,   //win device context 
                        const HDC memHDC,   //memorey device context 
                        const HBITMAP dip,  //handle of the 4 channel dip 
                        const int posx,     //the x postion in dest dc
                        const int posy,     //the y postion in dest dc
                        const int wdest,    //the destination width 
                        const int hdest,    //the destination height
                        const int wsrc,     //the width of image
                        const int hsrc);    //the height of image

//load tool tips common controls
bool LoadCommonControlsToolTips();

//fills an lpString (char[]) with content from std::string
char* LpStringByStdString(const std::string& s, char* lpString, const unsigned int length);

//fills an lpString (char[]) with content from std::string
wchar_t* LpWStringByStdString(const std::string& s, wchar_t* lpString, const unsigned int length);

//splits a 0-separated char-string into std-string
std::vector<std::string> SplitCharZeroString(const char* const lpString, const unsigned int length);

//builds a 0-separated char-string from std-strings 
char* LpStringZeroByStdStringVector(const std::vector<std::string>& filter, char* lpString, const unsigned int length);

//std::string from std::wstring
std::string StringByWString(const std::wstring& wstr);

// std::wstring from std::string
std::wstring WStringByString(const std::string& str);

} // end of namespace ttwin

#endif
