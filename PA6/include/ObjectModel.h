#ifndef OBJECTMODEL_H
#define OBJECTMODEL_H

/***************************************

@file ObjectModel.h

@brief A class to hold all of the model's buffer information

@note none

***************************************/

// headers ////////////////////////////////
#include<vector>
#include<string>
#include"graphics_headers.h"

// class definition ///////////////////////

class ObjectModel
{
    public:
        //contructors
        ObjectModel( );
        ObjectModel( const std::string& fileName );
        ObjectModel( const ObjectModel& src );
        
        //destructor
        ~ObjectModel( );

        //const get methods 
        const GLuint& getVB( );
        const GLuint& getIB( unsigned int index );
        const std::vector<Vertex>& getVertices( );
        const std::vector<unsigned int>& getIndices( unsigned int index );

        //access for setting and getting
        GLuint& vertexBuffer( );
        GLuint& indexBuffer( unsigned int index );
        GLuint& Texture( unsigned int index );
        std::vector<Vertex>& vertices( );
        std::vector<unsigned int>& indices( unsigned int index );

        GLint& TextureUniformLocation( );
        

        unsigned int getNumberOfIBs( );

        //file I/O
        bool loadModelFromFile( const std::string& fileName );

        //clear
        void clear( );

    private:
        std::vector<Vertex> Vertices;
        std::vector<std::vector<unsigned int>> Indices;
        GLuint VB;
        std::vector<GLuint> IB;
        std::vector<GLuint> texture;
        GLint textUniLoc;

};

#endif
