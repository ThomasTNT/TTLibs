#pragma once

#include <cmath>
#include <map>
#include <algorithm>  
#include <mutex>

#include "Logger.h"
#include "Gaussfilter.h"

namespace imgproc {

  //*****************************************************************************
  //*                                ImageSource                                *
  //*                                                                           *
  //*                        for VariableFastGaussFilter                        *
  //*                                                                           *
  //*                         overwrite this for your needs                     *
  //*****************************************************************************
  class ImageSource {

    public: 

      ImageSource(const unsigned int width, const unsigned int height) : Width(width), Height(height) {}
      virtual float GetPixelValueAt(const unsigned int x, const unsigned int y) const = 0;

      unsigned GetWidth() const { return Width; }
      unsigned GetHeight() const { return Height; }

    private:
      const unsigned int Width;
      const unsigned int Height;
  };

  //*****************************************************************************
  //*                           GaussFilterMatrix1D                             *
  //*                                                                           *
  //*****************************************************************************
  class GaussFilterMatrix1D {
    public: 
      GaussFilterMatrix1D();
      ~GaussFilterMatrix1D();
      float* Matrix;
      unsigned int Size;

  };


  //*****************************************************************************
  //*                         VariableFastGaussFilter                           *
  //*                                                                           *
  //*  implementing fast (two past) and variable sigma gauss filtering          *
  //*  cached values can be stored for fast frame processing                    *
  //*                                                                           *
  //*****************************************************************************
  class VariableFastGaussFilter {

    public:

      //constructor
      VariableFastGaussFilter();

      //destructor 
      ~VariableFastGaussFilter();
      
      //filter method horizontally 
      float FilterHor(const ImageSource& source, const float radius, const unsigned int x, const unsigned  int y);

      //filter method vertically 
      float FilterVer(const ImageSource& source, const float radius, const unsigned int x, const unsigned  int y);

    private:

      //returns a matrix for the given radius (range)
      //if none exists it will be created
      const GaussFilterMatrix1D* GetMatrix(const float radius);

      //created a matrix for given radius
      static GaussFilterMatrix1D* CreateMatrix(const float radius);

      //returns the begin of the radius range
      float GetRadiusRage(const float radius);

      std::map<float, const GaussFilterMatrix1D*> Matrizes; 



  };


}
