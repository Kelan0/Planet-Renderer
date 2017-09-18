#pragma once
#include <glm/glm.hpp>
#include "../../renderer/Camera.h"

class BoundingSphere;
class OOBB;

class BoundingVolume
{
public:
	static BoundingSphere* calculateBoundingSphere(glm::vec3 minBounds, glm::vec3 maxBounds, glm::vec3 centre);
	static OOBB* calculateBoundingBox(glm::vec3 X, glm::dvec3 Y, glm::dvec3 Z, glm::dvec3 dimensions, glm::dvec3 centre);
};

class BoundingSphere
{
private:
	float _radius;
	glm::vec3 _centre;

public:
	BoundingSphere(glm::vec3 position, float radius);
	~BoundingSphere();

	float getRadius() const;

	void setRadius(float radius);

	glm::vec3 getPosition() const;

	void setPosition(glm::vec3 position);

	int checkFrustum(Camera* camera, glm::vec3 position);

	void drawVolume(ShaderProgram* shaderProgram, glm::vec3 position);
};

class OOBB
{
private:
	glm::dvec3 _X;
	glm::dvec3 _Y;
	glm::dvec3 _Z;
	glm::dvec3 _centre;
	glm::dvec3* _corners;

public:
	OOBB(const glm::dvec3& x, const glm::dvec3& y, const glm::dvec3& z, const glm::dvec3& centre): _X(x), _Y(y), _Z(z), _centre(centre)
	{
		calculateCorners();
	}

	~OOBB();

	bool checkFrustum(Camera* camera);

	void renderBoundingBox();

	void calculateCorners();

	void setAxis(glm::dvec3 x, glm::dvec3 y, glm::dvec3 z, bool updateCorners = true);

	void setDimensions(glm::dvec3 dimensions, bool updateCorners = true);

	void setCentre(glm::dvec3 centre);

	glm::dvec3* getCorners() const;

	glm::dvec3 getX() const;
	
	glm::dvec3 getY() const;
	
	glm::dvec3 getZ() const;

	glm::dvec3 getCentre() const;
};
