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
  const std::vector<pair<GLenum, std::string>>& shaderInfo
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

  int index;

  // Init Camera
  m_camera = new Camera();
  if(!m_camera->Initialize(width, height))
  {
    printf("Camera Failed to Initialize\n");
    return false;
  }

  // Create the object
  objectVector.push_back(Object());
  objectVector.push_back(Object());

  //initialize the object's orbit rate 
  objectVector[0].updateOrbitRate(0.35f);
  objectVector[0].setOrbitalRadius(5.5f);
  objectVector[0].updateRotationRate(1.0f);

  objectVector[1].updateOrbitRate(0.4f);
  objectVector[1].setOrbitalRadius(3.5f);
  objectVector[1].updateRotationRate(0.4f);

  objectVector[ 0 ].addChild( 1 );
  objectVector[ 1 ].setChildStatusFlag( true );
  objectVector[ 1 ].setScale( glm::vec3( 0.6f, 0.63f, 0.6f ) );

  // Set up the shaders
  m_shader = new Shader();
  if(!m_shader->Initialize())
  {
    printf("Shader Failed to Initialize\n");
    return false;
  }
  
  for( index = 0; index < shaderInfo.size(); index++ )
  {
    if(!m_shader->AddShader(shaderInfo[index].first, shaderInfo[index].second))
    {
      if(shaderInfo[index].first == GL_VERTEX_SHADER)
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

  //enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

void Graphics::Update(unsigned int dt)
{
  int index;
  // Update the objects
  for( index = 0; index < objectVector.size( ); index++ )
  {

    if( !objectVector[ index ].isChild( ) )
    {
      objectVector[ index ].Update( dt );
      objectVector[ index ].UpdateChildren( dt, objectVector, true );
    }
    
  }
}

void Graphics::Render()
{
  int index;

  //clear the screen
  glClearColor(0.0, 0.0, 0.2, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Start the correct program
  m_shader->Enable();

  // Send in the projection and view to the shader
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView())); 

  // Render the objects
  for( index = 0; index < objectVector.size( ); index++ )
  {
    glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE,
                       glm::value_ptr(objectVector[index].GetModel()));
    objectVector[index].Render();
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
bool Graphics::updateRotation( int objectID, float speedOfRotation )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].updateRotationRate( speedOfRotation );
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
bool Graphics::updateOrbit( int objectID, float speedOfOrbit )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].updateOrbitRate( speedOfOrbit );
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
bool Graphics::toggleRotationDirection( int objectID )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].toggleRotationDirection( );
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
bool Graphics::toggleRotationPaused( int objectID )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].toggleRotationPaused( );
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
bool Graphics::toggleOrbitDirection( int objectID )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].toggleOrbitDirection( );
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
bool Graphics::toggleOrbitPaused( int objectID )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].toggleOrbitPaused( );
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
bool Graphics::toggleAllPaused( int objectID )
{
  if( objectID < objectVector.size( ) )
  {
    objectVector[objectID].toggleAllPaused( );
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
  int index;

  for( index = 0; index < objectVector.size( ); index++ )
  {
    if( !objectVector[ index ].isPaused( ) )
    {
      objectVector[ index ].toggleAllPaused( );
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
  for( index = 0; index < objectVector.size( ); index++ )
  {
    if( objectVector[ index ].isPaused( ) )
    {
      objectVector[ index ].toggleAllPaused( );
    }
  }

}
