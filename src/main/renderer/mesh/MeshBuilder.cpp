#include "MeshBuilder.h"
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <glm/gtc/constants.inl>
#include <glm/gtc/matrix_transform.inl>

MeshBuilder::MeshBuilder(): _numVertices(0), _renderMode(GL_TRIANGLES)
{
}


MeshBuilder::~MeshBuilder()
{
}

int MeshBuilder::getNumVertices()
{
	return _numVertices;
}

int MeshBuilder::getNumIndices()
{
	return indices.size();
}

int MeshBuilder::getVBOSizeBytes()
{
	return sizeof(Vertex) * _numVertices;
}

int MeshBuilder::getIBOSizeBytes()
{
	return sizeof(GLuint) * indices.size();
}

GLenum MeshBuilder::getRenderMode()
{
	return _renderMode;
}

GLuint MeshBuilder::addVertex()
{
	_minBounds.x = std::min(currentVertex.geometric.x, _minBounds.x);
	_minBounds.y = std::min(currentVertex.geometric.y, _minBounds.y);
	_minBounds.z = std::min(currentVertex.geometric.z, _minBounds.z);

	_maxBounds.x = std::max(currentVertex.geometric.x, _maxBounds.x);
	_maxBounds.y = std::max(currentVertex.geometric.y, _maxBounds.y);
	_maxBounds.z = std::max(currentVertex.geometric.z, _maxBounds.z);

	geometrics.insert(geometrics.begin() + _numVertices, currentVertex.geometric);
	normals.insert(normals.begin() + _numVertices, currentVertex.normal);
	textures.insert(textures.begin() + _numVertices, currentVertex.texture);
	colours.insert(colours.begin() + _numVertices, currentVertex.colour);

	vertices.push_back(currentVertex.geometric.x);
	vertices.push_back(currentVertex.geometric.y);
	vertices.push_back(currentVertex.geometric.z);
	vertices.push_back(currentVertex.normal.x);
	vertices.push_back(currentVertex.normal.y);
	vertices.push_back(currentVertex.normal.z);
	vertices.push_back(currentVertex.texture.s);
	vertices.push_back(currentVertex.texture.t);
	vertices.push_back(currentVertex.colour.r);
	vertices.push_back(currentVertex.colour.g);
	vertices.push_back(currentVertex.colour.b);
	vertices.push_back(currentVertex.colour.a);

	_numVertices++;
	return _numVertices - 1;
}

MeshBuilder* MeshBuilder::setGeometric(float x, float y, float z)
{
	currentVertex.geometric = glm::vec3(x, y, z);
	return this;
}

MeshBuilder* MeshBuilder::setNormal(float x, float y, float z)
{
	currentVertex.normal = glm::normalize(glm::vec3(x, y, z));
	return this;
}

MeshBuilder* MeshBuilder::setTexture(float u, float v)
{
	currentVertex.texture = glm::vec2(u, v);
	return this;
}

MeshBuilder* MeshBuilder::setColour(float r, float g, float b, float a)
{
	currentVertex.colour = glm::vec4(r, g, b, a);
	return this;
}

MeshBuilder* MeshBuilder::repeatTexture(int xRepetitions, int yRepetitions)
{
	for (int i = 0; i < _numVertices; i++)
	{
		textures[i] *= glm::vec2(xRepetitions, yRepetitions);
	}

	return this;
}

