#include "Music.h"

Music::Music()
{}
Music::~Music()
{}


int Music::LoadMusic(const char* filename)
{
	// Load music by path/name into a vector and return's position
	Mix_Music* m = NULL;
	m = Mix_LoadMUS(filename);
	if (m == NULL) 
	{
		printf("Failed to load music. SDL_Mixer error: %s\n", Mix_GetError());
		return -1;
	}
	music.push_back(m);
	return music.size() - 1;
}

int Music::LoadSound(const char* filename)
{
	// Load Sound by path/name into a vector and return's position
	Mix_Chunk* m = NULL;
	m = Mix_LoadWAV(filename);
	if (m == NULL)
	{
		printf("Failed to load music. SDL_Mixer error: %s\n", Mix_GetError());
		return -1;
	}
	sounds.push_back(m);
	return sounds.size() - 1;
}

void Music::SetMusicVolume(int v)
{
	// Set music volume
	musicVolume = (MIX_MAX_VOLUME * v) / 100;
	Mix_VolumeMusic(musicVolume);
}
void Music::SetSoundVolume(int v)
{
	// Set sound volume
	soundVolume = (MIX_MAX_VOLUME * v) / 100;
	Mix_Volume(-1,soundVolume);
}

int Music::PlayMusic(int m) 
{
	// Set music to be played in case no other music is playing anymore
	if (Mix_PlayingMusic() == 0)
	{
		Mix_Volume(1, musicVolume);
		Mix_PlayMusic(music[m], -1);
	}
	return 0;
}

int Music::PlaySound(int s)
{
	// Set sound to be played
	Mix_Volume(-1, soundVolume);
	Mix_PlayChannel(-1, sounds[s], 0);
	return 0;
}

int Music::InitMixer()
{
	// Itiliaze mixer and preset the default volumes
	Mix_Init(MIX_INIT_MP3);
	SDL_Init(SDL_INIT_AUDIO);
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_Mixer couldnt init. Err: %s\n", Mix_GetError());
		return -1;
	}
	SetMusicVolume(50);
	SetSoundVolume(50);
	return 0;
}

void Music::QuitMixer()
{
	// Quit mixer and free space
	for (int s = 0; s < sounds.size(); s++)
	{
		Mix_FreeChunk(sounds[s]);
		sounds[s] = NULL;
	}
	for (int s = 0; s < music.size(); s++)
	{
		Mix_FreeMusic(music[s]);
		music[s] = NULL;
	}
	Mix_Quit();
}

void Music::TogglePlay()
{
	// Pause or resume music
	if (Mix_PausedMusic() == 1)
	{
		Mix_ResumeMusic();
	}
	else {
		Mix_PauseMusic();
	}
}