#include "object.h"
#include <algorithm>

Object::Object()
{
    model = glm::mat4(1.0f);

	applyParent = true;

    bulletTransform = glm::mat4( 1.0f );

    rotationVector = glm::vec3( 0.0f, 1.0f, 0.0f );

    scaleFactor = glm::vec3( 1.0f, 1.0f, 1.0f );

    objectID = -1;
    parentID = -1;

    angle = 0.0f;

    objModelSelect = 0;

	type = BASE_OBJECT;

	parentModel = glm::mat4( 1.0f );

    render = true;
}

Object::Object( int nType )
{
	applyParent = true;
	model = glm::mat4( 1.0f );

	bulletTransform = glm::mat4( 1.0f );

	rotationVector = glm::vec3( 0.0f, 1.0f, 0.0f );

	scaleFactor = glm::vec3( 1.0f, 1.0f, 1.0f );

	objectID = -1;
	parentID = -1;

	angle = 0.0f;

    objModelSelect = 0;

	if( nType == BASE_OBJECT || nType == P_OBJECT || nType == PC_OBJECT )
	{
		type = nType;
	}
	else
	{
		type = BASE_OBJECT;
	}

	parentModel = glm::mat4( 1.0f );

    render = true;
}

Object::~Object()
{
    size_t index;

    for( index = 0; index < objModelPtr.size( ); index++ )
    {

        if( objModelPtr[ index ] != NULL )
        {
            objModelPtr[ index ]->decrementReference( );
        }


        objModelPtr[ index ] = NULL;
    }
}

void Object::Update( unsigned int dt )
{
    glm::mat4 lhMat, rhMat;

    unsigned int index;

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

	if( applyParent )
	{
		model = parentModel * rhMat;
	}
	else
	{
		model = rhMat;
	}

    transformVector.clear( );
}


glm::mat4 Object::GetModel()
{
    return model;
}


// GET ORIGIN //////////////////
/***************************************

@brief getOrigin

@details returns a reference to the object's Origin

@param None

@notes None

***************************************/
Origin Object::getOrigin( )
{
    Origin tmpOrigin;
    tmpOrigin.translation = translationVector;
    tmpOrigin.scale = scaleFactor;
    tmpOrigin.rotation = rotationVector;
    tmpOrigin.angle = angle;

    return tmpOrigin;
}

// GET OBJECT MODEL //////////////////
/***************************************

@brief getObjectModel

@details returns a reference to the object's Object Model

@param None

@notes None

***************************************/
ObjectModel & Object::getObjectModel( )
{
    return *objModelPtr[ objModelSelect ];
}


// HAS OBJECT MODEL //////////////////
/***************************************

@brief hasObjectModel

@details returns a true if the objModelPtr has been allocated

@param None

@notes Does not guarantee pointer validity!!!

***************************************/
bool Object::hasObjectModel( )
{
    if( objModelPtr.size( ) > 0 
        && objModelSelect < objModelPtr.size( ) 
        && objModelPtr[ objModelSelect ] != NULL )
    {
        return true;
    }

    return false;
}


// INITIALIZE //////////////////
/***************************************

@brief Initialize

@details Initializes the object with an ObjectModel

@param in: srcPtr: the object model to instance

@notes Not tested!!!

***************************************/
bool Object::Initialize( ObjectModel * const srcPtr )
{
    if( srcPtr == NULL )
    {
        return false;
    }

    objModelPtr.push_back( srcPtr );

    objModelSelect = objModelPtr.size( ) - 1;

    objModelPtr[ objModelSelect ]->incrementReference( );

    return true;
}




