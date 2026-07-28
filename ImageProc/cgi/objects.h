#ifndef __Objects_H
#define __Objects_H

#define _USE_MATH_DEFINES

#include <cmath>
#include <set>
#include <vector>
#include <list>
#include <sstream>
#include <iostream> //debug 

#ifndef NOT_IN_PS_PLUGIN
  //#include "Logger.h"
#endif

#include "cgibasics.h"

#ifdef _MSC_VER
  #ifndef _CPPRTTI
    #define DONT_USE_RTTI
  #endif
#endif

namespace cgi{
  
#ifdef DONT_USE_RTTI
  //forward deklaration for save down casts
  class Reference;
  class WireObject;
  class Light;
#endif

//************************************************************************
//*                                                                      *
//*                                Object                                *
//*                                                                      *
//* base class for any graphical object                                  *
//* An object can be a reference, a polygon, a lightsource etc.pp        *
//* Even a camera is an object.                                          *
//*                                                                      *
//* An object has a position, a rotation and can have a couble of child  *
//* objects which have as coordinate system  relative to their parent.   *
//* So it is possible tp create hierachies of objects.                   *                                                                                                           *
//*                                                                      *
//* Use Reference for simply place holder object which acts as root for  *
//* new coordinate system.                                               *
//************************************************************************
class Object{
  
    //------------------------- construction ------------------------------
  
  protected:
      
    // protected constructor to prevent instantiation
    Object(const Vector3D& pos, const Vector3D& rot, Object* parent);
           
  public:
    
    // copy constructor
    Object(Object& other);
    
    // destructor 
    virtual ~Object();
    
    // assignement operator 
    virtual Object& operator=(const Object& other);
    
    // add operator to combine objects - does nothing here
    virtual Object& operator+=(const Object& other) { return *this; }
    
    //------------------------------ down cast ---------------------------------

#ifdef DONT_USE_RTTI
    virtual WireObject* IsWireObject() { return NULL; }
    virtual Reference*  IsReference() { return NULL; }
    virtual Light*      IsLight() { return NULL; }
#endif    

    //----------------------- manipulating hierarchy ---------------------------
    
    //hierarchy manipulation
    Object* GetHierarchyParent() const;
    
    //hierarchy manipulation 
    Object& SetHierarchyParent(Object* parent); 
    
    //hierarchy manipulation
    std::set<Object*> GetHierarchyChildren() const;
    
    //hierarchy manipulation
    Object& AddHierarchyChild(Object* child);
    
    //hierarchy manipulation
    Object& DelHierarchyChild(Object* child);
    
    //hierarchy manipulation
    Object& SetHierarchyNewChildren(const std::set<Object*>& children);
    
    //hierarchy manipulation
    Object& SetHierarchyNewParentForOurChildren(Object* parent);

    //---------------- manipulating geometry and coordinates -------------------

    //position
    Object& SetPosition(const Vector3D& pos);
    Vector3D GetPosition() const;
    Object& Translate(const Vector3D& pos);
    
    //rotation
    Object& SetRotation(const Vector3D& rot);
    Vector3D GetRotation() const;
    Object& Rotate(const Vector3D& rot);
    
    //scale
    Object& SetScale(const Vector3D&  scale);
    Vector3D GetScale() const;
    Object& Scaling(const Vector3D&  scale);
    
    //sets scale to (1.0, 1.0, 1.0) and physically applies scale to eachs point
    //you have to call Object::ApplyScale() in your derived method
    //virtual void ApplyScale();

    //transformation
    Vector3D PointInWorld(const Vector3D& point);        
    virtual Vector3D PointInParent(const Vector3D& point);
    Vector3D WorldPointInThis(const Vector3D& point);
    virtual Vector3D ParentPointInThis(const Vector3D& point);

    //-------------------------- object properties  ----------------------------
    
    virtual Object* SetActive(const bool active);
    bool GetActive() const;
    
    Object& SetName(const std::string& name);
    std::string GetName() const;
    Object& SetIdentity(const std::string& id);
    std::string GetIdentity() const;
    
    //------------------------------ rendering ---------------------------------
    
    // resets the object for rendering
    virtual void InitRendering() = 0;
    
    //--------------------------------- info -----------------------------------
    
    //prints object only
    std::string ToString();
    
    //prints whole hierarchy
    std::string HierarchyToString();
    
  private: 
    
    // HELP method 
    void GetNameWidths(std::vector<unsigned int>& widths, unsigned int depth);
    
