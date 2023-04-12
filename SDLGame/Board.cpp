#include "Board.h"


SDL_Texture* Board::loadTexture(const std::string& path)
{
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

void Board::Init(SDL_Renderer* myRenderer) {

    //Initialize SDL_TTF
    TTF_Init();
    // Opens a font style and sets a size
    Sans = TTF_OpenFont("../Assets/Sans.ttf", 40);

    // Initialization of Textures
    renderer = myRenderer;
    textures[0] = loadTexture("../Assets/Color-1.png");
    textures[1] = loadTexture("../Assets/Color-2.png");
    textures[2] = loadTexture("../Assets/Color-3.png");
    textures[3] = loadTexture("../Assets/Color-4.png");
    textures[4] = loadTexture("../Assets/Color-5.png");
    texture_score = loadTexture("../Assets/WoodBoardScore.png");
    texture_grid = loadTexture("../Assets/WoodBoardGrid2.png");
    texture_hiddenLeafGrid = loadTexture("../Assets/HiddenLeafBoardGrid.png");
    texture_gameLogo = loadTexture("../Assets/GameLogoJewelCrush.png");

    bg = loadTexture("../Assets/Backdrop13.jpg");
    // Initialization of Grid
    FirstInit();
}
void Board::UpdateBoard(float deltaTime) {
    // Storing the delta time every frame to handle animations and delays
    DeltaTime = deltaTime;
    
    // Draw Background
    DrawBackground();

    // Draw Background
    DrawGameLogo();

    // Draw Score Background
    DrawScoreBackground();

    // Draw Score Background
    DrawGridBackground();

    // Check if it is Interactive
    CheckInteractive();

    // Remove dead blocks
    RemoveDeadBlocks();

    // Update grid, replace dead blocks below with the normal ones above
    UpdateGrid();

    // Verify if there are no falling blocks, otherwise it generates new blocks above
    if (!IsBoardFalling())
        SpawnNewBlocks();

    // Move the falling blocks
    MoveBlocks();

    // Animate the selected block
    TweekMarkedBlock();

    // Exchanges the selected blocks if there are any.
    SwapMovement();

    // Make matches of blocks that are in sequence
    SetMatchPoint();

    // Draw the board
    DrawBoard();

    // Draw Points
    DrawPoints();

    // Draw Leaf Detail
    DrawHiddenLeafGridBackground();
}

void Board::DrawBoard() {
    // Draws the board, leaving the selected block for last, in order to be at a higher z-index
    for (int i = 0; i < ROWS * COLUMNS; i++)
    {
        int x = i % COLUMNS;
        int y = i / COLUMNS;

        if (selectedBlock == &grid[x][y])
            continue;
        SDL_RenderCopy(renderer, textures[(int)grid[x][y].blockColor], NULL, &grid[x][y].rect);
    }
    if(selectedBlock != nullptr)
        SDL_RenderCopy(renderer, textures[(int)selectedBlock->blockColor], NULL, &selectedBlock->rect);
}

void Board::CheckInteractive() {
    // Whenever the board is changed, it forces a half second cooldown before going back to being interactive
    if (!IsBoardReady()) 
    {
        interactive = false;
        readyTimer = 0;
        return;
    }
    if (!interactive) {
        readyTimer += DeltaTime;
        if (readyTimer >= 0.5f) 
        {
            readyTimer = 0;
            interactive = true;
        }
        else {
            return;
        }
    }
}

void Board::FirstInit() {
    srand(time(NULL));
    // Boolean variable to avoid matching points at the first start of the grid
    bool validBlock = true;
    for (int i = 0; i < ROWS * COLUMNS; i++)
    {
        int x = i % COLUMNS;
        int y = i / COLUMNS;

        Block block = {
            (BlockColor)(rand() % 5),
            BlockState::Normal,
            BlockMarkerState::None,
            { x * TILE_SIZE+ BOARD_LEFT_PADDING, y * TILE_SIZE+ BOARD_UP_PADDING, TILE_SIZE, TILE_SIZE },
            x,
            y
        };

        do { // Continue to generate random colors until the selected color is not the same as the last 2 blocks in x and y
            validBlock = true;
            if (x > 1) 
            {
                if ((grid[x - 1][y].blockColor == block.blockColor) && (grid[x - 2][y].blockColor == block.blockColor)) 
                {
                    validBlock = false;
                    block.blockColor = (BlockColor)(rand() % 5);
                }
            }
            if (y > 1) 
            {
                if ((grid[x][y-1].blockColor == block.blockColor) && (grid[x][y-2].blockColor == block.blockColor)) 
                {
                    validBlock = false;
                    block.blockColor = (BlockColor)(rand() % 5);
                }
            }
        } while (!validBlock);

        // Assign the block to the board
        grid[x][y] = block;
    }
}

void Board::DrawBackground() 
{
    // Create a fullscreen rect with the background texture loaded
    SDL_Rect rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, bg, NULL, &rect);
}

