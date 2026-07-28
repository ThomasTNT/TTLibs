#include "TTImageText.h"


//******************************************************************************
//*                              TTImageText                                   *
//*                                                                            *
//*                    class for write text into images                        *
//******************************************************************************
 
//************
// constructor
//************
image::TTImageText::TTImageText() {}

//************
// destructor
//************
image::TTImageText::~TTImageText() {
  CleanUp();
}


//********
// CleanUp
//********
void image::TTImageText::CleanUp() {
  std::for_each(LetterImages.begin(), LetterImages.end(), [](auto element) {delete element.second; });
  LetterImages.clear();
}

//*******************************
// InitFont
//
// initializes the font 
// at the moment direct directory 
//*******************************
void image::TTImageText::InitFont(const std::string& fontDir) {
  ReadFontImages(fontDir);
}

//********************
// SetColor
//
// sets the font color
//********************
void image::TTImageText::SetColor(const image::t_proc r, const image::t_proc g, const image::t_proc b) {
  R = r;
  G = g;
  B = b;
}

//**************************
// SetImage
//
// set the image to write to
//**************************
void image::TTImageText::SetImage(TTImage* image) {
  Image = image;
}


//*******************
// StartTextAt
//
// starts the writing
//*******************
void image::TTImageText::StartTextAt(const int x, const int y) {
  StartX = x;
  StartY = y;
  CurrentX = StartX;
  CurrentY = StartY;
}


//**************
// WriteText
//
// write text at
//**************
void image::TTImageText::WriteText(const std::string& text) {
  for (unsigned int i = 0; i < text.length(); ++i) {
    const char c = text.at(i);
    if (c == '\n') {
      CurrentX = StartX;
      CurrentY += LineSpacing;
    }
    else{
      const TTImage * letterImage;
      if (LetterImages.find(c) != LetterImages.end()) {
        letterImage = LetterImages.at(c);
      }
      else {
        letterImage = LetterImages.at(INDEX_MISSING);
      }
      WriteLetterAtCurrent(*letterImage);
      CurrentX += LetterSpacing;
    }
  }
}


//*********************
// WriteLetterAtCurrent
//*********************
void image::TTImageText::WriteLetterAtCurrent(const TTImage& letterImage) {
  for (unsigned int y = 0; y < LetterImageY; ++y) {
    const int yg = static_cast<int>(y) + CurrentY;
    if (yg >= 0 && yg < Image->Height()) {
      for (unsigned int x = 0; x < LetterImageX; ++x) {
        const int xg = static_cast<int>(x) + CurrentX;
        if (xg >= 0 && xg < Image->Width()) {
          Image->AddRed(xg, yg, letterImage.GetRed(x, y) * R);
          Image->AddGreen(xg, yg, letterImage.GetGreen(x, y) * G);
          Image->AddBlue(xg, yg, letterImage.GetBlue(x, y) * B);
        }
      }
    }
  }
}


