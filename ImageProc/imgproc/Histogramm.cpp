#include "Histogramm.h"

//*****************************************************************************
//*                                Histogramm                                 *
//*                                                                           *
//*                           class for Histogramm                            *
//*                                                                           *
//*****************************************************************************

//************
// constructor
//************
imgproc::Histogramm::Histogramm(unsigned int steps) : Steps(steps) {
  HistData = new unsigned int[steps];
}
  
//***********
// destructor
//***********
imgproc::Histogramm::~Histogramm(){
  delete HistData;
}

//************************
// Reset
//
// sets histogramm to zero
//************************
void imgproc::Histogramm::Reset(){
  for (unsigned int i = 0; i < Steps; i++) HistData[i] = 0;
}

//*************
// AddValue
//
// adds a value
//*************
void imgproc::Histogramm::AddValue(unsigned int value){
  ++HistData[value];
}

//********************************
// AddValueRel
// adds a value relatively (0...1)
//********************************
void imgproc::Histogramm::AddValueRel(const float value){
  if (value <= 0.0f){
    ++HistData[0];
  } 
  else if (value >= (1.0f - (0.5f / static_cast<float>(Steps)))){
    ++HistData[Steps - 1];
  } 
  else{
    const unsigned int index = static_cast<unsigned int>(value * static_cast<float>(Steps));
    ++HistData[index];
  }
}

//*******************************
// GetIndexByRelValue
// 
// returns the index of the value
//*******************************
unsigned int imgproc::Histogramm::GetIndexByRelValue(const float value){
  if (value <= 0.0f){
   return 0;
  } 
  else if (value >= (1.0f - (0.5f / static_cast<float>(Steps)))){
    return (Steps - 1);
  } 
  else{
    const unsigned int index = static_cast<unsigned int>(value * static_cast<float>(Steps));
    return index;
  }
}

//*************************************
// GetRelValueByIndex
//
// returns the value of the given index 
//*************************************
float imgproc::Histogramm::GetRelValueByIndex(const unsigned int index){
  const float value = index >= Steps 
                    ? 1.0f 
                    : static_cast<float>(index) / static_cast<float>(Steps) + (0.5f / static_cast<float>(Steps));
  return value;
}


//**********************************************
// GetHistogramm
//
// returns the histogramm with native resolution
// last entry is maxvalue;
//**********************************************
std::vector<unsigned int> imgproc::Histogramm::GetHistogramm(){
  unsigned int max = 0;
  std::vector<unsigned int> result;
  for (unsigned int i = 0; i < Steps; i++){
    result.push_back(HistData[i]); 
    if (HistData[i] > max) max = HistData[i];
  }
  result.push_back(max);
  return result;
}


//*********************************************
// GetHistogramm
//
// returns the histogramm with lower resolution
// last entry is maxvalue;
//*********************************************
std::vector<unsigned int> imgproc::Histogramm::GetHistogramm(const unsigned int resolution){
  
  std::vector<unsigned int> histogramm;
  for (unsigned int i = 0; i < Steps; i++) histogramm.push_back(HistData[i]); 

  std::vector<unsigned int> result = mathutil::ResampleVector<unsigned int>(histogramm, resolution);
  
  //get the maxvalue by interating over vector 
  unsigned int max = 0;
  for (unsigned int i = 0; i < result.size(); i++) if (result[i] > max) max = result[i];
  result.push_back(max);

  return result;
}

//*******************************************************
// GetHistogramm
//
// returns the smoothed histogramm with native resolution
// last entry is maxvalue;
//*******************************************************
std::vector<unsigned int> imgproc::Histogramm::GetSmoothedHistogramm(const unsigned int smoothfactor){
  unsigned int max = 0;
  std::vector<unsigned int> result;
  for (unsigned int i = 0; i < Steps; i++){
    result.push_back(HistData[i]); 
    if (HistData[i] > max) max = HistData[i];
  }
  result = SmoothHistogramm(result, smoothfactor, true);
  return result;
}


//******************************************************
// GetHistogramm
//
// returns the smoothed histogramm with lower resolution
// last entry is maxvalue;
//******************************************************
std::vector<unsigned int> imgproc::Histogramm::GetSmoothedHistogramm(const unsigned int smoothfactor, const unsigned int resolution){
  
  std::vector<unsigned int> histogramm;
  for (unsigned int i = 0; i < Steps; i++) histogramm.push_back(HistData[i]); 
  histogramm = SmoothHistogramm(histogramm, smoothfactor, false);

  std::vector<unsigned int> result = mathutil::ResampleVector<unsigned int>(histogramm, resolution);
  
  //get the maxvalue by interating over vector 
  unsigned int max = 0;
  for (unsigned int i = 0; i < result.size(); i++) if (result[i] > max) max = result[i];
  result.push_back(max);

  return result;
}

//********************************
// SmoothHistogramm (static)
//
// smoothes the histogramm
// smoothfactor shall be around 10
//********************************
std::vector<unsigned int> imgproc::Histogramm::SmoothHistogramm(const std::vector<unsigned int>& hist, 
                                                                const unsigned int smoothfactor, 
                                                                const bool addMax)
{
  const unsigned int n = smoothfactor * 2 + 1;

  float wsum = 0.0f;
  std::vector<float> W;
  for (unsigned int i = 0; i < n; ++i){
    const float wi = i <= smoothfactor ? static_cast<float>(i + 1) / static_cast<float>(n * n) 
                                       : static_cast<float>(n - i) / static_cast<float>(n * n);
    wsum += wi;
    W.push_back(wi);
  }

  std::vector<unsigned int> result;
  float d;
  unsigned int di;
  unsigned int max = 0;
  unsigned int iend;
  const unsigned int histsize = hist.size();
  for (unsigned int h = 0; h < histsize; ++h){
    d = 0.0f;
    iend = histsize + smoothfactor - h;
    for (unsigned int i = (h > smoothfactor ? 0 : smoothfactor - h); 
                      i < (n < iend ? n : iend); 
                      ++i)
    {
      d += static_cast<float>(hist[h + i - smoothfactor]) * W[i];
    }
    d /= wsum;
    di = static_cast<unsigned int>(d + 0.5);
    result.push_back(static_cast<unsigned int>(di));
    if (di > max) max = di;
  }
  if (addMax) result.push_back(max);
  return result;
}


//************************************************
// TestPrint
// 
// prints the histogramm for testing and debugging
//************************************************
void imgproc::Histogramm::TestPrint(){
  std::cout << "--------- HISTOGRAMM ---------" << std::endl;
  for (unsigned int i = 0; i < Steps; ++i){
    std::cout << i << ": " << HistData[i] << std::endl;
  }
  std::cout << "---------END OF HISTOGRAMM ---------" << std::endl;
}

