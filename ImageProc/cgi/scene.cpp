#include "scene.h"
  
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

//*********************************************** 
// protected constructor to prevent instantiation
//***********************************************
cgi::Scene::Scene() : World(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), NULL),
                      Cam(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), &World)
{
}
 
//***********
// destructor
//***********
cgi::Scene::~Scene(){
  CleanUp();
} 
    
//----------------------------- object creation --------------------------------
  
// creates new Reference (Scene keeps owner!) 
cgi::Reference* cgi::Scene::CreateNewReference(const Vector3D& pos,
                                               const Vector3D& rot, 
                                               Object* parent,
                                               const std::string& name)
{
  //HINT: reference is still referenced by it's parent
  auto ref = std::make_unique<Reference>(pos, rot, parent != nullptr ? parent : &World, std::move(name));
  Reference* refPtr = ref.get();
  ObjectList.push_back(std::move(ref));
  return refPtr;
}
      
//--------------------------- object manipulation ------------------------------
                            
//********************
// CleanUp
//
// deletes all objects
//********************
void cgi::Scene::CleanUp(){
  ObjectList.clear();
}
    
//****************************************
// GetObjectByName
//
// returns the first object with this name
//****************************************
cgi::Object* cgi::Scene::GetObjectByName(const std::string& name){
  for (const auto& o : ObjectList) {
    if (o->GetName() == name) {
      return o.get();
    }
  }
  return nullptr;
}
    
//****************************************************************
// DeleteObjectAndChildren
//
// ATTENTION: obj and all its children are invalid after deleting!
//****************************************************************
void cgi::Scene::DeleteObjectAndChildren(Object* obj){
  //loop through children of this object and let delete them 
  std::set<Object*> children = obj->GetHierarchyChildren();
  for (std::set<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter){
    DeleteObjectAndChildren((*iter));    
  }
  //remove object from child list of its parent
  obj->GetHierarchyParent()->DelHierarchyChild(obj);
  //delete current object at last
  delete obj;
}

//********************************************
// DeleteObjectAndChildren
//
// ATTENTION: obj is invalid after deleting!
//
// all children will be attached to the parent
//********************************************
void cgi::Scene::DeleteObjectOnly(Object* obj){
  //loop through children and get them a new parent (our parent) 
  //add children to parent, too
  Object* parent = obj->GetHierarchyParent();
  std::set<Object*> children = obj->GetHierarchyChildren();
  for (std::set<Object*>::iterator iter = children.begin(); iter != children.end(); ++iter){
    (*iter)->SetHierarchyParent(parent);
    parent->AddHierarchyChild((*iter));
  } 
  //remove object from child list of its parent
  obj->GetHierarchyParent()->DelHierarchyChild(obj);
  //delete current object at last
  delete obj;
}

//**********************
// MoveObjectToNewParent
//**********************
void cgi::Scene::MoveObjectToNewParent(Object* obj, Object* newParent){
  obj->GetHierarchyParent()->DelHierarchyChild(obj);
  newParent->AddHierarchyChild(obj);
}

//*******************
// InsertObjectBefore
//*******************
void cgi::Scene::InsertObjectBefore(Object* obj, Object* other){
  obj->AddHierarchyChild(other);
  other->SetHierarchyParent(obj); 
  Object* parent = other->GetHierarchyParent();
  parent->DelHierarchyChild(other);
  parent->AddHierarchyChild(obj);
  obj->SetHierarchyParent(parent);
}

//******************
// InsertObjectAfter
//******************
void cgi::Scene::InsertObjectAfter(Object* obj, Object* other){
  other->SetHierarchyNewParentForOurChildren(obj);
  obj->SetHierarchyParent(other); 
  other->AddHierarchyChild(obj);
}

//***************
// ActivateObject
//***************
void cgi::Scene::ActivateObject(Object& obj){
  obj.SetActive(true);
}

