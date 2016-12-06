#include "sound.h"

static Uint8 *soundPosition;
static Uint32 soundRemaining;

Sound::Sound()
{
	 soundLoaded = false;
	 soundPlaying = false;

	 threadManager = NULL;
}
Sound::~Sound()
{

	SDL_PauseAudio( 1 );

	SDL_CloseAudio( );

	if( soundLoaded )
	{
		SDL_FreeWAV( soundBuffer );
	}

	if( threadManager != NULL )
	{
		threadManager->join();
		delete threadManager;
		threadManager = NULL;
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

	if( threadManager != NULL )
	{
		threadManager->join();
		delete threadManager;
		threadManager = NULL;
	}
	else
	{
		threadManager = new std::thread( &Sound::playSound, this );
	}
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
