#include "raylib-cpp.hpp"
#include <iostream>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <string>
#include <sstream>


enum class TEMPLATES
{
	I_BLOCK_TEMPLATE,
	J_BLOCK_TEMPLATE,
	L_BLOCK_TEMPLATE,
	O_BLOCK_TEMPLATE,
	S_BLOCK_TEMPLATE,
	T_BLOCK_TEMPLATE,
	Z_BLOCK_TEMPLATE,
	INVALID
};

const char TEMPLATE_ARR[7][2][4]{
	 {
	{'x', 'x', 'x', 'x'},
	{' ', ' ', ' ', ' '}
},

{
	{'x',' ', ' ', ' '},
	{'x', 'x', 'x', ' '}
},

 {
	{' ',' ', ' ', 'x'},
	{' ', 'x', 'x', 'x'}
},

 {
	{' ','x', 'x', ' '},
	{' ', 'x', 'x', ' '}
},

 {
	{' ',' ', 'x', 'x'},
	{' ', 'x', 'x', ' '}
},

 {
	{' ','x', ' ', ' '},
	{'x', 'x', 'x', ' '}
},

 {
	{'x','x', ' ', ' '},
	{' ', 'x', 'x', ' '}
}
};

std::map<TEMPLATES, char> TEMPLATE_CHAR_MAP{
	{TEMPLATES::I_BLOCK_TEMPLATE, 'i'},
	{TEMPLATES::J_BLOCK_TEMPLATE, 'j'},
	{TEMPLATES::L_BLOCK_TEMPLATE, 'l'},
	{TEMPLATES::O_BLOCK_TEMPLATE, 'o'},
	{TEMPLATES::S_BLOCK_TEMPLATE, 's'},
	{TEMPLATES::T_BLOCK_TEMPLATE, 't'},
	{TEMPLATES::Z_BLOCK_TEMPLATE, 'z'},
	{TEMPLATES::INVALID, 'f'}
};

std::map<char, Color> BLOCK_COLOR_MAP {
	{'i', SKYBLUE},
	{'j', DARKBLUE},
	{'l', ORANGE},
	{'o', YELLOW},
	{'s', GREEN},
	{'t', PURPLE},
	{'z', RED}
};

const int GRID_ROWS = 24;
const int GRID_COLUMNS = 10;

bool drawBlockTemplate(char grid[24][10], TEMPLATES t, int row, int col) {
	bool overlap = false;
	for (int i = row; i < row + 2; ++i)
		for (int j = col; j < col + 4; ++j) {
			float posX = ((float)285 / 10) * j;
			float posY = ((float)684 / 24) * i;
			if (TEMPLATE_ARR[(int)t][i - row][j - col] == 'x') {
				if (grid[i][j] != ' ')
					overlap = true;
				grid[i][j] = 'x';
			}
		}
	return !overlap;
}

void rotateFalling(char grid[24][10]) {
	bool canRotate = true;
	int pXMin = 23, pYMin = 9, pXMax = 0, pYMax = 0;
	for (int i = 23; i >= 0; i--)
		for (int j = 9; j >= 0; j--)
			if (grid[i][j] == 'x') {
				pXMin = std::min(pXMin, i);
				pXMax = std::max(pXMax, i);

				pYMin = std::min(pYMin, j);
				pYMax = std::max(pYMax, j);
			}
	float pX = roundf((float) (pXMin + pXMax) / 2);
	float pY = roundf((float) (pYMin + pYMax) / 2);

	char gridAlt[24][10] = { 0 };

	for (int i = 23; i >= 0; i--)
		for (int j = 9; j >= 0; j--)
			if (grid[i][j] == 'x') {
				if (i == pX && j == pY) {
					gridAlt[i][j] = 'x';
					continue;
				}
				float newI, newJ;
				newI = pX - (j - pY);
				newJ = (pY + i) - pX;

				if ((int)roundf(newI) < 0 || (int)roundf(newI) > 23 || (int)roundf(newJ) < 0 || (int)roundf(newJ) > 9) // blyat
					return;
				gridAlt[(int) roundf(newI)][(int) roundf(newJ)] = 'x';
			}

	for (int i = 23; i >= 0; i--)
		for (int j = 9; j >= 0; j--)
			if (grid[i][j] == 'x') {
				grid[i][j] = ' ';
			}

	for (int i = 23; i >= 0; i--)
		for (int j = 9; j >= 0; j--)
			if (gridAlt[i][j] == 'x') {
				grid[i][j] = 'x';
			}
}

