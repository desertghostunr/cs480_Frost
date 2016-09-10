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

  rotationRate = 1.0;
  orbitRate = 1.0;

  rotationControlMultiplier = 1.0;
  orbitControlMultiplier = -1.0;


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

  orbitalAngle += orbitControlMultiplier * orbitRate * dt * M_PI/8500;

  model = glm::translate( glm::mat4(1.0f), 
                          glm::vec3(7.5f * cos( orbitalAngle ), 0.0f,
                                    7.5f * sin( orbitalAngle) ) )
   * glm::rotate( glm::mat4(1.0f), (angle), glm::vec3(0.0, 1.0, 0.0));  
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

// UPDATE ROTATION //////////////////
/***************************************

@brief updateRotation

@details updates the objects rotation

@param in: rotFactor: controls the direction and speed of the object's rotation

@notes None

***************************************/
void Object::updateRotation( float rotFactor )
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


// UPDATE ORBIT //////////////////
/***************************************

@brief updateOrbit

@details updates the objects orbit

@param in: orbitFactor: controls the direction and speed of the object's orbit

@notes None

***************************************/
void Object::updateOrbit( float orbitFactor )
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

