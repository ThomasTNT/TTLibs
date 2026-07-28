#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <set>
#include <iomanip>

//*****************************************************************************
//*            these are simple mathe mathematical basic classes              *
//*****************************************************************************

namespace mathutil{

//************************************************************************
//*                                                                      *
//*                             MathException                            *
//*                                                                      *
//*                  exception class for mathutil etc.                   *
//*                                                                      *
//************************************************************************
class MathException{

  public:
    MathException(const std::string& errormessage);

    std::string GetErrorMessage() const;

  protected:
    std::string ErrorMessage;

};


//***************************************************************
//*  Vector                                                     *     
//*                                                             *
//*  this is a class represents an n-dimensional vector         *
//*  and some basic algorithm and operators                     *
//*                                                             *
//*  this class is not suited for fast 3D-Algorithms            *
//*  but for mathematical calculations such as equality systems *
//***************************************************************
template<typename T> class Matrix;

template<class T>
class Vector{

	public:

    //contruction and destruction
    explicit Vector<T>(unsigned int size = 0) { Init(size); }
		Vector<T>(const mathutil::Vector<T>& v) : Data(v.Data){}
    template <typename... Args>
    Vector(std::initializer_list<T> initList) : Data(initList) {}
		~Vector(){}
		
		void Init(unsigned int size, T value = 0){
      Data.clear();
      for (unsigned int i = 0; i < size; i++) Data.push_back(value);
    }
   
    //getter and setter
		void Set(const unsigned int pos, const T value){ if (pos < Data.size()) Data[pos] = value; }
    void Add(const unsigned int pos, const T value){ if (pos < Data.size()) Data[pos] += value; }
    void AddElement(const T value){ Data.push_back(value); }
    void InsertElement(const unsigned int pos, T value) { Data.insert(Data.begin() + pos, value); }
		T    Get(const unsigned pos) const { return (pos < Data.size()) ? Data[pos] : 0; }
    T    GetX() const { return Get(0); }
    T    GetY() const { return Get(1); }
    T    GetZ() const { return Get(2); }
    T    Last() const { return !Data.empty() ? Data.back() : 0; }
		unsigned int Size() const { return static_cast<unsigned int>(Data.size()); }
		
		//return absolute value
    double Absolute() const {
      if (Data.empty()) return 0;
      T sum = std::abs(Data[0]) * std::abs(Data[0]);
      for (int i = 0; i < Data.size(); i++) sum += std::abs(Data[i]) * std::abs(Data[i]);
      return sqrt(sum);
    }

    //normalize
    Vector<T> Normalize() const { 
      T squaredLength = 0;
      for (size_t i = 0; i < Data.size(); ++i) {
        squaredLength += Data[i] * Data[i];
      }
      T length = std::sqrt(squaredLength);

      // This prevents division by zero or numerical instability
      if (length < static_cast<T>(1e-6)) { 
        return Vector<T>(Data.size());
      }

      // Normalize each component by dividing by the length
      Vector<T> normalizedVector(Data.size());
      for (size_t i = 0; i < Data.size(); ++i) {
        normalizedVector.Data[i] = Data[i] / length;
      }
      return normalizedVector;
    }

    
    //clears the vector
    void Clear(){
      Data.clear();
    } 

    //to string 
    std::string ToString(){
      std::ostringstream os;
      os << "mathutil::Vector(";
      for (int i=0; i < Data.size(); i++){
        if (i > 0) os << ", ";
        os << Data[i];
      }
      os << ")";
      return os.str();
    }

    //operators 
    T operator[](unsigned int index){
      return Get(index);
    }

    T operator[](unsigned int index) const{
      return Get(index);
    }

    mathutil::Vector<T>& operator= (const mathutil::Vector<T>& v){
      if (&v == this) return *this;
      Data = v.Data;
      return *this;
    }
		mathutil::Vector<T>& operator+=(const mathutil::Vector<T>& v){
      for (unsigned int i = 0; i < Data.size() && i < v.Data.size(); i++) Data[i] += v.Data[i];
      return *this;
    }
    mathutil::Vector<T>  operator+ (const mathutil::Vector<T>& v) const {
      mathutil::Vector<T> result(*this);
      result += v;
      return result;
    }
		mathutil::Vector<T>& operator-=(const mathutil::Vector<T>& v){
      for (unsigned int i = 0; i < Data.size() && i < v.Data.size(); i++) Data[i] -= v.Data[i];
      return *this;
    }
    mathutil::Vector<T>  operator- (const mathutil::Vector<T>& v) const{
      mathutil::Vector<T> result(*this);
      result -= v;
      return result;
    }
    //scalar product (conflixts with when as member method)
    /*T operator* (const mathutil::Vector<T>& v){
      T result;
      for (unsigned int i = 0; i < Data.size() && i < v.Data.size(); i++) result += Data[i] * v.Data[i];
      return result;
    }*/

