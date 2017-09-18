#include "Chunk.h"
#include "../../renderer/mesh/MeshBuilder.h"
#include <iostream>
#include "../planets/Planet.h"
#include "../../VoxelGame.h"
#include "../../core/Player.h"
#include <algorithm>
#include <windows.h>
#include <glm/gtc/constants.inl>
#include <glm/gtx/rotate_vector.inl>

Chunk::Chunk(Planet* body, glm::dvec3 sphereVector, glm::dvec3 cubeVector, int depth): _planet(body), _divided(false), _root(this)
{
	_sphereVector = sphereVector;
	_cubeVector = cubeVector;
	_depth = depth;
}

Chunk::~Chunk()
{
//	if (_subChunks != nullptr)
//	{
//		delete[] _subChunks;
//	}
}

void Chunk::init()
{
	_chunkPosition = glm::normalize(_sphereVector);

	if (std::abs(_cubeVector.x) > 0.0) _tangent = glm::dvec3(0.0, 0.0, 1.0);
	else if (std::abs(_cubeVector.y) > 0.0) _tangent = glm::dvec3(1.0, 0.0, 0.0);
	else if (std::abs(_cubeVector.z) > 0.0) _tangent = glm::dvec3(1.0, 0.0, 0.0);

	_bitangent = glm::cross(_cubeVector, _tangent);
	_size = (4.0 / GRID_RESOLUTION) / (2 << _depth);
	glm::dvec4** _oceanHeights;

	_terrainHeights = new glm::dvec4*[GRID_RESOLUTION + 3];
	_oceanHeights = new glm::dvec4*[GRID_RESOLUTION + 3];

	for (int i = 0; i <= GRID_RESOLUTION + 2; i++)
	{
		int x = i - 1;
		_terrainHeights[i] = new glm::dvec4[GRID_RESOLUTION + 3];
		_oceanHeights[i] = new glm::dvec4[GRID_RESOLUTION + 3];
		for (int j = 0; j <= GRID_RESOLUTION + 2; j++)
		{
			int y = j - 1;
			glm::dvec3 v = glm::normalize(_sphereVector + _tangent * (x - (GRID_RESOLUTION) / 2.0) * _size + _bitangent * (y - (GRID_RESOLUTION) / 2.0) * _size);
			double h = _planet->getTerrainHeight(v);
			_terrainHeights[i][j] = glm::dvec4(v.x, v.y, v.z, h);
			_oceanHeights[i][j] = glm::dvec4(v.x, v.y, v.z, 0.0);

			if (h < _lowestPoint) _lowestPoint = h;
			if (h > _highestPoint) _highestPoint = h;
		}
	}

	generateVertices(_terrainHeights);

	if (_lowestPoint < 0.0)
	{
		generateVertices(_oceanHeights);
	}

	double a = _planet->cullRadius - _planet->getRadius();
	if (_lowestPoint < a)
	{
		_planet->cullRadius = _planet->getRadius() + _lowestPoint;
	}
	glm::dvec3 dimensions = glm::dvec3(_size * 4.0, _highestPoint - _lowestPoint, _size * 4.0);
	_boundingRadius = glm::length(dimensions) * _planet->getRadius() * 1.5;
}

