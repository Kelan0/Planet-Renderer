#pragma once
#include <glm/glm.hpp>
#include "../world/sceneGraph/components/MeshComponent.h"

#define OUTSIDE_FRUSTUM 0
#define INTERSECT_FRUSTUM 1
#define INSIDE_FRUSTUM 2

class ShaderProgram;

class Camera : public GameComponent
{
private:
	glm::dvec3 _position;
	glm::dvec3 _frustumPosition;
	glm::dvec3 _eulerRotations;

	glm::dvec3 _forward;
	glm::dvec3 _up;
	glm::dmat4x4 _viewMatrix;
	float frustum[6][4];

public:
	bool _updateFrustum;

	Camera(glm::dvec3 position) : _position(position), _forward(0.0, 0.0, -1.0), _up(0.0, 1.0, 0.0), _updateFrustum(true)
	{
	}

	~Camera();

	void init(GameObject* gameObject) override;

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram) override;

	void physics(double delta, GameObject* gameObject) override;
	
	void input(double delta, GameObject* gameObject) override;

	void translate(glm::dvec3 direction);

	void rotate(double angle, glm::dvec3 axis);

	void setPosition(glm::dvec3 position);

	void setRotation(glm::dvec3 eulerRotation);

	glm::dvec3 getPosition() const;

	glm::dvec3  getForward() const;

	glm::dvec3  getUp() const;

	glm::dvec3 getRight() const;

	glm::dmat4x4 getViewMatrix() const;

	double getPitch() const;

	double getYaw() const;

	double getRoll() const;

	bool pointInFrustum(glm::vec3 point);

	int sphereInFrustum(glm::vec3 position, float radius);

	int cubeInFrustum(glm::vec3 position, float sideLength);

	int meshInFrustum(std::vector<glm::vec3> vertices);
};