    //cross product
    mathutil::Vector<T> operator% (const mathutil::Vector<T>& v) const{
      if (Size() != 3 || v.Size() != 3) {
        throw MathException("Cross product is only defined for 3-dimensional vectors.");
      }
      mathutil::Vector<T> result(3);
      result.Set(0, Data[1] * v.Data[2] - Data[2] * v.Data[1]);
      result.Set(1, Data[2] * v.Data[0] - Data[0] * v.Data[2]);
      result.Set(2, Data[0] * v.Data[1] - Data[1] * v.Data[0]);
      return result;
    }

    //product with scalar
    mathutil::Vector<T>  operator* (T scalar) const{
      mathutil::Vector<T> result(*this);
      for (int i = 0; i < Data.size(); i++) result.Data[i] = scalar * result.Data[i];
      return result;
    }                

    //product with matrix
    Vector<T> operator*(const Matrix<T>& matrix) const;

    // ToString
    std::string ToString() const {
      std::ostringstream os;
      os << std::fixed << std::setprecision(3);
      for (unsigned int i = 0; i < Data.size(); ++i) {
        os << "| " << Data[i] << " |\n";
      }
      return os.str();
    }

	private:

    std::vector<T> Data;
};

//*********************************
// globaler skalar product operator
//*********************************
template<class T>
T operator*(const mathutil::Vector<T>& v1, const mathutil::Vector<T>& v2) {
  T result = T();
  for (unsigned int i = 0; i < v1.Size() && i < v2.Size(); i++) {
    result += v1.Get(i) * v2.Get(i);
  }
  return result;
}

//********************
// HELP ReorderVectors
//********************
template<class T>
void ReorderVectors(const mathutil::Vector<T>& X,
                    const mathutil::Vector<T>& Y,
                    mathutil::Vector<T>& Xo,
                    mathutil::Vector<T>& Yo)
{
  std::set<size_t> processed;

  for (size_t i = 0; i < X.Size(); ++i){
    T xmin = 99999999999;
    size_t indexMin = 0;
    for (unsigned int j = 0; j < X.Size(); ++j){
      if (processed.find(j) == processed.end()){
        const T x = X.Get(j);
        if (x < xmin){
          xmin = x;
          indexMin = j;
        }
      }
    }
    processed.insert(indexMin);
    Xo.AddElement(X.Get(indexMin));
    Yo.AddElement(Y.Get(indexMin));
  }

}

//****************************************************************
//*  Matrix                                                      *     
//*                                                              *
//*  this is a class represents an nxm matrix                    *
//*  and some basic algorithm and operators                      *
//*                                                              *
//*  this class is not suited for fast 3D-Algorithms             *
//*  but for mathematical calculations such as equality systems  *
//****************************************************************
template<class T>
class Matrix{

	public:

    //contruction and destruction
		Matrix<T>(const unsigned int n = 1, const unsigned int m = 1) : N(n), M(m){ Init(0); }
		Matrix<T>(const Matrix<T>& m) : N(m.N), M(m.M), Data(m.Data){}
		~Matrix(){}

