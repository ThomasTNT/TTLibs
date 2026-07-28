#include "raytracer.h"


//************************************************************************
//*                                                                      *
//*                           SimpleRaytracer                            *
//*                                                                      *
//*               this is a simple raytracer for rendering a scene       *
//************************************************************************  

//************
// constructor
//************
cgi::SimpleRaytracer::SimpleRaytracer() : debug(false){}

//************
// destructor
//************
cgi::SimpleRaytracer::~SimpleRaytracer(){
  CleanUp();
}
    
//*****************
// CleanUp
//
// frees all memory
//*****************
void cgi::SimpleRaytracer::CleanUp(){
  
  //delete all transformed polygons
  std::map<PolygonObject*, std::vector<Polygon*> > iter;
  for (std::map<PolygonObject*, std::vector<Polygon*> >::iterator iter = TransformedPolyObjects.begin();
       iter != TransformedPolyObjects.end();
       ++iter)
  {
    std::vector<Polygon*> polygons = (*iter).second;
    for (size_t i = 0; i < polygons.size(); i++) delete polygons[i];  
  }
  TransformedPolyObjects.clear();
  
  //delete all transformed lights
  for (size_t i = 0; i < TransformedLights.size(); i++) delete TransformedLights[i];  
}


//*********************
// RenderScene
//
// main render function
//*********************
void cgi::SimpleRaytracer::RenderScene(PolygonalScene& scene, const unsigned int frame, image::TTImage& outImage)
{
  //setting scene for rendering
  const unsigned int width = outImage.Width();
  const unsigned int height = outImage.Height();
  Camera &cam = scene.GetCamera();
  cam.SetResolution(width, height);
  cam.SetImageRatio(static_cast<double>(width) / static_cast<double>(height));
  
  //prepare rendering
  PrepareForRendering(scene);
  CalcRenderCubeRadius();
  PolygonsToCubes();
  
  //actual rendering
  RenderToImage(scene, outImage);
}

//*****************************************************************
// PrepareForRendering
//
// prepares the internal scene for rendering
//
// transforms the whole scene into a internal representation
// calcultes all neccessary information like normal vectors etc.pp.
//*****************************************************************
void cgi::SimpleRaytracer::PrepareForRendering(PolygonalScene& scene){
  CleanUp();

  scene.InitRendering();
  Camera &cam = scene.GetCamera();

  //----------------------- transform Polygons ---------------------

  std::map<Vector3D*, std::vector<Polygon*> > polygonMap;
  std::map<Vector3D*, std::vector<unsigned int> > polygonNormalIndexMap;

  //loop over all polygonal objects of the scene and transform them into camera coordinates
  for (;;) {
    PolygonObject* po = scene.NextPolyObject();
    if (!po) break;

    std::vector<Polygon*> polygons;
    po->InitRendering();
    for (;;) {
      const Polygon* poly = po->NextPolygon();
      if (!poly) break;
      if (poly->NumberOfPoints() == 3) {
        Polygon* transformedPoly = poly->DeepCopy();
        //transform all points of this polygon in camera coordinates
        for (unsigned int i = 0; i < 3; i++) {
          Vector3D* currPoint = transformedPoly->GetPoint(i);
          *currPoint = cam.WorldPointInThis(po->PointInWorld(*currPoint));
        }

        //make them relativly
        *transformedPoly->GetPoint(1) -= *transformedPoly->GetPoint(0);
        *transformedPoly->GetPoint(2) -= *transformedPoly->GetPoint(0);
        //calculate normales on each corner (for the first step they are equal)
        *transformedPoly->GetNormal(0) = (*transformedPoly->GetPoint(1) % *transformedPoly->GetPoint(2)).Norm();
        *transformedPoly->GetNormal(1) = *transformedPoly->GetNormal(0);
        *transformedPoly->GetNormal(2) = *transformedPoly->GetNormal(0);
        *transformedPoly->GetNormal(3) = *transformedPoly->GetNormal(0);

        polygons.push_back(transformedPoly);

        //memory which polygons have one point in common
        polygonMap[poly->GetPoint(0)].push_back(transformedPoly);
        polygonNormalIndexMap[poly->GetPoint(0)].push_back(1);

        polygonMap[poly->GetPoint(1)].push_back(transformedPoly);
        polygonNormalIndexMap[poly->GetPoint(1)].push_back(2);

        polygonMap[poly->GetPoint(2)].push_back(transformedPoly);
        polygonNormalIndexMap[poly->GetPoint(2)].push_back(3);
      }
    }
    TransformedPolyObjects[po] = polygons;
  }

  //----------------- calculate normals on polygon corners ---------------------

  const double MAXANGLE = 45; //degrees
  const double MAXANGLECOS = cos(MAXANGLE * PI / 180.0);

  //loop over all points
  for (std::map<Vector3D*, std::vector<Polygon*> >::iterator iter = polygonMap.begin(); iter != polygonMap.end(); ++iter) {

    Vector3D* point = iter->first;
    std::vector<Polygon*> polygons = iter->second;

    if (polygons.size() < 2) continue;

    Vector3D normal;
    Polygon* currPolygon = nullptr;
    Vector3D currNormal;
    Vector3D otherNormal;

    std::vector<bool> processed;
    for (size_t i = 0; i < polygons.size(); ++i) processed.push_back(false);

    bool allProcessed = true;
    std::vector<size_t> toProcess;

    do {

      allProcessed = true;
      toProcess.clear();

      //search the first unprocessed polygon and compare it with all other
      //unprocessed polygons
      for (size_t i = 0; i < polygons.size(); ++i) {
        if (!processed[i]) {
          allProcessed = false;
          processed[i] = true;
          //first one? 
          if (!currPolygon) {
            currPolygon = polygons[i];
            currNormal = *currPolygon->GetNormal(polygonNormalIndexMap[point][i]);
            normal = currNormal;
            toProcess.push_back(i);
          }
          //another one...
          else {
            otherNormal = *polygons[i]->GetNormal(polygonNormalIndexMap[point][i]);
            //compare it with the current normal vector
            if (otherNormal * currNormal >= MAXANGLECOS) {
              normal += otherNormal;
              toProcess.push_back(i);
            }
          }
        }
      }

      //apply normal vectors to all polygons
      if (toProcess.size() > 1) {
        normal = normal.Norm();
        for (size_t i = 0; i < toProcess.size(); ++i) {
          const size_t index = toProcess[i];
          *(polygons[index]->GetNormal(polygonNormalIndexMap[point][index])) = normal;
        }
      }
    } while (!allProcessed);
  }

  //----------------------- Lights  ----------------------
  //transform all light sources into camera coodinates
  Light* light = nullptr;
;
  do {
    light = scene.NextLight();
    if (light) {
      Light* transformedLight = light->DeepCopy();
      transformedLight->SetPosition(cam.WorldPointInThis(light->PositionInWorld()));
      DirectedLight* dirLight = dynamic_cast<DirectedLight*>(transformedLight);
      if (dirLight) {
        dirLight->SetTarget(cam.WorldPointInThis(dirLight->TargetInWorld()));
      }
      TransformedLights.push_back(transformedLight);
    }
  } while (light);
}


