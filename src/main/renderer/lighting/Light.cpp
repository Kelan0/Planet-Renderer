#include "Light.h"
#include <strstream>
#include <string>

Light::~Light()
{
}

glm::vec3 Light::getPosition() const
{
	return _position;
}

void Light::setPosition(const glm::vec3& position)
{
	this->_position = position;
}

glm::vec3 Light::getColour() const
{
	return _colour;
}

void Light::setColour(const glm::vec3& colour)
{
	this->_colour = colour;
}

glm::vec3 Light::getAttenuation() const
{
	return _attenuation;
}

void Light::setAttenuation(const glm::vec3& attenuation)
{
	this->_attenuation = attenuation;
}

glm::vec3 Light::getDirection() const
{
	return _direction;
}

void Light::setDirection(const glm::vec3& direction)
{
	this->_direction = direction;
}

float Light::getInnerCone() const
{
	return _innerCone;
}

void Light::setInnerCone(float innerCone)
{
	_innerCone = innerCone;
}

float Light::getOuterCone() const
{
	return _outerCone;
}

void Light::setOuterCone(float outerCone)
{
	_outerCone = outerCone;
}

LightType Light::getType() const
{
	return _type;
}

void Light::setType(LightType light)
{
	_type = light;
}

std::string Light::toString() const
{
	std::strstream str;
	str << "position: " << std::to_string(_position.x) << ", " << std::to_string(_position.y) << ", " << std::to_string(_position.z) << std::endl;			
	str << "colour: " << std::to_string(_colour.x) << ", " << std::to_string(_colour.y) << ", " << std::to_string(_colour.z) << std::endl;					
	str << "attenuation: " << std::to_string(_attenuation.x) << ", " << std::to_string(_attenuation.y) << ", " << std::to_string(_attenuation.z) << std::endl;
	str << "direction: " << std::to_string(_direction.x) << ", " << std::to_string(_direction.y) << ", " << std::to_string(_direction.z) << std::endl;
	str << "innerCone: " << std::to_string(_innerCone) << ", " << std::to_string(_innerCone) << ", " << std::to_string(_innerCone) << std::endl;
	str << "outerCone: " << std::to_string(_outerCone) << ", " << std::to_string(_outerCone) << ", " << std::to_string(_outerCone) << std::endl;

	return std::string(str.str());
}

Light* Light::createBlankLight()
{
	return new Light(glm::vec3(), glm::vec3(1.0000F, 0.6666F, 0.3333F), glm::vec3());
}
