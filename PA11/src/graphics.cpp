#include "graphics.h"
#include <algorithm>
#include <sstream>

#if defined( _WIN64 ) || ( _WIN32 )
	#if defined( M_PI )
		//do nothing
	#else
		#define M_PI 3.14159265359
	#endif
#endif

const float ShipController::MAX_SPEED = 4.11f;
const float ShipController::MAX_ROT = 2.5f;
const float ShipController::STD_FORCE = 0.75f;
const float ShipController::STD_REVERSE = -1.0f;
const float ShipController::STD_TORQUE = 0.75f;

//glm::decompose
//SDL window size
//convexTriangleMesh or convexhullshape for dynamic triangle mesh

//physics related callbacks
namespace ccb
{
	struct Ship
	{
		btRigidBody* shipPtr;
		btScalar maxSpeed;
		btScalar maxAngSpeed;

		Ship( ) : 
			shipPtr( NULL ), 
			maxSpeed( ShipController::MAX_SPEED ), 
			maxAngSpeed( ShipController::MAX_SPEED )
		{ }

		Ship( btRigidBody* bodyPtr, btScalar newMaxSpeed, btScalar newMaxAngSpeed ) 
			: shipPtr( bodyPtr ),
			maxSpeed( newMaxSpeed ),
			maxAngSpeed( newMaxAngSpeed )
		{ }

		Ship( const Ship& src ): 
			shipPtr( src.shipPtr ), 
			maxSpeed( src.maxSpeed ),
			maxAngSpeed( src.maxAngSpeed )
		{ }
	};

    std::vector<Ship> shipReg;


    void TickCallback( btDynamicsWorld * world, btScalar timeStep )
    {
		size_t index;
        btVector3 velocity;
		btVector3 angVelocity;
        btScalar speed;
		btScalar angSpeed;

		for( index = 0; index < shipReg.size( ); index++ )
		{
			if( shipReg[ index ].shipPtr != NULL )
			{
				velocity = shipReg[ index ].shipPtr->getLinearVelocity( );

				speed = velocity.length( );

				if( speed > shipReg[ index ].maxSpeed 
					&& shipReg[ index ].maxSpeed >= 0.0f )
				{
					velocity *= shipReg[ index ].maxSpeed / speed;
					shipReg[ index ].shipPtr->setLinearVelocity( velocity );
				}

				angVelocity = shipReg[ index ].shipPtr->getAngularVelocity( );

				angSpeed = angVelocity.length( );

				if( angSpeed > shipReg[ index ].maxAngSpeed 
					&& shipReg[ index ].maxAngSpeed >= 0.0f )
				{
					angVelocity *= shipReg[ index ].maxAngSpeed / angSpeed;
					shipReg[ index ].shipPtr->setAngularVelocity( angVelocity );
				}
			}			
		}

        
    }

    struct ScoreContactResultCallback : public btCollisionWorld::ContactResultCallback
    {

        //functions
        ScoreContactResultCallback( int * inPtr ) : scorePtr( inPtr ) { }

        btScalar addSingleResult
        (
            btManifoldPoint& cp,
            const btCollisionObjectWrapper * colObj0Wrap,
            int partId0,
            int index0,
            const btCollisionObjectWrapper  * colObj1Wrap,
            int partId1,
            int index1
        )
        {
            if( scorePtr != NULL )
            {
                *scorePtr += 1;

                std::cout << "Score: " << *scorePtr << std::endl;
            }

            return 0;
        }


        //member
        int * scorePtr;
    };

    struct TrapContactResultCallback : public btCollisionWorld::ContactResultCallback
    {

        //functions
        TrapContactResultCallback( bool * inPtr ) : returnPtr( inPtr ) { }

        btScalar addSingleResult
        (
            btManifoldPoint& cp,
            const btCollisionObjectWrapper * colObj0Wrap,
            int partId0,
            int index0,
            const btCollisionObjectWrapper  * colObj1Wrap,
            int partId1,
            int index1
        )
        {
            if( returnPtr != NULL )
            {
                *returnPtr = true;
            }

            return 0;
        }


        //member
        bool * returnPtr;
    };


};


//graphcs engine
Graphics::Graphics()
{
    broadphasePtr = NULL;
    collisionConfigPtr = NULL;
    dispatcherPtr = NULL;
    solverPtr = NULL;
    dynamicsWorldPtr = NULL;

    

    boxIndex = -1;
    ballIndex = 0;
    modelIndex = 0;

    shaderSelect = 0;
    playingStateFlag = false;
    returnBall = false;
    rightPaddleUp = false;
    leftPaddleUp = false;

    rightPaddle = leftPaddle = 0;

    numberOfBalls = 3;

    pauseNotifier = false;

	activeIdleState = true;

	numberOfRightCalls = 0;
	numberOfUpCalls = 0;

	goingUp = true;
	goingRight = true;
	gameOverStep = false;
    gameStarted = false;
}

Graphics::~Graphics()
{
    unsigned int index = 0;

	if( dynamicsWorldPtr != NULL )
	{
		for( index = 0; index < objectRegistry.getSize( ); index++ )
		{
			if( objectRegistry[ index ].getObjectsID( ) == Object::PC_OBJECT )
			{
				if( objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody != NULL )
				{
					dynamicsWorldPtr->removeRigidBody( objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody );

					delete objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody->getMotionState( );
					delete objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody;
					objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody = NULL;
				}

				if( objectRegistry[ index ].CompoundCollisionInfo( ).collisionShape != NULL )
				{
					delete objectRegistry[ index ].CompoundCollisionInfo( ).collisionShape;
					objectRegistry[ index ].CompoundCollisionInfo( ).collisionShape = NULL;
				}
			}

			if( objectRegistry[ index ].getObjectsID( ) == Object::P_OBJECT )
			{
				if( objectRegistry[ index ].CollisionInfo( ).rigidBody != NULL )
				{
					dynamicsWorldPtr->removeRigidBody( objectRegistry[ index ].CollisionInfo( ).rigidBody );

					delete objectRegistry[ index ].CollisionInfo( ).rigidBody->getMotionState( );
					delete objectRegistry[ index ].CollisionInfo( ).rigidBody;
					objectRegistry[ index ].CollisionInfo( ).rigidBody = NULL;
				}

				if( objectRegistry[ index ].CollisionInfo( ).collisionShape != NULL )
				{
					delete objectRegistry[ index ].CollisionInfo( ).collisionShape;

					objectRegistry[ index ].CollisionInfo( ).collisionShape = NULL;
				}
			}
		}
	} 

    if( dynamicsWorldPtr != NULL )
    {
        delete dynamicsWorldPtr;

        dynamicsWorldPtr = NULL;
    }

    if( solverPtr != NULL )
    {
        delete solverPtr;

        solverPtr = NULL;
    }

    if( dispatcherPtr != NULL )
    {
        delete dispatcherPtr;

        dispatcherPtr = NULL;
    }

    if( collisionConfigPtr != NULL )
    {
        delete collisionConfigPtr;

        collisionConfigPtr = NULL;
    }

    if( broadphasePtr != NULL )
    {
        delete broadphasePtr;

        broadphasePtr = NULL;
    }

	while( objectCollidedSound.SoundPlaying( ) )
	{
		//wait for sound to finish
	}
}

