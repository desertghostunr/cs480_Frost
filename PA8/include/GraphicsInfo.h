/***************************************

@file GraphicsInfo.h 

@brief A struct to hold info to pass to the graphics class from main

@note none

***************************************/

#ifndef GRAPHICSINFO_H
#define GRAPHICSINFO_H

#include <vector>
#include "graphics_headers.h"

struct ObjectInfo
{
    unsigned int modelID; //the ID of the model in the model vector

    std::vector<unsigned int> childID; //children IDs that correspond to others in the table

    glm::vec3 scale; //scale of the object relative to earth
    glm::vec3 position;
    glm::vec3 rotationAxes;
    glm::vec3 rotationAngles;

    ObjectInfo( ) :
        modelID( -1 ),
        childID(  ),
        scale( glm::vec3( 1.0f, 1.0f, 1.0f ) ),
        position( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        rotationAxes( glm::vec3( 0.0f, 0.0f, 0.0f ) ),
        rotationAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) )
    { }
    ObjectInfo( const ObjectInfo& objInfo ) :
        modelID( objInfo.modelID ),
        childID( objInfo.childID ),
        scale( objInfo.scale ),
        position( objInfo.position ),
        rotationAxes( objInfo.rotationAxes ),
        rotationAngles( objInfo.rotationAngles )


    {
        //nothing to do
    }

};


struct GraphicsInfo
{
    //members
    std::vector<std::pair<GLenum, std::string>> shaderVector;
    std::vector<std::string> modelVector;
    std::vector<ObjectInfo> objectData;

    //constructors
    GraphicsInfo( ) { }

    GraphicsInfo( const GraphicsInfo& src ): 
        shaderVector( src.shaderVector ),
        modelVector( src.modelVector ), 
        objectData( src.objectData )
    {

    }
};

#endif /* GRAPHICSINFO_H */