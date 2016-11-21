#ifndef SOUND_H
#define SOUND_H

#include <iostream>

#include <SDL2/SDL.h>

#include <string>
#include <thread>
#include <vector>
static Uint8 *soundPosition;
static Uint32 soundRemaining;
void myCallback( void *userData, Uint8 *stream, int length);

class Sound
{
    public:

        Sound();
        ~Sound();

        void loadSound( std::string soundPath );
        void playSound();

		void launchSound( );

		bool SoundPlaying( );


    private:

		std::vector<std::thread> threadManager;
        
        Uint32 soundLength;
        Uint8 *soundBuffer;
        SDL_AudioSpec soundSpec;
        bool soundLoaded;
		bool soundPlaying;

        friend void myCallback( void *userData, Uint8 *stream, int length);
 
};


#endif /* SOUND_H */
