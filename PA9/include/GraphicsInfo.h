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

    ObjectInfo( ) :
        modelID( -1 ),
        childID(  ),
        scale( glm::vec3( 1.0f, 1.0f, 1.0f ) ),
        bScale( glm::vec3( 1.0f, 1.0f, 1.0f ) ),
        position( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        rotationAxes( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        rotationAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        name(  )
    { }
    ObjectInfo( const ObjectInfo& objInfo ) :
        modelID( objInfo.modelID ),
        childID( objInfo.childID ),
        scale( objInfo.scale ),
        bScale( objInfo.bScale ),
        position( objInfo.position ),
        rotationAxes( objInfo.rotationAxes ),
        rotationAngles( objInfo.rotationAngles ),
        name( objInfo.name )
    {
        //nothing to do
    }

};

struct SpotLight
{
    glm::vec4 incoming;
    glm::vec4 ambient;
    float coneAngle;

    SpotLight( ):incoming( glm::vec4( 0.0, 0.0, 0.0, 1.0f ) ){ }

    SpotLight( const SpotLight& src) :
        incoming( src.incoming ),
        ambient( src.ambient ),
        coneAngle( src.coneAngle )
    {

    }

    const SpotLight& operator = ( const SpotLight& rh )
    {
        if( this != &rh )
        {
            incoming = rh.incoming;
            ambient = rh.ambient;
            coneAngle = rh.coneAngle;
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
    std::vector<glm::vec4> lights;
    std::vector<glm::vec4> ambient;

    SpotLight spotLight;

    //constructors
    GraphicsInfo( ):spotLight( ) { }

    GraphicsInfo( const GraphicsInfo& src ): 
        shaderVector( src.shaderVector ),
        modelVector( src.modelVector ), 
        objectData( src.objectData ),
        lights( src.lights ),
        ambient( src.ambient ),
        spotLight( src.spotLight ){ }
};

#endif /* GRAPHICSINFO_H */