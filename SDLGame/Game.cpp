#include "Game.hpp"

Game::Game()
{}
Game::~Game()
{}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) 
{
	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		std::cout << "Subsystems Initialised!.." << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
		{
			std::cout << "Window created" << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			std::cout << "Renderer created" << std::endl;
		}
		isRunning = true;

	} else {
		isRunning = false;
		return;
	}
	// Music & sounds Initialization
	soundHandler.InitMixer();
	soundHandler.SetMusicVolume(5);
	soundHandler.SetSoundVolume(20);
	soundHandler.gameMusic = soundHandler.LoadMusic("../Assets/gameMusic.wav");
	soundHandler.clickSound = soundHandler.LoadSound("../Assets/clickSound.wav");
	soundHandler.comboSound = soundHandler.LoadSound("../Assets/comboSound.wav");
	soundHandler.failSwapSound = soundHandler.LoadSound("../Assets/failSwapSound.wav");

	// Initialize board
	board.Init(renderer,&soundHandler);

}

void Game::handleEvents()
{
	SDL_Event sdlEvent;
	SDL_PollEvent(&sdlEvent);
	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_Point mousePosition;
	mousePosition.x = x;
	mousePosition.y = y;
	switch (sdlEvent.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			// If the click is outside the board then it deselects the selected block if it exists
			if (board.CheckBlockHover(mousePosition) == nullptr) 
			{
				board.CheckMouseClickUi(mousePosition);
				board.UnmarkBlock();

				// Prevents it from being possible to drag 
				leftMouseButtonDown = false;

				return;
			}
			// Handle the block click 
			soundHandler.PlaySound(soundHandler.clickSound);
			board.CheckMouseClick(mousePosition);

			// Start dragging the block
			leftMouseButtonDown = true;
			break;
		case SDL_MOUSEMOTION:
			// If any block is being dragged
			if (leftMouseButtonDown) 
			{
				// Handle block drag
				board.UpdateBlockOnDrag(mousePosition);

				// If the block have been dragged into another block to try to swap it will instantly stop dragging
				if (board.nextBlock != nullptr)
					leftMouseButtonDown = false;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			// If any block is being dragged
			if (leftMouseButtonDown)
			{
				// Stop dragging the block 
				leftMouseButtonDown = false;

				// Change the selected block marker state to Marked in case it is diferent from null
				if(board.selectedBlock != nullptr)
					board.selectedBlock->blockMarkerState = BlockMarkerState::Marked;
			}
			break;
	}
}

void Game::update()
{
	// -------------------------------------------------------------------------------
	TimeSpanType currentTime = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> elapsed = currentTime - _lastTimeStamp;
	_lastTimeStamp = currentTime;
	double deltaTime = elapsed.count();
	// -------------------------------------------------------------------------------

	// Game logic update
	board.UpdateBoard(deltaTime);

	MusicHandler();
}


void Game::MusicHandler()
{
	soundHandler.PlayMusic(soundHandler.gameMusic);
}


void Game::render()
{
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}

void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	IMG_Quit();
	SDL_Quit();
	std::cout << "Game Cleaned" << std::endl;
}