//***************
// ActivateObject
//***************
void cgi::Scene::DeactivateObject(Object& obj){
  obj.SetActive(false);
}

//******************************************
// GetCam
//
//returns the camera object for manipulation
//******************************************
cgi::Camera& cgi::Scene::GetCamera(){
  return Cam;  
}

//******************************************************
// Empty
//
// return true when there is absolutly nothing to render
//******************************************************
bool cgi::Scene::Empty() const {
  return false;
}

//************************************************************************
//*                                                                      *
//*                               WireScene                              *
//*                                                                      *
//*  base class for scenes that can be rendered with a WireRenderer      *
//*                                                                      *
//************************************************************************  

//**********************
// protected constructor
//**********************
cgi::WireScene::WireScene() : Scene(), BackColor(0.0, 0.0, 0.0) {}
  
//***********
// destructor
//***********
cgi::WireScene::~WireScene() {}  
    
//*******************************
// InitRendering
//
// resets the scene for rendering
//*******************************
void cgi::WireScene::InitRendering(){
  
  //reset all objects for rendering
  for (IteratorObject = ObjectList.begin(); IteratorObject != ObjectList.end(); ++IteratorObject){
    (*IteratorObject)->InitRendering();    
  }
  IteratorObject = ObjectList.begin();
}

//******************************************************
// Empty
//
// return true when there is absolutly nothing to render
//******************************************************
bool cgi::WireScene::Empty() const {
  //@TODO check for renderable objects????
  return ObjectList.empty();   
}

//*************
// SetBackColor
//*************
void cgi::WireScene::SetBackColor(const RGBColor& color){
 BackColor = color;
}

//*************
// GetBackColor
//*************
cgi::RGBColor cgi::WireScene::GetBackColor(){
  return BackColor;
}

//************************************************************************
//*                                                                      *
//*                            SimpleWireScene                           *
//*                                                                      *
//*                A simple scene for rendering wire object              *
//*                There is no illumination nor fog ect.                 *
//************************************************************************  

//************
// constructor
//************
cgi::SimpleWireScene::SimpleWireScene() : WireScene(){}
  
//************
// destructor
//************
cgi::SimpleWireScene::~SimpleWireScene() {}  
    
//********************************************
// CreateNewWireObject
//
// creates new WireObject (Scene keeps owner!) 
//********************************************
cgi::WireObject* cgi::SimpleWireScene::CreateNewWireObject(const Vector3D& pos,
                                                           const Vector3D& rot, 
                                                           Object* parent,
                                                           const std::string& name)
{
  auto wo = std::make_unique<WireObject>(pos, rot, parent != nullptr ? parent : &World, std::move(name));
  WireObject* woPtr = wo.get();
  ObjectList.push_back(std::move(wo));
  return woPtr;
}

//********************************************
// CreateCube
//
// creates new WireObject (Scene keeps owner!) 
//********************************************
cgi::WireObject* cgi::SimpleWireScene::CreateCube(const double size, 
                                                  Object* parent,
                                                  const std::string& name)
{                              
  auto wo = std::make_unique<WireObject>(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), parent != nullptr ? parent : &World, std::move(name));
  wo->CreateCube(size);
  WireObject* woPtr = wo.get();
  ObjectList.push_back(std::move(wo));
  return woPtr;
}