void Board::DrawGameLogo()
{
    // Draw game logo top-right Corner
    SDL_Rect rect = { SCREEN_WIDTH-180, 0, 180 , 180 };
    SDL_RenderCopy(renderer, texture_gameLogo, NULL, &rect);
}


void Board::DrawScoreBackground()
{
    // Draw score background wood board
    SDL_Rect rect = { SCREEN_WIDTH / 2 - 200, 25, 400, 70 };
    SDL_RenderCopy(renderer, texture_score, NULL, &rect);
}

void Board::DrawGridBackground()
{
    // Draw grid background
    SDL_Rect rect = { BOARD_LEFT_PADDING - TILE_SIZE -5, BOARD_UP_PADDING - TILE_SIZE -10, TILE_SIZE * 10 +15, TILE_SIZE * 10 +15 };
    SDL_RenderCopy(renderer, texture_grid, NULL, &rect);
}
void Board::DrawHiddenLeafGridBackground()
{
    // Draw a leaf detail in the grid top-right corner
    SDL_Rect rect = { BOARD_LEFT_PADDING - TILE_SIZE - 5, BOARD_UP_PADDING - TILE_SIZE - 10, TILE_SIZE * 10 + 15, TILE_SIZE * 10 + 15 };
    SDL_RenderCopy(renderer, texture_hiddenLeafGrid, NULL, &rect);
}

void Board::RemoveDeadBlocks()
{
    // Clears the rect of dead blocks
    for (int i = 0; i < ROWS * COLUMNS; i++)
    {
        int x = i % COLUMNS;
        int y = i / COLUMNS;
        
        if (grid[x][y].blockState == BlockState::Dead) {
            grid[x][y].rect = {};
        }
    }
}

void Board::UpdateGrid() 
{
    // Pulls all blocks from above to replace the dead blocks below, also assigning the falling state
    for (int x = 7; x >= 0; x--) 
    {
        for (int y = 7; y >= 0; y--) 
        {
            if (grid[x][y].blockState == BlockState::Dead && grid[x][y].blockMarkerState != BlockMarkerState::Marked && grid[x][y].blockState != BlockState::Moving)
            {
                for (int y2 = y-1; y2 >= 0; y2--)
                {
                    if (grid[x][y2].blockState != BlockState::Dead)
                    {
                        grid[x][y].rect = grid[x][y2].rect;
                        grid[x][y] = grid[x][y2];
                        grid[x][y].x = x;
                        grid[x][y].y = y;

                        grid[x][y].blockState = BlockState::Falling;
                        grid[x][y2].blockState = BlockState::Dead;
                        break;
                    }
                }
            }
        }
    }
}

void Board::TweekMarkedBlock() {
    // Animation for selected block to be highlighted
    if (selectedBlock == nullptr || selectedBlock->blockMarkerState != BlockMarkerState::Marked) 
    {
        return;
    } 
    tweekTimer += DeltaTime;
    if (tweekTimer >= 0.2f) 
    {
        tweekTimer = 0;
    }
    else 
    {
        return;
    }
    if(selectedBlock->rect.w >= TILE_SIZE)
    {
        selectedBlock->rect.w = TILE_SIZE * 0.95f;
        selectedBlock->rect.h = TILE_SIZE * 0.95f;

        selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + 2 + BOARD_LEFT_PADDING;
        selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + 2 + BOARD_UP_PADDING;
    }
    else
    {
        selectedBlock->rect.w = TILE_SIZE;
        selectedBlock->rect.h = TILE_SIZE;

        selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;
        selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;
    }
}

