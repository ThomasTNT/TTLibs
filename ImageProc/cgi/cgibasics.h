#pragma once

#include <cmath>
#include <string>
#include <sstream>

namespace cgi{
          
//************************************************************************
//*                                                                      *
//*                              RGBColor                                *
//*                                                                      *
//*      universal representation of an RGB-color with double values     *
//*                                                                      *
//*      0.0 ... 1.0                                                     *
//************************************************************************
class RGBColor{
      
  public:
    
    // default constructor                                                                
    RGBColor();
    
    // constructor 1
    RGBColor(const double r, const double g, const double b);
    
    // constructor 2 8 bit integer 
    RGBColor(const unsigned int r, const unsigned int g, const unsigned int b);
    
    // adds a color
    RGBColor& operator+= (const RGBColor& rhs);
    
    // adds two colors
    RGBColor operator+ (const RGBColor& rhs) const;
    
    // subtracts a color
    RGBColor& operator-= (const RGBColor& rhs);
    
    // subtracts two colors
    RGBColor operator- (const RGBColor& rhs) const;
    
    // multiply with other color
    RGBColor& operator*=(const RGBColor& rhs);
    
    // product of two colors
    RGBColor operator* (const RGBColor& rhs) const;
  
    // multiply with factor
    RGBColor& operator*=(const double factor);
    
    // multiply with factor
    RGBColor operator* (const double factor) const;
  
    // returns the complementary color
    RGBColor operator! () const;
    
    // returns the color clipped to 0.0 ... 1.0
    RGBColor Clip() const;

	  // blends the current color to the given color
	  RGBColor BlendTo(const RGBColor& rhs, const double factor) const;
    
    // checks if the color is black
    bool IsBlack() const;

    //to String
    std::string ToString() const;
    
    //fields are public
    double R, G, B;
};

//************************************************************************
//*                                                                      *
//*                              Material                                *
//*                                                                      *
//*                class representing all surface properties             *
//************************************************************************
class Material {

  public:
  
    Material() = default;

    RGBColor BaseColor{ 0.5, 0.5, 0.5 };
    RGBColor Ambient{ 0.5, 0.5, 0.5 };
    RGBColor Diffuse{ 0.5, 0.5, 0.5 };
    RGBColor Specular{ 0.5, 0.5, 0.5 };
    float    Roughness{ 2.0f };
    RGBColor Reflection{ 0.0, 0.0, 0.0 };
    RGBColor Refraction{ 0.0, 0.0, 0.0 };
    float    RefractionIndex{ 1.0f };
    RGBColor Transparency{ 0.0, 0.0, 0.0 };
    // Texture maps
    //TDib* ColorMap{ nullptr };
    //TDib* BumpMapX{ nullptr };
    //TDib* BumpMapY{ nullptr };
};

//************************************************************************
//*                                                                      *
//*                               Vector3D                               *
//*                                                                      *
//* three dimension vector for computer graphics for vectors and points  *
//* this is a specialized version instead of the mathutils::Vector class *                                                  *
//************************************************************************
class Vector3D{
  
  public:
    
    // contructor
    explicit Vector3D(const double x = 0.0, const double y = 0.0, const double z = 0.0);
    
    // copy contructor 
    Vector3D(const Vector3D& other);
    
    // destructor 
    ~Vector3D();
    
    // assignment operator
    Vector3D& operator=(const Vector3D& other);
    
    // adds a Vector
    Vector3D& operator+= (const Vector3D& rhs);
    
    // adds two Vectors
    Vector3D operator+ (const Vector3D& rhs) const;
    
    // subtracts a Vector
    Vector3D& operator-= (const Vector3D& rhs);
    
    // subtracts a Vector
    Vector3D operator- (const Vector3D& rhs) const;
    
    // scalar product of two Vectors
    double operator* (const Vector3D& rhs) const;
    
    // cross product of two Vectors
    Vector3D operator% (const Vector3D& rhs) const;
  
    // multiply with scalar
    Vector3D& operator*=(const double scalar);
    
    // multiply with scalar
    Vector3D operator* (const double scalar) const;
    
    // negate vector
    Vector3D operator! () const;

    // negate vector (neu) 
    Vector3D operator-() const;
    
    //return norm 
    double Abs() const;

    Vector3D Norm() const;
    
    //to String
    std::string ToString() const;
    
    //fields are public
    double X, Y, Z;
    
    //public flag for active
    bool Active;
};


//************************************************************************
//*                                                                      *
//*                                 Line                                 *
//*                                                                      *
//*           represents a straight line beween two vectors(points)      *                                     *
//************************************************************************
class Line{
  
  public:
    
    // constructor 
    Line(Vector3D* start, 
         Vector3D* end,
         const bool startIsActive = false, 
         const bool endIsActive = false, 
         const bool lineActive = false);
         
    // cleanup method in case the points have no other owner
    void DeletePoints();

    // makes a deep copy of the line
    Line* DeepCopy();

    //fields are public
    Vector3D* Start;
    Vector3D* End;
    
    //public flag for active
    bool LineActive;
};

//************************************************************************
//*                                                                      *
//*                             ColoredLine                              *
//*                                                                      *
//*                       Line with additional color                     *
//************************************************************************
class ColoredLine : public Line{
  
  public:
    
    // default contructor 
    // ATTENTION: you have to delete the points later explicitely
    ColoredLine();

    // constructor 
    ColoredLine(Vector3D* start, 
                Vector3D* end,
                RGBColor color = RGBColor(1.0, 1.0, 1.0),
                const int priority = 0,
                const bool startIsActive = false, 
                const bool endIsActive = false, 
                const bool lineActive = false);
         
    // makes a deep copy of the line
    ColoredLine* DeepCopy();

    //fields are public
    RGBColor Color;
    int Priority;
    
};

} //end of namespace cgi 


//global operators
cgi::RGBColor operator* (const double factor, const cgi::RGBColor& rhs);
cgi::Vector3D operator* (const double scalar, const cgi::Vector3D& rhs);
