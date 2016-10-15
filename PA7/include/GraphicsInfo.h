/***************************************

@file GraphicsInfo.h 

@brief A struct to hold info to pass to the graphics class from main

@note none

***************************************/

#ifndef GRAPHICSINFO_H
#define GRAPHICSINFO_H

#include <vector>
#include "graphics_headers.h"

struct PlanetInfo
{
    unsigned int objectID; //the ID of the Object
    std::string modelPath; //model path

    std::vector<unsigned int> childID; //children IDs that correspond to others in the table

    float scale; //scale of the object relative to earth
    float tilt; //tilt of the planet
    float orbitTilt; //tilt of the orbit
    float rotRate; //the rate of rotations
    glm::vec3 orbitRad; //radius of orbit
    float orbitRate; //rate of orbit
    glm::vec3 locLocal; //the location of the object in local space

    PlanetInfo( ){ }
    PlanetInfo( const PlanetInfo& planInfo ) :
        objectID( planInfo.objectID ),
        modelPath( planInfo.modelPath ),
        childID( planInfo.childID ),
        scale( planInfo.scale ),
        tilt( planInfo.tilt ),
        orbitTilt( planInfo.orbitTilt ),
        rotRate( planInfo.rotRate ),
        orbitRad( planInfo.orbitRad ),
        orbitRate( planInfo.orbitRate ),
        locLocal( planInfo.locLocal )

    {
        //nothing to do
    }

};


struct GraphicsInfo
{
    //members
    std::vector<std::pair<GLenum, std::string>> shaderVector;
    std::vector<std::string> modelVector;
    std::vector<PlanetInfo> planetData;
    

    //constructors
    GraphicsInfo( ) { }

    GraphicsInfo( const GraphicsInfo& src ): 
        shaderVector( src.shaderVector ),
        modelVector( src.modelVector ), 
        planetData( src.planetData ) 
    {

    }
};

#endif /* GRAPHICSINFO_H */