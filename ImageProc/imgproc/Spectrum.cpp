#include "Spectrum.h"

//***********************************************************
//*                        Spectrum                         *
//*                                                         *
//*        class for colorconversion RGB->Spectrum->RGB     *
//***********************************************************

//************
// constructor
//************
imgproc::Spectrum::Spectrum() : Resolution(0), debug(false)
{
}

//************
// destructor
//************
imgproc::Spectrum::~Spectrum(){}

//****************************************
// SpectralResolution
//
// returns the special spectral resolution
//****************************************
unsigned int imgproc::Spectrum::SpectralResolution() const {
  return Resolution;
}

//*************************
// FindDomColorVector
//
// finds dominating colors
//*************************
template <class T>
void imgproc::Spectrum::FindDomColorVector(T r, T g, T b,
                                           unsigned int &color1,
                                           T            &factor1,
                                           unsigned int &color2,
                                           T            &factor2,
                                           T            &white)
{

  //--------- remove white ----------
  white = r < g ? r : g;
  if (b < white) white = b;
  r -= white;
  g -= white;
  b -= white;

  //--------- primary and secondary color ---------

  //first color red? 
  if (r >= g && r >= b) {
    //second color green?
    if (g > b) {
      //red
      if ((r - g) > g) {
        color1 = 0; //red
        factor1 = r;
        color2 = 2; //green
        factor2 = g;
      }
      //yellow
      else {
        color1 = 1; //yellow
        factor1 = g;
        color2 = 0; //red 
        factor2 = r - g;
      }
    }
    //second color blue?
    else {
      //red 
      if ((r - b / 2) > b) {
        color1 = 0; //red
        factor1 = r;
        color2 = 4; //blue
        factor2 = b;
      }
      else {
        color1 = 5; //violett
        if (r > b / 2) {
          color2 = 0; //red
          factor1 = b;
          factor2 = r - b / 2;
        }
        else {
          color2 = 4; //blue
          factor1 = r * 2;
          factor2 = b - r * 2;
        }
      }
    }
  }
  //first color green? 
  else if (g >= r && g >= b) {
    //second color red?
    if (r > b) {
      //green
      if ((g - r) > r) {
        color1 = 2; //green
        factor1 = g;
        color2 = 0; //red
        factor2 = r;
      }
      //yellow
      else {
        color1 = 1; //yellow
        factor1 = r;
        color2 = 2; //green
        factor2 = g - r;
      }
    }
    //second color blue?
    else {
      //green
      if ((g - b) > b) {
        color1 = 2; //green
        factor1 = g;
        color2 = 4; //blue
        factor2 = b;
      }
      //cyan
      else {
        color1 = 3; //cyan
        factor1 = b;
        color2 = 2; //green
        factor2 = g - b;
      }
    }
  }
  //first color blue
  else {
    //second color red? 
    if (r > g) {
      if ((b - r / 2) > r / 2) {
        color1 = 4; //blue
        factor1 = b;
        color2 = 0; //red
        factor2 = r;
      }
      else {
        color1 = 6; //violett
        if (r > b / 2) {
          color2 = 0; //red
          factor1 = b;
          factor2 = r - b / 2;
        }
        else {
          color2 = 5; //blue
          factor1 = r * 2;
          factor2 = b - r * 2;
        }
      }
    }
    //second color green
    else {
      //blue
      if ((b - g) > g) {
        color1 = 4; //blue
        factor1 = b;
        color2 = 2; //green
        factor2 = g;
      }
      else {
        color1 = 3; //cyan
        factor1 = g;
        color2 = 4; //blue
        factor2 = b - g;
      }
    }
  }
}


//***************************
// STATIC: SpectrumFromRGB
//
// converst RGB into spectrum
// TODO: integer values!!!!!!
//***************************
std::vector<double> imgproc::Spectrum::SpectrumFromRGB(const double r, const double g, const double b){
  
	unsigned int color1, color2;
	double factor1, factor2; 
  double white;

  //spectral analysis
  SpectralFactorsFromRGB(r, g, b, color1, factor1, color2, factor2, white);
	
	const double* spectrum1 = &RGBSPECTRUMS[color1 * WAVELENGTH_SIZE];
	const double* spectrum2 = &RGBSPECTRUMS[color2 * WAVELENGTH_SIZE];
	
  //add all part spectrums to resulting spectrum and put it into the vector
  std::vector<double> spectrum;
  double value;
  for (unsigned i = 0; i < WAVELENGTH_SIZE; i++){
    value = spectrum1[i] * factor1 + spectrum2[i] * factor2 + white;
    spectrum.push_back(value);
  }

  return spectrum;
}
    