//************************************************
// CalcRenderCubeRadius
//
// effective rendering
//
// goes through all polygons and seaches 
// for the biggest postive or negative coordinate. 
//************************************************
void cgi::SimpleRaytracer::CalcRenderCubeRadius() {

  std::cout << "CalcRenderCubeRadius" << std::endl;

  double maxcoord = 0;
  double currCoord = 0;

  for (std::map<PolygonObject*, std::vector<Polygon*> >::iterator iter = TransformedPolyObjects.begin();
    iter != TransformedPolyObjects.end();
    ++iter)
  {
    //loop over all point in each polygons of this object
    std::vector<Polygon*> polygons = (*iter).second;
    for (size_t i = 0; i < polygons.size(); i++) {
      Polygon* poly = polygons[i];
      for (unsigned int i = 0; i < poly->NumberOfPoints(); i++) {
        Vector3D* point = poly->GetPoint(i);
        currCoord = fabs(point->X);
        if (currCoord > maxcoord) maxcoord = currCoord;
        currCoord = fabs(point->Y);
        if (currCoord > maxcoord) maxcoord = currCoord;
        currCoord = fabs(point->Z);
        if (currCoord > maxcoord) maxcoord = currCoord;
      }
    }
  }

  RenderCubeRadius = maxcoord;
  RenderCubeDiv = CUBE_DIV; //we set this hard 
  //2 -> 3 min  
  //3 -> 1 min
}


