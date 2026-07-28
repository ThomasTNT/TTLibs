#include "objects.h"

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

//***********************************************
// protected constructor to prevent instantiation
//***********************************************
cgi::Object::Object(const Vector3D& pos, const Vector3D& rot, Object* parent)
            :Position(pos),
             Rotation(rot),
             Scale(1.0, 1.0, 1.0),
             Parent(NULL), //will be set below
             Children(),
             Active(false),
             Name("object"),
             Identity("")
{
  if (parent) SetHierarchyParent(parent);
}
     
//*****************      
// copy constructor
//***************** 
cgi::Object::Object(Object& other) 
            :Position(other.Position),
             Rotation(other.Rotation),
             Scale(other.Scale),
             Parent(other.Parent),
             Children(other.Children),
             Active(other.Active),
             Name(other.Name),
             Identity(other.Identity)
{
}
    
//***********
// destructor
//*********** 
cgi::Object::~Object(){}
    
//*********************
// operator=
// 
// assignement operator
//********************* 
cgi::Object& cgi::Object::operator=(const Object& other){
  if (&other == this) return *this;
  Position = other.Position;
  Rotation = other.Rotation;
  Scale = other.Scale;
  Parent = other.Parent;
  Children = other.Children;
  Active = other.Active;
  Name = other.Name;
  Identity = other.Identity;    
  return *this;
}

//----------------------- manipulating hierarchy --------------------------
    
//****************************
// HIERARCHY GetHierachyParent
//****************************
cgi::Object* cgi::Object::GetHierarchyParent() const{
  return Parent;  
}

//*****************************
// HIERARCHY SetHierarchyParent
//*****************************
cgi::Object& cgi::Object::SetHierarchyParent(Object* parent){
  Parent = parent; 
  Parent->AddHierarchyChild(this);
  return *this;
}

//*******************************
// HIERARCHY GetHierarchyChildren
//*******************************
std::set<cgi::Object*> cgi::Object::GetHierarchyChildren() const{
  return Children;  
}

//****************************
// HIERARCHY AddHierarchyChild
//****************************
cgi::Object& cgi::Object::AddHierarchyChild(Object* child){
  Children.insert(child); 
  return *this;
}

//****************************
// HIERARCHY DelHierarchyChild
//****************************
cgi::Object& cgi::Object::DelHierarchyChild(Object* child){
  Children.erase(child);
  return *this;
}

//**********************************
// HIERARCHY SetHierarchyNewChildren
//**********************************
cgi::Object& cgi::Object::SetHierarchyNewChildren(const std::set<cgi::Object*>& children){
  Children = children;
  return *this;
}

//**********************************************
// HIERARCHY SetHierarchyNewParentForOurChildren
//**********************************************
cgi::Object& cgi::Object::SetHierarchyNewParentForOurChildren(Object* parent){
  for (std::set<Object*>::iterator iter = Children.begin(); iter != Children.end(); ++iter){
    (*iter)->SetHierarchyParent(parent);
    this->DelHierarchyChild((*iter));
    parent->AddHierarchyChild((*iter));
  } 
  return *this;
}

//------------------ manipulating geometry and coordinates ---------------------

//*********************
// GEOMETRY SetPosition
//*********************
cgi::Object& cgi::Object::SetPosition(const Vector3D& pos){
  Position = pos;
  return *this;
}

//*********************
// GEOMETRY GetPosition
//*********************
cgi::Vector3D cgi::Object::GetPosition() const{
  return Position;
}

//*******************
// GEOMETRY Translate
//*******************
cgi::Object& cgi::Object::Translate(const Vector3D& pos){
  Position += pos;
  return *this;
}

//*********************
// GEOMETRY SetRotation
//*********************
cgi::Object& cgi::Object::SetRotation(const Vector3D& rot){
  Rotation = rot;
  return *this;
}

