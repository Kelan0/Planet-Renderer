#pragma once
#include "SimplexNoise.h"
#include <glm/detail/type_vec3.hpp>
#include <map>
#include <vector>

enum Function
{
	ADD,
	SUB,
	MUL
};

class TerrainGenerator
{
protected:
	Function _function;
	SimplexNoise* _noise;
	double _maxHeight;
	double _minHeight;
	double _maxBound;
	double _minBound;
	double _noiseScale;
	double _noisePersistence;
	double _noiseLacunarity;
	double _noiseExponent;
	int _noiseOctaves;
	std::map<double, glm::vec3> _colourGradient;
	std::vector<TerrainGenerator*> _terrainGeierators;

	public:
	TerrainGenerator(double maxHeight, double minHeight, double noiseScale, double noisePersistence, double noiseLacunarity, int noiseOctaves, int seed, Function function);
	virtual ~TerrainGenerator();

	virtual double getUnscaledHeight(SimplexNoise* noise, double x, double y, double z);

	virtual double getScaledHeight(SimplexNoise* noise, double minHeight, double maxHeight, double x, double y, double z);

	virtual double getHeight(SimplexNoise* noise, double& minHeight, double& maxHeight, double x, double y, double z);

	virtual double getHeight(double x, double y, double z);

	Function getFunction();

	double getMaxHeight() const;

	double getMinHeight() const;

	double getMaxBound() const;

	double getMinBound() const;

	double getNoiseScale() const;

	double getNoisePersistence() const;

	double getNoiseExponent() const;

	int getNoiseOctaves() const;

	TerrainGenerator* setHeightBounds(double maxHeight, double minHeight);

	TerrainGenerator* setClampBounds(double maxBound, double minBound);

	TerrainGenerator* setNoiseScale(double noiseScale);

	TerrainGenerator* setNoisePersistence(double noisePersistence);

	TerrainGenerator* setNoiseExponent(double noiseExponent);

	TerrainGenerator* setNoiseOctaves(int noiseOctaves);

	TerrainGenerator* addColourGradient(double height, glm::vec3 colour);

	TerrainGenerator* addTerrainGenerator(TerrainGenerator* terrainGenerator);

	glm::vec3 getColour(double height);
};

class RidgeGenerator : public TerrainGenerator
{
protected:
	double _noiseGain;
public:
	RidgeGenerator(double maxHeight, double minHeight, double noiseScale, double noisePersistence, double noiseLacunarity, double noiseGain, int noiseOctaves, int seed, Function function) : TerrainGenerator(maxHeight, minHeight, noiseScale, noisePersistence, noiseLacunarity, noiseOctaves, seed, function), _noiseGain(noiseGain) {}

	double getUnscaledHeight(SimplexNoise* noise, double x, double y, double z) override;
};