//***************
// ReadFontImages
//***************
void image::TTImageText::ReadFontImages(const std::string& fontDir) {
  CleanUp();

  OpenLetterImage(fontDir, "full.tif", INDEX_FULL);
  if (LetterImages.find(INDEX_FULL) != LetterImages.end()) {
    const TTImage * const image = LetterImages.at(INDEX_FULL);
    LetterImageX = image->Width();
    LetterImageY = image->Height();
    LetterSpacing = LetterImageX;
    LineSpacing = LetterImageY + 5;
  }
  else {
    //throw Exception
  }

  OpenLetterImage(fontDir, "colon.tif", ':');
  OpenLetterImage(fontDir, "dot.tif", '.');
  OpenLetterImage(fontDir, "minus.tif", '-');
  OpenLetterImage(fontDir, "plus.tif", '+');
  OpenLetterImage(fontDir, "slash.tif", '/');
  OpenLetterImage(fontDir, "under.tif", '_');
  OpenLetterImage(fontDir, "and.tif", '&');
  OpenLetterImage(fontDir, "bracketround0.tif", '(');
  OpenLetterImage(fontDir, "bracketround1.tif", ')');
  OpenLetterImage(fontDir, "comma.tif", ',');
  OpenLetterImage(fontDir, "equals.tif", '=');
  OpenLetterImage(fontDir, "greater.tif", '>');
  OpenLetterImage(fontDir, "percent.tif", '%');
  OpenLetterImage(fontDir, "question.tif", '?');
  OpenLetterImage(fontDir, "smaller.tif", '<');
  OpenLetterImage(fontDir, "explamation.tif", '!');
  OpenLetterImage(fontDir, "hash.tif", '#');
  OpenLetterImage(fontDir, "pipe.tif", '|');
  OpenLetterImage(fontDir, "semicolon.tif", ';');
  OpenLetterImage(fontDir, "space.tif", ' ');
  OpenLetterImage(fontDir, "asterix.tif", '*');
  OpenLetterImage(fontDir, "quotes.tif", '"');

  OpenLetterImage(fontDir, "A.tif", 'A');
  OpenLetterImage(fontDir, "B.tif", 'B');
  OpenLetterImage(fontDir, "C.tif", 'C');
  OpenLetterImage(fontDir, "D.tif", 'D');
  OpenLetterImage(fontDir, "E.tif", 'E');
  OpenLetterImage(fontDir, "F.tif", 'F');
  OpenLetterImage(fontDir, "G.tif", 'G');
  OpenLetterImage(fontDir, "H.tif", 'H');
  OpenLetterImage(fontDir, "I.tif", 'I');
  OpenLetterImage(fontDir, "J.tif", 'J');
  OpenLetterImage(fontDir, "K.tif", 'K');
  OpenLetterImage(fontDir, "L.tif", 'L');
  OpenLetterImage(fontDir, "M.tif", 'M');
  OpenLetterImage(fontDir, "N.tif", 'N');
  OpenLetterImage(fontDir, "O.tif", 'O');
  OpenLetterImage(fontDir, "P.tif", 'P');
  OpenLetterImage(fontDir, "Q.tif", 'Q');
  OpenLetterImage(fontDir, "R.tif", 'R');
  OpenLetterImage(fontDir, "S.tif", 'S');
  OpenLetterImage(fontDir, "T.tif", 'T');
  OpenLetterImage(fontDir, "U.tif", 'U');
  OpenLetterImage(fontDir, "V.tif", 'V');
  OpenLetterImage(fontDir, "W.tif", 'W');
  OpenLetterImage(fontDir, "X.tif", 'X');
  OpenLetterImage(fontDir, "Y.tif", 'Y');
  OpenLetterImage(fontDir, "Z.tif", 'Z');
  OpenLetterImage(fontDir, "Ä.tif", 'Ä');
  OpenLetterImage(fontDir, "Ö.tif", 'Ö');
  OpenLetterImage(fontDir, "Ü.tif", 'Ü');

  OpenLetterImage(fontDir, "a_.tif", 'a');
  OpenLetterImage(fontDir, "b_.tif", 'b');
  OpenLetterImage(fontDir, "c_.tif", 'c');
  OpenLetterImage(fontDir, "d_.tif", 'd');
  OpenLetterImage(fontDir, "e_.tif", 'e');
  OpenLetterImage(fontDir, "f_.tif", 'f');
  OpenLetterImage(fontDir, "g_.tif", 'g');
  OpenLetterImage(fontDir, "h_.tif", 'h');
  OpenLetterImage(fontDir, "i_.tif", 'i');
  OpenLetterImage(fontDir, "j_.tif", 'j');
  OpenLetterImage(fontDir, "k_.tif", 'k');
  OpenLetterImage(fontDir, "l_.tif", 'l');
  OpenLetterImage(fontDir, "m_.tif", 'm');
  OpenLetterImage(fontDir, "n_.tif", 'n');
  OpenLetterImage(fontDir, "o_.tif", 'o');
  OpenLetterImage(fontDir, "p_.tif", 'p');
  OpenLetterImage(fontDir, "q_.tif", 'q');
  OpenLetterImage(fontDir, "r_.tif", 'r');
  OpenLetterImage(fontDir, "s_.tif", 's');
  OpenLetterImage(fontDir, "t_.tif", 't');
  OpenLetterImage(fontDir, "u_.tif", 'u');
  OpenLetterImage(fontDir, "v_.tif", 'v');
  OpenLetterImage(fontDir, "w_.tif", 'w');
  OpenLetterImage(fontDir, "x_.tif", 'x');
  OpenLetterImage(fontDir, "y_.tif", 'y');
  OpenLetterImage(fontDir, "z_.tif", 'z');
  OpenLetterImage(fontDir, "ä_.tif", 'ä');
  OpenLetterImage(fontDir, "ö_.tif", 'ö');
  OpenLetterImage(fontDir, "ü_.tif", 'ü');

  OpenLetterImage(fontDir, "1.tif", '1');
  OpenLetterImage(fontDir, "2.tif", '2');
  OpenLetterImage(fontDir, "3.tif", '3');
  OpenLetterImage(fontDir, "4.tif", '4');
  OpenLetterImage(fontDir, "5.tif", '5');
  OpenLetterImage(fontDir, "6.tif", '6');
  OpenLetterImage(fontDir, "7.tif", '7');
  OpenLetterImage(fontDir, "8.tif", '8');
  OpenLetterImage(fontDir, "9.tif", '9');
  OpenLetterImage(fontDir, "0.tif", '0');

}


//****************
// OpenLetterImage
//****************
void image::TTImageText::OpenLetterImage(const std::string& fontDir, const std::string& filename, const char c){
  const std::string filepath = fontDir + filename;
  image::TTFileImage * const inImage = image::GetTTFileImageByEnding(filepath);
  if (!inImage->Open(filepath)) {
    std::cerr << "Could not open image " << filepath << "!" << std::endl;
    inImage->Close();
    delete inImage;
    return;
  }
  LetterImages[c] = inImage;
}


//************
// ListFontDir
//************
std::vector<std::string> image::TTImageText::ListFontDir(const std::string& fontDir) {
  //later
  return std::vector<std::string>();
}