//*********************
// GEOMETRY GetRotation
//*********************
cgi::Vector3D cgi::Object::GetRotation() const{
  return Rotation;
}

//****************
// GEOMETRY Rotate
//****************
cgi::Object& cgi::Object::Rotate(const Vector3D& rot){
  Rotation += rot;
  return *this;
}

//******************
// GEOMETRY SetScale
//******************
cgi::Object& cgi::Object::SetScale(const Vector3D&  scale){
  Scale = scale;
  return *this;
}

//******************
// GEOMETRY GetScale
//******************
cgi::Vector3D cgi::Object::GetScale() const{
  return Scale;  
}

//*****************
// GEOMETRY Scaling
//*****************
cgi::Object& cgi::Object::Scaling(const Vector3D&  scale){
  Scale.X *= scale.X;
  Scale.Y *= scale.Y;
  Scale.Z *= scale.Z;
  return *this;
}

//**********************************************
// GEOMETRY Scale
//
// does nothing because object hasn't any points
//**********************************************
/*void cgi::Object::ApplyScale(){
  Scale.X = 1.0; 
  Scale.Y = 1.0;
  Scale.Z = 1.0; 
}*/


//*****************************************************
// GEOMETRY PointInWorld
//
// (inverse function to WorldPointInThis)
// Transforms the given point into world coordinates. 
// It calls recursively the PointInWorld of the parent.
//
// World in this case is the Object without any parent.
//*****************************************************
cgi::Vector3D cgi::Object::PointInWorld(const Vector3D& point){
  return Parent ? Parent->PointInWorld(PointInParent(point)) : PointInParent(point);
}

//****************************************************
// GEOMETRY PointInParent
//
// (inverse function to ParentPointInThis
// Transforms the given point into parent coordinates. 
//****************************************************
cgi::Vector3D cgi::Object::PointInParent(const Vector3D& point){
  
  const double x = point.X * Scale.X;
  const double y = point.Y * Scale.Y;
  const double z = point.Z * Scale.Z;
            
  const double alpha = M_PI * Rotation.X / 180.0; 
  const double CA = cos(alpha); 
  const double SA = sin(alpha);
  const double beta = M_PI * Rotation.Y / 180.0;
  const double CB = cos(beta);  
  const double SB = sin(beta);
  const double gamma = M_PI * Rotation.Z / 180.0;
  const double CG = cos(gamma); 
  const double SG = sin(gamma);

  return Vector3D(CB*CG*x + (SA*SB*CG-CA*SG)*y + (CA*SB*CG+SA*SG)*z + Position.X,
                  CB*SG*x + (SA*SB*SG+CA*CG)*y + (CA*SB*SG-SA*CG)*z + Position.Y,
                    -SB*x +           SA*CB *y +           CA*CB *z + Position.Z);
}

//*****************************************************
// GEOMETRY WorldPointInThis
//
// (inverse function to PointInWorld)
// Transforms the given point in world coordinates into
// own coodinates. 
// It calls recursively the PointInWorld of the parent.
//
// World in this case is the Object without any parent.
//*****************************************************
cgi::Vector3D cgi::Object::WorldPointInThis(const Vector3D& point){
  return ParentPointInThis(Parent ? Parent->WorldPointInThis(point) : point); 
}

//****************************************************
// GEOMETRY ParentPointInThis
//
// (inverse function to PointInParent
// Transforms the given point from parent coodinates 
// into own coordinates. 
//****************************************************
cgi::Vector3D cgi::Object::ParentPointInThis(const Vector3D& point){
  return point; // TODO later !!!!!!!!!!!!!!!!
}


//-------------------------- object properties  ----------------------------

//********
// SetName
//********
cgi::Object& cgi::Object::SetName(const std::string& name){ 
  Name = name; 
  return *this;
}

//********
// GetName
//********
std::string cgi::Object::GetName() const { return Name; }

