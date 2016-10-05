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
};

#endif // ENGINE_H