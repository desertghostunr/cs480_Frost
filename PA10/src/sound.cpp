#include "sound.h"


Sound::Sound()
{     
     soundLoaded = false;
}
Sound::~Sound()
{
 
    if( soundLoaded )
    {
        SDL_FreeWAV( soundBuffer );
    }
  
}

void Sound::loadSound( std::string soundPath )
{
    if( SDL_LoadWAV( soundPath.c_str(), &soundSpec, &soundBuffer, &soundLength ) == NULL )
    {
        std::cout << "Unable to load sound" << std::endl;
        return;
    }
    soundSpec.callback = myCallback;
    soundSpec.userdata = NULL;
    soundLoaded = true;

}


void Sound::playSound()
{
    soundPosition = soundBuffer;
    soundRemaining = soundLength;

    if( SDL_OpenAudio( &soundSpec, NULL ) < 0 )
    {
         std::cout << "Couldn't open audio: " << SDL_GetError() << std::endl;
         exit( -1 );   
    
    }

    SDL_PauseAudio( 0 );
    
    while( soundRemaining > 0 )
    {
        SDL_Delay(100);
    }

    SDL_CloseAudio();

    
 
}

void myCallback( void *userData, Uint8 *stream, int length)
{

    if( soundRemaining == 0 )
    {
        return;
    }

    length = ( length > soundRemaining ? soundRemaining : length );

    SDL_MixAudio( stream, soundPosition, length, SDL_MIX_MAXVOLUME );

    soundPosition += length;
    soundRemaining -= length;  




}