//***************************************************************
// STATIC: RGBFromSprectrum
//
// creates RGB from spectrum by using the color mathing functions
// TODO: integer values!!!!!!
//***************************************************************
void imgproc::Spectrum::RGBFromSpectrum(const std::vector<double> spectrum, double &r, double &g, double &b){
  r = Integral(spectrum, COLORMATCHING_WEIGHTED_R);
  g = Integral(spectrum, COLORMATCHING_WEIGHTED_G);
  b = Integral(spectrum, COLORMATCHING_WEIGHTED_B);
}

//*************************************************************************
// STATIC: RGBFromWavelength
//
// creates RGB from a given wavelength by using the color mathing functions
// R, G or B may be negative!!!!
// TODO: integer values!!!!!!
//*************************************************************************
void imgproc::Spectrum::RGBFromWavelength(double lambda, double &r, double &g, double &b){
  const unsigned int index = static_cast<unsigned int>(WAVELENGTH_SIZE * (lambda - WAVELENGTH_MIN) / (WAVELENGTH_MAX - WAVELENGTH_MIN) + 0.5);
  r = COLORMATCHING_WEIGHTED_R[index];
  g = COLORMATCHING_WEIGHTED_G[index];
  b = COLORMATCHING_WEIGHTED_B[index];
}

//*************************************************************************
// STATIC: RGBFromWavelength
//
// creates RGB from a given wavelength by using the color mathing functions
// R, G or B may be negative!!!!
// TODO: integer values!!!!!!
//*************************************************************************
void imgproc::Spectrum::RGBFromWavelengthIndex(const unsigned int index, double &r, double &g, double &b){
  r = COLORMATCHING_WEIGHTED_R[index];
  g = COLORMATCHING_WEIGHTED_G[index];
  b = COLORMATCHING_WEIGHTED_B[index];
}

//****************************************
// STATIC: Integral
//
// apllies a weight function to a spectrum
// TODO: integer values!!!!!!
//****************************************
double imgproc::Spectrum::Integral(const std::vector<double> spectrum, const double* weigthfunction){
  double result = 0;
  for (unsigned i = 0; i < WAVELENGTH_SIZE; i++){
    result += spectrum[i] * weigthfunction[i];
  }
  return result;
}

//------------------------- special functions for other resolution -----------------------------

//*********************************************************
// InitSpecialResolution
//
// initialize lookup tables for other spectral distribution
//*********************************************************
void imgproc::Spectrum::InitSpecialResolution(unsigned int size) {

  if (size == 0) size = 1;
  if (size > MAX_SPECTRAL_RESOLUTION) size = MAX_SPECTRAL_RESOLUTION;

  if (size != Resolution) {

    //initalize arrays
    Resolution = size;

    //resample color matching functions
    std::vector<float> colorMatchingRed;
    for (unsigned int w = 0; w < WAVELENGTH_SIZE; ++w) colorMatchingRed.push_back(static_cast<float>(COLORMATCHING_WEIGHTED_R[w]));
    SpecialColorMatching_Red = mathutil::ResampleVector<float>(colorMatchingRed, Resolution);

    std::vector<float> colorMatchingGreen;
    for (unsigned int w = 0; w < WAVELENGTH_SIZE; ++w) colorMatchingGreen.push_back(static_cast<float>(COLORMATCHING_WEIGHTED_G[w]));
    SpecialColorMatching_Green = mathutil::ResampleVector<float>(colorMatchingGreen, Resolution);

    std::vector<float> colorMatchingBlue;
    for (unsigned int w = 0; w < WAVELENGTH_SIZE; ++w) colorMatchingBlue.push_back(static_cast<float>(COLORMATCHING_WEIGHTED_B[w]));
    SpecialColorMatching_Blue = mathutil::ResampleVector<float>(colorMatchingBlue, Resolution);

    //resampled spectrums
    const float factor = static_cast<float>(Resolution) / static_cast<float>(WAVELENGTH_SIZE);
    for (unsigned int spectrumIndex = 0; spectrumIndex < NUMBER_OF_SPECTRUMS; spectrumIndex++) {
      std::vector<float> spectrum;
      for (unsigned int w = 0; w < WAVELENGTH_SIZE; ++w) {
        spectrum.push_back(static_cast<float>(RGBSPECTRUMS[spectrumIndex * WAVELENGTH_SIZE + w]) * factor);
      }
      std::vector<float> resampledSpectrum = mathutil::ResampleVector<float>(spectrum, Resolution);
      SpecialSpectrums.push_back(resampledSpectrum);
    }
  }
}


