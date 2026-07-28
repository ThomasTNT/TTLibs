#include "camera.h"

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

//************ 
// constructor
//************
cgi::Camera::Camera(const Vector3D& pos, const Vector3D& rot, Object* parent)
            :Object(pos, rot, parent),
             Focus(50.0),
             D(1.0 /tan(M_PI * Focus / 360.0)),
             RealWidth(100.0),
             Projection(PERSPECTIVE),
             ResX(768),
             ResY(576),
             ImageRatio(ResX / ResY) //assuming quadratic pixels
{
}
           
//***********         
// destructor
//***********
cgi::Camera::~Camera(){}
    
// --------------------------- camera properties -------------------------------

//*********
// SetFocus
//*********
cgi::Camera& cgi::Camera::SetFocus(const double focus){ 
  if (focus > 0){ 
    Focus = focus; 
    D = 1.0 / tan(M_PI * Focus / 360.0);
  }
  return *this;
}

//*********
// GetFocus
//*********
double cgi::Camera::GetFocus() const{ 
  return Focus; 
}
    
//*************
// SetRealWidth
//*************
cgi::Camera& cgi::Camera::SetRealWidth(const double width){
  RealWidth = width;
  return *this;
}

//*************
// GetRealWidth
//*************
double cgi::Camera::GetRealWidth(){ 
  return RealWidth; 
}

//*********************
// SetProjection
//
// sets projection type
//*********************
cgi::Camera& cgi::Camera::SetProjection(cgi::Projectiontype proj){
  Projection = proj;
  return *this;
}

//************************
// GetProjection
//
// returns projection type
//************************
cgi::Projectiontype cgi::Camera::GetProjectionType() const{
  return Projection;
}

//********************
// SetResolution
//
// resolution in pixel
//********************
cgi::Camera& cgi::Camera::SetResolution(const unsigned int x, const unsigned int y){
  ResX = x;
  ResY = y;
  return *this;
}

//********************
// GetResolution
//
// resolution in pixel
//********************
void cgi::Camera::GetResolution(unsigned int &x, unsigned int  &y) const{
  x = ResX;
  y = ResY;
}

//***************************************
// SetRatio
//
// sets ratio for anamorphotic distortion
//***************************************
cgi::Camera& cgi::Camera::SetImageRatio(const double ratio){
  ImageRatio = ratio;
  return *this;
}

//******************************************
// GetRatio
//
// returns ratio for anamorphotic distortion
//******************************************
double cgi::Camera::GetImageRatio() const{
  return ImageRatio;
}

// ----------------------------- transformation --------------------------------

//************************************************************************
// PointInParent
//
// overwritten object - methode: 
// calculates point from camera coodinates to parent coordinates
// camera uses a different calculation to handel pan, tilt, roll correctly
//************************************************************************
cgi::Vector3D cgi::Camera::PointInParent(const Vector3D& point){
  const double tilt = M_PI * Rotation.X / 180.0; 
  const double SA = sin(tilt); 
  const double CA = cos(tilt);
  const double pan  = M_PI * Rotation.Y / 180.0; 
  const double SB = sin(pan); 
  const double CB = cos(pan);
  const double roll = M_PI * Rotation.Z / 180.0; 
  const double SG = sin(roll); 
  const double CG = cos(roll);
  return Vector3D(
    ( CB * CG + SA * SB * SG) * point.X + (-CB * SG+SA * SB * CG) * point.Y + CA * SB * point.Z + Position.X,
                     CA * SG  * point.X +                CA * CG  * point.Y -      SA * point.Z + Position.Y,
    (-SB * CG  +SA * CB * SG) * point.X + ( SB * SG+SA * CB * CG) * point.Y + CA * CB * point.Z + Position.Z
  );
}      

