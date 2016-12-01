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

struct ShipController //TO DO: ADD a forceOn and torqueOn member to check when key signals were interrupted!
{
	static const float MAX_SPEED;
	static const float MAX_ROT;
	static const float STD_FORCE;
	static const float STD_REVERSE;
	static const float STD_TORQUE;

	size_t index;

	btVector3 force;
	bool forceOn;
	bool slowDown;

	bool shipReversed;
	int shipReverseCounter;

	btVector3 torque;
	bool torqueOn;
	float torqueAcc;
	bool slowRotDown;


	ShipController( ): 
		index( 0 ), 
		force( btVector3( 0, 0, 0 ) ),
		forceOn( false ),
		slowDown( false ),
		shipReversed( false ),
		shipReverseCounter( 0 ),
		torque( btVector3( 0, 0, 0 ) ),
		torqueOn( false ),
		torqueAcc( 0 ),
		slowRotDown( false ){ }

	ShipController( size_t newIndex ): 
		index( newIndex ), 
		force( btVector3( 0, 0, 0 ) ),
		forceOn( false ),
		slowDown( false ),
		shipReversed( false ),
		shipReverseCounter( 0 ),
		torque( btVector3( 0, 0, 0 ) ),
		torqueOn( false ),
		torqueAcc( 0 ),
		slowRotDown( false ){ }

	ShipController( const ShipController& src ): 
		index( src.index ), 
		force( src.force ),
		forceOn( src.forceOn ),
		slowDown( src.slowDown ),
		shipReversed( src.shipReversed ),
		shipReverseCounter( src.shipReverseCounter ),
		torque( src.torque ),
		torqueOn( src.torqueOn ),
		torqueAcc( src.torqueAcc ),
		slowRotDown( src.slowRotDown ){ }

	const ShipController& operator=( const ShipController& src )
	{
		if( this != &src )
		{
			index = src.index;
			force = src.force;
			forceOn = src.forceOn;
			slowDown = src.slowDown;
			shipReversed = src.shipReversed;
			shipReverseCounter = src.shipReverseCounter;
			torque = src.torque;
			torqueOn = src.torqueOn;
			torqueAcc = src.torqueAcc;
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

		void moveShip( size_t ship );

		void rotateShip( size_t ship, float torque );

		void slowShipToHalt( size_t ship );

		void reverseShip( size_t ship );

		void stopShipsRotation( size_t ship );

    private:
		//helper functions
		void applyShipForces( );

		bool sameSign( float first, float second );

		//Broadside specific members
		std::vector<ShipController> shipRegistry;
		btVector3 windDirection;
		btScalar windForce;

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