void Board::UnmarkBlock() {
    // Deselect selected blocks
    if (selectedBlock == nullptr)
        return;
    selectedBlock->rect.w = TILE_SIZE;
    selectedBlock->rect.h = TILE_SIZE;
    selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;

    selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;
    selectedBlock->blockMarkerState = BlockMarkerState::None;
    selectedBlock->dir = Dir::NONE;
    if(nextBlock!= nullptr)
        nextBlock->dir = Dir::NONE;
    selectedBlock = nullptr;
    nextBlock = nullptr;
}

void Board::MoveBlocks() {
    // If the block is in a falling state, reduce the Y of the block rect until the updated position is reached
    for (int x = 0; x < 8; x++) 
    {
        for (int y = 0; y < 8; y++) 
        {
            if (grid[x][y].blockState == BlockState::Falling) 
            {
                if (grid[x][y].rect.y >= (y) * TILE_SIZE + BOARD_UP_PADDING)
                {
                    grid[x][y].rect.y = (y) * TILE_SIZE + BOARD_UP_PADDING;
                    grid[x][y].blockState = BlockState::Normal;
                }
                else 
                {
                    grid[x][y].rect.y += FALLINGSPEED * DeltaTime;
                }
            }
        }
    }
}

void Board::SpawnNewBlocks() {
    // After all the blocks have already been visually updated the falling effect starts creating the new blocks
    // It will spawn blocks at the grid positions where the blocks are dead, randomly generate a color
    // Set them in the Falling state, and spawn on top of the board.
    for (int x = 0; x < 8; x++) 
    {
        for (int y = 7; y >= 0; y--) 
        {
            if (grid[x][0].blockState == BlockState::Normal) 
            {
                continue;
            }
            if (grid[x][y].blockState == BlockState::Dead) 
            {
                Block block = 
                {
                    (BlockColor)(rand() % 5),
                    BlockState::Falling,
                    BlockMarkerState::None,
                    { x * TILE_SIZE + BOARD_LEFT_PADDING, (-1) * TILE_SIZE + BOARD_UP_PADDING, TILE_SIZE, TILE_SIZE},
                    x,
                    y
                };
                grid[x][y] = block;
            }
        }
    }
}

bool Board::IsBoardReady() 
{
    // Returns false if any of the blocks are not in the normal state, true false otherwise
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (grid[x][y].blockState != BlockState::Normal)
                return false;
        }
    }
    return true;
}

bool Board::IsBoardFalling() 
{
    // Returns true if any of the blocks are in the falling state, returns false otherwise
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (grid[x][y].blockState == BlockState::Falling)
                return true;
        }
    }
    return false;
}

void Board::StartSwapping() 
{

    // Checks if any match point will be made with this exchange, if not, the exchange is canceled, otherwise it continues
    if (!CheckMatchPoint()) 
    {
        UnmarkBlock();
        return;
    }

    // Updates the direction in which the swap will be made in the selected block and in the block that will be replaced
    if (selectedBlock->x > nextBlock->x)
    {
        selectedBlock->dir = Dir::Left;
        nextBlock->dir = Dir::Right;
    }
    else if (selectedBlock->x < nextBlock->x)
    {
        selectedBlock->dir = Dir::Right;
        nextBlock->dir = Dir::Left;
    }
    else if (selectedBlock->y > nextBlock->y)
    {
        selectedBlock->dir = Dir::Up;
        nextBlock->dir = Dir::Down;
    }
    else if (selectedBlock->y < nextBlock->y)
    {
        selectedBlock->dir = Dir::Down;
        nextBlock->dir = Dir::Up;
    }
    
    // Updates the selected block to its original position and size
    SetBlockOriginPos(selectedBlock);

    // Update the states of the selected block and next block
    selectedBlock->blockMarkerState = BlockMarkerState::Unmarking;
    nextBlock->blockMarkerState = BlockMarkerState::Unmarking;
    selectedBlock->blockState = BlockState::Moving;
    nextBlock->blockState = BlockState::Moving;
}

