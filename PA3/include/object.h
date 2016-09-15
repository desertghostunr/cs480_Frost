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

    void updateRotationRate( float rotFactor );

    void toggleRotationDirection( );

    void toggleRotationPaused( );

    float getRateOfRotation( );

    void updateOrbitRate( float orbitFactor );

    void toggleOrbitDirection( );

    void toggleOrbitPaused( );

    float getRateOfOrbit( );

    void toggleAllPaused( );

    void setOrbitalRadius( float radius );

    glm::vec3 getCurrentPositionOfOrbit( );

    void setOrigin( const glm::vec3 & newOrigin );

  private:
    glm::mat4 model;
    glm::vec3 origin; //the offset for the model's location
    glm::vec3 positionInOribit; //the relative position in the object's orbit
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    GLuint VB;
    GLuint IB;

    float angle;
    float orbitalAngle;

    float orbitalRadius;

    float rotationRate;
    float orbitRate;

    float rotationControlMultiplier;
    float orbitControlMultiplier;

};

#endif /* OBJECT_H */