MeshBuilder* MeshBuilder::triangulate(int LOD)
{
	std::cout << "Triangulating mesh with " << LOD << " iterations of triangulation\n";

	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < LOD; i++)
	{
		auto a = std::chrono::high_resolution_clock::now();
		std::vector<GLuint> indicesCopy(indices);
		indices.clear();

		int startVerts = getNumVertices();
		for (int j = 0; j < indicesCopy.size(); j += 3)
		{
			GLuint v0 = indicesCopy[j + 0];
			GLuint v1 = indicesCopy[j + 1];
			GLuint v2 = indicesCopy[j + 2];

			GLuint s0 = getMidpoint(v0, v1);
			GLuint s1 = getMidpoint(v1, v2);
			GLuint s2 = getMidpoint(v2, v0);

			createTriangle(v0, s0, s2);
			createTriangle(s0, v1, s1);
			createTriangle(s2, s1, v2);
			createTriangle(s0, s1, s2);
		}

		int numVerts = getNumVertices() - startVerts;

		auto b = std::chrono::high_resolution_clock::now();

		double nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(b - a).count();
		std::cout << "Completed LOD iteration " << i << " / " << LOD << ". Took " << nanos << "ns (" << (nanos / 1000000000.0) << " seconds). Added " << numVerts << " _vertices\n";
	}

	auto end = std::chrono::high_resolution_clock::now();

	double nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

	std::cout << "Completed LOD calculation with " << LOD << " iterations. Took " << nanos << "ns (" << (nanos / 1000000000.0) << " seconds). Total _vertices: " << getNumVertices() << std::endl;
	return this;
}

MeshBuilder* MeshBuilder::normalize(float radius, glm::vec3 centre)
{
	for (int i = 0; i < getNumVertices(); i++)
	{
		geometrics[i] = glm::normalize(glm::vec3(geometrics[i]) - centre) * radius;
		normals[i] = glm::normalize(glm::vec3(geometrics[i]) - centre);

		int vertex = i * 12;

		vertices[vertex + 0] = geometrics[i].x;
		vertices[vertex + 1] = geometrics[i].y;
		vertices[vertex + 2] = geometrics[i].z;
		vertices[vertex + 3] = geometrics[i].x;
		vertices[vertex + 4] = geometrics[i].y;
		vertices[vertex + 5] = geometrics[i].z;
	}

	return this;
}

MeshBuilder* MeshBuilder::normalize(double radius, glm::dvec3 centre)
{
	for (int i = 0; i < getNumVertices(); i++)
	{
		geometrics[i] = glm::normalize(glm::dvec3(geometrics[i]) - centre) * radius;
		normals[i] = glm::normalize(glm::dvec3(geometrics[i]) - centre);

		int vertex = i * 12;

		vertices[vertex + 0] = geometrics[i].x;
		vertices[vertex + 1] = geometrics[i].y;
		vertices[vertex + 2] = geometrics[i].z;
		vertices[vertex + 3] = geometrics[i].x;
		vertices[vertex + 4] = geometrics[i].y;
		vertices[vertex + 5] = geometrics[i].z;
	}

	return this;
}

MeshBuilder* MeshBuilder::offset(glm::vec3 offset)
{
	for (int i = 0; i < getNumVertices(); i++)
	{
		geometrics[i] += offset;

		int vertex = i * 12;

		vertices[vertex + 0] = geometrics[i].x;
		vertices[vertex + 1] = geometrics[i].y;
		vertices[vertex + 2] = geometrics[i].z;
	}

	return this;
}

MeshBuilder* MeshBuilder::rotate(glm::vec3 rotation, glm::vec3 origin)
{
	return this;
}

MeshBuilder* MeshBuilder::scale(glm::vec3 scale)
{
	for (int i = 0; i < getNumVertices(); i++)
	{
		geometrics[i] *= scale;

		int vertex = i * 12;

		vertices[vertex + 0] = geometrics[i].x;
		vertices[vertex + 1] = geometrics[i].y;
		vertices[vertex + 2] = geometrics[i].z;
	}

	return this;
}

void MeshBuilder::createTriangle(int v0, int v1, int v2)
{
	indices.push_back(v0);
	indices.push_back(v1);
	indices.push_back(v2);
}

void MeshBuilder::createQuad(int v0, int v1, int v2, int v3)
{
	createTriangle(v0, v3, v2);
	createTriangle(v0, v2, v1);
}

void MeshBuilder::getData(std::vector<float>& dest)
{
	for (int i = 0; i < _numVertices; i++)
	{
		dest.push_back(geometrics[i].x);
		dest.push_back(geometrics[i].y);
		dest.push_back(geometrics[i].z);
		dest.push_back(normals[i].x);
		dest.push_back(normals[i].y);
		dest.push_back(normals[i].z);
		dest.push_back(textures[i].s);
		dest.push_back(textures[i].t);
		dest.push_back(colours[i].r);
		dest.push_back(colours[i].g);
		dest.push_back(colours[i].b);
		dest.push_back(colours[i].a);
	}
}

