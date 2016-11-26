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


//physics related callbacks
namespace ccb
{

    btRigidBody* ballPtr;

    btScalar maxSpeed;


    void TickCallback( btDynamicsWorld * world, btScalar timeStep )
    {
        btVector3 velocity, angularVelocity;
        btScalar speed;

        if( ballPtr == NULL )
        {
            return;
        }

        velocity = ballPtr->getLinearVelocity( );
        angularVelocity = ballPtr->getAngularVelocity( );

        speed = velocity.length( );

        if( speed > maxSpeed )
        {
            velocity *= maxSpeed / speed;
            ballPtr->setLinearVelocity( velocity );
        }

        speed = angularVelocity.length( );

        if( speed > maxSpeed )
        {
            angularVelocity *= maxSpeed / speed;
            ballPtr->setAngularVelocity( angularVelocity );
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

    ccb::ballPtr = NULL;

    boxIndex = -1;
    ballIndex = 0;
    modelIndex = 0;

    ccb::maxSpeed = 50;

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
    const GraphicsInfo& progInfo
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
    // Init Camera
    m_camera = new Camera();
    cameraTracking = false;
    if(!m_camera->Initialize(width, height))
    {
        printf("Camera Failed to Initialize\n");
        return false;
    }
	
    score = 0;

    objectCollidedSound.loadSound("sounds/bumpSound.wav");

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
        if( objectRegistry[ index ].getName( ) == "ball" )
        {
            ballIndex = index;

            tmpShapePtr = new btSphereShape( ( objectRegistry[ index ].getBScale( ).x /2.0f ) + 1.0f );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );
            
            mass = 1;

            inertia = btVector3( 0, 0, 0 );


            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 1.0f;
            rigidBodyConstruct.m_friction = 1.0f;
  

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setLinearFactor( btVector3( 1, 0, 1 ) );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

            ccb::ballPtr = tmpRigidBody;
            
        }
        else if( objectRegistry[ index ].getName( ) == "bumber" )
        {
            tmpShapePtr = new btCylinderShape( btVector3( ( objectRegistry[ index ].getBScale( ).y / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f ) );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 1.5f;
            rigidBodyConstruct.m_friction = 0.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );
            
        }
        else if( objectRegistry[ index ].getName( ) == "bumberSquare" )
        {
            tmpShapePtr = new btBoxShape( btVector3( ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).y / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).z / 2.0f ) + 1.0f ) );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 1.0f;
            rigidBodyConstruct.m_friction = 1.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

        }
        else if( objectRegistry[ index ].getName( ) == "ballReturn" )
        {
            tmpShapePtr = new btBoxShape( btVector3( ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).y / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).z / 2.0f ) - 1.0f ) );

            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z - 5.5f ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );

            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 0.0f;
            rigidBodyConstruct.m_friction = 1.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

        }
        else if( objectRegistry[ index ].getName( ) == "paddleR" 
                 || objectRegistry[ index ].getName( ) == "paddleL" )
        {
            

            //this is where paddle indices are assigned
            if( objectRegistry[ index ].getName( ) == "paddleR" )
            {
                rightPaddle = index;
            }
            else
            {
                leftPaddle = index;
            }

            tmpShapePtr = new btBoxShape( btVector3( ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).y / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).z / 2.0f ) + 1.0f ) );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 100;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 1.0f;
            rigidBodyConstruct.m_friction = 1.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_KINEMATIC_OBJECT );
            tmpRigidBody->setActivationState( DISABLE_DEACTIVATION );
        }

        else if( objectRegistry[ index ].getName( ) == "plunger" )
        {


            //this is where plunger indices are assigned
            boxIndex = index;
            
            tmpShapePtr = new btBoxShape( btVector3( ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).y / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).z / 2.0f ) + 1.0f ) );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 100;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 2.5f;
            rigidBodyConstruct.m_friction = 0.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_KINEMATIC_OBJECT );
            tmpRigidBody->setActivationState( DISABLE_DEACTIVATION );
        }
        else if( objectRegistry[ index ].getName( ) == "ocean" )
        {
            tmpCompoundShape = new btCompoundShape( );

            transform.setIdentity( );
            transform.setOrigin( btVector3( 0, -1 * objectRegistry[ index ].getBScale( ).y / 2, 0 ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( 0, 1, 0 ), 10 );

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            transform.setIdentity( );
            transform.setOrigin( btVector3( 0, 1 * objectRegistry[ index ].getBScale( ).y / 2, 0 ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( 0, -1, 0 ), 10);

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            transform.setIdentity( );

            boxEdges.a = -1 * objectRegistry[ index ].getBScale( ).z / 2;

            transform.setOrigin( btVector3( 0, 0, boxEdges.a ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( 0, 0, 1 ), 10 );

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            transform.setIdentity( );

            boxEdges.g = 1 * objectRegistry[ index ].getBScale( ).z / 2;

            transform.setOrigin( btVector3( 0, 0, boxEdges.g ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( 0, 0, -1 ), 10 );

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            transform.setIdentity( );

            boxEdges.r = 1 * objectRegistry[ index ].getBScale( ).x / 2;

            transform.setOrigin( btVector3( boxEdges.r, 0, 0 ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( -1, 0, 0 ), 10 );

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            transform.setIdentity( );

            boxEdges.b = -1 * objectRegistry[ index ].getBScale( ).x / 2;

            transform.setOrigin( btVector3( boxEdges.b, 0, 0 ) );

            tmpShapePtr = new btStaticPlaneShape( btVector3( 1, 0, 0 ), 10);

            tmpCompoundShape->addChildShape( transform, tmpShapePtr );

            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpCompoundShape, inertia );

            rigidBodyConstruct.m_restitution = 0.95f;
            rigidBodyConstruct.m_friction = 1.25f;
            

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            objectRegistry[ index ].CompoundCollisionInfo( ).collisionShape = tmpCompoundShape;
            objectRegistry[ index ].CompoundCollisionInfo( ).rigidBody = tmpRigidBody;


        }
		else if( objectRegistry[ index ].getName( ) == "sky" )
		{
			//do nothing
		}
        else
        {
            std::cout << "Error: Unexpected Object Name!!!" << std::endl;
            return false;
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
        //put step rotation function here ////
        updateLeftPaddle( dt );
        updateRightPaddle( dt );
        /////////////////////////////////////

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
}

void Graphics::Render()
{
    unsigned int index;

    glm::vec4 tmpVec;

    glm::mat4 tmpMat;

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

    for( index = 0; 
         index < std::min( numberOfSpotLights, (unsigned int )spotLight.size( ) ); 
         index++ )
    {

        tmpMat = objectRegistry[ spotLight[ index ].oTFIndex ].GetModel( );

        tmpVec = tmpMat * glm::vec4( 0.0, 0.0, 0.0, 1.0 );

        glUniform4f( spotLight[ index ].followLoc, tmpVec.x + 0.0, 
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

        objectRegistry[index].Render();
    }

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

    ccb::ScoreContactResultCallback cylinderCallBack( &score );
    ccb::TrapContactResultCallback returnBallCallBack( &returnBall );
    

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

        if( ( objectRegistry[ objectID ].getName( ) == "bumber" )
            && playingStateFlag )
        {
            dynamicsWorldPtr->contactPairTest( objectRegistry[ ballIndex ].CollisionInfo( ).rigidBody, 
                                               objectRegistry[ objectID ].CollisionInfo( ).rigidBody, 
                                               cylinderCallBack );
        }

        if( objectRegistry[ objectID ].getName( ) == "ballReturn" && playingStateFlag )
        {
            dynamicsWorldPtr->contactPairTest( objectRegistry[ ballIndex ].CollisionInfo( ).rigidBody,
                                               objectRegistry[ objectID ].CollisionInfo( ).rigidBody,
                                               returnBallCallBack );
        }
    }
	else if( objectRegistry[ objectID ].getObjectType( ) == Object::BASE_OBJECT )
	{
		objectRegistry[ objectID ].commitTranslation( );
		objectRegistry[ objectID ].incrementAngle( dt );
		objectRegistry[ objectID ].commitRotation( );
		
	}

    if( tempScore != score )
    {
        objectCollidedSound.launchSound();
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
             objectRegistry[ childsID ].setOrigin( 
                                       objectRegistry[ objectID ].getOrigin( ) );
            
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


    m_numLights = shaderRegistry[ shaderSelect ].GetUniformLocation( "numberOfLights" );

    if( m_numLights == INVALID_UNIFORM_LOCATION )
    {
        printf( "number of lights not found\n" );
        return false;
    }

    m_numSpotLights = shaderRegistry[ shaderSelect ].GetUniformLocation( "numberOfSpotLights" );

    if( m_numSpotLights == INVALID_UNIFORM_LOCATION )
    {
        printf( "number of spot lights not found\n" );
        return false;
    }
    

    for( index = 0; index < spotLight.size( ); index++ )
    {
        strStream << "spotLight[" << index << "].position";

        spotLight[ index ].followLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( spotLight[ index ].followLoc == INVALID_UNIFORM_LOCATION )
        {
            printf( "spot light follow loc not found\n" );
            return false;
        }

        strStream << "spotLight[" << index << "].ambient";

        spotLight[ index ].ambientLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( spotLight[ index ].ambientLoc == INVALID_UNIFORM_LOCATION )
        {
            printf( "spot light ambient loc not found\n" );
            return false;
        }

        strStream << "spotLight[" << index << "].direction";
        spotLight[ index ].incomingLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );
        strStream.str( std::string( ) );

        if( spotLight[ index ].incomingLoc == INVALID_UNIFORM_LOCATION )
        {
            printf( "spot light direction not found\n" );
            return false;
        }

        strStream << "spotLight[" << index << "].clip";
        spotLight[ index ].cosineLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );
        strStream.str( std::string( ) );

        if( spotLight[ index ].cosineLoc == INVALID_UNIFORM_LOCATION )
        {
            printf( "spot light clip cosine not found\n" );
            return false;
        }
    }

    

    m_diffuse = shaderRegistry[ shaderSelect ].GetUniformLocation( "DiffuseColor" );
    if( m_diffuse == INVALID_UNIFORM_LOCATION )
    {
        printf( "diffuse color not found\n" );
        return false;
    }

    m_specular = shaderRegistry[ shaderSelect ].GetUniformLocation( "SpecularColor" );
    if( m_specular == INVALID_UNIFORM_LOCATION )
    {
        printf( "specular color not found\n" );
        return false;
    }

    m_shininess = shaderRegistry[ shaderSelect ].GetUniformLocation( "Shininess" );
    if( m_shininess == INVALID_UNIFORM_LOCATION )
    {
        printf( "shininess not found\n" );
        return false;
    }

    for( index = 0; index < lights.size( ); index++ )
    {
        strStream << "light[" << index << "].ambient";

        lights[ index ].ambientLoc  = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( lights[ index ].ambientLoc == INVALID_UNIFORM_LOCATION )
        {
            printf( "light ambient not found\n" );
            return false;
        }

        strStream << "light[" << index << "].position";

        lights[ index ].incomingLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( strStream.str( ).c_str( ) );

        strStream.str( std::string( ) );

        if( lights[ index ].incomingLoc == INVALID_UNIFORM_LOCATION )
        {
            printf( "light position not found\n" );
            return false;
        }
    }    

    // Locate the projection matrix in the shader
    m_projectionMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "projectionMatrix" );
    if( m_projectionMatrix == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    // Locate the view matrix in the shader
    m_viewMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "viewMatrix" );
    if( m_viewMatrix == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_viewMatrix not found\n" );
        return false;
    }

    // Locate the model matrix in the shader
    m_modelMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "modelMatrix" );
    if( m_modelMatrix == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_modelMatrix not found\n" );
        return false;
    }

    tmpTextLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( "textureSampler" );

    if( tmpTextLoc == INVALID_UNIFORM_LOCATION )
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











