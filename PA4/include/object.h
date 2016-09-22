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

    void UpdateChildren( unsigned int dt, 
                         std::vector<Object>& objectTable, 
                         bool setParentAsOrigin = true );

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

    void setOrigin( const glm::mat4 & newOrigin );

    void addChild( int childsWorldID );

    int getChildsWorldID( int childsLocalID );

    int getNumberOfChildren( );

    void setChildStatusFlag( bool flag );

    bool isChild( );

    void setScale( const glm::vec3& scale );

    glm::vec3 getScale( );

    void createOrbitInTranslationVector( const glm::vec3& angle, 
                                         const glm::vec3& radius, 
                                         const glm::vec3& localOffset );

    void setTranslationVector( const glm::vec3& transVec );

    void setRotationVector( const glm::vec3 rotVec );

    void commitTranslation( );

    void commitRotation( );

    void commitScale( );

    void commitOrigin( );

    void incrementAngle( unsigned int dt );

    float getAngle( );

    void setAngle( float newAngle );

    void incrementOrbitAngle( unsigned int dt );

    float getOrbitAngle( );

    void setOrbitAngle( float newAngle );

    bool isPaused( );

    void createSatelliteTransform( );

  private:
    glm::mat4 model;
    glm::mat4 origin; //the offset for the model's location
    glm::vec3 scaleFactor; //scale factor
    glm::vec3 translationVector; //translation in cartesian space
    glm::vec3 rotationVector; //rotation vector in cartesian space
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

    //vector of transforms to apply to the object 
    std::vector<glm::mat4> transformVector;
    //children vector containg the worldID's of the objects children
    std::vector<int> childrenVector;

    bool isChildFlag;
};

#endif /* OBJECT_H */
