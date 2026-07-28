#pragma once

#include "scene.h"
#include "TTImage.h"
#include <vector>
#include <stack>
#include <map>
 
namespace cgi{
  
//************************************************************************
//*                                                                      *
//*                           SimpleRaytracer                            *
//*                                                                      *
//*               this is a simple raytracer for rendering a scene       *
//************************************************************************  

const unsigned int CUBE_DIV = 1; //default cube dif

class SimpleRaytracer{
  
  public: 
    
    // constructor
    SimpleRaytracer();
    
    // destructor  
    virtual ~SimpleRaytracer();
    
    // frees all memory
    void CleanUp();

    //main render function
    void RenderScene(PolygonalScene& scene, const unsigned int frame, image::TTImage& outImage);
    
  protected: 
    
    //prepares the internal scene for rendering
    void PrepareForRendering(PolygonalScene& scene);
    
    //effective rendering
    void CalcRenderCubeRadius();
    
    //effective rendering
    void PolygonsToCubes();
    
    //effective rendering
    bool IsPolygonInCube(Polygon* poly, 
                         const double x0, const double x1, 
                         const double y0, const double y1, 
                         const double z0, const double z1);
    
    //effective rendering
    inline bool IsPointInCube(const Vector3D& p, 
                              const double x0, const double x1, 
                              const double y0, const double y1, 
                              const double z0, const double z1);
                              
    // checks if a given triangle edge intersects with cube surface
    inline bool IntersectTriangleEgdeCube(const Vector3D& p,
                                          const Vector3D& v, 
                                          const double x0, const double x1, 
                                          const double y0, const double y1, 
                                          const double z0, const double z1);
                                             
    // checks if a given triangle edge intersects with cube surface
    inline bool IntersectCubeEdgeTriangel(const Vector3D& pt0,
                                          const Vector3D& pt1, 
                                          const Vector3D& pt2,
                                          const double x0, const double x1, 
                                          const double y0, const double y1, 
                                          const double z0, const double z1);
                                                                                   
    //actual render function
    void RenderToImage(PolygonalScene& scene, image::TTImage& resultImage);
    
    // gets the color for one ray
    RGBColor TraceOneRay(PolygonalScene& scene,
                         const Vector3D& origin,
                         const Vector3D& direction,
                         const Polygon* const lastPoly,
                         std::stack<double>  refractionIndex,
                         const unsigned int currDepth,
                         const unsigned int MAXDEPTH);
                         
    // seaches for the next polygon that is intersected by the ray
    double GetNearestValidPolygon(const Vector3D& rayOrigin,
                                  const Vector3D& rayDirection,
                                  const Polygon*  currentPolygon,
                                  Material& surface,
                                  Vector3D& intersection,
                                  Vector3D& normal,
                                  Polygon* &nearestPoly);
    
    //searches all cubes through with the ray goes
    void CubesOfRay(std::list<int>& cubes, 
                    const Vector3D& rayOrigin,
                    const Vector3D& rayDirection);
    
    // checks when the ray changes into a new row of cubes
    std::list<double> GetChanges(const double pos0, 
                                 const double d, 
                                 const double a,
                                 const double maxdist);
                                                
    // gets te shadow for one light source
    RGBColor GetShadow(const Vector3D& IP, 
                       const Vector3D& L, 
                       const double dl, 
                       const Polygon* currentPolygon);
    
    // calculates the distance to the given poylgon, 
    double DistanceToPolygon(const Vector3D& rayOrigin,
                             const Vector3D& rayDirection,
                             const Vector3D& p0,
                             const Vector3D& p1,
                             const Vector3D& p2,
                             Vector3D &intersect, 
                             double &u, 
                             double &v,
                             const bool square = false); //special case for checking sqares, should be false for triangles
                                                          
    //map the hold all tranformed polygonal objects
    std::map<PolygonObject*, std::vector<Polygon*> > TransformedPolyObjects;
    std::vector<Light*> TransformedLights;
    
    
    
    
    //temporare values for effective rendering
    double RenderCubeRadius;   // actually the half of the cube
    unsigned int RenderCubeDiv; //actually the half of the cube
    std::map<int, std::list<Polygon*> > CubesWithPolygons;
    
    bool debug; 
};


} //end of namespace cgi
