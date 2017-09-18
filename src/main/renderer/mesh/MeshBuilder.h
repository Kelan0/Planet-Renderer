#pragma once
#include <vector>
#include <GL/glew.h>
#include <map>
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 geometric;
	glm::vec3 normal;
	glm::vec2 texture;
	glm::vec4 colour;
};

class MeshBuilder
{
private:
	glm::vec3 _centre;
	glm::vec3 _maxBounds;
	glm::vec3 _minBounds;

public:
	MeshBuilder();
	~MeshBuilder();

	std::vector<glm::vec3> geometrics;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textures;
	std::vector<glm::vec4> colours;

	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;

	std::map<long long, GLuint> cachedLOD;

	int _numVertices;
	GLenum _renderMode;

	Vertex currentVertex;

	int getNumVertices();

	int getNumIndices();

	int getVBOSizeBytes();

	int getIBOSizeBytes();

	GLenum getRenderMode();

	GLuint addVertex();

	MeshBuilder* setGeometric(float x, float y, float z);
	
	MeshBuilder* setNormal(float x, float y, float z);
	
	MeshBuilder* setTexture(float u, float v);
	
	MeshBuilder* setColour(float r, float g, float b, float a);

	MeshBuilder* repeatTexture(int xRepetitions, int yRepetitions);

	MeshBuilder* triangulate(int LOD);

	MeshBuilder* normalize(float radius = 1.0, glm::vec3 centre = glm::vec3(0.0));

	MeshBuilder* normalize(double radius = 1.0F, glm::dvec3 centre = glm::dvec3(0.0F));

	MeshBuilder* offset(glm::vec3 offset);

	MeshBuilder* rotate(glm::vec3 rotation, glm::vec3 origin = glm::vec3(0.0F));

	MeshBuilder* scale(glm::vec3 scale);

	void createTriangle(int v0, int v1, int v2);

	void createQuad(int v0, int v1, int v2, int v3);

	void getData(std::vector<float>& dest);

	void getIndices(std::vector<GLuint>& dest);

	void uploadVertices(GLenum drawMode);

	void uploadIndices(GLenum drawMode);

	glm::vec3 getCentre();

	glm::vec3 getMinBounds() const;

	glm::vec3 getMaxBounds() const;

	GLuint getMidpoint(int v0, int v1);

	static MeshBuilder* createMeshBuilder(int numVertices, GLenum renderMode = GL_TRIANGLES);

	static MeshBuilder* createCube(float size = 1.0F, float width = 1.0F, float height = 1.0F, float depth = 1.0F, GLenum renderMode = GL_TRIANGLES);

	static MeshBuilder* createSphere(float radius = 0.5F, int rings = 30, int sectors = 30, GLenum renderMode = GL_TRIANGLES);

	static MeshBuilder* createIcosphere(float radius = 0.5F, int subdivisions = 3, GLenum renderMode = GL_TRIANGLES, MeshBuilder* mb = nullptr);

	static MeshBuilder* createIcosahedron(float radius = 0.5F, GLenum renderMode = GL_TRIANGLES, MeshBuilder* mb = nullptr);

	static MeshBuilder* createTorus(float innerRadius = 0.25F, float outerRadius = 0.75F, int rings = 30, int sectors = 30, GLenum renderMode = GL_TRIANGLES);

	static MeshBuilder* createPlane(float width = 1.0F, float height = 1.0F, int xDivisions = 1, int yDivisions = 1, glm::vec3 tangent = glm::vec3(1.0F, 0.0F, 0.0F), glm::vec3 bitangent = glm::vec3(0.0F, 0.0F, 1.0F), GLenum renderMode = GL_TRIANGLES);
};

