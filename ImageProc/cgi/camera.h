#ifndef __CAMERA_H
#define __CAMERA_H

#include "objects.h"

namespace cgi{
  
//*************************************************************************
//*                                                                       *
//*                                Camera                                 *
//*                                                                       *
//*                         camera for rendering                          *
//*                                                                       *
//* A camera is a normal object. It can have children and can be attached *
//* to a parent.                                                          *
//*                                                                       *
//* Camera does not acutally rendering, but doas all the transformation.  * 
//*************************************************************************

enum Projectiontype {NOTHING, PERSPECTIVE, ORTHOGRAPHIC, THREESIXTY};

class Camera : public Object{
  
  public:
    
    // constructor
    Camera(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
           const Vector3D& rot = Vector3D(0.0, 0.0, 0.0), 
           Object* parent = NULL);
           
    // destructor
    virtual ~Camera();
    
    // ------------------------- camera properties -----------------------------
    
    //focus - used for perspective projection
    Camera& SetFocus(const double focus);
    double GetFocus() const;
    
    //real width - used for othographic projection
    Camera&  SetRealWidth(const double width);
    double GetRealWidth();
    
    //projcetion type
    Camera&  SetProjection(Projectiontype proj);
    Projectiontype GetProjectionType() const;
    
    //resolution in pixel
    Camera& SetResolution(const unsigned int x, const unsigned int y);
    void GetResolution(unsigned int  &x, unsigned int  &y) const;
    
    //image ratio (x / y) 
    Camera& SetImageRatio(const double ratio);
    double GetImageRatio() const;

    // --------------------------- transformation ------------------------------
    
    //overwritten object - methode: 
    //calculates point from camera coodinates to parent coordinates
    //camera uses a different calculation to handel pan, tilt, roll correctly
    virtual Vector3D PointInParent(const Vector3D& point);       
    
    //overwritten object - methode: 
    //calculates point from parent coodinates into camera coordinates
    //camera uses a different calculation to handel pan, tilt, roll correctly
    virtual Vector3D ParentPointInThis(const Vector3D& point);
    
    //gets real coordinates of a point on the screen 
    //v-coordinate it given via zPoint
    Vector3D ProjInReal(const int x, const int y, const Vector3D& zPoint);
    
    //render transformation
    bool WorldLineToScreen(const Vector3D& p1, const Vector3D& p2, double &x1, double &y1,  double &x2, double &y2);
    bool LineInScreen(const Vector3D& p1, const Vector3D& p2, double &x1, double &y1, double &x2, double&y2);
    
    
    //???????? do we need this ????????????
    double AngleX(const int x, const Vector3D& point);
    double AngleY(const int y, const Vector3D& point);


    // --------------------------- rendering -----------------------------------
    
    // empty implementation of not used pure virtual method
    virtual void InitRendering(){};
    
    // returns the ray viewing vector
    Vector3D RayDirection(const int x, const int y) const;
    
  protected:
    
    Vector3D RayDirectionPerspective(const int x, const int y) const;
    Vector3D RayDirectionThreeSixty(const int x, const int y) const;

    //HELP functions
    bool IsPointTooLeft(const Vector3D& point);
    bool IsPointTooRight(const Vector3D& point);
    bool IsPointTooHigh(const Vector3D& point);
    bool IsPointTooDeep(const Vector3D& point);

    //fields
    Projectiontype Projection;
    double Focus;
    double D;
    double RealWidth;
    unsigned int ResX, ResY;
    double ImageRatio;     
  
};



} // end of namespace cgi

#endif
