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
    unsigned int objectID;
    std::string modelPath;

    std::vector<unsigned int> childID;

    glm::vec3 scale;
    glm::vec3 tilt;
    float rotRate;
    glm::vec3 orbitRad;
    glm::vec3 locLocal;
    glm::ce3 tilt;

    PlanetInfo( ){ }
    PlanetInfo( const PlanetInfo& planInfo ): 
        objectID( planInfo.objectID ), 
        modelPath( planInfo.modelPath ),
        childID( planInfo.childID ),
        scale( planInfo.scale ),
        tilt( planInfo.tilt ),
        rotRate( planInfo.rotRate ),
        orbitRad( planInfo.orbitRad ),
        locLocal( planInfo.locLocal ),
        tilt( planInfo.tilt )

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