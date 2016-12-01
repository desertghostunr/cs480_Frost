/***************************************

@file GraphicsInfo.h 

@brief A struct to hold info to pass to the graphics class from main

@note none

***************************************/

#ifndef GRAPHICSINFO_H
#define GRAPHICSINFO_H

#include <vector>
#include <string>
#include "graphics_headers.h"

//global constants
const std::string OBJECT_TYPE = "Object";
const std::string P_OBJECT_TYPE = "PObject";
const std::string P_C_OBJECT_TYPE = "PCObject";

struct ObjectInfo
{
    unsigned int modelID; //the ID of the model in the model vector

    std::vector<unsigned int> childID; //children IDs that correspond to others in the table

    glm::vec3 scale; //scale of the object relative to earth
    glm::vec3 bScale;
    glm::vec3 position;
    glm::vec3 rotationAxes;
    glm::vec3 rotationAngles;

    std::string name;
	std::string type;

    ObjectInfo( ) :
        modelID( -1 ),
        childID(  ),
        scale( glm::vec3( 1.0f, 1.0f, 1.0f ) ),
        bScale( glm::vec3( 1.0f, 1.0f, 1.0f ) ),
        position( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        rotationAxes( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        rotationAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        name( ),
		type( ){ }

    ObjectInfo( const ObjectInfo& objInfo ) :
        modelID( objInfo.modelID ),
        childID( objInfo.childID ),
        scale( objInfo.scale ),
        bScale( objInfo.bScale ),
        position( objInfo.position ),
        rotationAxes( objInfo.rotationAxes ),
        rotationAngles( objInfo.rotationAngles ),
        name( objInfo.name ),
		type( objInfo.type )
    {
        //nothing to do
    }

};

//lighting info

struct Light
{
    glm::vec4 incoming;
    glm::vec4 ambient;

    GLint incomingLoc;
    GLint ambientLoc;

    Light( ):incoming( glm::vec4( 0.0, 0.0, 0.0, 1.0f ) ) { }

    Light( const Light& src ) :
        incoming( src.incoming ),
        ambient( src.ambient ),
        incomingLoc( src.incomingLoc ),
        ambientLoc( src.ambientLoc )
    {

    }

    const Light& operator = ( const Light& rh )
    {
        if( this != &rh )
        {
            incoming = rh.incoming;
            ambient = rh.ambient;

            incomingLoc = rh.incomingLoc;
            ambientLoc = rh.ambientLoc;
        }

        return *this;
    }
};

struct SpotLight
{
    glm::vec4 incoming;
    glm::vec4 ambient;
    float coneAngle;
    float cosine;

    float spotHeight;

    std::string objectToFollow;

    unsigned int oTFIndex;

    GLint incomingLoc;
    GLint ambientLoc;
    GLint cosineLoc;
    GLint followLoc;

    SpotLight( ):incoming( glm::vec4( 0.0, 0.0, 0.0, 1.0f ) ){ }

    SpotLight( const SpotLight& src) :
        incoming( src.incoming ),
        ambient( src.ambient ),
        coneAngle( src.coneAngle ),
        cosine( src.cosine ),
        spotHeight( src.spotHeight ),
        objectToFollow( src.objectToFollow ),
        oTFIndex( src.oTFIndex ),
        incomingLoc( src.incomingLoc ),
        ambientLoc( src.ambientLoc ),
        cosineLoc( src.cosineLoc ), 
        followLoc( src.followLoc )
    {

    }

    const SpotLight& operator = ( const SpotLight& rh )
    {
        if( this != &rh )
        {
            incoming = rh.incoming;
            ambient = rh.ambient;
            coneAngle = rh.coneAngle;
            cosine = rh.cosine;
            spotHeight = rh.spotHeight;
            objectToFollow = rh.objectToFollow;
            oTFIndex = rh.oTFIndex;

            incomingLoc = rh.incomingLoc;
            ambientLoc = rh.ambientLoc;
            cosineLoc = rh.cosineLoc;
            followLoc = rh.followLoc;
        }

        return *this;
    }
};

struct GraphicsInfo
{
    //members
    std::vector<std::vector<std::pair<GLenum, std::string>>> shaderVector;
    std::vector<std::string> modelVector;
    std::vector<ObjectInfo> objectData;
    std::vector<Light> lights;

    std::vector<SpotLight> spotLight;
	glm::vec2 windowSize;
	std::string windowName;
	glm::vec3 windDirection;

    //constructors
	GraphicsInfo( ) : spotLight( ) 
	{
		windDirection = glm::vec3( 1.0f, 1.0f, 1.0f ); 
	}

    GraphicsInfo( const GraphicsInfo& src ): 
        shaderVector( src.shaderVector ),
        modelVector( src.modelVector ), 
        objectData( src.objectData ),
        lights( src.lights ),
        spotLight( src.spotLight ),
		windowSize( src.windowSize ),
		windowName( src.windowName ), 
		windDirection( src.windDirection ){ }
};
#endif /* GRAPHICSINFO_H */