void Object::Render()
{
    unsigned int index;

    //no model, nothing to render
    if( !hasObjectModel( ) || !render )
    {
        return;
    }

    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glEnableVertexAttribArray( 2 );

    glBindBuffer( GL_ARRAY_BUFFER, objModelPtr[ objModelSelect ]->vertexBuffer( ) );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), 0 );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           ( void* ) offsetof( Vertex, uv ) );
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ),
                           ( void* ) offsetof( Vertex, normal ) );

    for( index = 0; 
         index < std::max( objModelPtr[ objModelSelect ]->getNumberOfIBs( ), //get max size
                           objModelPtr[ objModelSelect ]->getNumberOfTextures( ) );
         index++ )
    {
        if( index < objModelPtr[ objModelSelect ]->getNumberOfTextures( ) )
        {
            glUniform1i( objModelPtr[ objModelSelect ]->TextureUniformLocation( ), 0 );
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_2D, objModelPtr[ objModelSelect ]->Texture( index ) );
        }
        
        

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, objModelPtr[ objModelSelect ]->indexBuffer(
            std::min( index, objModelPtr[ objModelSelect ]->getNumberOfIBs( ) - 1 ) ) ); //prevent out of bounds

        //draw faces associated with a texture
        glDrawElements( GL_TRIANGLES,
                        objModelPtr[ objModelSelect ]->getIndices( index ).size( ),
                        GL_UNSIGNED_INT, 0 );
    }

    glDisableVertexAttribArray( 0 );
    glDisableVertexAttribArray( 1 );
    glDisableVertexAttribArray( 2 );
}

// SET ORIGIN //////////////////
/***************************************

@brief setOrigin

@details sets the object's origin

@param in: newOrigin: the newOrigin to use

@notes None

***************************************/
bool Object::setOrigin( const Origin & newOrigin )
{
    parentOrigin = newOrigin;

    return true;
}

// ADD CHILD //////////////////
/***************************************

@brief addChild

@details adds a childsWorldID to the table

@param in: childsWorldID: the id of the child in object table

@notes None

***************************************/
bool Object::addChild( unsigned int childsWorldID )
{
    if( ( childsWorldID != objectID ) 
            && ( childsWorldID != parentID ) 
            && ( ( int ) childsWorldID != -1 ) )
    {
        childrenVector.push_back( childsWorldID );
        return true;
    }

    return false;
    
}

// ADD PARENT //////////////////
/***************************************

@brief addParent

@details adds a parentsWorldID to the table

@param in: parentsWorldID: the id of the parent in object table

@notes None

***************************************/
bool Object::addParent( unsigned int parentsWorldID )
{ 
    unsigned int index;

    for( index = 0; index < childrenVector.size( ); index++ )
    {
        if( parentsWorldID == childrenVector[ index ] )
        {
            return false;
        }
    }

    if( ( parentsWorldID != objectID ) 
            && ( ( int ) parentsWorldID != -1 ) )
    {
        parentID = parentsWorldID;

        return true;
    }

    return false;
    
}

// GET CHILDS WORLD ID /////////////////////
/***************************************

@brief getChildsWorldID

@details gets the location of the child in object table

@param in: childsLocalID: the local ID of the child

@notes returns -1 if id is out of bounds

***************************************/

unsigned int Object::getChildsWorldID( unsigned int childsLocalID )
{
    if( childsLocalID < childrenVector.size() )
    {
        return childrenVector[ childsLocalID ];
    }

    return -1;
}

// GET PARENTS WORLD ID /////////////////////
/***************************************

@brief getParentsWorldID

@details gets the location of the parent in object table

@param None

@notes returns -1 if id is not set

***************************************/

unsigned int Object::getParentsWorldID( )
{
    return parentID;
}


// SET OBJECTS ID /////////////////////
/***************************************

@brief setObjectsID

@details sets the objects id

@param in: id: the id of the object

@notes none

***************************************/
bool Object::setObjectsID( unsigned int id )
{
    unsigned int index;

    for( index = 0; index < childrenVector.size( ); index++ )
    {
        if( id == childrenVector[ index ] )
        {
            return false;
        }
    }

    if( ( id != parentID ) && ( ( int ) id != -1 ) )
    {
        objectID = id;

        return true;
    }

    return false;
    
}

// GET OBJECTS ID /////////////////////
/***************************************

@brief getObjectsID

@details gets the objects id

@param None

@notes returns -1 if the id hasn't been set yet

***************************************/
unsigned int Object::getObjectsID( )
{
    return objectID;
}