bool Graphics::Initialize
(
    int width, 
    int height, 
    const GraphicsInfo& progInfo,
	long long time
)
{
    // Used for the linux OS
    #if !defined(__APPLE__) && !defined(MACOSX)
        // cout << glewGetString(GLEW_VERSION) << endl;
        glewExperimental = GL_TRUE;

        auto status = glewInit();

        // This is here to grab the error that comes from glew init.
        // This error is an GL_INVALID_ENUM that has no effects on the performance
        glGetError();
        //Check for error
        if (status != GLEW_OK)
        {
            std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
            return false;
        }
    #endif

    // For OpenGL 3
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    bool successFlag;
    unsigned int index, pIndex, sIndex;

    std::vector<bool> taken;
    

    btCollisionShape * tmpShapePtr = NULL;
    btDefaultMotionState* tmpMotionState = NULL;

    btRigidBody * tmpRigidBody = NULL;

    btCompoundShape * tmpCompoundShape = NULL;

    btScalar mass;
    btVector3 inertia;
    btTransform transform;

	oceanHeightMap = HeightMap( ( long ) time );

	oceanHeightMap.generateHeightMap( 1024, 1024 );

	//wind direction
	std::cout << "Wind direction: " << progInfo.windDirection.x << ", ";
	std::cout << progInfo.windDirection.y << ", " << progInfo.windDirection.z;
	std::cout << "." << std::endl;

	windDirection = btVector3( progInfo.windDirection.x, 
							   progInfo.windDirection.y, 
							   progInfo.windDirection.z );

	windDirection = windDirection.normalized( );

	if( progInfo.windForce <= 0.01f )
	{
		windForce = ShipController::STD_FORCE;
	}
	else
	{
		windForce = progInfo.windForce;
	}

    // Init Camera
    m_camera = new Camera();
    cameraTracking = false;
    if(!m_camera->Initialize(width, height))
    {
        printf("Camera Failed to Initialize\n");
        return false;
    }
	
    score = 0;

    objectCollidedSound.loadSound("sounds/Canon_Fire.wav");

    modelRegistry.clear( );


    for( index = 0; index < progInfo.modelVector.size( ); index++ )
    {
        modelRegistry.push_back( Instance( ) );
        modelRegistry[ modelRegistry.size( ) - 1 ].modelPath
                                                = progInfo.modelVector[ index ];

        successFlag = modelRegistry[ modelRegistry.size( ) - 1 ]
                        .objModel.loadModelFromFile( progInfo.modelVector[ index ] );
        

        if( !successFlag )
        {
            std::cout << "Error Loading " << modelRegistry[ 
                             modelRegistry.size( ) - 1 ].modelPath << std::endl;

            return false;
        }

    }
    
    for( pIndex = 0; pIndex < progInfo.objectData.size( ); pIndex++ )
    {
		if( progInfo.objectData[ pIndex ].type == P_OBJECT_TYPE )
		{
			objectRegistry.addObject( -1, Object::P_OBJECT );
		}
		else if( progInfo.objectData[ pIndex ].type == P_C_OBJECT_TYPE )
		{
			objectRegistry.addObject( -1, Object::PC_OBJECT );
		}
		else
		{
			objectRegistry.addObject( -1, Object::BASE_OBJECT );
		}
        
        if( progInfo.objectData[ pIndex ].modelID < modelRegistry.size( ) )
        {

            objectRegistry[ objectRegistry.getSize( ) - 1 ]
                .Initialize( &modelRegistry[ progInfo.objectData[ pIndex ].modelID ].objModel );

            modelRegistry[ modelRegistry.size( ) - 1 ].objModel.incrementReference( );
        }
        else
        {
            std::cout << "Invalid model ID provided!" << std::endl;
            return false;
        }

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].setScale( progInfo.objectData[ pIndex ].scale );

        objectRegistry[ objectRegistry.getSize( ) - 1 
        ].setRotationVector( progInfo.objectData[ pIndex ].rotationAxes );

        objectRegistry[ objectRegistry.getSize( ) - 1 ].setAngle( progInfo.objectData[ pIndex ].rotationAngles.y );

        objectRegistry[ objectRegistry.getSize( ) - 1 ].setTranslationVector( progInfo.objectData[ pIndex ].position );

        objectRegistry[ objectRegistry.getSize( ) - 1 ].Name( ) = progInfo.objectData[ pIndex ].name;

		//set lighting code
		if( objectRegistry[ objectRegistry.getSize( ) - 1 ].getName( ) == "sky" )
		{
			objectRegistry[ objectRegistry.getSize( ) - 1 ].LightCode( ) = Object::NO_LIGHTING;
		}
		else if( objectRegistry[ objectRegistry.getSize( ) - 1 ].getName( ) == "ocean" )
		{
			objectRegistry[ objectRegistry.getSize( ) - 1 ].LightCode( ) = Object::WAVE;
		}
		else
		{
			objectRegistry[ objectRegistry.getSize( ) - 1 ].LightCode( ) = Object::DEFAULT_LIGHTING;
		}


        objectRegistry[ objectRegistry.getSize( ) - 1 ].getBScale( ) = progInfo.objectData[ pIndex ].bScale;
    }
    
    for( pIndex = 0; pIndex < objectRegistry.getSize( ); pIndex++ )
    {

        for( index = 0; index < progInfo.objectData[ pIndex ].childID.size( ); index++ )
        {
            objectRegistry.setChild( progInfo.objectData[ pIndex ].childID[ index ], pIndex );
        }


    }

    //lighting information /////////////////////////////////////////////////////

    lights = progInfo.lights;

    if( lights.size( ) > MAX_NUM_LIGHTS )
    {
        lights.resize( MAX_NUM_LIGHTS );
    }
    
    spotLight = progInfo.spotLight;

    if( spotLight.size( ) > MAX_NUM_LIGHTS )
    {
        spotLight.resize( MAX_NUM_LIGHTS );
    }

    for( index = 0; index < spotLight.size( ); index++ )
    {
        spotLight[ index ].cosine = glm::cos( glm::radians( spotLight[ index ].coneAngle ) );

        spotLight[ index ].oTFIndex = -1;
    }    
    
    numberOfLights = lights.size( );
    numberOfSpotLights = spotLight.size( );

    taken.resize( 0 );

    taken.resize( objectRegistry.getSize( ), false );

    for( pIndex = 0; pIndex < spotLight.size( ); pIndex++ )
    {
        sIndex = -1;

        for( index = 0; index < objectRegistry.getSize( ); index++ )
        {            
            if( !taken[ index ] 
                && spotLight[ pIndex ].objectToFollow == objectRegistry[ index ].getName( ) )
            {
                sIndex = index;

                taken[ index ] = true;

                break;
            }
        }

        if( (int)sIndex == -1 )
        {
            for( index = 0; index < objectRegistry.getSize( ); index++ )
            {
                if( spotLight[ pIndex ].objectToFollow == objectRegistry[ index ].getName( ) )
                {
                    sIndex = index;
                }
            }
        }

        spotLight[ pIndex ].oTFIndex = sIndex;
    }

    for( pIndex = 0; pIndex < spotLight.size( ); pIndex++ )
    {
        if( (int)spotLight[ pIndex ].oTFIndex == -1 )
        {
            std::cout << "Error unmapped spot light to object!" << std::endl;
            return false;
        }
    }


    ////////////////////////////////////////////////////////////////////////////

    
    shaderRegistry.resize( progInfo.shaderVector.size( ) );

    for( sIndex = 0; sIndex < progInfo.shaderVector.size( ); sIndex++ )
    {
        // Set up the shaders      

        if( !shaderRegistry[ sIndex ].Initialize( ) )
        {
            printf( "Shader Failed to Initialize\n" );
            return false;
        }

        for( index = 0; index < progInfo.shaderVector[ sIndex ].size( ); index++ )
        {
            if( !shaderRegistry[ sIndex ].
                AddShader( progInfo.shaderVector[ sIndex ][ index ].first, 
                           progInfo.shaderVector[ sIndex ][ index ].second ) )
            {
                if( progInfo.shaderVector[ sIndex ][ index ].first == GL_VERTEX_SHADER )
                {
                    printf( "Vertex Shader failed to Initialize\n" );
                }
                else
                {
                    printf( "Fragment Shader failed to Initialize\n" );
                }
                return false;
            }
        }

        if( !shaderRegistry[ sIndex ].Finalize( ) )
        {
            printf( "Program to Finalize\n" );
            return false;
        }
    }

    if( !linkToCurrentShaderProgram( ) ) 
    {
        std::cout << "Failed to find the proper uniforms in the program!" << std::endl;
        return false;
    }

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);   

    // INITIALIZE BULLET //////////////////////////////////////////////
    broadphasePtr = new btDbvtBroadphase( );
    collisionConfigPtr = new btDefaultCollisionConfiguration( );
    dispatcherPtr = new btCollisionDispatcher( collisionConfigPtr );
    solverPtr = new btSequentialImpulseConstraintSolver( );
    dynamicsWorldPtr = new btDiscreteDynamicsWorld( dispatcherPtr, broadphasePtr, solverPtr, collisionConfigPtr );
    dynamicsWorldPtr->setGravity( btVector3( 0.0f, -9.8f, 0.0f ) );
    ///////////////////////////////////////////////////////////////////

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( objectRegistry[ index ].getName( ) == "ship" )
        {
            ballIndex = index;

			tmpShapePtr = new btBoxShape( btVector3( objectRegistry[ index ].getBScale( ).x + 1, objectRegistry[ index ].getBScale( ).y + 1, objectRegistry[ index ].getBScale( ).z + 1 ) );

            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );
            
            mass = 2.5f;

            inertia = btVector3( 0, 0, 0 );


            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 0.015f;
            rigidBodyConstruct.m_friction = 100.0f;
  

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setLinearFactor( btVector3( 1, 0, 1 ) );
			tmpRigidBody->setAngularFactor( btVector3( 0, 1, 0 ) );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

			//initialize Ship Registries
			ccb::shipReg.push_back( ccb::Ship( tmpRigidBody, 
											   ShipController::MAX_SPEED, 
											   ShipController::MAX_ROT ) );
			shipRegistry.push_back( index );
        }
        else if( objectRegistry[ index ].getName( ) == "ocean" )
        {
            tmpCompoundShape = new btCompoundShape( );

            transform.setIdentity( );
            transform.setOrigin( btVector3( 0, -1 * objectRegistry[ index ].getBScale( ).y / 2, 0 ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( 0, 1, 0 ), 10 );

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpCompoundShape, inertia );

            rigidBodyConstruct.m_restitution = 0.0f;
            rigidBodyConstruct.m_friction = 1000.00f;
            

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            objectRegistry[ index ].CompoundCollisionInfo( ).collisionShape = tmpCompoundShape;
            objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody = tmpRigidBody;


        }


        if( objectRegistry[ index ].getObjectType( ) == Object::P_OBJECT )
        {
            objectRegistry[ index ].CollisionInfo( ).collisionShape = tmpShapePtr;
            objectRegistry[ index ].CollisionInfo( ).rigidBody = tmpRigidBody;
        }        

		if( objectRegistry[ index ].getObjectType( ) == Object::P_OBJECT 
			|| objectRegistry[ index ].getObjectType( ) == Object::PC_OBJECT )
		{
			dynamicsWorldPtr->addRigidBody( tmpRigidBody );
		}        

        tmpCompoundShape = NULL;
        tmpShapePtr = NULL;
        tmpMotionState = NULL;
        tmpRigidBody = NULL;
    }

    dynamicsWorldPtr->setInternalTickCallback( ccb::TickCallback );

    return true;
}

