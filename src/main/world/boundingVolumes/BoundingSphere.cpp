#include "BoundingVolume.h"
#include <algorithm>
#include "../../VoxelGame.h"

BoundingSphere* BoundingVolume::calculateBoundingSphere(glm::vec3 minBounds, glm::vec3 maxBounds, glm::vec3 centre)
{
	float radius = std::max(glm::distance(minBounds, centre), glm::distance(maxBounds, centre));
	return new BoundingSphere(centre, radius);
}

BoundingSphere::BoundingSphere(glm::vec3 centre, float radius): _centre(centre), _radius(radius)
{
}

BoundingSphere::~BoundingSphere()
{
}

float BoundingSphere::getRadius() const
{
	return _radius;
}

void BoundingSphere::setRadius(float radius)
{
	_radius = radius;
}

glm::vec3 BoundingSphere::getPosition() const
{
	return _centre;
}

void BoundingSphere::setPosition(glm::vec3 position)
{
	_centre = position;
}

int BoundingSphere::checkFrustum(Camera* camera, glm::vec3 position)
{
	return camera->sphereInFrustum(_centre + position, _radius);
}

void BoundingSphere::drawVolume(ShaderProgram* shaderProgram, glm::vec3 position)
{

}
