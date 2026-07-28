#pragma once

#include <string>
#include <map>
#include <algorithm>

#include "TTImageFiles.h"
#include "TTImage.h"

namespace image {


//******************************************************************************
//*                              TTImageText                                   *
//*                                                                            *
//*                    class for write text into images                        *
//******************************************************************************
const char INDEX_FULL = 255;
const char INDEX_MISSING = INDEX_FULL;

class TTImageText {

  public:
    //constructor
    TTImageText();

    //destructorS
    ~TTImageText();

    //initializes the font 
    //at the moment direct directory 
    void InitFont(const std::string& fontDir);

    //sets the font color
    void SetColor(const image::t_proc r, const image::t_proc g, const image::t_proc b);

    //set the image to write to
    void SetImage(TTImage* image);

    //starts the writing
    void StartTextAt(const int x, const int y);

    //write text at
    void WriteText(const std::string& text);

  private:

    void ReadFontImages(const std::string& fontDir);
    void CleanUp();
    std::vector<std::string> ListFontDir(const std::string& fontDir);
    void OpenLetterImage(const std::string& fontDir, const std::string& filename, const char c);
    void WriteLetterAtCurrent(const TTImage& letterImage);


    TTImage* Image = nullptr;

    std::map<char, const TTImage*> LetterImages; 
    int LetterSpacing = 0;
    int LineSpacing = 0;
    unsigned int LetterImageX = 0; //at the moment only fixed font
    unsigned int LetterImageY = 0; 

    int StartX = 0;
    int StartY = 0;
    int CurrentX = 0;
    int CurrentY = 0;

    image::t_proc R = image::TTColorConv::ProcMax();
    image::t_proc G = image::TTColorConv::ProcMax();
    image::t_proc B = image::TTColorConv::ProcMax();
   
};


} //end of namespace image