//*********************************************************
// InitSpecialResolutionInt
//
//
// initialize lookup tables for other spectral distribution
//*********************************************************
void imgproc::Spectrum::InitSpecialResolutionInt(unsigned int size){

  if (size == 0) size = 1;
  if (size > MAX_SPECTRAL_RESOLUTION) size = MAX_SPECTRAL_RESOLUTION;

  if (size != Resolution){
    
    //initalize arrays
    Resolution = size;
    
    //copy content for all sub functions
    const double amountOut = static_cast<double>(WAVELENGTH_SIZE) / static_cast<double>(Resolution);
    double freeOut = 0;  //indicates how much has to be copied until the out slot/index is full
    double restIn  = 0;  //indicates how much is not coiped yet from the current in-index
    double amountToCopy; //what is currently to copied from in-index to out-index 
    int    inIndex = -1; 
    for (unsigned int outIndex = 0; outIndex < Resolution; outIndex++){
      
      //---------- start new out entry -------------
      SpecialColorMatchingsInt[outIndex                 ] = 0; //colormatching function red
      SpecialColorMatchingsInt[outIndex +     Resolution] = 0; //colormatching function green
      SpecialColorMatchingsInt[outIndex + 2 * Resolution] = 0; //colormatching function blue
      for (unsigned int spectrumIndex = 0; spectrumIndex < NUMBER_OF_SPECTRUMS; spectrumIndex++){
        SpecialSpectrumsInt[outIndex + spectrumIndex * Resolution] = 0; // a spectrum is never negative
      }
      freeOut = amountOut;
    
      //as long as there is something to copy
      while (freeOut > 0){

        //-------- start new input if there is no value left ---------
        if (restIn <= 0){
          ++inIndex;
          restIn = 1.0;
        }
        //how much from inIndex has to be copied to the out array
        amountToCopy = freeOut < restIn ? freeOut : restIn;
        
        //----------- copy the values from input to output ------------
        SpecialColorMatchingsInt[outIndex                 ] += static_cast<U32>(amountToCopy * COLORMATCHING_WEIGHTED_R[inIndex] * 0x8000 + 0.5);
        SpecialColorMatchingsInt[outIndex +     Resolution] += static_cast<U32>(amountToCopy * COLORMATCHING_WEIGHTED_G[inIndex] * 0x8000 + 0.5);
        SpecialColorMatchingsInt[outIndex + 2 * Resolution] += static_cast<U32>(amountToCopy * COLORMATCHING_WEIGHTED_B[inIndex] * 0x8000 + 0.5);
        for (unsigned int spectrumIndex = 0; spectrumIndex < NUMBER_OF_SPECTRUMS; spectrumIndex++){
          SpecialSpectrumsInt[outIndex + spectrumIndex * Resolution] += static_cast<U32>(amountToCopy * RGBSPECTRUMS[inIndex + spectrumIndex * WAVELENGTH_SIZE] * 32768.0 / amountOut  + 0.5);
        }
        freeOut -= amountToCopy;
        restIn  -= amountToCopy;
      }

    } //end of for over all new indices
  } //end of main if

}

//**********************************************************
// ShortenRedSpecialResolution
// 
// shortens the red side of the spektrum
// don't use this for smearing the spektrum, but it shortens 
// calculation time when simply adding 
// the spéktrum. Die Red-side is dark red and very long
//**********************************************************
void imgproc::Spectrum::ShortenRedSpecialResolution(){
  if (Resolution == 0) return;


}

//**************************
// SpectrumFromRGB
//
// /RGB -> special spectrum
//**************************
std::vector<float> imgproc::Spectrum::SpecialSpectrumFromRGB(const float r, const float g, const float b) const {

  unsigned int color11, color12, color21, color22; //color indices (indices of partial sprectrum) 
  float factor11, factor12, factor21, factor22;    //color factors
  float white;                                     //white factor

  SpectralFactorsFromRGB(r, g, b, color11, factor11, color12, factor12, color21, factor21, color22, factor22, white);
  
  //result will be written into CurrSpecialSpectrum 
  return SpecialSpectrumFromFactors(color11, factor11, color12, factor12, color21, factor21, color22, factor22, white);
}


//***************************
// SpectrumFromRGB DEPRECATED
//
// /RGB -> special spectrum
//***************************
std::vector<U32> imgproc::Spectrum::SpecialSpectrumFromRGBInt(const U32 r, const U32 g, const U32 b) const {

  unsigned int color11, color12, color21, color22; //color indices (indices of partial sprectrum) 
  U32 factor11, factor12, factor21, factor22;      //color factors
  U32 white;                                       //white factor
  
  SpectralFactorsFromRGBInt(r, g, b, color11, factor11, color12, factor12, color21, factor21, color22, factor22, white);
  
  //result will be written into CurrSpecialSpectrum 
  return SpecialSpectrumFromFactorsInt(color11, factor11, color12, factor12, color21, factor21, color22, factor22, white);
}


