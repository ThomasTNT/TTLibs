#ifndef _MATHROUTINES_H_
#define _MATHROUTINES_H_

#include <iostream>
#include <vector>

#include <cmath>

#include "mathbasics.h"

const double INVALIDDOUBLE = 999999999;

//*****************************************************************************
//*              some more or less basical mathematical routines              *
//*****************************************************************************

namespace mathutil{

//***********************************************************
//*  LSE                                                    *     
//*                                                         *
//*  class for calculation of a linear system of equations  *
//*                                                         *
//*  Gaussian algorithm                                     *
//*  originally based on "numerical recipes in C"           *
//*  used for Thomas Thiele's degree dissertation           *
//*                                                         *
//*  it	must be unique (N == M)                             *
//***********************************************************
class LSE{

	public:

    //contruction and deconstruction
		LSE(const Matrix<double>& A);
		~LSE();

    //some getter and setter (more or less for test only)
		mathutil::Matrix<double> GetMatrix() const;
		mathutil::Vector<double> GetVector() const;

    //main methode 
		mathutil::Vector<double> GetSolution(const mathutil::Vector<double>& b);

	private:

		void CalcMatrix();
		Matrix<double> AI; 
    mathutil::Vector<double> BI;
		mathutil::Vector<double> XI;
		unsigned int* InterchangeLine; 
};



//***********************************************************
//*  SplineApproximation                                    *     
//*                                                         *
//*  class for spline approximation                         *
//*                                                         *
//*  used for Thomas Thiele's degree dissertation           *
//*                                                         *
//*  vector x contains x-values, vector y contains y-values *
//*  x must be strictly increasing                          *
//***********************************************************
class SplineApprox{

	public:

    //contruction and deconstruction
		SplineApprox();
		SplineApprox(const mathutil::Vector<double>& x, const mathutil::Vector<double>& y);
	
    //interpolation methode
		double GetY(double x) const;

    //disables the whole interpolation 
    //simply a contant will be return a y-value
    //DO WE NEED THIS FEATURE ANYMORE?????
    void SetConstant(double value);

    //some getter and setter
    unsigned int GetN() const;

		bool   SetXY(const mathutil::Vector<double>& x, const mathutil::Vector<double>& y);

    bool   SetXSingleValue(unsigned int i, double value);
    double GetXSingleValue(unsigned int i) const;

		bool   SetYSingleValue(unsigned int i, double value);
    double GetYSingleValue(unsigned int i) const;

    void Clear();

    //create a lookup table
		//int *GetLut(int n); //x-Werte von 0 bis n-1 (nur int-werte!)  ????????
		
	private:
		
    bool CalculateY2();
		mathutil::Vector<double> X,Y;
		mathutil::Vector<double> H;
		mathutil::Vector<double> Y2;
		unsigned int N; //number of supporting points
		double Constant;
		bool IsConstant;

};



//*********************************************
// BinominalCoefficient
//
// calculates a binominal coefficent (n over k)
//*********************************************
int BinominalCoefficient(int n, int k);



//*********************************************
// creates a new vector with a different
// resolution
// some kind of resampling
//*********************************************
template<class T>
std::vector<T> ResampleVector(std::vector<T> in, unsigned int newResolution){
  std::vector<T> result;
  for (unsigned int i = 0; i < newResolution; i++) result.push_back(static_cast<T>(0));
  if (in.size() == 0) return result;

  const double amountOut = static_cast<double>(in.size()) / static_cast<double>(newResolution);
  double freeOut = 0;  //indicates how much has to be copied until the out slot/index is full
  double restIn = 0;  //indicates how much is not coiped yet fromthe current in-index
  double amountToCopy; //what is currently to copied from in-index to out-index 
  int    inIndex = -1;
  for (unsigned int outIndex = 0; outIndex < newResolution; outIndex++) {

    //---------- start new out entry ------------
    freeOut = amountOut;

    //as long as there is something to copy
    while (freeOut > 0) {

      //-------- start new input if there is no value left ---------
      if (restIn <= 0) {
        ++inIndex;
        //there may be rounding errors so there may be not defined end
        //and we deal with a little remaining rest 
        if (inIndex >= in.size()) break;
        restIn = 1.0;
      }
      //how much from inIndex has to be copied to the out array
      amountToCopy = freeOut < restIn ? freeOut : restIn;

      //----------- copy the values from input to output ------------
      result[outIndex] += static_cast<T>(amountToCopy * static_cast<double>(in[inIndex]));
      freeOut -= amountToCopy;
      restIn -= amountToCopy;
    } // end of while
  } //end of for over all new indices

  return result;
}

//************************************************
// BooleanLikelihood
//
// returns randomly true with the given likelihood
//************************************************
bool BooleanLikelihood(const double likelihood);


} //end of namespace mathutil

#endif

