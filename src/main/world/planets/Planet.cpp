#include "Planet.h"
#include "../../InputHandler.h"
#include <iostream>
#include "../../VoxelGame.h"
#include "../../core/Player.h"
#include <thread>
#include "../../renderer/mesh/MeshBuilder.h"
#include <algorithm>
#include "terrain/WorldGenerator.h"
#include <glm/gtc/constants.inl>

void Atmosphere::applyUniforms(Planet* planet, ShaderProgram* shaderProgram)
{
	glm::vec3 planetPosition = glm::vec3(planet->getAbsoluteTransformation().getPosition());
	glm::vec3 cameraPosition = glm::vec3(VoxelGame::player->getCamera()->getPosition());
	glm::vec3 lightPosition = glm::vec3(VoxelGame::planets["solar"]->getAbsoluteTransformation().getPosition());
	glm::vec3 invWavelength = glm::vec3(1.0 / pow(wavelength.x, 4.0), 1.0 / pow(wavelength.y, 4.0), 1.0 / pow(wavelength.z, 4.0)); //day time, blue scattering
	float innerRadius = (planet->getRadius()) * VoxelGame::scale;
	float outerRadius = (planet->getRadius() + height) * VoxelGame::scale;
	float ESun = 16.0F;
	float g = -0.999F;
	shaderProgram->setUniform("v3CameraPos", cameraPosition);
	shaderProgram->setUniform("v3PlanetPos", planetPosition);
	shaderProgram->setUniform("v3LightDir", glm::normalize(lightPosition));
	shaderProgram->setUniform("v3InvWavelength", invWavelength);
	shaderProgram->setUniform("fOuterRadius", outerRadius);
	shaderProgram->setUniform("fInnerRadius", innerRadius);
	shaderProgram->setUniform("fESun", ESun);
	shaderProgram->setUniform("fKr", rayleighConstant);
	shaderProgram->setUniform("fKm", mieConstant);
	shaderProgram->setUniform("fScaleDepth", scaleDepth);
	shaderProgram->setUniform("g", g);
	shaderProgram->setUniform("fAtmosphericDensity", density);
}

void Atmosphere::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
	Planet* planet = (Planet*)gameObject;
	this->setInheritScale(false);
	this->setTransformation(Transformation(glm::dvec3(), glm::dvec3(), glm::dvec3(planet->getRadius() + height)));
	this->shaderProgram->useProgram(true);
	applyUniforms(planet, this->shaderProgram);

	VoxelGame::skydome->render(delta, this, this->shaderProgram);
}

double Atmosphere::getHeight() const
{
	return height;
}

double Atmosphere::getDensity() const
{
	return density;
}

double Atmosphere::getScaleDepth() const
{
	return scaleDepth;
}

double Atmosphere::getRayleightConstant() const
{
	return rayleighConstant;
}

double Atmosphere::getMieConstant() const
{
	return mieConstant;
}

glm::vec3 Atmosphere::getWavelength() const
{
	return wavelength;
}

MeshComponent* Atmosphere::getSkydome() const
{
	return skydome;
}

Planet::Planet(double gravity, double radius, double mass, double orbitalDistance, double splitThreshold, std::string name) : _gravity(gravity), _radius(radius), _mass(mass), _orbitalDistance(orbitalDistance), _splitThreshold(splitThreshold), _name(name)
{
	this->maxSplitsPerTick = 8;
	this->maxMergesPerTick = 8;
	this->setInheritScale(false);
	this->getRelativeTransformation()->scale = glm::dvec3(radius);
}

Planet::~Planet()
{
}

void Planet::initAtmosphere(Atmosphere* atmosphere)
{
	_atmosphere = atmosphere;
}

