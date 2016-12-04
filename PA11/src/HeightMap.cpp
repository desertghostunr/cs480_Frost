#include "HeightMap.h"

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>


HeightMap::HeightMap( long seed ): uniformLoc( 0 ), texture( 0 ), noiseGen( seed )
{
	//done in initializer
}

HeightMap::HeightMap( const HeightMap & src ): 
	uniformLoc( src.uniformLoc ), 
	texture( src.texture ), 
	noiseGen( src.noiseGen )
{
	//done in initializer
}

HeightMap::~HeightMap( )
{
	//done by class members
}

GLint& HeightMap::HeightMapUniform( )
{
	return uniformLoc;
}

GLuint HeightMap::HeightMapTexture( )
{
	return texture;
}

bool HeightMap::generateHeightMap( int xSize, int ySize )
{
	cv::Mat map;

	int row, col;

	if( xSize <= 0 || ySize <= 0 )
	{
		return false;
	}

	//allocate buffer to generate map in
	map = cv::Mat( xSize, ySize, CV_32FC1 );

	for( row = 0; row < map.size( ).width; row++ )
	{
		for( col = 0; col < map.size( ).height; col++ )
		{
			map.at<float>( cv::Point( row, col ) ) = noiseGen.getNoise( row, col );
		}
	}

	glGenTextures( 1, &texture );

	glBindTexture( GL_TEXTURE_2D, texture );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_R32F,
				  map.size( ).width,
				  map.size( ).height, 0,
				  GL_RED, GL_FLOAT, map.data );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	map.release( );

	return true;
}
