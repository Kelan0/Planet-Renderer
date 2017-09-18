#include <GL/glew.h>
#include "../VoxelGame.h"
#include "WindowHandler.h"
#include <string>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "../Logger.h"

bool WindowHandler::_isFullscreen = false;
bool WindowHandler::_isVSyncEnabled = false;
int  WindowHandler::_width = 0;
int  WindowHandler::_height = 0;
SDL_Window* WindowHandler::_windowHandle;
SDL_GLContext* WindowHandler::_openGLContext;
std::vector<ShaderProgram*> WindowHandler::_shaderPrograms;
 
void WindowHandler::disposeDisplay()
{
	VoxelGame::logger.info("Disposing display");

	VoxelGame::logger.info("Destroying window");
	SDL_DestroyWindow(_windowHandle);

	VoxelGame::logger.info("Quitting SDL");
	SDL_Quit();
}

void WindowHandler::disposeOpenGL()
{
	SDL_GL_DeleteContext(_openGLContext);
}

bool WindowHandler::initDisplay(char* windowTitle, unsigned int width, unsigned int height, bool doubleBuffered)
{
	//======== SDL / OpenGL ========
	VoxelGame::logger.info("Initializing SDL");
	SDL_Init(SDL_INIT_EVERYTHING);

	VoxelGame::logger.info("Creating SDL display with dimensions " + std::to_string(width) + ", " + std::to_string(height));
	_windowHandle = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	if (_windowHandle == nullptr)
	{
		VoxelGame::logger.error("An error occurred while creating teh SDL window, application will exit");
		VoxelGame::logger.error("Window handle was NULL");
		return false;
	}

	VoxelGame::logger.info("Creating SDL OpenGL context for the current thread");
	SDL_GLContext ctxt = SDL_GL_CreateContext(_windowHandle);
	_openGLContext = &ctxt;

	VoxelGame::logger.info("Initializing GLEW");
	GLenum error = glewInit();

	if (error)
	{
		VoxelGame::logger.error("An error occurred while initializing GLEW. application will exit");
		VoxelGame::logger.error((char*)glewGetErrorString(error));
		return false;
	}

	VoxelGame::logger.info("Setting openGL SDL attributes");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, doubleBuffered);

	//======== SHADERS ========


	//======== DONE ========
	setWindowSize(width, height);
	_isFullscreen = false;
	_isVSyncEnabled = false;
	VoxelGame::logger.info(std::string("Successfully initialized SDL with OpenGL version ") + (char*)glGetString(GL_VERSION));
	return true;
}

bool WindowHandler::isFullscreen()
{
	return _isFullscreen;
}

bool WindowHandler::isVSyncEnabled()
{
	return _isVSyncEnabled;
}

const std::vector<ShaderProgram*> WindowHandler::getShaderPrograms()
{
	return _shaderPrograms;
}

int WindowHandler::getWidth()
{
	return _width;
}

int WindowHandler::getHeight()
{
	return _height;
}

float WindowHandler::getAspectRatio()
{
	return (float)_width / (float)_height;
}

int WindowHandler::addShaderProgram(ShaderProgram* shaderProgram)
{
	if (shaderProgram != nullptr && shaderProgram->isComplete())
	{
		int index = _shaderPrograms.size();
		_shaderPrograms.push_back(shaderProgram);
		return index;
	}
	return -1;
}

void WindowHandler::setFullscreen(bool fullscreen)
{
	_isFullscreen = fullscreen;
}

void WindowHandler::setVSyncEnabled(bool vSyncEnabled)
{
	_isVSyncEnabled = vSyncEnabled;
}

void WindowHandler::setMousePosition(int x, int y)
{
	SDL_WarpMouseInWindow(_windowHandle, x, y);
}

void WindowHandler::setWindowSize(int width, int height)
{
	_width = width;
	_height = height;
	glViewport(0, 0, width, height);
}

void WindowHandler::postRender()
{
	SDL_GL_SwapWindow(_windowHandle);
}
