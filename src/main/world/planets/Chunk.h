#pragma once
#include "../../renderer/mesh/Mesh.h"
#include "../sceneGraph/SceneGraph.h"
#include "../sceneGraph/components/MeshComponent.h"
#include "../boundingVolumes/BoundingVolume.h"
#include <mutex>

#define GRID_RESOLUTION 16 //the number of squares along one edge of the chunk grid
#define UNIT_RESOLUTION 1 //the number of texture pixels inbetween two vertices of the grid
#define TEXTURE_RESOLUTION GRID_RESOLUTION * UNIT_RESOLUTION
#define VERTICES_PER_CHUNK (GRID_RESOLUTION + 1) * (GRID_RESOLUTION + 1)

class Planet;

class Chunk
{
private:
	glm::dvec4** _terrainHeights;
	std::vector<float> _textureData;
	std::vector<float> _vertices;
	std::vector<GLuint> _indices;

	int _depth;
	bool _divided;
	bool _needsUpdate;
	double _size;
	double _boundingRadius;
	double _highestPoint;
	double _lowestPoint;

	glm::dvec3 _sphereVector;
	glm::dvec3 _cubeVector;
	glm::dvec3 _chunkPosition;
	glm::dvec3 _chunkCentre;
	glm::dvec3 _tangent;
	glm::dvec3 _bitangent;
	glm::dvec3 _corners[4];

	Chunk* _subChunks[4];
	Chunk* _root;

	Planet* _planet;
	Transformation _planetPosition;
public:
	
	Chunk(Planet* body, glm::dvec3 sphereVector, glm::dvec3 cubeVector, int depth);
	~Chunk();

	void init();

	void generateVertices(glm::dvec4** heights);

	void render(ShaderProgram* shaderProgram);

	void update(std::vector<Chunk>& updateList);

	void divideChildren();

	void mergeChildren();

	bool shouldSplit();

	bool shouldMerge();

	bool shouldRender() const;

	bool pointInsideChunk(glm::dvec3 point);

	Chunk*& child_t_l();
	Chunk*& child_t_r();
	Chunk*& child_b_l();
	Chunk*& child_b_r();

	Chunk*& getRoot();

	glm::dvec3& corner_tl();
	glm::dvec3& corner_tr();
	glm::dvec3& corner_bl();
	glm::dvec3& corner_br();

	Chunk* setCorners(glm::dvec3 tl, glm::dvec3 tr, glm::dvec3 br, glm::dvec3 bl);

	glm::dvec3 getSphereVector() const;

	glm::dvec3 getCubeVector() const;

	glm::dvec3 getTerrainNormal(int x, int y);

	glm::dvec3 distanceToCube(glm::dvec3 vector, float sideLength) const;

	glm::dvec4 getHeightmapAt(double gridx, double gridy) const;

	int getDepth() const;

	std::vector<float> getVertices() const;

	std::vector<GLuint> getIndices() const;
};

