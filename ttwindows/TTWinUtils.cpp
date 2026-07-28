#include "TTWinUtils.h"


//*****************************************************************************
//*       here are some utility functions and classes used by TTWin           *
//*                                                                           *
//*****************************************************************************

//***********************
//RGB (int) -> HSB (int)
//***********************
void ttwin::HSBInt8FromRGBInt8(unsigned int &HR, unsigned int &SG, unsigned int &BB){
	unsigned int cmax = (HR > SG) ? HR : SG;
  if (BB > cmax) cmax = BB;
  unsigned int cmin = (HR < SG) ? HR : SG;
  if (BB < cmin) cmin = BB;
 
  const float cmaxminusmin = static_cast<float>(cmax - cmin); 
  const unsigned int saturation =   cmax != 0
                        ? static_cast<unsigned int>(255.0f * cmaxminusmin / static_cast<float>(cmax) + 0.5f)
                        : 0;
                  
  float hue = 1000.0;

  if (saturation == 0){
    hue = 0.0f; 
  }
  else{
    if (HR == cmax){
      hue = 60.0f * (static_cast<float>(SG) - static_cast<float>(BB) ) / cmaxminusmin;
    }
    else if (SG == cmax){
      hue = 60.0f * (2.0f + (static_cast<float>(BB) - static_cast<float>(HR)) / cmaxminusmin );
    }
    else{
      hue = 60.0f * (4.0f + (static_cast<float>(HR) - static_cast<float>(SG)) / cmaxminusmin );
    }
    if (hue < 0.0f) hue = hue + 360.0f;
  }
 
  BB = cmax;
  SG = saturation;
  HR = static_cast<unsigned int>(hue + 0.5f);
}

//**************************************
//HSB (16bit/8bit int) -> RGB (8bit int)
//**************************************
void ttwin::RGBInt8FromHSBInt8(unsigned int &RH, unsigned int &GS, unsigned int &BB){
  if (GS == 0){
    RH = GS = BB;
  }
  else{
    const float h = static_cast<float>(RH) / 60.0f;
    const int  hi = static_cast<int>(h);
    const float f = h - static_cast<float>(hi);
   
    const unsigned int p = static_cast<unsigned int>(static_cast<float>(BB) * (1.0f - static_cast<float>(GS) / 255.0f) + 0.5f);
    const unsigned int q = static_cast<unsigned int>(static_cast<float>(BB) * (1.0f - f * static_cast<float>(GS) / 255.0f) + 0.5f);
    const unsigned int t = static_cast<unsigned int>(static_cast<float>(BB) * (1.0f - (1.0f - f) * static_cast<float>(GS) / 255.0f) + 0.5f);
   
    switch (hi) {
      case 0:
        RH = BB;
        GS = t;
        BB = p;
        break;
      case 1:
        RH = q;
        GS = BB;
        BB = p;
        break;
      case 2:
        RH = p;
        GS = BB;
        BB = t;
        break;
      case 3:
        RH = p;
        GS = q;
        BB = BB;
        break;
      case 4:
        RH = t;
        GS = p;
        BB = BB;
        break;
      case 5:
        RH = BB;
        GS = p;
        BB = q;
        break;
    }
  }
}


