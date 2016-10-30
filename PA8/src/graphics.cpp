#include "graphics.h"


Graphics::Graphics()
{
    broadphasePtr = NULL;
    collisionConfigPtr = NULL;
    dispatcherPtr = NULL;
    solverPtr = NULL;
    dynamicsWorldPtr = NULL;
}

Graphics::~Graphics()
{
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
    unsigned int index, pIndex;
    GLint tmpTextLoc;

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

    }

    for( pIndex = 0; pIndex < objectRegistry.getSize( ); pIndex++ )
    {

        for( index = 0; index < progInfo.objectData[ pIndex ].childID.size( ); index++ )
        {
            objectRegistry.setChild( progInfo.objectData[ pIndex ].childID[ index ], pIndex );
        }


    }

    // Set up the shaders
    m_shader = new Shader();
    if(!m_shader->Initialize())
    {
        printf("Shader Failed to Initialize\n");
        return false;
    }
    
    for( index = 0; index < progInfo.shaderVector.size(); index++ )
    {
        if(!m_shader->AddShader( progInfo.shaderVector[index].first, 
                                 progInfo.shaderVector[index].second ) )
        {
            if( progInfo.shaderVector[index].first == GL_VERTEX_SHADER )
            {
                printf("Vertex Shader failed to Initialize\n");
            }
            else
            {
                printf("Fragment Shader failed to Initialize\n");
            }
            return false;
        }
    }

    // Connect the program
    if(!m_shader->Finalize())
    {
        printf("Program to Finalize\n");
        return false;
    }

    // Locate the projection matrix in the shader
    m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
    if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) 
    {
        printf("m_projectionMatrix not found\n");
        return false;
    }

    // Locate the view matrix in the shader
    m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
    if (m_viewMatrix == INVALID_UNIFORM_LOCATION) 
    {
        printf("m_viewMatrix not found\n");
        return false;
    }

    // Locate the model matrix in the shader
    m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
    if (m_modelMatrix == INVALID_UNIFORM_LOCATION) 
    {
        printf("m_modelMatrix not found\n");
        return false;
    }

    tmpTextLoc = m_shader->GetUniformLocation( "textureSampler" );

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
    dynamicsWorldPtr->setGravity( btVector3( 0.0, -9.8, 0.0 ) );
    ///////////////////////////////////////////////////////////////////

    return true;
}

void Graphics::Update(unsigned int dt)
{
    unsigned int index;

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

    //clear the screen
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Start the correct program
    m_shader->Enable();

    // Send in the projection and view to the shader
    glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
    glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView())); 

    // Render the objects
    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        glUniformMatrix4fv( m_modelMatrix, 1, GL_FALSE,
                            glm::value_ptr(objectRegistry[index].GetModel()));
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
    if( ( objectID > objectRegistry.getSize( ) ) )
    {
         return false;
    }

    //here is an example for the general call of transforms for a planet
    //This does the basic operations... I still don't know how to tilt orbits
    //Play around with changing the order of events / things
    // write transforms here //////////////////////////

    objectRegistry[ objectID ].commitScale( );

    objectRegistry[ objectID ].incrementAngle( dt );    

    objectRegistry[ objectID ].commitRotation( );

    objectRegistry[ objectID ].commitParentLocation( );    

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