//*****************************
// SpecialWavelengthFromFactors
//
// factors -> special spectrum
//*****************************
std::vector<float> imgproc::Spectrum::SpecialSpectrumFromFactors(unsigned int c11,
                                                                 float        f11,
                                                                 unsigned int c12,
                                                                 float        f12,
                                                                 unsigned int c21,
                                                                 float        f21,
                                                                 unsigned int c22,
                                                                 float        f22,
                                                                 float          w) const
{
  const std::vector<float>& spectrum11 = SpecialSpectrums[c11];
  const std::vector<float>& spectrum12 = SpecialSpectrums[c12];
  const std::vector<float>& spectrum21 = SpecialSpectrums[c21];
  const std::vector<float>& spectrum22 = SpecialSpectrums[c22];

  //add all part spectrums to resulting spectrum and put it into the vector
  std::vector<float> result;
  for (unsigned int i = 0; i < Resolution; i++) {
    const float value = spectrum11[i] * f11
                      + spectrum12[i] * f12
                      + spectrum21[i] * f21
                      + spectrum22[i] * f22
                      + w;
    result.push_back(value);
  }

  return result;
}

//*****************************
// SpecialWavelengthFromFactors
//
// DEPRECATED
// factors -> special spectrum
//*****************************
std::vector<U32> imgproc::Spectrum::SpecialSpectrumFromFactorsInt(unsigned int c11,
                                                                 U32          f11,
                                                                 unsigned int c12,
                                                                 U32          f12,
                                                                 unsigned int c21,
                                                                 U32          f21,
                                                                 unsigned int c22,
                                                                 U32          f22,
                                                                 U32          w) const
{
  
  const U32* spectrum11 = &SpecialSpectrumsInt[c11 * Resolution];
	const U32* spectrum12 = &SpecialSpectrumsInt[c12 * Resolution];
  const U32* spectrum21 = &SpecialSpectrumsInt[c21 * Resolution];
	const U32* spectrum22 = &SpecialSpectrumsInt[c22 * Resolution];
  
  //add all part spectrums to resulting spectrum and put it into the vector
  std::vector<U32> result;
  for (unsigned int i = 0; i < Resolution; i++){
    U32 value =   spectrum11[i] < 0x10000
                ? ((spectrum11[i] * f11) >> 15)
                : (((spectrum11[i] >> 8) * f11) >> 7);
                           
    value +=   spectrum12[i] < 0x10000
             ? ((spectrum12[i] * f12) >> 15)
             : (((spectrum12[i] >> 8) * f12) >> 7);
   
    value +=   spectrum21[i] < 0x10000
             ? ((spectrum21[i] * f21) >> 15)
             : (((spectrum21[i] >> 8) * f21) >> 7);
                           
    value +=   spectrum22[i] < 0x10000
             ? ((spectrum22[i] * f22) >> 15)
             : (((spectrum22[i] >> 8) * f22) >> 7);
                                                      
    value +=  w;
    result.push_back(value);
  }
  
  return result;
}


//*******************************************************************
// SpecialWavelengthFromFactors
//
// factors -> special spectral intensity
//
// just like SpecialSpectrumFromFactors but only one wavelength index
//*******************************************************************
U32 imgproc::Spectrum::SpectralIntensityFromFactors(const unsigned int c11,
                                                    const U32          f11,
                                                    const unsigned int c12,
                                                    const U32          f12,
                                                    const unsigned int c21,
                                                    const U32          f21,
                                                    const unsigned int c22,
                                                    const U32          f22,
                                                    const U32          w,
                                                    const unsigned int index) const
{
  
  U32 spectralIntensity = SpecialSpectrumsInt[c11 * Resolution + index];
  U32 result =     spectralIntensity < 0x10000
                ? ((spectralIntensity * f11) >> 15)
                : (((spectralIntensity >> 8) * f11) >> 7);
                           
  spectralIntensity = SpecialSpectrumsInt[c12 * Resolution + index];                                                   
  result +=   spectralIntensity < 0x10000
            ? ((spectralIntensity * f12) >> 15)
            : (((spectralIntensity >> 8) * f12) >> 7);
   
  spectralIntensity = SpecialSpectrumsInt[c21 * Resolution + index];
  result +=   spectralIntensity < 0x10000
            ? ((spectralIntensity * f21) >> 15)
            : (((spectralIntensity >> 8) * f21) >> 7);
        
  spectralIntensity = SpecialSpectrumsInt[c22 * Resolution + index];             
  result +=   spectralIntensity < 0x10000
            ? ((spectralIntensity * f22) >> 15)
            : (((spectralIntensity >> 8) * f22) >> 7);
                                          
  result +=  w;
  
  return result;
}

