#include "BoundingVolume.h"
#include "../../VoxelGame.h"
#include <iostream>

OOBB::~OOBB()
{

}

bool OOBB::checkFrustum(Camera* camera)
{
	for (int i = 0; i < 8; i++)
	{
		if (camera->pointInFrustum(glm::vec3(_corners[i] + _centre)))
		{
			return true;
		}
	}

	return false;
}

void OOBB::renderBoundingBox()
{
	glPointSize(6);
	for (int i = 0; i < 8; i++)
	{
		glm::dvec3 p = (_corners[i] + _centre) * VoxelGame::scale;

		glVertexAttrib4f(3, 1.0, 0.0, 0.0, 1.0);
		glVertexAttrib2f(2, 0.0, 0.0);
		glVertexAttrib3f(1, 0.0, 0.0, 0.0);
		glVertexAttrib3f(0, p.x, p.y, p.z);
//		std::cout << "[" << p.x << ", " << p.y << ", " << p.z << "] ";
	}
	glDrawArrays(GL_POINTS, 0, 8);
	glPointSize(1);
}

void OOBB::calculateCorners()
{
	double d = 0.5;
	glm::dvec3 bottomCentre = -d * _Y;
	glm::dvec3 topCentre = d * _Y;

	_corners = new glm::dvec3[8];
	_corners[0] = bottomCentre + d * _X + d * _Z; //tl
	_corners[1] = bottomCentre - d * _X + d * _Z; //tr
	_corners[2] = bottomCentre - d * _X - d * _Z; //br
	_corners[3] = bottomCentre + d * _X - d * _Z; //bl
	_corners[4] = topCentre + d * _X + d * _Z;
	_corners[5] = topCentre - d * _X + d * _Z;
	_corners[6] = topCentre - d * _X - d * _Z;
	_corners[7] = topCentre + d * _X - d * _Z;
}

void OOBB::setAxis(glm::dvec3 x, glm::dvec3 y, glm::dvec3 z, bool updateCorners)
{
	_X = x;
	_Y = y;
	_Z = z;

	if (updateCorners)
	{
		calculateCorners();
	}
}

void OOBB::setDimensions(glm::dvec3 dimensions, bool updateCorners)
{
	_X = glm::normalize(_X) * dimensions.x;
	_Y = glm::normalize(_Y) * dimensions.y;
	_Z = glm::normalize(_Z) * dimensions.z;
	if (updateCorners)
	{
		calculateCorners();
	}
}

void OOBB::setCentre(glm::dvec3 centre)
{
	_centre = centre;
}

glm::dvec3* OOBB::getCorners() const
{
	return _corners;
}

glm::dvec3 OOBB::getX() const
{
	return _X;
}

glm::dvec3 OOBB::getY() const
{
	return _Y;
}

glm::dvec3 OOBB::getZ() const
{
	return _Z;
}

glm::dvec3 OOBB::getCentre() const
{
	return _centre;
}