//************************************************************
// GetAlphaDipImage
//
// read image resources and combines them to one 4 channel DIB
//************************************************************
HBITMAP ttwin::GetAlphaDibImage(const HINSTANCE Hinst,  //hinstance 
                                const HDC winHDC,       //device context of window
                                const WORD resIdImage,  //resourceId of image (RGB bitmap)
                                const WORD resIdMask,   //resourceId of mask  (even a RGB bitmap but only blue is used for alpha)
                                const int width,        //the width of image
                                const int height)       //the height of image
  {
    //-------------- open source images ------------

    //the source image
    const HANDLE image = ::LoadImage(Hinst, MAKEINTRESOURCE(resIdImage), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR); 

    //the source image maske
    const HANDLE mask = ::LoadImage(Hinst, MAKEINTRESOURCE(resIdMask), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR); 

    //-------------- create 4 channel DIP ----------

    //the device context of dip
    const HDC dipHDC = ::CreateCompatibleDC(winHDC);

    //create 4 channel dip 
    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;         // four 8-bit components (with alpha)
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = width * height * 4;

    VOID *pvBits;          // pointer to DIB section
    const HBITMAP hDIP = ::CreateDIBSection(dipHDC, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
    
    //-------------- fills the image ------------
    const HDC imageHDC = ::CreateCompatibleDC(winHDC);
    ::SelectObject(imageHDC, image);
    const HDC maskHDC = ::CreateCompatibleDC(winHDC);
    ::SelectObject(maskHDC, mask);
    BYTE* pixels = reinterpret_cast<unsigned char*>(pvBits);
    for (int h = 0; h < height; ++h){
      for (int w = 0; w < width; ++w){
        const COLORREF color = ::GetPixel(imageHDC, w, h);
        const COLORREF mask = ::GetPixel(maskHDC, w, h);
        pixels[((h * width) + w) * 4 + 0] = GetBValue(color); //blue
        pixels[((h * width) + w) * 4 + 1] = GetGValue(color); //green
        pixels[((h * width) + w) * 4 + 2] = GetRValue(color); //red
        pixels[((h * width) + w) * 4 + 3] = GetBValue(mask);  //alpha
      }
    }

    //--------------- clean up -------------------

    //dc has to be cleaned first
    ::DeleteDC(imageHDC);  
    ::DeleteDC(maskHDC);
    //the the images
    ::DeleteObject(image); 
    ::DeleteObject(mask);
     
    ::DeleteDC(dipHDC); //?????

    return hDIP;
  }


//********************************************************
// BlendAlphaDipImage
//
// blends the 4 channel DIB to the windows device context
//********************************************************
void ttwin::BlendAlphaDibImage(const HDC winHDC,   //win device context 
                               const HDC memHDC,   //memorey device context 
                               const HBITMAP dip,  //handle of the 4 channel dip 
                               const int posx,     //the x postion in dest dc
                               const int posy,     //the y postion in dest dc
                               const int wdest,    //the destination width 
                               const int hdest,    //the destination height
                               const int wsrc,     //the width of image
                               const int hsrc)     //the height of image
{
  SelectObject(memHDC, dip);

  BLENDFUNCTION bf;
  bf.BlendOp = AC_SRC_OVER;
  bf.BlendFlags = 0;
  bf.SourceConstantAlpha = 0xff; 
  bf.AlphaFormat = AC_SRC_ALPHA; 
  if (!::AlphaBlend(winHDC, posx, posy, wdest, hdest, memHDC, 0, 0, wsrc, hsrc, bf))
  {
    const DWORD errorcode = ::GetLastError();
    //::MessageBox(NULL, "errorcode", "ERROR AlphaBlend", MB_OK);
  }
}

//*******************************
// LoadCommonControlsToolTips
//
// load tool tips common controls
//*******************************
bool ttwin::LoadCommonControlsToolTips(){
  INITCOMMONCONTROLSEX initCtrls;
  initCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
  initCtrls.dwICC = ICC_TAB_CLASSES;
  return InitCommonControlsEx(&initCtrls) != FALSE;
}

//*********************************************************
// LPStringByStdString
// 
// fills an lpString (char[]) with content from std::string
//*********************************************************
char* ttwin::LpStringByStdString(const std::string& s, char* lpString, const unsigned int length){
  unsigned int i = 0;
  for (; i < s.length() && i < length; ++i) lpString[i] = s[i];
  lpString[i] = 0x00;
  return lpString;
}

//*********************************************************
// LPWStringByStdString
// 
// fills an lpWString (char[]) with content from std::string
//*********************************************************
wchar_t* ttwin::LpWStringByStdString(const std::string& s, wchar_t* lpString, const unsigned int length){
  unsigned int i = 0;
  for (; i < s.length() && i < length; ++i) lpString[i] = s[i];
  lpString[i] = 0x00;
  return lpString;
}

//*************************************************
// SplitCharZeroString
//
// splits a 0-separated char-string into std-string
//*************************************************
std::vector<std::string> ttwin::SplitCharZeroString(const char* const lpString, const unsigned int length){
  std::vector<std::string> result;
  char * const buffer = new char[length];
  std::string temp;
  for (unsigned int i = 0; i < length; ++i){
    const char c = lpString[i];
    if (c == '\0'){
      if (!temp.empty()) result.push_back(temp);
      temp = "";
    }
    else{
      temp += c;
    }
  }
  delete[] buffer;
  return result;
}

//**************************************************
// LpStringZeroByStdStringVector
// 
// builds a 0-separated char-string from std-strings 
//**************************************************
char* ttwin::LpStringZeroByStdStringVector(const std::vector<std::string>& filter, char* lpString, const unsigned int length){
  if (!filter.empty()){ 
    unsigned int pos = 0; 
    for (unsigned int i = 0; i < filter.size(); ++i){
      for (unsigned int ic = 0; ic <= filter[i].length() && pos < (length - 2); ++ic, ++pos){
        lpString[pos] = (ic < filter[i].length()) ? filter[i][ic] : '\0';
      }
    }
    lpString[pos] = '\0';
  }
  else{
    lpString[0] = '\0';
    lpString[1] = '\0';
  }
  return lpString;
}

//******************************
// StringByWString
//
// std::string from std::wstring
//******************************
std::string ttwin::StringByWString(const std::wstring& wstr){
  std::string s(wstr.begin(), wstr.end());
  return s;
}

//******************************
// WStringByString
//
// std::wstring from std::string
//******************************
std::wstring ttwin::WStringByString(const std::string& str){
  std::wstring s(str.begin(), str.end());
  return s;
}