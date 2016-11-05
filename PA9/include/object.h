#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <string>
#include "graphics_headers.h"
#include "ObjectModel.h"


struct CollisionPtr
{
    btCollisionShape* collisionShape;
    btRigidBody* rigidBody;

    CollisionPtr(  ): collisionShape( NULL ), rigidBody( NULL ){ }

    bool empty( )
    {
        return ( collisionShape == NULL ) && ( rigidBody == NULL );
    }
};

struct CompoundCollisionPtr
{
    btCompoundShape* collisionShape;
    btRigidBody* rigidBody;

    CompoundCollisionPtr( ) : collisionShape( NULL ), rigidBody( NULL ) { }

    bool empty( )
    {
        return ( collisionShape == NULL ) && ( rigidBody == NULL );
    }
};

struct Origin
{
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;

    float angle;

    Origin( ):
        translation( glm::vec3( 0.0f ) ),
        rotation( ( 0.0f ) ),
        scale( glm::vec3( 1.0f ) ),
        angle( 0.0f ){ }

    Origin( const Origin& src ):
        translation( src.translation ),
        rotation( src.rotation ),
        scale( src.scale ),
        angle( 0.0f ){ }

    const Origin& operator =( const Origin& src )
    {
        if(  this != &src )
        {
            translation = src.translation;
            rotation = src.rotation;
            scale = src.scale;
            angle = src.angle;
        }
        return *this;
    }
};

class Object
{
    public:
        Object();
        ~Object();

        void Update(unsigned int dt);

        void Render();

        glm::mat4 GetModel();

        Origin getOrigin( );

        ObjectModel& getObjectModel( );

        bool hasObjectModel( );

        bool Initialize( ObjectModel * const srcPtr );

        bool setOrigin( const Origin& newOrigin );

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

        void commitTranslation( );

        void commitRotation( );

        void commitScale( );

        void commitParentLocation( );

        void incrementAngle( unsigned int dt );

        float getAngle( );

        void setAngle( float newAngle );

        std::string& Name( );

        const std::string& getName( );

        void setBulletTransform( const glm::mat4& bTrans );

        void commitBulletTransform( );

        CollisionPtr& CollisionInfo( );

        CompoundCollisionPtr& TableCollider( );

        glm::vec3 & getBScale( );

        const glm::vec3 & getTransVec( );

    private:
        //model info
        glm::mat4 model;
        ObjectModel* objModelPtr;

        //children vector containg the worldID's of the objects children
        std::vector<unsigned int> childrenVector;

        //transform information
        Origin parentOrigin;
        glm::vec3 scaleFactor; //scale factor
        glm::vec3 translationVector; //translation in cartesian space
        glm::vec3 rotationVector; //rotation vector in cartesian space

        glm::mat4 bulletTransform;

        //vector of transforms to apply to the object 
        std::vector<glm::mat4> transformVector;

        unsigned int objectID;
        unsigned int parentID;

        float angle;

        glm::vec3 bScale;

        std::string name;
        CollisionPtr collisionInfo;

        CompoundCollisionPtr tableCollider;
};

#endif /* OBJECT_H */


