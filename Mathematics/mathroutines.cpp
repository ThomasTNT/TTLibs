#include "mathroutines.h"


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

//***********
//constructor
//***********
mathutil::LSE::LSE(const Matrix<double>& A) : AI(A), BI(A.GetN()), XI(A.GetM()) {
	InterchangeLine = new unsigned int[A.GetN() * A.GetM()];
	CalcMatrix();
}

//**********
//destructor
//**********
mathutil::LSE::~LSE(){
  delete[] InterchangeLine; 
}

//**************
//TEST GetMatrix
//**************
mathutil::Matrix<double> mathutil::LSE::GetMatrix() const { return AI; }

//**************
//TEST GetVector
//**************
mathutil::Vector<double> mathutil::LSE::GetVector() const { return BI; }

//****************
//MAIN GetSolution
//
//main methode 
//****************
mathutil::Vector<double> mathutil::LSE::GetSolution(const mathutil::Vector<double>& b){
  BI = b;
	//Pivotisierung
	double temp;
	for (unsigned int k = 0; k < AI.GetN() - 1; k++){
		if (InterchangeLine[k] > k){
			temp = BI.Get(k);
			BI.Set(k, BI.Get(InterchangeLine[k]));
			BI.Set(InterchangeLine[k], temp);
		}		
	}
	//calculate right sides
	for (unsigned int k = 1; k < AI.GetN(); k++){
		for(unsigned int j = 0; j < k; j++){
			BI.Set(k, BI.Get(k) - AI.Get(k,j) * BI.Get(j));
		}
	}
	//calculate result vector XI 
	//only for N == M!!!!
	for (int k = AI.GetN() - 1; k >= 0; k--){
		for (unsigned int j = k + 1; j < AI.GetM(); j++){
			BI.Set(k, BI.Get(k) - AI.Get(k,j) * XI.Get(j) );
		}
		XI.Set(k, BI.Get(k) / AI.Get(k,k));
	}
	return XI;
}

//**********************
//CALCULATION CalcMatrix
//**********************
void mathutil::LSE::CalcMatrix(){
  const unsigned int n = AI.GetN();
	const unsigned int m = AI.GetM();
  
  //calculate only if sensful matrix exists
	if (n > 0 && m >= 0){ 
		double pmax; 
    unsigned int currinterchangeline;
		double temp;
    //loop over all steps
		for (unsigned int k = 0; k < n - 1; k++){ 
			//look for greatest element
			pmax = 0; 
      currinterchangeline = 0;
			for (unsigned int i = k; i < n; i++){
				if (fabs(AI.Get(i, k)) > pmax){
					pmax = AI.Get(i, k);
					currinterchangeline = i;
				}	
			}
			InterchangeLine[k] = currinterchangeline;
			//interchange lines
			if (currinterchangeline > k){
				for (unsigned int j = 0; j < n; j++){
					temp = AI.Get(currinterchangeline, j);
					AI.Set(currinterchangeline, j, AI.Get(k, j));
					AI.Set(k, j, temp);
				}
			}
			//calculate L(stored at the zero elements of the matrix)
			for (unsigned int i = k + 1; i < n; i++) AI.Set(i, k, AI.Get(i, k) / AI.Get(k, k));
			//claculate matrix
			for (unsigned int j = k + 1; j < m; j++){
				for (unsigned int i = k + 1; i < n; i++){
					AI.Set(i, j, AI.Get(i, j) - AI.Get(i, k) * AI.Get(k, j) );
				}
			}
		}//end of loop over all steps
	}
}


//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************


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

//*************
//constructor 1
//*************
mathutil::SplineApprox::SplineApprox() : N(0), IsConstant(false) {}

//*************
//constructor 2
//*************
mathutil::SplineApprox::SplineApprox(const mathutil::Vector<double>& x, const mathutil::Vector<double>& y){
	SetXY(x, y);
}

//******************************************
// SetConstant
//
// disables the whole interpolation 
// simply a contant will be return a y-value
// DO WE NEED THIS FEATURE ANYMORE?????
//******************************************
void mathutil::SplineApprox::SetConstant(double value){
	N = 0;
	Constant = value;
	IsConstant = true;
}

//*********
// GetN
//*********
unsigned int mathutil::SplineApprox::GetN() const { 
  return N; 
}

//*******************************************
// SetXY 
//
// returns false if some values are not valid
//*******************************************
bool mathutil::SplineApprox::SetXY(const mathutil::Vector<double>& x, const mathutil::Vector<double>& y){
	N = 0;
  if ((x.Size() < 3) ||(x.Size() > y.Size())){ 
    throw MathException("SplineApprox::SetXY: Not enough values!");
	}	
	//check if strictly increasing 
	bool ok = 1;
	double tmpx = x.Get(0);
	for (unsigned int i = 1; i < x.Size(); i++){ 
		if (tmpx < x.Get(i)) tmpx = x.Get(i);
		else { ok = false; break; }
	}
	if (!ok){ 
    throw MathException("SplineApprox::SetXY: X is not monoton!");
  }
  
	X = x; 
  Y = y; 
	N = X.Size();
	H = mathutil::Vector<double>(N-1);
	for (unsigned int i = 0; i < N-1; i++) H.Set(i, X.Get(i+1) - X.Get(i));
	IsConstant = false;
	
	return CalculateY2();
}