void Chunk::generateVertices(glm::dvec4** heights)
{
	int indexOffset = _vertices.size() * sizeof(float) / sizeof(Vertex); //offset all the indices by the number of vertices that already exist
	for (int i = 0; i <= GRID_RESOLUTION; i++)
	{
		for (int j = 0; j <= GRID_RESOLUTION; j++)
		{
			glm::dvec4 point = heights[i + 1][j + 1];
			double h = point.w;
			glm::dvec3 v(point.x, point.y, point.z);

			glm::dvec4 p1 = heights[i + 0][j + 1];
			glm::dvec4 p2 = heights[i + 1][j + 0];
			glm::dvec4 p3 = heights[i + 2][j + 1];
			glm::dvec4 p4 = heights[i + 1][j + 2];

			glm::dvec3 v1 = glm::dvec3(p1) + glm::dvec3(p1) * p1.w - v;
			glm::dvec3 v2 = glm::dvec3(p2) + glm::dvec3(p2) * p2.w - v;
			glm::dvec3 v3 = glm::dvec3(p3) + glm::dvec3(p3) * p3.w - v;
			glm::dvec3 v4 = glm::dvec3(p4) + glm::dvec3(p4) * p4.w - v;

			glm::dvec3 v12 = glm::cross(v1, v2);
			glm::dvec3 v23 = glm::cross(v2, v3);
			glm::dvec3 v34 = glm::cross(v3, v4);
			glm::dvec3 v41 = glm::cross(v4, v1);

			glm::dvec3 n = glm::normalize(v12 + v23 + v34 + v41);
			glm::dvec2 t = glm::dvec2(double(i) / GRID_RESOLUTION, double(j) / GRID_RESOLUTION);
			glm::dvec3 c = glm::dvec3(_planet->getTerrain()->getColour(h / _planet->getTerrain()->getMaxHeight()));

			//geometric
			_vertices.push_back(v.x + v.x * h);
			_vertices.push_back(v.y + v.y * h);
			_vertices.push_back(v.z + v.z * h);
			//normal
			_vertices.push_back(n.x);
			_vertices.push_back(n.y);
			_vertices.push_back(n.z);
			//texture
			_vertices.push_back(t.x);
			_vertices.push_back(t.y);
			//colour
			_vertices.push_back(c.x);
			_vertices.push_back(c.y);
			_vertices.push_back(c.z);
			_vertices.push_back(1.0);

			if (i < GRID_RESOLUTION && j < GRID_RESOLUTION)
			{
				int topLeft = i * (GRID_RESOLUTION + 1) + j;
				int topRight = topLeft + 1;
				int bottomLeft = (i + 1) * (GRID_RESOLUTION + 1) + j;
				int bottomRight = bottomLeft + 1;

				_indices.push_back(indexOffset + bottomLeft);
				_indices.push_back(indexOffset + topRight);
				_indices.push_back(indexOffset + topLeft);
				_indices.push_back(indexOffset + bottomRight);
				_indices.push_back(indexOffset + topRight);
				_indices.push_back(indexOffset + bottomLeft);
			}
		}
	}
}

void Chunk::render(ShaderProgram* shaderProgram)
{
//	shaderProgram->setUniform("useLighting", false);
//	glPointSize(8);
//	glBegin(GL_LINES);
//	glVertexAttrib4f(3, 1.0, 0.0, 1.0, 1.0);
//	
//	glm::dvec3 r = glm::normalize(_sphereVector);
//	glm::dvec3 q = r;// (r + r * (_highestPoint + _lowestPoint) * 0.5);
//	glm::dvec3 p;
//
//	p = q + _boundingBox->getCorners()[0]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[1]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[1]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[2]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[2]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[3]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[3]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[0]; glVertexAttrib3f(0, p.x, p.y, p.z);
//
//	p = q + _boundingBox->getCorners()[4]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[5]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[5]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[6]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[6]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[7]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[7]; glVertexAttrib3f(0, p.x, p.y, p.z);
//	p = q + _boundingBox->getCorners()[4]; glVertexAttrib3f(0, p.x, p.y, p.z);
//
//	glDrawArrays(GL_LINES, 0, 1);
//	glEnd();
//	glPointSize(1);
//
////	_boundingBox->renderBoundingBox();
//	shaderProgram->setUniform("useLighting", true);
}

