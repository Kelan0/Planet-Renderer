#pragma once

#define GRAVITATIONAL_FIELD_STRENGTH 9.807
#include "world/planets/Planet.h"

class Player;
class Logger;
class ResourceManager;
class RenderingHandler;
class FontRenderer;
class InputHandler;
class GuiScreen;

class VoxelGame
{
public:
	static Logger logger;
	static ResourceManager resourceManager;
	static RenderingHandler* renderingHandler;
	static FontRenderer* fontRenderer;
	static InputHandler inputHandler;
	static Player* player;
	static double maxScale;
	static double minScale;
	static double scale;

	static GuiScreen* guiScreen;
	static Light* _light;
//	static std::vector<Planet*> allPlanets;
	static std::map<const char*, Planet*> planets;

	static ShaderProgram* voxelRenderer;
	static ShaderProgram* atmosphereRenderer;
	static ShaderProgram* gridRenderer;
	static ShaderProgram* guiRenderer;

	static MeshComponent* skydome;
};
