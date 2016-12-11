#ifndef ENGINE_H
#define ENGINE_H

#if defined( _WIN64 ) || ( _WIN32 )
    #include<Windows.h>
#else
    #include <sys/time.h>
#endif

#include <assert.h>

#include<vector>

#include "window.h"
#include "graphics.h"

struct Coord
{
    int x;
    int y;

    Coord( ) : x( 0 ), y( 0 )
    {

    }

    Coord( int newX, int newY ) : x( newX ), y( newY )
    {

    }

    Coord( const Coord& src ) : x( src.x ), y( src.y )
    {

    }

    const Coord& operator = ( const Coord& src )
    {
        if( this != &src )
        {
            x = src.x;
            y = src.y;
        }

        return *this;
    }
};


class Engine
{
    public:
        Engine(string name, int width, int height);
        Engine(string name);
        ~Engine();
        bool Initialize( const GraphicsInfo& progInfo );
        void Run();
        void Keyboard();
        void Mouse();
        unsigned int getDT();
        long long GetCurrentTimeMillis();
    
    private:
        // Window related variables
        Window *m_window;        
        string m_WINDOW_NAME;
        int m_WINDOW_WIDTH;
        int m_WINDOW_HEIGHT;
        bool m_FULLSCREEN;
        SDL_Event m_event;

        Graphics *m_graphics;
        unsigned int m_DT;
        long long m_currentTimeMillis;
        bool m_running;

        int trackingID;

        bool specularLight;
        bool ambientLight;
        bool spotLight;
        bool viewControls;
        bool splitScreen;

        bool redLight;
        bool greenLight;
        bool blueLight;      

        Coord previousMousePosition;

};

#endif // ENGINE_H
