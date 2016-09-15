#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <vector>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "object.h"

class Graphics
{
  public:
    Graphics();
    ~Graphics();
    bool Initialize(int width, int height, 
                    const std::vector<pair<GLenum, std::string>>& shaderInfo);
    void Update(unsigned int dt);
    void Render();

    void updateRotation( float speedOfRotation );
    void updateOrbit( float speedOfOrbit );

    void toggleRotationDirection( );
    void toggleRotationPaused( );
    
    void toggleOrbitDirection( );
    void toggleOrbitPaused( );

    void toggleAllPaused( );

  private:
    std::string ErrorString(GLenum error);

    Camera *m_camera;
    Shader *m_shader;

    GLint m_projectionMatrix;
    GLint m_viewMatrix;
    GLint m_modelMatrix;

    Object *m_cube;
    Object *m_moon;
};

#endif /* GRAPHICS_H */
