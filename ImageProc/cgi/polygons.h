#pragma once

#include <vector>
#include <cmath>

#include "cgibasics.h"
#include "objects.h"
#include "GeometryAlgorithms.h"

namespace cgi{
  
//************************************************************************
//*                                                                      *
//*                                Polygon                               *
//*                                                                      *
//*                     class that represents a polygon                  *
//************************************************************************
class Polygon{
  
  public:
  
    //constructor 
    Polygon();
    
    //destructor
    ~Polygon();
    
    //deletes all points
    void DeletePoints(const bool normalsOnly = false); 
      
    //creates a deep copy of this polygon
    cgi::Polygon* DeepCopy() const;
    
    // GetPoint
    Vector3D* GetPoint(unsigned int index) const;
    
    // GetNormal
    Vector3D* GetNormal(unsigned int index);

    //adds a point
    void AddPoint(Vector3D* p);
    
    //set Surface
    void SetSurface(const Material& surface);
    
    //set Surface for manipulation
    Material& GetSurface();
    
    //get number of points
    unsigned int NumberOfPoints() const;
    
  private:
  
    //the surface material 
    Material Surface;
    
    //all points of this polygon
    std::vector<Vector3D*> Points;
    
    //all normal vectors of this polygon (correspondes to Points) 
    std::vector<Vector3D*> Normals;
};
  
  
//************************************************************************
//*                                                                      *
//*                             PolygonObject                            *
//*                                                                      *
//*                       object based on polygons                       *
//************************************************************************  
class PolygonObject : public WireRenderObject{
  
  public:
    
    // constructor 
    PolygonObject(const Vector3D& pos = Vector3D(0.0, 0.0, 0.0),
                  const Vector3D& rot = Vector3D(0.0, 0.0, 0.0), 
                  Object* parent = nullptr,
                  std::string name = "");
    
    // destructor
    virtual ~PolygonObject();
    
    //creates a new point
    Vector3D* CreateNewPoint(const double x, const double y, const double z);

    // create a cube
    void AddTriangle(Vector3D* p0, Vector3D* p1, Vector3D* p2, const Material& surface);
    
    // creates a cube
    void CreateCube(const double size, const Material& surface = Material());
    
    // creates a quader
    void CreateQuader(const double sizeX, const double sizeY, const double sizeZ, const Material& surface = Material());

    // creates a sphere
    void CreateSphere(const double radius, unsigned int n, const Material& surface = Material());
    
    // creates a sphere
    void CreateCircle(const double radius, unsigned int n, const Material& surface = Material());

    // sets the material for all polygons
    void SetMaterial(const Material& surface);
    
    // resets the object for rendering
    virtual void InitRendering();
    
    //WIRE get the next line of the object, return false when there are no more lines
    virtual bool NextLineInWorld(Vector3D &p0, Vector3D &p1, RGBColor& color, int &priority);
    virtual bool NextLineInWorld(ColoredLine &line);
    
    //RAYTRACING
    const Polygon* NextPolygon();
    
    
  protected:

    
    
    std::set<std::unique_ptr<Polygon>> Polygons;
    std::vector<std::unique_ptr<Vector3D>> AllPoints;
    std::set<std::unique_ptr<Polygon>>::iterator IteratorPolygons;
    int currIndexOfCurrentPolygon; 
    
  private:
    
    // counter
    static unsigned int count;
};



} //end of namespace cgi


