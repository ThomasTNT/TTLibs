#include "polygons.h"



//************************************************************************
//*                                                                      *
//*                                Polygon                               *
//*                                                                      *
//*                     class that represents a polygon                  *
//************************************************************************

//************ 
// constructor
//************
cgi::Polygon::Polygon(){}
           
//***********         
// destructor
//***********
cgi::Polygon::~Polygon(){
  DeletePoints(true);
}

//********************************************
// DeletePoints
// 
// deletes all points
//
// this function is useful in case of DeepCopy 
// where new Vector3D-Objects are created
//********************************************
void cgi::Polygon::DeletePoints(const bool normalsOnly){
  Points.clear();
  Normals.clear();
}
      

//************************************
// DeepCopy
// 
// creates a deep copy of this polygon
//************************************
cgi::Polygon* cgi::Polygon::DeepCopy() const {
  Polygon* newPoly = new Polygon();
  newPoly->Surface = Surface;
  for (size_t i = 0; i < Points.size(); i++) newPoly->Points.push_back(new Vector3D(*Points[i]));
  for (size_t i = 0; i < Normals.size(); i++) newPoly->Normals.push_back(new Vector3D(*Normals[i]));
  return newPoly;
}

//************
// AddPoint
// 
//adds a point
//************
void cgi::Polygon::AddPoint(Vector3D* p) {
  if (!p || Points.size() >= 3) {
    return;
  }
  Points.push_back(p);
  if (Normals.empty()) Normals.push_back(new Vector3D);
  Normals.push_back(new Vector3D);
}

    
//*********
// GetPoint
//*********
cgi::Vector3D* cgi::Polygon::GetPoint(unsigned int index) const {
  return index >= 0 && index < Points.size() ? Points[index] : nullptr;
}

//*********
// GetNormal
//*********
cgi::Vector3D* cgi::Polygon::GetNormal(unsigned int index){
  return index >= 0 && index < Normals.size() ? Normals[index] : nullptr;
}

//***********
// SetSurface
//***********
void cgi::Polygon::SetSurface(const Material& surface){
  Surface = surface;
}
    
//***********
// GetSurface
//
// set Surface for manipulation
//***********    
cgi::Material& cgi::Polygon::GetSurface(){
  return Surface;
}

//*********************
// NumberOfPoints
//
// get number of points
//*********************
unsigned int cgi::Polygon::NumberOfPoints() const {
  return static_cast<unsigned int>(Points.size());
}

//************************************************************************
//*                                                                      *
//*                             PolygonObject                            *
//*                                                                      *
//*                       object based on polygons                       *
//************************************************************************  

// counter for PolygonObject
unsigned int cgi::PolygonObject::count = 0;

//************ 
// constructor
//************
cgi::PolygonObject::PolygonObject(const Vector3D& pos, 
                                  const Vector3D& rot, 
                                  Object* parent,
                                  std::string name)
                    :WireRenderObject(pos, rot, parent)
{
  ++count;
  if (name.length() == 0){ 
    std::ostringstream os;
    os << "PolygonObject_" << count;  
    name = os.str();
  }
  Name = name;
}
    
//***********         
// destructor
//***********
cgi::PolygonObject::~PolygonObject(){

}


//************************************************
// AddTriangle
//
// this function makes sure we only have triangles
//************************************************
void cgi::PolygonObject::AddTriangle(Vector3D* p0, Vector3D* p1, Vector3D* p2, const Material& surface) {
  auto poly = std::make_unique<Polygon>();
  poly->AddPoint(p0);
  poly->AddPoint(p1);
  poly->AddPoint(p2);
  poly->SetSurface(surface);
  Polygons.insert(std::move(poly));
}

//**************************
// CreateCube
//
// create a cube for testing
//**************************
void cgi::PolygonObject::CreateCube(const double size, const Material& surface) {

  Vector3D* p0 = CreateNewPoint(size / 2, size / 2, size / 2);
  Vector3D* p1 = CreateNewPoint(size / 2, size / 2, -size / 2);
  Vector3D* p2 = CreateNewPoint(-size / 2, size / 2, -size / 2);
  Vector3D* p3 = CreateNewPoint(-size / 2, size / 2, size / 2);
  Vector3D* p4 = CreateNewPoint(size / 2, -size / 2, size / 2);
  Vector3D* p5 = CreateNewPoint(size / 2, -size / 2, -size / 2);
  Vector3D* p6 = CreateNewPoint(-size / 2, -size / 2, -size / 2);
  Vector3D* p7 = CreateNewPoint(-size / 2, -size / 2, size / 2);

  //top
  AddTriangle(p0, p1, p2, surface);
  AddTriangle(p0, p2, p3, surface);

  //bottom
  AddTriangle(p4, p6, p5, surface);
  AddTriangle(p4, p7, p6, surface);

  //right
  AddTriangle(p0, p4, p1, surface);
  AddTriangle(p1, p4, p5, surface);

  //left
  AddTriangle(p2, p7, p3, surface);
  AddTriangle(p2, p6, p7, surface);

  //front
  AddTriangle(p3, p7, p4, surface);
  AddTriangle(p0, p3, p4, surface);

  //back
  AddTriangle(p1, p5, p6, surface);
  AddTriangle(p1, p6, p2, surface);
}

