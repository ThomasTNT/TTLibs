#ifndef __LIGHT_H
#define __LIGHT_H

#include "objects.h"

namespace cgi{
  
//*************************************************************************
//*                                                                       *
//*                                  Light                                *
//*                                                                       *
//*                  base class for all kind of light sources             * 
//*************************************************************************
class Light : public Object{

  protected:  
    
    //constrcutor 
    Light(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
          Object* parent = NULL,
          const RGBColor& color = RGBColor(1.0, 1.0, 1.0),
          std::string name = "");

  public:   
           
    //destructor 
    virtual ~Light();
    
    //gets a copy
    virtual Light* DeepCopy();

    //returns the light color 
    RGBColor GetIlumination() const;
    
    //sets the light color 
    void SetIlumination(const RGBColor& color);

    //gets the postion of the light in world coordinates
    Vector3D PositionInWorld();
    
    // resets the object for rendering - not needed
    virtual void InitRendering(){}
    
#ifdef DONT_USE_RTTI
    // save down cast
    virtual Light* IsLight() { return this; }
#endif

  protected:
    
    // the light itself
    RGBColor Color;
    
  private: 
    
    // counter
    static unsigned int count;
    
};

//*************************************************************************
//*                                                                       *
//*                             DirectedLight                             *
//*                                                                       *
//*            base class for all kind of directed light sources          * 
//*************************************************************************
class DirectedLight : public Light{
  protected:  
    
    //constrcutor 
    DirectedLight(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
                  const Vector3D& target = Vector3D(0.0, 0.0, 0.0),
                  Object* parent = NULL,
                  const RGBColor& color = RGBColor(1.0, 1.0, 1.0),
                  std::string name = "");
          
  public:   
           
    //destructor 
    virtual ~DirectedLight();

    //gets a copy
    virtual Light* DeepCopy();
    
    //target, actually we abuse rotation
    DirectedLight& SetTarget(const Vector3D& target);
    Vector3D GetTarget() const;
    DirectedLight& TranslateTarget(const Vector3D& target);
    
    //gets the target of the light in world coordinates
    Vector3D TargetInWorld();
};



//*************************************************************************
//*                                                                       *
//*                             InfiniteLight                             *
//*                                                                       *
//*                   a simple directed light like the sky                *
//*************************************************************************
class InfiniteLight : public DirectedLight{
  
  public:
    
    //constrcutor 
    InfiniteLight(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
                  const Vector3D& target = Vector3D(0.0, 0.0, 0.0),
                  Object* parent = NULL,
                  const RGBColor& color = RGBColor(1.0, 1.0, 1.0),
                  std::string name = "");
   
               
    //destructor 
    ~InfiniteLight();
    
    //gets a copy
    virtual Light* DeepCopy();
};


//*************************************************************************
//*                                                                       *
//*                             SphereLight                               *
//*                                                                       *
//*              a undirected unidirection light like a ideal lamp        *
//*************************************************************************
class SphereLight : public Light{
  
  public:
    
    //constrcutor 
    SphereLight(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
                Object* parent = NULL,
                const RGBColor& color = RGBColor(1.0, 1.0, 1.0),
                std::string name = ""); 
             
    //destructor 
    ~SphereLight();
    
    //gets a copy
    virtual Light* DeepCopy();
};

//*************************************************************************
//*                                                                       *
//*                             SphereLight                               *
//*                                                                       *
//*              a undirected unidirection light like a ideal lamp        *
//*************************************************************************
class Projector : public DirectedLight{
  
  public:
    
    //constrcutor 
    Projector(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
             const Vector3D& target = Vector3D(0.0, 0.0, 0.0),
             Object* parent = NULL,
             const RGBColor& color = RGBColor(1.0, 1.0, 1.0),
             std::string name = "",
             double character = 30.0);  
                     
    //destructor 
    ~Projector();
    
    //gets a copy
    virtual Light* DeepCopy();
    
    //character of the opening angle
    void SetCharacter(const double character);
    double GetCharacter() const;

  protected:
    
    double Character;
    
};



} //end of namespace cgi

#endif
