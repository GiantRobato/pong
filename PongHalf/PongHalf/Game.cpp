#include "Game.h"

const int thickness = 15;
const float paddleSize = 100.0f;

// TODO: add player 2 collision

Game::Game():mWindow(nullptr),mIsRunning(true) {

}

bool Game::Initialize() {
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to init SDL: %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"Pong!",
		100, 100,
		1024, 768,
		0
	);

	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	mPaddlePos.x = 10.0f;
	mPaddlePos.y = 768.0f / 2.0f;
	mPaddle2Pos.x = 1024 - 10.0f - thickness;
	mPaddle2Pos.y = 768.0f / 2.0f;
	mBallPos.x = 1024.0f / 2.0f;
	mBallPos.y = 768.0f / 2.0f;
	mTicksCount = 0;
	mPaddleDir = 0;
	mBallVel.x = -200.0f;
	mBallVel.y = 235.0f;

	//
	mBalls.emplace_back(Ball{ Vector2{600.0f, 768.0f / 2.0f},Vector2{150.0f, 220.0f} });

	return true;
}

void Game::ShutDown() {
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	SDL_Quit();
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event) ){
		switch (event.type) {
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	//handle keyboard inputs
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W]) mPaddleDir -= 1; //up is decreasing y
	if (state[SDL_SCANCODE_S]) mPaddleDir += 1; //down is increasing y

	mPaddleDir2 = 0;
	if (state[SDL_SCANCODE_I]) mPaddleDir2 -= 1; //up is decreasing y
	if (state[SDL_SCANCODE_K]) mPaddleDir2 += 1; //down is increasing y

}

void Game::UpdateGame() {
	float deltaTime = GetDeltaTime();

	//update paddle pos
	if (mPaddleDir != 0) {
		mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
		//clamp paddlePos to edges
		if (mPaddlePos.y < paddleSize / 2.0f + thickness) {
			mPaddlePos.y = paddleSize / 2 + thickness;
		} else if (mPaddlePos.y > 768.0f - (paddleSize/2) + thickness/2) {
			mPaddlePos.y = 768.0f - (paddleSize/2) + thickness/2;
		}
	}

	if (mPaddleDir2 != 0) {
		mPaddle2Pos.y += mPaddleDir2 * 300.0f * deltaTime;
		//clamp paddlePos to edges
		if (mPaddle2Pos.y < paddleSize / 2.0f + thickness) {
			mPaddle2Pos.y = paddleSize / 2 + thickness;
		} else if (mPaddle2Pos.y > 768.0f - (paddleSize / 2) + thickness / 2) {
			mPaddle2Pos.y = 768.0f - (paddleSize / 2) + thickness / 2;
		}
	}

	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;

	//update all balls positions
	for (auto& mBall : mBalls) {
		mBall.pos.x += mBall.vel.x * deltaTime;
		mBall.pos.y += mBall.vel.y * deltaTime;
	}

	if (mBallPos.y <= thickness && mBallVel.y < 0.0f) mBallVel.y *= -1;
	if (mBallPos.y >= 768 - thickness && mBallVel.y > 0.0f) mBallVel.y *= -1;

	//do edge wall detection
	for (auto& mBall : mBalls) {
		if (mBall.pos.y <= thickness && mBall.vel.y < 0.0f) mBall.vel.y *= -1;
		if (mBall.pos.y >= 768 - thickness && mBall.vel.y > 0.0f) mBall.vel.y *= -1;
	}

	//detect collision
	//Paddle 1
	float ydiff = mPaddlePos.y - mBallPos.y;
	bool isAtXPosOfPaddle = mBallPos.x <= 25.0f && mBallPos.x >= 20.0f;
	if (ydiff <= paddleSize / 2.0f &&
		isAtXPosOfPaddle &&
		mBallVel.x < 0.0f) {
		mBallVel.x *= -1.0f;
	}
	ydiff = mPaddle2Pos.y - mBallPos.y;
	isAtXPosOfPaddle = mBallPos.x >= 999.0f && mBallPos.x <= 1009.0f;
	if (ydiff <= paddleSize / 2.0f &&
		isAtXPosOfPaddle &&
		mBallVel.x > 0.0f) {
		mBallVel.x *= -1.0f;
	}

	//paddle collision with all the balls
	for (auto& mBall : mBalls) {
		ydiff = mPaddlePos.y - mBall.pos.y;
		isAtXPosOfPaddle = mBall.pos.x <= 25.0f && mBall.pos.x >= 20.0f;
		if (ydiff <= paddleSize / 2.0f &&
			isAtXPosOfPaddle &&
			mBallVel.x < 0.0f) {
			mBall.vel.x *= -1.0f;
		}
		ydiff = mPaddle2Pos.y - mBall.pos.y;
		isAtXPosOfPaddle = mBall.pos.x >= 999.0f && mBall.pos.x <= 1009.0f;
		if (ydiff <= paddleSize / 2.0f &&
			isAtXPosOfPaddle &&
			mBall.vel.x > 0.0f) {
			mBall.vel.x *= -1.0f;
		}
	}

}

float Game::GetDeltaTime() {
	//rate limit delta time to 60fps
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	//convert to seconds
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	mTicksCount = SDL_GetTicks();

	//clamp deltaTime
	if (deltaTime > 0.05f) deltaTime - 0.05f;
	return deltaTime;
}

//clear backbuffer, draw scene, swap buffers
void Game::GenerateOutput() {
	//set back buffer color
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
	SDL_RenderClear(mRenderer);

	//set color of objects
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	//draw top wall
	SDL_Rect wall{ 0, 0, 1024, thickness};
	SDL_RenderFillRect(mRenderer, &wall);

	SDL_Rect wallBottom{0, 768-thickness, 1024, thickness };
	SDL_RenderFillRect(mRenderer, &wallBottom);

	//draw ball
	SDL_Rect ball{
		static_cast<int>(mBallPos.x - thickness / 2),
		static_cast<int>(mBallPos.y - thickness / 2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);

	//draw all balls
	for (auto mBall : mBalls) {
		SDL_Rect ballRect{
			static_cast<int>(mBall.pos.x - thickness / 2),
			static_cast<int>(mBall.pos.y - thickness / 2),
			thickness,
			thickness
		};
		SDL_RenderFillRect(mRenderer, &ballRect);
	}

	//paddle

	SDL_Rect paddle {
		static_cast<int>(mPaddlePos.x ),
		static_cast<int>(mPaddlePos.y - paddleSize/2),
		thickness,
		5 * thickness
	};
	SDL_RenderFillRect(mRenderer, &paddle);

	SDL_Rect paddle2{
		static_cast<int>(mPaddle2Pos.x),
		static_cast<int>(mPaddle2Pos.y - paddleSize / 2),
		thickness,
		5 * thickness
	};
	SDL_RenderFillRect(mRenderer, &paddle2);

	SDL_RenderPresent(mRenderer);
}

void Game::RunLoop() {
	while (mIsRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}


