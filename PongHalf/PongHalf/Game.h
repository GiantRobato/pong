#pragma once
#include "SDL/SDL.h"

#include <vector>

/*
properties c/c++
additional include directories: SDL\include;
Linker -> General
Additional library directories: SDL\lib\win\x86
input -> additional dependencies
SDL2.lib; SDL2main.lib
post build events:
//copy sdl dlls into out dir
// i - destination is a folder
// s - copies folders and sub folders
// y - overwrites existing without prompting you
xcopy "$(ProjectDir)\..\external\SDL\lib\win\x86\*.dll" "$(OutDir)" /i /s /y
*/

struct Vector2 {
	float x;
	float y;
};

struct Ball {
	Vector2 pos;
	Vector2 vel;
};

class Game {
public:
	Game();
	bool Initialize();
	void RunLoop();
	void ShutDown();
private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	float GetDeltaTime(); //helper function

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	bool mIsRunning;
	//player 1 data
	Vector2 mPaddlePos;
	int mPaddleDir;

	//player 2 data
	Vector2 mPaddle2Pos;
	int mPaddleDir2;

	Vector2 mBallPos;
	Uint32 mTicksCount;
	Vector2 mBallVel;

	std::vector<Ball> mBalls;
};