//*************************************************************************
// RGBFromWavelength
//
// creates RGB from a given wavelength by using the color mathing functions
// R, G or B may be negative
//*************************************************************************
void imgproc::Spectrum::RGBFromSpecialWavelengthIndex(const unsigned int index, float &r, float &g, float &b) const {
  r = SpecialColorMatching_Red[index];
  g = SpecialColorMatching_Green[index];
  b = SpecialColorMatching_Blue[index];
}

//*************************************************************************
// RGBFromWavelength
//
// creates RGB from a given wavelength by using the color mathing functions
// R, G or B may be negative!!!!
// DEPRECATED
//*************************************************************************
void imgproc::Spectrum::RGBFromSpecialWavelengthIndexInt(const unsigned int index, S32 &r, S32 &g, S32 &b) const {
  r = SpecialColorMatchingsInt[index                 ];
  g = SpecialColorMatchingsInt[index +     Resolution];
  b = SpecialColorMatchingsInt[index + 2 * Resolution];
}

//*********************************************************
// STATIC: SpectralFactorsFromRGB 
//
// calculates the spectral factors from the given RGB value
// integer variant
//*********************************************************
void imgproc::Spectrum::SpectralFactorsFromRGB(const float r, const float g, const float b,
                                               unsigned int &c11,
                                               float        &f11,
                                               unsigned int &c12,
                                               float        &f12,
                                               unsigned int &c21,
                                               float        &f21,
                                               unsigned int &c22,
                                               float        &f22,
                                               float        &w) 
{

  //find both dominating partial spectrums
  FindDomColorVector(r, g, b, c11, f11, c21, f21, w);

  //handle white 
  const float whitelevels[] = { 0.1f, 0.2f, 0.5f, 0.7f };
  const float reducedColor[] = { 0.9f, 0.8f, 0.5f, 0.3f }; //1.0 - whitelevel 

  //SIMPLE CASE: pure white
  if (f11 == 0.0f && f21 == 0.0f) {
    f11 = 0.0f;
    c11 = 0;
    c12 = 0;
    f12 = 0.0f;
    c21 = 0;
    f21 = 0.0f;
    c22 = 0;
    f22 = 0.0f;
    return;
  }

  //SIMPLE CASE: pure color 
  if (w == 0.0f) {
    c12 = 0;
    f12 = 0.0f;
    c22 = 0;
    f22 = 0.0f;
    return;
  }

  //look for the lower white level
  // 0 means full saturateted color
  // 1 means 0.1 white etc...
  unsigned int whiteIndexLower = 0;
  for (unsigned int i = 0; i < 4; i++) {
    const float additionalWhite = (f11 + f21) * whitelevels[i] / reducedColor[i];
    if (additionalWhite > w) break;
    whiteIndexLower = i + 1;
  }

  //factors of the "lower color" (means color with less white) 
  const float factor1Lower = whiteIndexLower > 0 ? f11 / reducedColor[whiteIndexLower - 1] : f11;
  const float factor2Lower = whiteIndexLower > 0 ? f21 * reducedColor[whiteIndexLower - 1] : f21;

  //the additional white with the "lower color" (means color with less white) 
  const float additionalWhiteLower = whiteIndexLower > 0 ? whitelevels[whiteIndexLower - 1] * (factor1Lower + factor2Lower) : 0;

  //SIMPLE CASE:    
  //very white color, but not pure white
  //-> use color with maximum white and add pure white
  if (whiteIndexLower >= 4) {
    c11 += (whiteIndexLower) * 6;
    f11 = factor1Lower;
    c12 = 0;
    f12 = 0.0f;
    c21 += (whiteIndexLower) * 6;
    f21 = factor2Lower;
    c22 = 0;
    f22 = 0.0f;
    w -= additionalWhiteLower;
    return;
  }

  //factors of the "higher color" (color with more white than needed)
  const float factor1Higher = f11 / reducedColor[whiteIndexLower];
  const float factor2Higher = f21 / reducedColor[whiteIndexLower];

  //the additional white with the "higher color" (color with more white than needed)
  const float additionalWhiteHigher = whitelevels[whiteIndexLower] * (factor1Higher + factor2Higher);

  const float additionalWhiteDiff = additionalWhiteHigher - additionalWhiteLower;
  const float blendFactorLower = additionalWhiteDiff > 0.0f ? (additionalWhiteHigher - w) / additionalWhiteDiff : 1.0f;
  const float blendFactorHigher = 1.0f - blendFactorLower;

  //set the factors
  f11 = factor1Lower * blendFactorLower;
  f12 = factor1Higher * blendFactorHigher;
  f21 = factor2Lower * blendFactorLower;
  f22 = factor2Higher * blendFactorHigher;

  //estimate the remaining white level by subtracting the partial whites
  //indeed it should be zero
  float whiteSubtrahend = whiteIndexLower > 0.0f ? f11 * whitelevels[whiteIndexLower - 1] : 0.0f;
  if (w > whiteSubtrahend) {
    w -= whiteSubtrahend;
    whiteSubtrahend = f12 * whitelevels[whiteIndexLower];
    if (w > whiteSubtrahend) {
      w -= whiteSubtrahend;
      whiteSubtrahend = whiteIndexLower > 0 ? f21 * whitelevels[whiteIndexLower - 1] : 0;
      if (w > whiteSubtrahend) {
        w -= whiteSubtrahend;
        whiteSubtrahend = f22 * whitelevels[whiteIndexLower];
        if (w > whiteSubtrahend) {
          w -= whiteSubtrahend;
        }
        else {
          w = 0.0f;
        }
      }
      else {
        w = 0.0f;
      }
    }
    else {
      w = 0.0f;
    }
  }
  else {
    w = 0.0f;
  }

  //set actual color indicies
  c11 += (whiteIndexLower) * 6;
  c12 = c11 + 6;
  c21 += (whiteIndexLower) * 6;
  c22 = c21 + 6;

}