void Graphics::Update(unsigned int dt)
{
    unsigned int index;

	applyShipForces( );

	if( gameOverStep )
	{
		dynamicsWorldPtr->stepSimulation( dt, 10 );
		gameOverStep = false;
	}

    if( returnBall )
    {
        resetBall( );
        dynamicsWorldPtr->stepSimulation( dt, 10 );
    }

    if( playingStateFlag )
    {

        dynamicsWorldPtr->stepSimulation( dt, 10 );

    }
	else if( activeIdleState )
	{
		idleSplash( dt );
	}

    // Update the objects
    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( !objectRegistry[ index ].isChild( ) )
        {
            updateChildren( index, dt );
        }        
    }

	if( playingStateFlag )
	{
		//put camera stuff here ///////
		m_camera->updateCamera( true, objectRegistry[ shipRegistry[ 0 ].index ].getPositionInWorld( ) );
		//////////////////////////////
	}


	for( index = 0; index < shipRegistry.size( ); index++ )
	{
		//right guns
		if( shipRegistry[ index ].rightReloadTime > 0 )
		{
			shipRegistry[ index ].rightReloadTime -= dt;
			//std::cout << "Right guns reloading: " << shipRegistry[ index ].rightReloadTime << std::endl;
		}
		else
		{
			shipRegistry[ index ].rightReloadTime = 0;
		}

		//left guns
		if( shipRegistry[ index ].leftReloadTime > 0 )
		{
			shipRegistry[ index ].leftReloadTime -= dt;
			//std::cout << "Left guns reloading: " << shipRegistry[ index ].leftReloadTime << std::endl;
		}
		else
		{
			shipRegistry[ index ].leftReloadTime = 0;
		}

		if( shipRegistry[ index ].healthPoints <= 0 )
		{
			std::cout << "Player " << index + 1 << " loses!" << std::endl;

			playingStateFlag = false;
		}
	}
	
}

void Graphics::Render( unsigned int dt )
{
    unsigned int index;
	int lightCode = -1;
	float normedCTime;

    glm::vec4 tmpVec;

    //clear the screen
    glClearColor(0.2, 0.15, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the correct program
    shaderRegistry[ shaderSelect ].Enable( );

    // Send in the projection and view to the shader
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView()));

    glUniform1i( m_numLights, ( GLint ) lights.size( ) );
    glUniform1i( m_numSpotLights, ( GLint ) spotLight.size( ) );

	cumultiveTime += dt;
	normedCTime = cumultiveTime;
	normedCTime /= 1000000.0f;

	glUniform1f( m_time, normedCTime );

	

    for( index = 0; 
         index < std::min( numberOfLights, ( unsigned int )lights.size( ) ); 
         index++ )
    {
        glUniform4f( lights[ index ].ambientLoc, lights[ index ].ambient.r, 
                     lights[ index ].ambient.g, lights[ index ].ambient.b, 
                     lights[ index ].ambient.a );

        glUniform4f( lights[ index ].incomingLoc, lights[ index ].incoming.r, 
                     lights[ index ].incoming.g, lights[ index ].incoming.b, 
                     lights[ index ].incoming.a );
    } 

	for( index = 0; index < shipRegistry.size( ); index += 1 )
	{
		if( 2 * index > spotLight.size( ) || ( 2 * index ) + 1 > spotLight.size( ) )
		{
			std::cout << "Error: too few spotLights!" << std::endl;
		}

		tmpVec = glm::vec4( shipRegistry[ index ].rightHit.getX( ), 
							shipRegistry[ index ].rightHit.getY( ), 
							shipRegistry[ index ].rightHit.getZ( ), 1.0 );

		glUniform4f( spotLight[ 2 * index ].followLoc, tmpVec.x,
					 tmpVec.y + spotLight[ 2 * index ].spotHeight, tmpVec.z, 1.0 );

		glUniform4f( spotLight[ 2 * index ].ambientLoc, spotLight[ 2 * index ].ambient.r,
					 spotLight[ 2 * index ].ambient.g, spotLight[ 2 * index ].ambient.b,
					 spotLight[ 2 * index ].ambient.a );

		glUniform3f( spotLight[ 2 * index ].incomingLoc, spotLight[ 2 * index ].incoming.r,
					 spotLight[ 2 * index ].incoming.g, spotLight[ 2 * index ].incoming.b );

		glUniform1f( spotLight[ 2 * index ].cosineLoc, spotLight[ 2 * index ].cosine );

		tmpVec = glm::vec4( shipRegistry[ index ].leftHit.getX( ),
							shipRegistry[ index ].leftHit.getY( ),
							shipRegistry[ index ].leftHit.getZ( ), 1.0 );

		glUniform4f( spotLight[ ( 2 * index ) + 1 ].followLoc, tmpVec.x,
					 tmpVec.y + spotLight[ ( 2 * index ) + 1 ].spotHeight, tmpVec.z, 1.0 );

		glUniform4f( spotLight[ ( 2 * index ) + 1].ambientLoc, spotLight[ ( 2 * index ) + 1 ].ambient.r,
					 spotLight[ ( 2 * index ) + 1].ambient.g, spotLight[ ( 2 * index ) + 1 ].ambient.b,
					 spotLight[ ( 2 * index ) + 1 ].ambient.a );

		glUniform3f( spotLight[ ( 2 * index ) + 1 ].incomingLoc, spotLight[ ( 2 * index ) + 1 ].incoming.r,
					 spotLight[ ( 2 * index ) + 1 ].incoming.g, spotLight[ ( 2 * index ) + 1 ].incoming.b );

		glUniform1f( spotLight[ ( 2 * index ) + 1 ].cosineLoc, spotLight[ ( 2 * index ) + 1 ].cosine );

	}

    for( index  = 2 * index; 
         index < std::min( numberOfSpotLights, (unsigned int )spotLight.size( ) ); 
         index++ )
    {
		tmpVec = objectRegistry[ spotLight[ index ].oTFIndex ].getPositionInWorld( );

        glUniform4f( spotLight[ index ].followLoc, tmpVec.x, 
                     tmpVec.y + spotLight[ index ].spotHeight, tmpVec.z, 1.0 );

        glUniform4f( spotLight[ index ].ambientLoc, spotLight[ index ].ambient.r, 
                     spotLight[ index ].ambient.g, spotLight[ index ].ambient.b, 
                     spotLight[ index ].ambient.a );

        glUniform3f( spotLight[ index ].incomingLoc, spotLight[ index ].incoming.r, 
                     spotLight[ index ].incoming.g, spotLight[ index ].incoming.b );

        glUniform1f( spotLight[ index ].cosineLoc, spotLight[ index ].cosine );
    }    

    // Render the objects
    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        glUniformMatrix4fv( m_modelMatrix, 1, GL_FALSE,
                            glm::value_ptr(objectRegistry[index].GetModel()));

        tmpVec = objectRegistry[ index ].getObjectModel( ).getDiffuse( );

        glUniform4f( m_diffuse, tmpVec.r, tmpVec.g, tmpVec.b, tmpVec.a );

        tmpVec = objectRegistry[ index ].getObjectModel( ).getSpecular( );

        glUniform4f( m_specular, tmpVec.r, tmpVec.g, tmpVec.b, tmpVec.a );

        glUniform1f( m_shininess, objectRegistry[ index ].getObjectModel( ).getShininess( ) );

		if( lightCode != objectRegistry[ index ].LightCode( ) )
		{
			lightCode = objectRegistry[ index ].LightCode( );
			glUniform1i( m_objectType, lightCode );
		}

		if( lightCode == Object::WAVE )
		{
			glUniform1i( oceanHeightMap.HeightMapUniform( ), 1 );
			glActiveTexture( GL_TEXTURE1 );
		    glBindTexture( GL_TEXTURE_2D, oceanHeightMap.HeightMapTexture( ) );
		}

        objectRegistry[index].Render();
    }

	shaderRegistry[ shaderSelect ].Disable( );

    // Get any errors from OpenGL
    auto error = glGetError();
    if ( error != GL_NO_ERROR )
    {
        string val = ErrorString( error );
        std::cout<< "Error initializing OpenGL! " << error << ", " << val << std::endl;
    }
}