TEMPLATES spawnNextBlock(char grid[24][10], TEMPLATES t) {
	// decide where to spawn
	int row = 0;
	int col = rand() % 6; //generate a random between 0-5

	// spawn it
	bool ableToDraw = drawBlockTemplate(grid, t, row, col);
	if (!ableToDraw)
		return TEMPLATES::INVALID;

	// decide on next template
	int templatenumber = rand() % 7; 
	return (TEMPLATES)templatenumber;
}

// show template function
void showBlock(int positionX, int positionY, TEMPLATES t) {
	
	for (int curRow = 0; curRow < 2; curRow++) {
		for (int curCol = 0; curCol < 4; curCol++) {
			float offsetX = ((float)25) * curCol;
			float offsetY = ((float)25) * curRow;

			// check if template grid has 'x'
			if (TEMPLATE_ARR[(int)t][curRow][curCol] == 'x') {
				// draw x onto the screen based on positionX + positionY
				DrawRectangle(offsetX + positionX, offsetY + positionY, 25, 25, BLOCK_COLOR_MAP[TEMPLATE_CHAR_MAP[t]]);
				DrawRectangleLines(offsetX + positionX, offsetY + positionY, 26, 26, RAYWHITE);

			}
		}

	}
}

void PrintCenterText(const char* text, int screenWidth, int screenHeight) {
	DrawText(text, (screenWidth / 2) - (MeasureText(text, 40) / 2), screenHeight / 2, 40, BLOCK_COLOR_MAP[TEMPLATE_CHAR_MAP[(TEMPLATES)(rand() % 7)]]);
}

void ClearFallingBlock(char grid[24][10]) {
	for (int i = 23; i >= 0; i--) {
		for (int j = 9; j >= 0; j--) {
			if (grid[i][j] == 'x') {
				grid[i][j] = ' ';
			}
			
		}
		
	}

}

void blinkRow(char grid[24][10], int row) {
	float posY = ((float)684 / 24) * row;
	float time = 1.0f;
	while (time > 0.0f) {
		BeginDrawing();
		time -= GetFrameTime();
		if (((int)(time * 10)) % 2 == 0) {
			for (int j = 0; j < 10; j++) {
				float posX = ((float)285 / 10) * j;
				DrawRectangle(50 + posX, 50 + posY, ((float)285 / 10), ((float)684 / 24), BLOCK_COLOR_MAP[grid[row][j]]);
				DrawRectangleLines(50 + posX, 50 + posY, ((float)285 / 10) + 1, ((float)684 / 24) + 1, RAYWHITE);
			}
		}
		else {
			DrawRectangle(50, 50 + posY, 285, ((float)684 / 24), BLOCK_COLOR_MAP[TEMPLATE_CHAR_MAP[(TEMPLATES)(rand() % 7)]]);
		}
		EndDrawing();
	}
}