void MeshBuilder::getIndices(std::vector<GLuint>& dest)
{
	dest = std::vector<GLuint>(indices);
}

void MeshBuilder::uploadVertices(GLenum drawMode)
{
	
}

void MeshBuilder::uploadIndices(GLenum drawMode)
{
	
}

glm::vec3 MeshBuilder::getCentre()
{
	_centre = glm::vec3();
	for (int i = 0; i < geometrics.size(); i++)
	{
		_centre += glm::vec3(geometrics[i]);
	}

	return _centre / (float)geometrics.size();
}

glm::vec3 MeshBuilder::getMinBounds() const
{
	return _minBounds;
}

glm::vec3 MeshBuilder::getMaxBounds() const
{
	return _maxBounds;
}

GLuint MeshBuilder::getMidpoint(int p1, int p2)
{
	bool firstIsSmaller = p1 < p2;
	long long smallerIndex = firstIsSmaller ? p1 : p2;
	long long greaterIndex = firstIsSmaller ? p2 : p1;
	long long key = (smallerIndex << 32) + greaterIndex;

	if (cachedLOD.count(key) != 0)
	{
		return cachedLOD[key];
	}

	glm::vec3 geometric = (geometrics[p1] + geometrics[p2]) / 2.0F;
	glm::vec3 normal = (normals[p1] + normals[p2]) / 2.0F;
	glm::vec2 texture = (textures[p1] + textures[p2]) / 2.0F;
	glm::vec4 colour = (colours[p1] + colours[p2]) / 2.0F;
	
	setGeometric(geometric.x, geometric.y, geometric.z);
	setNormal(normal.x, normal.y, normal.z);
	setTexture(texture.s, texture.t);
	setColour(colour.r, colour.g, colour.b, colour.a);

	GLuint index = addVertex();
	
	cachedLOD.insert(std::make_pair(key, index));
	return index;
}

MeshBuilder* MeshBuilder::createMeshBuilder(int numVertices, GLenum renderMode)
{
	MeshBuilder* mb = new MeshBuilder();

	mb->currentVertex.colour = glm::vec4(1.0F, 1.0F, 1.0F, 1.0F);

	mb->geometrics.reserve(numVertices);
	mb->normals.reserve(numVertices);
	mb->textures.reserve(numVertices);
	mb->colours.reserve(numVertices);
	mb->vertices.reserve(numVertices * 12);
	mb->indices.reserve(numVertices);
	return mb;
}

