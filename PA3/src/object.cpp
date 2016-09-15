#include "object.h"

Object::Object()
{  
  /*
    # Blender File for a Cube
    o Cube
    v 1.000000 -1.000000 -1.000000
    v 1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 1.000000
    v -1.000000 -1.000000 -1.000000
    v 1.000000 1.000000 -0.999999
    v 0.999999 1.000000 1.000001
    v -1.000000 1.000000 1.000000
    v -1.000000 1.000000 -1.000000
    s off
    f 2 3 4
    f 8 7 6
    f 1 5 6
    f 2 6 7
    f 7 8 4
    f 1 4 8
    f 1 2 4
    f 5 8 6
    f 2 1 6
    f 3 2 7
    f 3 7 4
    f 5 1 8
  */

  Vertices = {
    {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}
  };

  Indices = {
    2, 3, 4,
    8, 7, 6,
    1, 5, 6,
    2, 6, 7,
    7, 8, 4,
    1, 4, 8,
    1, 2, 4,
    5, 8, 6,
    2, 1, 6,
    3, 2, 7,
    3, 7, 4,
    5, 1, 8
  };

  // The index works at a 0th index
  for(unsigned int i = 0; i < Indices.size(); i++)
  {
    Indices[i] = Indices[i] - 1;
  }

  angle = 0.0f;
  orbitalAngle = 0.0f;

  orbitalRadius = 1.0f;

  rotationRate = 1.0f;
  orbitRate = 1.0f;

  rotationControlMultiplier = 1.0f;
  orbitControlMultiplier = -1.0f;

  origin = glm::vec3(0.0f, 0.0f,0.0f);
  positionInOribit = glm::vec3(0.0f, 0.0f,0.0f);

  glGenBuffers(1, &VB);
  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &IB);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
}

Object::~Object()
{
  Vertices.clear();
  Indices.clear();
}

void Object::Update(unsigned int dt)
{
 
  angle += rotationControlMultiplier * rotationRate * dt * M_PI/1000;

  orbitalAngle += orbitControlMultiplier * orbitRate * dt * M_PI/1000;

  //calculate the position in the object's orbit
  positionInOribit = glm::vec3( orbitalRadius * cos( orbitalAngle ), 
                                0.0f,
                                orbitalRadius * sin( orbitalAngle) );

  //based on matrix multiplication the transforms are applied
  //rotation -> orbital position -> origin
  model = glm::translate(glm::mat4(1.0f), origin) //move object to origin
        * glm::translate( glm::mat4(1.0f), positionInOribit ) //move the object to oribital position
        * glm::rotate( glm::mat4(1.0f), (angle), glm::vec3(0.0, 1.0, 0.0)); //rotate the object
}

glm::mat4 Object::GetModel()
{
  return model;
}

void Object::Render()
{
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, VB);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);

  glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

// UPDATE ROTATION RATE //////////////////
/***************************************

@brief updateRotationRate

@details updates the objects rotation rate

@param in: rotFactor: controls the direction and speed of the object's rotation

@notes None

***************************************/
void Object::updateRotationRate( float rotFactor )
{  
  rotationRate = rotFactor;  
}

// TOGGLE ROTATION DIRECTION //////////////////
/***************************************

@brief toggleRotationDirection

@details toggles the direction the object is rotating and cancels the pause state

@param None

@notes None

***************************************/
void Object::toggleRotationDirection( )
{
  if( rotationControlMultiplier == 0.0 )
  {
    rotationControlMultiplier = 1.0;
  }

  rotationRate *= -1.0;
}

// GET RATE OF ROTATION //////////////////
/***************************************

@brief getRateOfRotation

@details returns the rate of rotation

@param None

@notes None

***************************************/
float Object::getRateOfRotation( )
{
  return rotationRate;
}

// TOGGLE ROTATION PAUSED //////////////////
/***************************************

@brief toggleRotationPaused

@details toggles whether or not the rotation is paused

@param None

@notes None

***************************************/
void Object::toggleRotationPaused( )
{
  if( rotationControlMultiplier == 0.0)
  {
    rotationControlMultiplier = 1.0;
  }
  else
  {
    rotationControlMultiplier = 0.0;
  }
}


// UPDATE ORBIT RATE //////////////////
/***************************************

@brief updateOrbitRate

@details updates the objects orbit rate

@param in: orbitFactor: controls the direction and speed of the object's orbit

@notes None

***************************************/
void Object::updateOrbitRate( float orbitFactor )
{  
  orbitRate = orbitFactor;  
}

// TOGGLE ORBIT DIRECTION //////////////////
/***************************************

@brief toggleOrbitDirection

@details toggles the direction the object is orbiting and cancels the pause state

@param None

@notes None

***************************************/
void Object::toggleOrbitDirection( )
{
  if( orbitControlMultiplier == 0.0 )
  {
    orbitControlMultiplier = -1.0;
  }

  orbitRate *= -1.0;
}

// GET RATE OF ORBIT  //////////////////
/***************************************

@brief getRateOfOrbit

@details returns the rate of orbit

@param None

@notes None

***************************************/
float Object::getRateOfOrbit( )
{
  return orbitRate;
}

// TOGGLE ORBIT PAUSED //////////////////
/***************************************

@brief toggleOrbitPaused

@details toggles whether or not the orbit is paused

@param None

@notes None

***************************************/
void Object::toggleOrbitPaused( )
{
  if( orbitControlMultiplier == 0.0 )
  {
    orbitControlMultiplier = -1.0;
  }
  else
  {
    orbitControlMultiplier = 0.0;
  }
}

// TOGGLE ALL PAUSED //////////////////
/***************************************

@brief toggleAllPaused

@details toggles whether or not the orbit and rotation is paused

@param None

@notes None

***************************************/
void Object::toggleAllPaused( )
{
  if( ( rotationControlMultiplier == 0.0 ) && ( orbitControlMultiplier == 0.0 ) )
  {
    rotationControlMultiplier = 1.0;
    orbitControlMultiplier = -1.0;
  }
  else
  {
    rotationControlMultiplier = 0.0;
    orbitControlMultiplier = 0.0;
  }
}

// SET ORBITAL RADIUS //////////////////
/***************************************

@brief setOrbitalRadius

@details sets the object's radius of orbit

@param in: radius: the radius to use.

@notes None

***************************************/
void Object::setOrbitalRadius( float radius )
{
  orbitalRadius = radius;
}

// GET CURRENT POSITION OF ORBIT //////////////////
/***************************************

@brief getCurrentPositionOfOrbit

@details gets the object's current position of orbit

@param None

@notes None

***************************************/
glm::vec3 Object::getCurrentPositionOfOrbit( )
{
  return positionInOribit;
}

// SET ORIGIN //////////////////
/***************************************

@brief setOrigin

@details sets the object's origin

@param in: newOrigin: the newOrigin to use

@notes None

***************************************/
void Object::setOrigin( const glm::vec3 & newOrigin )
{
  origin = newOrigin;
}