//************
// SetIdentity
//************
cgi::Object& cgi::Object::SetIdentity(const std::string& id){ 
  Identity = id; 
  return *this;
}

//************
// GetIdentity
//************
std::string cgi::Object::GetIdentity() const { return Identity; }

//**********
// SetActive
//**********
cgi::Object* cgi::Object::SetActive(const bool active){ 
  Active = active; 
  return this;
}
  
//**********
// GetActive
//**********
bool cgi::Object::GetActive() const { return Active; }

//--------------------------------- info -----------------------------------
    
//********************
// ToString
//
// prints object only
//********************
std::string cgi::Object::ToString(){
  return Active ? Name + "*" : Name;  
}
    
//***********************
// HierarchyToString
//
// prints whole hierarchy
//***********************
std::string cgi::Object::HierarchyToString(){
  //loop through the hierarchy to get the format sizes
  std::vector<unsigned int> widths;
  GetNameWidths(widths, 0);
  
  //loop recursively through all children
  std::vector<std::string> path;
  std::list<std::vector<std::string> > paths;
  BuildHierarchyToString(paths, path, 0);
  
  std::string result; 
  
  for (std::list<std::vector<std::string> >::iterator iter = paths.begin();
       iter != paths.end();
       ++iter)
  {
    for (size_t i = 0; i < (*iter).size(); ++i){
      if (i > 0) result += " -> ";
      std::string objectName = (*iter)[i];
      while (objectName.length() < widths[i]) objectName += " "; 
      result += objectName;
    }        
    result += "\n";
  }
  
  return result;
}
   
//****************************
// HELP GetNameWidths
//
// called by HierarchyToString
//****************************
void cgi::Object::GetNameWidths(std::vector<unsigned int>& widths, unsigned int depth){
  
  //check width of the current depth
  const unsigned int currWidth = static_cast<unsigned int>(Name.length());
  if (widths.size() <= depth){
    widths.push_back(currWidth);   
  }
  else{
    if (widths[depth] < currWidth) widths[depth] = currWidth; 
  }
  //loop through children
  ++depth;
  for (std::set<Object*>::iterator iter = Children.begin(); iter != Children.end(); ++iter){
    (*iter)->GetNameWidths(widths, depth);  
  }
}
    
//****************************
// HELP BuildHierarchyToString
//
// called by HierarchyToString
//****************************
void cgi::Object::BuildHierarchyToString(std::list<std::vector<std::string> >& paths,
                                         std::vector<std::string>& path, 
                                         unsigned int depth)
{
  //add ourself to the path
  if (path.size() <= depth) path.resize(depth + 1);
  path[depth] = Name;
  
  //create print path if we have no children
  //clear path after adding 
  if (Children.size() <= 0){
    paths.push_back(path);
    path.clear();
  }
  //if we have children forward recursively to them 
  else{
    ++depth;
    for (std::set<Object*>::iterator iter = Children.begin(); iter != Children.end(); ++iter){
      (*iter)->BuildHierarchyToString(paths, path, depth);
    }
  }
}
    
    
//************************************************************************
//*                                                                      *
//*                              Reference                               *
//*                                                                      *
//* Reference is an empty object which acts as a new coordinate system.  *
//* It is to build an hierachie of graphical objects.                    *
//************************************************************************

// "reference counter" hihi
unsigned int cgi::Reference::count = 0;
    
//************
// constructor
//************ 
cgi::Reference::Reference(const Vector3D& pos,
                          const Vector3D& rot, 
                          Object* parent,
                          std::string name)
               :Object(pos, rot, parent)
{
  ++count;
  if (name.length() == 0){ 
    std::ostringstream os;
    os << "reference_" << count;  
    name = os.str();
  }
  Name = name;
}

//**********    
// destrutor 
//**********
cgi::Reference::~Reference(){}
    


//***************************************************************************
//*                                                                         *
//*                         ObjectWithPoints                                *
//*                                                                         *
//* This is the base class for all kind of objects that has got any points. *
//* All points have to be stored in the point list to access them           *
//***************************************************************************