//*********************************************************
// STATIC: SpectralFactorsFromRGB Integervariante
//
// DEPRECATED
//
// calculates the spectral factors from the given RGB value
// integer variant
//*********************************************************
void imgproc::Spectrum::SpectralFactorsFromRGBInt(const U32 r, const U32 g, const U32 b,
                                                  unsigned int &c11,
                                                  U32          &f11,
                                                  unsigned int &c12,
                                                  U32          &f12,
                                                  unsigned int &c21,
                                                  U32          &f21,
                                                  unsigned int &c22,
                                                  U32          &f22,
                                                  U32          &w)   //0...1 -> 0 ...0x8000
{
  
  //find both dominating partial spectrums
  FindDomColorVector(r, g, b, c11, f11, c21, f21, w);
  
  //handle white 
  const U32 whitelevels[]            = {3277, 6554, 16384, 22938}; //{0.1, 0.2, 0.5, 0.7};
  //const U32 reducedColor[]           = {29491, 26214, 16384, 9830}; //{0.9, 0.8, 0.5, 0.3}; //1.0 - whitelevel
	const U32 reciprocalReducedColor[] = {36409, 40960, 65536, 109227}; //{1.11111111111, 1.25, 2.0, 3.33333333333}; 
   
  //SIMPLE CASE: pure white
  if (f11 == 0 && f21 == 0){
    f11 = 0;
    c12 = 0;
    f12 = 0;
    c21 = 0;
    c22 = 0;
    f22 = 0;
    return;
  }
  
  //SIMPLE CASE: pure color 
  if (w == 0){
    c12 = 0;
    f12 = 0;
    c22 = 0;
    f22 = 0;
    return;
  } 
  
  //look for the lower white level
  // 0 means full saturateted color
  // 1 means 0.1 white etc...
  unsigned int whiteIndexLower = 0;
  U32 additionalWhite;
  for (unsigned int i = 0; i < 4; i++){
    additionalWhite = ((((f11 + f21) * reciprocalReducedColor[i]) >> 15) * whitelevels[i]) >> 15;
    if (additionalWhite > w) break;
    whiteIndexLower = i + 1;
  }

  //factors of the "lower color" (means color with less white) 
  const U32 factor1Lower = whiteIndexLower > 0 ? ((f11 * reciprocalReducedColor[whiteIndexLower - 1]) >> 15) : f11;
  const U32 factor2Lower = whiteIndexLower > 0 ? ((f21 * reciprocalReducedColor[whiteIndexLower - 1]) >> 15) : f21;

  //the additional white with the "lower color" (means color with less white) 
  const U32 additionalWhiteLower = whiteIndexLower > 0 ? ((whitelevels[whiteIndexLower - 1] * (factor1Lower + factor2Lower)) >> 15) : 0;
     
  //SIMPLE CASE:    
  //very white color, but not pure white
  //-> use color with maximum white and add pure white
  if (whiteIndexLower >= 4){
    c11 += (whiteIndexLower) * 6;
    f11 = factor1Lower;
    c12 = 0; 
    f12 = 0;
    c21 += (whiteIndexLower) * 6;
    f21 = factor2Lower;
    c22 = 0; 
    f22 = 0;
    w -= additionalWhiteLower;
    return;
  }

  //factors of the "higher color" (color with more white than needed)
  const U32 factor1Higher = (f11 * reciprocalReducedColor[whiteIndexLower]) >> 15;
  const U32 factor2Higher = (f21 * reciprocalReducedColor[whiteIndexLower]) >> 15;

  //the additional white with the "higher color" (color with more white than needed)
  const U32 additionalWhiteHigher = (whitelevels[whiteIndexLower] * (factor1Higher + factor2Higher)) >> 15;

  const U32 additionalWhiteDiff = additionalWhiteHigher - additionalWhiteLower;
  const U32 blendFactorLower = additionalWhiteDiff > 0 ? 0x100 * (additionalWhiteHigher - w) / additionalWhiteDiff : 0x100;
  const U32 blendFactorHigher = 0x100 - blendFactorLower;

  //set the factors
  f11 =  (factor1Lower * blendFactorLower)  >> 8;
  f12 = (factor1Higher * blendFactorHigher) >> 8;
  f21 =  (factor2Lower * blendFactorLower)  >> 8;
  f22 = (factor2Higher * blendFactorHigher) >> 8;

  //estimate the remaining white level by subtracting the partial whites
  //indeed it should be zero
  U32 whiteSubtrahend = whiteIndexLower > 0 ? (f11 * whitelevels[whiteIndexLower - 1]) >> 15 : 0;
  if (w > whiteSubtrahend){
    w -= whiteSubtrahend;
    whiteSubtrahend = (f12 * whitelevels[whiteIndexLower]) >> 15;
    if (w > whiteSubtrahend){
      w -= whiteSubtrahend;
      whiteSubtrahend = whiteIndexLower > 0 ? (f21 * whitelevels[whiteIndexLower - 1]) >> 15 : 0;
      if (w > whiteSubtrahend){
        w -= whiteSubtrahend;
        whiteSubtrahend = (f22 * whitelevels[whiteIndexLower]) >> 15;
        if (w > whiteSubtrahend){
          w -= whiteSubtrahend;
        }
        else{
          w = 0; 
        }
      }
      else{
        w = 0; 
      }
    }
    else{
      w = 0; 
    }
  }
  else{
    w = 0; 
  }

  //set actual color indicies
  c11 += (whiteIndexLower) * 6;
  c12 = c11 + 6;
  c21 += (whiteIndexLower) * 6;
  c22 = c21 + 6;

}
  

