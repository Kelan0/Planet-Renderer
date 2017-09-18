#include "Mesh.h"
#include <GL/glew.h>
#include "MeshBuilder.h"
#include <glm/gtc/matrix_transform.inl>
#include <iostream>
#include "../../world/planets/Chunk.h"

Transformation Transformation::operator+(const Transformation& other) const
{
	Transformation trans;
	trans.position = position + other.position;
	trans.rotation = rotation + other.rotation;
	trans.scale = scale + other.scale;

	return trans;
}

Transformation Transformation::operator+=(const Transformation& other)
{
	this->position += other.position;
	this->rotation += other.rotation;
	this->scale += other.scale;

	return *this;
}

Transformation Transformation::operator-(const Transformation& other) const
{
	Transformation trans;
	trans.position = position - other.position;
	trans.rotation = rotation - other.rotation;
	trans.scale = scale - other.scale;

	return trans;
}

Transformation Transformation::operator-() const
{
	Transformation transformation;
	transformation.position = -position;
	transformation.rotation = -rotation;

	return transformation;
}

Transformation Transformation::operator-=(const Transformation& other)
{
	this->position -= other.position;
	this->rotation -= other.rotation;
	this->scale -= other.scale;

	return *this;
}

Transformation Transformation::operator*(const Transformation& other) const
{
	glm::dmat4x4 ret(1.0F);

	ret = glm::rotate(ret, other.rotation.x, glm::dvec3(1.0, 0.0, 0.0));
	ret = glm::rotate(ret, other.rotation.y, glm::dvec3(0.0, 1.0, 0.0));
	ret = glm::rotate(ret, other.rotation.z, glm::dvec3(0.0, 0.0, 1.0));
	ret = glm::translate(ret, other.position);
	ret = glm::scale(ret, other.scale);

	Transformation transform;
	transform.position = glm::dvec3(ret * glm::dvec4(position, 1.0F));
	transform.rotation = glm::dvec3(ret * glm::dvec4(rotation, 0.0F));
	transform.scale = other.scale * scale;

	return transform;
}

glm::dmat4x4 Transformation::getTransformationMatrix()
{
	glm::dmat4x4 ret(1.0F);

	ret = glm::translate(ret, position);
	ret = glm::rotate(ret, rotation.x, glm::dvec3(1.0F, 0.0F, 0.0F));
	ret = glm::rotate(ret, rotation.y, glm::dvec3(0.0F, 1.0F, 0.0F));
	ret = glm::rotate(ret, rotation.z, glm::dvec3(0.0F, 0.0F, 1.0F));
	ret = glm::scale(ret, scale);

	return ret;
}

glm::dvec3 Transformation::getPosition() const
{
	return position;
}

glm::dvec3 Transformation::getRotation() const
{
	return rotation;
}

glm::dvec3 Transformation::getScale() const
{
	return scale;
}

Mesh::Mesh(MeshBuilder* meshBuilder): _vao(0), _ibo(0)
{
	_numVertices = meshBuilder->getNumVertices();
	_numIndices = meshBuilder->getNumIndices();

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(ATTRIBUTE_VERTEX_GEOMETRIC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, geometric)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, normal)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, texture)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_COLOUR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, colour)));
	glBufferData(GL_ARRAY_BUFFER, meshBuilder->getIBOSizeBytes(), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshBuilder->getIBOSizeBytes(), nullptr, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
	updateBuffers(meshBuilder->vertices, meshBuilder->indices);
}

Mesh::Mesh(GLuint numVertices, GLuint numIndices)
{
	_numVertices = numVertices;
	_numIndices = numIndices;

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(ATTRIBUTE_VERTEX_GEOMETRIC, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, geometric)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, normal)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, texture)));
	glVertexAttribPointer(ATTRIBUTE_VERTEX_COLOUR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(offsetof(Vertex, colour)));
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);

	updateBuffers(std::vector<float>(numVertices), std::vector<GLuint>(numIndices));
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &_ibo);
	glDeleteVertexArrays(1, &_vao);
}

GLuint const Mesh::getVAO() const
{
	return _vao;
}

GLuint const Mesh::getVBO() const
{
	return _vbo;
}

GLuint const Mesh::getIBO() const
{
	return _ibo;
}

int Mesh::getNumVertices()
{
	return _numVertices;
}

int Mesh::getNumIndices()
{
	return _numIndices;
}

void Mesh::updateBuffers(std::vector<float> vertexData, std::vector<GLuint> indexData, int vertexOffsetBytes, int indexOffsetBytes)
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertexData[0]), &vertexData[0], GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(indexData[0]), &indexData[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_numVertices = vertexData.size() * sizeof(float) / sizeof(Vertex);
	_numIndices = indexData.size();
}

void Mesh::render()
{
	glBindVertexArray(_vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glEnableVertexAttribArray(ATTRIBUTE_VERTEX_GEOMETRIC);
	glEnableVertexAttribArray(ATTRIBUTE_VERTEX_NORMAL);
	glEnableVertexAttribArray(ATTRIBUTE_VERTEX_TEXTURE);
	glEnableVertexAttribArray(ATTRIBUTE_VERTEX_COLOUR);

	glDrawElements(GL_TRIANGLES, getNumIndices(), GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(ATTRIBUTE_VERTEX_GEOMETRIC);
	glDisableVertexAttribArray(ATTRIBUTE_VERTEX_NORMAL);
	glDisableVertexAttribArray(ATTRIBUTE_VERTEX_TEXTURE);
	glDisableVertexAttribArray(ATTRIBUTE_VERTEX_COLOUR);
	glBindVertexArray(0);
}
