/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "main.hpp"
#include "resource_dir.h"


GAME_STATE gameState;
int frameCount;
std::queue<PendingExplosion> explosionQueue;

float rowValue;
float colValue;
float playerValue;
Rectangle menuRect;
Rectangle rowSliderRect;
Rectangle colSliderRect;
Rectangle playerSliderRect;
Rectangle buttonRect;
Rectangle restartButtonRect;
Rectangle undoButtonRect;
Rectangle redoButtonRect;
Shader coreShader;
Shader auraShader;
int baseColorLocCore;
int baseColorLocAura;
int timeLocAura;
int mvpLocCore;
int mvpLocAura;
int main ()
{
	// Tell the window to use vsync and work on high DPI display
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	// Get the current monitor's width and height to set window size dynamically
	InitWindow(1280, 720, "Chain Reaction Raylib"); // Initial dummy size, overridden by canvas style in WASM
	SetTargetFPS(60);


	frameCount = 0;
	gameState = GAME_STATE_START;

	rowValue = 6.0f; // Default value for rows
	colValue = 6.0f; // Default value for columns
	playerValue = 3.0f; // Default value for players

	// Make menuRect twice as big, leave space at the top
	menuRect = { static_cast<float>(GetScreenWidth()/2 - 400), static_cast<float>(GetScreenHeight()/2 - 200), 800, 500 };

	rowSliderRect = { menuRect.x + (menuRect.width - 600) / 2, menuRect.y + 200, 600, 30 };
	colSliderRect = { menuRect.x + (menuRect.width - 600) / 2, menuRect.y + 200 + 60, 600, 30 };
	playerSliderRect = { menuRect.x + (menuRect.width - 600) / 2, menuRect.y + 200 + 2 * 60, 600, 30 };
	buttonRect = { menuRect.x + (menuRect.width - 300) / 2, menuRect.y + 200 + 3 * 60 + 40, 300, 50 };
	restartButtonRect = { static_cast<float>(GetScreenWidth()/2 - 100), static_cast<float>(GetScreenHeight()/2 + 150), 200, 40 };


	// draw the undo and redo buttons at the very left of the screen relative to height, under each other
	redoButtonRect = { 10, static_cast<float>(GetScreenHeight() - 50), 80, 40 };
	undoButtonRect = { 10, static_cast<float>(GetScreenHeight() - 100), 80, 40 };

	// Load resources, initialize game state, etc.
	SearchAndSetResourceDir("resources");
	// Load shaders
	coreShader = LoadShader("cell_core.vs", "cell_core.fs");
	auraShader = LoadShader("cell_aura.vs", "cell_aura.fs");
	// Get shader locations
	baseColorLocCore = GetShaderLocation(coreShader, "baseColor");
	baseColorLocAura = GetShaderLocation(auraShader, "baseColor");
	timeLocAura = GetShaderLocation(auraShader, "time");
	mvpLocCore = GetShaderLocation(coreShader, "mvp");
	mvpLocAura = GetShaderLocation(auraShader, "mvp");

	int prevWidth = GetScreenWidth();
	int prevHeight = GetScreenHeight();
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{

		int newWidth = GetScreenWidth();
		int newHeight = GetScreenHeight();

		if (newWidth != prevWidth || newHeight != prevHeight) {
			SetWindowSize(newWidth, newHeight); // Update raylib's internal framebuffer size
			resizeAssets(newWidth, newHeight); // Resize any assets that depend on window size
			prevWidth = newWidth;
			prevHeight = newHeight;
    	}

		incrementFrameCount();
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);


		
		
		switch (gameState)
		{
			case GAME_STATE_START:
				drawStart();
				break;
			case GAME_STATE_MENU:
				drawMenu();
				break;
			case GAME_STATE_PLAYING:
				drawPlaying();
				drawValidCursor();
				moveCameraviaRightClick();
				zoomCameraViaScroll();
				resetCameraviaMiddleClick();
				break;
			case GAME_STATE_GAME_OVER:
				drawGameOver();
				moveCameraviaRightClick();
				zoomCameraViaScroll();
				resetCameraviaMiddleClick();
				break;
			case GAME_STATE_EXIT:
				drawExit();
				break;
			default:
				break;
		}

		mousePressed();
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