std::string Graphics::ErrorString(GLenum error)
{
    if(error == GL_INVALID_ENUM)
    {
        return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
    }

    else if(error == GL_INVALID_VALUE)
    {
        return "GL_INVALID_VALUE: A numeric argument is out of range.";
    }

    else if(error == GL_INVALID_OPERATION)
    {
        return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
    }

    else if(error == GL_INVALID_FRAMEBUFFER_OPERATION)
    {
        return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
    }

    else if(error == GL_OUT_OF_MEMORY)
    {
        return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
    }
    else
    {
        return "None";
    }
}

// UPDATE LIST //////////////////
/***************************************

@brief updateList

@details is a list of transforms for an object

@param in: objectID: the id or index of the Object

@param in: dt: the time delta

@notes Must be filled in for the use with a program by the programmer

***************************************/
bool Graphics::updateList( unsigned int objectID, unsigned int dt )
{
    btTransform trans;
    btScalar modTrans[ 16 ];
    int tempScore = score;    

    if( ( objectID > objectRegistry.getSize( ) ) )
    {
         return false;
    }

    //here is an example for the general call of transforms for a planet
    //This does the basic operations... I still don't know how to tilt orbits
    //Play around with changing the order of events / things
    // write transforms here //////////////////////////    

    objectRegistry[ objectID ].commitScale( );

    if( objectRegistry[ objectID ].getObjectType( ) == Object::P_OBJECT 
		&& !objectRegistry[ objectID ].CollisionInfo( ).empty( ) )
    {
        objectRegistry[ objectID ].CollisionInfo( ).rigidBody->getMotionState( )->getWorldTransform( trans );

        trans.getOpenGLMatrix( modTrans );

        objectRegistry[ objectID ].setBulletTransform( glm::make_mat4( modTrans ) );

        objectRegistry[ objectID ].commitBulletTransform( );
    }
	else if( objectRegistry[ objectID ].getObjectType( ) == Object::BASE_OBJECT )
	{
		if( objectRegistry[ objectID ].getName( ) == "sky" )
		{
			objectRegistry[ objectID ].incrementAngle( dt, 0.0065 );
			objectRegistry[ objectID ].commitRotation( );
			objectRegistry[ objectID ].commitTranslation( );
		}
		else if( objectRegistry[ objectID ].getName( ) == "sail" )
		{
			objectRegistry[ objectID ].commitRotation( );
			objectRegistry[ objectID ].commitTranslation( );
		}
		else if( objectRegistry[ objectID ].getName( ) == "sail_nt" )
		{
			objectRegistry[ objectID ].commitTranslation( );
		}
		
		
	}    

    objectRegistry[ objectID ].Update( dt );
    //////////////////////////////////////////////////

    return true;
}


// UPDATE CHILDREN //////////////////
/***************************************

@brief updateChildren

@details recursively updates an object and its object's children

@param in: objectID: the id or index of the Object

@param in: dt: the time delta

@notes None

***************************************/
 bool Graphics::updateChildren( unsigned int objectID, unsigned int dt )
 {
     bool noErrors = true;
     unsigned int index, childsID;

     if( ( objectID > objectRegistry.getSize( ) ) )
     {
         return false;
     }

     noErrors = ( noErrors  && updateList( objectID, dt ) );

     for( index = 0; 
          index < objectRegistry[ objectID ].getNumberOfChildren( ); 
          index++ )
     {
         childsID = objectRegistry[ objectID ].getChildsWorldID( index );

         if( childsID < objectRegistry.getSize( ) )
         {
			objectRegistry[ childsID ].setParentModel( objectRegistry[ objectID ].GetModel( ) );
            
            noErrors = ( noErrors  && updateChildren( childsID, dt ) );
         }
        
     }

     return true;


 }

// Change Perspective  //////////////////
/***************************************

@brief toggleAllPaused

@details toggles which perspective to look at the solar system

@param in: which perspective to set view towards

@notes None

***************************************/
void Graphics::ChangePerspectiveStatic( int position )
{
    if( position == 1 )
    {
	m_camera->LookTopDown();
    }
    else if( position == 2 )
    {
	m_camera->LookSideToSide();
    }
    m_camera->updateLookAt();
    cameraTracking = false;
        
}

void Graphics::cameraLeftOrRight( bool left )
{
   if( left )
   {
      m_camera->moveLeft();  
   }
   else
   {
      m_camera->moveRight();
   }
   //m_camera->updateLookAt();
   cameraTracking = false;
}

void Graphics::cameraUpOrDown( bool up)
{
   if( up )
   {
      m_camera->moveUp();  
   }
   else
   {
      m_camera->moveDown();
   }
   //m_camera->updateLookAt();
   cameraTracking = false;
}

void Graphics::cameraZoomInOrOut(bool in)
{
   if( in )
   {
      m_camera->zoomIn();  
   }
   else
   {
      m_camera->zoomOut();
   }
   m_camera->updateLookAt();
   
}

void Graphics::startTracking( int planet )
{
    if( planet < 0 )
    {
        planet = 0;
    }

   cameraTracking = true;
}

