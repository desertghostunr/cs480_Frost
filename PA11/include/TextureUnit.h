#ifndef TEXTUREUNIT_H
#define TEXTUREUNIT_H

#include"graphics_headers.h"
#include<string>

struct TextureUnit
{
    std::string name;
    GLuint texture;

    TextureUnit( ): name( ), texture( -1 )
    {

    }

    TextureUnit( const TextureUnit& src ): name( src.name ), texture( src.texture )
    {

    }

    const TextureUnit& operator = ( const TextureUnit& src )
    {
        if( this != &src )
        {
            name = src.name;
            texture = src.texture;
        }

        return *this;
    }

};

#endif // !TEXTUREUNIT_H