void Chunk::update(std::vector<Chunk>& updateList)
{
	_planetPosition = _planet->getAbsoluteTransformation(false);
	_chunkCentre = _planetPosition.position + (_chunkPosition + _chunkPosition * (_lowestPoint + _highestPoint) * 0.5) * _planet->getRadius();

	if (VoxelGame::player->getCamera()->_updateFrustum)
	{
		if (shouldSplit())
		{
			divideChildren();
		}
		else if (shouldMerge())
		{
			mergeChildren();
		}
	}

	if (_divided) //if the chunk has been divided, render it's subchunks
	{
		for (int i = 0; i < 4; ++i)
		{
			_subChunks[i]->update(updateList);
		}
	}
	else
	{
		updateList.push_back(*this);
	}
}

void Chunk::divideChildren()
{
	glm::dvec3 tl = corner_tl();
	glm::dvec3 tr = corner_tr();
	glm::dvec3 br = corner_br();
	glm::dvec3 bl = corner_bl();
	glm::dvec3 c = (tl + tr + br + bl) / 4.0;

	glm::dvec3 tc = (tl + tr) / 2.0;
	glm::dvec3 rc = (tr + br) / 2.0;
	glm::dvec3 bc = (bl + br) / 2.0;
	glm::dvec3 lc = (tl + bl) / 2.0;

	glm::dvec3 tl_v = (tl + c) / 2.0;
	glm::dvec3 tr_v = (tr + c) / 2.0;
	glm::dvec3 br_v = (br + c) / 2.0;
	glm::dvec3 bl_v = (bl + c) / 2.0;

	child_t_l() = (new Chunk(_planet, tl_v, _cubeVector, _depth + 1))->setCorners(tl, tc, c, lc); child_t_l()->init();
	child_t_r() = (new Chunk(_planet, tr_v, _cubeVector, _depth + 1))->setCorners(tc, tr, rc, c); child_t_r()->init();
	child_b_r() = (new Chunk(_planet, br_v, _cubeVector, _depth + 1))->setCorners(c, rc, br, bc); child_b_r()->init();
	child_b_l() = (new Chunk(_planet, bl_v, _cubeVector, _depth + 1))->setCorners(lc, c, bc, bl); child_b_l()->init();

	child_t_l()->getRoot() = getRoot();
	child_t_r()->getRoot() = getRoot();
	child_b_r()->getRoot() = getRoot();
	child_b_l()->getRoot() = getRoot();
	_divided = true;
	_planet->splitCounter++;
}

void Chunk::mergeChildren()
{
	if (this != nullptr && _divided)
	{
		_divided = false;
		if (_subChunks != nullptr)
		{
			for (int i = 0; i < 4; i++)
			{
				_subChunks[i]->mergeChildren();
				delete _subChunks[i]; //TODO FIX MEMORY LEAK, CHUNKS STAY IN MEMORY
			}
		}
	}
	_planet->mergeCounter++;
}

bool Chunk::shouldSplit()
{
	if (_depth < 0 || _depth >= MAX_LOD_DEPTH || _planet->splitCounter >= _planet->maxSplitsPerTick || _divided)
	{
		return false;
	}

	glm::dvec3 chunk = _planetPosition.position + (_chunkPosition + _chunkPosition * _planet->getTerrainHeight(VoxelGame::player->getHeadPosition())) * _planet->getRadius();
	glm::dvec3 player = VoxelGame::player->getHeadPosition(false);
	
	double distance = glm::distance(chunk, player);

	return distance < _planet->getSplitDistances()[_depth];
}

bool Chunk::shouldMerge()
{
	if (_depth < 0 || _depth >= MAX_LOD_DEPTH || _planet->mergeCounter >= _planet->maxMergesPerTick || !_divided)
	{
		return false;
	}

	glm::dvec3 chunk = _planetPosition.position + (_chunkPosition + _chunkPosition * _planet->getTerrainHeight(VoxelGame::player->getHeadPosition())) * _planet->getRadius();
	glm::dvec3 player = VoxelGame::player->getHeadPosition(false);

	double distance = glm::distance(chunk, player);

	return distance > _planet->getMergeDistances()[_depth];
}