//create a function similar to this to rotate the paddles
void Graphics::moveBox( glm::vec3 pos )
{
    btTransform currPos;
    btVector3 change;

    if( !playingStateFlag )
    {
        if( pauseNotifier )
        {
            return;
        }

        pauseNotifier = true;
        std::cout << std::endl;
        std::cout << "The game is either paused or has not begun!" << std::endl;
        std::cout << "Press the space bar to start the game." << std::endl;
        std::cout << "Press p to unpause the game." << std::endl;
        std::cout << std::endl;
        return;
    }

    if( objectRegistry.getSize( ) > boxIndex && !objectRegistry[ boxIndex ].CollisionInfo( ).empty( ) )
    {
        objectRegistry[ boxIndex ].CollisionInfo( ).rigidBody->getMotionState( )->getWorldTransform( currPos );

        change = currPos.getOrigin( ) + btVector3( 0, 0, pos.z );

        change.setZ( std::max( change.getZ( ), boxEdges.g + objectRegistry[ boxIndex ].getBScale( ).z - 115 ) );
        change.setZ( std::min( change.getZ( ), boxEdges.g + objectRegistry[ boxIndex ].getBScale( ).z - 95 ) );

        currPos.setOrigin( change );

        objectRegistry[ boxIndex ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( currPos );
    }
    
}

void Graphics::cycleShaderProgram( )
{
    shaderSelect++;    

    if( shaderSelect >= shaderRegistry.size( ) )
    {
        shaderSelect = 0;
    }

    std::cout << "Shader Program " << shaderSelect + 1 << " selected." << std::endl;

    linkToCurrentShaderProgram( );
}

void Graphics::changeBrightness( std::string lightSelect, float redParam, 
                               float greenParam, float blueParam )
{
    if( lightSelect == "ambient" )
    {
        if( lights[ 0 ].ambient.r >= 0.95f && redParam > 0.0f )
        {
            lights[ 0 ].ambient.r = 1.0f;
        }
        else if( lights[ 0 ].ambient.r <= 0.05f && redParam < 0.0f )
        {
            lights[ 0 ].ambient.r = 0.0f;
        }
        else
        {
            lights[ 0 ].ambient.r = lights[ 0 ].ambient.r + redParam;
        }



        if( lights[ 0 ].ambient.g >= 0.95f && greenParam > 0.0f )
        {
            lights[ 0 ].ambient.g = 1.0f;
        }
        else if( lights[ 0 ].ambient.g <= 0.05f && greenParam < 0.0f )
        {
            lights[ 0 ].ambient.g = 0.0f;
        }
        else
        {
            lights[ 0 ].ambient.g = lights[ 0 ].ambient.g + greenParam;
        }



        if( lights[ 0 ].ambient.b >= 0.95f && blueParam > 0.0f )
        {
            lights[ 0 ].ambient.b = 1.0f;
        }
        else if( lights[ 0 ].ambient.b <= 0.05f && blueParam < 0.0f )
        {
            lights[ 0 ].ambient.b = 0.0f;
        }
        else
        {
            lights[ 0 ].ambient.b = lights[ 0 ].ambient.b + blueParam;
        }
    }


    else if( lightSelect == "specular" )
    {
        std::cout << modelRegistry[ modelIndex ].modelPath << "'s specular: ";
        std::cout << modelRegistry[ modelIndex ].objModel.getSpecular( ).r << ", ";
        std::cout << modelRegistry[ modelIndex ].objModel.getSpecular( ).g << ", ";
        std::cout << modelRegistry[ modelIndex ].objModel.getSpecular( ).b << "." << std::endl;

        if( modelRegistry[ modelIndex ].objModel.getSpecular().r >= 0.95f && redParam > 0.0f )
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().r = 1.0f;
        }
        else if( modelRegistry[ modelIndex ].objModel.getSpecular().r <= 0.05f && redParam < 0.0f )
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().r = 0.0f;
        }
        else
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().r = modelRegistry[ modelIndex ].objModel.getSpecular().r + redParam;
        }



        if( modelRegistry[ modelIndex ].objModel.getSpecular().g >= 0.95f && greenParam > 0.0f )
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().g = 1.0f;
        }
        else if( modelRegistry[ modelIndex ].objModel.getSpecular().g <= 0.05f && greenParam < 0.0f )
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().g = 0.0f;
        }
        else
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().g = modelRegistry[ modelIndex ].objModel.getSpecular().g + greenParam;
        }



        if( modelRegistry[ modelIndex ].objModel.getSpecular().b >= 0.95f && blueParam > 0.0f )
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().b = 1.0f;
        }
        else if( modelRegistry[ modelIndex ].objModel.getSpecular().b <= 0.05f && blueParam < 0.0f )
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().b = 0.0f;
        }
        else
        {
            modelRegistry[ modelIndex ].objModel.getSpecular().b = modelRegistry[ modelIndex ].objModel.getSpecular().b + blueParam;
        }        
    }


    else if( lightSelect == "spot"  && !spotLight.empty( ) )
    {
        std::cout << "Spot Light Brightness: ";
        std::cout << spotLight[ 0 ].ambient.r << ", ";
        std::cout << spotLight[ 0 ].ambient.g << ", ";
        std::cout << spotLight[ 0 ].ambient.b << "." << std::endl;


        if( spotLight[ 0 ].ambient.r >= 0.95f && redParam > 0.0f )
        {
            spotLight[ 0 ].ambient.r = 1.0f;
        }
        else if( spotLight[ 0 ].ambient.r <= 0.05f && redParam < 0.0f )
        {
            spotLight[ 0 ].ambient.r = 0.0f;
        }
        else
        {
            spotLight[ 0 ].ambient.r = spotLight[ 0 ].ambient.r + redParam;
        }



        if( spotLight[ 0 ].ambient.g >= 0.95f && greenParam > 0.0f )
        {
            spotLight[ 0 ].ambient.g = 1.0f;
        }
        else if( spotLight[ 0 ].ambient.g <= 0.05f && greenParam < 0.0f )
        {
            spotLight[ 0 ].ambient.g = 0.0f;
        }
        else
        {
            spotLight[ 0 ].ambient.g = spotLight[ 0 ].ambient.g + greenParam;
        }



        if( spotLight[ 0 ].ambient.b >= 0.95f && blueParam > 0.0f )
        {
            spotLight[ 0 ].ambient.b = 1.0f;
        }
        else if( spotLight[ 0 ].ambient.b <= 0.05f && blueParam < 0.0f )
        {
            spotLight[ 0 ].ambient.b = 0.0f;
        }
        else
        {
            spotLight[ 0 ].ambient.b = spotLight[ 0 ].ambient.b + blueParam;
        }   
    }
}


void Graphics::changeModelRegistryIndex( int i )
{

    modelIndex = modelIndex + i;
    if( modelIndex >= (int)modelRegistry.size() )
    {
        modelIndex = 0;
    } 
    if( modelIndex <= -1 )
    {
        modelIndex = modelRegistry.size() - 1;
    } 

    std::cout << modelRegistry[ modelIndex ].modelPath << std::endl;
}

void Graphics::chanceSpotLightSize( float increment )
{
    if( spotLight.empty( ) )
    {
        return;
    }

    spotLight[ 0 ].coneAngle += increment;

    std::cout << "Spot Light Angle: " << spotLight[ 0 ].coneAngle << std::endl;

    if( spotLight[ 0 ].coneAngle < 0.0f )
    {
        spotLight[ 0 ].coneAngle = 0.0f;
    }
    else if( spotLight[ 0 ].coneAngle > 360.0f )
    {
        spotLight[ 0 ].coneAngle = 360.0f;
    }

    spotLight[ 0 ].cosine = glm::cos( glm::radians( spotLight[ 0 ].coneAngle ) );
}