    static Matrix<T> Identity(size_t N) {
      Matrix<T> result(N, N);
      for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
          result.Set(i, j, static_cast<T>(0));
        }
      }
      for (size_t i = 0; i < N; ++i) {
        result.Set(i, i, static_cast<T>(1));
      }
      return result;
    }

    void Init(T value = 0){
      Data.clear();
      for (unsigned int i = 0; i < N; i++){
        std::vector<T> row;
        for (unsigned int j = 0; j < M; j++) row.push_back(value);
        Data.push_back(row);
      }
    }
    
    //getter and setter
		void Set(const unsigned int i, const unsigned int j, T value){ if (i < N && j < M) Data[i][j] = value; }
		void Add(const unsigned int i, const unsigned int j, T value){ if (i < N && j < M) Data[i][j] += value; }
		void SetRow(const unsigned int i, const mathutil::Vector<T> &v){ if (i < N) for (unsigned int j = 0; j < M && j < v.Size(); j++) Data[i][j] = v.Get(j); }
		void SetCol(const unsigned int j, const mathutil::Vector<T> &v){ if (j < M) for (unsigned int i = 0; i < N && i < v.Size(); i++) Data[i][j] = v.Get(i); }
		void SetDiagonal(T value = 1){ for (unsigned int i = 0; i < N && i < M; i++) Data[i][i] = value; }
		
		T    Get(const unsigned int i, const unsigned int j) const { return (i < N && j < M) ? Data[i][j] : 0; }
		
		mathutil::Vector<T> GetRow(int i) const {
      mathutil::Vector<T> result;
      if (i < N){
        for (int j = 0; j < M; j++) result.AddElement(Data[i][j]);  
      }
      return result;
    }
    
    mathutil::Vector<T> GetCol(int j) const{ 
      mathutil::Vector<T> result;
      if (j < M){
        for (int i = 0; i < N; i++) result.AddElement(Data[i][j]);  
      }  
      return result;
    }
		
		unsigned int GetN() const { return N; }
		unsigned int GetM() const { return M; }
		
		std::string toString(){}

    //operators
		Matrix<T>& operator=(const Matrix<T>& m){
      if (&m == this) return *this;
      N = m.N;
      M = m.M;
      Data = m.Data; 
      return *this;
    }
		
    Matrix<T> operator+=(const Matrix<T>& other){
      for (int i = 0; i < N && i < other.N; i++){
        for (int j = 0; j < M && i < other.M; j++) Data[i][j] += other.Data[i][j];
      } 
    }
    
    Matrix<T> operator+(const Matrix<T>& other){
      Matrix<T> result(*this);
      result += other;
      return result;
    }
    
    Matrix<T> operator-=(const Matrix<T>& other){
      for (int i = 0; i < N && i < other.N; i++){
        for (int j = 0; j < M && i < other.M; j++) Data[i][j] -= other.Data[i][j];
      } 
    }
    
    Matrix<T> operator-(const Matrix<T>& other){
      Matrix<T> result(*this);
      result -= other;
      return result;
    }
    
    Matrix<T> operator*=(T scalar){
      for (int i = 0; i < N; i++){
        for (int j = 0; j < M; j++) Data[i][j] = Data[i][j] * scalar;
      }
    }
    
    Matrix<T> operator*(T scalar){
      Matrix<T> result(*this);
      result *= scalar;
      return result;
    }
    
   
    Matrix<T> operator*(const Matrix<T>& other) const {
      if (M != other.N) {
        throw MathException("Matrix dimensions do not match for multiplication (this.Cols != other.Rows).");
      }
      Matrix<T> result(N, other.M); 
      for (unsigned int i = 0; i < N; ++i) { 
        for (unsigned int j = 0; j < other.M; ++j) {
          T sum = T();
          for (unsigned int k = 0; k < M; ++k) {
            sum += this->Get(i, k) * other.Get(k, j);
          }
          result.Data[i][j] = sum;
        }
      }
      return result;
    }

    // ToString
    std::string ToString() const{ 
      std::ostringstream os;
      os << std::fixed << std::setprecision(3);
      for (unsigned int i = 0; i < N; ++i){
        os << "| ";
        for (unsigned int j = 0; j < M; ++j){
          os << Data[i][j];
          if (j < M - 1){
            os << " "; 
          }
        }
        os << " |\n";
      }
      return os.str();
    }


	private:
		unsigned int N, M;
		std::vector< std::vector<T> > Data;
};

template<typename T>
Vector<T> Vector<T>::operator*(const Matrix<T>& matrix) const {
  if (this->Size() != matrix.GetN()) {
    throw MathException("Vector size must match matrix row count.");
  }

  Vector<T> result;
  result.Data.resize(matrix.GetM(), T());
  for (unsigned int j = 0; j < matrix.GetM(); ++j) {
    for (unsigned int i = 0; i < matrix.GetN(); ++i) {
      result.Add(j, Data[i] * matrix.Get(i, j));
    }
  }

  return result;
}

} // end of namespace mathutil
