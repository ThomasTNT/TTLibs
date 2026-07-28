#include "cgibasics.h"


//************************************************************************
//*                                                                      *
//*                              RGBColor                                *
//*                                                                      *
//*      universal representation of an RGB-color with double values     *
//*                                                                      *
//*      0.0 ... 1.0                                                     *
//************************************************************************

//********************
// default constructor
//********************
cgi::RGBColor::RGBColor() : R(0.0), G(0.0), B(0.0) {};
    
//**************
// constructor 1
//**************
cgi::RGBColor::RGBColor(const double r, const double g, const double b) : R(r), G(g), B(b) {};
    
//****************************
// constructor 2 8 bit integer 
//****************************
cgi::RGBColor::RGBColor(const unsigned int r, const unsigned int g, const unsigned int b)
              :R(static_cast<double>(r) / 256.0),
               G(static_cast<double>(g) / 256.0),
               B(static_cast<double>(b) / 256.0)
{
}

//*************
// operator+=
//
// adds a color
//*************
cgi::RGBColor& cgi::RGBColor::operator+= (const RGBColor& rhs){
  R += rhs.R; 
  G += rhs.G;
  B += rhs.B;
  return *this;
}
     
//****************
// operator+
//
// adds two colors
//****************
cgi::RGBColor cgi::RGBColor::operator+ (const RGBColor& rhs) const {
  return RGBColor(*this) += rhs;                    
}
   
//*******************
// operator-=
//
// subtracts a color
//******************
cgi::RGBColor& cgi::RGBColor::operator-= (const RGBColor& rhs){
  R -= rhs.R; 
  G -= rhs.G;
  B -= rhs.B;
  return *this;
}
 
//*********************
// operator-
//
// subtracts two colors
//*********************
cgi::RGBColor cgi::RGBColor::operator- (const RGBColor& rhs) const {
  return RGBColor(*this) -= rhs;
}

 
//**********************
// operator*=
//
// multiply with other color
//**********************
cgi::RGBColor& cgi::RGBColor::operator*=(const RGBColor& rhs){
  R *= rhs.R;
  G *= rhs.G;
  B *= rhs.B;
  return *this;
}
 
//**********************
// global operator*
//
// product of two colors
//**********************
cgi::RGBColor cgi::RGBColor::operator* (const RGBColor& rhs) const {
  return RGBColor(R * rhs.R, G * rhs.G, B * rhs.B);
}

//**********************
// operator*=
//
// multiply with factor
//**********************
cgi::RGBColor& cgi::RGBColor::operator*=(const double factor){
  R *= factor;
  G *= factor;
  B *= factor;
  return *this;
}

//*********************
// operator*
//
// multiply with factor
//*********************
cgi::RGBColor cgi::RGBColor::operator* (const double factor) const {
  return RGBColor(factor * R, factor * G, factor * B);
}
    
//********************
// operator! 
//
// complementary color
//******************** 
cgi::RGBColor cgi::RGBColor::operator! () const {
  return RGBColor(1.0 - R, 1.0 - G, 1.0 - B);
}
    
//*****************************************
// Clip
//
// returns the color clipped to 0.0 ... 1.0
//*****************************************
cgi::RGBColor cgi::RGBColor::Clip() const {
  return RGBColor(R < 0.0 ? 0.0 : R > 1.0 ? 1.0 : R,
                  G < 0.0 ? 0.0 : G > 1.0 ? 1.0 : G,
                  B < 0.0 ? 0.0 : B > 1.0 ? 1.0 : B);
}

//*****************************************
// BlendTo
//
// blends the current color to the given color
//*****************************************
cgi::RGBColor cgi::RGBColor::BlendTo(const RGBColor& rhs, const double factor) const {
  return RGBColor((1.0 - factor) * R + factor * rhs.R, 
	                (1.0 - factor) * G + factor * rhs.G, 
				          (1.0 - factor) * B + factor * rhs.B);
}

//*****************************************
// IsBlack
//
// checks if the color is black
//*****************************************
bool cgi::RGBColor::IsBlack() const {
  return (R == 0.0 && G == 0.0 && B == 0.0);
}

//*********
// toString
//*********
std::string cgi::RGBColor::ToString() const{
  std::ostringstream os;
  os << "RGBColor(" << R << ", " << G << ", " << B << ")";
  return os.str();  
}


//***************************
// global operator*
//
// product with scalar factor
//***************************
cgi::RGBColor operator* (const double factor, const cgi::RGBColor& rhs){
  return cgi::RGBColor(rhs.R * factor, rhs.G * factor, rhs.B * factor);
}


//************************************************************************
//*                                                                      *
//*                               Vector3D                               *
//*                                                                      *
//* three dimension vector for computer graphics for vectors and points  *
//* this is a specialized version instead of the mathutils::Vector class *                                                  *
//************************************************************************

//************
// constructor
//************
cgi::Vector3D::Vector3D(const double x, const double y, const double z) : X(x), Y(y), Z(z), Active(false) {}
    
//****************
// copy contructor 
//****************
cgi::Vector3D::Vector3D(const Vector3D& other) : X(other.X), Y(other.Y), Z(other.Z), Active(other.Active) {}
    
//***********
// destructor
//***********
cgi::Vector3D::~Vector3D() {}
    
//********************
// assignment operator
//********************
cgi::Vector3D& cgi::Vector3D::operator=(const Vector3D& other){
  if (&other == this) return *this;
  X = other.X; 
  Y = other.Y; 
  Z = other.Z;
  Active = other.Active;
  return *this;
}
    
