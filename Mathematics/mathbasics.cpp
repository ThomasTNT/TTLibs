#include "mathbasics.h"

//************************************************************************
//*                                                                      *
//*                             MathException                            *
//*                                                                      *
//*                  exception class for mathutil etc.                   *
//*                                                                      *
//************************************************************************

//************
// constructor
//************
mathutil::MathException::MathException(const std::string& errormessage) : ErrorMessage(errormessage)
{
  std::cerr << ErrorMessage << std::endl;
}

//****************
// GetErrorMessage
//****************
std::string mathutil::MathException::GetErrorMessage() const{
  return ErrorMessage;

}


//***************************************************************
//*  Vector                                                     *     
//*                                                             *
//*  this is a class represents an n-dimensional vector         *
//*  and some basic algorithm and operators                     *
//*                                                             *
//*  this class is not suited for fast 3D-Algorithms            *
//*  but for mathematical calculations such as equality systems *
//***************************************************************

//implementation is in mathbasics.h because of templates

//***************************************************************
//*  Matrix                                                     *     
//*                                                             *
//*  this is a class represents an nxm matrix                   *
//*  and some basic algorithm and operators                     *
//*                                                             *
//*  this class is not suited for fast 3D-Algorithms            *
//*  but for mathematical calculations such as equality systems *
//***************************************************************

//implementation is in mathbasics.h because of templates