//***********************************************
// protected constructor to prevent instantiation
//***********************************************
cgi::ObjectWithPoints::ObjectWithPoints(const Vector3D& pos, 
                                        const Vector3D& rot, 
                                        Object* parent)
                      :Object(pos, rot, parent)
{

}

//***********
// destructor
//***********
cgi::ObjectWithPoints::~ObjectWithPoints(){
  for (std::set<Vector3D*>::iterator iter = Points.begin(); iter != Points.end(); ++iter){
    delete (*iter);
  }
}
    
//**********************************************
// TranslateObject
//
// actually applies transformation to the points
//**********************************************
void cgi::ObjectWithPoints::TranslateObject(const Vector3D& trans){
  for (std::set<Vector3D*>::iterator iter = Points.begin(); iter != Points.end(); ++iter){
    *(*iter) += trans;
  }
  //if (Modell) UpdateModell(true);
}

//**********************************************
// RotateObject
//
// actually applies transformation to the points
//**********************************************
void cgi::ObjectWithPoints::RotateObject(const Vector3D& rot){
  
  const double alpha = M_PI * rot.X / 180.0; 
  const double CA = cos(alpha); 
  const double SA = sin(alpha);
  const double beta = M_PI * rot.Y / 180.0;
  const double CB = cos(beta);  
  const double SB = sin(beta);
  const double gamma = M_PI * rot.Z / 180.0;
  const double CG = cos(gamma); 
  const double SG = sin(gamma);

  for (std::set<Vector3D*>::iterator iter = Points.begin(); iter != Points.end(); ++iter){
    *(*iter) = Vector3D( CB*CG*((*iter)->X) + (SA*SB*CG-CA*SG)*((*iter)->Y) + (CA*SB*CG+SA*SG)*((*iter)->Z),
                         CB*SG*((*iter)->X) + (SA*SB*SG+CA*CG)*((*iter)->Y) + (CA*SB*SG-SA*CG)*((*iter)->Z),
                           -SB*((*iter)->X) +           SA*CB *((*iter)->Y) +           CA*CB *((*iter)->Z));
  }
 
  //if (Modell) UpdateModell(true);
}

//**********************************************
// ScaleObject
//
// actually applies transformation to the points
//**********************************************
void cgi::ObjectWithPoints::ScaleObject(const Vector3D& scale){
  for (std::set<Vector3D*>::iterator iter = Points.begin(); iter != Points.end(); ++iter){
    (*iter)->X *= scale.X;
    (*iter)->Y *= scale.Y;
    (*iter)->Z *= scale.Z;
  }
  //if (Modell) UpdateModell(true);
}


//***************************************************************************
//*                                                                         *
//*                             WireRenderObject                            *
//*                                                                         *
//* This is the base class for all kind of objects that can be renders      *
//* with a wire renderer                                                    *
//***************************************************************************

//************
// constructor 
//************
cgi::WireRenderObject::WireRenderObject(const Vector3D& pos, 
                                        const Vector3D& rot, 
                                        Object* parent)
                       :ObjectWithPoints(pos, rot, parent)
{
}
 
//***********
// destructor 
//***********
cgi::WireRenderObject::~WireRenderObject(){}
  


//***************************************************************************
//*                                                                         *
//*                               WireObject                                *
//*                                                                         *
//* A simple object consists only of points and streight lines.             *
//* It has no surfaces, no colors and therefor no material.                 *
//***************************************************************************

// counter for WireObjects
unsigned int cgi::WireObject::count = 0;

//************
// constructor 
//************
cgi::WireObject::WireObject(const Vector3D& pos, 
                            const Vector3D& rot, 
                            Object* parent,
                            std::string name)
                :WireRenderObject(pos, rot, parent)
{
  ++count;
  if (name.length() == 0){ 
    std::ostringstream os;
    os << "WireObject_" << count;  
    name = os.str();
  }
  Name = name;
}
 

