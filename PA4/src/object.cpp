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

  model = glm::mat4(1.0f);

  rotationVector = glm::vec3( 0.0f, 1.0f, 0.0f );

  angle = 0.0f;
  orbitalAngle = 0.0f;

  orbitalRadius = 1.0f;

  rotationRate = 1.0f;
  orbitRate = 1.0f;

  rotationControlMultiplier = 1.0f;
  orbitControlMultiplier = -1.0f;

  origin = glm::mat4(1.0f);

  isChildFlag = false;

  scaleFactor = glm::vec3( 1.0f, 1.0f, 1.0f );
  
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

void Object::Update( unsigned int dt )
{
  glm::mat4 lhMat, rhMat;

  int index;

  if( transformVector.empty( ) )
  {
    return;
  }

  if( transformVector.size( ) == 1 )
  {
    model = transformVector[ 0 ];

    transformVector.clear( );

    return;
  }

  //apply all transforms in the order they were added
  rhMat = transformVector[ 0 ];

  for( index = 1; index < transformVector.size( ); index++ )
  {
    lhMat = transformVector[ index ];
    rhMat = lhMat * rhMat;
  }

  model = rhMat;

  transformVector.clear( );
}

// UPDATE CHILDREN //////////////////
/***************************************

@brief updateChildren

@details updates the object's children

@param in: dt: the time delta

@param in: the table containing all objects

@param in: setParentAsOrigin: whether or not to set the parent as the origin of 
                              the child

@notes Recursively calls this on the childs children

***************************************/
void Object::UpdateChildren
( 
  unsigned int dt, 
  std::vector<Object>& objectTable,
  bool setParentAsOrigin
)
{
  int index, worldIndex;
  
  for( index = 0; index < childrenVector.size( ); index++ )
  {
    worldIndex = getChildsWorldID( index );

    if( ( worldIndex >= 0 ) && ( worldIndex < objectTable.size() ) ) 
    {
      if( setParentAsOrigin )
      {
      
        objectTable[ worldIndex ].setOrigin( model );
      }
      objectTable[ worldIndex ].incrementAngle( dt );
      objectTable[ worldIndex ].incrementOrbitAngle( dt );
      objectTable[ worldIndex ].createSatelliteTransform( );

      objectTable[ worldIndex ].Update( dt );
      
      //recursive call to children
      objectTable[ worldIndex ].UpdateChildren( dt, objectTable, 
                                                setParentAsOrigin );
    }   
  }
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

// SET ORIGIN //////////////////
/***************************************

@brief setOrigin

@details sets the object's origin

@param in: newOrigin: the newOrigin to use

@notes None

***************************************/
void Object::setOrigin( const glm::mat4 & newOrigin )
{
  origin = newOrigin;
}

// ADD CHILD //////////////////
/***************************************

@brief addChild

@details adds a childsWorldID to the table

@param in: childsWorldID: the id of the child in object table

@notes None

***************************************/
void Object::addChild( int childsWorldID )
{
  childrenVector.push_back( childsWorldID );
}

// GET CHILDS WORLD ID /////////////////////
/***************************************

@brief getChildsWorldID

@details gets the location of the child in object table

@param in: childsLocalID: the local ID of the child

@notes returns -1 if id is out of bounds

***************************************/

int Object::getChildsWorldID( int childsLocalID )
{
  if( ( childsLocalID >= 0 ) && ( childsLocalID < childrenVector.size() ) )
  {
    return childrenVector[ childsLocalID ];
  }

  return -1;
}

// GET NUMBER OF CHILDREN /////////////////////
/***************************************

@brief getNumberOfChildren

@details returns the number of children an object has.

@param None

@notes None

***************************************/
int Object::getNumberOfChildren( )
{
  return childrenVector.size( );
}

// SET CHILD STATUS FLAG /////////////////////
/***************************************

@brief setChildStatusFlag

@details sets the child status flag

@param in: flag: the flag value

@notes none

***************************************/
void Object::setChildStatusFlag( bool flag )
{
  isChildFlag = flag;
}

// IS CHILD /////////////////////
/***************************************

@brief isChild

@details returns true if the object is a child of another object

@param None

@notes none

***************************************/
bool Object::isChild( )
{
  return isChildFlag;
}

// SET SCALE  /////////////////////
/***************************************

@brief setScale

@details sets the scale of the object

@param in: scale: the objects new scale

@notes none

***************************************/
void Object::setScale( const glm::vec3& scale )
{
  scaleFactor = scale;
}

// SET SCALE  /////////////////////
/***************************************

@brief getScale

@details gets the scale of the object

@param None

@notes none

***************************************/
glm::vec3 Object::getScale( )
{
  return scaleFactor;
}

// CREATE ORBIT IN TRANSLATION VECTOR /////////////////////
/***************************************

@brief createOrbitInTranslationVector

@details creates an orbit in the translation

@param in: angle: a vector of angles to use_facet

@param in: radius: a vector of radii to apply

@param in: localOffset: a vector of offsets within the orbit calculation

@notes none

***************************************/
void Object::createOrbitInTranslationVector
( 
  const glm::vec3& angle,
  const glm::vec3& radius,
  const glm::vec3& localOffset 
)
{
  translationVector = glm::vec3( radius.x * cos( angle.x ), 
                                 radius.y * tan( angle.y ),
                                 radius.z * sin( angle.z ) );
}

// SET TRANSLATION VECTOR /////////////////////
/***************************************

@brief setTranslationVector

@details sets the translation vector for object

@param in: transVec: the vector to set the translation vector

@notes none

***************************************/
void Object::setTranslationVector( const glm::vec3& transVec )
{
  translationVector = transVec;
}

// SET ROTATION VECTOR /////////////////////
/***************************************

@brief setRotationVector

@details sets the rotation vector for the object

@param in: rotVec: the vector to set the orient the rotation around

@notes none

***************************************/
void Object::setRotationVector( const glm::vec3 rotVec )
{
  rotationVector = rotVec;
}

// COMMIT TRANSLATION /////////////////////
/***************************************

@brief commitTranslation

@details commits the translation vector for object

@param None

@notes none

***************************************/
void Object::commitTranslation( )
{
  transformVector.push_back( glm::translate( glm::mat4(1.0f),
                                             translationVector ) );
}

// COMMIT ROTATION/////////////////////
/***************************************

@brief commitRotation

@details commits the rotation vector for object

@param None

@notes none

***************************************/
void Object::commitRotation(  )
{
  transformVector.push_back( glm::rotate( glm::mat4(1.0f), 
                                          (angle), rotationVector ) );
}

// COMMIT SCALE /////////////////////
/***************************************

@brief commitScale

@details commits the scale vector on object

@param None

@notes none

***************************************/
void Object::commitScale( )
{
  transformVector.push_back( glm::scale( scaleFactor ) );
}

// COMMIT ORIGIN /////////////////////
/***************************************

@brief commitOrigin 

@details commits the origin matrix on object

@param None

@notes none

***************************************/
void Object::commitOrigin( )
{
  transformVector.push_back( origin );
}

// INCREMENT ANGLE /////////////////////
/***************************************

@brief incrementAngle 

@details increments the angle of the object's rotation

@param in: dt: the delta of time

@notes none

***************************************/
void Object::incrementAngle( unsigned int dt )
{
  angle += rotationControlMultiplier * rotationRate * dt * M_PI/1000;
}

// GET ANGLE /////////////////////
/***************************************

@brief getAngle 

@details gets the angle of the object's rotation

@param None

@notes none

***************************************/
float Object::getAngle( )
{
  return angle;
}

// SET ANGLE /////////////////////
/***************************************

@brief setAngle 

@details sets the angle of the object's rotation

@param in: newAngle: the new angle value

@notes none

***************************************/
void Object::setAngle( float newAngle )
{
  angle = newAngle;
}

// INCREMENT ORBIT ANGLE /////////////////////
/***************************************

@brief incrementOrbitAngle 

@details increments the angle of the object's orbit

@param in: dt: the delta of time

@notes none

***************************************/
void Object::incrementOrbitAngle( unsigned int dt )
{
  orbitalAngle += orbitControlMultiplier * orbitRate * dt * M_PI/1000;
}

// GET ORBITAL ANGLE /////////////////////
/***************************************

@brief getOrbitalAngle 

@details gets the angle of the object's orbit

@param None

@notes none

***************************************/
float Object::getOrbitAngle( )
{
  return orbitalAngle;
}

// SET ORBIT ANGLE /////////////////////
/***************************************

@brief setOrbitAngle 

@details sets the angle of the object's rotation

@param in: newAngle: the new angle value

@notes none

***************************************/
void Object::setOrbitAngle( float newAngle )
{
  orbitalAngle = newAngle;
}


// IS PAUSED  /////////////////////
/***************************************

@brief isPaused( )

@details returns whether or not the object is paused

@param None

@notes none

***************************************/
bool Object::isPaused( )
{
  return ( ( rotationControlMultiplier == 0.0 ) 
            && ( orbitControlMultiplier == 0.0 ) );
}

// CREATE SATELLITE TRANSFORM  /////////////////////
/***************************************

@brief createSatelliteTransform

@details creates the transforms necessary for a satellite

@param None

@notes none

***************************************/
void Object::createSatelliteTransform( )
{
  createOrbitInTranslationVector( glm::vec3( orbitalAngle, 0.0f, orbitalAngle ),
                                  glm::vec3( orbitalRadius, 0.0f, orbitalRadius ),
                                  glm::vec3( 0.0f, 0.0f, 0.0f ) );
  commitScale( );
  commitRotation( );
  commitTranslation( );
  commitOrigin( );
}
