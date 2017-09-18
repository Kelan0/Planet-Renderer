#pragma once
#include "Shader.h"
#include <map>
#include <glm/glm.hpp>
#include "lighting/Light.h"
#include <vector>
#include <unordered_map>
#include "../resources/ResourceManager.h"

struct Uniform
{
	GLuint id;
	std::string name;
	void* value;
};

class ShaderProgram
{
private:
	std::map<GLuint, Shader*> _shaders;
	std::unordered_map<std::string, GLint> _uniforms;
	std::unordered_map<std::string, GLint> _attributes;
	std::vector<Light*> lights;
	GLuint _programID;
	bool _completed;

public:
	ShaderProgram();
	~ShaderProgram();

	void addShader(Shader* shader);

	void addShader(GLuint type, const char* file);

	void addAttribute(GLint location, char* attribute);

	void completeProgram();

	void addLight(Light* light);

	void renderLights();

	void useProgram(bool use) const;

	GLuint getProgramID() const;

	bool isComplete() const;

	void setUniform(std::string uniform, float f);

	void setUniform(std::string uniform, float f, float f1);

	void setUniform(std::string uniform, float f, float f1, float f2);

	void setUniform(std::string uniform, float f, float f1, float f2, float f3);

	void setUniform(std::string uniform, double d);

	void setUniform(std::string uniform, double d, double d1);

	void setUniform(std::string uniform, double d, double d1, double d2);

	void setUniform(std::string uniform, double d, double d1, double d2, double d3);

	void setUniform(std::string uniform, int i);

	void setUniform(std::string uniform, int i, int i1);

	void setUniform(std::string uniform, int i, int i1, int i2);

	void setUniform(std::string uniform, int i, int i1, int i2, int i3);

	void setUniform(std::string uniform, bool b);

	void setUniform(std::string uniform, glm::vec2 v);

	void setUniform(std::string uniform, glm::vec3 v);

	void setUniform(std::string uniform, glm::vec4 v);

	void setUniform(std::string uniform, glm::mat2x2 m);

	void setUniform(std::string uniform, glm::mat2x3 m);

	void setUniform(std::string uniform, glm::mat2x4 m);

	void setUniform(std::string uniform, glm::mat3x2 m);

	void setUniform(std::string uniform, glm::mat3x3 m);

	void setUniform(std::string uniform, glm::mat3x4 m);

	void setUniform(std::string uniform, glm::mat4x2 m);

	void setUniform(std::string uniform, glm::mat4x3 m);

	void setUniform(std::string uniform, glm::mat4x4 m);

	void setUniform(std::string uniform, Light* light);

	void* getUniformValue(std::string uniform);

	GLint getUniform(std::string uniform);
};

