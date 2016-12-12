#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <iostream>
#include <vector>
using namespace std;

#include "graphics_headers.h"
#include "camera.h"
#include "shader.h"
#include "sound.h"
#include "object.h"

#include "GraphicsInfo.h"
#include "ObjectTable.h"

#include"Instance.h"

#include"HeightMap.h"

#include "ShipController.h"

#include"TextureUnit.h"

class Graphics
{
    public:
        static const int MAX_NUM_LIGHTS = 8;
        static const bool LEFT_PADDLE = true;
        static const bool RIGHT_PADDLE = false;

        Graphics();
        ~Graphics();
        bool Initialize(int width, int height, 
						 const GraphicsInfo& progInfo, 
						 long long time = 0 );

        void Update(unsigned int dt);
        void Render( unsigned int dt );

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

        void changeSpotLightSize( float increment );

        bool linkToCurrentShaderProgram( );

        void togglePausedState( );

        void startGame( );

        void resetBall( );

        void turnPaddle( bool select );

        void resetView( );

		void idleSplash( unsigned int dt );

		void turnOffSplash( );

		void moveShip( size_t ship );

		void rotateShip( size_t ship, float torque );

		void slowShipToHalt( size_t ship );

		void reverseShip( size_t ship );

		void stopShipsRotation( size_t ship );

		void fireGuns( size_t ship );

        void toggleLeft( int index );
         
        void toggleRight( int index );

        void startSplitScreen( int width, int height );
                
        void toggleSplitScreenView();

        bool isPlaying( );

    private:
		//helper functions
		void applyShipForces( unsigned int dt );

		bool shipRayTest( const btVector3& src, const btVector3 &dest, 
						  btVector3& hit, btCollisionObject& hitObject, 
						  btScalar& hitFraction );

		bool sameSign( float first, float second );

		//Broadside specific members
		std::vector<ShipController> shipRegistry;
		btVector3 windDirection;
		btScalar windForce;
		HeightMap oceanHeightMap;

		unsigned int cumultiveTime;

        bool splitScreen;
        int screenWidth;
        int screenHeight;
        bool wideView;
 
        std::vector<HUD> hud;
		

        //pinball game specific
        int score;
        int numberOfBalls;
        bool playingStateFlag;
		bool activeIdleState;
		bool goingUp;
		bool goingRight;
		int numberOfRightCalls;
		int numberOfUpCalls;
        bool returnBall;
		bool gameOverStep;
        unsigned int leftPaddle;
        unsigned int rightPaddle;
        unsigned int boxIndex;
        unsigned int ballIndex;
        int modelIndex;
        bool gameStarted;

        glm::vec4 boxEdges;


        bool rightPaddleUp;
        bool leftPaddleUp;

        void updateLeftPaddle( unsigned int dt );
        void updateRightPaddle( unsigned int dt );

        bool pauseNotifier;


        //lighting
        std::vector<Light> lights;

        std::vector<SpotLight> spotLight;

        unsigned int numberOfLights;

        unsigned int numberOfSpotLights;


        std::string ErrorString(GLenum error);
	
        bool cameraTracking;

        float savedSpotLightSize;

        std::vector<Camera> m_camera;

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
		GLint m_objectType;
		GLint m_time;

        ObjectTable objectRegistry; //holds the scene's objects
 
        Sound objectCollidedSound;

        std::vector<Instance> modelRegistry;


        //bullet members /////////////////////////////
        btBroadphaseInterface *broadphasePtr;
        btDefaultCollisionConfiguration *collisionConfigPtr;
        btCollisionDispatcher *dispatcherPtr;
        btSequentialImpulseConstraintSolver *solverPtr;
        btDiscreteDynamicsWorld *dynamicsWorldPtr;

        /////////////////////////////////////////////

          
};


#endif /* GRAPHICS_H */
