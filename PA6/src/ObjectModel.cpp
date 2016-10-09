/***************************************

@file ObjectModel.h

@brief A class to hold all of the model's buffer information

@note none

***************************************/

//header files
#include "ObjectModel.h"

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>

//opencv
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>


// DEFAULT CONSTRUCTOR //////////////////
/***************************************

@brief ObjectModel

@details creates an empty ObjectModel

@param None

@notes None

***************************************/
ObjectModel::ObjectModel( ): Vertices( ), Indices( )
{
    //nothing to do
}

// PARAMETERIZED CONSTRUCTOR //////////////////
/***************************************

@brief ObjectModel

@details creates an ObjectModel and fills it with data from file

@param in: fileName: the name of the file

@notes None

***************************************/
ObjectModel::ObjectModel( const std::string & fileName ): Vertices( ), Indices( )
{
    loadModelFromFile( fileName );
}

// COPY CONSTRUCTOR //////////////////
/***************************************

@brief ObjectModel

@details creates an ObjectModel from another ObjectModel

@param in: src: the ObjectModel to copy

@notes None

***************************************/
ObjectModel::ObjectModel
( 
    const ObjectModel & src //the ObjectModel to copy
): VB( src.VB ), IB( src.IB ), Vertices( src.Vertices ), Indices( src.Indices )
{
    //nothing to do
}


// DESTRUCTOR //////////////////
/***************************************

@brief ~ObjectModel

@details deallocates the object

@param None

@notes None

***************************************/
ObjectModel::~ObjectModel( )
{
    Vertices.clear( );
    Indices.clear( );
}



// GET VB //////////////////
/***************************************

@brief getVB

@details returns a constant reference to the VB

@param None

@notes None

***************************************/
const GLuint & ObjectModel::getVB( )
{
    return VB;
}

// GET IB //////////////////
/***************************************

@brief getIB

@details returns a constant reference to the IB

@param None

@notes None

***************************************/
const GLuint & ObjectModel::getIB( )
{
    return IB;
}


// GET VERTICES //////////////////
/***************************************

@brief getVERTICES

@details returns a constant reference to the Vertices

@param None

@notes None

***************************************/
const std::vector<Vertex>& ObjectModel::getVertices( )
{
    return Vertices;
}

// GET INDICES ///////////////////
/***************************************

@brief getIndices

@details returns a constant reference to the Indices

@param None

@notes None

***************************************/
const std::vector<unsigned int>& ObjectModel::getIndices( )
{
    return Indices;
}


// VERTEX BUFFER ///////////////////
/***************************************

@brief vertexBuffer

@details returns a reference to the VB

@param None

@notes None

***************************************/
GLuint & ObjectModel::vertexBuffer( )
{
    return VB;
}


// INDEX BUFFER ///////////////////
/***************************************

@brief indexBuffer

@details returns a reference to the IB

@param None

@notes None

***************************************/
GLuint & ObjectModel::indexBuffer( )
{
    return IB;
}

// VERTICES ///////////////////
/***************************************

@brief vertices

@details returns a reference to the Vertices

@param None

@notes None

***************************************/
std::vector<Vertex>& ObjectModel::vertices( )
{
    return Vertices;
}


// INDICES ///////////////////
/***************************************

@brief indices

@details returns a reference to the IB

@param None

@notes None

***************************************/
std::vector<unsigned int>& ObjectModel::indices( )
{
    return Indices;
}


