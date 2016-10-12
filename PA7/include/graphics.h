#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <vector>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"

#include "GraphicsInfo.h"
#include "ObjectTable.h"

class Graphics
{
    public:
        Graphics();
        ~Graphics();
        bool Initialize(int width, int height, 
                                        const GraphicsInfo& progInfo );
        void Update(unsigned int dt);
        void Render();

        bool updateList( unsigned int objectID, unsigned int dt );

        bool updateChildren( unsigned int objectID, unsigned int dt );

        bool updateRotation( unsigned int objectID, float speedOfRotation );
        bool updateOrbit( unsigned int objectID, float speedOfOrbit );

        bool toggleRotationDirection( unsigned int objectID );
        bool toggleRotationPaused( unsigned int objectID );
        
        bool toggleOrbitDirection( unsigned int objectID );
        bool toggleOrbitPaused( unsigned int objectID );

        bool toggleAllPaused( unsigned int objectID );
        void toggleAllObjectsPaused( );

    private:
        std::string ErrorString(GLenum error);

        Camera *m_camera;
        Shader *m_shader;

        GLint m_projectionMatrix;
        GLint m_viewMatrix;
        GLint m_modelMatrix;

        ObjectTable objectRegistry; //holds the scene's objects
};

#endif /* GRAPHICS_H */
