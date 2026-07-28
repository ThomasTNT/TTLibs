#ifndef __HISTOGRAMM_H_
#define __HISTOGRAMM_H_

#include <vector>

#include "mathroutines.h"

namespace imgproc{

//*****************************************************************************
//*                                Histogramm                                 *
//*                                                                           *
//*                           class for Histogramm                            *
//*                                                                           *
//*****************************************************************************
const unsigned int HISTOGRAMM_STEPS_DEFAULT = 256; //0...255

class Histogramm {
  public:
    
    //constructor
    Histogramm(unsigned int steps = HISTOGRAMM_STEPS_DEFAULT);
  
    //destructor
    ~Histogramm();

    //sets histogramm to zero
    void Reset();

    //adds a value (direct index)
    void AddValue(const unsigned int value);

    //adds a value relatively (0...1)
    void AddValueRel(const float value);

    //returns the index of the value 
    unsigned int GetIndexByRelValue(const float value);

    //returns the value of the given index 
    float GetRelValueByIndex(const unsigned int index);

    // returns the histogramm with native resolution
    std::vector<unsigned int> GetHistogramm();
  
    //returns the histogramm with lower resolution
    std::vector<unsigned int> GetHistogramm(const unsigned int resolution);

    // returns the smoothed histogramm with native resolution
    std::vector<unsigned int> GetSmoothedHistogramm(const unsigned int smoothfactor);
  
    //returns the smoothed histogramm with lower resolution
    std::vector<unsigned int> GetSmoothedHistogramm(const unsigned int smoothfactor, const unsigned int resolution);

    //smoothes the histogramm
    //smoothfactor shall be around 10
    static std::vector<unsigned int> SmoothHistogramm(const std::vector<unsigned int>& hist, const unsigned int smoothfactor, const bool addMax);

    //prints the histogramm for testing and debugging
    void TestPrint();

  private:

    //histogramm
    unsigned int* HistData;

    unsigned int Steps;

};

} //end of namespace imgproc

#endif