void Planet::init(GameObject* gameObject)
{
	cullRadius = _radius;
	if (_terrain == nullptr) //setup a generic earth-like planet
	{
		double maxHeight = (30.0 / _radius);
		int seed = (int)std::chrono::high_resolution_clock::now().time_since_epoch().count();
		_terrain = new TerrainGenerator(maxHeight, -maxHeight, 1.0, 0.6, 1.0, 8, 0, ADD);// ->setFrequency(0.9)->setLacunarity(2.0)->setPersistence(0.6);

		_terrain->addColourGradient(-1.0000, glm::vec3(255.0F, 255.0F, 255.0F) / 255.0F);
		_terrain->addColourGradient(+1.0000, glm::vec3(255.0F, 255.0F, 255.0F) / 255.0F);
	}
	//	float radius = 1.0F;
	//	float arcLength = 2.0F * glm::pi<float>() * radius * (360.0F / angle);

	double size = 2.0F;

	//The 8 corners of the cube
	glm::dvec3 v0 = (glm::dvec3(0, 0, 0) - glm::dvec3(0.5)) * size;
	glm::dvec3 v1 = (glm::dvec3(0, 0, 1) - glm::dvec3(0.5)) * size;
	glm::dvec3 v2 = (glm::dvec3(0, 1, 0) - glm::dvec3(0.5)) * size;
	glm::dvec3 v3 = (glm::dvec3(0, 1, 1) - glm::dvec3(0.5)) * size;
	glm::dvec3 v4 = (glm::dvec3(1, 0, 0) - glm::dvec3(0.5)) * size;
	glm::dvec3 v5 = (glm::dvec3(1, 0, 1) - glm::dvec3(0.5)) * size;
	glm::dvec3 v6 = (glm::dvec3(1, 1, 0) - glm::dvec3(0.5)) * size;
	glm::dvec3 v7 = (glm::dvec3(1, 1, 1) - glm::dvec3(0.5)) * size;

	_cube[CUBE_X_POS] = new Chunk(this, glm::dvec3(+1.0F, 0.0F, 0.0F), glm::dvec3(+1.0F, 0.0F, 0.0F), 0);
	_cube[CUBE_X_NEG] = new Chunk(this, glm::dvec3(-1.0F, 0.0F, 0.0F), glm::dvec3(-1.0F, 0.0F, 0.0F), 0);
	_cube[CUBE_Y_POS] = new Chunk(this, glm::dvec3(0.0F, +1.0F, 0.0F), glm::dvec3(0.0F, +1.0F, 0.0F), 0);
	_cube[CUBE_Y_NEG] = new Chunk(this, glm::dvec3(0.0F, -1.0F, 0.0F), glm::dvec3(0.0F, -1.0F, 0.0F), 0);
	_cube[CUBE_Z_POS] = new Chunk(this, glm::dvec3(0.0F, 0.0F, +1.0F), glm::dvec3(0.0F, 0.0F, +1.0F), 0);
	_cube[CUBE_Z_NEG] = new Chunk(this, glm::dvec3(0.0F, 0.0F, -1.0F), glm::dvec3(0.0F, 0.0F, -1.0F), 0);

	(_cube[CUBE_X_POS]->getRoot() = _cube[CUBE_X_POS])->setCorners(v4, v6, v7, v5);//->setNeighbours(_cube[CUBE_Y_POS], _cube[CUBE_Z_NEG], _cube[CUBE_Y_NEG], _cube[CUBE_Z_POS]);
	(_cube[CUBE_X_NEG]->getRoot() = _cube[CUBE_X_NEG])->setCorners(v0, v2, v3, v1);//->setNeighbours(_cube[CUBE_Y_POS], _cube[CUBE_Z_POS], _cube[CUBE_Y_NEG], _cube[CUBE_Z_NEG]);
	(_cube[CUBE_Y_POS]->getRoot() = _cube[CUBE_Y_POS])->setCorners(v2, v6, v7, v3);//->setNeighbours(_cube[CUBE_Z_NEG], _cube[CUBE_X_POS], _cube[CUBE_Z_POS], _cube[CUBE_X_NEG]);
	(_cube[CUBE_Y_NEG]->getRoot() = _cube[CUBE_Y_NEG])->setCorners(v0, v4, v5, v1);//->setNeighbours(_cube[CUBE_Z_NEG], _cube[CUBE_X_NEG], _cube[CUBE_Z_POS], _cube[CUBE_X_POS]);
	(_cube[CUBE_Z_POS]->getRoot() = _cube[CUBE_Z_POS])->setCorners(v1, v5, v7, v3);//->setNeighbours(_cube[CUBE_Y_POS], _cube[CUBE_X_POS], _cube[CUBE_Y_NEG], _cube[CUBE_X_NEG]);
	(_cube[CUBE_Z_NEG]->getRoot() = _cube[CUBE_Z_NEG])->setCorners(v0, v4, v6, v2);//->setNeighbours(_cube[CUBE_Y_POS], _cube[CUBE_X_NEG], _cube[CUBE_Y_NEG], _cube[CUBE_X_POS]);

	double cubeInSphereEdgeSize = _radius * 2.0 / sqrt(3);

	for (int i = 0; i < MAX_LOD_DEPTH; i++)
	{
		double edgeSize = cubeInSphereEdgeSize / pow(2.0, i);
		_chunkSplitDistances[i] = edgeSize / tan(_splitThreshold);
		_chunkMergeDistances[i] = edgeSize / tan(_splitThreshold);

		std::cout << "Generated LOD split for level " << i << " at distance " << _chunkSplitDistances[i] << ": " << tan(_splitThreshold) << "\n";
	}

	for (int i = 0; i < 6; i++)
	{
		_cube[i]->init();
//		addChild(_cube[i]);

//		for (int j = 0; j < 5; j++)
//		{
//			_cube[i]->divideChildren(this);
//		}
	}

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(ATTRIBUTE_VERTEX_GEOMETRIC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, geometric)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, normal)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, texture)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_COLOUR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, colour)));
	glBufferData(GL_ARRAY_BUFFER, MAX_CHUNKS * VERTICES_PER_CHUNK * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_CHUNKS * VERTICES_PER_CHUNK * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);

	GameObject::init(this);
}