//--------------------------------- rendering ----------------------------------


    
//************************************************
// GetNextScreenLine
//
// get a line in screen coordinates
// returns 0 when there are no more lines
// returns 1 when a line is to draw
// returns 2 when a line exists but is not visible
//************************************************
unsigned int cgi::SimpleWireScene::GetNextScreenLine(double &x0, double &y0, 
                                                     double &x1, double &y1, 
                                                     RGBColor &color,
                                                     int &priority)
{
  //loop through every line of every Wire object 
  //if there are no more lines return false
  if (IteratorObject == ObjectList.end()) return false;
  
  Vector3D p0, p1;
  bool foundLine = false;
  while (!foundLine && IteratorObject != ObjectList.end()){
#ifdef DONT_USE_RTTI
    WireObject* wo = (*IteratorObject)->IsWireObject();
#else
    WireObject* wo = dynamic_cast<WireObject*>(IteratorObject->get());
#endif
    if (wo){
      if (wo->NextLineInWorld(p0, p1, color, priority)){
        foundLine = true; 
        if (Cam.WorldLineToScreen(p0, p1, x0, y0, x1, y1)){
          return 1;  
        }
      }
    } 
    //try the next object, the current one has no (more) lines
    if (!foundLine) ++IteratorObject;
  }
  return foundLine ? 2 : 0;
}

//************************************************
// GetNextScreenLine (ColoredLine)
//
// get a line in screen coordinates
// returns 0 when there are no more lines
// returns 1 when a line is to draw
// returns 2 when a line exists but is not visible
//************************************************
unsigned int cgi::SimpleWireScene::GetNextScreenLine(ColoredLine &line)
{
  //loop through every line of every Wire object 
  //if there are no more lines return false
  if (IteratorObject == ObjectList.end()) return false;
  
  bool foundLine = false;
  while (!foundLine && IteratorObject != ObjectList.end()){
#ifdef DONT_USE_RTTI
    WireObject* wo = (*IteratorObject)->IsWireObject();
#else
    WireObject* wo = dynamic_cast<WireObject*>(IteratorObject->get());
#endif
    if (wo){
      if (wo->NextLineInWorld(line)){
        foundLine = true; 
        if (Cam.WorldLineToScreen(*line.Start, *line.End, line.Start->X, line.Start->Y, line.End->X, line.End->Y)){
          line.Start->Z = 0;
          line.End->Z = 0;
          return 1;  
        }
      }
    } 
    //try the next object, the current one has no (more) lines
    if (!foundLine) ++IteratorObject;
  }
  return foundLine ? 2 : 0;
}

//************************************************************************
//*                                                                      *
//*                            PolygonalScene                            *
//*                                                                      *
//*                A scene for rendering polygonal objects               *
//************************************************************************  

//************
// constructor
//************
cgi::PolygonalScene::PolygonalScene() : Scene(){}
  
//************
// destructor
//************
cgi::PolygonalScene::~PolygonalScene() {}  
    
//******************************************
// GetBackColor
//
// returns the background color of the scene
//******************************************
const cgi::RGBColor& cgi::PolygonalScene::GetBackColor() const {
  return BackColor;
}

//***************************************
// SetBackColor
//
// sets the background color of the scene
//***************************************
void cgi::PolygonalScene::SetBackColor(const RGBColor& color) {
  BackColor = color;
}

//***************************************
// GetAmbientLight
//
// returns the ambient light of the scene
//***************************************
const cgi::RGBColor& cgi::PolygonalScene::GetAmbientLight() const {
  return AmbientLight;
}

//************************************
// SetAmbientLight
//
// sets the ambient light of the scene
//************************************
void cgi::PolygonalScene::SetAmbientLight(const RGBColor& color) {
  AmbientLight = color;
}

//*******************************
// InitRendering
//
// resets the scene for rendering
//*******************************
void cgi::PolygonalScene::InitRendering(){
  for (IteratorObject = ObjectList.begin(); IteratorObject != ObjectList.end(); ++IteratorObject) {
    (*IteratorObject)->InitRendering();
  }
  IteratorObject = ObjectList.begin();
  IteratorLights = Lights.begin();
}


//***********************************************
// CreateCube
//
// creates new PolygonObject (Scene keeps owner!) 
//***********************************************  
cgi::PolygonObject* cgi::PolygonalScene::CreateCube(const double size, 
                                                    Object* parent,
                                                    const std::string& name)
{
  auto poly = std::make_unique<PolygonObject>(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), parent != nullptr ? parent : &World, std::move(name));
  poly->CreateCube(size);
  PolygonObject* polyPtr = poly.get();
  ObjectList.push_back(std::move(poly));
  return polyPtr;
}