//****************************
// CreateQuader
//
// create a quader for testing
//****************************
void cgi::PolygonObject::CreateQuader(const double sizeX, const double sizeY, const double sizeZ, const Material& surface) {

  Vector3D* p0 = CreateNewPoint(sizeX / 2, sizeY / 2, sizeZ / 2);
  Vector3D* p1 = CreateNewPoint(sizeX / 2, sizeY / 2, -sizeZ / 2);
  Vector3D* p2 = CreateNewPoint(-sizeX / 2, sizeY / 2, -sizeZ / 2);
  Vector3D* p3 = CreateNewPoint(-sizeX / 2, sizeY / 2, sizeZ / 2);
  Vector3D* p4 = CreateNewPoint(sizeX / 2, -sizeY / 2, sizeZ / 2);
  Vector3D* p5 = CreateNewPoint(sizeX / 2, -sizeY / 2, -sizeZ / 2);
  Vector3D* p6 = CreateNewPoint(-sizeX / 2, -sizeY / 2, -sizeZ / 2);
  Vector3D* p7 = CreateNewPoint(-sizeX / 2, -sizeY / 2, sizeZ / 2);

  //top
  AddTriangle(p0, p1, p2, surface);
  AddTriangle(p0, p2, p3, surface);

  //bottom
  AddTriangle(p4, p6, p5, surface);
  AddTriangle(p4, p7, p6, surface);

  //right
  AddTriangle(p0, p4, p1, surface);
  AddTriangle(p1, p4, p5, surface);

  //left
  AddTriangle(p2, p7, p3, surface);
  AddTriangle(p2, p6, p7, surface);

  //front
  AddTriangle(p3, p7, p4, surface);
  AddTriangle(p0, p3, p4, surface);

  //back
  AddTriangle(p1, p5, p6, surface);
  AddTriangle(p1, p6, p2, surface);
}

//****************************
// CreateSphere
//
// create a sphere for testing
//****************************
void cgi::PolygonObject::CreateSphere(const double radius, unsigned int n, const Material& surface){
  
  if (n < 4) n = 4;
  
  //setting points in a local vector
  std::vector<Vector3D*> points; 

  points.push_back(CreateNewPoint(0.0, -radius, 0.0));
  points.push_back(CreateNewPoint(0.0,  radius, 0.0));

  const double delta = 2.0 * PI / n;
  for (unsigned int bn = 1; bn < (n / 2); ++bn){
    const double beta = -PI / 2.0 + bn * delta;
    for (unsigned int an = 0; an < n; ++an){
      const double alpha = an * delta;
      points.push_back(CreateNewPoint(cos(alpha) * cos(beta) * radius,
                     	                sin(beta) * radius,
                             		     -sin(alpha) * cos(beta) * radius));
    }
  }
  
  //set polygon
  for (unsigned int k = 2; k <= n + 1; ++k){
    const int k1 = (k < n + 1) ? k + 1: 2;
    //bottom
    AddTriangle(points[0], points[k1], points[k], surface);
    //top
    const int offset = n * (n / 2 - 2);
    AddTriangle(points[1], points[k + offset], points[k1 + offset], surface);
    //middle (rest)
    for (unsigned int l = 0; l < (n / 2 - 2); ++l){
      const int offset1 = l * n;
      AddTriangle(points[k + offset1], points[k1 + offset1], points[k1 + offset1 + n], surface);
      AddTriangle(points[k + offset1], points[k1 + offset1 + n], points[k + offset1 + n], surface);
    }
  }
}
    
