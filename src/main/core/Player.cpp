#include "Player.h"
#include "../VoxelGame.h"
#include "../world/planets/Planet.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <SDL.h>
#include "../InputHandler.h"
#include "../Logger.h"

Player::~Player()
{
}

void Player::init()
{
//	GameObject::init();
	flying = true;
}

void Player::physics(double delta, GameObject* gameObject)
{
//	//this is a fix for a glitch that caused the players worldPosition to become infinite, when the movement was being updated.
//	_lock.lock();
//	glm::dvec3 toMove = this->toMove;
//	_lock.unlock();
}

void Player::render(double delta, GameObject* gameObject, ShaderProgram* shaderProgram)
{
//	GameObject::render(delta, gameObject, shaderProgram);
	glm::dvec3 planetPos = currentPlanet->getAbsoluteTransformation(false).getPosition();
	double playerHeight = getHeight();

//	d = 1.0 - glm::clamp((playerAltitude - min) / (max - min), 0.0, 1.0);

	Planet* closestPlanet = nullptr;
	double closestDistance = DBL_MAX;

	for (auto it: VoxelGame::planets)
	{
		Planet* planet = it.second;
		if (planet != nullptr)
		{
			double distance = planet->getDistance(worldPosition, false);

			if (distance < closestDistance)
			{
				closestPlanet = planet;
				closestDistance = distance;
			}
		}
	}
	currentPlanet = closestPlanet;

	double base = 3.0;
	double logDist = log10(std::max(playerHeight, 0.000001)) / log10(base);
	double scale = glm::clamp(std::pow(base, base - logDist), VoxelGame::minScale, VoxelGame::maxScale);

	if (camera->_updateFrustum)
	{
		VoxelGame::scale = scale;
	}

	glm::dvec3 instantVelocity = velocity;
	if (!flying)
	{
		applyGravity(instantVelocity);
		timeFlying = 0.0;
	}
	else
	{
		timeFlying += delta;
		timeAirborn = 0.0;
	}
	if (!isnan(glm::dot(toMove, toMove)) && (flying || timeAirborn <= 0.1))
	{
		instantVelocity += toMove * moveSpeed * delta;
	}

	glm::dvec3 velocityScaled = instantVelocity / scale;
	glm::dvec3 player = worldPosition + velocityScaled;
	glm::dvec3 planetToPlayer = glm::normalize(player - planetPos);

	double terrainHeight = currentPlanet != nullptr ? currentPlanet->getRadius() + currentPlanet->getRadius() * currentPlanet->getTerrainHeight(planetToPlayer) + 0.002 : 0.0;
	double distance = glm::distance(planetPos, player);

	timeAirborn += delta;
	if (distance < terrainHeight)
	{
		timeAirborn = 0.0;
		velocityScaled += (planetToPlayer * abs(distance - terrainHeight));
	}

	worldPosition += velocityScaled;

	camera->setPosition(getHeadPosition());
	camera->render(delta, gameObject, shaderProgram);
}

void Player::input(double delta, GameObject* gameObject)
{
	if (VoxelGame::inputHandler.keyPressed(SDL_SCANCODE_F))
	{
		camera->_updateFrustum = !camera->_updateFrustum;
	}

	glm::dvec3 right = camera->getRight();
	glm::dvec3 up = camera->getUp();
	glm::dvec3 forward = camera->getForward();// glm::cross(glm::vec3(0.0F, 1.0F, 0.0F), right);
	double roll = 0.0F;

	glm::dvec3 toMove;

	if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_W))
	{
		toMove += forward;
	} if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_A))
	{
		toMove -= right;
	} if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_S))
	{
		toMove -= forward;
	} if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_D))
	{
		toMove += right;
	} if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_Q))
	{
		roll += moveSpeed * delta * 15.0F;
	} if (VoxelGame::inputHandler.keyDown(SDL_SCANCODE_E))
	{
		roll -= moveSpeed * delta * 15.0F;
	} if (VoxelGame::inputHandler.keyPressed(SDL_SCANCODE_F3))
	{
		flying = !flying;
	}

	glm::vec2 mouseVel = glm::vec2(VoxelGame::inputHandler.getMouseVelocity()) * float(mouseSpeed * delta);

	camera->rotate(-mouseVel.y, right);
	camera->rotate(-mouseVel.x, up);
	camera->rotate(-roll, forward);

	_lock.lock();
	this->toMove = glm::normalize(toMove);
	_lock.unlock();
}

void Player::handleCollisions()
{
}

void Player::setCurrentBody(Planet* body)
{
	currentPlanet = body;
}

void Player::applyGravity(glm::dvec3& velocity)
{
	if (currentPlanet != nullptr)
	{
		glm::dvec3 planetToPlayer = ((this->getFeetPosition(true) - currentPlanet->getAbsoluteTransformation(true).getPosition()) / VoxelGame::scale) * 1000.0;
		double distanceSquared = glm::dot(planetToPlayer, planetToPlayer);

		glm::dvec3 direction = planetToPlayer / sqrt(distanceSquared);
		double G = 6.671281903963040991511534289E-11;
		double mass = currentPlanet->getMass();
		double gravity = ((G * mass) / distanceSquared);
		velocity -= 0.001 * gravity * direction * timeAirborn * VoxelGame::minScale; // divide by delta time ??
		std::cout << gravity << ", " << timeAirborn << "s, " << glm::length(velocity) << "km/s\n";
	}
}

glm::dvec3 Player::getHeadPosition(bool doScale)
{
	if (doScale)
	{
		return worldPosition * VoxelGame::scale;// +glm::vec3(0.0F, PLAYER_HEIGHT * VoxelGame::scale, 0.0F);
	} else
	{
		return worldPosition;
	}
}

glm::dvec3 Player::getFeetPosition(bool doScale)
{
	if (doScale)
	{
		return worldPosition * VoxelGame::scale;
	} else
	{
		return worldPosition;
	}
}

Camera* Player::getCamera() const
{
	return camera;
}

Planet* Player::getCurrentBody() const
{
	return currentPlanet;
}

double Player::getAltitude() const
{
	if (currentPlanet != nullptr)
	{
		glm::dvec3 player = VoxelGame::player->getHeadPosition();
		glm::dvec3 planet = currentPlanet->getAbsoluteTransformation().position;
		double radius = currentPlanet->getRadius();

		double distance = glm::length(player - planet);

		return (distance / (radius * VoxelGame::scale) - 1.0F) * radius;
	}

	return NAN;
}

double Player::getHeight() const
{
	glm::dvec3 player = VoxelGame::player->getHeadPosition();
	glm::dvec3 planet = currentPlanet->getAbsoluteTransformation().position;
	double height = currentPlanet->getTerrainHeight(glm::normalize(player - planet)) * currentPlanet->getRadius();

	return getAltitude() - height;
}

int Player::getLODFromDistance(glm::dvec3 position) const
{
	float distance = glm::distance(position, this->worldPosition);

	int maxLOD = 24;
	int minLOD = 0;
	float base = 3.0F;

	float log = std::log10(distance) / log10(base);

	return std::max(std::min((int)log, maxLOD), minLOD);
}