int main()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	srand(time(NULL));

	int screenWidth = 500;
	int screenHeight = 812;

	int playWidth = 285;
	int playHeight = 684;

	int nextBlockPreviewBoxX = 370;
	int nextBlockPreviewBoxY = 50;

	int holdBlockPreviewBoxX = 370;
	int holdBlockPreviewBoxY = 150;


	float timePerDrop = 0.5;
	float lastTimePerDrop = 0.5;
	float fastCountdown = 2.0f;
	int fastBlocksToDrop = 5;
	bool fastMeme = false;

	char playGrid[GRID_ROWS][GRID_COLUMNS] = {	
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} ,
		{' ',' ',' ',' ',' ',' ',' ',' ',' ',' '} 
	};	
	raylib::Color textColor(LIGHTGRAY);
	raylib::Window w(screenWidth, screenHeight, "now streaming [local audio]");

	SetTargetFPS(60);
	float comSec = 0.0;
	int bottomSect = 24;
	int scorePoints = 0;
	std::string scoreShow = "Score: ";
	std::string actualScore;


	TEMPLATES curTemplate = (TEMPLATES)(rand() % 7);
	TEMPLATES nextTemplate = spawnNextBlock(playGrid, curTemplate);
	TEMPLATES holdTemplate = TEMPLATES::INVALID;
	TEMPLATES showholdTemplate = holdTemplate;
	TEMPLATES shownextTemplate = nextTemplate;

	bool gameOver = false;
	bool gameStarted = false;
	bool droppingSameBlock = true;
	bool goFast = false;
	bool holdingBlock = false;
	//--------------------------------------------------------------------------------------
	InitAudioDevice();
	while (!IsAudioDeviceReady)
		continue;

	Sound goFastMusic = LoadSound("fast.wav");
	Camera2D goFastCamera = { 0 };
	goFastCamera.zoom = 3.0;

	// Main game loop
	while (!w.ShouldClose())    // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		comSec += GetFrameTime();
		if (goFast && fastCountdown > 0.0f) {
			PrintCenterText("PREPARE FOR FAST IN", screenWidth, screenHeight - 20);

			PrintCenterText(std::to_string(fastCountdown).c_str(), screenWidth, screenHeight + 40);
			fastCountdown -= GetFrameTime();
		}
		if (fastCountdown <= 0.0f)
			timePerDrop = 0.0;
		//----------------------------------------------------------------------------------

		// game over check
		for (int j = 0; j < 10; j++) {
			if (playGrid[0][j] != ' ' && playGrid[0][j] != 'x') {
				gameOver = true;
				gameStarted = false;
				break;
			}
		}
		
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		if (timePerDrop == 0.0 && fastCountdown <= 0.0f) {
			goFastCamera.zoom = (rand() % 251 + 101) / (float) 100;
			BeginMode2D(goFastCamera);
		}

		ClearBackground(BLACK);
		DrawText("TETRIS", 50, 30, 20, BLOCK_COLOR_MAP[TEMPLATE_CHAR_MAP[curTemplate]]);
		
		// grid & misc
		for (int i = 0; i < 10; i++)
			DrawRectangle(50 + (((float)285 / 10) * i), 50, ((float)684 / 24), playHeight, (i % 2 != 0) ? BLACK : Color{ 80, 80, 80, 200 });
		DrawRectangleLines(50, 50, playWidth, playHeight, RAYWHITE);

		DrawText("Next Block:", nextBlockPreviewBoxX, nextBlockPreviewBoxY - 20, 18, RAYWHITE);
		DrawRectangleLines(nextBlockPreviewBoxX, nextBlockPreviewBoxY, 100, 50, RAYWHITE);
		if (nextTemplate != TEMPLATES::INVALID) {
			showBlock(nextBlockPreviewBoxX, nextBlockPreviewBoxY, nextTemplate);
		}

		DrawText("Held Block:", holdBlockPreviewBoxX, holdBlockPreviewBoxY - 20, 18, RAYWHITE);
		DrawRectangleLines(holdBlockPreviewBoxX, holdBlockPreviewBoxY, 100, 50, RAYWHITE);
		if (holdTemplate != TEMPLATES::INVALID) {
			showBlock(holdBlockPreviewBoxX, holdBlockPreviewBoxY, holdTemplate);
		}
		// instructions
		DrawText("Right Arrow: \n Moves the block \n to the right", 350, 290, 15, ORANGE);
		DrawText("Left Arrow: \n Moves the block \n to the left", 350, 360, 15, GREEN);
		DrawText("Up Arrow \n Rotates the block", 350, 435, 15, PURPLE);
		DrawText("H Key: \n Holds the block/ \n spawn held block", 350, 490, 15, SKYBLUE);
		DrawText("Space Key: \n Insta-drops block", 350, 570, 15, RED);

		// score text
		std::stringstream scoreInt;
		scoreInt << scoreShow << scorePoints;
		DrawText(scoreInt.str().c_str(), 370, 240, 20, RAYWHITE);

		// update new falling block positions to x
		for (int i = 0; i < 24; ++i)
			for (int j = 0; j < 10; ++j) {
				float posX = ((float)285 / 10) * j;
				float posY = ((float)684 / 24) * i;
				if (playGrid[i][j] == 'y') {
					playGrid[i][j] = 'x';
					goFastCamera.target.x = posX;
					goFastCamera.target.y = posY;
					goFastCamera.offset.x = screenWidth / 2;
					goFastCamera.offset.y = screenHeight / 2;
				}
			}		

		// render grid
		for (int i = 0; i < 24; ++i)
			for (int j = 0; j < 10; ++j) {
				float posX = ((float)285 / 10) * j;
				float posY = ((float)684 / 24) * i;
				if (playGrid[i][j] != ' ') {
					if (playGrid[i][j] == 'x')
						DrawRectangle(50 + posX, 50 + posY, ((float)285 / 10), ((float)684 / 24), BLOCK_COLOR_MAP[TEMPLATE_CHAR_MAP[curTemplate]]);
					else 
						DrawRectangle(50 + posX, 50 + posY, ((float)285 / 10), ((float)684 / 24), BLOCK_COLOR_MAP[playGrid[i][j]]);
					DrawRectangleLines(50 + posX, 50 + posY, ((float)285 / 10) + 1, ((float)684 / 24) + 1, RAYWHITE	);
				}

			}

		if (!gameStarted) {
			if (!gameOver)
				PrintCenterText("SPACE TO START", screenWidth, screenHeight);
			else {
				scorePoints = 0;
				if ((int)comSec % 2 == 0)
					PrintCenterText("GAME OVER", screenWidth, screenHeight);
				else
					PrintCenterText("SPACE TO RESTART", screenWidth, screenHeight);
			}
			if (IsKeyPressed(KEY_SPACE)) {
				comSec = 0.0;
				gameStarted = true;
				if (gameOver) {
					// clear the grid
					for (int i = 0; i < 24; i++)
						for (int j = 0; j < 10; j++)
							playGrid[i][j] = ' ';

					// spawn a block
					curTemplate = (TEMPLATES)(rand() % 7);
					nextTemplate = spawnNextBlock(playGrid, curTemplate);

					// not gameover anymore
					gameOver = false;
				}
			}
			EndDrawing();
			continue;
		}

		if (timePerDrop == 0.0 && fastCountdown <= 0.0f)
			EndMode2D();

		//increased speed power ups
		/*if (scorePoints == 500) {
			timePerDrop = 0.4;
		}

		if (scorePoints == 1000) {
			timePerDrop = 0.35;
		}

		if (scorePoints == 2000) {
			timePerDrop = 0.25;
		}
		if (scorePoints == 4000) {
			timePerDrop = 0.15;
		}*/

		if (IsKeyPressed(KEY_SPACE) && droppingSameBlock) {
			if (fastMeme) {
				PlaySound(goFastMusic);
				goFast = true;
			}
			else {
				lastTimePerDrop = timePerDrop;
				timePerDrop = 0.0;
			}
		}
		else if (IsKeyReleased(KEY_SPACE) && !goFast) {
			StopSound(goFastMusic);
			timePerDrop = lastTimePerDrop;
			droppingSameBlock = true;
			goFast = false;
			fastCountdown = 2.0f;
			fastBlocksToDrop = 5;
		}

		if (IsKeyPressed(KEY_UP)) {
			rotateFalling(playGrid);
		}

		// right movement
		if (IsKeyPressed(KEY_RIGHT)) {
			bool gotSpace = true;
			for (int i = 0; i < 23; i++) {
				for (int j = 9; j >= 0; j--)
					if (playGrid[i][j] == 'x')
						if (j == 9 || (playGrid[i][j + 1] != ' ' && playGrid[i][j + 1] != 'x'))
							gotSpace = false;
			}

			if (gotSpace)
				for (int i = 23; i >= 0; i--) {
					for (int j = 9; j >= 0; j--)
						if (playGrid[i][j] == 'x') {
							playGrid[i][j + 1] = 'x';
							playGrid[i][j] = ' ';

						}

				}
		}

		// left movement
		if (IsKeyPressed(KEY_LEFT)){
			bool gotSpace = true;
			for (int i = 0; i < 23; i++) {
				for (int j = 0; j < 10; j++)
					if (playGrid[i][j] == 'x')
						if (j == 0 || (playGrid[i][j - 1] != ' ' && playGrid[i][j - 1] != 'x'))
							gotSpace = false;
			}

			if (gotSpace)
				for (int i = 23; i >= 0; i--) {
					for (int j = 0; j <= 9; j++)
						if (playGrid[i][j] == 'x') {
							playGrid[i][j - 1] = 'x';
							playGrid[i][j] = ' ';

						}

				}
		}
		//holding a template
		if (IsKeyPressed(KEY_H)) {
			TEMPLATES copyHold = holdTemplate;
			if (copyHold == TEMPLATES::INVALID) {
				holdTemplate = curTemplate;
				curTemplate = nextTemplate;
				ClearFallingBlock(playGrid);
				nextTemplate = spawnNextBlock(playGrid, curTemplate);
			}
			else {
				holdTemplate = curTemplate;
				curTemplate = copyHold;
				ClearFallingBlock(playGrid);
				spawnNextBlock(playGrid, curTemplate);
			}
			
		}




		// update falling block
		if (comSec >= timePerDrop) 
		{

			bool collision = false;
			for (int i = 23; i >= 0; i--) {
				// check entire row for collision
				for (int j = 0; j < 10; j++)
				{
					if (playGrid[i][j] == 'x') {
						if (i == 23 || (playGrid[i + 1][j] != ' ' && playGrid[i + 1][j] != 'x')) {
							collision = true;
							goto col;
						}

					}
				}
			}
			// update + clear row
			for (int i = 23; i >= 0; i--)
				for (int j = 0; j < 10; j++)
				{
					if (playGrid[i][j] == 'x') {
						playGrid[i + 1][j] = 'y';
						playGrid[i][j] = ' ';
					}
				}

			col:
			if (collision) {
				droppingSameBlock = false;
				for (int i = 23; i >= 0; i--)
					for (int j = 0; j < 10; j++)
					{
						if (playGrid[i][j] == 'x') {
							playGrid[i][j] = TEMPLATE_CHAR_MAP[curTemplate];

						}
					}

				// row clearing
				for (int i = 0; i < 24; i++) {
					bool rowIsFilled = true;
					for (int j = 0; j < 10; j++) {					
						if (playGrid[i][j] == ' ')
						{
							rowIsFilled = false;
						}
					}

					if (!rowIsFilled)
						continue;

					/*EndDrawing();
					blinkRow(playGrid, i);
					BeginDrawing();*/
					for (int j = 0; j < 10; j++)
						playGrid[i][j] = ' ';

					for (int x = i; x > 0; x--)
						for (int y = 0; y < 10; y++)
							playGrid[x][y] = playGrid[x - 1][y];
					scorePoints += 100;
				}
				
 				curTemplate = nextTemplate;
				nextTemplate = spawnNextBlock(playGrid, curTemplate);
				if (!goFast)
					timePerDrop = lastTimePerDrop;
				else fastBlocksToDrop--;
				if (fastBlocksToDrop == 0) {
					fastCountdown = 2.0f;
					timePerDrop = lastTimePerDrop;
					goFast = false;
					fastBlocksToDrop = 5;
					StopSound(goFastMusic);
				}
				

				if (nextTemplate == TEMPLATES::INVALID)
					gameOver = true;
			}

			
			comSec = 0.0;
		}
		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	
	CloseAudioDevice();

	return 0;
}
