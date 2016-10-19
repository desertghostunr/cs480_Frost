#include "graphics.h"

Graphics::Graphics()
{
    
}

Graphics::~Graphics()
{

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

    //each planet is at the index of its number - 1
    //mercury is 0, venus is 1, earth is 2, mars is 3, etc.
    planetRegistry = progInfo.planetIndex;


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

    for( pIndex = 0; pIndex < progInfo.planetData.size( ); pIndex++ )
    {
        objectRegistry.addObject( );

        if( progInfo.planetData[ pIndex ].modelID < modelRegistry.size( ) )
        {

            objectRegistry[ objectRegistry.getSize( ) - 1 ]
                .Initialize( &modelRegistry[ progInfo.planetData[ pIndex ].modelID ].objModel );

            modelRegistry[ modelRegistry.size( ) - 1 ].objModel.incrementReference( );
        }
        else
        {
            std::cout << "Invalid model ID provided!" << std::endl;
            return false;
        }

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].setScale( progInfo.planetData[ pIndex ].scale );

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].setTiltAngle( progInfo.planetData[ pIndex ].tilt );

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].updateOrbitRate( progInfo.planetData[ pIndex ].orbitRate );

        objectRegistry[ objectRegistry.getSize( ) - 1 
                      ].updateRotationRate( progInfo.planetData[ pIndex ].rotRate );

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].setOrbitalRadius( progInfo.planetData[ pIndex ].orbitRad );

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].setOrbitDistanceMultiplier( 1.0f );

        objectRegistry[ objectRegistry.getSize( ) - 1 
                      ].setRotationVector( glm::vec3( 0.0f, 1.0f, 0.0f ) );

        objectRegistry[ objectRegistry.getSize( ) - 1
                      ].getOrigin( ).orbitTilt = progInfo.planetData[ pIndex ].orbitTilt;

    }

    for( pIndex = 0; pIndex < objectRegistry.getSize( ); pIndex++ )
    {

        for( index = 0; index < progInfo.planetData[ pIndex ].childID.size( ); index++ )
        {
            objectRegistry.setChild( progInfo.planetData[ pIndex ].childID[ index ], pIndex );
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

    planetIndex = 0;

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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
    m_camera->updateCamera( cameraTracking, objectRegistry[ planetRegistry[ planetIndex ]].getOrigin().translation  );
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

    objectRegistry[ objectID ].commitTilt( );

    objectRegistry[ objectID ].incrementOrbitAngle( dt );

    objectRegistry[ objectID ].commitOrbitalTranslation( );

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

// UPDATE ROTATION //////////////////
/***************************************

@brief updateRotation

@details toggles the direction of rotation

@param in: objectID: the id or index of the Object

@param in: speedOfRotation: controls the speed of rotation

@notes None

***************************************/
bool Graphics::updateRotation( unsigned int objectID, float speedOfRotation )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].updateRotationRate( speedOfRotation );
        return true;
    }
    return false;
}


// UPDATE ORBIT//////////////////
/***************************************

@brief updateOrbit

@details toggles the direction of orbit

@param in: objectID: the id or index of the Object

@param in: speedOfOrbit: controls the speed of orbit

@notes None

***************************************/
bool Graphics::updateOrbit( unsigned int objectID, float speedOfOrbit )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].updateOrbitRate( speedOfOrbit );
        return true;
    }
    return false;
}


// TOGGLE ROTATION DIRECTION //////////////////
/***************************************

@brief toggleRotationDirection

@details toggles the direction the object is rotating and cancels the pause state

@param in: objectID: the id or index of the Object

@notes None

***************************************/
bool Graphics::toggleRotationDirection( unsigned int objectID )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].toggleRotationDirection( );
        return true;
    }
    return false;
}

// TOGGLE ROTATION PAUSED //////////////////
/***************************************

@brief toggleRotationPaused

@details toggles whether or not the rotation is paused

@param in: objectID: the id or index of the Object

@notes None

***************************************/
bool Graphics::toggleRotationPaused( unsigned int objectID )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].toggleRotationPaused( );
        return true;
    }
    return false;
}


// TOGGLE ORBIT DIRECTION //////////////////
/***************************************

@brief toggleOrbitDirection

@details toggles the direction the object is orbiting and cancels the pause state

@param in: objectID: the id or index of the Object

@notes None

***************************************/
bool Graphics::toggleOrbitDirection( unsigned int objectID )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].toggleOrbitDirection( );
        return true;
    }
    return false;
}

// TOGGLE ORBIT PAUSED //////////////////
/***************************************

@brief toggleOrbitPaused

@details toggles whether or not the orbit is paused

@param in: objectID: the id or index of the Object

@notes None

***************************************/
bool Graphics::toggleOrbitPaused( unsigned int objectID )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].toggleOrbitPaused( );
        return true;
    }
    return false;
}

// TOGGLE ALL PAUSED //////////////////
/***************************************

@brief toggleAllPaused

@details toggles whether or not the orbit and rotation is paused

@param in: objectID: the id or index of the Object

@notes None

***************************************/
bool Graphics::toggleAllPaused( unsigned int objectID )
{
    if( objectID < objectRegistry.getSize( ) )
    {
        objectRegistry[objectID].toggleAllPaused( );
        return true;
    }
    return false;
}

// TOGGLE ALL OBJECTS PAUSED //////////////////
/***************************************

@brief toggleAllObjectsPaused

@details toggles whether or not the orbit and rotation is paused for all objects

@param None

@notes None

***************************************/
void Graphics::toggleAllObjectsPaused( )
{
    bool allPausedAtCall = true;
    unsigned int index;

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( !objectRegistry[ index ].isPaused( ) )
        {
            objectRegistry[ index ].toggleAllPaused( );
            allPausedAtCall = false;
        }
    }

    //if some objects weren't paused at function call, 
    //then we've paused all of them
    if( !allPausedAtCall )
    {
        return;
    }

    //however, if they all were paused
    //then we need to unpause them
    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( objectRegistry[ index ].isPaused( ) )
        {
            objectRegistry[ index ].toggleAllPaused( );
        }
    }

}

void Graphics::changeOrbitSpeed( unsigned int opCode )
{
    unsigned int index;

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( opCode == INCREASE_SPEED )
        {
            objectRegistry[ index ].incrementOrbitSpeed( );
        }
        else if( opCode == DECREASE_SPEED  )
        {
            objectRegistry[ index ].decrementOrbitSpeed( );
        }
        else
        {
            objectRegistry[ index ].resetOrbitSpeed( );
        }
        
    }
}

void Graphics::changeRotSpeed( unsigned int opCode )
{
    unsigned int index;

    for( index = 0; index < objectRegistry.getSize( ); index++ )
    {
        if( opCode == INCREASE_SPEED )
        {
            objectRegistry[ index ].incrementRotationSpeed( );
        }
        else if( opCode == DECREASE_SPEED )
        {
            objectRegistry[ index ].decrementRotationSpeed( );
        }
        else
        {
            objectRegistry[ index ].resetRotationSpeed( );
        }

    }
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
   planetIndex = planet;
}