//********************************************
// SetXValue
//
// sets the x-value on given position
// make sure that it keeps strictly increasing
// returns success
//********************************************
bool mathutil::SplineApprox::SetXSingleValue(unsigned int i, double value){
	if ( (i < N) && (i == 0 || value > X.Get(i-1)) && (i == N-1 || value < X.Get(i+1)) ){
		X.Set(i, value);
		if (i >= 1)   H.Set(i-1, X.Get(i)-X.Get(i-1) );
		if (i <= N-2) H.Set(i  , X.Get(i)-X.Get(i) );
		return CalculateY2();	
	}
	else {
    //std::cerr << "SplineApprox::SetXValue: Could not set X-value" << std::endl;
		return false;
	}
}

//**************************************
// GetXValue
//
// returns the x-value on given position
//**************************************
double mathutil::SplineApprox::GetXSingleValue(unsigned int i) const{
  return (i < N) ? X.Get(i) : INVALIDDOUBLE;
}

//**************************************
// SetYValue
//
// sets the y-value on given position
// returns success
//**************************************
bool mathutil::SplineApprox::SetYSingleValue(unsigned int i, double value){
	if (i < N) {
		Y.Set(i, value);
		return CalculateY2();
	}
	else {
		//std::cerr << "SplineApprox::SetYValue: Could not set Y-value" << std::endl;
		return 0;
	}
}

//**************************************
// GetYValue
//
// returns the y-value on given position
//**************************************
double mathutil::SplineApprox::GetYSingleValue(unsigned int i) const{
  return (i < N) ? Y.Get(i) : INVALIDDOUBLE;
}

//******
// Clear
//******
void mathutil::SplineApprox::Clear(){
  N = 0;
  IsConstant = false;
  Constant = 0.0f;
  X.Clear();
  Y.Clear();
  H.Clear();
  Y2.Clear();
}


//*******************************************
// CALCULATION CalculateY2 
//
// returns false if some values are not valid
//*******************************************
bool mathutil::SplineApprox::CalculateY2(){
	if (N <= 2){ 
    //std::cerr << "SplineApprox::CalculateY2: Not enought values for spline-appraximation!" << std::endl; 
    return false; 
  }
	Matrix<double> A(N - 2, N - 2);
	mathutil::Vector<double> B(N - 2);
	
	for (unsigned int i = 0; i < N-2; i++){
		if (i > 0) A.Set( i, i-1, H.Get(i) );
		A.Set( i, i, 2 * (H.Get(i)+H.Get(i+1)) );
		if (i < N-3) A.Set(i, i+1, H.Get(i+1) );
		B.Set(i, ( -(Y.Get(i+1) - Y.Get(i)) * 6/H.Get(i) +(Y.Get(i+2) - Y.Get(i+1)) * 6/H.Get(i+1) ) );
	}
	
	mathutil::Vector<double> C = LSE(A).GetSolution(B);
	
	Y2 = mathutil::Vector<double>(N);
	Y2.Set(0, 0);     //natural Splines
	Y2.Set(N-1, 0);
	for (unsigned int i = 1; i < N-1; i++) Y2.Set(i, C.Get(i-1));
	return true;
}

//***********************************************
// MAIN GetY 
//
// returns the interpolated value y for a given x
//***********************************************
double mathutil::SplineApprox::GetY(double x) const{
  //constant case DO WE NEED THIS FEATURE ANYMORE?????
	if (IsConstant) return Constant;
	
  if (N <= 2){ 
    //std::cerr << "SplineApprox::GetY: No approximation-datas!" << std::endl; 
    return 0; 
  }
	unsigned int i = 0;
	while ((x >= X.Get(i)) && (i < N)) i++;
	if (i >= N) i--;
	if (i > 0) i--;
	
  //create interpolation polygon
	const double a  = (Y2.Get(i+1) - Y2.Get(i)) / (6 * H.Get(i));
	const double b  = Y2.Get(i) / 2;
	const double c  = (Y.Get(i+1) - Y.Get(i)) / H.Get(i) - (Y2.Get(i+1) + 2 * Y2.Get(i)) * H.Get(i) / 6;
	const double d  = Y.Get(i);
	const double dx = x - X.Get(i);
	return a * dx*dx*dx + b * dx*dx + c * dx + d;	
}

//*********************************************
// BinominalCoefficient
//
// calculates a binominal coefficent (n over k)
//*********************************************
int mathutil::BinominalCoefficient(int n, int k){
  int result;
  if (k == 0) return 1;
  if (2 * k > n) result = BinominalCoefficient(n, n-k);
  else{
    result = n;
    for (int i = 2; i <= k; i++){
      result = result * (n + 1 - i);
      result = result / i;
    }
  }
  return result;
}

//*********************************************
// creates a new vector with a different
// resolution
// some kind of resampling
//*********************************************
/*template<class T>
std::vector<T> mathutil::ResampleVector(std::vector<T> in, unsigned int newResolution) {
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
}*/

//************************************************
// BooleanLikelihood
//
// returns randomly true with the given likelihood
//************************************************
bool mathutil::BooleanLikelihood(const double likelihood){
  return likelihood > 0 && static_cast<int>(static_cast<double>(rand()) / (likelihood * (RAND_MAX + 1.0)) ) == 0; 
}