// LOAD MODEL FROM FILE //////////////////
/***************************************

@brief loadModelFromFile

@details loads a model from file

@param in: fileName: the file name of the object we are loading.

@notes File must have triangular faces

***************************************/
bool ObjectModel::loadModelFromFile( const std::string& fileName )
{
    //assimp
    Assimp::Importer importer;
    aiMaterial* mtlPtr = NULL;
    aiColor4D mColor;

    //open the model file 
    const aiScene* scene = importer.ReadFile( fileName.c_str( ), //file to open
                                              aiProcess_Triangulate ); //triangulate faces

    //vertex to temporarily store data
    Vertex tmpVert( glm::vec3( 1.0f, 1.0f, 1.0f ), //vertex
                    glm::vec3( 1.0f, 1.0f, 1.0f ) ); //color


    //indexing
    unsigned int mIndex, fIndex, vIndex, iIndex, tIndex, offset;

    //image container using opencv
    std::vector<cv::Mat> textureImg; //image matrix
    std::vector<std::string> textureFileNames;

    if( scene == NULL )
    {
        std::cout << "Failed to load " << fileName << std::endl;
        return false;
    }

    //clear old vertices content
    Vertices.clear( );
    Indices.clear( );

    //load vertices and faces
    for( mIndex = 0; mIndex < scene->mNumMeshes; mIndex++ )
    {
        mtlPtr = scene->mMaterials[ scene->mMeshes[ mIndex ]->mMaterialIndex ];

        if( mtlPtr != NULL )
        {
            if( AI_SUCCESS //flag for the success of an ai return
                == aiGetMaterialColor( mtlPtr, //pointer to material object
                                       AI_MATKEY_COLOR_DIFFUSE, //the color to grab
                                       &mColor ) ) //the color object
            {
                //assign colors for this mesh
                tmpVert.color.r = mColor.r;
                tmpVert.color.g = mColor.g;
                tmpVert.color.b = mColor.b;
            }
        }

        //create offset for the concatenation of meshes into one VBO and IBO
        offset = Vertices.size( );

        for( fIndex = 0; fIndex < scene->mMeshes[ mIndex ]->mNumFaces; fIndex++ )
        {
            for( iIndex = 0;
                 iIndex < scene->mMeshes[ mIndex ]->mFaces[ fIndex ].mNumIndices;
                 iIndex++ )
            {
                //get index to access vertices at
                vIndex = scene->mMeshes[ mIndex ]->mFaces[ fIndex ].mIndices[ iIndex ];

                //assign vertices
                tmpVert.vertex.x = scene->mMeshes[ mIndex ]->mVertices[ vIndex ].x;
                tmpVert.vertex.y = scene->mMeshes[ mIndex ]->mVertices[ vIndex ].y;
                tmpVert.vertex.z = scene->mMeshes[ mIndex ]->mVertices[ vIndex ].z;

                //push back vertices
                Vertices.push_back( tmpVert );

                //push back indices
                Indices.push_back( offset + //add offset
                                   scene->mMeshes[ mIndex ]->
                                          mFaces[ fIndex ].mIndices[ iIndex ] );

            }
        }
    }

    //load texture without altering the contents of the data; note that 
    //this function can also be set to force the pixels to processed
    //as 8-bit, 16-bit, 32-bit, or grayscale.
    //The color channels are store contiguously for each pixel as BGR
    //the Mat::convertTo function can also be used to convert the image
    //to 8-bit, 16-bit, 32-bit, single precision floating-point,
    //or double precision floating-point.
    //the 8-bit, 16-bit, 32-bit types can be signed or unsigned
    for( tIndex = 0; tIndex < textureFileNames.size( ); tIndex++ )
    {
        textureImg.push_back( cv::imread( textureFileNames[ tIndex ],
                                          CV_LOAD_IMAGE_UNCHANGED ).clone( ) );
    }


    //in the case that the loading of the texture image failed
    if( textureImg.empty( ) )
    {
        std::cout << "Error opening texture file(s)." << std::endl;
    }

    glGenBuffers( 1, &VB );
    glBindBuffer( GL_ARRAY_BUFFER, VB );
    glBufferData( GL_ARRAY_BUFFER, //buffer type
                  sizeof( Vertex ) * Vertices.size( ), //size
                  &Vertices[ 0 ], //data
                  GL_STATIC_DRAW ); //draw mode

    glGenBuffers( 1, &IB );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, //buffer type
                  sizeof( unsigned int ) * Indices.size( ), //size
                  &Indices[ 0 ], //data
                  GL_STATIC_DRAW ); //draw mode

    return true;
}


// CLEAR ///////////////////
/***************************************

@brief clear

@details clears the Vertices and Indices

@param None

@notes None

***************************************/
void ObjectModel::clear( )
{
    Vertices.clear( );
    Indices.clear( );
}
