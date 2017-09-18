#define GLEW_NO_GLU
#include "Engine.h"
#include "Logger.h"
#include "VoxelGame.h"
#include "world/sceneGraph/SceneGraph.h"
#include "world/planets/Planet.h"
#include "renderer/RenderingHandler.h"
#include "renderer/font/FontRenderer.h"
#include "InputHandler.h"
#include "renderer/mesh/MeshBuilder.h"
#include "world/sceneGraph/components/RenderComponent.h"
#include "renderer/Camera.h"
#include "renderer/WindowHandler.h"
#include "resources/ResourceManager.h"
#include "core/Player.h"
#include "renderer/gui/GuiScreen.h"
#include <iostream>
#include <glm/gtc/constants.inl>
#include <glm/gtc/matrix_transform.inl>

Engine* engine;

Logger VoxelGame::logger;
ResourceManager VoxelGame::resourceManager;
RenderingHandler* VoxelGame::renderingHandler;
FontRenderer* VoxelGame::fontRenderer;
InputHandler VoxelGame::inputHandler;
Player* VoxelGame::player;
double VoxelGame::minScale = 0.0010F;
double VoxelGame::maxScale = 1000.0F;
double VoxelGame::scale = minScale;

GuiScreen* VoxelGame::guiScreen;
Light* VoxelGame::_light;
//std::vector<Planet*> VoxelGame::allPlanets;

GameObject* root;
std::map<const char*, Planet*> VoxelGame::planets;

ShaderProgram* VoxelGame::voxelRenderer;
ShaderProgram* VoxelGame::atmosphereRenderer;
ShaderProgram* VoxelGame::gridRenderer;
ShaderProgram* VoxelGame::guiRenderer;

MeshBuilder* cubicSphere;
MeshComponent* VoxelGame::skydome;

int logDist;

void physics(double delta);
void render(double delta);
void input(std::vector<SDL_Event> events, double delta);
void init();
void setupPlanets();
void stop();
void renderDebug(double delta);

void physics(double delta)
{
	VoxelGame::player->physics(delta, root);
	root->physics(delta, nullptr);
}

