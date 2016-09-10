#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include "graphics_headers.h"

class Object
{
  public:
    Object();
    ~Object();
    void Update(unsigned int dt);
    void Render();

    glm::mat4 GetModel();

    void updateRotation( float rotFactor );

    void toggleRotationDirection( );

    void toggleRotationPaused( );

    float getRateOfRotation( );

    void updateOrbit( float orbitFactor );

    void toggleOrbitDirection( );

    void toggleOrbitPaused( );

    float getRateOfOrbit( );

    void toggleAllPaused( );

  private:
    glm::mat4 model;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    float angle;
    float orbitalAngle;

    float rotationRate;
    float orbitRate;

    float rotationControlMultiplier;
    float orbitControlMultiplier;

};

#endif /* OBJECT_H */
