#pragma once
#include "SDL.h"
#include <iostream>
#include "Board.h"
#include "Block.h"
#include <chrono>
#include <SDL_mixer.h>
#include "Music.h"

#define TimeSpanType std::chrono::steady_clock::time_point


class Game {
public:
	Game();
	~Game();
	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	
	void handleEvents();
	void update();
	void render();
	void clean();

	bool running() { return isRunning; };
	TimeSpanType _lastTimeStamp;

	void MusicHandler();

private:
	int cnt = 0;
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Board board;
	bool leftMouseButtonDown = false;
	Music soundHandler;
};