void Board::SwapBlock(Block* block) 
{
    // Visually updates the position of the block in the direction the swap will be made
    // After the change is visually performed, the state of the block changes to Moved
    switch (block->dir)
    {
        case Dir::Left:

            if (block->rect.x <= (block->x-1)*TILE_SIZE + BOARD_LEFT_PADDING)
            {
                block->rect.x = (block->x - 1) * TILE_SIZE + BOARD_LEFT_PADDING;
                block->blockState = BlockState::Moved;
            }
            else
            {
                block->rect.x -= FALLINGSPEED* DeltaTime;
            }
        break;

        case Dir::Right:
            if (block->rect.x >= (block->x + 1) * TILE_SIZE + BOARD_LEFT_PADDING)
            {
                block->rect.x = (block->x + 1) * TILE_SIZE + BOARD_LEFT_PADDING;
                block->blockState = BlockState::Moved;
            }
            else
            {
                block->rect.x += FALLINGSPEED * DeltaTime;
            }
        break;

        case Dir::Up:

            if (block->rect.y <= (block->y - 1) * TILE_SIZE + BOARD_UP_PADDING)
            {
                block->rect.y = (block->y - 1) * TILE_SIZE + BOARD_UP_PADDING;
                block->blockState = BlockState::Moved;
            }
            else
            {
                block->rect.y -= FALLINGSPEED * DeltaTime;
            }
        break;

        case Dir::Down:
            if (block->rect.y >= (block->y + 1) * TILE_SIZE + BOARD_UP_PADDING)
            {
                block->rect.y = (block->y + 1) * TILE_SIZE + BOARD_UP_PADDING;
                block->blockState = BlockState::Moved;
            }
            else
            {
                block->rect.y += FALLINGSPEED * DeltaTime;
            }
        break;
    }
}

void Board::SwapMovement() {
    // Changes the block visually and after it has been moved, it changes the block inside the matrix
    if (selectedBlock == nullptr || nextBlock == nullptr)
        return;
    if (selectedBlock->blockState == BlockState::Moving || nextBlock->blockState == BlockState::Moving) {
        SwapBlock(selectedBlock);
        SwapBlock(nextBlock);
        return;
    }

    std::swap(selectedBlock->x, nextBlock->x);
    std::swap(selectedBlock->y, nextBlock->y);
    std::swap(*selectedBlock, *nextBlock);


    selectedBlock->blockMarkerState = BlockMarkerState::None;
    selectedBlock->blockState = BlockState::Normal;
    nextBlock->blockMarkerState = BlockMarkerState::None;
    nextBlock->blockState = BlockState::Normal;
    UnmarkBlock();

}

Block* Board::CheckBlockHover(SDL_Point mousePosition) {
    // Returns the block that is hover by the mouse position, or null if the mouse is outside the board
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (SDL_PointInRect(&mousePosition, &grid[x][y].rect)) {
                return &grid[x][y];
            }
        }
    }
    return nullptr;
}