    // HELP method 
    void BuildHierarchyToString(std::list<std::vector<std::string> >& paths,
                                std::vector<std::string>& path, 
                                unsigned int depth);
    
  protected:
    
    //fields 
    Vector3D Position;
    Vector3D Rotation;
    Vector3D Scale;
    
    Object*            Parent;
    std::set<Object*>  Children;
    bool               Active;
    std::string        Name;
    std::string        Identity;
};


//************************************************************************
//*                                                                      *
//*                              Reference                               *
//*                                                                      *
//* Reference is an empty object which acts as a new coordinate system.  *
//* It is to build an hierachie of graphical objects.                    *
//************************************************************************
class Reference : public Object{
  
  public:
    
    // constructor will be called from scene only
    Reference(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
              const Vector3D& rot = Vector3D(0.0, 0.0, 0.0), 
              Object* parent = NULL,
              std::string name = "");

#ifdef DONT_USE_RTTI
    // save down cast
    virtual Reference* IsReference() { return this; }
#endif
             
    // destrutor 
    virtual ~Reference();
    
    // empty implementation of not used pure virtual method
    virtual void InitRendering(){};
    
  private: 
    
    // "reference counter" hihi
    static unsigned int count;
};

//***************************************************************************
//*                                                                         *
//*                         ObjectWithPoints                                *
//*                                                                         *
//* This is the base class for all kind of objects that has got any points. *
//* All points have to be stored in the point list to access them           *
//***************************************************************************
class ObjectWithPoints : public Object{
  
  protected:
    
    // protected constructor to prevent instantiation
    ObjectWithPoints(const Vector3D& pos, const Vector3D& rot, Object* parent);
    
  public: 
    
    // destructor deletes all points
    virtual ~ObjectWithPoints();
    
    //changing the points of the object actually 
    void TranslateObject(const Vector3D& trans);
    void RotateObject(const Vector3D& rot);
    void ScaleObject(const Vector3D& scale); 
    
  protected: 
    
    std::set<Vector3D*> Points; 
    std::set<Vector3D*>::iterator InteratorPoints; 
};

//***************************************************************************
//*                                                                         *
//*                             WireRenderObject                            *
//*                                                                         *
//* This is the base class for all kind of objects that can be renders      *
//* with a wire renderer. It only some kind of a marker interface.          *
//***************************************************************************
class WireRenderObject : public ObjectWithPoints{
  
  public:
    
    // constructor 
    WireRenderObject(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
                     const Vector3D& rot = Vector3D(0.0, 0.0, 0.0), 
                     Object* parent = NULL);
    
    // destructor deletes all lines but not the points
    virtual ~WireRenderObject();
    
    //get the next line of the object, return false when there are no more lines
    virtual bool NextLineInWorld(Vector3D &p0, Vector3D &p1, RGBColor& color, int &priority) = 0;
};


//***************************************************************************
//*                                                                         *
//*                               WireObject                                *
//*                                                                         *
//* A simple object consists only of points and streight lines.             *
//* It has no surfaces, no colors and therefor no material.                 *
//***************************************************************************
class WireObject : public WireRenderObject{
  
  public:
    
    // constructor 
    WireObject(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
               const Vector3D& rot = Vector3D(0.0, 0.0, 0.0), 
               Object* parent = NULL,
               std::string name = "");
    
    // destructor deletes all lines but not the points
    virtual ~WireObject();

#ifdef DONT_USE_RTTI
    // save down cast
    virtual WireObject* IsWireObject() { return this; }
#endif

    // adds a new line to the object
    WireObject& AddLine(ColoredLine* line);
    WireObject& AddLine(Vector3D* p0, Vector3D* p1, const RGBColor& color = RGBColor(1.0, 1.0, 1.0), const int priority = 0);
    WireObject& CreateCube(const double size);

    WireObject& SetPriority(const int priority);

    //------------------------------ rendering ---------------------------------
    
    // resets the object for rendering
    virtual void InitRendering();
    
    //get the next line of the object, return false when there are no more lines
    virtual bool NextLineInWorld(Vector3D &p0, Vector3D &p1, RGBColor &color, int &priority);
    virtual bool NextLineInWorld(ColoredLine &line);
    
  protected: 
    
    std::set<ColoredLine*> Lines;  
    std::set<ColoredLine*>::iterator IteratorLines;
    
  private: 
    
    // counter
    static unsigned int count;
    
};

} // end of namespace cgi


#endif

