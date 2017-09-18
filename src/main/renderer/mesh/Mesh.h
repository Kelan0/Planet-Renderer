#pragma once

#define ATTRIBUTE_VERTEX_GEOMETRIC 0
#define ATTRIBUTE_VERTEX_NORMAL 1
#define ATTRIBUTE_VERTEX_TEXTURE 2
#define ATTRIBUTE_VERTEX_COLOUR 3

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class MeshBuilder;

struct Transformation
{
	glm::dvec3 position;
	glm::dvec3 rotation;
	glm::dvec3 scale;

	Transformation(glm::dvec3 position = glm::dvec3(), glm::dvec3 rotation = glm::dvec3(), glm::dvec3 scale = glm::dvec3(1.0F)) : position(position), rotation(rotation), scale(scale) {}

	Transformation operator+(const Transformation& other) const;

	Transformation operator+=(const Transformation& other);

	Transformation operator-(const Transformation& other) const;

	Transformation operator-() const;

	Transformation operator-=(const Transformation& other);

	Transformation operator*(const Transformation& other) const;

	glm::dmat4x4 getTransformationMatrix();

	glm::dvec3 getPosition() const;
	
	glm::dvec3 getRotation() const;
	
	glm::dvec3 getScale() const;
};

class Mesh
{
private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ibo;

public:
	GLuint _numVertices;
	GLuint _numIndices;
	Mesh(MeshBuilder* meshBuilder);

	Mesh(GLuint numVertices, GLuint numIndices);

	~Mesh();

	GLuint const getVAO() const;

	GLuint const getVBO() const;

	GLuint const getIBO() const;

	int getNumVertices();

	int getNumIndices();

	void updateBuffers(std::vector<float> vertexData, std::vector<GLuint> indexData, int offsetBytes = 0, int indexOffsetBytes = 0);

	void render();
};

