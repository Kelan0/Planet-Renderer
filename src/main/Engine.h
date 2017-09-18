#pragma once
#include <SDL.h>
#include "renderer/ShaderProgram.h"
#include <vector>

#define ERROR_WINDOW_INITIALIZATION 0x1
#define ERROR_PHYSICS_INITIALIZATION 0x2

class Engine
{
private:
	void(*initFunc)();
	void(*stopFunc)();
	void(*renderFunc)(double delta);
	void(*physicsFunc)(double delta);
	void(*inputFunc)(std::vector<SDL_Event> events, double delta);

	SDL_Thread* _physicsThread;
	bool _doRun;
	float _framerateLimit;
	float _tickrateLimit;
	float _framerateCurrent;
	float _tickrateCurrent;

public:
	Engine(void(*initFunc)(), void(*stopFunc)(), void(*renderFunc)(double delta), void(*physicsFunc)(double delta), void(*inputFunc)(std::vector<SDL_Event> events, double delta));
	~Engine();

	void setFramerate(float framerate);
	
	void setTickrate(float tickrate);

	void handleInput(SDL_Event& event);
	
	int start(unsigned int windowWidth, unsigned int windowHeight, char* windowTitle);

	int stop();

	int physicsThread();

	float getFramerateLimit() const;

	float getTickrateLimit() const;

	float getFramerate() const;

	float getTickrate() const;

	bool doRun() const;
};

