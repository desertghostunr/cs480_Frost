#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include "graphics_headers.h"
#include "ObjectModel.h"

class Object
{
    public:
        Object();
        ~Object();

        void Update(unsigned int dt);

        void Render();

        glm::mat4 GetModel();

        ObjectModel& getObjectModel( );

        bool hasObjectModel( );

        bool Initialize( ObjectModel * const srcPtr );

        void updateRotationRate( float rotFactor );

        void toggleRotationDirection( );

        void toggleRotationPaused( );

        float getRateOfRotation( );

        void updateOrbitRate( float orbitFactor );

        void toggleOrbitDirection( );

        void toggleOrbitPaused( );

        float getRateOfOrbit( );

        void toggleAllPaused( );

        void setOrbitalRadius( glm::vec2 radius );

        bool setOrigin( const glm::mat4 & newOrigin );

        bool addChild( unsigned int childsWorldID );

        bool addParent( unsigned int parentsWorldID );

        unsigned int getChildsWorldID( unsigned int childsLocalID );
        
        unsigned int getParentsWorldID( );

        bool setObjectsID( unsigned int id );

        unsigned int getObjectsID( );

        unsigned int getNumberOfChildren( );

        bool isChild( );

        void setScale( const glm::vec3& scale );

        glm::vec3 getScale( );        

        void setTranslationVector( const glm::vec3& transVec );

        void setRotationVector( const glm::vec3 rotVec );

        void setTiltAngle( float tilt );

        void commitTranslation( );

        void commitRotation( );

        void commitTilt( );

        void commitScale( );

        void commitOriginScale( );

        void commitOriginTranslation( );

        void commitOriginRotation( );

        void commitOrbitalTranslation( );

        void incrementAngle( unsigned int dt );

        float getAngle( );

        void setAngle( float newAngle );

        void incrementOrbitAngle( unsigned int dt );

        float getOrbitAngle( );

        void setOrbitAngle( float newAngle );

        bool isPaused( );

    private:
        
        // model information
        glm::mat4 model;
        ObjectModel* objModelPtr;

        //children vector containg the worldID's of the objects children
        std::vector<unsigned int> childrenVector;

        //transform information
        glm::vec3 originScaleFactor; //scale factor
        glm::vec3 originTranslationVector; //translation in cartesian space
        glm::quat originRotation; //rotation vector in cartesian space
        glm::vec3 scaleFactor; //scale factor
        glm::vec3 translationVector; //translation in cartesian space
        glm::vec3 rotationVector; //rotation vector in cartesian space

        //vector of transforms to apply to the object 
        std::vector<glm::mat4> transformVector;

        //angle of rotation / orbit information (specialization)
        float angle;
        float tiltAngle;
        float orbitalAngle;

        glm::vec2 orbitalRadius;

        float rotationRate;
        float orbitRate;

        float rotationControlMultiplier;
        float orbitControlMultiplier;

        unsigned int objectID;
        unsigned int parentID;
};

#endif /* OBJECT_H */