//gets the location of the shader programs uniforms
bool Graphics::linkToCurrentShaderProgram( )
{
    unsigned int index;
    GLint tmpTextLoc;
    std::ostringstream strStream;


	m_time = shaderRegistry[ shaderSelect ].GetUniformLocation( "time" );

	if( m_time == -1 )
	{
		printf( "time not found!\n" );
		return false;
	}

	tmpTextLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( "waveMap" );

	oceanHeightMap.HeightMapUniform( ) = tmpTextLoc;

	if( tmpTextLoc == -1 )
	{
		printf( "waveMap not found\n" );
		return false;
	}

	m_objectType = shaderRegistry[ shaderSelect ].GetUniformLocation( "typeOfObject" );
	if( m_objectType == -1 )
	{
		printf( "object type code not found\n" );
		return false;
	}

    m_numLights = shaderRegistry[ shaderSelect ].GetUniformLocation( "numberOfLights" );

    if( m_numLights == -1 )
    {
        printf( "number of lights not found\n" );
        return false;
    }

    m_numSpotLights = shaderRegistry[ shaderSelect ].GetUniformLocation( "numberOfSpotLights" );

    if( m_numSpotLights == -1 )
    {
        printf( "number of spot lights not found\n" );
        return false;
    }
    

    for( index = 0; index < spotLight.size( ); index++ )
    {
        strStream << "spotLight[" << index << "].position";

        spotLight[ index ].followLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( spotLight[ index ].followLoc == -1 )
        {
            printf( "spot light follow loc not found\n" );
            return false;
        }

        strStream << "spotLight[" << index << "].ambient";

        spotLight[ index ].ambientLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( spotLight[ index ].ambientLoc == -1 )
        {
            printf( "spot light ambient loc not found\n" );
            return false;
        }

        strStream << "spotLight[" << index << "].direction";
        spotLight[ index ].incomingLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );
        strStream.str( std::string( ) );

        if( spotLight[ index ].incomingLoc == -1 )
        {
            printf( "spot light direction not found\n" );
            return false;
        }

        strStream << "spotLight[" << index << "].clip";
        spotLight[ index ].cosineLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );
        strStream.str( std::string( ) );

        if( spotLight[ index ].cosineLoc == -1 )
        {
            printf( "spot light clip cosine not found\n" );
            return false;
        }
    }

    

    m_diffuse = shaderRegistry[ shaderSelect ].GetUniformLocation( "DiffuseColor" );
    if( m_diffuse == -1 )
    {
        printf( "diffuse color not found\n" );
        return false;
    }

    m_specular = shaderRegistry[ shaderSelect ].GetUniformLocation( "SpecularColor" );
    if( m_specular == -1 )
    {
        printf( "specular color not found\n" );
        return false;
    }

    m_shininess = shaderRegistry[ shaderSelect ].GetUniformLocation( "Shininess" );
    if( m_shininess == -1 )
    {
        printf( "shininess not found\n" );
        return false;
    }

    for( index = 0; index < lights.size( ); index++ )
    {
        strStream << "light[" << index << "].ambient";

        lights[ index ].ambientLoc  = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( lights[ index ].ambientLoc == -1 )
        {
            printf( "light ambient not found\n" );
            return false;
        }

        strStream << "light[" << index << "].position";

        lights[ index ].incomingLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( lights[ index ].incomingLoc == -1 )
        {
            printf( "light position not found\n" );
            return false;
        }
    }    

    // Locate the projection matrix in the shader
    m_projectionMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "projectionMatrix" );
    if( m_projectionMatrix == -1 )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    // Locate the view matrix in the shader
    m_viewMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "viewMatrix" );
    if( m_viewMatrix == -1 )
    {
        printf( "m_viewMatrix not found\n" );
        return false;
    }

    // Locate the model matrix in the shader
    m_modelMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "modelMatrix" );
    if( m_modelMatrix == -1 )
    {
        printf( "m_modelMatrix not found\n" );
        return false;
    }

    tmpTextLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( "textureSampler" );

    if( tmpTextLoc == -1 )
    {
        printf( "texture location uniform not found\n" );
        return false;
    }

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        objectRegistry[ index ]
            .getObjectModel( ).TextureUniformLocation( ) = tmpTextLoc;
    }

    return true;
}

void Graphics::togglePausedState( )
{
    if( !gameStarted){
        return;
    }
    playingStateFlag = !playingStateFlag;
    pauseNotifier = false;
}

void Graphics::startGame( )
{
    if( !playingStateFlag )
    {
		if( activeIdleState )
		{
			resetView( );
		}

        playingStateFlag = true;
        pauseNotifier = false;
		activeIdleState = false;

		score = 0;
		numberOfBalls = 3;

        gameStarted = true;

    }
}

void Graphics::resetBall( )
{
    btTransform currPos;
    btVector3 change;

    returnBall = false;

    if( numberOfBalls <= 1 )
    {

        playingStateFlag = false;
		activeIdleState = true;

        std::cout << "Game over!" << std::endl;
        std::cout << "Press space to restart the game." << std::endl;

		gameOverStep = true;
        gameStarted = false;
    }
	else
	{
		gameOverStep = false;
	}

	numberOfBalls -= 1;

    if( objectRegistry.getSize( ) > ballIndex && !objectRegistry[ ballIndex ].CollisionInfo( ).empty( ) )
    {
        currPos = objectRegistry[ ballIndex ].CollisionInfo( ).rigidBody->getWorldTransform( );

        change = btVector3( objectRegistry[ ballIndex ].getTransVec( ).x,
                            objectRegistry[ ballIndex ].getTransVec( ).y,
                            objectRegistry[ ballIndex ].getTransVec( ).z ) - currPos.getOrigin( );

        objectRegistry[ ballIndex ].CollisionInfo( ).rigidBody->translate( change );

        objectRegistry[ ballIndex ].CollisionInfo( ).rigidBody->setLinearVelocity( btVector3( 0, 0, 0 ) );
        objectRegistry[ ballIndex ].CollisionInfo( ).rigidBody->setAngularVelocity( btVector3( 0, 0, 0 ) );
    }
}

//implement the code for turning the paddle here and in the engine class
//when calling this function pass in Graphics::LEFT_PADDLE for the left paddle
//and Graphics::RIGHT_PADDLE for the right paddle

void Graphics::turnPaddle( bool select )
{
    if( !playingStateFlag )
    {
        if( pauseNotifier )
        {
            return;
        }

        pauseNotifier = true;
        std::cout << std::endl;
        std::cout << "The game is either paused or has not begun!" << std::endl;
        std::cout << "Press the space bar to start the game." << std::endl;
        std::cout << "Press p to unpause the game." << std::endl;
        std::cout << std::endl;
        return;
    }

    if( select == LEFT_PADDLE )
    {
        leftPaddleUp = true; 


    }
    else if( select == RIGHT_PADDLE )
    {
        rightPaddleUp = true;
    }
}

void Graphics::resetView( )
{
    m_camera->resetView( );
}

void Graphics::idleSplash( unsigned int dt )
{
	if( numberOfUpCalls < 1500 && goingUp )
	{
		m_camera->moveUp( );
		numberOfUpCalls += dt;
	}
	else if( numberOfUpCalls > -1500 && goingRight )
	{
		m_camera->moveDown( );
		numberOfUpCalls -= dt;

		goingUp = false;
	}
	else
	{
		goingUp = true;
	}

	if( numberOfRightCalls < 8000 && goingRight )
	{
		m_camera->moveRight( );
		numberOfRightCalls += dt;
	}
	else if( numberOfRightCalls > -8000 && goingUp )
	{
		m_camera->moveLeft( );
		numberOfRightCalls -= dt;
		goingRight = false;
	}
	else
	{
		goingRight = true;
	}
}

void Graphics::turnOffSplash( )
{
	activeIdleState = false;
}

void Graphics::moveShip( size_t ship )
{
	shipRegistry[ ship ].slowDown = false;
	shipRegistry[ ship ].shipReversed = false;
	shipRegistry[ ship ].forceOn = true;
}

void Graphics::rotateShip( size_t ship, float torque )
{
	if( ship < shipRegistry.size( ) 
		&& !shipRegistry[ ship ].shipReversed )
	{
		shipRegistry[ ship ].torque = btVector3( 0.0f, torque, 0.0f );

		shipRegistry[ ship ].slowRotDown = false;
		shipRegistry[ ship ].torqueOn = true;

	}
	
}

void Graphics::slowShipToHalt( size_t ship )
{
	if( ship < shipRegistry.size( ) )
	{
		shipRegistry[ ship ].slowDown = true;
		shipRegistry[ ship ].forceOn = false;
		shipRegistry[ ship ].shipReverseCounter = 0;
		std::cout << "Slowing!" << std::endl;
	}
}

void Graphics::reverseShip( size_t ship )
{
	if( ship < shipRegistry.size( ) 
		&& !shipRegistry[ ship ].slowDown 
		&& !shipRegistry[ ship ].torqueOn )
	{
		shipRegistry[ ship ].shipReversed = true;
		shipRegistry[ ship ].forceOn = true;
	}

}

void Graphics::stopShipsRotation( size_t ship )
{
	if( ship < shipRegistry.size( ) )
	{
		shipRegistry[ ship ].slowRotDown = true;
		shipRegistry[ ship ].torqueOn = false;
		std::cout << "Stopping ship rotation!" << std::endl;
	}
}

