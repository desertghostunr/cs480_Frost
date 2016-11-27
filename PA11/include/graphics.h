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

struct ShipController
{
	static const int MAX_ROT = 2;

	size_t index;
	btVector3 force;
	bool slowDown;

	float accTorque;
	bool slowRotDown;

	ShipController( ): 
		index( 0 ), 
		force( btVector3( 0, 0, 0 ) ),
		slowDown( false ), 
		accTorque( 0.0f ),
		slowRotDown( false ){ }

	ShipController( size_t newIndex ): 
		index( newIndex ), 
		force( btVector3( 0, 0, 0 ) ),
		slowDown( false ),
		accTorque( 0.0f ),
		slowRotDown( false ){ }

	ShipController( const ShipController& src ): 
		index( src.index ), 
		force( src.force ),
		slowDown( src.slowDown ),
		accTorque( src.accTorque ),
		slowRotDown( src.slowRotDown ){ }

	const ShipController& operator=( const ShipController& src )
	{
		if( this != &src )
		{
			index = src.index;
			force = src.force;
			slowDown = src.slowDown;
			accTorque = src.accTorque;
			slowRotDown = src.slowRotDown;
		}

		return *this;
	}
};


class Graphics
{
    public:
        static const int MAX_NUM_LIGHTS = 8;
        static const bool LEFT_PADDLE = true;
        static const bool RIGHT_PADDLE = false;

        Graphics();
        ~Graphics();
        bool Initialize(int width, int height, const GraphicsInfo& progInfo );
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

        void togglePausedState( );

        void startGame( );

        void resetBall( );

        void turnPaddle( bool select );

        void resetView( );

		void idleSplash( unsigned int dt );

		void turnOffSplash( );

		void moveShip( size_t ship, float force );

		void rotateShip( size_t ship, float torque );

		void applyShipForces( );

    private:
		//helper functions
		bool sameSign( float first, float second );

		//Broadside specific members
		std::vector<ShipController> shipRegistry;

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
