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

class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height, 
                    const GraphicsInfo& progInfo );
    void Update(unsigned int dt);
    void Render();

    bool updateRotation( int objectID, float speedOfRotation );
    bool updateOrbit( int objectID, float speedOfOrbit );

    bool toggleRotationDirection( int objectID );
    bool toggleRotationPaused( int objectID );
    
    bool toggleOrbitDirection( int objectID );
    bool toggleOrbitPaused( int objectID );

    bool toggleAllPaused( int objectID );
    void toggleAllObjectsPaused( );

  private:
    std::string ErrorString(GLenum error);

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;

    std::vector<Object> objectVector; //this vector holds the scene's objects
};

#endif /* GRAPHICS_H */
