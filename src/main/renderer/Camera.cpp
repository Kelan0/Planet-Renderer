#include "Camera.h"

#include <glm/gtx/rotate_vector.hpp>
#include "ShaderProgram.h"
#include <glm/gtc/type_ptr.inl>
#include "RenderingHandler.h"
#include "../VoxelGame.h"
#include "font/FontRenderer.h"
#include <SDL.h>
#include "../InputHandler.h"
#include <iostream>

Camera::~Camera()
{
}

void Camera::init(GameObject* gameObject)
{

}

void Camera::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
	glm::dvec3 pos;
	_viewMatrix = glm::lookAt(pos, pos + _forward, _up);

	if (_updateFrustum)
	{
		_frustumPosition = _position;
		float* proj = &(VoxelGame::renderingHandler->getProjectionMatrix())[0].x;
		float* modl = &(glm::mat4x4(_viewMatrix))[0].x;

		float clip[16];

		clip[ 0] = modl[ 0] * proj[0] + modl[ 1] * proj[4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
		clip[ 1] = modl[ 0] * proj[1] + modl[ 1] * proj[5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
		clip[ 2] = modl[ 0] * proj[2] + modl[ 1] * proj[6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
		clip[ 3] = modl[ 0] * proj[3] + modl[ 1] * proj[7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];
		clip[ 4] = modl[ 4] * proj[0] + modl[ 5] * proj[4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
		clip[ 5] = modl[ 4] * proj[1] + modl[ 5] * proj[5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
		clip[ 6] = modl[ 4] * proj[2] + modl[ 5] * proj[6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
		clip[ 7] = modl[ 4] * proj[3] + modl[ 5] * proj[7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];
		clip[ 8] = modl[ 8] * proj[0] + modl[ 9] * proj[4] + modl[10] * proj[ 8] + modl[11] * proj[12];
		clip[ 9] = modl[ 8] * proj[1] + modl[ 9] * proj[5] + modl[10] * proj[ 9] + modl[11] * proj[13];
		clip[10] = modl[ 8] * proj[2] + modl[ 9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[ 8] * proj[3] + modl[ 9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];
		clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[ 8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[ 9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

		float t;

		/* Extract the numbers for the RIGHT plane */
		frustum[0][0] = clip[3] - clip[0];
		frustum[0][1] = clip[7] - clip[4];
		frustum[0][2] = clip[11] - clip[8];
		frustum[0][3] = clip[15] - clip[12];

		/* Normalize the result */
		t = sqrt(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
		frustum[0][0] /= t;
		frustum[0][1] /= t;
		frustum[0][2] /= t;
		frustum[0][3] /= t;

		/* Extract the numbers for the LEFT plane */
		frustum[1][0] = clip[3] + clip[0];
		frustum[1][1] = clip[7] + clip[4];
		frustum[1][2] = clip[11] + clip[8];
		frustum[1][3] = clip[15] + clip[12];

		/* Normalize the result */
		t = sqrt(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
		frustum[1][0] /= t;
		frustum[1][1] /= t;
		frustum[1][2] /= t;
		frustum[1][3] /= t;

		/* Extract the BOTTOM plane */
		frustum[2][0] = clip[3] + clip[1];
		frustum[2][1] = clip[7] + clip[5];
		frustum[2][2] = clip[11] + clip[9];
		frustum[2][3] = clip[15] + clip[13];

		/* Normalize the result */
		t = sqrt(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
		frustum[2][0] /= t;
		frustum[2][1] /= t;
		frustum[2][2] /= t;
		frustum[2][3] /= t;

		/* Extract the TOP plane */
		frustum[3][0] = clip[3] - clip[1];
		frustum[3][1] = clip[7] - clip[5];
		frustum[3][2] = clip[11] - clip[9];
		frustum[3][3] = clip[15] - clip[13];

		/* Normalize the result */
		t = sqrt(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
		frustum[3][0] /= t;
		frustum[3][1] /= t;
		frustum[3][2] /= t;
		frustum[3][3] /= t;

		/* Extract the FAR plane */
		frustum[4][0] = clip[3] - clip[2];
		frustum[4][1] = clip[7] - clip[6];
		frustum[4][2] = clip[11] - clip[10];
		frustum[4][3] = clip[15] - clip[14];

		/* Normalize the result */
		t = sqrt(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
		frustum[4][0] /= t;
		frustum[4][1] /= t;
		frustum[4][2] /= t;
		frustum[4][3] /= t;

		/* Extract the NEAR plane */
		frustum[5][0] = clip[3] + clip[2];
		frustum[5][1] = clip[7] + clip[6];
		frustum[5][2] = clip[11] + clip[10];
		frustum[5][3] = clip[15] + clip[14];

		/* Normalize the result */
		t = sqrt(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
		frustum[5][0] /= t;
		frustum[5][1] /= t;
		frustum[5][2] /= t;
		frustum[5][3] /= t;
	}
}

void Camera::physics(double delta, GameObject* gameObject)
{

}

void Camera::input(double delta, GameObject* gameObject)
{

}

void Camera::translate(glm::dvec3 direction)
{
	_position += direction;
}

void Camera::rotate(double angle, glm::dvec3 axis)
{
	_forward = glm::rotate(_forward, angle, axis);
	_up = glm::rotate(_up, angle, axis);
}

void Camera::setPosition(glm::dvec3 position)
{
	_position = position;
}

void Camera::setRotation(glm::dvec3 eulerRotation)
{
	glm::dvec3 forward = _forward;
	glm::dvec3 up = _up;

	_forward = glm::rotate(_forward, eulerRotation.y, up);
	_forward = glm::rotate(_forward, eulerRotation.x, getRight());
	_up = glm::rotate(_up, eulerRotation.z, forward);
	_up = glm::rotate(_up, eulerRotation.x, getRight());
}

glm::dvec3 Camera::getPosition() const
{
	return _position;
}

glm::dvec3 Camera::getForward() const
{
	return _forward;
}

glm::dvec3 Camera::getUp() const
{
	return _up;
}

glm::dvec3 Camera::getRight() const
{
	return glm::cross(_forward, _up);
}

glm::dmat4x4 Camera::getViewMatrix() const
{
	return _viewMatrix;
}

double Camera::getPitch() const
{
	return _eulerRotations.x;
}

double Camera::getYaw() const
{
	return _eulerRotations.y;
}

double Camera::getRoll() const
{
	return _eulerRotations.z;
}

bool Camera::pointInFrustum(glm::vec3 point)
{
	point *= VoxelGame::scale;
	point -= _frustumPosition;
	
	for (int i = 0; i < 6; i++)
	{
		if (frustum[i][0] * point.x + frustum[i][1] * point.y + frustum[i][2] * point.z + frustum[i][3] <= 0)
		{
			return false;
		}
	}
	return true;
}

int Camera::sphereInFrustum(glm::vec3 position, float radius)
{
	position *= VoxelGame::scale;
	position -= _frustumPosition;

	int c = 0;
	float d;

	for (int i = 0; i < 6; i++)
	{
		d = frustum[i][0] * position.x + frustum[i][1] * position.y + frustum[i][2] * position.z + frustum[i][3];
		
		if (d <= -radius)
		{
			return OUTSIDE_FRUSTUM;
		}
		
		if (d > radius)
		{
			c++;
		}
	}
	return (c == 6) ? INSIDE_FRUSTUM : INTERSECT_FRUSTUM;
}

int Camera::cubeInFrustum(glm::vec3 position, float sideLength)
{
	position -= _frustumPosition;
	int c;
	int c2 = 0;

	for (int i = 0; i < 6; i++)
	{
		c = 0;
		if (frustum[i][0] * (position.x - sideLength) + frustum[i][1] * (position.y - sideLength) + frustum[i][2] * (position.z - sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x + sideLength) + frustum[i][1] * (position.y - sideLength) + frustum[i][2] * (position.z - sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x - sideLength) + frustum[i][1] * (position.y + sideLength) + frustum[i][2] * (position.z - sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x + sideLength) + frustum[i][1] * (position.y + sideLength) + frustum[i][2] * (position.z - sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x - sideLength) + frustum[i][1] * (position.y - sideLength) + frustum[i][2] * (position.z + sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x + sideLength) + frustum[i][1] * (position.y - sideLength) + frustum[i][2] * (position.z + sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x - sideLength) + frustum[i][1] * (position.y + sideLength) + frustum[i][2] * (position.z + sideLength) + frustum[i][3] > 0)
		{
			c++;
		}
		if (frustum[i][0] * (position.x + sideLength) + frustum[i][1] * (position.y + sideLength) + frustum[i][2] * (position.z + sideLength) + frustum[i][3] > 0)
		{
			c++;
		}

		if (c == 0)
		{
			return OUTSIDE_FRUSTUM;
		}
		if (c == 8)
		{
			c2++;
		}
	}
	return (c2 == 6) ? INSIDE_FRUSTUM : INTERSECT_FRUSTUM;
}

int Camera::meshInFrustum(std::vector<glm::vec3> vertices)
{
	int i = 0;
	for (auto it = vertices.begin(); it != vertices.end(); ++it)
	{
		glm::vec3 vertex = *it;

		if (pointInFrustum(vertex))
		{
			i++;
		} else if (i > 0)
		{
			return INTERSECT_FRUSTUM;
		}
	}

	return (i == vertices.size()) ? INSIDE_FRUSTUM : OUTSIDE_FRUSTUM;
}
