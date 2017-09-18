#pragma once
#include <vector>
#include <glm/detail/type_vec3.hpp>
#include <map>
#include "SimplexNoise.h"
#include "../Chunk.h"
#include "TerrainGenerator.h"

class WorldGenerator
{
protected:
	int _octaves;
	double _frequency;
	double _persistence;
	double _lacunarity;
	double _amplitude;
	double _maxHeight;
	Function _function;
	std::map<double, glm::vec3> _colourGradient;
	std::vector<WorldGenerator*> _generators;
	SimplexNoise* _noise;

public:
	WorldGenerator(int octaves = 0, double amplitude = 1.0, double frequency = 1.0, double persistence = 0.5, double lacunarity = 2.0, Function function = ADD, int seed = 0);

	virtual ~WorldGenerator();

	void init();

	virtual double getHeight(double x, double y, double z);

	virtual double getHeight(SimplexNoise* noise, double x, double y, double z, int& counter);

	int getOctaves() const;

	double getFrequency() const;

	double getPersistence() const;

	double getLacunarity() const;

	double getAmplitude() const;

	double getMaxHeight() const;

	Function getFunction() const;

	glm::vec3 getColour(double height);

	WorldGenerator* setSeed(int seed);

	WorldGenerator* setOctaves(int octaves);
	
	WorldGenerator* setFrequency(double frequency);
	
	WorldGenerator* setPersistence(double persistence);
	
	WorldGenerator* setLacunarity(double lacunarity);

	WorldGenerator* setAmplitude(double amplitude);

	WorldGenerator* setMaxHeight(double maxHeight);

	WorldGenerator* setFunction(Function function);

	WorldGenerator* addTerrainGenerator(WorldGenerator* generator);

	WorldGenerator* addColourGradient(double height, glm::vec3 colour);
};

//class RidgeGenerator : public WorldGenerator
//{
//protected:
//	double _gain;
//public:
//	RidgeGenerator(int octaves = 0, double amplitude = 1.0, double frequency = 1.0, double persistence = 0.5, double lacunarity = 2.0, double gain = 2.0, Function function = ADD, int seed = 0) : WorldGenerator(octaves, amplitude, frequency, persistence, lacunarity, function, seed), _gain(gain) {}
//
//	double getHeight(SimplexNoise* noise, double x, double y, double z, int& count) override;
//};

