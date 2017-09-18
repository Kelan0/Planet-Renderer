#include "TerrainGenerator.h"
#include <complex>
#include <iostream>
#include <algorithm>
#include <glm/detail/func_common.inl>

TerrainGenerator::TerrainGenerator(double maxHeight, double minHeight, double noiseScale, double noisePersistence, double noiseLacunarity, int noiseOctaves, int seed, Function function): _function(function), _maxHeight(maxHeight), _minHeight(minHeight), _maxBound(maxHeight), _minBound(minHeight), _noiseScale(noiseScale), _noisePersistence(noisePersistence), _noiseLacunarity(noiseLacunarity), _noiseOctaves(noiseOctaves), _noiseExponent(1)
{
	_noise = new SimplexNoise();
	_noise->setSeed(seed);
}

TerrainGenerator::~TerrainGenerator()
{
}

double TerrainGenerator::getUnscaledHeight(SimplexNoise* noise, double x, double y, double z)
{
	double result = 0.0;
	double frequency = _noiseScale;
	double amplitude = 1.0;
	double maxAmplitude = 0.0;

	for (int i = 0; i < _noiseOctaves; i++)
	{
		result += noise->noise(x * frequency, y * frequency, z * frequency) * amplitude;
		maxAmplitude += amplitude;

		frequency *= _noiseLacunarity;
		amplitude *= _noisePersistence;
	}

	result /= maxAmplitude;
	double d = pow(abs(result), _noiseExponent) * (abs(result) / result);
	return d;
}

double RidgeGenerator::getUnscaledHeight(SimplexNoise* noise, double x, double y, double z)
{
	double result = 0.0;
	double frequency = _noiseScale;
	double amplitude = 1.0;
	double maxAmplitude = 0.0;

	for (int i = 0; i < _noiseOctaves; i++)
	{
		result += (1.0 - abs(noise->noise(x * frequency, y * frequency, z * frequency))) * amplitude;
		maxAmplitude += amplitude;

		frequency *= _noiseLacunarity;
		amplitude *= _noisePersistence;
	}

	result /= maxAmplitude;
	result = pow(result, _noiseGain);

	return result;
}

double TerrainGenerator::getScaledHeight(SimplexNoise* noise, double minHeight, double maxHeight, double x, double y, double z)
{
	double scaledHeight = getUnscaledHeight(noise, x, y, z) * (maxHeight - minHeight) * 0.5 + (maxHeight + minHeight) * 0.5;
	return glm::clamp(scaledHeight, _minBound, _maxBound);
}

double TerrainGenerator::getHeight(SimplexNoise* noise, double& minHeight, double& maxHeight, double x, double y, double z)
{
	minHeight += _minHeight;
	maxHeight += _maxHeight;

	double result = getScaledHeight(noise, _minHeight, _maxHeight, x, y, z);

	for (int i = 0; i < _terrainGeierators.size(); i++)
	{
		TerrainGenerator* terrainGenerator = _terrainGeierators[i];

		double d = terrainGenerator->getHeight(noise, minHeight, maxHeight, x, y, z);

		if (terrainGenerator->getFunction() == ADD) result += d;
		else if (terrainGenerator->getFunction() == SUB) result -= d;
		else if (terrainGenerator->getFunction() == MUL) result *= d;
	}

	return result;
}

double TerrainGenerator::getHeight(double x, double y, double z)
{
	double minHeight, maxHeight;
	return getHeight(_noise, minHeight, maxHeight, x, y, z);
}

Function TerrainGenerator::getFunction()
{
	return _function;
}

double TerrainGenerator::getMaxHeight() const
{
	return _maxHeight;
}

double TerrainGenerator::getMinHeight() const
{
	return _minHeight;
}

double TerrainGenerator::getMaxBound() const
{
	return _maxBound;
}

double TerrainGenerator::getMinBound() const
{
	return _minBound;
}

double TerrainGenerator::getNoiseScale() const
{
	return _noiseScale;
}

double TerrainGenerator::getNoisePersistence() const
{
	return _noisePersistence;
}

double TerrainGenerator::getNoiseExponent() const
{
	return _noiseExponent;
}

int TerrainGenerator::getNoiseOctaves() const
{
	return _noiseOctaves;
}

TerrainGenerator* TerrainGenerator::setHeightBounds(double maxHeight, double minHeight)
{
	_maxHeight = std::max(maxHeight, minHeight);
	_minHeight = std::min(maxHeight, minHeight);
	return this;
}

TerrainGenerator* TerrainGenerator::setClampBounds(double maxBound, double minBound)
{
	_maxBound = std::max(maxBound, minBound);
	_minBound = std::min(maxBound, minBound);
	return this;
}

TerrainGenerator* TerrainGenerator::setNoiseScale(double noiseScale)
{
	_noiseScale = noiseScale;
	return this;
}

TerrainGenerator* TerrainGenerator::setNoisePersistence(double noisePersistence)
{
	_noisePersistence = noisePersistence;
	return this;
}

TerrainGenerator* TerrainGenerator::setNoiseExponent(double noiseExponent)
{
	_noiseExponent = noiseExponent;
	return this;
}

TerrainGenerator* TerrainGenerator::setNoiseOctaves(int noiseOctaves)
{
	_noiseOctaves = noiseOctaves;
	return this;
}

TerrainGenerator* TerrainGenerator::addColourGradient(double height, glm::vec3 colour)
{
	_colourGradient[height] = colour;
	return this;
}

TerrainGenerator* TerrainGenerator::addTerrainGenerator(TerrainGenerator* terrainGenerator)
{
	if (terrainGenerator != nullptr)
	{
		_terrainGeierators.push_back(terrainGenerator);
	}
	return this;
}

glm::vec3 TerrainGenerator::getColour(double height)
{
	if (_colourGradient.size() > 0)
	{
		auto it = _colourGradient.begin();

		double d0 = (*it).first;
		glm::vec3 v0 = (*it).second;

		while (it != _colourGradient.end())
		{
			double d1 = (*it).first;
			glm::vec3 v1 = (*it).second;

			if (d0 <= height && height <= d1)
			{
				float delta = (height - d0) / (d1 - d0);
				return v0 * (1.0F - delta) + v1 * (0.0F + delta);
			}
			d0 = d1;
			v0 = v1;
			++it;
		}

		return v0;
	}
	return glm::vec3(1.0);
}
