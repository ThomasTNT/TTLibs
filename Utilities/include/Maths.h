#ifndef __UTIL_MATH_H
#define __UTIL_MATH_H




#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <complex>

namespace util{


class Vector{

  public:
    
    Vector(int size = 3, int arraysize = 3);

    //Vector(const Vector& other);
    //~Vector();

    //void add(T value);
    void   set(std::complex<double> value, int index);
    std::complex<double> get(int index) const;
    int    size() const;
    double absolute() const;

    std::string toString();

    std::complex<double> operator[](int index);
    //Vector& operator=(const Vector& other);
    
    Vector operator+(const Vector& other);
    Vector& operator+=(const Vector& other);
    Vector operator-(const Vector& other);
    Vector& operator-=(const Vector& other);
    
    Vector operator*(std::complex<double> scalar);
    std::complex<double> operator*(const Vector& other);
    Vector operator%(const Vector& other);


  private:
   
    //std::complex<double>* array;  
    std::vector<std::complex<double> > array;
    int     length;
    int     arraysize;
};

} //end of namespace util

util::Vector operator*(std::complex<double> scalar, const util::Vector& v);

namespace util{

class Matrix{

  public:
    Matrix(int sizex = 32, int sizey = 32);
    Matrix(const Matrix& other);
    ~Matrix();

    void set(std::complex<double> value, int x, int y);
    std::complex<double> get(int x, int y) const;
    void init(std::complex<double> value = 0);
    void setDiagonal(std::complex<double> value = 1);
    std::complex<double> getValue(int x, int y) const;
    int    getSizeX() const;
    int    getSizeY() const;
    Vector getRow(int y) const;
    Vector getCol(int x) const;

    std::string toString();

    //Vector& operator[](int y);

    Matrix& operator=(const Matrix& other);
    
    Matrix operator+(const Matrix& other);
    Matrix operator+=(const Matrix& other);
    Matrix operator-(const Matrix& other);
    Matrix operator-=(const Matrix& other);
    
    Matrix operator*(std::complex<double> scalar);
    Matrix operator*=(std::complex<double> scalar);
    Vector operator*(const Vector& vec);

  private:
   
    //std::complex<double>** matrix;  
    std::vector< std::vector< std::complex<double> > > matrix;
    int sizex, sizey;
};

} //end of namespace util

util::Matrix operator*(std::complex<double> scalar, const util::Matrix& m);

namespace util{


} //end of namespace util

#endif