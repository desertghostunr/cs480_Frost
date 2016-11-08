/***************************************

@file ObjectModel.h

@brief A class to hold all of the model's buffer information

@note none

***************************************/

//header files
#include "ObjectModel.h"
#include <algorithm>

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
ObjectModel::ObjectModel( ): reference( 0 ), Vertices( ), Indices( ) 
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
ObjectModel::ObjectModel( const std::string & fileName ): reference( 0 ), 
                                                          Vertices( ), Indices( )
                                                          
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
):  diffuse(src.diffuse), specular(src.specular ), shininess( src.shininess),
    reference( 0 ), Vertices( src.Vertices ), Indices( src.Indices ), 
    VB( src.VB ), IB( src.IB ),  texture( src.texture ), textUniLoc( src.textUniLoc )
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

@param in index: which IB to return

@notes None

***************************************/
const GLuint & ObjectModel::getIB( unsigned int index )
{
    return IB[index ];
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

@param in index: which IB to return

@notes None

***************************************/
const std::vector<unsigned int>& ObjectModel::getIndices( unsigned int index )
{
    return Indices[ index ];
}

// INCREMENT REFERENCE ///////////////////
/***************************************

@brief incrementReference

@details increments the reference counter

@param None

@notes None

***************************************/
void ObjectModel::incrementReference( )
{
    reference++;
}

// DECREMENT REFERENCE ///////////////////
/***************************************

@brief decrementReference

@details decrements the reference counter

@param None

@notes None

***************************************/
void ObjectModel::decrementReference( )
{
    if( reference == 0 )
    {
        return;
    }

    reference--;
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

@param in index: which IB to return

@notes None

***************************************/
GLuint & ObjectModel::indexBuffer( unsigned int index )
{
    return IB[index ];
}

// TEXTURE ///////////////////
/***************************************

@brief Texture

@details returns a reference to the Texture

@param in index: which texture to return

@notes None

***************************************/
GLuint & ObjectModel::Texture( unsigned int index )
{
    return texture[ index ];
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

@param in index: which IB to return

@notes None

***************************************/
std::vector<unsigned int>& ObjectModel::indices( unsigned int index )
{
    return Indices[ index ];
}

// TEXTURE UNIFORM LOCATION ///////////////////
/***************************************

@brief TextureUniformLocation

@details returns the location of the Texture uniform in the shader program

@param None

@notes None

***************************************/
GLint & ObjectModel::TextureUniformLocation( )
{
    return textUniLoc;
}


// GET NUMBER OF IBS///////////////////
/***************************************

@brief getNumberOfIBs

@details returns the size of the IBO

@param None

@notes None

***************************************/
unsigned int ObjectModel::getNumberOfIBs( )
{
    return IB.size( );
}


// GET NUMBER OF TEXTURES ///////////////////
/***************************************

@brief getNumberOfTextures

@details returns the size of the Texture vector

@param None

@notes None

***************************************/
unsigned int ObjectModel::getNumberOfTextures( )
{
    return texture.size( );
}

// GET REFERENCE COUNT ///////////////////
/***************************************

@brief getReferenceCount

@details returns the number of references using this ObjecModel

@param None

@notes None

***************************************/
unsigned int ObjectModel::getReferenceCount( )
{
    return reference;
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
    aiVector3D uv, normals;

    aiString textFPath;

    aiColor4D mColor;

    glm::vec4 color;

    //open the model file 
    const aiScene* scene = importer.ReadFile( fileName.c_str( ), //file to open
                                              aiProcess_Triangulate ); //triangulate faces

    //vertex to temporarily store data
    Vertex tmpVert( glm::vec3( 1.0f, 1.0f, 1.0f ), //vertex
                    glm::vec2( 0.0f, 0.0f ), //uv
                    glm::vec3( 1.0f, 1.0f, 1.0f ) ); //normals


    //indexing
    unsigned int mIndex, fIndex, vIndex, iIndex, tIndex, offset;

    //image container using opencv
    cv::Mat tmpImg;
    std::vector<cv::Mat> textureImg; //image matrix
    std::vector<std::string> textureFileNames;

    double addedValue = 0.0, multipliedValue= 1.0;

    std::string pathString = fileName.substr( 0, fileName.find_last_of( "\\/" ) + 1 );

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
        Indices.push_back( std::vector<unsigned int>( ) );

        mtlPtr = scene->mMaterials[ scene->mMeshes[ mIndex ]->mMaterialIndex ];

        if( mtlPtr != NULL )
        {
            for( tIndex = 0;
                 tIndex < mtlPtr->GetTextureCount( aiTextureType_DIFFUSE );
                 tIndex++ )
            {
                mtlPtr->GetTexture( aiTextureType_DIFFUSE, tIndex, &textFPath );
                
                textureFileNames.push_back( textFPath.C_Str( ) );                
            }

            if( mtlPtr->GetTextureCount( aiTextureType_DIFFUSE ) == 0 )
            {
                if( AI_SUCCESS == aiGetMaterialColor( mtlPtr, AI_MATKEY_COLOR_DIFFUSE, &mColor ) )
                {
                    color.r = mColor.r;
                    color.g = mColor.g;
                    color.b = mColor.b;
                    color.a = mColor.a;

                    textureFileNames.push_back( "No Texture" );
                }
            }

            if( AI_SUCCESS == aiGetMaterialColor( mtlPtr, AI_MATKEY_COLOR_DIFFUSE, &mColor ) )
            {
                diffuse.r = mColor.r;
                diffuse.g = mColor.g;
                diffuse.b = mColor.b;
                diffuse.a = mColor.a;
            }

            if( AI_SUCCESS == aiGetMaterialColor( mtlPtr, AI_MATKEY_COLOR_SPECULAR, &mColor ) )
            {
                specular.r = mColor.r;
                specular.g = mColor.g;
                specular.b = mColor.b;
                specular.a = mColor.a;
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

                if( scene->mMeshes[ mIndex ]->HasTextureCoords( 0 ) )
                {
                    uv = scene->mMeshes[ mIndex ]->mTextureCoords[ 0 ][ vIndex ];

                    tmpVert.uv.x = uv.x;
                    tmpVert.uv.y = 0.0f - uv.y;
                }                
                else
                {
                    tmpVert.uv.x = 0.0f;
                    tmpVert.uv.y = 0.0f;
                }

                if( scene->mMeshes[ mIndex ]->HasNormals( ) )
                {
                    normals = scene->mMeshes[ mIndex ]->mNormals[ vIndex ];

                    tmpVert.normal.x = normals.x;
                    tmpVert.normal.y = normals.y;
                    tmpVert.normal.z = normals.z;
                }
                else
                {
                    tmpVert.normal.x = 0.0f;
                    tmpVert.normal.y = 0.0f;
                    tmpVert.normal.z = 0.0f;
                }


                //push back vertices
                Vertices.push_back( tmpVert );

                //push back indices
                Indices[ mIndex ].push_back( offset + //add offset
                                   scene->mMeshes[ mIndex ]->
                                          mFaces[ fIndex ].mIndices[ iIndex ] );

            }
        }
    }

    //set shininess
    shininess = 10.0f;

    //load texture as it is; note that 
    //this function can also be set to force the pixels to processed
    //as 8-bit, 16-bit, 32-bit, or grayscale.
    //The color channels are store contiguously for each pixel as BGR
    //the Mat::convertTo function can also be used to convert the image
    //to 8-bit, 16-bit, 32-bit, single precision floating-point,
    //or double precision floating-point.
    //the 8-bit, 16-bit, 32-bit types can be signed or unsigned
    for( tIndex = 0; tIndex < textureFileNames.size( ); tIndex++ )
    {
        if( textureFileNames[ tIndex ] == "No Texture" )
        {            
            tmpImg = cv::Mat( 256, 256, CV_8UC4 );
            tmpImg.setTo( cv::Scalar( 255 * color.b, 255 * color.g, 255 * color.r, 255 * color.a ) );
        }
        else
        {
            tmpImg = cv::imread( pathString + textureFileNames[ tIndex ],
                                 CV_LOAD_IMAGE_UNCHANGED );
        }
        

        if( tmpImg.type( ) != CV_8UC4 )
        {
            switch( tmpImg.channels( ) )
            {
                case 1:
                {
                    cv::cvtColor( tmpImg.clone( ), tmpImg, cv::COLOR_GRAY2BGRA );
                    break;
                }
                case 3:
                {
                    cv::cvtColor( tmpImg.clone( ), tmpImg, cv::COLOR_BGR2BGRA );
                    break;
                }
                case 4:
                {
                    //nothing to do
                    break;
                }
                default:
                {
                    std::cout << "Invalid Texture: " << pathString + textureFileNames[ tIndex ] << std::endl;
                    std::cout << "This image has an invalid number of channels: " << tmpImg.channels( ) << std::endl;
                    return false;
                }
            }

            switch( tmpImg.depth( ) )
            {
                case CV_8U:
                {
                    addedValue = 0.0;
                    multipliedValue = 1.0;
                    break;
                }
                case CV_8S:
                {
                    addedValue = 128.0;
                    multipliedValue = 1.0;
                    break;
                }
                case CV_16U:
                {
                    addedValue = 0.0;
                    multipliedValue = 255.0 / 65535.0;
                    break;
                }
                case CV_16S:
                {
                    addedValue = 128.0;
                    multipliedValue = 255.0 / 65535.0;
                    break;
                }
                case CV_32F:
                {
                    addedValue = 0.0;
                    multipliedValue = 255.0;
                    break;
                }
                default:
                {
                    std::cout << "Invalid Texture: " << pathString + textureFileNames[ tIndex ] << std::endl;
                    std::cout << "This image has an invalid channel depth!" <<std::endl;
                    return false;
                }

                if( multipliedValue == 1.0 && addedValue == 0.0 )
                {
                    //don't convert
                }
                else
                {
                    tmpImg.convertTo( tmpImg, CV_8U, multipliedValue, addedValue );
                }
            }
        }

        textureImg.push_back( tmpImg.clone( ) );

        tmpImg.release( );
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

    IB.resize( Indices.size( ) );   

    for( iIndex = 0; iIndex < IB.size( ); iIndex++ )
    {
        glGenBuffers( 1, &IB[ iIndex ] );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IB[ iIndex ] );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, //buffer type
                      sizeof( unsigned int ) * Indices[ iIndex ].size( ), //size
                      &Indices[iIndex][ 0 ], //data
                      GL_STATIC_DRAW ); //draw mode
    }

    texture.resize( textureImg.size( ) );

    for( tIndex = 0; tIndex < texture.size( ); tIndex++ )
    {
        glGenTextures( 1, &texture[ tIndex ] );        

        glBindTexture( GL_TEXTURE_2D, texture[ tIndex ] );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 
                      textureImg[ tIndex ].size( ).width, 
                      textureImg[ tIndex ].size( ).height, 0, 
                      GL_BGRA, GL_UNSIGNED_BYTE, textureImg[ tIndex ].data );        

        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        textureImg[ tIndex ].release( );
    }

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

// ASSIGNMENT OPERATOR ///////////////////
/***************************************

@brief operator=

@details assigns another ModelObject to this

@param in: rhObjectModel: the object we are copying

@notes None

***************************************/
const ObjectModel & ObjectModel::operator=( const ObjectModel & rhObjModel )
{
    if( this != &rhObjModel )
    {
        specular = rhObjModel.specular;
        diffuse = rhObjModel.diffuse;
        shininess = rhObjModel.shininess;
        Vertices = rhObjModel.Vertices;
        Indices = rhObjModel.Indices;
        VB = rhObjModel.VB;
        IB = rhObjModel.IB;
        texture = rhObjModel.texture;
        textUniLoc = rhObjModel.textUniLoc;
    }

    return *this;
}

glm::vec4 & ObjectModel::getDiffuse( )
{
    return diffuse;
}

glm::vec4 & ObjectModel::getSpecular( )
{
    return specular;
}

float & ObjectModel::getShininess( )
{
    return shininess;
}
