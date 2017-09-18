#pragma once
#include "../sceneGraph/SceneGraph.h"
#include <mutex>
#include "Chunk.h"
#include "../../util/WorldConstants.h"
#include "terrain/WorldGenerator.h"
#include "../../renderer/mesh/MeshBuilder.h"
#include "terrain/TerrainGenerator.h"

#define MAX_CHUNKS 4000
#define CUBE_X_POS 0
#define CUBE_X_NEG 1
#define CUBE_Y_POS 2
#define CUBE_Y_NEG 3
#define CUBE_Z_POS 4
#define CUBE_Z_NEG 5

class Atmosphere : public GameObject
{
protected:
	float height;
	float density;
	float scaleDepth;
	float rayleighConstant;
	float mieConstant;
	glm::vec3 wavelength;
	MeshComponent* skydome;
	ShaderProgram*& shaderProgram;

public:
	Atmosphere(double height, double density, double scaleDepth, double rayleighConstant, double mieConstant, const glm::vec3 wavelength, MeshComponent* skydome, ShaderProgram*& shaderProgram): height(height), density(density), scaleDepth(scaleDepth), rayleighConstant(rayleighConstant), mieConstant(mieConstant), wavelength(wavelength), shaderProgram(shaderProgram)
	{
		if (skydome != nullptr) //faster if skydome is created once and used throughout all atmospheres, but if not, it is created here.
			this->skydome = skydome;
		else
			this->skydome = new MeshComponent(new Mesh(MeshBuilder::createIcosphere(1.0, 4)));

		skydome->setPreRenderFunc([](double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
		{
			shaderProgram->setUniform("useLighting", false);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			glDepthFunc(GL_LESS);
			glCullFace(GL_FRONT);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		});

		skydome->setPostRenderFunc([](double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
		{
			shaderProgram->setUniform("useLighting", true);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		});
	}

	void applyUniforms(Planet* planet, ShaderProgram* shaderProgram);

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) override;

	double getHeight() const;

	double getDensity() const;
	
	double getScaleDepth() const;
	
	double getRayleightConstant() const;
	
	double getMieConstant() const;
	
	glm::vec3 getWavelength() const;

	MeshComponent* getSkydome() const;
};

class Planet : public GameObject
{
private:
	double _gravity;
	double _radius;
	double _mass;
	double _orbitalDistance;
	double _splitThreshold;
	double _chunkSplitDistances[MAX_LOD_DEPTH];
	double _chunkMergeDistances[MAX_LOD_DEPTH];

	std::string _name;
	std::mutex _mutex;
	std::vector<Chunk> _updateList;
	std::vector<Chunk> _renderList;
	TerrainGenerator* _terrain;
	Atmosphere* _atmosphere;

	Chunk* _cube[6];
	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;
//	Mesh* _mesh;

public:
	double cullRadius;
	int splitCounter;
	int mergeCounter;
	int maxSplitsPerTick;
	int maxMergesPerTick;

	Planet(double gravity, double radius, double mass, double orbitalDistance, double splitThreshold, std::string name);
	~Planet();

	void initAtmosphere(Atmosphere* atmosphere);

	void init(GameObject* gameObject) override;

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) override;

	void physics(double delta, GameObject* gameObject) override;

	void input(double delta, GameObject* gameObject) override;

	bool pointBehindHorizon(glm::dvec3 camera, glm::dvec3 point);

	int getRenderChunkCount();

	double getGravity() const;

	double getRadius() const;

	double getMass() const;

	double getOrbitDistance() const;

	double getTerrainHeight(glm::dvec3 position);

	double getDistance(glm::dvec3 position, bool scaled = true);

	const double* getSplitDistances() const;

	const double* getMergeDistances() const;

	glm::dvec3 getRotatedPosition(glm::dvec3 position);

	glm::dvec3 getSurfaceVelocity(glm::dvec3 position);

	glm::dvec3 getRelativeCoords(glm::dvec3 position);

	std::string getName() const;

	TerrainGenerator*& getTerrain();
};