//-------------------------------------- private functions  -------------------------------------

//*********************************************************
// STATIC: SpectralFactorsFromRGB
//
// calculates the spectral factors from the given RGB value
// float values
//*********************************************************
inline void imgproc::Spectrum::SpectralFactorsFromRGB(const double r, const double g, const double b,
                                                      unsigned int &color1, 
                                                      double       &factor1, 
                                                      unsigned int &color2, 
                                                      double       &factor2, 
                                                      double       &white)
{
  
  //find both dominating partial spectrums
  FindDomColorVector(r, g, b, color1, factor1, color2, factor2, white);
  
	//handle white 
	const double whitelevels[] = {0.1, 0.2, 0.5, 0.7};
	const double reducedColor[] = {0.9, 0.8, 0.5, 0.3}; //1.0 - whitelevel
	
	unsigned int white1 = 0;
	unsigned int white2 = 0;
	double newfactor;
	double newfactor1 = factor1;
	double newfactor2 = factor2;
	
	double additionalWhite;
	
	for (unsigned int i = 0; i < 4; i++){
		newfactor = factor1 / reducedColor[i];
		additionalWhite = newfactor * whitelevels[i];
		if (additionalWhite > white) break;
		white1 = i + 1;
		newfactor1 = newfactor;
		newfactor = factor2 / reducedColor[i];
		additionalWhite = additionalWhite + newfactor * whitelevels[i];
		if (additionalWhite > white) break;
		white2 = i + 1;
		newfactor2 = newfactor;
  }

	//reduce pure white
	if (white1 > 0) white -= whitelevels[white1 - 1] * newfactor1;
	if (white2 > 0) white -= whitelevels[white2 - 1] * newfactor2;
	
	//get real color vectsor indices and set results
	color1 = color1 + white1 * 6;
	color2 = color2 + white2 * 6;
  factor1 = newfactor1;
  factor2 = newfactor2;
}



//****************************************************************************
// STATIC: FindDomColorVector
//
// function searches the best matching partial spectrum to build the given RGB
// HINT: works for both integer and float values the same
//****************************************************************************
//DEFINTION IS IN HEADER FILE, BECAUSE OF TEMPLATE


//--------------------------------------- older versions ---------------------------------------

