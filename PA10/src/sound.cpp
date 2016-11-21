#include "sound.h"


Sound::Sound()
{     
     soundLoaded = false;
	 soundPlaying = false;
}
Sound::~Sound()
{
	size_t index;

	SDL_PauseAudio( 1 );

	SDL_CloseAudio( );

    if( soundLoaded )
    {
        SDL_FreeWAV( soundBuffer );
    }

	for( index = 0; index < threadManager.size( ); index++ )
	{
		threadManager[ index ].join( );
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

	if( SDL_OpenAudio( &soundSpec, NULL ) < 0 )
	{
		std::cout << "Couldn't open audio: " << SDL_GetError( ) << std::endl;
		exit( -1 );

	}

	SDL_PauseAudio( 1 );

}


void Sound::playSound()
{
	soundPlaying = true;
    soundPosition = soundBuffer;
    soundRemaining = soundLength;

    SDL_PauseAudio( 0 );
    
    while( soundRemaining > 0 )
    {
        SDL_Delay(100);
    }

	SDL_PauseAudio( 1 );

	soundPlaying = false;
 
}

void Sound::launchSound( )
{
	if( soundPlaying )
	{
		return;
	}

	threadManager.push_back( std::thread( &Sound::playSound, this ) );
}

bool Sound::SoundPlaying( )
{
	return soundPlaying;
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
