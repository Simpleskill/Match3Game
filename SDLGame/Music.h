#pragma once
#include "SDL.h"
#include <iostream>
#include <chrono>
#include <SDL_mixer.h>
#include <vector>


class Music {
public:
	Music();
	~Music();
	int LoadMusic(const char* filename);
	int LoadSound(const char* filename);

	void SetMusicVolume(int v);
	void SetSoundVolume(int v);

	int PlayMusic(int m);
	int PlaySound(int s);
	int InitMixer();
	void QuitMixer();
	void TogglePlay();

	std::vector<Mix_Chunk*> sounds;
	std::vector<Mix_Music*> music;

	int gameMusic;
	int clickSound;
	int comboSound;
	int failSwapSound;

private:
	int volume;
	int musicVolume;
	int soundVolume;
	
};