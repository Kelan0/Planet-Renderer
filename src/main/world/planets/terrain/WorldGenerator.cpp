#include "WorldGenerator.h"
#include <algorithm>
#include <iostream>

WorldGenerator::WorldGenerator(int octaves, double amplitude, double frequency, double persistence, double lacunarity, Function function, int seed) : _octaves(octaves), _frequency(frequency), _persistence(persistence), _lacunarity(lacunarity), _amplitude(amplitude), _function(function)
{
	_noise = new SimplexNoise();
	_noise->setSeed(seed);

	init();
}

WorldGenerator::~WorldGenerator()
{
}

void WorldGenerator::init()
{
//	_maxHeight = 0.0;
//	double height = _amplitude;
//	for (int i = 0; i < _octaves; i++)
//	{
//		_maxHeight += height;
//		height *= _persistence;
//	}
}

double WorldGenerator::getHeight(double x, double y, double z)
{
	int i = 0;
	double height = (getHeight(_noise, x, y, z, i) * 2.0 - 1.0) * getMaxHeight();

	return height;
}

double WorldGenerator::getHeight(SimplexNoise* noise, double x, double y, double z, int& counter)
{
	counter++;
	double result = 0.0;

	double amplitude = _amplitude;
	double frequency = _frequency;
	double maxAmplitude = 1.0;

	for (int i = 0; i < _octaves; i++)
	{
		result += (noise->noise(x * frequency, y * frequency, z * frequency) * 0.5 + 0.5) * amplitude;
		maxAmplitude += amplitude;
		frequency *= _lacunarity;
		amplitude *= _persistence;
	}

	result /= maxAmplitude;

	for (WorldGenerator* worldGenerator : _generators)
	{
		double n = worldGenerator->getHeight(noise, x, y, z, counter);
		switch (worldGenerator->getFunction())
		{
		case ADD: result += n;
		case SUB: result -= n;
		case MUL: result *= n;
		default: result += n;
		}
	}

	return result;
}

double RidgeGenerator::getHeight(SimplexNoise* noise, double x, double y, double z, int& counter)
{
	counter++;
	double result = 0.0;
	double maxValue = 1.0;
	double amplitude = _amplitude;
	double frequency = _frequency;

	for (int i = 0; i < _octaves; i++)
	{
		result += ((1.0 - abs(noise->noise(x * frequency, y * frequency, z * frequency)))) * amplitude;

		maxValue += amplitude;
		amplitude *= _persistence;
		frequency *= _lacunarity;
	}

	result /= maxValue;
	result = pow(result, _gain);

	for (WorldGenerator* worldGenerator : _generators)
	{
		double n = worldGenerator->getHeight(noise, x, y, z, counter);

		switch (worldGenerator->getFunction())
		{
		case ADD: result += n;
		case SUB: result -= n;
		case MUL: result *= n;
		default: result += n;
		}
	}

	return result;
}

int WorldGenerator::getOctaves() const
{
	return _octaves;
}

double WorldGenerator::getFrequency() const
{
	return _frequency;
}

double WorldGenerator::getPersistence() const
{
	return _persistence;
}

double WorldGenerator::getLacunarity() const
{
	return _lacunarity;
}

double WorldGenerator::getAmplitude() const
{
	return _amplitude;
}

double WorldGenerator::getMaxHeight() const
{
	return _maxHeight;
}

Function WorldGenerator::getFunction() const
{
	return _function;
}

glm::vec3 WorldGenerator::getColour(double height)
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

WorldGenerator* WorldGenerator::setSeed(int seed)
{
	_noise->setSeed(seed);
	return this;
}

WorldGenerator* WorldGenerator::setOctaves(int octaves)
{
	_octaves = octaves;
	init();
	return this;
}

WorldGenerator* WorldGenerator::setFrequency(double frequency)
{
	_frequency = frequency;
	return this;
}

WorldGenerator* WorldGenerator::setPersistence(double persistence)
{
	_persistence = persistence;
	init();
	return this;
}

WorldGenerator* WorldGenerator::setLacunarity(double lacunarity)
{
	_lacunarity = lacunarity;
	return this;
}

WorldGenerator* WorldGenerator::setAmplitude(double amplitude)
{
	_amplitude = amplitude;
	init();
	return this;
}

WorldGenerator* WorldGenerator::setMaxHeight(double maxHeight)
{
	_maxHeight = maxHeight;
	return this;
}

WorldGenerator* WorldGenerator::setFunction(Function function)
{
	_function = function;
	return this;
}

WorldGenerator* WorldGenerator::addTerrainGenerator(WorldGenerator* generator)
{
	if (this != nullptr && generator != nullptr && this != generator)
	{
		_generators.push_back(generator);
	}
	return this;
}

WorldGenerator* WorldGenerator::addColourGradient(double height, glm::vec3 colour)
{
	_colourGradient[height] = colour;
	return this;
}
