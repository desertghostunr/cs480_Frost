
#include "engine.h"

Engine::Engine(string name, int width, int height)
{
    m_WINDOW_NAME = name;
    m_WINDOW_WIDTH = width;
    m_WINDOW_HEIGHT = height;
    m_FULLSCREEN = false;

    m_graphics = NULL;
    m_window = NULL;
}

Engine::Engine(string name)
{
    m_WINDOW_NAME = name;
    m_WINDOW_HEIGHT = 0;
    m_WINDOW_WIDTH = 0;
    m_FULLSCREEN = true;

    m_graphics = NULL;
    m_window = NULL;
}

Engine::~Engine()
{
    if( m_graphics != NULL )
    {
        delete m_graphics;
        m_graphics = NULL;
    }
    if( m_window != NULL )
    {
        delete m_window;
        m_window = NULL;
    }
    
    m_window = NULL;
    m_graphics = NULL;
}

bool Engine::Initialize( const GraphicsInfo& progInfo )
{
    // Start a window
    
    m_window = new Window();
    if(!m_window->Initialize(m_WINDOW_NAME, &m_WINDOW_WIDTH, &m_WINDOW_HEIGHT))
    {
        printf("The window failed to initialize.\n");
        return false;
    }    

    trackingID = -1;
    // Start the graphics
    
    m_graphics = new Graphics();
    if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, progInfo ))
    {
        printf("The graphics failed to initialize.\n");
        return false;
    }

    // Set the time
    m_currentTimeMillis = GetCurrentTimeMillis();
    // No errors
    return true;
}

void Engine::Run()
{
    m_running = true;

    while(m_running)
    {
        // Update the DT
        m_DT = getDT();

        // Check the keyboard input

        while(SDL_PollEvent(&m_event) != 0)
        {
            Mouse( );
            Keyboard();
        }


        // Update and render the graphics
        m_graphics->Update(m_DT);
        m_graphics->Render();

        // Swap to the Window
        m_window->Swap();
    }
}