//****************************
// CreateCircle
//
// create a circle for testing
//****************************
void cgi::PolygonObject::CreateCircle(const double radius, unsigned int n, const Material& surface) {

  if (n < 4) n = 4;

  std::vector<Vector3D*> points;
  points.push_back(CreateNewPoint(0.0, 0.0, 0.0));

  const double delta = 2.0 * PI / n;

  // We only need one loop to go around the circle
  for (unsigned int an = 0; an < n; ++an) {
    const double alpha = an * delta;
    points.push_back(CreateNewPoint(cos(alpha) * radius,
                     0.0,
                     -sin(alpha) * radius));
  }
    
  for (unsigned int k = 1; k <= n; ++k) {
    const int k1 = (k < n) ? k + 1 : 1;
    AddTriangle(points[0], points[k1], points[k], surface);
  }

}

//***********************************
//
// 
// sets the material for all polygons
//***********************************
void cgi::PolygonObject::SetMaterial(const Material& surface){
  for (IteratorPolygons = Polygons.begin(); IteratorPolygons != Polygons.end(); ++IteratorPolygons){
    (*IteratorPolygons)->SetSurface(surface);
  } 
}
    
//********************************
// InitRendering
//
// resets the object for rendering
//********************************
void cgi::PolygonObject::InitRendering(){
  IteratorPolygons = Polygons.begin();  
  currIndexOfCurrentPolygon = 0;
}

//******************************************
// NextLineInWorld
//
// get the next line of the object, 
// return false when there are no more lines
//******************************************
bool cgi::PolygonObject::NextLineInWorld(Vector3D &p0, Vector3D &p1, RGBColor& color, int &priority){
  
  bool found = false;
  
  if (IteratorPolygons == Polygons.end()) return found;
  
  Vector3D *p0Org, *p1Org;
  while (!found && IteratorPolygons != Polygons.end()){
    //get the point on the current position
    p0Org = (*IteratorPolygons)->GetPoint(currIndexOfCurrentPolygon);
    if (p0Org){
      p1Org = (*IteratorPolygons)->GetPoint(currIndexOfCurrentPolygon + 1);
      if (!p1Org){
        p1Org = (*IteratorPolygons)->GetPoint(0);  
        if (p1Org != p0Org){
          p0 = PointInWorld(*p0Org);
          p1 = PointInWorld(*p1Org);
          Material mat = (*IteratorPolygons)->GetSurface();
          color = 0.5 * (mat.Diffuse + mat.Ambient); //arithmetic middle
          priority = 0; //using a kind of z-buffering? 
          found = true;
          ++currIndexOfCurrentPolygon;
        }
      }
    }
    //try the next object, the current one has no (more) lines
    if (!found){ 
      ++IteratorPolygons;
      currIndexOfCurrentPolygon = 0;
    }
  }
  return found;
}

//******************************************
// NextLineInWorld (ColoredLine)
//
// get the next line of the object, 
// return false when there are no more lines
//******************************************
bool cgi::PolygonObject::NextLineInWorld(ColoredLine &line){
  bool found = false;
  
  if (IteratorPolygons == Polygons.end()) return found;
  
  Vector3D *p0Org, *p1Org;
  while (!found && IteratorPolygons != Polygons.end()){
    //get the point on the current position
    p0Org = (*IteratorPolygons)->GetPoint(currIndexOfCurrentPolygon);
    if (p0Org){
      p1Org = (*IteratorPolygons)->GetPoint(currIndexOfCurrentPolygon + 1);
      if (!p1Org) p1Org = (*IteratorPolygons)->GetPoint(0);  
      if (p1Org != p0Org){
        *line.Start = PointInWorld(*p0Org);
        *line.End = PointInWorld(*p1Org);
        Material mat = (*IteratorPolygons)->GetSurface();
        line.Color = 0.5 * (mat.Diffuse + mat.Ambient); //arithmetic middle
        line.Priority = 0; //using a kind of z-buffering? 
        found = true;
        ++currIndexOfCurrentPolygon;
      }
    }
    //try the next object, the current one has no (more) lines
    if (!found){ 
      ++IteratorPolygons;
      currIndexOfCurrentPolygon = 0;
    }
  }
  return found;
}


//****************
// NextPolygon
//
// for raytracning
//****************
const cgi::Polygon* cgi::PolygonObject::NextPolygon() {
  if (IteratorPolygons == Polygons.end()) return nullptr;
  const Polygon* poly = (*IteratorPolygons).get();
  ++IteratorPolygons;
  return poly;
}
    

//****************
// CreateNewPoint
//
// for raytracning
//****************
cgi::Vector3D* cgi::PolygonObject::CreateNewPoint(const double x, const double y, const double z) {
  auto point_ptr = std::make_unique<Vector3D>(x, y, z);
  Vector3D* raw_ptr = point_ptr.get();
  this->AllPoints.push_back(std::move(point_ptr));
  return raw_ptr;
}