//************************************************************************
// ParentPointInThis
//
// overwritten object - methode: 
// calculates point from parent coodinates into camera coordinates
// camera uses a different calculation to handel pan, tilt, roll correctly
//************************************************************************
cgi::Vector3D cgi::Camera::ParentPointInThis(const Vector3D& point){
  const double tilt = M_PI * Rotation.X / 180.0; 
  const double SA = sin(tilt); 
  const double CA = cos(tilt);
  const double pan  = M_PI * Rotation.Y / 180.0; 
  const double SB = sin(pan ); 
  const double CB = cos(pan );
  const double roll = M_PI * Rotation.Z / 180.0; 
  const double SG = sin(roll); 
  const double CG = cos(roll);
  return Vector3D(
      ( CB * CG + SA * SB * SG) * point.X    + CA * SG * point.Y    + (-SB * CG + SA * CB * SG) * point.Z
    + (-CB * CG - SA * SB * SG) * Position.X - CA * SG * Position.Y - (-SB * CG + SA * CB * SG) * Position.Z,

      (-CB * SG + SA * SB * CG) * point.X    + CA * CG * point.Y    + ( SB * SG + SA * CB * CG) * point.Z
    + ( + CB * SG-SA * SB * CG) * Position.X - CA * CG * Position.Y - ( SB * SG + SA * CB * CG) * Position.Z,

                       CA * SB  * point.X -    SA * point.Y         +                  CA * CB  * point.Z
                      -CA * SB  * Position.X + SA * Position.Y      -                  CA * CB  * Position.Z
  );  
}

//***********************************************
// ProjInReal
//
// gets real coordinates of a point on the screen 
// v-coordinate it given via zPoint
//***********************************************
cgi::Vector3D cgi::Camera::ProjInReal(const int x, const int y, const Vector3D& zPoint){
  
  const Vector3D last = WorldPointInThis(zPoint);
  
  if (Projection == PERSPECTIVE || Projection == THREESIXTY){
    const Vector3D real((2.0 * static_cast<double>(x) / static_cast<double>(ResX) - 1.0) * last.Z / D,
                        (1.1 - 2.0 * static_cast<double>(y) / static_cast<double>(ResY)) * last.Z / (D * ImageRatio),
                        last.Z);
    return PointInParent(real);
  }
  else{
    const Vector3D real((2.0 * static_cast<double>(x) / static_cast<double>(ResX) - 1.0) * RealWidth / 2.0,
                        (static_cast<double>(ResY) / 2.0 - static_cast<double>(y)) * RealWidth / static_cast<double>(ResX),
                        last.Z);
    return PointInParent(real);
  }
}


//**********************
// WorldLineToScreen
// 
// render transformation
//**********************
bool cgi::Camera::WorldLineToScreen(const Vector3D& p1, const Vector3D& p2, 
                                    double &x1, double &y1, double &x2, double &y2)
{
  return LineInScreen(WorldPointInThis(p1), WorldPointInThis(p2), x1, y1, x2, y2);
}


//**********************
// LineInScreen
// 
// render transformation
//**********************
bool cgi::Camera::LineInScreen(const Vector3D& p1, const Vector3D& p2, 
                               double &x1, double &y1, double &x2, double &y2)
                               
