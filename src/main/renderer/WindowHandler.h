#pragma once

#include <SDL.h>
#include <vector>
#include "ShaderProgram.h"

class WindowHandler
{
private:
	static bool _isFullscreen;
	static bool _isVSyncEnabled;
	static int _width;
	static int _height;
	static SDL_Window* _windowHandle;
	static SDL_GLContext* _openGLContext;
	static std::vector<ShaderProgram*> _shaderPrograms;
	static glm::mat4x4 _projection3D;

public:
	static void disposeDisplay();

	static void disposeOpenGL();

	static bool initDisplay(char* windowTitle, unsigned int width, unsigned int height, bool doubleBuffer);

	static bool isFullscreen();

	static bool isVSyncEnabled();

	static const std::vector<ShaderProgram*> getShaderPrograms();

	static int getWidth();

	static int getHeight();

	static float getAspectRatio();

	static int addShaderProgram(ShaderProgram* shaderProgram);

	static void setFullscreen(bool fullscreen);

	static void setVSyncEnabled(bool vSyncEnabled);

	static void setMousePosition(int x, int y);

	static void setWindowSize(int width, int height);

	static void postRender();
};