// GET NUMBER OF CHILDREN /////////////////////
/***************************************

@brief getNumberOfChildren

@details returns the number of children an object has.

@param None

@notes None

***************************************/
unsigned int Object::getNumberOfChildren( )
{

    return childrenVector.size( );

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
    return ( ( int ) parentID != -1 );
}

// SET SCALE    /////////////////////
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

// SET SCALE    /////////////////////
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

void Object::commitTranslation( )
{
    transformVector.push_back( glm::translate( translationVector ) );
}

// COMMIT ROTATION/////////////////////
/***************************************

@brief commitRotation

@details commits the rotation vector for object

@param None

@notes none

***************************************/
void Object::commitRotation( )
{
    transformVector.push_back( glm::rotate( glm::mat4( 1.0f ),
                               ( angle ), rotationVector ) );
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

void Object::commitParentLocation( )
{
    transformVector.push_back( glm::translate( glm::mat4( 1.0f ), parentOrigin.translation ) );

}

// INCREMENT ANGLE /////////////////////
/***************************************

@brief incrementAngle 

@details increments the angle of the object's rotation

@param in: dt: the delta of time

@notes none

***************************************/
void Object::incrementAngle( unsigned int dt, double multiplier )
{
    angle += ( multiplier * dt ) * ( M_PI / 1000 );
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

std::string & Object::Name( )
{
    return name;
}

const std::string & Object::getName( )
{
    return name;
}

void Object::setBulletTransform( const glm::mat4 & bTrans )
{
    bulletTransform = bTrans;
}

void Object::commitBulletTransform( )
{
    transformVector.push_back( bulletTransform );
}

CollisionPtr & Object::CollisionInfo( )
{
    return collisionInfo;
}

CompoundCollisionPtr & Object::CompoundCollisionInfo( )
{
    return compoundCollisionInfo;
}


glm::vec3 & Object::getBScale( )
{
    return bScale;
}


const glm::vec3 & Object::getTransVec( )
{
    return translationVector;
}

int Object::getObjectType( )
{
	return type;
}

glm::vec4 Object::getPositionInWorld( )
{ 
	return model * glm::vec4( 0.0, 0.0, 0.0, 1.0 );
}

/********************

@brief getRotationInWorld

@details gets the rotation in world space

@param in: forward: the forward direction local to the object
					i.e. to set the x axis as forward pass in glm::vec3( 1.0f, 0.0f, 0.0f )
					i.e. to set the y axis as forward pass in glm::vec3( 0.0f, 1.0f, 0.0f )
					i.e. to set the z axis as forward pass in glm::vec3( 0.0f, 0.0f, 1.0f )

@notes None

*********************/


glm::vec3 Object::getRotationInWorld( glm::vec3 forward )
{
	glm::vec3 inverseScale;
	glm::mat4 tmpMat;
	glm::mat3 rotMat;

	inverseScale = glm::vec3( 1.0f / scaleFactor.x, 1.0f / scaleFactor.y, 1.0f / scaleFactor.z );

	//remove scaling
	tmpMat = glm::scale( model, inverseScale );

	rotMat = glm::mat3( tmpMat );

	return rotMat * forward;
}

int & Object::LightCode( )
{
	return lightCode;
}

void Object::setParentModel( const glm::mat4 & parent )
{
	parentModel = parent;
}

void Object::useParent( bool use )
{
	applyParent = use;
}

bool Object::setObjectModel( unsigned int oSelect )
{
    if( oSelect >= objModelPtr.size( ) )
    {
        return false;
    }

    objModelSelect = oSelect;

    return true;
}

bool Object::interpolateModels( float interpValue, size_t startModel, size_t endModel )
{
    if( objModelPtr.size( ) <= endModel || objModelPtr.size( ) <= startModel )
    {
        return false;
    }

    if( interpValue >= 0.5f )
    {
        objModelSelect = endModel;
    }
    else if( interpValue < 0.5f )
    {
        objModelSelect = startModel;
    }
    
    return true;
}

void Object::setRender( bool renderMe )
{
    render = renderMe;
}