//*******************************
// PolygonsToCubes
//
// effective rendering
//
// puts every polygon into a cube 
//*******************************
void cgi::SimpleRaytracer::PolygonsToCubes(){
  
  std::cout << "PolygonsToCubes" << std::endl;
  
  const double a = RenderCubeRadius / static_cast<double>(RenderCubeDiv);
  
  double x0, x1, y0, y1, z0, z1;
  int cubenumber;
  for (int z = -static_cast<int>(RenderCubeDiv); z < static_cast<int>(RenderCubeDiv); z++){
    z0 = static_cast<double>(z) * RenderCubeRadius;
    z1 = z0 + a; 
    for (int y = -static_cast<int>(RenderCubeDiv); y < static_cast<int>(RenderCubeDiv); y++){
      y0 = static_cast<double>(y) * RenderCubeRadius;
      y1 = y0 + a; 
      for (int x = -static_cast<int>(RenderCubeDiv); x < static_cast<int>(RenderCubeDiv); x++){
        x0 = static_cast<double>(x) * RenderCubeRadius;
        x1 = x0 + a;
        cubenumber =   (x + RenderCubeDiv)
                     + (y + RenderCubeDiv) * (RenderCubeDiv * 2)
                     + (z + RenderCubeDiv) * (RenderCubeDiv * 2) * (RenderCubeDiv * 2);
                     
        std::cout << "check CUBE: " << x << ", " << y << ", " << z << " -> " << cubenumber << std::endl;
        
        //loop through every polygon 
        for (std::map<PolygonObject*, std::vector<Polygon*> >::iterator iter = TransformedPolyObjects.begin();
             iter != TransformedPolyObjects.end();
             ++iter)
        {
          std::vector<Polygon*> polygons = (*iter).second;
          for (size_t i = 0; i < polygons.size(); i++){
            Polygon* poly = polygons[i];
            
            if (IsPolygonInCube(poly, x0, x1, y0, y1, z0, z1)){
              std::cout << "CUBE: " << cubenumber << " Poly: " << reinterpret_cast<int>(poly) << std::endl;
              CubesWithPolygons[cubenumber].push_back(poly);
            }
          }
        }
         
      }  
    }
  }     
}

//***********************************************
// IsPolygonInCube
//
// effective rendering
//
// returns true if at least one geometrical point 
// of the polygon is inside the cube
//***********************************************
bool cgi::SimpleRaytracer::IsPolygonInCube(Polygon* poly, 
                                           const double x0, const double x1, 
                                           const double y0, const double y1, 
                                           const double z0, const double z1)
{
  //1st: check if at least one point is inside cube
  Vector3D p0 = *poly->GetPoint(0);
  if (IsPointInCube(p0, x0, x1, y0, y1, z0, z1)) return true;
  
  Vector3D p1 = p0 + *poly->GetPoint(1);                                             
  if (IsPointInCube(p1, x0, x1, y0, y1, z0, z1)) return true;
  
  Vector3D p2 = p0 + *poly->GetPoint(2);                                             
  if (IsPointInCube(p2, x0, x1, y0, y1, z0, z1)) return true;

  //2nd intersection polygon edge -> cube side
  if (IntersectTriangleEgdeCube(p0, *poly->GetPoint(1), x0, x1, y0, y1, z0, z1)) return true;
  if (IntersectTriangleEgdeCube(p0, *poly->GetPoint(2), x0, x1, y0, y1, z0, z1)) return true;
  if (IntersectTriangleEgdeCube(p1, (p2 - p1), x0, x1, y0, y1, z0, z1)) return true;

  //3rd intersection cube edge -> polygon 
  if (IntersectCubeEdgeTriangel(p0, p1, p2, x0, x1, y0, y1, z0, z1)) return true;
  
  return false;
}

//***********************************************
// IsPointInCube
//
// effective rendering
//
// returns true if the point is inside the cube
//***********************************************
inline bool cgi::SimpleRaytracer::IsPointInCube(const Vector3D& p, 
                                                const double x0, const double x1, 
                                                const double y0, const double y1, 
                                                const double z0, const double z1)
{
  return p.X >= x0 && p.X <= x1 && p.Y >= y0 && p.Y <= y1 && p.Z >= z0 && p.Z <= z1;
}


//*************************************************************
// IntersectTriangleEgdeCube
//
// checks if a given triangle edge intersects with cube surface
//*************************************************************
inline bool cgi::SimpleRaytracer::IntersectTriangleEgdeCube(const Vector3D& p,
                                                            const Vector3D& dir, 
                                                            const double x0, const double x1, 
                                                            const double y0, const double y1, 
                                                            const double z0, const double z1)
{
  Vector3D intersect;
  double u, v;
  
  //behind
  double size = DistanceToPolygon(p, dir, Vector3D(x0, y0, z0), Vector3D(x0, y1, z0), Vector3D(x1, y0, z0), intersect, u, v, true);
  if (size >= 0 && size <= 1) return true; 
  
  //left
  size = DistanceToPolygon(p, dir, Vector3D(x0, y0, z0), Vector3D(x0, y0, z1), Vector3D(x0, y1, z0), intersect, u, v, true);
  if (size >= 0 && size <= 1) return true; 
  
  //front
  size = DistanceToPolygon(p, dir, Vector3D(x0, y0, z1), Vector3D(x1, y0, z1), Vector3D(x0, y1, z1), intersect, u, v, true);
  if (size >= 0 && size <= 1) return true; 
  
  //right
  size = DistanceToPolygon(p, dir, Vector3D(x1, y0, z0), Vector3D(x1, y0, z1), Vector3D(x1, y1, z0), intersect, u, v, true);
  if (size >= 0 && size <= 1) return true; 
  
  //top
  size = DistanceToPolygon(p, dir, Vector3D(x0, y1, z0), Vector3D(x0, y1, z0), Vector3D(x0, y1, z0), intersect, u, v, true);
  if (size >= 0 && size <= 1) return true; 
  
  //bottom
  size = DistanceToPolygon(p, dir, Vector3D(x0, y0, z0), Vector3D(x1, y0, z1), Vector3D(x0, y0, z1), intersect, u, v, true);
  if (size >= 0 && size <= 1) return true;
  
  //no intersection
  return false;                                             
}



