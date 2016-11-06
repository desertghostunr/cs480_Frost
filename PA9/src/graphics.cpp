#include "graphics.h"
#include <algorithm>

namespace ballCallBack
{

    btRigidBody* ballPtr;

    btScalar maxSpeed;


    void myTickCallback( btDynamicsWorld * world, btScalar timeStep )
    {
        btVector3 velocity;
        btScalar speed;

        if( ballPtr == NULL )
        {
            return;
        }

        velocity = ballPtr->getLinearVelocity( );
        speed = velocity.length( );

        if( speed > maxSpeed )
        {
            velocity *= maxSpeed / speed;
            ballPtr->setLinearVelocity( velocity );
        }
    }

};

Graphics::Graphics()
{
    broadphasePtr = NULL;
    collisionConfigPtr = NULL;
    dispatcherPtr = NULL;
    solverPtr = NULL;
    dynamicsWorldPtr = NULL;

    ballCallBack::ballPtr = NULL;

    boxIndex = ballIndex = 0;
    modelIndex = 0;

    ballCallBack::maxSpeed = 50;

    shaderSelect = 0;
}

Graphics::~Graphics()
{
    unsigned int index = 0;

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( dynamicsWorldPtr != NULL )
        {
            if( objectRegistry[ index ].TableCollider( ).rigidBody != NULL )
            {
                dynamicsWorldPtr->removeRigidBody( objectRegistry[ index ].TableCollider( ).rigidBody );

                delete objectRegistry[ index ].TableCollider( ).rigidBody->getMotionState( );
                delete objectRegistry[ index ].TableCollider( ).rigidBody;
                objectRegistry[ index ].TableCollider( ).rigidBody = NULL;
            }

            if( objectRegistry[ index ].TableCollider( ).collisionShape != NULL )
            {
                delete objectRegistry[ index ].TableCollider( ).collisionShape;
                objectRegistry[ index ].TableCollider( ).collisionShape = NULL;
            }

            
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
    GLint tmpTextLoc;

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
        
        objectRegistry.addObject( );
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
    
    incomingLights.resize( progInfo.lights.size( ) );
    ambient.resize( progInfo.lights.size( ) );
    for( index = 0; index < std::min( ( int )progInfo.lights.size( ), 100 ); index++ )
    {
        incomingLights[ index ] = progInfo.lights[ index ];
        ambient[index] = progInfo.ambient[index];
    }

    spotLight = progInfo.spotLight;
    spotLight.coneAngle = glm::cos( glm::radians( spotLight.coneAngle ) );

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


    m_spotLight = shaderRegistry[ shaderSelect ].GetUniformLocation( "lightPosition" );
    if( m_spotLight == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_sAmbient = shaderRegistry[ shaderSelect ].GetUniformLocation( "sAmbient" );
    if( m_sAmbient == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_lightDir = shaderRegistry[ shaderSelect ].GetUniformLocation( "lightDir" );
    if( m_lightDir == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_clipCosine = shaderRegistry[ shaderSelect ].GetUniformLocation( "clip" );
    if( m_clipCosine == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_diffuse = shaderRegistry[ shaderSelect ].GetUniformLocation( "DiffuseColor" );
    if( m_diffuse == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_specular = shaderRegistry[ shaderSelect ].GetUniformLocation( "SpecularColor" );
    if( m_specular == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_shininess = shaderRegistry[ shaderSelect ].GetUniformLocation( "Shininess" );
    if( m_shininess == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_ambient = shaderRegistry[ shaderSelect ].GetUniformLocation( "AmbientColor" );
    if( m_ambient == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    m_light = shaderRegistry[ shaderSelect ].GetUniformLocation( "LightArray" );
    if( m_light == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
        return false;
    }

    // Locate the projection matrix in the shader
    m_projectionMatrix = shaderRegistry[shaderSelect].GetUniformLocation("projectionMatrix");
    if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) 
    {
        printf("m_projectionMatrix not found\n");
        return false;
    }

    // Locate the view matrix in the shader
    m_viewMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation("viewMatrix");
    if (m_viewMatrix == INVALID_UNIFORM_LOCATION) 
    {
        printf("m_viewMatrix not found\n");
        return false;
    }

    // Locate the model matrix in the shader
    m_modelMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation("modelMatrix");
    if (m_modelMatrix == INVALID_UNIFORM_LOCATION) 
    {
        printf("m_modelMatrix not found\n");
        return false;
    }

    tmpTextLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( "textureSampler" );

    if( tmpTextLoc == INVALID_UNIFORM_LOCATION )
    {
        printf( "texture location uniform not found!!!\n" );
        return false;
    }

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        objectRegistry[ index ]
            .getObjectModel( ).TextureUniformLocation( ) = tmpTextLoc;
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
    dynamicsWorldPtr->setGravity( btVector3( 0.0, -9.5, 2.5 ) );
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

            rigidBodyConstruct.m_restitution = 1.25f;
            rigidBodyConstruct.m_friction = 1.0f;
  

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setLinearFactor( btVector3( 1, 0, 1 ) );

            ballCallBack::ballPtr = tmpRigidBody;
            
        }
        else if( objectRegistry[ index ].getName( ) == "cylinder" )
        {
            tmpShapePtr = new btCylinderShape( btVector3( objectRegistry[ index ].getBScale( ).y, ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f ) );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 1.0f;
            rigidBodyConstruct.m_friction = 1.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );
            
        }
        else if( objectRegistry[ index ].getName( ) == "cube" )
        {
            boxIndex = index;

            tmpShapePtr = new btBoxShape( btVector3( ( objectRegistry[ index ].getBScale( ).x / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).y / 2.0f ) + 1.0f, ( objectRegistry[ index ].getBScale( ).z / 2.0f ) + 1.0f ) );


            tmpMotionState = new btDefaultMotionState( btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( objectRegistry[ index ].getTransVec( ).x, objectRegistry[ index ].getTransVec( ).y, objectRegistry[ index ].getTransVec( ).z ) ) );

            mass = 0;

            inertia = btVector3( 0, 0, 0 );

            tmpShapePtr->calculateLocalInertia( mass, inertia );


            btRigidBody::btRigidBodyConstructionInfo rigidBodyConstruct( mass, tmpMotionState, tmpShapePtr, inertia );

            rigidBodyConstruct.m_restitution = 0.5f;
            rigidBodyConstruct.m_friction = 1.0f;

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            tmpRigidBody->setCollisionFlags( tmpRigidBody->getCollisionFlags( ) | btCollisionObject::CF_KINEMATIC_OBJECT );
            tmpRigidBody->setActivationState( DISABLE_DEACTIVATION );
        }
        else if( objectRegistry[ index ].getName( ) == "table" )
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

            rigidBodyConstruct.m_restitution = 1.0f;
            rigidBodyConstruct.m_friction = 1.0f;
            

            tmpRigidBody = new btRigidBody( rigidBodyConstruct );

            objectRegistry[ index ].TableCollider( ).collisionShape = tmpCompoundShape;
            objectRegistry[ index ].TableCollider( ).rigidBody = tmpRigidBody;


        }
        else
        {
            std::cout << "Error: Unexpected Object Name!!!" << std::endl;
            return false;
        }


        if( objectRegistry[ index ].getName( ) != "table" )
        {
            objectRegistry[ index ].CollisionInfo( ).collisionShape = tmpShapePtr;
            objectRegistry[ index ].CollisionInfo( ).rigidBody = tmpRigidBody;
        }        

        dynamicsWorldPtr->addRigidBody( tmpRigidBody );

        tmpCompoundShape = NULL;
        tmpShapePtr = NULL;
        tmpMotionState = NULL;
        tmpRigidBody = NULL;
    }

    dynamicsWorldPtr->setInternalTickCallback( ballCallBack::myTickCallback );

    return true;
}

void Graphics::Update(unsigned int dt)
{
    unsigned int index;

    dynamicsWorldPtr->stepSimulation( dt, 10 );


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
    glClearColor(0.6, 0.6, 0.6, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the correct program
    shaderRegistry[ shaderSelect ].Enable( );

    // Send in the projection and view to the shader
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView())); 

    glUniform4f( m_ambient, ambient[0].r, ambient[0].g, ambient[0].b, ambient[0].a );
    glUniform4f( m_light, incomingLights[ 0 ].r, incomingLights[ 0 ].g, incomingLights[ 0 ].b, incomingLights[ 0 ].a );

    tmpMat = objectRegistry[ ballIndex ].GetModel( );

    tmpVec = tmpMat * glm::vec4( 0.0, 0.0, 0.0, 1.0 );

    glUniform4f( m_spotLight, tmpVec.x + 0.0, tmpVec.y + 80.0, tmpVec.z - 50, 1.0 );
    glUniform4f( m_sAmbient, spotLight.ambient.r, spotLight.ambient.g, spotLight.ambient.b, spotLight.ambient.a );
    glUniform3f( m_lightDir, spotLight.incoming.r, spotLight.incoming.g, spotLight.incoming.b );
    glUniform1f( m_clipCosine, spotLight.coneAngle );

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

    

    if( ( objectID > objectRegistry.getSize( ) ) )
    {
         return false;
    }

    //here is an example for the general call of transforms for a planet
    //This does the basic operations... I still don't know how to tilt orbits
    //Play around with changing the order of events / things
    // write transforms here //////////////////////////

    objectRegistry[ objectID ].commitScale( );

    if( !objectRegistry[ objectID ].CollisionInfo( ).empty( ) )
    {
        objectRegistry[ objectID ].CollisionInfo( ).rigidBody->getMotionState( )->getWorldTransform( trans );

        trans.getOpenGLMatrix( modTrans );

        objectRegistry[ objectID ].setBulletTransform( glm::make_mat4( modTrans ) );

        objectRegistry[ objectID ].commitBulletTransform( );
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
   m_camera->updateLookAt();
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
   m_camera->updateLookAt();
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

void Graphics::moveBox( glm::vec3 pos )
{
    btTransform currPos;
    btVector3 change;

    if( objectRegistry.getSize( ) > boxIndex && !objectRegistry[ boxIndex ].CollisionInfo( ).empty( ) )
    {
        objectRegistry[ boxIndex ].CollisionInfo( ).rigidBody->getMotionState( )->getWorldTransform( currPos );

        change = currPos.getOrigin( ) + btVector3( pos.x, 0, pos.z );

        change.setX( std::min( change.getX( ), boxEdges.r - objectRegistry[ boxIndex ].getBScale( ).x ) );
        change.setX( std::max( change.getX( ), boxEdges.b + objectRegistry[ boxIndex ].getBScale( ).x ) );

        change.setZ( std::min( change.getZ( ), boxEdges.g - objectRegistry[ boxIndex ].getBScale( ).z ) );
        change.setZ( std::max( change.getZ( ), boxEdges.a + objectRegistry[ boxIndex ].getBScale( ).z ) );

        currPos.setOrigin( change );

        objectRegistry[ boxIndex ].CollisionInfo( ).rigidBody->getMotionState( )->setWorldTransform( currPos );

        objectRegistry[ boxIndex ].CollisionInfo( ).rigidBody->translate( btVector3( pos.x, pos.y, pos.z ) );
    }
    
}

void Graphics::cycleShaderProgram( )
{
    unsigned int index;
    GLint tmpTextLoc;

    shaderSelect++;    

    if( shaderSelect >= shaderRegistry.size( ) )
    {
        shaderSelect = 0;
    }

    std::cout << "Shader Program " << shaderSelect + 1<< " selected." << std::endl;


    m_spotLight = shaderRegistry[ shaderSelect ].GetUniformLocation( "lightPosition" );
    if( m_spotLight == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_sAmbient = shaderRegistry[ shaderSelect ].GetUniformLocation( "sAmbient" );
    if( m_sAmbient == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_lightDir = shaderRegistry[ shaderSelect ].GetUniformLocation( "lightDir" );
    if( m_lightDir == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_clipCosine = shaderRegistry[ shaderSelect ].GetUniformLocation( "clip" );
    if( m_clipCosine == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_diffuse = shaderRegistry[ shaderSelect ].GetUniformLocation( "DiffuseColor" );
    if( m_diffuse == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_specular = shaderRegistry[ shaderSelect ].GetUniformLocation( "SpecularColor" );
    if( m_specular == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_shininess = shaderRegistry[ shaderSelect ].GetUniformLocation( "Shininess" );
    if( m_shininess == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_ambient = shaderRegistry[ shaderSelect ].GetUniformLocation( "AmbientColor" );
    if( m_ambient == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    m_light = shaderRegistry[ shaderSelect ].GetUniformLocation( "LightArray" );
    if( m_light == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    // Locate the projection matrix in the shader
    m_projectionMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "projectionMatrix" );
    if( m_projectionMatrix == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_projectionMatrix not found\n" );
    }

    // Locate the view matrix in the shader
    m_viewMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "viewMatrix" );
    if( m_viewMatrix == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_viewMatrix not found\n" );
    }

    // Locate the model matrix in the shader
    m_modelMatrix = shaderRegistry[ shaderSelect ].GetUniformLocation( "modelMatrix" );
    if( m_modelMatrix == INVALID_UNIFORM_LOCATION )
    {
        printf( "m_modelMatrix not found\n" );
    }

    tmpTextLoc = shaderRegistry[ shaderSelect ].GetUniformLocation( "textureSampler" );

    if( tmpTextLoc == INVALID_UNIFORM_LOCATION )
    {
        printf( "texture location uniform not found!!!\n" );
    }

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        objectRegistry[ index ]
            .getObjectModel( ).TextureUniformLocation( ) = tmpTextLoc;
    }
}

void Graphics::changeBrightness( std::string lightSelect, float redParam, 
                               float greenParam, float blueParam )
{
    if( lightSelect == "ambient" )
    {
        if( ambient[0].r >= 0.95f && redParam > 0.0f )
        {
            ambient[0].r = 1.0f;
        }
        else if( ambient[0].r <= 0.05f && redParam < 0.0f )
        {
            ambient[0].r = 0.0f;
        }
        else
        {
            ambient[0].r = ambient[0].r + redParam;
        }



        if( ambient[0].g >= 0.95f && greenParam > 0.0f )
        {
            ambient[0].g = 1.0f;
        }
        else if( ambient[0].g <= 0.05f && greenParam < 0.0f )
        {
            ambient[0].g = 0.0f;
        }
        else
        {
            ambient[0].g = ambient[0].g + greenParam;
        }



        if( ambient[0].b >= 0.95f && blueParam > 0.0f )
        {
            ambient[0].b = 1.0f;
        }
        else if( ambient[0].b <= 0.05f && blueParam < 0.0f )
        {
            ambient[0].b = 0.0f;
        }
        else
        {
            ambient[0].b = ambient[0].b + blueParam;
        }
    }


    else if( lightSelect == "specular" )
    {
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


    else if( lightSelect == "spot" )
    {
        
    } 


}


void Graphics::changeModelRegistryIndex( int i )
{

    modelIndex = modelIndex + i;
    if( modelIndex >= modelRegistry.size() )
    {
        modelIndex = 0;
    } 
    if( modelIndex <= -1 )
    {
        modelIndex = modelRegistry.size() - 1;
    } 
}














