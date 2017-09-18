#pragma once
#include <glm/glm.hpp>
#include "../renderer/Camera.h"
#include <mutex>

#define PLAYER_HEIGHT 0.0F

class Planet;

class Player// : public GameObject
{
private:
	glm::dvec3 velocity;
//	glm::dvec3 planetPosition;
	glm::dvec3 worldPosition;
	glm::dvec3 toMove;

	double moveSpeed;
	double mouseSpeed;
	double timeAirborn;
	double timeFlying;
	double d;
	bool jumping;
	bool flying;

	Camera* camera;
	Planet* currentPlanet;

	std::mutex _lock;

public:
	Player(glm::dvec3 position): worldPosition(position), moveSpeed(75.0F), mouseSpeed(0.2F), currentPlanet(nullptr)
	{
		camera = new Camera(position + glm::dvec3(0.0, PLAYER_HEIGHT, 0.0));
	}

	~Player();

	void init();

	void physics(double delta, GameObject* gameObject);

	void render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram);

	void input(double delta, GameObject* gameObject);

	void handleCollisions();

	void setCurrentBody(Planet* body);

	void applyGravity(glm::dvec3& velocity);

	glm::dvec3 getHeadPosition(bool doScale = true);

	glm::dvec3 getFeetPosition(bool doScale = true);

	Camera* getCamera() const;

	Planet* getCurrentBody() const;

	double getAltitude() const;

	double getHeight() const;

	int getLODFromDistance(glm::dvec3 position) const;
};

