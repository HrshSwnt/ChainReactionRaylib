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
std::queue<PendingExplosion> nextQueue;
Shader cellShader;
int baseColorLoc;
int main ()
{
	// Tell the window to use vsync and work on high DPI display
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1920, 1080, "Hello Raylib");
	SetTargetFPS(60);	// Set the target frames per second to 60

	frameCount = 0;
	gameState = GAME_STATE_START;

	// Load resources, initialize game state, etc.
	SearchAndSetResourceDir("resources");
	cellShader = LoadShader("cell.vs", "cell.fs");
	baseColorLoc = GetShaderLocation(cellShader, "baseColor");

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
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
				break;
			case GAME_STATE_GAME_OVER:
				drawGameOver();
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
