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

#include"Instance.h"

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

        void ChangePerspectiveStatic( int position );

        void cameraLeftOrRight( bool left );
        void cameraUpOrDown( bool up);
        void cameraZoomInOrOut(bool in);

        void startTracking( int planet );

        void moveBox( glm::vec3 pos );



    private:
        std::string ErrorString(GLenum error);
	
        bool cameraTracking;

        Camera *m_camera;
        Shader *m_shader;

        GLint m_projectionMatrix;
        GLint m_viewMatrix;
        GLint m_modelMatrix;

        ObjectTable objectRegistry; //holds the scene's objects

        std::vector<Instance> modelRegistry;


        //bullet members /////////////////////////////
        btBroadphaseInterface *broadphasePtr;
        btDefaultCollisionConfiguration *collisionConfigPtr;
        btCollisionDispatcher *dispatcherPtr;
        btSequentialImpulseConstraintSolver *solverPtr;
        btDiscreteDynamicsWorld *dynamicsWorldPtr;

        /////////////////////////////////////////////

        unsigned int boxIndex;

        glm::vec4 boxEdges;     
};


#endif /* GRAPHICS_H */