MeshBuilder* MeshBuilder::createCube(float size, float width, float height, float depth, GLenum renderMode)
{
	MeshBuilder* mb = createMeshBuilder(4);

	mb->setColour(1.0F, 1.0F, 1.0F, 1.0F);

	int v00 = mb->setGeometric(+0.5F, +0.5F, -0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
	int v01 = mb->setGeometric(-0.5F, +0.5F, -0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
	int v02 = mb->setGeometric(-0.5F, +0.5F, +0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
	int v03 = mb->setGeometric(+0.5F, +0.5F, +0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();
	
	int v04 = mb->setGeometric(+0.5F, -0.5F, -0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
	int v05 = mb->setGeometric(-0.5F, -0.5F, -0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
	int v06 = mb->setGeometric(-0.5F, -0.5F, +0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
	int v07 = mb->setGeometric(+0.5F, -0.5F, +0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();
	
	int v08 = mb->setGeometric(+0.5F, +0.5F, +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(1.0F, 0.0F)->addVertex();
	int v09 = mb->setGeometric(-0.5F, +0.5F, +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(0.0F, 0.0F)->addVertex();
	int v10 = mb->setGeometric(-0.5F, -0.5F, +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(0.0F, 1.0F)->addVertex();
	int v11 = mb->setGeometric(+0.5F, -0.5F, +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(1.0F, 1.0F)->addVertex();
	
	int v12 = mb->setGeometric(+0.5F, -0.5F, -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(1.0F, 0.0F)->addVertex();
	int v13 = mb->setGeometric(-0.5F, -0.5F, -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(0.0F, 0.0F)->addVertex();
	int v14 = mb->setGeometric(-0.5F, +0.5F, -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(0.0F, 1.0F)->addVertex();
	int v15 = mb->setGeometric(+0.5F, +0.5F, -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(1.0F, 1.0F)->addVertex();
	
	int v16 = mb->setGeometric(-0.5F, +0.5F, +0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
	int v17 = mb->setGeometric(-0.5F, +0.5F, -0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
	int v18 = mb->setGeometric(-0.5F, -0.5F, -0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
	int v19 = mb->setGeometric(-0.5F, -0.5F, +0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();

	int v20 = mb->setGeometric(+0.5F, +0.5F, +0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
	int v21 = mb->setGeometric(+0.5F, +0.5F, -0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
	int v22 = mb->setGeometric(+0.5F, -0.5F, -0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
	int v23 = mb->setGeometric(+0.5F, -0.5F, +0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();

	mb->createQuad(v00, v03, v02, v01);
	mb->createQuad(v04, v05, v06, v07);
	mb->createQuad(v08, v11, v10, v09);
	mb->createQuad(v12, v15, v14, v13);
	mb->createQuad(v16, v19, v18, v17);
	mb->createQuad(v20, v21, v22, v23);
//
//	int v00 = mb->setGeometric(size * width * -0.5F, size * height * -0.5F, size * depth * +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(0.0F, 1.0F)->addVertex();
//	int v01 = mb->setGeometric(size * width * +0.5F, size * height * -0.5F, size * depth * +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(1.0F, 1.0F)->addVertex();
//	int v02 = mb->setGeometric(size * width * -0.5F, size * height * +0.5F, size * depth * +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(0.0F, 0.0F)->addVertex();
//	int v03 = mb->setGeometric(size * width * +0.5F, size * height * +0.5F, size * depth * +0.5F)->setNormal(0.0F, 0.0F, +1.0F)->setTexture(1.0F, 0.0F)->addVertex();
//	int v04 = mb->setGeometric(size * width * -0.5F, size * height * -0.5F, size * depth * -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(1.0F, 1.0F)->addVertex();
//	int v05 = mb->setGeometric(size * width * +0.5F, size * height * -0.5F, size * depth * -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(0.0F, 1.0F)->addVertex();
//	int v06 = mb->setGeometric(size * width * -0.5F, size * height * +0.5F, size * depth * -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(1.0F, 0.0F)->addVertex();
//	int v07 = mb->setGeometric(size * width * +0.5F, size * height * +0.5F, size * depth * -0.5F)->setNormal(0.0F, 0.0F, -1.0F)->setTexture(0.0F, 0.0F)->addVertex();
//	int v08 = mb->setGeometric(size * width * -0.5F, size * height * +0.5F, size * depth * +0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
//	int v09 = mb->setGeometric(size * width * +0.5F, size * height * +0.5F, size * depth * +0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();
//	int v10 = mb->setGeometric(size * width * -0.5F, size * height * +0.5F, size * depth * -0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
//	int v11 = mb->setGeometric(size * width * +0.5F, size * height * +0.5F, size * depth * -0.5F)->setNormal(0.0F, +1.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
//	int v12 = mb->setGeometric(size * width * -0.5F, size * height * -0.5F, size * depth * +0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();
//	int v13 = mb->setGeometric(size * width * +0.5F, size * height * -0.5F, size * depth * +0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
//	int v14 = mb->setGeometric(size * width * -0.5F, size * height * -0.5F, size * depth * -0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
//	int v15 = mb->setGeometric(size * width * +0.5F, size * height * -0.5F, size * depth * -0.5F)->setNormal(0.0F, -1.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
//	int v16 = mb->setGeometric(size * width * -0.5F, size * height * -0.5F, size * depth * +0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();
//	int v17 = mb->setGeometric(size * width * -0.5F, size * height * +0.5F, size * depth * +0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
//	int v18 = mb->setGeometric(size * width * -0.5F, size * height * -0.5F, size * depth * -0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
//	int v19 = mb->setGeometric(size * width * -0.5F, size * height * +0.5F, size * depth * -0.5F)->setNormal(-1.0F, 0.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
//	int v20 = mb->setGeometric(size * width * +0.5F, size * height * -0.5F, size * depth * +0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(0.0F, 1.0F)->addVertex();
//	int v21 = mb->setGeometric(size * width * +0.5F, size * height * +0.5F, size * depth * +0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(0.0F, 0.0F)->addVertex();
//	int v22 = mb->setGeometric(size * width * +0.5F, size * height * -0.5F, size * depth * -0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(1.0F, 1.0F)->addVertex();
//	int v23 = mb->setGeometric(size * width * +0.5F, size * height * +0.5F, size * depth * -0.5F)->setNormal(+1.0F, 0.0F, 0.0F)->setTexture(1.0F, 0.0F)->addVertex();
//
//	mb->createTriangle(v00, v01, v02);
//	mb->createTriangle(v01, v03, v02);
//	mb->createTriangle(v06, v07, v05);
//	mb->createTriangle(v06, v05, v04);
//	mb->createTriangle(v08, v09, v10);
//	mb->createTriangle(v09, v10, v11);
//	mb->createTriangle(v12, v13, v14);
//	mb->createTriangle(v13, v14, v15);
//	mb->createTriangle(v17, v16, v18);
//	mb->createTriangle(v17, v18, v19);
//	mb->createTriangle(v20, v21, v22);
//	mb->createTriangle(v21, v22, v23);

	return mb;
}

MeshBuilder* MeshBuilder::createSphere(float radius, int rings, int sectors, GLenum renderMode)
{
	float const R = 1.0F / (float)(rings - 1);
	float const S = 1.0F / (float)(sectors - 1);
	int r, s;

	MeshBuilder* mb = createMeshBuilder(rings * sectors);

	mb->setColour(1.0F, 1.0F, 1.0F, 1.0F);
	std::vector<GLuint> indices;
	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const x = cos(2.0F * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
			float const y = sin(-glm::half_pi<float>() + glm::pi<float>() * r * R);
			float const z = sin(2.0F * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);

			mb->setGeometric(x * radius, y * radius, z * radius)->setNormal(x, y, z)->setTexture(1.0f - s * S, 1.0F - r * R)->addVertex();
		}
	}

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			int v0 = r * sectors + s;
			int v1 = r * sectors + (s + 1);
			int v2 = (r + 1) * sectors + (s + 1);
			int v3 = (r + 1) * sectors + s;

			mb->createQuad(v0, v1, v2, v3);
		}
	}

	return mb;
}

MeshBuilder* MeshBuilder::createIcosphere(float radius, int subdivisions, GLenum renderMode, MeshBuilder* mb)
{
	mb = createIcosahedron(0.5, GL_TRIANGLES, mb);
	mb->triangulate(subdivisions);
	mb->normalize(radius);

	return mb;
}

MeshBuilder* MeshBuilder::createIcosahedron(float radius, GLenum renderMode, MeshBuilder* mb)
{
	if (mb == nullptr)
	{
		mb = MeshBuilder::createMeshBuilder(12);
	}
	float t = glm::golden_ratio<float>();

	mb->setGeometric(-1.0F, +t, +0.0F)->setNormal(-1.0F, +t, +0.0F)->addVertex();
	mb->setGeometric(+1.0F, +t, +0.0F)->setNormal(+1.0F, +t, +0.0F)->addVertex();
	mb->setGeometric(-1.0F, -t, +0.0F)->setNormal(-1.0F, -t, +0.0F)->addVertex();
	mb->setGeometric(+1.0F, -t, +0.0F)->setNormal(+1.0F, -t, +0.0F)->addVertex();
	mb->setGeometric(+0.0F, -1.0F, +t)->setNormal(+0.0F, -1.0F, +t)->addVertex();
	mb->setGeometric(+0.0F, +1.0F, +t)->setNormal(+0.0F, +1.0F, +t)->addVertex();
	mb->setGeometric(+0.0F, -1.0F, -t)->setNormal(+0.0F, -1.0F, -t)->addVertex();
	mb->setGeometric(+0.0F, +1.0F, -t)->setNormal(+0.0F, +1.0F, -t)->addVertex();
	mb->setGeometric(+t, +0.0F, -1.0F)->setNormal(+t, +0.0F, -1.0F)->addVertex();
	mb->setGeometric(+t, +0.0F, +1.0F)->setNormal(+t, +0.0F, +1.0F)->addVertex();
	mb->setGeometric(-t, +0.0F, -1.0F)->setNormal(-t, +0.0F, -1.0F)->addVertex();
	mb->setGeometric(-t, +0.0F, +1.0F)->setNormal(-t, +0.0F, +1.0F)->addVertex();

	mb->createTriangle(0, 11, 5);
	mb->createTriangle(0, 5, 1);
	mb->createTriangle(0, 1, 7);
	mb->createTriangle(0, 7, 10);
	mb->createTriangle(0, 10, 11);
	mb->createTriangle(1, 5, 9);
	mb->createTriangle(5, 11, 4);
	mb->createTriangle(11, 10, 2);
	mb->createTriangle(10, 7, 6);
	mb->createTriangle(7, 1, 8);
	mb->createTriangle(3, 9, 4);
	mb->createTriangle(3, 4, 2);
	mb->createTriangle(3, 2, 6);
	mb->createTriangle(3, 6, 8);
	mb->createTriangle(3, 8, 9);
	mb->createTriangle(4, 9, 5);
	mb->createTriangle(2, 4, 11);
	mb->createTriangle(6, 2, 10);
	mb->createTriangle(8, 6, 7);
	mb->createTriangle(9, 8, 1);

	return mb;
}

MeshBuilder* MeshBuilder::createTorus(float innerRadius, float outerRadius, int rings, int sectors, GLenum renderMode)
{
	float pi = glm::pi<float>();

	MeshBuilder* mb = createMeshBuilder(rings * sectors);

	std::vector<glm::vec3> p(rings);
	float a = 0.0F;
	float step = 2.0F * pi / (rings - 1);

	for (int i = 0; i < rings; ++i)
	{
		p[i] = glm::vec3(cos(a) * outerRadius, sin(a) * outerRadius, 0.0F);
		a += step;
	}

	for (int r = 0; r < rings; r++)
	{
		glm::vec3 u = glm::normalize(p[r]) * innerRadius;

		a = 0.0F;
		step = 2.0F * pi / (sectors - 1);
		for (int s = 0; s < sectors; s++)
		{
			glm::vec3 geometric = p[r] + cos(a) * u + glm::vec3(0.0F, 0.0F, sin(a) * innerRadius);
			glm::vec3 normal = glm::normalize(geometric - p[r]);
			glm::vec2 texture(float(r) / (rings - 1), float(s) / (sectors - 1));
			mb->setGeometric(geometric.x, geometric.y, geometric.z)->setNormal(normal.x, normal.y, normal.z)->setTexture(texture.x, texture.y)->addVertex();
			a += step;
		}
	}

	int index = 0;
	for (int i = 0; i < rings; ++i)
	{
		int i1 = i;
		int i2 = (i1 + 1) % rings;

		for (int j = 0; j < sectors; ++j)
		{
			int j1 = j;
			int j2 = (j1 + 1) % sectors;

			mb->createTriangle(i1 * sectors + j1, i1 * sectors + j2, i2 * sectors + j1);
			mb->createTriangle(i2 * sectors + j2, i2 * sectors + j1, i1 * sectors + j2);
		}
	}

	return mb;
}

MeshBuilder* MeshBuilder::createPlane(float width, float height, int xDivisions, int yDivisions, glm::vec3 tangent, glm::vec3 bitangent, GLenum renderMode)
{
	xDivisions = std::max(1, xDivisions) + 1;
	yDivisions = std::max(1, yDivisions) + 1;
	MeshBuilder* mb = createMeshBuilder(xDivisions * yDivisions);
	mb->setColour(1.0F, 1.0F, 1.0F, 1.0F);

	glm::vec3 normal = glm::normalize(glm::cross(tangent, bitangent));
	glm::vec3 c;
	glm::vec3 bl(c - (tangent * width) - (bitangent * height));
	glm::vec3 br(c + (tangent * width) - (bitangent * height));
	glm::vec3 tr(c + (tangent * width) + (bitangent * height));
	glm::vec3 tl(c - (tangent * width) + (bitangent * height));

	for (float i = 0.0F; i < xDivisions; i++)
	{
		float x = i / (xDivisions - 1);
		glm::vec3 it = ((0.0F + x) * tl + (1.0F - x) * tr);
		glm::vec3 ib = ((0.0F + x) * bl + (1.0F - x) * br);

		for (float j = 0.0F; j < yDivisions; j++)
		{
			//			glm::vec3 geometric = glm::vec3(((float)j / ((float)xDivisions - 1) * width) - (width / 2.0F), 0.0F, ((float)i / ((float)yDivisions - 1) * height) - (height / 2.0F));

			float y = j / (yDivisions - 1);
			glm::vec3 geometric = ((0.0F + y) * it + (1.0F - y) * ib);

			mb->setGeometric(geometric.x, geometric.y, geometric.z);
			mb->setNormal(normal.x, normal.y, normal.z);
			mb->setTexture((float)i / ((float)xDivisions - 1), (float)j / ((float)yDivisions - 1));
			mb->addVertex();

			if (i < xDivisions - 1 && j < yDivisions - 1)
			{
				int topLeft = i * xDivisions + j;
				int topRight = topLeft + 1;
				int bottomLeft = (i + 1) * yDivisions + j;
				int bottomRight = bottomLeft + 1;

				mb->createTriangle(topLeft, topRight, bottomLeft);
				mb->createTriangle(bottomLeft, topRight, bottomRight);
			}
		}
	}

	return mb;
}

//MeshBuilder* MeshBuilder::createPlane(float width, float height, int xDivisions, int yDivisions, glm::vec3 tangent, glm::vec3 bitangent, GLenum renderMode)
//{
//	xDivisions = std::max(1, xDivisions) + 1;
//	yDivisions = std::max(1, yDivisions) + 1;
//	MeshBuilder* mb = createMeshBuilder(xDivisions * yDivisions);
//	mb->setColour(1.0F, 1.0F, 1.0F, 1.0F);
//
//	glm::vec3 normal = glm::cross(tangent, bitangent);
//	glm::vec3 n0 = glm::normalize(glm::vec3(0.0F, 1.0F, 0.0F));
//	glm::vec3 n1 = glm::normalize(normal);
//
//	glm::vec3 axis = glm::cross(n1, n0);
//	float angle = glm::degrees(acos(glm::dot(n1, n0)));// / glm::length(n0) * glm::length(n1))));
//
//	for (float i = 0.0F; i < xDivisions; i++)
//	{
//		for (float j = 0.0F; j < xDivisions; j++)
//		{
//			glm::vec3 geometric = glm::vec3(((float)j / ((float)xDivisions - 1) * width) - (width / 2.0F), 0.0F, ((float)i / ((float)yDivisions - 1) * height) - (height / 2.0F));
//
//			if (glm::dot(axis, axis) > 0.0F)
//			{
////				geometric = glm::mat3x3(glm::rotate(glm::mat4x4(1.0F), angle, axis)) * geometric;
//				glm::mat4x4 m(1.0);
//
//				m = glm::rotate(m, angle, axis);
//				geometric = glm::mat3x3(m) * geometric;
//			}
//
//			mb->setGeometric(geometric.x, geometric.y, geometric.z);
//			mb->setNormal(n1.x, n1.y, n1.z);
//			mb->setTexture((float)j / ((float)xDivisions - 1), (float)i / ((float)yDivisions - 1));
//			mb->addVertex();
//		}
//	}
//
//	for (int i = 0; i < yDivisions - 1; i++)
//	{
//		for (int j = 0; j < xDivisions - 1; j++)
//		{
//			int topLeft = i * xDivisions + j;
//			int topRight = topLeft + 1;
//			int bottomLeft = (i + 1) * yDivisions + j;
//			int bottomRight = bottomLeft + 1;
//
//			mb->createTriangle(topLeft, topRight, bottomLeft);
//			mb->createTriangle(bottomLeft, topRight, bottomRight);
//		}
//	}
//
//	return mb;
//}
