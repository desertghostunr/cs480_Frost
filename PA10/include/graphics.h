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
        static const int MAX_NUM_LIGHTS = 8;

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

        void cycleShaderProgram( );

        void changeBrightness( std::string lightSelect, float redParam, 
                               float greenParam, float blueParam ); 

        void changeModelRegistryIndex( int i );

        void chanceSpotLightSize( float increment );

        bool linkToCurrentShaderProgram( );

    private:
        //lighting
        std::vector<Light> lights;

        std::vector<SpotLight> spotLight;

        unsigned int numberOfLights;

        unsigned int numberOfSpotLights;


        std::string ErrorString(GLenum error);
	
        bool cameraTracking;

        Camera *m_camera;

        std::vector<Shader> shaderRegistry;
        unsigned int shaderSelect;

        GLint m_projectionMatrix;
        GLint m_viewMatrix;
        GLint m_modelMatrix;
        GLint m_specular;
        GLint m_diffuse;
        GLint m_shininess;
        GLint m_numLights;
        GLint m_numSpotLights;

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
        unsigned int ballIndex;
        int modelIndex;

        glm::vec4 boxEdges;     
};


#endif /* GRAPHICS_H */