void Planet::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
	Transformation transform = this->getAbsoluteTransformation();
	transform.position -= VoxelGame::player->getCamera()->getPosition();
	glm::mat4x4 trans = glm::mat4x4(transform.getTransformationMatrix());

	for (int i = 0; i < _components.size(); i++)
	{
		if (_components[i] != nullptr)
		{
			_components[i]->render(delta, this, shaderProgram);
		}
	}

	for (int i = 0; i < _children.size(); i++)
	{
		if (_children[i] != nullptr)
		{
			_children[i]->render(delta, this, shaderProgram);
		}
	}

	shaderProgram->useProgram(true);
	if (_atmosphere != nullptr)
	{
		shaderProgram->setUniform("hasAtmosphere", true);
		_atmosphere->applyUniforms(this, shaderProgram);
		_atmosphere->render(delta, this, shaderProgram);
	} else
	{
		shaderProgram->setUniform("hasAtmosphere", false);
	}

	shaderProgram->useProgram(true);
	shaderProgram->setUniform("modelMatrix", trans);
	shaderProgram->setUniform("useLighting", true);

	_mutex.lock();
	_renderList = _updateList;
	_mutex.unlock();

	if (_renderList.size() > 0)
	{
		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
//		glCullFace(GL_BACK);

		for (Chunk chunk : _renderList)
		{
			if (!chunk.shouldRender())
				continue;
			
			std::vector<float> vertices = chunk.getVertices();
			std::vector<GLuint> indices = chunk.getIndices();

			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

			glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), &indices[0]);

			glEnableVertexAttribArray(ATTRIBUTE_VERTEX_GEOMETRIC);
			glEnableVertexAttribArray(ATTRIBUTE_VERTEX_NORMAL);
			glEnableVertexAttribArray(ATTRIBUTE_VERTEX_TEXTURE);
			glEnableVertexAttribArray(ATTRIBUTE_VERTEX_COLOUR);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glDisableVertexAttribArray(ATTRIBUTE_VERTEX_GEOMETRIC);
			glDisableVertexAttribArray(ATTRIBUTE_VERTEX_NORMAL);
			glDisableVertexAttribArray(ATTRIBUTE_VERTEX_TEXTURE);
			glDisableVertexAttribArray(ATTRIBUTE_VERTEX_COLOUR);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			chunk.render(shaderProgram);
		}
	}

	shaderProgram->useProgram(false);
}