//**************************
// SpectrumFromRGB
//
// /RGB -> special spectrum
//**************************
std::vector<U32> imgproc::Spectrum::SpecialSpectrumFromRGBOld(const U32 r, const U32 g, const U32 b){

	unsigned int color1, color2;
	U32 factor1, factor2; 
  U32 white;

  //spectral analysis
  SpectralFactorsFromRGBIntOld(r, g, b, color1, factor1, color2, factor2, white);
	
  const U32* spectrum1 = &SpecialSpectrumsInt[color1 * Resolution];
	const U32* spectrum2 = &SpecialSpectrumsInt[color2 * Resolution];
	
  //add all part spectrums to resulting spectrum and put it into the vector
  std::vector<U32> result;
  for (unsigned int i = 0; i < Resolution; i++){
    U32 value =   spectrum1[i] < 0x10000
                ? ((spectrum1[i] * factor1) >> 15)
                : (((spectrum1[i] >> 8) * factor1) >> 7);
                           
    value +=   spectrum2[i] < 0x10000
             ? ((spectrum2[i] * factor2) >> 15)
             : (((spectrum2[i] >> 8) * factor2) >> 7);
                             
    value +=  white;
    result.push_back(value);
  }
  
  return result;
}

//**********************************
// IndexByLambda
//
// wavelength in nm -> index rounded
//**********************************
unsigned int imgproc::Spectrum::IndexByLambda(unsigned int lambda) const{
  if (lambda < WAVELENGTH_MIN) return 0;
  if (lambda > WAVELENGTH_MAX) return Resolution - 1;
  return static_cast<unsigned int>(static_cast<double>(Resolution) * static_cast<double>(lambda - WAVELENGTH_MIN) / static_cast<double>(WAVELENGTH_MAX - WAVELENGTH_MIN));
}

//***************************
// LambdaByIndex
//
// index -> wavelength in nm
//***************************
unsigned int imgproc::Spectrum::LambdaByIndex(unsigned int index) const {
  if (index == 0) return WAVELENGTH_MIN;
  if (index >= Resolution) return WAVELENGTH_MAX;
  return WAVELENGTH_MIN + static_cast<unsigned int>(static_cast<double>(WAVELENGTH_MAX - WAVELENGTH_MIN) * static_cast<double>(index) / static_cast<double>(Resolution));
}

//*********************************************************
// STATIC: SpectralFactorsFromRGB Integervariante
//
// calculates the spectral factors from the given RGB value
// integer variant
//
// old version - perhaps it is faster so we keep it 
//*********************************************************
inline void imgproc::Spectrum::SpectralFactorsFromRGBIntOld(const U32 r, const U32 g, const U32 b,
                                                            unsigned int &color1, 
                                                            U32          &factor1, //0...1 -> 0 ...0x8000
                                                            unsigned int &color2, 
                                                            U32          &factor2, //0...1 -> 0 ...0x8000
                                                            U32          &white)   //0...1 -> 0 ...0x8000
{
  
  //find both dominating partial spectrums
  FindDomColorVector(r, g, b, color1, factor1, color2, factor2, white);
  
	//handle white 
  const U32 whitelevels[]            = {3277, 6554, 16384, 22938}; //{0.1, 0.2, 0.5, 0.7};
  //const U32 reducedColor[]         = {29491, 26214, 16384, 9830}; //{0.9, 0.8, 0.5, 0.3}; //1.0 - whitelevel
	const U32 reciprocalReducedColor[] = {36409, 40960, 65536, 109227}; //{1.11111111111, 1.25, 2.0, 3.33333333333}; 

	unsigned int white1 = 0;
	unsigned int white2 = 0;
	U32 newfactor;
	U32 newfactor1 = factor1;
	U32 newfactor2 = factor2;
	
	U32 additionalWhite;
	
	for (unsigned int i = 0; i < 4; i++){
		newfactor = (factor1 * reciprocalReducedColor[i]) >> 15;
		additionalWhite = (newfactor * whitelevels[i]) >> 15;
		if (additionalWhite > white) break;
		white1 = i + 1;
		newfactor1 = newfactor;
		newfactor = (factor2 * reciprocalReducedColor[i]) >> 15;
		additionalWhite = additionalWhite + ((newfactor * whitelevels[i]) >> 15);
		if (additionalWhite > white) break;
		white2 = i + 1;
		newfactor2 = newfactor;
  }

	//reduce pure white and clip to zero 
  U32 reducedWhite;
  if (white1 > 0){
    reducedWhite = (whitelevels[white1 - 1] * newfactor1) >> 15;
    if (white > reducedWhite) white -= reducedWhite; 
    else white = 0;
  }
  if (white2 > 0){
    reducedWhite = (whitelevels[white2 - 1] * newfactor2) >> 15;
    if (white > reducedWhite) white -= reducedWhite; 
    else white = 0;
  }
	
	//get real color vectsor indices and set results
	color1 = color1 + white1 * 6;
	color2 = color2 + white2 * 6;
  factor1 = newfactor1;
  factor2 = newfactor2;
}



