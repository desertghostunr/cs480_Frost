/*************************************

@file HeightMap.h

@brief Generates a heightmap for the ocean scene

@author Andrew Frost (12/03/2016)

@Note none

*************************************/


// Pre-compiler directives //////////////////
#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H


//Header files /////////////////////////////
#include "graphics_headers.h"
#include "OpenSimplexNoise.h"

//Class definition ////////////////////////

class HeightMap
{
	public:
		HeightMap( long seed = 0 );
		HeightMap( const HeightMap& src );

		~HeightMap( );

		GLint& HeightMapUniform( );
		GLuint HeightMapTexture( );

		bool generateHeightMap( int xSize, int ySize );

	private:
		GLint uniformLoc;
		GLuint texture;
		OpenSimplexNoise noiseGen;

};



#endif // !HEIGHTMAP_H