void Graphics::fireGuns( size_t ship, bool left )
{
	if( ship < shipRegistry.size( ) )
	{
		if( left )
		{
			if( shipRegistry[ ship ].leftReloadTime <= 0 )
			{
				shipRegistry[ ship ].firingLeft = true;
			}			
		}
		else
		{
			if( shipRegistry[ ship ].rightReloadTime <= 0 )
			{
				shipRegistry[ ship ].firingRight = true;
			}
		}
		
	}
}

// APPLY SHIP FORCES /////////////////////
/****************************************

@brief applyShipForces

@details handles all of the ship motion
		 and force related management

@param None

@notes Also handles raytesting

****************************************/


void Graphics::applyShipForces( )
{
	Object* shipPtr = NULL;
	btRigidBody* shipBodyPtr = NULL;

	btVector3 rawVelocity;
	btScalar velocity;
	btScalar angVel;

	btVector3 relativeForce;
	btVector3 correctedForce;
	btVector3 shipDirection;
	btMatrix3x3 shipRot;

	float windScalar;

	float angle;

	size_t index, cIndex;

	btVector3 shipPosition;

	btVector3 shipTarget;

	btTransform worldTransform;

	btVector3 hitPosition;

	btCollisionObject hitObject;

	btScalar hitFract;

	btScalar modTrans[ 16 ];

	glm::mat4 glModelMat;

	glm::vec4 glPositionVector;

	for( index = 0; index < shipRegistry.size( ); index++ )
	{
		shipPtr = &objectRegistry[ shipRegistry[ index ].index ];

		if( !shipPtr->CollisionInfo( ).empty( ) )
		{
			shipBodyPtr = shipPtr->CollisionInfo( ).rigidBody;
		}

		if( !shipPtr->CompoundCollisionInfo( ).empty( ) )
		{
			shipBodyPtr = shipPtr->CompoundCollisionInfo( ).rigidBody;
		}

		if( shipBodyPtr != NULL )
		{
			//information about the ships travel

			rawVelocity = shipBodyPtr->getLinearVelocity( );
			velocity = rawVelocity.length( );

			angVel = shipBodyPtr->getAngularVelocity( ).length( );

			shipRot = shipBodyPtr->getWorldTransform( ).getBasis( );

			//check controller flags and alter force accordingly
			if( shipRegistry[ index ].forceOn 
				&& shipRegistry[ index ].torqueOn 
				&& velocity >= ShipController::MAX_SPEED / 1.75f )
			{
				shipRegistry[ index ].slowDown = true;				
			}
			if( shipRegistry[ index ].forceOn
				&& shipRegistry[ index ].torqueOn )
			{
				shipRegistry[ index ].slowDown = false;
				shipRegistry[ index ].force = btVector3( windForce, 0.0f, 0.0f );
			}
			else if( shipRegistry[ index ].torqueOn && velocity >= ShipController::MAX_SPEED / 1.25f )
			{
				shipRegistry[ index ].slowDown = true;
			}
			else if( shipRegistry[ index ].torqueOn )
			{
				shipRegistry[ index ].slowDown = false;
				shipRegistry[ index ].force = btVector3( ShipController::STD_FORCE, 0.0f, 0.0f );
			}
			else if( shipRegistry[ index ].forceOn )
			{
				shipRegistry[ index ].slowDown = false;
				if( shipRegistry[ index ].shipReversed 
					&& shipRegistry[ index ].shipReverseCounter <= 0 )
				{
					shipRegistry[ index ].force = btVector3( ShipController::STD_REVERSE, 0.0f, 0.0f );
					shipRegistry[ index ].shipReverseCounter++;
				}
				else if( !shipRegistry[ index ].shipReversed )
				{
					shipRegistry[ index ].force = btVector3( windForce, 0.0f, 0.0f );
				}
			}
			else if( velocity >= 0.1f )
			{
				shipRegistry[ index ].slowDown = true;
			}

			//dot product for wind power and ray testing

			shipDirection = shipRot * btVector3( 1.0f, 0.0f, 0.0f );

			shipDirection = shipDirection.normalized( );

			windScalar = windDirection.dot( shipDirection );

			//compute the maximum level of force possible based on sail position
			windScalar = std::max( windScalar,
				( float ) std::max( windScalar
									+ glm::cos( glm::radians( 80.0f ) ),
									windScalar
									+ glm::cos( glm::radians( -80.0f ) ) ) );


			windScalar = std::min( windScalar, 1.00f );
			windScalar = std::max( windScalar, -1.00f );

			windScalar = std::max( windScalar, 0.008f );

			angle = glm::acos( windScalar );

			if( angle <= glm::radians( 60.0f ) )
			{
				angle = 0.0f - ( angle );
			}
			else
			{
				angle = 0.0f - glm::radians( 60.0f );
			}

			for( cIndex = 0; cIndex < objectRegistry[ shipRegistry[ index ].index ].getNumberOfChildren( ); cIndex++ )
			{
				if( objectRegistry[
					objectRegistry[
						shipRegistry[ index ].index
					].getChildsWorldID( cIndex )
				].getName( ) == "sail" )
				{
					objectRegistry[ objectRegistry[ shipRegistry[ index ].index ].getChildsWorldID( cIndex ) ].setAngle( angle );
				}

			}

			//turn the ship

			if( angVel >= ShipController::MAX_ROT
				&& sameSign( shipRegistry[ index ].torque.getY( ),
							 shipRegistry[ index ].torqueAcc )
				&& !shipRegistry[ index ].slowRotDown )
			{
				ccb::shipReg[ index ].maxAngSpeed = ShipController::MAX_ROT;
			}
			else if( shipRegistry[ index ].slowRotDown )
			{
				if( angVel > 0.01f )
				{
					if( shipRegistry[ index ].torqueAcc < 0.0f )
					{
						shipRegistry[ index ].torque
							= btVector3( 0.0f, ShipController::MAX_ROT / 1.25f, 0.0f );

					}
					else if( shipRegistry[ index ].torqueAcc > 0.0f )
					{
						shipRegistry[ index ].torque
							= btVector3( 0.0f, -1.0f * ( ShipController::MAX_ROT / 1.25f ), 0.0f );
					}
					else
					{
						shipRegistry[ index ].torque = btVector3( 0.0f, 0.0f, 0.0f );
						std::cout << "Error reducing speed!" << std::endl;
					}

					ccb::shipReg[ index ].maxAngSpeed -= ( ccb::shipReg[ index ].maxAngSpeed / 1.25f );
					shipRegistry[ index ].torqueAcc += shipRegistry[ index ].torque.getY( );
					shipRegistry[ index ].torque = btVector3( 0.0f, 0.0f, 0.0f );

					std::cout << "Slowing Rot" << std::endl;
				}
				else
				{
					shipRegistry[ index ].slowRotDown = false;
					shipRegistry[ index ].torqueAcc = 0.0;
					shipRegistry[ index ].torque = btVector3( 0.0f, 0.0f, 0.0f );
					shipBodyPtr->setAngularVelocity( btVector3( 0.0f, 0.0f, 0.0f ) );
					ccb::shipReg[ index ].maxAngSpeed = ShipController::MAX_ROT;
					std::cout << "Stopped Rot" << std::endl;
				}

			}
			else
			{
				shipRegistry[ index ].torqueAcc += shipRegistry[ index ].torque.getY( );
				shipBodyPtr->applyTorque( shipRegistry[ index ].torque );
				ccb::shipReg[ index ].maxAngSpeed = ShipController::MAX_ROT;
				shipRegistry[ index ].torque = btVector3( 0.0f, 0.0f, 0.0f );
			}

			//move forward or back
			if( velocity > 0.1f 
				&& shipRegistry[ index ].slowDown )
			{
				ccb::shipReg[ index ].maxSpeed -= ( ccb::shipReg[ index ].maxSpeed / 50.0f );
				shipRegistry[ index ].shipReverseCounter = 0;
			}
			else if( shipRegistry[ index ].slowDown )
			{
				std::cout << "Halted!" << std::endl;
				shipBodyPtr->setLinearVelocity( btVector3( 0, 0, 0 ) );
				shipRegistry[ index ].slowDown = false;
				shipRegistry[ index ].shipReversed = false;
			}
			else if( shipRegistry[ index ].force.length( ) > 0 )
			{			

				ccb::shipReg[ index ].maxSpeed = std::max( ShipController::MAX_SPEED * windScalar, 
														   ShipController::MAX_SPEED * 0.33333334f );

				std::cout << "Wind Scalar: " << windScalar << std::endl;

				relativeForce = ( windScalar * shipRegistry[ index ].force );

				correctedForce = shipRot * relativeForce;

				shipRegistry[ index ].force = correctedForce;

				shipBodyPtr->applyCentralImpulse( shipRegistry[ index ].force );
				shipRegistry[ index ].force = btVector3( 0, 0, 0 );
			}

			shipBodyPtr->activate( );



			shipTarget = shipDirection.cross( btVector3( 0, 1, 0 ) );

			worldTransform = shipBodyPtr->getWorldTransform( );

			shipPosition = worldTransform.getOrigin( );

			shipTarget *= 150.0f;

			shipRegistry[ index ].rightHit = shipTarget + shipPosition;
			shipRegistry[ index ].leftHit = -1.0f * shipTarget + shipPosition;

			if( shipRegistry[ index ].firingLeft )
			{
				objectCollidedSound.launchSound( );
				shipRegistry[ index ].leftReloadTime = ShipController::RELOAD_TIME_MS;
			}

			if( shipRegistry[ index ].firingRight )
			{
				objectCollidedSound.launchSound( );
				shipRegistry[ index ].rightReloadTime = ShipController::RELOAD_TIME_MS;
			}

			//raytesting
			if( shipRegistry[ index ].firingRight 
				&& shipRayTest( shipPosition, shipRegistry[ index ].rightHit,
							     hitPosition, hitObject, hitFract ) )
			{

				shipRegistry[ index ].rightHit = hitPosition;

				if( shipRegistry[ index ].firingRight )
				{
					for( cIndex = 0; cIndex < shipRegistry.size( ); cIndex++ )
					{
						if( objectRegistry[ shipRegistry[ cIndex ].index ]
							.CollisionInfo( ).rigidBody != shipBodyPtr &&
							hitObject.getUserPointer( )
							== objectRegistry[ shipRegistry[ cIndex ].index ]
							.CollisionInfo( ).rigidBody->getUserPointer( ) )
						{
							shipRegistry[ cIndex ].healthPoints -= ( 50 * hitFract );
						}
					}
				}
				else
				{
					std::cout << "In Range! " << index <<std::endl;
				}
			}

			if( shipRayTest( shipPosition, shipRegistry[ index ].leftHit,
							             hitPosition, hitObject, hitFract ) )
			{
				shipRegistry[ index ].leftHit = hitPosition;

				if( shipRegistry[ index ].firingLeft )
				{
					for( cIndex = 0; cIndex < shipRegistry.size( ); cIndex++ )
					{
						if( objectRegistry[ shipRegistry[ cIndex ].index ]
							.CollisionInfo( ).rigidBody != shipBodyPtr &&
							hitObject.getUserPointer( )
							== objectRegistry[ shipRegistry[ cIndex ].index ]
							.CollisionInfo( ).rigidBody->getUserPointer( ) )
						{
							shipRegistry[ cIndex ].healthPoints -= ( 50 * hitFract );
						}
					}
				}
				else
				{
					std::cout << "In Range! " << index << std::endl;
				}
			}

			//convert hit positions into OpenGL space

			worldTransform.setIdentity( );
			worldTransform.setOrigin( shipRegistry[ index ].leftHit );
			worldTransform.getOpenGLMatrix( modTrans );
			glModelMat = glm::make_mat4( modTrans );
			glPositionVector = glModelMat * glm::vec4( 0.0, 0.0, 0.0, 1.0 );

			shipRegistry[ index ].leftHit = btVector3( glPositionVector.x, 
													   glPositionVector.y, 
													   glPositionVector.z );

			worldTransform.setIdentity( );
			worldTransform.setOrigin( shipRegistry[ index ].rightHit );
			worldTransform.getOpenGLMatrix( modTrans );
			glModelMat = glm::make_mat4( modTrans );
			glPositionVector = glModelMat * glm::vec4( 0.0, 0.0, 0.0, 1.0 );

			shipRegistry[ index ].rightHit = btVector3( glPositionVector.x,
													    glPositionVector.y,
													    glPositionVector.z );
			

			//we have fired
			shipRegistry[ index ].firingLeft = false;

			shipRegistry[ index ].firingRight = false;



		}

		shipPtr = NULL;
		shipBodyPtr = NULL;
	}
}

