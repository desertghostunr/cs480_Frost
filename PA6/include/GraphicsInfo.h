/***************************************

@file GraphicsInfo.h 

@brief A struct to hold info to pass to the graphics class from main

@note none

***************************************/

#ifndef GRAPHICSINFO_H
#define GRAPHICSINFO_H

#include <vector>


struct GraphicsInfo
{
    //members
    std::vector<std::pair<GLenum, std::string>> shaderVector;
    std::vector<std::string> modelVector;

    //constructors
    GraphicsInfo( ) { }

    GraphicsInfo( const GraphicsInfo& src ): shaderVector( src.shaderVector ),
                  modelVector( src.modelVector ) { }
};

#endif /* GRAPHICSINFO_H */