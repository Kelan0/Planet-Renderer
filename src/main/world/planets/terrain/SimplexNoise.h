#pragma once

/**
 * I take no credit for the code in this class, I simply converted Java code to C++ code from this:
 * https://github.com/Renanse/Ardor3D/blob/master/ardor3d-math/src/main/java/com/ardor3d/math/functions/SimplexNoise.java
 */
class SimplexNoise
{
private:
	static const int grad3[][3];

	static const int grad4[][4];

	static const int simplex[][4];

	static double dot(const int g[], double x, double y);

	static double dot(const int g[], double x, double y, double z);

	static double dot(const int g[], double x, double y, double z, double w);

	// To remove the need for index wrapping, double the permutation table length
	int perm[512];
public:
	SimplexNoise();

	~SimplexNoise();

	void setSeed(int seed);

	double noise(double xin, double yin);

	double noise(double xin, double yin, double zin);

	double noise(double xin, double yin, double zin, double win);
};