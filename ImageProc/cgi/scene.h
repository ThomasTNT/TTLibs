#pragma once

#include <list>
#include "objects.h"
#include "polygons.h"
#include "camera.h"
#include "light.h"

namespace cgi{
  
//************************************************************************
//*                                                                      *
//*                                 scene                                *
//*                                                                      *
//* scene represents the scene that is to render. It you want, a kind    *
//* of a virtual studio with virtual objects.                            *
//*                                                                      * 
//* practically it is a container including objects (bodies, references, *
//* lightsources and the camera.                                         *
//*                                                                      *
//* it has one root reference that cannot be changed                     *
//*                                                                      *
//* scene it the abstract base class for all kind of scenes.             *
//*                                                                      *
//* Scene becomes owner of all objects. You have not and you must not    *
//* delete it by yourself!                                               *
//************************************************************************  
class Scene{
  
  protected:
    
    // protected constructor to prevent instantiation
    Scene();
    
  public:
    
    // destructor
    virtual ~Scene();  
    
    //--------------------------- object creation ------------------------------
  
    // creates new Reference (Scene keeps owner!) 
    // a Scene can only produce references since it can have only references
    Reference* CreateNewReference(const Vector3D& pos,
                                  const Vector3D& rot, 
                                  Object* parent,
                                  const std::string& name); 
                                  
    //------------------------- object manipulation ----------------------------
    
    Object* GetObjectByName(const std::string& name);
    
    //ATTENTION: obj and all its children are invalid after deleting!
    void DeleteObjectAndChildren(Object* obj);
    
    //ATTENTION: obj is invalid after deleting!
    void DeleteObjectOnly(Object* obj);
    
    void MoveObjectToNewParent(Object* obj, Object* newParent);
    void InsertObjectBefore(Object* obj, Object* other);
    //obj will adopt all children of other
    void InsertObjectAfter(Object* obj, Object* other);
    
    void ActivateObject(Object& obj);
    void DeactivateObject(Object& obj);
    
    //returns the camera object for manipulation
    Camera& GetCamera();
    
    //------------------------------- rendering --------------------------------
    
    // resets the scene for rendering
    virtual void InitRendering() = 0;
    
    //return true when there is absolutly nothing to render
    virtual bool Empty() const;
    
  protected:
    
    
    // deletes all objects
    void CleanUp();
    
    // the root reference
    Reference World;
    
    // the render camera
    Camera Cam;
    
    // list of all objects
    std::list<std::unique_ptr<Object>> ObjectList;
    std::list<std::unique_ptr<Object>>::iterator IteratorObject;
  
};
  
//************************************************************************
//*                                                                      *
//*                               WireScene                              *
//*                                                                      *
//*  base class for scenes that can be rendered with a WireRenderer      *
//*                                                                      *
//************************************************************************  
class WireScene : public Scene{ 
  
  protected:
    
    // protected constructor
    WireScene();
  
  public: 
      
    // destructor
    ~WireScene();  
    
    //------------------------------- rendering --------------------------------
    
    // resets the scene for rendering
    virtual void InitRendering();
    
    // return true when there is absolutly nothing to render
    virtual bool Empty() const;
    
    // get a line in screen coordinates, return true as long as there are anymore lines
    virtual unsigned int GetNextScreenLine(double &x0, double &y0, double &x1, double &y1, RGBColor &color, int &priority) = 0;
    virtual unsigned int GetNextScreenLine(ColoredLine &line) = 0;
    
    // set back color
    void SetBackColor(const RGBColor& color);
    
    // get back color
    RGBColor GetBackColor();
    
  protected: 
    
    RGBColor BackColor; 
};
  
//************************************************************************
//*                                                                      *
//*                            SimpleWireScene                           *
//*                                                                      *
//*                A simple scene for rendering wire objects              *
//*                There is no illumination nor fog ect.                 *
//************************************************************************  
class SimpleWireScene : public WireScene{  
  
  public: 
    
    // constructor
    SimpleWireScene();
  