void Board::UpdateBlockOnDrag(SDL_Point mousePosition) 
{
    // Check if it is interactive, only proceed if it is true
    if (!interactive) 
        return;

    // Check there is any selected block, only proceed if it is true
    if (selectedBlock == nullptr)
        return;

    // Get the diference between mouse position and selected block position
    int diffx = mousePosition.x - ((selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING) + TILE_SIZE / 2);
    int diffy = mousePosition.y - ((selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING) + TILE_SIZE / 2);

    // Saves the mouse direction comparing to the selected block
    Dir mouseDir;
    if (abs(diffy) > abs(diffx)) {
        if (diffy > 0) 
        {
            mouseDir = Dir::Down;

            // If there is no block to change in that direction, it updates the block position to its original position.
            if (selectedBlock->y == 7)
            {
                SetBlockOriginPos(selectedBlock);
                return;
            }
        }
        else
        {
            mouseDir = Dir::Up;

            // If there is no block to change in that direction, it updates the block position to its original position.
            if (selectedBlock->y == 0)
            {
                SetBlockOriginPos(selectedBlock);
                return;
            }
        }
    }
    else {
        if (diffx > 0)
        {
            mouseDir = Dir::Right;

            // If there is no block to change in that direction, it updates the block position to its original position.
            if (selectedBlock->x == 7)
            {
                SetBlockOriginPos(selectedBlock);
                return;
            }
        }
        else
        {
            mouseDir = Dir::Left;

            // If there is no block to change in that direction, it updates the block position to its original position.
            if (selectedBlock->x == 0)
            {
                SetBlockOriginPos(selectedBlock);
                return;
            }
        }
    }

    // If the mouse is in the center of the selected block, the block position is updated to its original position
    if (abs(diffx) < TILE_SIZE/4 && abs(diffy) < TILE_SIZE / 4)
    {
        SetBlockOriginPos(selectedBlock);
        return;
    }
    // If the mouse is moved far enough away from the selected block, an attempt is made to change
    else if (abs(diffx) > TILE_SIZE*0.5f || abs(diffy) > TILE_SIZE * 0.5f)
    {
        switch (mouseDir)
        {
        case Dir::Left:
            selectedBlock->rect.x = (selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING) - TILE_SIZE * 0.5f;
            selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;
            nextBlock = &grid[selectedBlock->x-1][selectedBlock->y];
            break;
        case Dir::Right:
            selectedBlock->rect.x = (selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING) + TILE_SIZE * 0.5f;
            selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;
            nextBlock = &grid[selectedBlock->x + 1][selectedBlock->y];
            break;
        case Dir::Up:
            selectedBlock->rect.y = (selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING) - TILE_SIZE * 0.5f;
            selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;
            nextBlock = &grid[selectedBlock->x][selectedBlock->y - 1];
            break;
        case Dir::Down:
            selectedBlock->rect.y = (selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING) + TILE_SIZE * 0.5f;
            selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;
            nextBlock = &grid[selectedBlock->x][selectedBlock->y + 1];
            break;
        default:
            break;
        }
        StartSwapping();
        return;
    }
    // If the mouse is close to any direction of another block, it is visually approximated to the block that the exchange attempt will be made.
    else
    {
        selectedBlock->blockMarkerState = BlockMarkerState::Marking;
        switch (mouseDir)
        {
            case Dir::Left:
                selectedBlock->rect.x = mousePosition.x - (TILE_SIZE / 2);
                selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;
                break;
            case Dir::Right:
                selectedBlock->rect.x = mousePosition.x - (TILE_SIZE / 2);
                selectedBlock->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;
                break;
            case Dir::Up:
                selectedBlock->rect.y = mousePosition.y - (TILE_SIZE / 2);
                selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;
                break;
            case Dir::Down:
                selectedBlock->rect.y = mousePosition.y - (TILE_SIZE/2);
                selectedBlock->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;
                break;
            default:
                break;
        }
    }
}

void Board::DrawPoints() {

    SDL_Color Black = { 0, 0, 0 };
    SDL_Color White = { 255, 255, 255 };

    string score_text = "score: " + std::to_string(points);


    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, const_cast<char*>(score_text.c_str()), Black);

    // Convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = SCREEN_WIDTH/2 - surfaceMessage->w/2;  //controls the rect's x coordinate 
    Message_rect.y = 35; // controls the rect's y coordinte
    Message_rect.w = surfaceMessage->w; // controls the width of the rect
    Message_rect.h = surfaceMessage->h; // controls the height of the rect

    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    // Don't forget to free your surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}



void Board::SetBlockOriginPos(Block* block) 
{
    // Updates the position of a block to its original position
    block->rect.x = selectedBlock->x * TILE_SIZE + BOARD_LEFT_PADDING;
    block->rect.y = selectedBlock->y * TILE_SIZE + BOARD_UP_PADDING;

    // Updates the size of a block to its original size
    block->rect.w = TILE_SIZE;
    block->rect.h = TILE_SIZE;
}

bool Board::CheckMatchPoint()
{
    // Another temporary auxiliary board is created with the data of the main board but with the future exchange of blocks
    // Returns true if there is a match, and false otherwise.
    Block auxGrid[8][8];

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            auxGrid[x][y] = grid[x][y];
        }

    }

    std::swap(auxGrid[selectedBlock->x][selectedBlock->y].x, auxGrid[nextBlock->x][nextBlock->y].x);
    std::swap(auxGrid[selectedBlock->x][selectedBlock->y].y, auxGrid[nextBlock->x][nextBlock->y].y);
    std::swap(auxGrid[selectedBlock->x][selectedBlock->y], auxGrid[nextBlock->x][nextBlock->y]);


    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8 - 2; x++)
        {
            if ((auxGrid[x][y].blockColor == auxGrid[x + 1][y].blockColor) && (auxGrid[x][y].blockColor == auxGrid[x + 2][y].blockColor)) {
                return true;
            }
        }
    }

    for (int y = 0; y < 8-2; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if ((auxGrid[x][y].blockColor == auxGrid[x][y+1].blockColor) && (auxGrid[x][y].blockColor == auxGrid[x][y+2].blockColor)) {
                return true;
            }
        }
    }
    return false;
}

