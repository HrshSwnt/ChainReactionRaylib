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
float cpuValue;
Rectangle menuRect;
Rectangle rowSliderRect;
Rectangle colSliderRect;
Rectangle playerSliderRect;
Rectangle CPUSliderRect;
Rectangle buttonRect;
Rectangle restartButtonRect;
Rectangle exitButtonRect;
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
	InitWindow(1920, 1080, "Chain Reaction Raylib"); // Initial dummy size, overridden by canvas style in WASM
	SetTargetFPS(60);


	frameCount = 0;
	gameState = GAME_STATE_START;

	rowValue = 6.0f; // Default value for rows
	colValue = 6.0f; // Default value for columns
	playerValue = 3.0f; // Default value for players
	cpuValue = 1.0f; // Default value for CPU players

	// Make menuRect twice as big, leave space at the top
	resizeAssets(GetScreenWidth(), GetScreenHeight());

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


		// Show FPS
		DrawFPS(10, 200);
		
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
