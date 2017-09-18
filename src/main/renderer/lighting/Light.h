#pragma once
#include <glm/glm.hpp>
#include <string>

enum LightType
{
	POINT_LIGHT = 0,
	SPOT_LIGHT = 1,
	DIRECTIONAL_LIGHT = 2
};

class Light
{
private:
	glm::vec3 _position;
	glm::vec3 _colour;
	glm::vec3 _attenuation;
	glm::vec3 _direction;
	float _innerCone;
	float _outerCone;

	LightType _type;
public:
	//SPOT LIGHT
	Light(glm::vec3 position, glm::vec3 colour, glm::vec3 attenuation, glm::vec3 direction, float innerCone, float outerCone):
		_position(position), _colour(colour), _attenuation(attenuation), _direction(direction), _innerCone(innerCone), _outerCone(outerCone), _type(SPOT_LIGHT) {}

	//POINT LLIGHT
	Light(glm::vec3 position, glm::vec3 colour, glm::vec3 attenuation):
		_position(position), _colour(colour), _attenuation(attenuation), _direction(), _innerCone(0.0F), _outerCone(0.0F), _type(POINT_LIGHT) {}

	//DIRECTIONAL LIGHT
	Light(glm::vec3 colour, glm::vec3 direction):
		_position(), _colour(colour), _attenuation(1.0F), _direction(), _innerCone(0.0F), _outerCone(0.0F), _type(DIRECTIONAL_LIGHT) {}

	~Light();

	glm::vec3 getPosition() const;

	void setPosition(const glm::vec3& position);

	glm::vec3 getColour() const;

	void setColour(const glm::vec3& colour);
	
	glm::vec3 getAttenuation() const;
	
	void setAttenuation(const glm::vec3& attenuation);
	
	glm::vec3 getDirection() const;
	
	void setDirection(const glm::vec3& direction);
	
	float getInnerCone() const;
	
	void setInnerCone(float innerCone);
	
	float getOuterCone() const;
	
	void setOuterCone(float outerCone);

	LightType getType() const;

	void setType(LightType light);

	std::string toString() const;

	static Light* createBlankLight();
};