void Board::SetMatchPoint() {
    // Check if there are any matches on the entire board
    // Changes block state to dead in all match sequences
    // 0.6 second interval for scores
    matchTimer += DeltaTime;
    if (matchTimer >= 0.6f) {
        matchTimer = 0;
    }
    else {
        return;
    }

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            // Check horizontal lines
            int count = 1;
            while (x + count < 8 && grid[y][x + count].blockColor == grid[y][x].blockColor) {
                ++count;
            }
            if (count >= 3) {
                // Set blocks to dead to disappear
                for (int i = 0; i < count; ++i) {
                    grid[y][x + i].blockState = BlockState::Dead;
                    points++;
                }
            }

            // Check vertical lines
            count = 1;
            while (y + count < 8 && grid[y + count][x].blockColor == grid[y][x].blockColor) {
                ++count;
            }
            if (count >= 3) {
                // Set blocks to dead to disappear
                for (int i = 0; i < count; ++i) {
                    grid[y+i][x].blockState = BlockState::Dead;
                    points++;
                }
            }
        }
    }
}

void Board::CheckMouseClick(SDL_Point mousePosition) 
{
    // Check if it is interactive, only proceed if it is true
    if (!interactive) 
    {
        return;
    }
        
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            if (SDL_PointInRect(&mousePosition, &grid[x][y].rect)) 
            {
                // If there is no block selected, the block clicked becomes the selected one.
                if (selectedBlock == nullptr) 
                {
                    selectedBlock = &grid[x][y];
                }
                else {
                    // If the block clicked is the block that is already selected, it becomes deselected.
                    if (selectedBlock == &grid[x][y]) 
                    {
                        UnmarkBlock();
                    }
                    else 
                    {
                        // If there is already a block selected and the block that was clicked on is in a valid position for replacement (up, down, left or right), an attempt is made to replace it.
                        if (((abs(selectedBlock->x - grid[x][y].x) == 1) && (abs(selectedBlock->y - grid[x][y].y) == 0)) || ((abs(selectedBlock->x - grid[x][y].x) == 0) && (abs(selectedBlock->y - grid[x][y].y) == 1)))
                        {
                            nextBlock = &grid[x][y];
                            StartSwapping();
                        }
                        // If there is already a selected block and the block that was clicked on is NOT in a valid position for replacement, the selected block is replaced by the clicked block
                        else 
                        {
                            UnmarkBlock();
                            selectedBlock = &grid[x][y];
                        }
                    }
                }

                return;
            }
        }
    }
    // If there is a block selected and it is clicked outside the board
    if (selectedBlock != nullptr) 
        UnmarkBlock();
}

string Board::GetColorName(BlockColor blockColor)
{

    switch (blockColor) {
        case BlockColor::Black :
            return "Black";
            break;
        case BlockColor::White:
            return "White";
            break;
        case BlockColor::Pink:
            return "Pink";
            break;
        case BlockColor::Blue:
            return "Blue";
            break;
        case BlockColor::Orange:
            return "Orange";
            break;
    }
}

string Board::GetDirName(Dir dir)
{
    switch (dir) {
    case Dir::Left:
        return "Left";
        break;
    case Dir::Right:
        return "Right";
        break;
    case Dir::Up:
        return "Up";
        break;
    case Dir::Down:
        return "Down";
        break;
    case Dir::NONE:
        return "NONE";
        break;
    }
}

string Board::BlockDescription(Block block) 
{
    string res = "";
    res += "(x:" + std::to_string(block.x) + " , " + "y:" + std::to_string(block.y) +")";
    res += "\t|\t";
    res += "Color: " + GetColorName(block.blockColor);
    res += "\t|\t";
    res += "Dir:" + GetDirName(block.dir);
    return res;
}