void render(double delta)
{
	std::string FPS =		"FPS:       " + std::to_string(engine->getFramerate());
	std::string UPS =		"UPS:       " + std::to_string(engine->getTickrate());
	std::string altitude =  "Altitude:  " + std::to_string(VoxelGame::player->getAltitude()) + "Km (above sea level)";
	std::string height =	"Height:    " + std::to_string(VoxelGame::player->getHeight()) + "Km (above terrain)";
	std::string scale =		"Scale:     " + std::to_string(VoxelGame::scale) + "GL/Km (GL Units / Kilometer)";
	std::string position =	"Position:  " + std::to_string(VoxelGame::player->getHeadPosition().x) + ", " + std::to_string(VoxelGame::player->getHeadPosition().y) + ", " + std::to_string(VoxelGame::player->getHeadPosition().z);
	std::string chunks =	"Chunks:    " + std::to_string(VoxelGame::player->getCurrentBody()->getRenderChunkCount());
	std::string planet =	"Planet:    " + VoxelGame::player->getCurrentBody()->getName();

	float fontSize = 10;
	glm::vec4 colour(0.0F, 1.0F, 0.0F, 1.0F);
	float y = WindowHandler::getHeight();

	VoxelGame::fontRenderer->drawText(FPS.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(FPS.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(UPS.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(UPS.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(planet.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(planet.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(altitude.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(altitude.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(height.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(height.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(scale.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(scale.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(position.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(position.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);
	VoxelGame::fontRenderer->drawText(chunks.c_str(), "verdana", 0.0F, y = y - VoxelGame::fontRenderer->getTextSize(chunks.c_str(), "verdana", fontSize).y - 10.0F, fontSize, colour);

	glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	VoxelGame::voxelRenderer->useProgram(true);
	VoxelGame::voxelRenderer->setUniform("useLighting", true);
	VoxelGame::voxelRenderer->setUniform("useTexture", false);
	VoxelGame::voxelRenderer->setUniform("lights[0]", VoxelGame::_light);
	VoxelGame::renderingHandler->preRender();
	VoxelGame::player->render(delta, root, VoxelGame::voxelRenderer);
	root->render(delta, nullptr, VoxelGame::voxelRenderer);

	VoxelGame::voxelRenderer->useProgram(false);
	VoxelGame::atmosphereRenderer->useProgram(false);

	VoxelGame::renderingHandler->postRender();

	glDisable(GL_DEPTH_TEST);
	VoxelGame::fontRenderer->render();
	renderDebug(delta);
	glEnable(GL_DEPTH_TEST);

//	int i = VoxelGame::player->getCamera()->sphereInFrustum(glm::vec3(earth->getAbsoluteTransformation().position), ((Planet*)earth)->getRadius() * VoxelGame::scale);
//
//	std::cout << "Planet ";
//	if (i == OUTSIDE_FRUSTUM) std::cout << "outside";
//	if (i == INSIDE_FRUSTUM) std::cout << "inside";
//	if (i == INTERSECT_FRUSTUM) std::cout << "intersects";
//	std::cout << " frustum\n";
}

void input(std::vector<SDL_Event> events, double delta)
{
	VoxelGame::inputHandler.reset(delta);

	for (SDL_Event event : events)
		VoxelGame::inputHandler.update(event);

	VoxelGame::player->input(delta, root);
	root->input(delta, nullptr);

	if (VoxelGame::inputHandler.keyPressed(SDL_SCANCODE_F1))
	{
		GLenum polyMode = VoxelGame::renderingHandler->getPolyMode() + 1;

		if (polyMode > GL_FILL)
			polyMode = GL_POINT;
		VoxelGame::renderingHandler->setPolyMode(polyMode);
	}

	float FOV = VoxelGame::renderingHandler->getFOV();
	float amt = 1.0F;
	float f = 0.1;
	float maxFOV = 180 - f;
	float minFOV = f;
	float zoomSpeed = f / (maxFOV + FOV);

	if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_EQUALS))
	{
		amt += zoomSpeed;
	}

	if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_MINUS))
	{
		amt -= zoomSpeed;
	}

	VoxelGame::renderingHandler->setFOV(FOV / amt);

	if (VoxelGame::inputHandler.keyPressed(SDL_SCANCODE_F2))
	{
		VoxelGame::renderingHandler->setFOV(70.0);
	}

	if (FOV > maxFOV) VoxelGame::renderingHandler->setFOV(maxFOV);;
	if (FOV < minFOV) VoxelGame::renderingHandler->setFOV(minFOV);;

	if (VoxelGame::inputHandler.keyPressed(SDL_SCANCODE_F3))
	{
		
	}
}

void init()
{
	VoxelGame::logger.info("Initializing game");
	engine->setFramerate(1000);
	engine->setTickrate(60);

	VoxelGame::renderingHandler = new RenderingHandler();
	VoxelGame::fontRenderer = new FontRenderer();

	VoxelGame::voxelRenderer = new ShaderProgram();
	VoxelGame::voxelRenderer->addShader(GL_VERTEX_SHADER, "res/shaders/voxelRenderer/VertexShader.glsl");
	VoxelGame::voxelRenderer->addShader(GL_FRAGMENT_SHADER, "res/shaders/voxelRenderer/FragmentShader.glsl");
	VoxelGame::voxelRenderer->addAttribute(ATTRIBUTE_VERTEX_GEOMETRIC, "position");
	VoxelGame::voxelRenderer->addAttribute(ATTRIBUTE_VERTEX_NORMAL, "normal");
	VoxelGame::voxelRenderer->addAttribute(ATTRIBUTE_VERTEX_TEXTURE, "texture");
	VoxelGame::voxelRenderer->addAttribute(ATTRIBUTE_VERTEX_COLOUR, "colour");
	VoxelGame::voxelRenderer->completeProgram();

	VoxelGame::atmosphereRenderer = new ShaderProgram();
	VoxelGame::atmosphereRenderer->addShader(GL_VERTEX_SHADER, "res/shaders/atmosphereRenderer/VertexShader.glsl");
	VoxelGame::atmosphereRenderer->addShader(GL_FRAGMENT_SHADER, "res/shaders/atmosphereRenderer/FragmentShader.glsl");
	VoxelGame::atmosphereRenderer->addAttribute(ATTRIBUTE_VERTEX_GEOMETRIC, "position");
	VoxelGame::atmosphereRenderer->addAttribute(ATTRIBUTE_VERTEX_NORMAL, "normal");
	VoxelGame::atmosphereRenderer->addAttribute(ATTRIBUTE_VERTEX_TEXTURE, "texture");
	VoxelGame::atmosphereRenderer->addAttribute(ATTRIBUTE_VERTEX_COLOUR, "colour");
	VoxelGame::atmosphereRenderer->completeProgram();
	
	VoxelGame::renderingHandler->addShaderProgram(VoxelGame::voxelRenderer);
	VoxelGame::renderingHandler->addShaderProgram(VoxelGame::atmosphereRenderer);
	VoxelGame::renderingHandler->init();
	VoxelGame::renderingHandler->setFOV(70.0);
	VoxelGame::renderingHandler->setNearPlane(0.1);
	VoxelGame::renderingHandler->setFarPlane(1000000000000000.0);
	VoxelGame::fontRenderer->init();

	root = new GameObject();

	setupPlanets();

	root->setTransformation(Transformation(glm::dvec3(0.0, 0.0, -VoxelGame::planets["earth"]->getOrbitDistance())));
	root->init(nullptr);

	VoxelGame::player = new Player(glm::normalize(glm::dvec3(-0.1, -0.2, -0.8)) * (VoxelGame::planets["earth"]->getRadius() + 100));
	VoxelGame::player->init();

	VoxelGame::_light = new Light(glm::vec3(0.0, 0.0F, -VoxelGame::planets["earth"]->getOrbitDistance()), glm::vec3(1.0F), glm::vec3());
	VoxelGame::player->setCurrentBody(VoxelGame::planets["earth"]);
}


void setupPlanets()
{
	double earthOrbit = 149000000.0;
	double lunaOrbit = 20000.0;// 384400.0; //20,000Km was the moons distance when it formed

	double solarRadus = 695700.0;
	double earthRadius = 6386.0;
	double lunaRadius = 1737.0;

	double solarMass = 1988920001144583600000000000000.0;
	double earthMass = 5973327139217744000000000.0;
	double lunaMass = 73476730900000000000000.0;

	VoxelGame::skydome = new MeshComponent(new Mesh(MeshBuilder::createIcosphere(1.0, 5)));

	Planet* solar = new Planet(274.0, solarRadus, solarMass, 0.0, 0.3, "solar");
	{
		RenderComponent* sunRender = new RenderComponent([](double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
		{
			shaderProgram->setUniform("useLighting", false);
		});
		solar->addComponent(sunRender);
	}
	
	Planet* earth = new Planet(9.807, earthRadius, earthMass, earthOrbit, 0.32, "earth");
	{
		double landOceanRatio = 1.0;
		double maxHeight = 50.0 / earthRadius;
		double minHeight = -50.0 / earthRadius;
		double skydomeHeight = 300.0;
		double cloudHeight = 40.0;

		int seed = 0;// (int)std::chrono::high_resolution_clock::now().time_since_epoch().count();

		TerrainGenerator* terrain = new TerrainGenerator(maxHeight, minHeight, 1.0, 0.56, 1.95, 20, seed, ADD);
		{
			TerrainGenerator* smallDetail = new TerrainGenerator(maxHeight / 2.0, minHeight, 0.07, 0.33, 2.1, 22, seed, ADD);
			TerrainGenerator* mountain = new RidgeGenerator(maxHeight, 0.0, 35.0, 0.5, 2.0, 2.4, 16, seed, ADD);
			{
				TerrainGenerator* multiplier1 = new TerrainGenerator(1.8, -0.75, 0.3, 0.42, 1.12, 20, seed, MUL);
				{
					TerrainGenerator* multiplier2 = new TerrainGenerator(1.4, -3.0, 1.0, 0.56, 1.95, 20, seed, MUL);
					multiplier2->setNoiseExponent(0.375);
					multiplier2->setClampBounds(1.0, 0.0);
					multiplier1->addTerrainGenerator(multiplier2);
				}
				multiplier1->setNoiseExponent(32.0);
				multiplier1->setClampBounds(10.0, 0.0);

				mountain->addTerrainGenerator(multiplier1);
			}
			terrain->addTerrainGenerator(mountain);
			terrain->addTerrainGenerator(smallDetail);
		}
		earth->getTerrain() = terrain;
		earth->getTerrain()->addColourGradient(-1.0000, glm::vec3(000.0F, 000.0F, 128.0F) / 255.0F); // deeps
		earth->getTerrain()->addColourGradient(-0.2500, glm::vec3(000.0F, 000.0F, 255.0F) / 255.0F); // shallow
		earth->getTerrain()->addColourGradient(+0.8500, glm::vec3(128.0F, 128.0F, 128.0F) / 255.0F); // rock
		earth->getTerrain()->addColourGradient(+0.0625, glm::vec3(240.0F, 240.0F, 064.0F) / 255.0F); // sand
		earth->getTerrain()->addColourGradient(+0.0000, glm::vec3(000.0F, 128.0F, 255.0F) / 255.0F); // shore
		earth->getTerrain()->addColourGradient(+0.9000, glm::vec3(255.0F, 255.0F, 255.0F) / 255.0F); // snow
		earth->getTerrain()->addColourGradient(+0.0626, glm::vec3(032.0F, 160.0F, 000.0F) / 255.0F); // grass
		earth->getTerrain()->addColourGradient(+0.3750, glm::vec3(005.0F, 100.0F, 012.0F) / 255.0F); // jungle
		earth->getTerrain()->addColourGradient(+1.0000, glm::vec3(255.0F, 255.0F, 255.0F) / 255.0F); // snow

		earth->getRelativeTransformation()->position = glm::normalize(glm::dvec3(0.0, 0.0, 1.0)) * earthOrbit;

//		MeshComponent* clouds = new MeshComponent(new Mesh(MeshBuilder::createSphere((earthRadius + cloudHeight) / (earthRadius + skydomeHeight), 50, 50)));
//		clouds->setTexture(Texture::loadTexture("res/textures/clouds.png"));
//		clouds->setCullmode(0);

		RenderComponent* earthRender = new RenderComponent([](double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
		{
			float secondsPerSecond = 0.0 * 24.016 * 60.0; //24.016 seconds per real second, same as 24.016 hours per real hour. 1 day will take 1 hour
			float rotationRate = (1.0F / 86164.1) * secondsPerSecond;
			gameObject->getRelativeTransformation()->rotation.y += 360.0 * delta * rotationRate;// += glm::vec3(0.0F, 1.0F / 200.0F, 0.0F);
		});

		Atmosphere* atmosphere = new Atmosphere(skydomeHeight, 0.24, 0.1, 0.0025, 0.0010, glm::vec3(0.850, 0.670, 0.465), VoxelGame::skydome, VoxelGame::atmosphereRenderer);
		earth->initAtmosphere(atmosphere);
		earth->addComponent(earthRender);
//		earth->addComponent(clouds);
	}

	Planet* luna = new Planet(1.622, lunaRadius, lunaMass, lunaOrbit, 0.3, "luna");
	{
		luna->getRelativeTransformation()->position = glm::normalize(glm::dvec3(0.8, 0.05, -0.2)) * lunaOrbit;
//		VoxelGame::allPlanets.push_back((Planet*)luna);
	}

	VoxelGame::planets["solar"] = solar;
	VoxelGame::planets["earth"] = earth;
	VoxelGame::planets["luna"] = luna;

	earth->addChild(luna);
	solar->addChild(earth);
	root->addChild(solar);
}

void stop()
{
	VoxelGame::inputHandler.grabMouse(false);
	VoxelGame::logger.info("Stopping game");
}

void renderGuis(double delta)
{
	VoxelGame::guiRenderer->useProgram(true);
	VoxelGame::guiScreen->render(VoxelGame::guiRenderer);
}

void renderDebug(double delta)
{
	float aspect = WindowHandler::getAspectRatio();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	VoxelGame::renderingHandler->perspectiveGL(70.0F, aspect, 0.05F, 2048.0F);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

	float scale = 0.06F;
	Camera* camera = VoxelGame::player->getCamera();
	glm::dmat3x3 rotationMatrix(camera->getViewMatrix());
	glm::dvec3 o = rotationMatrix * glm::dvec3(0.0F, 0.0F, 0.0F);
	glm::dvec3 x = rotationMatrix * glm::dvec3(1.0F, 0.0F, 0.0F);
	glm::dvec3 y = rotationMatrix * glm::dvec3(0.0F, 1.0F, 0.0F);
	glm::dvec3 z = rotationMatrix * glm::dvec3(0.0F, 0.0F, 1.0F);
	glm::dvec3 g = rotationMatrix * glm::normalize(VoxelGame::player->getCurrentBody()->getAbsoluteTransformation().position - VoxelGame::player->getHeadPosition());

	glTranslatef(-0.5F * aspect, -0.5F, -0.875F);
	glScalef(scale, scale, scale);

	glLineWidth(4.0);

	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(o.x, o.y, o.z);
	glVertex3f(x.x, x.y, x.z);
	glColor3f(0, 1, 0);
	glVertex3f(o.x, o.y, o.z);
	glVertex3f(y.x, y.y, y.z);
	glColor3f(0, 0, 1);
	glVertex3f(o.x, o.y, o.z);
	glVertex3f(z.x, z.y, z.z);
	glColor3f(1, 1, 1);
	glVertex3f(o.x, o.y, o.z);
	glVertex3f(g.x, g.y, g.z);
	glEnd();

	glPopMatrix();

	glLineWidth(1.0F);

}

int main(int argc, char** argv)
{
	VoxelGame::logger.info("Starting game");

	engine = new Engine(init, stop, render, physics, input);
	return engine->start(1600, 900, "Game");
}
