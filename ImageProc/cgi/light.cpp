#include "light.h"

//*************************************************************************
//*                                                                       *
//*                                Light                                  *
//*                                                                       *
//*                  base class for all kind of light sources             * 
//*************************************************************************

// counter for lights
unsigned int cgi::Light::count = 0;

//************
// constrcutor 
//************
cgi::Light::Light(const Vector3D& pos,
                  Object* parent,
                  const RGBColor& color,
                  std::string name)
           :Object(pos, pos, parent), Color(color)
{
  ++count;
  if (name.length() == 0){ 
    std::ostringstream os;
    os << "Light_" << count;  
    name = os.str();
  }
  Name = name;
}
          
//***********
// destructor 
//***********
cgi::Light::~Light(){}

//*********
// DeepCopy
//*********
cgi::Light* cgi::Light::DeepCopy(){
  return new Light(Position, NULL, Color, Name);
}


//************************
// GetIlumination
// 
// returns the light color 
//************************
cgi::RGBColor cgi::Light::GetIlumination() const{
  return Color;  
}
    
//*********************
// SetIlumination
// 
// sets the light color 
//*********************
void cgi::Light::SetIlumination(const RGBColor& color){
  Color = color; 
}

//***************************************************
// PositionInWorld
// 
// gets the postion of the light in world coordinates
//***************************************************
cgi::Vector3D cgi::Light::PositionInWorld(){
  return Parent ? Parent->PointInWorld(Position) : Position;
}


//*************************************************************************
//*                                                                       *
//*                             DirectedLight                             *
//*                                                                       *
//*            base class for all kind of directed light sources          * 
//*************************************************************************

//************
// constrcutor 
//************
cgi::DirectedLight::DirectedLight(const Vector3D& pos,
                                  const Vector3D& target,
                                  Object* parent,
                                  const RGBColor& color,
                                  std::string name)
                   :Light(pos, parent, color, name)
{
  Rotation = target;
}
          
//***********
// destructor 
//***********
cgi::DirectedLight::~DirectedLight(){}

//*********
// DeepCopy
//*********
cgi::Light* cgi::DirectedLight::DeepCopy(){
  return new DirectedLight(Position, Rotation, NULL, Color, Name);
}

//**************************************
// SetTarget
//
// actually we abuse Rotation for target
//**************************************
cgi::DirectedLight& cgi::DirectedLight::SetTarget(const Vector3D& target){
  SetRotation(target);
  return *this;
}

//**************************************
// GetTarget
//
// actually we abuse Rotation for target
//**************************************
cgi::Vector3D cgi::DirectedLight::GetTarget() const{
  return GetRotation();  
}

//**************************************
// TranslateTarget
//
// actually we abuse Rotation for target
//**************************************
cgi::DirectedLight& cgi::DirectedLight::TranslateTarget(const Vector3D& target){
  Rotate(target);
  return *this;
}
    
//**************************************************
// TargetInWorld
// 
// gets the target of the light in world coordinates
//**************************************************
cgi::Vector3D cgi::DirectedLight::TargetInWorld(){
  return Parent ? Parent->PointInWorld(Rotation) : Rotation;  
}



//*************************************************************************
//*                                                                       *
//*                             InfiniteLight                             *
//*                                                                       *
//*                   a simple directed light like the sky                *
//*************************************************************************

//************
// constrcutor
//************ 
cgi::InfiniteLight::InfiniteLight(const Vector3D& pos,
                                  const Vector3D& target,
                                  Object* parent,
                                  const RGBColor& color,
                                  std::string name)
                   :DirectedLight(pos, target, parent, color, name)
{
}

//***********             
// destructor
//***********
cgi::InfiniteLight::~InfiniteLight(){}

//*********
// DeepCopy
//*********
cgi::Light* cgi::InfiniteLight::DeepCopy(){
  return new InfiniteLight(Position, Rotation, NULL, Color, Name);
}

//*************************************************************************
//*                                                                       *
//*                             SphereLight                               *
//*                                                                       *
//*              a undirected unidirection light like a ideal lamp        *
//*************************************************************************

//************
// constrcutor
//************ 
cgi::SphereLight::SphereLight(const Vector3D& pos,
                              Object* parent,
                              const RGBColor& color,
                              std::string name)
                 :Light(pos, parent, color, name)
{
  
}
    
//***********             
// destructor
//***********
cgi::SphereLight::~SphereLight(){}

//*********
// DeepCopy
//*********
cgi::Light* cgi::SphereLight::DeepCopy(){
  return new SphereLight(Position, NULL, Color, Name);
}

//*************************************************************************
//*                                                                       *
//*                             SphereLight                               *
//*                                                                       *
//*              a undirected unidirection light like a ideal lamp        *
//*************************************************************************

//************
// constrcutor
//************ 
cgi::Projector::Projector(const Vector3D& pos,
                          const Vector3D& target,
                          Object* parent,
                          const RGBColor& color,
                          std::string name,
                          double character)
               :DirectedLight(pos, target, parent, color, name),
                Character(character)           
{
}

//***********             
// destructor
//***********
cgi::Projector::~Projector(){}
    
//*********
// DeepCopy
//*********
cgi::Light* cgi::Projector::DeepCopy(){
  return new Projector(Position, Rotation, NULL, Color, Name, Character);
}

//*******************************
// SetCharacter
//
// character of the opening angle
//*******************************
void cgi::Projector::SetCharacter(const double character){
  Character = character;      
}

//*******************************
// GetCharacter
//
// character of the opening angle
//*******************************
double cgi::Projector::GetCharacter() const{
  return Character;  
}






