//***********************************************
// CreateQuader
//
// creates new PolygonObject (Scene keeps owner!) 
//***********************************************  
cgi::PolygonObject* cgi::PolygonalScene::CreateQuader(const Vector3D& pos,
                                                      const Vector3D& rot, 
                                                      const double sizeX,
                                                      const double sizeY,
                                                      const double sizeZ,
                                                      Object* parent,
                                                      const std::string& name)
{
  auto poly = std::make_unique<PolygonObject>(pos, rot, parent != nullptr ? parent : &World, std::move(name));
  poly->CreateQuader(sizeX, sizeY, sizeZ);
  PolygonObject* polyPtr = poly.get();
  ObjectList.push_back(std::move(poly));
  return polyPtr;
}

//***********************************************
// CreateSphere
//
// creates new PolygonObject (Scene keeps owner!) 
//***********************************************  
cgi::PolygonObject* cgi::PolygonalScene::CreateSphere(const double radius, 
                                                      const unsigned int n,
                                                      Object* parent,
                                                      const std::string& name)
{
  auto poly = std::make_unique<PolygonObject>(Vector3D(0.0, 0.0, 0.0), Vector3D(0.0, 0.0, 0.0), parent != nullptr ? parent : &World, std::move(name));
  poly->CreateSphere(radius, n);
  PolygonObject* polyPtr = poly.get();
  ObjectList.push_back(std::move(poly));
  return polyPtr;
}

//***********************************************
// CreateCicle
//
// creates new PolygonObject (Scene keeps owner!) 
//***********************************************  
cgi::PolygonObject* cgi::PolygonalScene::CreateCircle(const Vector3D& pos,
                                                      const Vector3D& rot,
                                                      const double radius,
                                                      const unsigned int n,
                                                      Object* parent,
                                                      const std::string& name)
{
  auto poly = std::make_unique<PolygonObject>(pos, rot, parent != nullptr ? parent : &World, std::move(name));
  poly->CreateCircle(radius, n);
  PolygonObject* polyPtr = poly.get();
  ObjectList.push_back(std::move(poly));
  return polyPtr;
}

//***********************************************
// CreateCube
//
// creates new PolygonObject (Scene keeps owner!) 
//***********************************************  
cgi::PolygonObject* cgi::PolygonalScene::CreatePolygonObject(const Vector3D& pos,
                                                             const Vector3D& rot, 
                                                             Object* parent,
                                                             std::string name)
{
  auto poly = std::make_unique<PolygonObject>(pos, rot, parent != nullptr ? parent : &World, std::move(name));
  PolygonObject* polyPtr = poly.get();
  ObjectList.push_back(std::move(poly));
  return polyPtr;
}
                                       
//************************************
// CreateInfiniteLight
//
// creates a new infinite light source
//************************************
cgi::InfiniteLight* cgi::PolygonalScene::CreateInfiniteLight(const Vector3D& pos,
                                                             const Vector3D& target,
                                                             Object* parent,
                                                             const RGBColor& color,
                                                             std::string name)
{
  auto light = std::make_unique<InfiniteLight>(pos, target, parent != nullptr ? parent : &World, color, std::move(name));
  InfiniteLight* lightPtr = light.get();
  Lights.push_back(light.get());
  ObjectList.push_back(std::move(light));
  return lightPtr;
}                                                             

//************************************
// CreateSphereLight
//
// creates a new SphereLight source
//************************************                 
cgi::SphereLight* cgi::PolygonalScene::CreateSphereLight(const Vector3D& pos,
                                                         Object* parent,
                                                         const RGBColor& color,
                                                         std::string name)
{
 
  auto light = std::make_unique<SphereLight>(pos, parent != nullptr ? parent : &World, color, std::move(name));
  SphereLight* lightPtr = light.get();
  Lights.push_back(light.get());
  ObjectList.push_back(std::move(light));
  return lightPtr;
}    
                                   