void Planet::physics(double delta, GameObject* gameObject)
{
	std::vector<Chunk> updateList;
	splitCounter = mergeCounter = 0;
	for (int i = 0; i < 6; i++)
	{
		_cube[i]->update(updateList);
	}

	_mutex.lock();
	_updateList = updateList;
	_mutex.unlock();

	GameObject::physics(delta, gameObject);
}

void Planet::input(double delta, GameObject* gameObject)
{
	GameObject::input(delta, gameObject);
}

bool Planet::pointBehindHorizon(glm::dvec3 camera, glm::dvec3 point)
{
	glm::dvec3 cameraPos = camera;
	glm::dvec3 planetPos = getAbsoluteTransformation(false).position;

//	double dist = glm::distance(camera / VoxelGame::scale, planetPos);
//	double radius = std::min(_radius, dist);

	double radius = _radius - _terrain->getMaxHeight();
	planetPos *= VoxelGame::scale;
	radius *= VoxelGame::scale;

	glm::dvec3 targetPos = (point * VoxelGame::scale - planetPos) / radius;

	glm::dvec3 planetToCamera = (cameraPos - planetPos) / radius;
	glm::dvec3 cameraToTarget = targetPos - planetToCamera;

	double planetToCameraD2 = glm::dot(planetToCamera, planetToCamera) - 1.0;
	double cameraToTargetD2 = glm::dot(cameraToTarget, cameraToTarget);

	double d = -glm::dot(cameraToTarget, planetToCamera);

	return d > planetToCameraD2 && d * d / cameraToTargetD2 > planetToCameraD2;
}

int Planet::getRenderChunkCount()
{
	_mutex.lock();
	int i = _updateList.size();
	_mutex.unlock();
	return i;
}

double Planet::getGravity() const
{
	return _gravity;
}

double Planet::getRadius() const
{
	return _radius;
}

double Planet::getMass() const
{
	return _mass;
}

double Planet::getOrbitDistance() const
{
	return _orbitalDistance;
}

double Planet::getTerrainHeight(glm::dvec3 position)
{
	position = glm::normalize(position);
	if (_terrain != nullptr && !isnan(glm::dot(position, position))) //if the position was [0,0,0], the length will be NaN. This breaks things.
	{
		double h = _terrain->getHeight(position.x, position.y, position.z);

//		if (h >= 1.0 || isnan(h))
//		std::cout << "Height at position [" << worldPosition.x << ", " << worldPosition.y << ", " << worldPosition.z << "]:" <<  h << std::endl;
		return glm::clamp(h, -1.0, 1.0);
	}

	return 0.0;
}

double Planet::getDistance(glm::dvec3 position, bool scaled)
{
	return glm::distance(getAbsoluteTransformation(scaled).position, position);
}

const double* Planet::getSplitDistances() const
{
	return _chunkSplitDistances;
}

const double* Planet::getMergeDistances() const
{
	return _chunkMergeDistances;
}

glm::dvec3 Planet::getRotatedPosition(glm::dvec3 position)
{
	//TODO make this faster (do not create a new transformation with a new rotation matrix every time.)
	return glm::dvec3(Transformation(glm::dvec3(), getAbsoluteTransformation(false).rotation, glm::dvec3(1.0)).getTransformationMatrix() * glm::dvec4(position, 1.0));
}

glm::dvec3 Planet::getSurfaceVelocity(glm::dvec3 position)
{
	glm::dvec3 rotatedPosition = glm::dvec3(Transformation(glm::dvec3(), getAbsoluteTransformation(false).rotation, glm::dvec3(1.0)).getTransformationMatrix() * glm::dvec4(position, 1.0));
	glm::dvec3 unrotatedPosition = position;
	return rotatedPosition - unrotatedPosition;
}

glm::dvec3 Planet::getRelativeCoords(glm::dvec3 position)
{
	return glm::dvec3(0.0);
}

std::string Planet::getName() const
{
	return _name;
}

TerrainGenerator*& Planet::getTerrain()
{
	return _terrain;
}