{
  Vector3D point1, point2;
  Vector3D ps;
  
  double t, tz, tn;
  
  //Logger::getInstance().println("Camera::LineInScreen: " + p1.ToString() + " " + p2.ToString());
  
  switch (Projection){
  //------------------------ perspective projection ----------------------------
  case PERSPECTIVE:
    
    //do nothing if line is outside of the view 
    if (   (((p1.X == 0.0) && (p1.Y == 0.0) && (p1.Z == 0.0)) || ((p2.X == 0.0) && (p2.Y == 0.0) && (p2.Z == 0.0))) 
        || ((p1.Z >= 0.0) && (p2.Z >= 0.0))
        || (IsPointTooLeft(p1)  && IsPointTooLeft(p2))
        || (IsPointTooRight(p1) && IsPointTooRight(p2))
        || (IsPointTooHigh(p1)  && IsPointTooHigh(p2))
        || (IsPointTooDeep(p1)  && IsPointTooDeep(p2))
       )
    { 
      //Logger::getInstance().println("Camera::LineInScreen: OUTSIDE");
      return false;
    }
    
    //clipping left side
    tz = p1.Z - D * p1.X; 
    tn = p1.Z - p2.Z - D * (p1.X - p2.X);
    if (!(tn == 0)){  //falls Nenner == 0  = >  t = unendlich  = >  parallel, kein Clipping
      t = tz / tn;
      if ((t > 0) && (t < 1)){  //nur in diesem Fall wird Strecke geschitten
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (IsPointTooLeft(p1)){
          point1 = ps; 
          point2 = p2;
        }
        else {
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //clipping right side
    tz = p1.Z + D * p1.X; 
    tn = p1.Z - p2.Z + D * (p1.X - p2.X);
    if (!(tn == 0)){
      t = tz / tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (IsPointTooRight(p1)){
          point1 = ps; 
          point2 = p2;
        }
        else {
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //clipping top
    tz = p1.Z + D * ImageRatio * p1.Y; 
    tn = p1.Z - p2.Z + D *  ImageRatio * (p1.Y - p2.Y);
    if (!(tn == 0)){
      t = tz / tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (IsPointTooHigh(p1)){
          point1 = ps; 
          point2 = p2;
        }
        else {
          point1 = p1; 
          point2 = ps;
          }
      }
    }
    //clipping bottom
    tz = p1.Z - D * ImageRatio * p1.Y; 
    tn = p1.Z - p2.Z - D * ImageRatio * (p1.Y - p2.Y);
    if (!(tn == 0)){
      t = tz / tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (IsPointTooDeep(p1)){
          point1 = ps; 
          point2 = p2;
        }
        else {
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //transformation oft clipped line
    x1 = (1.0 - D * p1.X / p1.Z) * static_cast<double>(ResX) / 2.0  +  0.5;
    y1 = (1.0 + D * ImageRatio * p1.Y / p1.Z) * static_cast<double>(ResY) / 2.0 + 0.5;
    x2 = (1.0 - D * p2.X / p2.Z) * static_cast<double>(ResX) / 2.0 + 0.5;
    y2 = (1.0 + D * ImageRatio * p2.Y / p2.Z) * static_cast<double>(ResY) / 2.0 + 0.5;
    return true;
  
  //-------------------------- orthographic projection -------------------------
  case ORTHOGRAPHIC: 
     
    const double left   = -RealWidth / 2.0;
    const double right  =  RealWidth / 2.0;
    const double top    =  RealWidth / (2.0 * ImageRatio);
    const double bottom = -RealWidth / (2.0 * ImageRatio);
    
    if (   ((p1.Z > 0.0) && (p2.Z > 0.0))
        || ((p1.X < left) && (p2.X < left))
        || ((p1.X > right) && (p2.X > right))
        || ((p1.Y < bottom) && (p2.Y < bottom))
        || ((p1.Y > top) && (p2.Y > top)))
    { 
      return false;
    }
    
    //clipping left
    tn = p2.X - p1.X; 
    tz = left - p1.X;
    if (!(tn == 0)){
      t = tz / tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (p1.X<p2.X){
          point1 = ps; 
          point2 = p2;
        }
        else{
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //clipping right
    tn = p2.X - p1.X; 
    tz = right - p1.X;
    if (!(tn == 0)){
      t = tz / tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (p1.X>p2.X){
          point1 = ps; 
          point2 = p2;
        }
        else{
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //clipping bottom
    tn = p2.Y - p1.Y; 
    tz = bottom - p1.Y;
    if (!(tn == 0)){
      t = tz/tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (p1.Y<p2.Y){
          point1 = ps; 
          point2 = p2;
        }
        else{
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //clipping top
    tn = p2.Y - p1.Y; 
    tz = top - p1.Y;
    if (!(tn == 0)){
      t = tz / tn;
      if ((t > 0) && (t < 1)){
        ps = Vector3D(p1.X + t * (p2.X - p1.X), p1.Y + t * (p2.Y - p1.Y), p1.Z + t * (p2.Z - p1.Z));
        if (p1.Y > p2.Y){
          point1 = ps; 
          point2 = p1;
        }
        else{
          point1 = p1; 
          point2 = ps;
        }
      }
    }
    //transformation oft clipped line
    x1 = (1.0 + 2.0 * p1.X / RealWidth) * static_cast<double>(ResX) / 2.0;
    y1 = static_cast<double>(ResY) / 2.0 - p1.Y * static_cast<double>(ResX) / RealWidth;
    x2 = (1.0 + 2.0 * p2.X / RealWidth) * static_cast<double>(ResX) / 2.0;
    y2 = static_cast<double>(ResY)/2 - p2.Y * static_cast<double>(ResX) / RealWidth;
    return true;
    
  } //end of switch

  //when we are her something went wrong
  return false;
}

//**************************************
// AngleX
// 
// ???????? do we need this ????????????
//**************************************
double cgi::Camera::AngleX(const int x, const Vector3D& point){
  if (Projection == PERSPECTIVE){
    return 180 * atan(((1.0 - 2.0 * static_cast<double>(x) / static_cast<double>(ResX))) / D) / M_PI;
  }
  else{
    //?????????? why, it's the same ......
    return 180 * atan(((1.0 - 2.0 * static_cast<double>(x) / static_cast<double>(ResX))) / D) / M_PI;
  }
}

//**************************************
// AngleY
// 
// ???????? do we need this ????????????
//**************************************
double cgi::Camera::AngleY(const int y, const Vector3D& point){
  if (Projection==PERSPECTIVE){
    return 180 * atan(((1.0 - 2.0 * static_cast<double>(y) / static_cast<double>(ResY)) / ImageRatio) / D) / M_PI;
  }
  else{
    //?????????? why, it's the same ......
    return 180 * atan(((1.0 - 2.0 * static_cast<double>(y) / static_cast<double>(ResY)) / ImageRatio) / D) / M_PI;
  }
}

//************************************************************
// RayDirection
//
// returns the ray viewing vector for given screen coordinates
//************************************************************
cgi::Vector3D cgi::Camera::RayDirection(const int x, const int y) const{
  switch (Projection){
    case PERSPECTIVE: return RayDirectionPerspective(x, y);
    case THREESIXTY: return RayDirectionThreeSixty(x, y);
    case ORTHOGRAPHIC: //not implemented
                       break;
  }
}
  

//************************************************************
// RayDirectionPerspective
//
// returns the ray viewing vector for given screen coordinates
//************************************************************
cgi::Vector3D cgi::Camera::RayDirectionPerspective(const int x, const int y) const{
  return Vector3D(2.0 * static_cast<double>(x) / static_cast<double>(ResX) - 1.0, 
                 (2.0 * static_cast<double>(y) / static_cast<double>(ResY) - 1.0) / ImageRatio, 
                  -D).Norm();
}

//************************************************************
// RayDirectionThreeSixty
//
// returns the ray viewing vector for given screen coordinates
//************************************************************
cgi::Vector3D cgi::Camera::RayDirectionThreeSixty(const int x, const int y) const{
 
  const float lambda = (static_cast<float>(x) / (ResX - 1.0f)) * (2.0f * M_PI) - M_PI;
  const float phi = M_PI / 2.0f - (static_cast<float>(y) / (ResY - 1.0f)) * M_PI;

  return Vector3D(std::cos(phi) * std::sin(lambda),
                  std::sin(phi),
                  std::cos(phi) * std::cos(lambda)).Norm();

}




// ------------------------------ HELP functions--------------------------------

//********************
// HELP IsPointTooLeft
//********************    
bool cgi::Camera::IsPointTooLeft(const Vector3D& point){
  return (point.Z == 0) || ((D * point.X / fabsl(point.Z)) > 1.0);
}

//*********************
// HELP IsPointTooRight
//*********************
bool cgi::Camera::IsPointTooRight(const Vector3D& point){
  return (point.Z == 0) || ((-D * point.X / fabsl(point.Z)) > 1.0);
}

//********************
// HELP IsPointTooHigh
//********************
bool cgi::Camera::IsPointTooHigh(const Vector3D& point){
  return (point.Z == 0) || ((-D * point.Y / fabsl(point.Z)) > (1.0 / ImageRatio)); 
}

//********************
// HELP IsPointTooDeep
//********************
bool cgi::Camera::IsPointTooDeep(const Vector3D& point){
  return (point.Z == 0) || ((D * point.Y / fabsl(point.Z)) > (1.0 / ImageRatio)); 
}

    