//********************
// operator+=
//
// adds a Vector
//********************
cgi::Vector3D& cgi::Vector3D::operator+= (const Vector3D& rhs){
  X += rhs.X; 
  Y += rhs.Y; 
  Z += rhs.Z;
  return *this;
}

//*****************
// operator+
//
// adds two Vectors
//*****************
cgi::Vector3D cgi::Vector3D::operator+ (const Vector3D& rhs) const {
  return Vector3D(*this) += rhs;
}

//*******************
// operator+
//
// subtracts a Vector
//*******************
cgi::Vector3D& cgi::Vector3D::operator-= (const Vector3D& rhs){
  X -= rhs.X; 
  Y -= rhs.Y; 
  Z -= rhs.Z;
  return *this;
}

//*******************
// operator-
//
// subtracts a Vector
//*******************
cgi::Vector3D cgi::Vector3D::operator- (const Vector3D& rhs) const {
  return Vector3D(*this) -= rhs;  
}


//******************************
// operator*
//
// scalar product of two Vectors
//******************************
double cgi::Vector3D::operator* (const Vector3D& rhs) const{
  return X * rhs.X + Y * rhs.Y + Z * rhs.Z;
}

//*****************************
// operator*
//
// cross product of two Vectors
//******************************
cgi::Vector3D cgi::Vector3D::operator% (const Vector3D& rhs)  const {
  return Vector3D(Y * rhs.Z - Z * rhs.Y, 
                  Z * rhs.X - X * rhs.Z, 
                  X * rhs.Y - Y * rhs.X);
}

//*********************
// operator*= 
// 
// multiply with scalar
//*********************
cgi::Vector3D& cgi::Vector3D::operator*=(const double scalar){
  X *= scalar;
  Y *= scalar;
  Z *= scalar;
  return *this;
}

//*********************
// operator*
//
// multiply with scalar
//*********************
cgi::Vector3D cgi::Vector3D::operator* (const double scalar)  const {
  return Vector3D(*this) *= scalar;
}

//**************
// operator!
// 
// negate vector
//**************
cgi::Vector3D cgi::Vector3D::operator! () const{
  return Vector3D(-X, -Y, -Z);
}

//********************
// operator-
// 
// negate vector (neu)
//********************
cgi::Vector3D cgi::Vector3D::operator- () const {
  return Vector3D(-X, -Y, -Z);
}


//************
// abs
//
// return norm 
//************
double cgi::Vector3D::Abs() const{
  return sqrt(X * X + Y * Y + Z * Z);
}

//*****************************************
// Norm
//
//*****************************************
cgi::Vector3D cgi::Vector3D::Norm() const {
  double length = sqrt(X * X + Y * Y + Z * Z);
  if (length < 1e-6) {
    return Vector3D(0.0, 0.0, 0.0);
  }
  return Vector3D(X / length, Y / length, Z / length);
}

//********
//toString
//********
std::string cgi::Vector3D::ToString() const{
  std::ostringstream os;
  os << "Vector3D(" << X << ", " << Y << ", " << Z << ")";
  return os.str();  
}


//*********************
// global operator*  
//
// mulitply with scalar
//*********************
cgi::Vector3D operator* (const double scalar, const cgi::Vector3D& rhs){
  return cgi::Vector3D(scalar * rhs.X, scalar * rhs.Y, scalar * rhs.Z);
}


//************************************************************************
//*                                                                      *
//*                                 Line                                 *
//*                                                                      *
//*           represents a straight line beween two vectors(points)      *                                     *
//************************************************************************

//************
// constructor
//************ 
cgi::Line::Line(Vector3D* start, 
                Vector3D* end,
                const bool startIsActive, 
                const bool endIsActive, 
                const bool lineActive)
          :Start(start),
           End(end),
           LineActive(lineActive)
{
  Start->Active = startIsActive;
  End->Active = endIsActive;
}         

//******************************************************
// DeletePoints
//
// cleanup method in case the points have no other owner
//******************************************************
void cgi::Line::DeletePoints(){
  delete Start;
  Start = NULL;
  delete End;
  End = NULL;
}

//******************************
// DeepCopy
//
// makes a deep copy of the line
//******************************
cgi::Line* cgi::Line::DeepCopy(){
  Line* copy = new Line(new Vector3D(*Start), 
                        new Vector3D(*End), 
                        Start->Active,
                        End->Active,
                        LineActive);
  return copy;
}

//************************************************************************
//*                                                                      *
//*                             ColoredLine                              *
//*                                                                      *
//*                       Line with additional color                     *
//************************************************************************

//*******************
// default contructor 
//*******************
// ATTENTION: you have to delete the points later explicitely
cgi::ColoredLine::ColoredLine() : Line(new Vector3D(0.0, 0.0, 0.0), 
                                       new Vector3D(0.0, 0.0, 0.0),
                                       false, 
                                       false, 
                                       false),
                                  Color(0.0, 0.0, 0.0), 
                                  Priority(0)
{
}

//************
// constructor
//************ 
cgi::ColoredLine::ColoredLine(Vector3D* start, 
                              Vector3D* end,
                              RGBColor color,
                              int priority,
                              const bool startIsActive, 
                              const bool endIsActive, 
                              const bool lineActive)
                 :Line(start, end, startIsActive, endIsActive, lineActive),
                  Color(color), Priority(priority)
{
}

//******************************
// DeepCopy
//
// makes a deep copy of the line
//******************************
cgi::ColoredLine* cgi::ColoredLine::DeepCopy(){
  ColoredLine* copy = new ColoredLine(new Vector3D(*Start), 
                                      new Vector3D(*End), 
                                      Color,
                                      Priority,
                                      Start->Active,
                                      End->Active,
                                      LineActive);
  return copy;
}
