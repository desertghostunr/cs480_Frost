
#include "engine.h"

Engine::Engine(string name, int width, int height)
{
    m_WINDOW_NAME = name;
    m_WINDOW_WIDTH = width;
    m_WINDOW_HEIGHT = height;
    m_FULLSCREEN = false;
    splitScreen = false;

    m_graphics = NULL;
    m_window = NULL;
}

Engine::Engine(string name)
{
    m_WINDOW_NAME = name;
    m_WINDOW_HEIGHT = 0;
    m_WINDOW_WIDTH = 0;
    m_FULLSCREEN = true;
    splitScreen = false;

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
    specularLight = false;
    ambientLight = true;
    spotLight = false;

    redLight = true;
    greenLight = true;
    blueLight = true; 

    viewControls = false;

    // Start the graphics
    
    m_graphics = new Graphics();
    if(!m_graphics->Initialize(m_WINDOW_WIDTH, m_WINDOW_HEIGHT, 
                                progInfo, GetCurrentTimeMillis( ) ))
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
            Keyboard( );
        }


        // Update and render the graphics
        m_graphics->Update(m_DT);
        m_graphics->Render(m_DT);
        //m_graphics->RenderB(m_DT);
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
                
                if( splitScreen )
                {
                    m_graphics->moveShip( 1 );
                }
                else if( ambientLight )
                {
                    if( redLight )
                    {
                       m_graphics->changeBrightness( "ambient", 0.05, 0.0, 0.0 );
                    }
                    if( greenLight )
                    {
                       m_graphics->changeBrightness( "ambient" , 0.0, 0.05, 0.0 );
                    }
                    if( blueLight )
                    {
                       m_graphics->changeBrightness( "ambient" , 0.0, 0.0, 0.05 );
                    }
                }
                else if( specularLight )
                {
                    if( redLight )
                    {
                       m_graphics->changeBrightness( "specular" , 0.05, 0.0, 0.0 );
                    }
                    if( greenLight )
                    {
                       m_graphics->changeBrightness( "specular" , 0.0, 0.05, 0.0 );
                    }
                    if( blueLight )
                    {
                       m_graphics->changeBrightness( "specular" , 0.0, 0.0, 0.05 );
                    }             
                }
                else if( spotLight )
                {
                    if( redLight )
                    {
                       m_graphics->changeBrightness( "spot" , 0.05, 0.0, 0.0 );
                    }
                    if( greenLight )
                    {
                       m_graphics->changeBrightness( "spot" , 0.0, 0.05, 0.0 );
                    }
                    if( blueLight )
                    {
                       m_graphics->changeBrightness( "spot" , 0.0, 0.0, 0.05 );
                    }                
                }
                else if( viewControls )
                {
                    m_graphics->cameraUpOrDown( true );
                }
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_DOWN )
        {
            if( m_graphics != NULL )
            {
                
                if( splitScreen )
                {
                    //m_graphics->reverseShip( 0 );
                }
                else if( ambientLight )
                {
                    if( redLight )
                    {
                       m_graphics->changeBrightness( "ambient", -0.05, 0.0, 0.0 );
                    }
                    if( greenLight )
                    {
                       m_graphics->changeBrightness( "ambient" , 0.0, -0.05, 0.0 );
                    }
                    if( blueLight )
                    {
                       m_graphics->changeBrightness( "ambient" , 0.0, 0.0, -0.05 );
                    }
                }
                else if( specularLight )
                {
                    if( redLight )
                    {
                       m_graphics->changeBrightness( "specular" , -0.05, 0.0, 0.0 );
                    }
                    if( greenLight )
                    {
                       m_graphics->changeBrightness( "specular" , 0.0, -0.05, 0.0 );
                    }
                    if( blueLight )
                    {
                       m_graphics->changeBrightness( "specular" , 0.0, 0.0, -0.05 );
                    }             
                }
                else if( spotLight )
                {
                    if( redLight )
                    {
                       m_graphics->changeBrightness( "spot" , -0.05, 0.0, 0.0 );
                    }
                    if( greenLight )
                    {
                       m_graphics->changeBrightness( "spot" , 0.0, -0.05, 0.0 );
                    }
                    if( blueLight )
                    {
                       m_graphics->changeBrightness( "spot" , 0.0, 0.0, -0.05 );
                    }                
                }
                else if( viewControls )
                {
                    m_graphics->cameraUpOrDown( false );
                }
            }        
        }
        else if( m_event.key.keysym.sym == SDLK_RIGHT )
        {
            if( m_graphics != NULL )
            {   
                
                if( splitScreen )
                {
                    m_graphics->rotateShip( 1, ( -1.0f * ShipController::STD_TORQUE ) );
                }
                else if( specularLight )
                {
                    m_graphics->changeModelRegistryIndex( 1 );
                }
                else if( spotLight )
                {
                    m_graphics->changeSpotLightSize( 1 );
                }
                else if( viewControls )
                {
                    m_graphics->cameraLeftOrRight( false );
                }

            }            
        }
        else if( m_event.key.keysym.sym == SDLK_LEFT )
        {
            if( m_graphics != NULL )
            {
                if( splitScreen )
                {
                    m_graphics->rotateShip( 1, ShipController::STD_TORQUE );
                }
                else if( specularLight )
                {
                    m_graphics->changeModelRegistryIndex( -1 );
                }
                else if( spotLight )
                {
                    m_graphics->changeSpotLightSize( -1 );
                }
                else if( viewControls )
                {
                    m_graphics->cameraLeftOrRight( true );
                }
            }            
        }
        else if( ( m_event.key.keysym.sym == SDLK_r ) )
        {
            if( m_graphics != NULL )
            {
               redLight = true;
               greenLight = false;
               blueLight = false; 
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_g ) )
        {
            if( m_graphics != NULL )
            {
               redLight = false;
               greenLight = true;
               blueLight = false; 
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_b ) )
        {
            if( m_graphics != NULL )
            {
               redLight = false;
               greenLight = false;
               blueLight = true;
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_o ) )
        {
            if( m_graphics != NULL )
            {
                blueLight = true;
                greenLight = true;
                redLight = true;
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_x ) )
        {
            if( m_graphics != NULL )
            {
                m_graphics->cycleShaderProgram( );
            }
        }
        else if( ( m_event.key.keysym.sym == SDLK_c ) )
        {
            m_graphics->lookForward( 0 );
        }
        else if( m_event.key.keysym.sym == SDLK_SPACE )
        {
            if( !m_graphics->isPlaying( ))
            {
                m_graphics->startGame( );
                m_graphics->startSplitScreen( m_WINDOW_WIDTH, m_WINDOW_HEIGHT );
                splitScreen = true;
                
            }
        }
        else if( m_event.key.keysym.sym == SDLK_1 )
        {
            if( m_graphics != NULL )
            {
                ambientLight = true;
                specularLight = false;
                viewControls = false;
                spotLight = false;
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_2 )
        {
            if( m_graphics != NULL )
            {
                ambientLight = false;
                specularLight = true;
                viewControls = false;
                spotLight = false;
            }
        }
        else if( m_event.key.keysym.sym == SDLK_3 )
        {
            if( m_graphics != NULL )
            {
                ambientLight = false;
                specularLight = false;
                viewControls = false;
                spotLight = true;
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_p )
        {
            if( m_graphics != NULL )
            {
                m_graphics->togglePausedState( );
            }
        }        
        else if( m_event.key.keysym.sym == SDLK_v ) 
        {
            ambientLight = false;
            specularLight = false;
            spotLight = false;
            viewControls = true;

            m_graphics->turnOffSplash( );
        }
        else if( m_event.key.keysym.sym == SDLK_w )
        {
            m_graphics->moveShip( 0 );
            //std::cout << "W down" << std::endl;
        }
        if( m_event.key.keysym.sym == SDLK_s )
        {
            //m_graphics->reverseShip( 0 );
            //std::cout << "S down" << std::endl;
        }
        else if( m_event.key.keysym.sym == SDLK_a )
        {
            m_graphics->rotateShip( 0, ShipController::STD_TORQUE );
            //std::cout << "A down" << std::endl;
        }
        else if( m_event.key.keysym.sym == SDLK_d )
        {
            m_graphics->rotateShip( 0, ( -1.0f * ShipController::STD_TORQUE ) );
            //std::cout << "D down" << std::endl;
        }
        else if( m_event.key.keysym.sym == SDLK_q )
        {
            //m_graphics->fireGuns( 0, false );
                        m_graphics->toggleLeft( 0 );
        }
        else if( m_event.key.keysym.sym == SDLK_e )
        {
            //m_graphics->fireGuns( 0, true );
                        m_graphics->toggleRight( 0 );
        }
        else if( m_event.key.keysym.sym == SDLK_f )
        {
            m_graphics->fireGuns( 0 );
        }
        else if( splitScreen && m_event.key.keysym.sym == SDLK_LEFTBRACKET )
        {
              //m_graphics->fireGuns( 0, false );
              m_graphics->toggleLeft( 1 );
        }
        else if( splitScreen && m_event.key.keysym.sym == SDLK_RIGHTBRACKET )
        {
            //m_graphics->fireGuns( 0, true );
             m_graphics->toggleRight( 1 );
        }
        else if( splitScreen && m_event.key.keysym.sym == SDLK_BACKSLASH )
        {
            m_graphics->fireGuns( 1 );
        }
                
        else if( m_event.key.keysym.sym == SDLK_4 )
        {
            if( m_graphics->isPlaying( ) )
            {
                m_graphics->startSplitScreen( m_WINDOW_WIDTH, m_WINDOW_HEIGHT );
                splitScreen = true;
            }            
        }
        else if( m_event.key.keysym.sym == SDLK_RSHIFT 
                 || m_event.key.keysym.sym == SDLK_LSHIFT )
        {
            m_graphics->resetView( );
        }
        else if( splitScreen && m_event.key.keysym.sym == SDLK_TAB )
        {
            m_graphics->toggleSplitScreenView();
        }
        else if( m_event.key.keysym.sym == SDLK_RETURN 
                 || m_event.key.keysym.sym == SDLK_RETURN )
        {
            m_graphics->lookForward( 1 );
        }
 
    }
    else if( m_event.type == SDL_KEYUP ) 
    {	
        if( m_graphics != NULL )
        {
            if( m_event.key.keysym.sym == SDLK_w )
            {
                m_graphics->slowShipToHalt( 0 );
                //std::cout << "W up" << std::endl;
            }
            else if( m_event.key.keysym.sym == SDLK_s )
            {
                //m_graphics->slowShipToHalt( 0 );
                //std::cout << "S up" << std::endl;
            }
            else if( m_event.key.keysym.sym == SDLK_a )
            {
                m_graphics->stopShipsRotation( 0 );
                //std::cout << "A up" << std::endl;
            }
            else if( m_event.key.keysym.sym == SDLK_d )
            {
                m_graphics->stopShipsRotation( 0 );
                //std::cout << "D up" << std::endl;
            }
                        if( splitScreen )
                        {
                if( m_event.key.keysym.sym == SDLK_UP )
                {
                m_graphics->slowShipToHalt( 1 );
                //std::cout << "W up" << std::endl;
                }
                else if( m_event.key.keysym.sym == SDLK_DOWN )
                {
                //m_graphics->slowShipToHalt( 0 );
                //std::cout << "S up" << std::endl;
                }
                else if( m_event.key.keysym.sym == SDLK_LEFT )
                {
                m_graphics->stopShipsRotation( 1 );
                //std::cout << "A up" << std::endl;
                }
                else if( m_event.key.keysym.sym == SDLK_RIGHT )
                {
                m_graphics->stopShipsRotation( 1 );
                //std::cout << "D up" << std::endl;
                }
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