    // destructor
    ~SimpleWireScene();  
    
    //--------------------------- object creation ------------------------------
  
    // creates new WireObject (Scene keeps owner!) 
    WireObject* CreateNewWireObject(const Vector3D& pos,
                                    const Vector3D& rot, 
                                    Object* parent,
                                    const std::string& name); 
    
    // creates new WireObject (Scene keeps owner!) 
    WireObject* CreateCube(const double size, 
                           Object* parent,
                           const std::string& name); 

    //------------------------------- rendering --------------------------------
    
    // get a line in screen coordinates, return true as long as there are anymore lines
    virtual unsigned int GetNextScreenLine(double &x0, double &y0, double &x1, double &y1, RGBColor &color, int &priority);
    virtual unsigned int GetNextScreenLine(ColoredLine &line);
};


//************************************************************************
//*                                                                      *
//*                            PolygonalScene                            *
//*                                                                      *
//*                A scene for rendering polygonal objects               *
//************************************************************************  
class PolygonalScene : public Scene {

  public: 
    
    // constructor
    PolygonalScene();
  
    // destructor
    ~PolygonalScene();  
    
    //--------------------------- object creation ------------------------------
    
    // creates new PolygonObject (Scene keeps owner!) 
    PolygonObject* CreateCube(const double size, 
                              Object* parent,
                              const std::string& name); 
                              
    // creates new WireObject (Scene keeps owner!) 
    PolygonObject* CreateQuader(const Vector3D& pos,
                                const Vector3D& rot,
                                const double sizeX,
                                const double sizeY,
                                const double sizeZ,
                                Object* parent,
                                const std::string& name);

    // creates new PolygonObject (Scene keeps owner!) 
    PolygonObject* CreateSphere(const double radius, 
                                const unsigned int n,
                                Object* parent,
                                const std::string& name); 

    // creates new PolygonObject (Scene keeps owner!) 
    PolygonObject* CreateCircle(const Vector3D& pos,
                                const Vector3D& rot,
                                const double radius,
                                const unsigned int n,
                                Object* parent,
                                const std::string& name);


    // creates new PolygonObject (Scene keeps owner!) 
    PolygonObject* CreatePolygonObject(const Vector3D& pos,
                                       const Vector3D& rot, 
                                       Object* parent,
                                       std::string name); 
  
    // creates a new infinite light source
    InfiniteLight* CreateInfiniteLight(const Vector3D& pos,
                                       const Vector3D& target,
                                       Object* parent,
                                       const RGBColor& color,
                                       std::string name);
    
    // creates a new infinite light source
    SphereLight* CreateSphereLight(const Vector3D& pos,
                                   Object* parent,
                                   const RGBColor& color,
                                   std::string name);                                   
                                       
    // creates a new projector
    Projector* CreateProjector(const Vector3D& pos,
                               const Vector3D& target,
                               Object* parent,
                               const RGBColor& color,
                               std::string name,
                               double character);
                                      
    //------------------------------- rendering --------------------------------

    // Getter for the background color
    const RGBColor& GetBackColor() const;

    // Setter for the background color
    void SetBackColor(const RGBColor& color);

    // Getter for the ambient light color
    const RGBColor& GetAmbientLight() const;

    // Setter for the ambient light color
    void SetAmbientLight(const RGBColor& color);
    
    // resets the scene for rendering
    virtual void InitRendering();
    
    // get a line in screen coordinates, return true as long as there are anymore lines
    virtual unsigned int GetNextScreenLine(double &x0, double &y0, double &x1, double &y1, RGBColor &color, int &priority);
    virtual unsigned int GetNextScreenLine(ColoredLine &line); 
    
    // get the next object
    PolygonObject* NextPolyObject();
    
    // get the next light
    Light* NextLight();
    
  protected: 
    
    RGBColor BackColor;
    RGBColor AmbientLight;

    std::list<Light*> Lights;
    std::list<Light*>::iterator IteratorLights;
    
};    

} //end of namespace cgi 
