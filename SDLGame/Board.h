#pragma once
#include "Block.h"
#include "Music.h"
#include "SDL.h"
#include "SDL_image.h"
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include <chrono>   
#include <string>   
#include <cmath>
#include "Effect.h"

#define TimeSpanType std::chrono::steady_clock::time_point
using std::string;

class Board {

public:
	Board();
	~Board();
	void Init(SDL_Renderer* myRenderer, Music* soundHandler);
	SDL_Texture* loadTexture(const std::string& path);
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
	void FirstInit();
	void UpdateBoard(float deltaTime);
	void DrawBoard();
	void DrawScoreBackground();
	void DrawGridBackground();
	void DrawHiddenLeafGridBackground();
	void DrawGameLogo();
	void DrawBackground();
	void RemoveDeadBlocks();
	void MoveBlocks();
	void SpawnNewBlocks();
	void UpdateGrid();
	bool IsBoardReady();
	bool IsBoardFalling();
	void CheckMouseClick(SDL_Point mousePosition);
	void TweekMarkedBlock();
	void UnmarkBlock();
	void StartSwapping();
	void SwapBlock(Block* block);
	Block* CheckBlockHover(SDL_Point mousePosition);
	void SwapMovement();
	void SetBlockOriginPos(Block* block);
	void UpdateBlockOnDrag(SDL_Point mousePosition);
	bool CheckMatchPoint(bool shouldPredict);
	void CheckInteractive();
	void SetMatchPoint();
	void DrawSoundUi();
	void DrawMusicUi();
	void CheckMouseClickUi(SDL_Point mousePosition);
	string GetColorName(BlockColor blockColor);
	string GetDirName(Dir dir);
	string BlockDescription(Block block);
	void DrawPoints();
	SDL_Texture* bg;
	Block* selectedBlock;
	Block* nextBlock;
	int points = 0;
	SDL_Rect rectSound;
	SDL_Rect rectMusic;
	void DrawParticles();
	std::vector<Effect> effects;
private:
	SDL_Renderer* renderer;
	SDL_Texture* textures_blocks[5];
	SDL_Texture* textures_particles[5];
	SDL_Texture* texture_score;
	SDL_Texture* texture_grid;
	SDL_Texture* texture_hiddenLeafGrid;
	SDL_Texture* texture_gameLogo;
	SDL_Texture* texture_soundOn;
	SDL_Texture* texture_soundOff;
	SDL_Texture* texture_musicOn;
	SDL_Texture* texture_musicOff;

	Block grid[8][8];
	float DeltaTime;
	float tweekTimer;
	float matchTimer;
	float readyTimer;
	bool interactive = true;
	TTF_Font* Sans;

	SDL_Color Black = { 0, 0, 0 };
	SDL_Color White = { 255, 255, 255 };
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int TILE_SIZE = 50;
	const float FALLINGSPEED = 8.5f * SCREEN_HEIGHT;
	const int ROWS = 8;
	const int COLUMNS = 8;
	const int BOARD_LEFT_PADDING = 200;
	const int BOARD_UP_PADDING = 140;
	Music* SoundHandler;

};