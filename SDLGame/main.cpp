#include "Game.hpp"

int main(int argc, char* argv[])
{
	Game game;

	game.init("JewelCrush", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);
	while (game.running()) {
		game.handleEvents();
		game.update();
		game.render();
	}
	game.clean();

	return 0;
}