//*************************************************************
// IntersectTriangleEgdeCube
//
// checks if one cube edge intersects with triangel surface
//*************************************************************
inline bool cgi::SimpleRaytracer::IntersectCubeEdgeTriangel(const Vector3D& pt0,
                                                            const Vector3D& pt1, 
                                                            const Vector3D& pt2,
                                                            const double x0, const double x1, 
                                                            const double y0, const double y1, 
                                                            const double z0, const double z1)
{
  Vector3D intersect;
  double u, v;
  
  Vector3D p0(x0, y0, z0); 
  Vector3D p1(x1, y0, z0); 
  Vector3D p2(x1, y0, z1); 
  Vector3D p3(x0, y0, z1); 
  Vector3D p4(x0, y1, z0); 
  Vector3D p5(x1, y1, z0); 
  Vector3D p6(x1, y1, z1); 
  Vector3D p7(x0, y1, z1); 
  
  double size = DistanceToPolygon(p0, (p1 - p0), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p1, (p2 - p1), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p2, (p3 - p2), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p3, (p0 - p3), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  
  size = DistanceToPolygon(p4, (p5 - p4), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p5, (p6 - p5), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p6, (p7 - p6), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p7, (p4 - p7), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true;
   
  size = DistanceToPolygon(p0, (p4 - p0), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p1, (p5 - p1), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p2, (p6 - p2), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true; 
  size = DistanceToPolygon(p3, (p7 - p3), pt0, pt1, pt2, intersect, u, v, false);
  if (size >= 0 && size <= 1) return true;
  
  return false;
}


//***********************
// RenderScene
//
// actual render function
//***********************
void cgi::SimpleRaytracer::RenderToImage(PolygonalScene& scene, image::TTImage& resultImage){
  
  std::cout << "RenderToImage" << std::endl;
  
  const unsigned int MAXDEPTH = 5;
  

  const unsigned int width = resultImage.Width();
  const unsigned int height = resultImage.Height();

  Vector3D origin; //is always 0.0, 0.0, 0.0
  
  for (unsigned y = 0; y < height; y++){
    std::cout << "line: " << y << std::endl;
    for (unsigned x = 0; x < width; x++){
      
      if (x == width / 2 && y == height / 2) {
        std::cout << "sfsfsfsfsfsf" << std::endl;
      }
      const Vector3D direction = scene.GetCamera().RayDirection(x, y);
      //std::cout << "direction: " << direction.ToString() << std::endl;
      
      std::stack<double> refractionStack; 
      const RGBColor pixel = TraceOneRay(scene, origin, direction, nullptr, refractionStack, 0, MAXDEPTH).Clip();
      //todo 
      const image::TTColor pixelColor = image::TTColor(static_cast<image::t_proc>(pixel.R), static_cast<image::t_proc>(pixel.G), static_cast<image::t_proc>(pixel.B));

      resultImage.SetColor(x, y, pixelColor);
    }
  }
}

//***************************
// RenderScene
//
// gets the color for one ray
//***************************
cgi::RGBColor cgi::SimpleRaytracer::TraceOneRay(PolygonalScene& scene,
                                                const Vector3D& O,
                                                const Vector3D& E,
                                                const Polygon* const lastPoly,
                                                std::stack<double> refractionIndexStack,
                                                const unsigned int currDepth,
                                                const unsigned int MAXDEPTH)
{
  //HINT: E == rayDirection and N are already normized
  
  Polygon* currentPoly;
  Material surface; //material of surface
  Vector3D IP; //intersection point 
  Vector3D N;  //norm vector of surface

  if (GetNearestValidPolygon(O, E, lastPoly, surface, IP, N, currentPoly) > -1){
    
    if (debug){
      std::cout << "surface: " << surface.Ambient.ToString() << " " << surface.Diffuse.ToString() << " " << surface.Specular.ToString() << std::endl;   
      std::cout << "intersection: " << IP.ToString() << std::endl;
      std::cout << "normal: " << N.ToString() << std::endl;
    }
    
    //--------------------- get illumination ----------------------
     
    //loop over all light sources
    RGBColor diffuseLight;
    RGBColor specularLight;
    for (size_t i = 0; i < TransformedLights.size(); ++i){
      
      const Light* const light = TransformedLights[i];
      //we do not skip negative lights anymore! Let the black sun shine...
      //if (candle.IsBlack()) continue; //skip black (and negative) lights
      
      //get subtypes
      const InfiniteLight* const infiniteLight = dynamic_cast<const InfiniteLight*>(light);
      const SphereLight*   const sphereLight   = dynamic_cast<const SphereLight*>(light);
      const Projector*     const projector     = dynamic_cast<const Projector*>(light);
       
      //get direction of illumination 
      Vector3D L;  //direction of light to IP on the surface
      double dl;
      if (infiniteLight){
        L = (infiniteLight->GetPosition() - infiniteLight->GetTarget()).Norm();
        dl = -1; 
      }
      else{
        L = light->GetPosition() - IP;
        dl = L.Abs();
        if (dl == 0) continue; //skip light source on this position 
        L = L.Norm();
      }
      
      //make light visible 
      //@TODO: later
      //needed to check if lightsource comes from behind
      const double cln = L * N;
      const double cen = E * N;
      
      RGBColor shadow;
      //current polygon is transparent for this light
      if (false){
      
      }
      //light source from behind? 
      else if (((cln < 0) && (cen < 0)) || ((cln > 0) && (cen > 0))){
        shadow = RGBColor(0.0, 0.0, 0.0);
      }
      //other polygons between lightsource?
      else{
        if (!infiniteLight){
          shadow = GetShadow(IP, light->GetPosition(), dl, currentPoly);
        }
        else{
          shadow = GetShadow(IP, L + IP, dl, currentPoly);
        }
      }
      
      //calculate illumination when shadowing is not completely
      if (shadow.IsBlack()) continue;

      RGBColor candle = light->GetIlumination();
      candle *= shadow;

      RGBColor lux;
      if (infiniteLight){
        lux = candle;
      }
      else if (projector){
        const Vector3D P = (projector->GetPosition() - projector->GetTarget()).Norm();
        double p = fabs(P * L);
        if (p < 0.0) p = 0.0;
        lux = candle * (pow(p, projector->GetCharacter()) * (1.0 / (0.0 + 1.0 * dl)) );
      }
      else{
        lux = candle * (1.0 / (0.0 + 1.0 * dl));
      }
      
      const Vector3D R = N * (2.0 * (N * L)) - L; //reflexions vector
      specularLight += lux * pow(fabs(R * (!E)), surface.Roughness);
      diffuseLight += lux * fabs(L * N);
    }
    
    RGBColor pixelColor = scene.GetAmbientLight() * surface.Ambient
                          + diffuseLight  * surface.Diffuse
                          + specularLight * surface.Specular;
                              
    //--------------------- refraction ----------------------
    if (!surface.Transparency.IsBlack() && currDepth < MAXDEPTH){
      
      //in case that the last refraction index is equal to the one of the surface
      //we assume that we will leave the body 
      //PROBLEM: intersected bodies! 
      
      //1.0 -> N1
      double refractionRatio = 0.0;
      if (refractionIndexStack.empty()){
        refractionIndexStack.push(surface.RefractionIndex); 
        refractionRatio = 1.0 / surface.RefractionIndex;
      }
      else{
        refractionRatio = refractionIndexStack.top();
        //N1 -> N2
        if (refractionRatio != surface.RefractionIndex){
          refractionIndexStack.push(surface.RefractionIndex); 
          refractionRatio /= surface.RefractionIndex;  
        }
        //N2 -> N1
        else{
          refractionIndexStack.pop();
          //N2 -> 1.0
          if (refractionIndexStack.empty()){
            refractionRatio = surface.RefractionIndex;
          }
          //N2 -> N1
          else{
            refractionRatio = surface.RefractionIndex / refractionIndexStack.top();  
          }
        }  
      }  
        
      Vector3D T = E + N * (N * E); //tangential vector (orthogonal to normal vector) - used for refraction
      T *= refractionRatio;
      Vector3D Br = T - N; //refraction vector 
      Br = Br.Norm();
      
      //Br = E; simple case, no refraction
      
      pixelColor *= !surface.Transparency;
      pixelColor += TraceOneRay(scene, IP, Br, currentPoly, refractionIndexStack, currDepth + 1, MAXDEPTH) * surface.Transparency;
    }
    
    //--------------------- reflection ----------------------
    if (!surface.Reflection.IsBlack() && currDepth < MAXDEPTH){
      const Vector3D R = E - (N * (2.0 * (N * E)));  //reflexions vector
      pixelColor *= !surface.Reflection;
      pixelColor += TraceOneRay(scene, IP, R, currentPoly, refractionIndexStack, currDepth + 1, MAXDEPTH) * surface.Reflection;
    }
    
    return pixelColor;
  }
  //no surface in this direction found
  else{
    return scene.GetBackColor();  
  }
  
}


//*********************************************
// CubesOfRay
//
// searches all cubes through with the ray goes
//
// HINT: see CalcRenderCubeRadius()
//*********************************************    
void cgi::SimpleRaytracer::CubesOfRay(std::list<int>& cubes, 
                                      const Vector3D& rayOrigin,
                                      const Vector3D& rayDirection)
{
  const double a = RenderCubeRadius / static_cast<double>(RenderCubeDiv);
  unsigned int x = static_cast<int>((rayOrigin.X + RenderCubeRadius) / a);
  unsigned int y = static_cast<int>((rayOrigin.Y + RenderCubeRadius) / a);
  unsigned int z = static_cast<int>((rayOrigin.Z + RenderCubeRadius) / a);
  //std::cout << "Start in Cube: " << x << ", " << y << ", " << z << std::endl;
  
  std::list<double> changesX = GetChanges(rayOrigin.X, rayDirection.X, a, RenderCubeRadius * 2);
  std::list<double> changesY = GetChanges(rayOrigin.Y, rayDirection.Y, a, RenderCubeRadius * 2);
  std::list<double> changesZ = GetChanges(rayOrigin.Z, rayDirection.Z, a, RenderCubeRadius * 2);
  
  //for (std::list<double>::iterator it = changesX.begin(); it != changesX.end(); ++it) std::cout << "change X: " << (*it) << std::endl; 
  //for (std::list<double>::iterator it = changesY.begin(); it != changesY.end(); ++it) std::cout << "change Y: " << (*it) << std::endl; 
  //for (std::list<double>::iterator it = changesZ.begin(); it != changesZ.end(); ++it) std::cout << "change Z: " << (*it) << std::endl; 
  
  double cx, cy, cz; 
  
  unsigned int next; // 1==x, 2==y, 3==z, 0==nothing more
  
  std::list<double>::iterator itx = changesX.begin();
  std::list<double>::iterator ity = changesY.begin();
  std::list<double>::iterator itz = changesZ.begin();
  int ix = 0;
  int iy = 0;
  int iz = 0;
  const int dix = rayDirection.X >= 0 ? 1 : -1;
  const int diy = rayDirection.Y >= 0 ? 1 : -1;
  const int diz = rayDirection.Z >= 0 ? 1 : -1;
  
  //add first cube at first
  unsigned int cubenumber = x + y * RenderCubeDiv * 2 + z * RenderCubeDiv * RenderCubeDiv * 4; 
  cubes.push_back(cubenumber);
  
  do{
    
    next = 0;
    
    cx = itx != changesX.end() ? (*itx) : 0.0;
    cy = ity != changesY.end() ? (*ity) : 0.0;
    cz = itz != changesZ.end() ? (*itz) : 0.0;
    
    //std::cout << "current check: " << ix << ", " << iy << ", " << iz << std::endl;
    //std::cout << "    change at: " << cx << ", " << cy << ", " << cz << std::endl;
    
    
    //x vorhanden?
    if (cx > 0.0) next = 1; 
    
    //y vorhanden und näher als x? 
    if ((cy > 0.0) && ((cy < cx) || next == 0)) next = 2; 
  
    //z < next
    if ( (cz > 0.0) && ((next == 0) || (next == 1 && cz < cx) || (next == 2 && cz < cy)) ) next = 3;
    
    switch (next){
      case 1: ++itx; ix += dix; break;
      case 2: ++ity; iy += diy; break;
      case 3: ++itz; iz += diz; break;  
    }
    
    //std::cout << "       next: " << next << std::endl;
    
    if (next > 0){
      cubenumber = (ix + x) + (iy + y) * RenderCubeDiv * 2 + (iz + z) * RenderCubeDiv * RenderCubeDiv * 4; 
      cubes.push_back(cubenumber);   
    }
  } while (next > 0);
  
}
    

//****************************************************
// GetChanges
//
// checks when the ray changes into a new row of cubes
//****************************************************
std::list<double> cgi::SimpleRaytracer::GetChanges(const double pos0,     //relative position in the first cube (in units), is positive!
                                                   const double d,        //projection of 1 
                                                   const double a,        //size of cubes
                                                   const double maxdist)
{
  //std::cout << "GetChanges...." << std::endl;
  std::list<double> changes;
  if (d == 0.0) return changes;
  
  const double dabs = fabs(d);
  //std::cout << "  GetChanges.X: " << pos0 << " " << d << " " << a << " " << maxdist << " " << dabs << std::endl;
  
  const double changePeriode = a / dabs;  //innerhalb dieser Strecke wechselt es
  double toFirstChange = d > 0 ? ((a - pos0) / dabs) : (pos0 / dabs); //dauer bis zum ersten Wechsel
  if (toFirstChange <= 0) toFirstChange = changePeriode;
  
  //std::cout << "  changePeriode: " << changePeriode << " toFirstChange: " << toFirstChange << std::endl;
  
  double change = toFirstChange;
  while (change <= maxdist){
    changes.push_back(change);
    //std::cout << "GetChanges changes.push_back:" << change << std::endl;
    change += changePeriode;
  }
 
  return changes;
} 
  
//************************************************************
// GetNearestValidPolygon
//
// seaches for the next polygon that is intersected by the ray
// TODO: this is only useful for polygonal objects 
//       maybe we should use a more general method
//************************************************************
double cgi::SimpleRaytracer::GetNearestValidPolygon(const Vector3D& rayOrigin,
                                                    const Vector3D& rayDirection,
                                                    const Polygon*  currentPolygon, //this is to prevent that the last polygon was found
                                                    Material& surface,
                                                    Vector3D& intersection,
                                                    Vector3D& normal,
                                                    Polygon* &nearestPoly) 
{
  //loop over all polygonal objects
  double dmin = -1;
  nearestPoly = NULL;
  Vector3D currIntersection;
  double d;
  double u, v, nearestU, nearestV;
  
  //------------ SIMPLE VERSION loop over all polygons of the scene ------------
  
  for (std::map<PolygonObject*, std::vector<Polygon*> >::iterator iter = TransformedPolyObjects.begin();
       iter != TransformedPolyObjects.end();
       ++iter)
  {
    //loop over all polygons of this object
    std::vector<Polygon*> polygons = (*iter).second;
    for (size_t i = 0; i < polygons.size(); i++){
      Polygon* poly = polygons[i];
      if (poly != currentPolygon){
        d = DistanceToPolygon(rayOrigin, rayDirection, 
                              *poly->GetPoint(0), 
                              *poly->GetPoint(1), 
                              *poly->GetPoint(2),
                              currIntersection, u, v, 
                              false);  
        if ((d > -1) && (d < dmin || dmin < 0)){
          dmin = d;
          nearestPoly = poly;
          intersection = currIntersection;  
          nearestU = u;
          nearestV = v;
        }
      } 
    }  
  }
  
  
  //-------- NEW VERSION WE ONLY LOOK INTO THE CUBES THROUGH WHICH THE RAY GOES -----------
  /*
  std::list<int> cubes;
  CubesOfRay(cubes, rayOrigin, rayDirection);
  
  //std::cout << "-------------------- CUBES --------------------" << std::endl; 
  
  //loop over all cubes
  for (std::list<int>::iterator it = cubes.begin(); it != cubes.end(); ++it){
    const unsigned int cubenumber = *it;
    //std::cout << "ray goes through cube: " << cubenumber << std::endl; 
    
    //loop over all polygons in this cube
    std::list<Polygon*> polygonList = CubesWithPolygons[cubenumber];
    for (std::list<Polygon*>::iterator it = polygonList.begin(); 
         it != polygonList.end();
         ++it)
    {
      Polygon* poly = *it;
      //std::cout << "found polygon: " << (int)poly << std::endl; 
      if (poly != currentPolygon){
        d = DistanceToPolygon(rayOrigin, rayDirection, 
                              *poly->GetPoint(0), 
                              *poly->GetPoint(1), 
                              *poly->GetPoint(2),
                              currIntersection, u, v, 
                              false);  
        if ((d > -1) && (d < dmin || dmin < 0)){
          dmin = d;
          nearestPoly = poly;
          intersection = currIntersection;  
          nearestU = u;
          nearestV = v;
        }
      } 
    }
    //break, all other cubes and therefor all polygon in it are further away
    if (nearestPoly) break;
  }
  */
  
  if (nearestPoly){
    surface = nearestPoly->GetSurface();
    
    //flat shading
    //normal = *nearestPoly->GetNormal(0); //use the polygon wide normal vector
    
    //interpolation of normals
    normal = ((1.0 - nearestU - nearestV) * *nearestPoly->GetNormal(1)) + (nearestU * *nearestPoly->GetNormal(2)) + (nearestV * *nearestPoly->GetNormal(3));
    normal = normal.Norm();
  }
  return dmin;
}    


//*******************************************************
// GetShadow
//
// goes through all surface and summs the shadow between
// current intersection point and lightsources
//******************************************************
cgi::RGBColor cgi::SimpleRaytracer::GetShadow(const Vector3D& IP, 
                                              const Vector3D& L, 
                                              const double dl, 
                                              const Polygon*  currentPolygon) //this is to prevent that the last polygon was found
{
  RGBColor shadow(1.0, 1.0, 1.0);
  
  Vector3D rayDirection = (L - IP).Norm();
  
  Vector3D currIntersection;
  double d;
  double u, v;
  for (std::map<PolygonObject*, std::vector<Polygon*> >::iterator iter = TransformedPolyObjects.begin();
       iter != TransformedPolyObjects.end();
       ++iter)
  {
    //loop over all polygons of this object
    std::vector<Polygon*> polygons = (*iter).second;
    for (size_t i = 0; i < polygons.size(); i++){
      Polygon* poly = polygons[i];
      if (poly != currentPolygon){
        d = DistanceToPolygon(IP, rayDirection, 
                              *poly->GetPoint(0), 
                              *poly->GetPoint(1), 
                              *poly->GetPoint(2),
                              currIntersection, u, v, 
                              false);  
        if ((d > 0) && (d < dl || dl < 0)){
          shadow *= poly->GetSurface().Transparency;
        }
        //we need not to look any further if it is already completely shadowed
        if (shadow.IsBlack()) return shadow;
      } 
    }  
  }
  return shadow;
}
                            
//*************************************************************
// DistanceToPolygon
//
// calculates the distance to the given poylgon, 
// the intersection point, u and v
// in case that the polygon will not intersected, it returns -1
//*************************************************************
double cgi::SimpleRaytracer::DistanceToPolygon(const Vector3D& rayOrigin,
                                               const Vector3D& rayDirection,
                                               const Vector3D& p0,
                                               const Vector3D& p1,
                                               const Vector3D& p2,
                                               Vector3D &intersect, 
                                               double &u, 
                                               double &v,
                                               const bool square)
{
  double distance= -1;
  //if (debug) std::cout << "DistanceToPolygon: " << rayDirection.ToString() << " -> " << p0.ToString() << "-" << p1.ToString() << "-" << p2.ToString() << std::endl;
  //matrix elements
  //double a = p1.X; double b = p2.X; double c = -rayDirection.X; double j = rayOrigin.X - p0.X;
  //double d = p1.Y; double e = p2.Y; double f = -rayDirection.Y; double k = rayOrigin.Y - p0.Y;
  //double g = p1.Z; double h = p2.Z; double i = -rayDirection.Z; double l = rayOrigin.Z - p0.Z;
  const double ae = p1.X * p2.Y; 
  const double bf = -p2.X * rayDirection.Y; 
  const double cd = -rayDirection.X * p1.Y;
  const double af = -p1.X * rayDirection.Y; 
  const double bd = p2.X * p1.Y; 
  const double ce = -rayDirection.X * p2.Y;
  const double je = (rayOrigin.X - p0.X) * p2.Y; 
  const double ck = -rayDirection.X * (rayOrigin.Y - p0.Y);
  const double jf = (-rayOrigin.X + p0.X) * rayDirection.Y; 
  const double bk = p2.X * (rayOrigin.Y - p0.Y);
  const double jd = (rayOrigin.X - p0.X) * p1.Y; 
  const double ak = p1.X * (rayOrigin.Y - p0.Y);
  
  //determinants
  //const double D0 = a*e*i + b*f*g + c*d*h - a*f*h - b*d*i - c*e*g;
  //const double D1 = j*e*i + b*f*l + c*k*h - j*f*h - b*k*i - c*e*l;
  //const double D2 = a*k*i + j*f*g + c*d*l - a*f*l - j*d*i - c*k*g;
  //const double D3 = a*e*l + b*k*g + j*d*h - a*k*h - b*d*l - j*e*g;
  const double D0 = -ae * rayDirection.Z + bf * p1.Z + cd * p2.Z - af * p2.Z + bd * rayDirection.Z - ce * p1.Z;
  const double D1 = -je * rayDirection.Z + bf * (rayOrigin.Z - p0.Z) + ck * p2.Z - jf * p2.Z + bk * rayDirection.Z - ce * (rayOrigin.Z - p0.Z);
  const double D2 = -ak * rayDirection.Z + jf * p1.Z + cd * (rayOrigin.Z - p0.Z) - af * (rayOrigin.Z - p0.Z) + jd * rayDirection.Z - ck * p1.Z;
  const double D3 =  ae * (rayOrigin.Z - p0.Z) + bk * p1.Z + jd * p2.Z - ak * p2.Z - bd * (rayOrigin.Z - p0.Z) - je * p1.Z;

  //solutions
  if (D0 != 0){ //if determinant == 0, no intersection
    u = D1 / D0;
    v = D2 / D0;
    const double t = D3 / D0;
    intersect = rayOrigin + t * rayDirection;
    //if (debug) std::cout << "intersect: " << intersect.ToString() << std::endl;
    if (!square){
      if ((t > 0) && (((u >= 0) && (v >= 0)) && ((v + u) <= 1))) distance = t * rayDirection.Abs();
    }
    else {
      if ((t > 0) && ((u >= 0) && (u <= 1)) && ((v >= 0) && (v <= 1))) distance = t * rayDirection.Abs();
    }
  }
  
  return distance;
}
                       