void Engine::Keyboard()
{

    if(m_event.type == SDL_QUIT)
    {
        m_running = false;
    }
    else if (m_event.type == SDL_KEYDOWN)
    {
        // handle key down events here
        if (m_event.key.keysym.sym == SDLK_ESCAPE)
        {
            m_running = false;
        }
        else if( m_event.key.keysym.sym == SDLK_UP )
        {
            if( m_graphics != NULL )
            {                
                m_graphics->cameraUpOrDown( true );
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_w )
        {
            if( m_graphics != NULL )
            {                
                m_graphics->cameraZoomInOrOut( true );
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_DOWN )
        {
            if( m_graphics != NULL )
            {
                m_graphics->cameraUpOrDown( false );
            }        
        }
        else if( m_event.key.keysym.sym == SDLK_s )
        {
            if( m_graphics != NULL )
            {
                m_graphics->cameraZoomInOrOut( false );
            }        
        }
        else if( m_event.key.keysym.sym == SDLK_RIGHT )
        {
            if( m_graphics != NULL )
            {
                m_graphics->cameraLeftOrRight( false );
            }            
        }
        else if( ( m_event.key.keysym.sym == SDLK_PERIOD ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_COMMA ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_o ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_l ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_k ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_r ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_a ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_d ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_x ) )
        {
            if( m_graphics != NULL )
            {
                
            }
        }
        else if( m_event.key.keysym.sym == SDLK_LEFT )
        {
            if( m_graphics != NULL )
            {
                m_graphics->cameraLeftOrRight( true );
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_SPACE )
        {
            if( m_graphics != NULL )
            {
                
            }            
        }
	else if( m_event.key.keysym.sym == SDLK_1 )
        {
            if( m_graphics != NULL )
            {
                m_graphics->ChangePerspectiveStatic( 1 );
            }            
        }
	else if( m_event.key.keysym.sym == SDLK_2 )
        {
            if( m_graphics != NULL )
            {
                m_graphics->ChangePerspectiveStatic( 2 );
            }            
        }
	else if( m_event.key.keysym.sym == SDLK_q )
        {
            if( m_graphics != NULL )
            {
		if( trackingID <= 0 )
  		{
		   trackingID = 8;		  
		}
		else
		{
		   trackingID -= 1;
		}

                m_graphics->startTracking( trackingID );
            }            
        }
	else if( m_event.key.keysym.sym == SDLK_e )
        {
            if( m_graphics != NULL )
            {
		if( trackingID >= 8 )
  		{
		   trackingID = 0;		  
		}
		else
		{
		   trackingID += 1;
		}
                m_graphics->startTracking( trackingID );
            }            
        }
    }
}

// MOUSE //////////////////
/***************************************

@brief Mouse( )

@details checks for mouse events

@param None

@notes None

***************************************/
void Engine::Mouse( )
{
    Coord currentPos;
    Coord posChange;

    static bool mouse_initialized = false;

    if( m_event.type == SDL_MOUSEBUTTONDOWN )
    {
        if( SDL_GetMouseState( NULL, NULL ) & SDL_BUTTON( SDL_BUTTON_LEFT )    )             
        {
            if( m_graphics != NULL )
            {
                if( SDL_SetRelativeMouseMode( SDL_TRUE ) )
                {
                    printf( "The mouse is in Relative Mode. \n" );
                }
            } 
        }
        else if( SDL_GetMouseState( NULL, NULL ) & SDL_BUTTON( SDL_BUTTON_RIGHT ) )
        {
            if( m_graphics != NULL )
            {
                if( SDL_SetRelativeMouseMode( SDL_FALSE ) )
                {
                    printf( "The mouse is in Relative Mode. \n" );
                }
            } 
        }
        else if( SDL_GetMouseState( NULL, NULL ) & SDL_BUTTON( SDL_BUTTON_MIDDLE ) )
        {
            if( m_graphics != NULL )
            {
                
            } 
        }
        else if( SDL_GetMouseState( NULL, NULL ) & SDL_BUTTON( SDL_BUTTON_X1 ) )
        {
            if( m_graphics != NULL )
            {                
                
            } 
        }
        else if( SDL_GetMouseState( NULL, NULL ) & SDL_BUTTON( SDL_BUTTON_X2 ) )
        {
            if( m_graphics != NULL )
            {
                
            } 
        }
    }
    else if( m_event.type == SDL_MOUSEMOTION )
    {
        if( m_graphics != NULL )
        {
            if( !mouse_initialized )
            {
                if( SDL_SetRelativeMouseMode( SDL_TRUE ) )
                {
                    printf( "The mouse is in Relative Mode. \n" );
                }

                mouse_initialized = true;
            }

            currentPos.x = m_event.motion.x;
            currentPos.y = m_event.motion.y;

            posChange.x = currentPos.x - previousMousePosition.x;
            posChange.y = currentPos.y - previousMousePosition.y;

            m_graphics->moveBox( glm::vec3( posChange.x, 0.0, posChange.y ) );

            previousMousePosition = currentPos;
        }
    }
    
}

unsigned int Engine::getDT()
{
    long long TimeNowMillis = GetCurrentTimeMillis();
    assert(TimeNowMillis >= m_currentTimeMillis);
    unsigned int DeltaTimeMillis = (unsigned int)(TimeNowMillis - m_currentTimeMillis);
    m_currentTimeMillis = TimeNowMillis;
    return DeltaTimeMillis;
}

long long Engine::GetCurrentTimeMillis()
{
#if defined( _WIN64 ) || ( _WIN32 )
    FILETIME fileTime;
    LARGE_INTEGER largeInt;
    uint64_t time;

    GetSystemTimeAsFileTime( &fileTime );
    largeInt.LowPart = fileTime.dwLowDateTime;
    largeInt.HighPart = fileTime.dwHighDateTime;

    time = largeInt.QuadPart;
    time -= 116444736000000000LL;
    time /= 10000;
    return ( long long ) time;
#else
    timeval t;
    gettimeofday(&t, NULL);
    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif
}
