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

    unsigned int index;
    GLint tmpTextLoc;

    // Init Camera
    m_camera = new Camera();
    if(!m_camera->Initialize(width, height))
    {
        printf("Camera Failed to Initialize\n");
        return false;
    }

    // Create the object
    objectRegistry.addObject( );
    
    if( !objectRegistry[ 0 ].Initialize( progInfo.modelVector[ 0 ] ) )
    {
        std::cout<<"Failed to load "<<progInfo.modelVector[ 0 ]<<"."<<std::endl;
        return false;
    }

    objectRegistry[ 0 ].setRotationVector( glm::vec3( 1.0f, 1.0f, -1.0f ) );

    objectRegistry[ 0 ].updateRotationRate( 0.1f );

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
            objectRegistry[ index ].setScale( glm::vec3( 2.5, 2.5, 2.5 ) );
            objectRegistry[ index ].commitScale( );
            objectRegistry[ index ].incrementAngle( dt );

            objectRegistry[ index ].commitRotation( );

            objectRegistry[ index ].Update( dt );

        }
        
    }
}

void Graphics::Render()
{
    unsigned int index;

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
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