bool Graphics::shipRayTest
( 
	const btVector3 & src, 
	const btVector3 & dest, 
	btVector3 & hit,
	btCollisionObject& hitObject,
	btScalar & hitFraction
)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback( src, dest );

	dynamicsWorldPtr->rayTest( src, dest, rayCallback );

	if( rayCallback.hasHit( ) )
	{
		hit = rayCallback.m_hitPointWorld;

		hitObject = *rayCallback.m_collisionObject;

		hitFraction = rayCallback.m_closestHitFraction;

		return true;
	}

	return false;
}

bool Graphics::sameSign( float first, float second )
{
	if( ( first >= 0.0f && second >= 0.0f ) 
		|| ( first < 0.0f && second < 0.0f ) )
	{
		return true;
	}

	return false;
}

void Graphics::updateLeftPaddle( unsigned int dt )
{
    btTransform turn;
    turn.setIdentity();
    btQuaternion quat;  
    btScalar x, y, z;

    objectRegistry[ leftPaddle ].CollisionInfo( ).rigidBody->getWorldTransform().getBasis().getEulerZYX(z, y, x);
    objectRegistry[ leftPaddle ].CollisionInfo( ).rigidBody->getMotionState()->getWorldTransform( turn );
    //std::cout << "This is before: " << y << std::endl;
    if( y >= 1.5f && leftPaddleUp )
    {
        leftPaddleUp = false;
    }
    else if( leftPaddleUp )
    {
        y += dt * M_PI/250;
        //std::cout << "This is after: " << y << std::endl;
        if( y > 1.55 )
        {
         y = 1.5;     
        }
        quat.setEulerZYX( 0, y , 0 );
        turn.setRotation(quat);
        
        objectRegistry[ leftPaddle ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( turn );

    }
    else if( y <= btRadians( 0 ) ) 
    {
        y = btRadians( 0 );
        ///std::cout << "This is after: " << y << std::endl;
        quat.setEulerZYX( 0, y , 0 );
        turn.setRotation(quat);
        
        objectRegistry[ leftPaddle ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( turn );
     }
     else
     {
        y -= dt * M_PI/250;
        //std::cout << "This is after: " << y << std::endl;
        quat.setEulerZYX( 0, y , 0 );
        turn.setRotation(quat);
        
        objectRegistry[ leftPaddle ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( turn );
     }
        

    

}

void Graphics::updateRightPaddle( unsigned int dt )
{
    btTransform turn;
    turn.setIdentity();
    btQuaternion quat;  
    btScalar x, y, z;

    objectRegistry[ rightPaddle ].CollisionInfo( ).rigidBody->getWorldTransform().getBasis().getEulerZYX(z, y, x);
    objectRegistry[ rightPaddle ].CollisionInfo( ).rigidBody->getMotionState()->getWorldTransform( turn );
    //std::cout << "This is before: " << y << std::endl;
    if( y <= -1.5f && rightPaddleUp )
    {
        rightPaddleUp = false;
    }
    else if( rightPaddleUp )
    {
        
        y -= dt * M_PI/250;
        //std::cout << "This is after: " << y << std::endl;
        if( y < -1.55 )
        {
         y = -1.5;
        }
        quat.setEulerZYX( 0, y , 0 );
        turn.setRotation(quat);
        
        objectRegistry[ rightPaddle ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( turn );
    }
    else if( y >= btRadians( 0 ) ) 
    {
        y = btRadians( 0 );
        //std::cout << "This is after: " << y << std::endl;
        quat.setEulerZYX( 0, y , 0 );
        turn.setRotation(quat);
        
        objectRegistry[ rightPaddle ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( turn );
     }
     else
     {
        y += dt * M_PI/250;
        //std::cout << "This is after: " << y << std::endl;
        quat.setEulerZYX( 0, y , 0 );
        turn.setRotation(quat);
        
        objectRegistry[ rightPaddle ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( turn );
     }
        
}