bool Chunk::shouldRender() const
{
	bool inFrustum = VoxelGame::player->getCamera()->sphereInFrustum(glm::vec3(_chunkCentre), _boundingRadius * VoxelGame::scale);
	bool occluded = false;

	if (inFrustum && !occluded)
	{
		return true;
	}

	return false;
}

bool Chunk::pointInsideChunk(glm::dvec3 point)
{
	return false;
}

Chunk*& Chunk::child_t_l()
{
	return _subChunks[0];
}

Chunk*& Chunk::child_t_r()
{
	return _subChunks[1];
}

Chunk*& Chunk::child_b_l()
{
	return _subChunks[2];
}

Chunk*& Chunk::child_b_r()
{
	return _subChunks[3];
}

Chunk*& Chunk::getRoot()
{
	return _root;
}

glm::dvec3& Chunk::corner_tl()
{
	return _corners[0];
}

glm::dvec3& Chunk::corner_tr()
{
	return  _corners[1];
}

glm::dvec3& Chunk::corner_bl()
{
	return _corners[3];
}

glm::dvec3& Chunk::corner_br()
{
	return _corners[2];
}

Chunk* Chunk::setCorners(glm::dvec3 tl, glm::dvec3 tr, glm::dvec3 br, glm::dvec3 bl)
{
	corner_tl() = tl;
	corner_tr() = tr;
	corner_bl() = bl;
	corner_br() = br;

	return this;
}

glm::dvec3 Chunk::getSphereVector() const
{
	return _sphereVector;
}

glm::dvec3 Chunk::getCubeVector() const
{
	return _cubeVector;
}

glm::dvec3 Chunk::getTerrainNormal(int x, int y)
{
	glm::dvec3 n;

	glm::dvec3 vL = glm::normalize(_sphereVector + _tangent * ((x - 1) - (GRID_RESOLUTION) / 2.0) * _size + _bitangent * ((y - 0) - (GRID_RESOLUTION) / 2.0) * _size);
	glm::dvec3 vR = glm::normalize(_sphereVector + _tangent * ((x + 1) - (GRID_RESOLUTION) / 2.0) * _size + _bitangent * ((y + 0) - (GRID_RESOLUTION) / 2.0) * _size);
	glm::dvec3 vD = glm::normalize(_sphereVector + _tangent * ((x - 0) - (GRID_RESOLUTION) / 2.0) * _size + _bitangent * ((y - 1) - (GRID_RESOLUTION) / 2.0) * _size);
	glm::dvec3 vU = glm::normalize(_sphereVector + _tangent * ((x + 0) - (GRID_RESOLUTION) / 2.0) * _size + _bitangent * ((y + 1) - (GRID_RESOLUTION) / 2.0) * _size);

	float hL = glm::length(vL + vL * _planet->getTerrainHeight(vL));
	float hR = glm::length(vR + vR * _planet->getTerrainHeight(vR));
	float hD = glm::length(vD + vD * _planet->getTerrainHeight(vD));
	float hU = glm::length(vU + vU * _planet->getTerrainHeight(vU));
	
	n.x = hL - hR;
	n.y = hD - hU;
	n.z = 2.0;
	return glm::normalize(n);
}

glm::dvec3 Chunk::distanceToCube(glm::dvec3 vector, float sideLength) const
{
	sideLength /= 2.0F;
	vector = glm::normalize(vector);

	double m = max(max(std::abs(vector.x), std::abs(vector.y)), std::abs(vector.z));

	return vector * (sideLength / m);
}

glm::dvec4 Chunk::getHeightmapAt(double gridx, double gridy) const
{
	int x = (int)((gridx / GRID_RESOLUTION) * TEXTURE_RESOLUTION);
	int y = (int)((gridy / GRID_RESOLUTION) * TEXTURE_RESOLUTION);

	return _terrainHeights[x + 1][y + 1];
}

int Chunk::getDepth() const
{
	return _depth;
}

std::vector<float> Chunk::getVertices() const
{
	return _vertices;
}

std::vector<GLuint> Chunk::getIndices() const
{
	return _indices;
}
