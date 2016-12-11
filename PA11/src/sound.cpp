#include "sound.h"

Uint8 *soundPosition;
Uint32 soundRemaining;
SDL_atomic_t audioCallbackLeft;

void myCallback( void *userData, Uint8 *stream, int length );

Sound::Sound()
{
	 soundLoaded = false;
	 soundPlaying = false;
     alive = true;
     soundInterrupt = false;

	 threadManager = NULL;
}
Sound::~Sound()
{
    alive = false;
    
    if( threadManager != NULL )
    {

        threadManager->join( );
        delete threadManager;
        threadManager = NULL;
    }

    SDL_PauseAudioDevice( dev, 1 );

	SDL_CloseAudioDevice( dev );

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

    dev = SDL_OpenAudioDevice( NULL, 0, &soundSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE );

	if( dev == 0 )
	{
		std::cout << "Couldn't open audio: " << SDL_GetError( ) << std::endl;

	}

	SDL_PauseAudioDevice(  dev, 1 );

    threadManager = new std::thread( &Sound::playSound, this );

}


void Sound::playSound()
{
    while( alive )
    {
        if( soundPlaying )
        {
            soundPosition = soundBuffer;
            soundRemaining = soundLength;

            SDL_PauseAudioDevice( dev, 0 );

            while( soundRemaining > 0 && !soundInterrupt )
            {
                SDL_Delay( 200 );
            }

            SDL_PauseAudioDevice( dev, 1 );

            soundPlaying = false;


            if( soundInterrupt )
            {
                soundInterrupt = false;
                soundPlaying = true;
            }
        }
    }
 
}

void Sound::launchSound( )
{   
    if( soundPlaying )
    {
        soundInterrupt = true;
        return;
    }

    soundPlaying = true;
}

bool Sound::SoundPlaying( )
{
	return soundPlaying;
}

void myCallback( void *userData, Uint8 *stream, int length)
{
    Sint32 localAudioCbLeft = SDL_AtomicGet( &audioCallbackLeft );
    
    Uint32 index;

    for( index = 0; index < length; index++ )
    {
        stream[ index ] = soundPosition[localAudioCbLeft ];

        localAudioCbLeft++;

        if( localAudioCbLeft == soundRemaining )
        {
            localAudioCbLeft = 0;
            soundRemaining = 0;
        }

        
    }

    SDL_AtomicSet( &audioCallbackLeft, localAudioCbLeft );

}
