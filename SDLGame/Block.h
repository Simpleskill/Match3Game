#pragma once
#include "SDL.h"
#include "SDL_image.h"

enum class BlockColor
{
	Black,
	White,
	Pink,
	Blue,
	Orange
};

enum class BlockState
{
	Normal,
	Moving,
	Moved,
	Falling,
	Disappearing,
	Dead
};
enum class BlockMarkerState
{
	None,
	Marking,
	Unmarking,
	Marked
};

enum class Dir
{
	NONE,
	Left,
	Right,
	Up,
	Down
};

struct Block
{
	BlockColor			blockColor;
	BlockState			blockState;
	BlockMarkerState	blockMarkerState;
	SDL_Rect rect;
	int x;
	int y;
	Dir dir;
};