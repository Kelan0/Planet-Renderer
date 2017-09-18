#include "Engine.h"
#include "VoxelGame.h"
#include "renderer/WindowHandler.h"
#include <string>
#include <iostream>
#include <chrono>
#include "Logger.h"

int staticPhysicsThread(void* ptr)
{
	Engine* engine = (Engine*)ptr;

	return engine->physicsThread();
}

Engine::Engine(void(*initFunc)(), void(*stopFunc)(), void(*renderFunc)(double delta), void(*physicsFunc)(double delta), void(*inputFunc)(std::vector<SDL_Event> events, double delta)) : _physicsThread(nullptr), _doRun(true), _framerateLimit(60.0F), _framerateCurrent(0.0F), _tickrateLimit(860.0F), _tickrateCurrent(0.0F)
{
	this->initFunc = initFunc;
	this->stopFunc = stopFunc;
	this->renderFunc = renderFunc;
	this->physicsFunc = physicsFunc;
	this->inputFunc = inputFunc;
}

Engine::~Engine()
{
}

void Engine::setFramerate(float framerate)
{
	_framerateLimit = framerate;
}

void Engine::setTickrate(float tickrate)
{
	if (tickrate != tickrate || tickrate < 1) //check NaN or less than 1
	{
		VoxelGame::logger.error("Framerate cannot be limited to any value less that 1.0");
		return;
	}

	_tickrateLimit = tickrate;
}

void Engine::handleInput(SDL_Event& event)
{
	if (event.type == SDL_QUIT)
	{
		stop();
	}
}

int Engine::start(unsigned int windowWidth, unsigned int windowHeight, char* windowTitle)
{
	VoxelGame::logger.info("Starting engine");

	_doRun = true;
	int ret = 0;
	bool flag = false;

	double inputDelta = 1.0F;
	double renderDelta = 1.0F;
	
	if (WindowHandler::initDisplay(windowTitle, windowWidth, windowHeight, true))
	{
		initFunc();
		flag = true;
	}

	if (!flag)
	{
		ret = ERROR_WINDOW_INITIALIZATION;
		_doRun = false;
	} else
	{
		_physicsThread = SDL_CreateThread(staticPhysicsThread, "PHYSICS_THREAD", this);

		if (_physicsThread == nullptr)
		{
			ret = ERROR_PHYSICS_INITIALIZATION;
			_doRun = false;
		}
	}

	std::chrono::high_resolution_clock::time_point lastRender = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point lastInput = std::chrono::high_resolution_clock::now();

	double nsPerFrame = 1000000000.0 / (getFramerateLimit() <= 0.0 ? -1.0 : getFramerateLimit());
	double framesPassed;

	while (_doRun)
	{
		std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
		double nsElapsed = double(std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastRender).count());
		framesPassed = nsElapsed / (nsPerFrame > 0.0 ? nsPerFrame : nsElapsed);

		bool doRender = false;

		while (framesPassed >= 1.0)
		{
			doRender = true;

			SDL_Event event;
			std::vector<SDL_Event> events;

			while (SDL_PollEvent(&event))
			{
				handleInput(event);
				events.push_back(event);
			}
			inputFunc(events, inputDelta);

			inputDelta = double(std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastRender).count()) / 1000000000.0;
			lastInput = now;

			framesPassed--;
		}

		if (doRender)
		{
			renderFunc(renderDelta);
			WindowHandler::postRender();
			_framerateCurrent = 1.0F / renderDelta;

			lastRender = now;
			renderDelta = nsElapsed / 1000000000.0;
		}
		else
		{
			SDL_Delay(1);
		}
	}

	stop();
	VoxelGame::logger.info(std::string("Engine stopped, exiting with error code ") + std::to_string(ret));
	system("PAUSE");
	return ret;
}

int Engine::stop()
{
	_doRun = false;
	this->stopFunc();

	WindowHandler::disposeOpenGL();
	WindowHandler::disposeDisplay();
	return 0;
}

int Engine::physicsThread()
{
	double physicsDelta = 1.0F;

	double nsPerTick = 1000000000.0 / getTickrateLimit();
	double ticksPassed;

	std::chrono::high_resolution_clock::time_point lastTick = std::chrono::high_resolution_clock::now();

	while (_doRun)
	{
		std::chrono::high_resolution_clock::time_point preTick = std::chrono::high_resolution_clock::now();
		double nsElapsed = double(std::chrono::duration_cast<std::chrono::nanoseconds>(preTick - lastTick).count());

		ticksPassed = nsElapsed / nsPerTick;

		while (ticksPassed > 1.0)
		{
			physicsFunc(physicsDelta);
			std::chrono::high_resolution_clock::time_point postTick = std::chrono::high_resolution_clock::now();
			_tickrateCurrent = 1.0F / physicsDelta;
			ticksPassed--;

			physicsDelta = double(std::chrono::duration_cast<std::chrono::nanoseconds>(postTick - lastTick).count()) / 1000000000.0;
			lastTick = postTick;
		}
	}

	return 0;
}

float Engine::getFramerateLimit() const
{
	return _framerateLimit;
}

float Engine::getTickrateLimit() const
{
	return _tickrateLimit;
}

float Engine::getFramerate() const
{
	return _framerateCurrent;
}

float Engine::getTickrate() const
{
	return _tickrateCurrent;
}

bool Engine::doRun() const
{
	return _doRun;
}