//************************
// CreateProjector
//
// creates a new projector
//************************
cgi::Projector* cgi::PolygonalScene::CreateProjector(const Vector3D& pos,
                                                     const Vector3D& target,
                                                     Object* parent,
                                                     const RGBColor& color,
                                                     std::string name,
                                                     double character)
{
  auto light = std::make_unique<Projector>(pos, target, parent != NULL ? parent : &World, color, std::move(name), character);
  Projector* lightPtr = light.get();
  Lights.push_back(light.get());
  ObjectList.push_back(std::move(light));
  return lightPtr;
}
                                                     
//************************************************
// GetNextScreenLine
//
// get a line in screen coordinates
// returns 0 when there are no more lines
// returns 1 when a line is to draw
// returns 2 when a line exists but is not visible
//************************************************
unsigned int cgi::PolygonalScene::GetNextScreenLine(double &x0, double &y0, double &x1, double &y1, RGBColor &color, int &priority){
  //loop through every line of every Wire object 
  //if there are no more lines return false
  if (IteratorObject == ObjectList.end()) return false;
  
  Vector3D p0, p1;
  bool foundLine = false;
  while (!foundLine && IteratorObject != ObjectList.end()){
    PolygonObject* po = dynamic_cast<PolygonObject*>(IteratorObject->get());
    if (po){
      if (po->NextLineInWorld(p0, p1, color, priority)){
        foundLine = true; 
        if (Cam.WorldLineToScreen(p0, p1, x0, y0, x1, y1)){
          return 1;  
        }
      }
    } 
    //try the next object, the current one has no (more) lines
    if (!foundLine) ++IteratorObject;
  }
  return foundLine ? 2 : 0; 
}

//************************************************
// GetNextScreenLine (ColoredLine)
//
// get a line in screen coordinates
// returns 0 when there are no more lines
// returns 1 when a line is to draw
// returns 2 when a line exists but is not visible
//************************************************
unsigned int cgi::PolygonalScene::GetNextScreenLine(ColoredLine &line)
{
  //loop through every line of every Wire object 
  //if there are no more lines return false
  if (IteratorObject == ObjectList.end()) return false;
  
  bool foundLine = false;
  while (!foundLine && IteratorObject != ObjectList.end()){
#ifdef DONT_USE_RTTI
    PolygonObject* po = dynamic_cast<PolygonObject*>(*IteratorObject);
#else
    PolygonObject* po = dynamic_cast<PolygonObject*>(IteratorObject->get());
#endif
    if (po){
      if (po->NextLineInWorld(line)){
        foundLine = true; 
        if (Cam.WorldLineToScreen(*line.Start, *line.End, line.Start->X, line.Start->Y, line.End->X, line.End->Y)){
          line.Start->Z = 0;
          line.End->Z = 0;
          return 1;  
        }
      }
    } 
    //try the next object, the current one has no (more) lines
    if (!foundLine) ++IteratorObject;
  }
  return foundLine ? 2 : 0;
}

//******************************
// NextPolyObject
//
// get the next polygonal object
//******************************
cgi::PolygonObject* cgi::PolygonalScene::NextPolyObject(){
  while (IteratorObject != ObjectList.end()){
    PolygonObject* po = dynamic_cast<PolygonObject*>(IteratorObject->get());
    ++IteratorObject;
    if (po) return po;
  }
  return NULL;
}

//*******************
// NextLight
//
// get the next light
//*******************
cgi::Light* cgi::PolygonalScene::NextLight(){
  if (IteratorLights == Lights.end()) return NULL;
  Light* light = *IteratorLights;
  ++IteratorLights;
  return light;
}