//*************************************
// destructor 
//
// deletes all lines but not the points
//*************************************
cgi::WireObject::~WireObject(){
  for (IteratorLines = Lines.begin(); IteratorLines != Lines.end(); ++IteratorLines){
    delete (*IteratorLines);
  } 
}
    
//******************************
// AddLine
//
// adds a new line to the object
//******************************
cgi::WireObject& cgi::WireObject::AddLine(cgi::ColoredLine* line){
  Lines.insert(line);
  Points.insert(line->Start);
  Points.insert(line->End);
  return *this;
}

//******************************
// AddLine
//
// adds a new line to the object
//******************************
cgi::WireObject& cgi::WireObject::AddLine(Vector3D* p0, Vector3D* p1, const RGBColor& color, int priority){
  return AddLine(new ColoredLine(p0, p1, color, priority));
}

//******************************
// CreateCube
//
// create a centered cube
//******************************
cgi::WireObject& cgi::WireObject::CreateCube(const double size){
  
  Vector3D* p0 = new Vector3D( size/2,  size/2,  size/2);
  Vector3D* p1 = new Vector3D(-size/2,  size/2,  size/2);
  Vector3D* p2 = new Vector3D(-size/2, -size/2,  size/2);
  Vector3D* p3 = new Vector3D( size/2, -size/2,  size/2);
  Vector3D* p4 = new Vector3D( size/2,  size/2, -size/2);
  Vector3D* p5 = new Vector3D(-size/2,  size/2, -size/2);
  Vector3D* p6 = new Vector3D(-size/2, -size/2, -size/2);
  Vector3D* p7 = new Vector3D( size/2, -size/2, -size/2);
  
  AddLine(p0, p1);
  AddLine(p1, p2);
  AddLine(p2, p3);
  AddLine(p3, p0);
  
  AddLine(p4, p5);
  AddLine(p5, p6);
  AddLine(p6, p7);
  AddLine(p7, p4);
  
  AddLine(p0, p4);
  AddLine(p1, p5);
  AddLine(p2, p6);
  AddLine(p3, p7);

  return *this;
}

//**************************
// SetPriority
//
// sets priorityon all lines
//**************************
cgi::WireObject& cgi::WireObject::SetPriority(const int priority){
  for (IteratorLines = Lines.begin(); IteratorLines != Lines.end(); ++IteratorLines){
   (*IteratorLines)->Priority = priority;
  } 
  return *this;
}


//------------------------------ rendering ---------------------------------
    
//********************************
// InitRendering
//
// resets the object for rendering
//********************************
void cgi::WireObject::InitRendering(){
  IteratorLines = Lines.begin();  
}

//******************************************
// NextLineInWorld
//
// get the next line of the object, 
// return false when there are no more lines
//******************************************
bool cgi::WireObject::NextLineInWorld(Vector3D &p0, Vector3D &p1, RGBColor &color, int &priority){
  if (IteratorLines == Lines.end()) return false; 
  p0 = PointInWorld(*((*IteratorLines)->Start));
  p1 = PointInWorld(*((*IteratorLines)->End));
  color = (*IteratorLines)->Color;
  priority = (*IteratorLines)->Priority;
  ++IteratorLines;
  return true;
}

//******************************************
// NextLineInWorld (ColoredLine)
//
// get the next line of the object, 
// return false when there are no more lines
//******************************************
bool cgi::WireObject::NextLineInWorld(ColoredLine &line){
  if (IteratorLines == Lines.end()) return false;  
  *line.Start = PointInWorld(*((*IteratorLines)->Start));
  *line.End = PointInWorld(*((*IteratorLines)->End));
  line.Color = (*IteratorLines)->Color;
  line.Priority = (*IteratorLines)->Priority;
  ++IteratorLines;
  return true;
}
