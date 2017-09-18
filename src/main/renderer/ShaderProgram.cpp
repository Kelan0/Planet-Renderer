#include "ShaderProgram.h"
#include "../VoxelGame.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Logger.h"
#include "../resources/ResourceManager.h"
#include <iostream>


ShaderProgram::ShaderProgram() : _programID(0), _completed(false) {}


ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::addShader(Shader* shader)
{
	if (_completed)
	{
		VoxelGame::logger.error("Cannot add a GLSL shader object to this shader program, the program has already been linked");
		return;
	}
	if (shader == nullptr)
	{
		VoxelGame::logger.warn(std::string("Failed to add GLSL shader object to program ID ") + std::to_string(_programID) + std::string(" The shader was invalid or NULL"));
		return;
	}
	if (_shaders.count(shader->getType()) != 0)
	{
		VoxelGame::logger.error(std::string("This shader program already has a ") + Shader::getShaderAsString(shader->getType()) + std::string(" attached"));
		return;
	}

	_shaders.insert(std::pair<GLuint, Shader*>(shader->getType(), shader));
}

void ShaderProgram::addShader(GLuint type, const char* file)
{
	Shader* shader = VoxelGame::resourceManager.loadShader(type, file);
	addShader(shader);
}

void ShaderProgram::addAttribute(GLint location, char* attribute)
{
	if (attribute == nullptr || _attributes.count(attribute) != 0)
		return;

	_attributes.insert(std::make_pair(attribute, location));
}

void ShaderProgram::completeProgram()
{
	if (_completed)
	{
		VoxelGame::logger.error("Cannot complete shader program, the program has already been linked");
	}
	_programID = glCreateProgram();
	VoxelGame::logger.info("Attaching shaders");
	for (std::pair<GLuint, Shader*> entry : _shaders)
	{
		entry.second->attachTo(_programID);
	}

	VoxelGame::logger.info("Binding attribute locations");
	for (auto it = _attributes.begin(); it != _attributes.end(); ++it)
	{
		VoxelGame::logger.info(std::string("Adding attribute location ") + "\"" + it->first + "\"");
		glBindAttribLocation(_programID, it->second, it->first.c_str());
	}

	GLint status = GL_FALSE;
	int logLength;

	glLinkProgram(_programID);
	glGetProgramiv(_programID, GL_LINK_STATUS, &status);
	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0)
	{
		GLchar* programError = new GLchar[logLength];
		glGetProgramInfoLog(_programID, logLength, nullptr, programError);
		VoxelGame::logger.error(std::string("Failed to link program: ") + programError);
	}

	glValidateProgram(_programID);
	glGetProgramiv(_programID, GL_VALIDATE_STATUS, &status);
	glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0)
	{
		GLchar* programError = new GLchar[logLength];
		glGetProgramInfoLog(_programID, logLength, nullptr, programError);
		VoxelGame::logger.error(std::string("Failed to validate program: ") + programError);
	}

	_completed = true;
}

void ShaderProgram::addLight(Light* light)
{
	lights.push_back(light);
}

void ShaderProgram::renderLights()
{	
	for (int i = 0; i < 10; i++)
	{
		Light* light = nullptr;

		if (i < lights.size())
			light = lights[i];

		if (light == nullptr)
			light = new Light(glm::vec3(), glm::vec3(), glm::vec3());

		setUniform(std::string("lights[") + std::to_string(i) + "]", light);
	}

	lights.clear();
}

void ShaderProgram::useProgram(bool use) const
{
	if (use)
	{
		glUseProgram(_programID);
	}
	else
	{
		glUseProgram(0);
	}
}

GLuint ShaderProgram::getProgramID() const
{
	return _programID;
}

bool ShaderProgram::isComplete() const
{
	return _completed;
}

void ShaderProgram::setUniform(std::string uniform, float f)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform1f(id, f);
	}
}

void ShaderProgram::setUniform(std::string uniform, float f, float f1)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform2f(id, f, f1);
	}
}

void ShaderProgram::setUniform(std::string uniform, float f, float f1, float f2)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform3f(id, f, f1, f2);
	}
}

void ShaderProgram::setUniform(std::string uniform, float f, float f1, float f2, float f3)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform4f(id, f, f1, f2, f3);
	}
}

void ShaderProgram::setUniform(std::string uniform, double d)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform1d(id, d);
	}
}

void ShaderProgram::setUniform(std::string uniform, double d, double d1)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform2d(id, d, d1);
	}
}

void ShaderProgram::setUniform(std::string uniform, double d, double d1, double d2)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform3d(id, d, d1, d2);
	}
}

void ShaderProgram::setUniform(std::string uniform, double d, double d1, double d2, double d3)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform4d(id, d, d1, d2, d3);
	}
}

void ShaderProgram::setUniform(std::string uniform, int i)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform1i(id, i);
	}
}

void ShaderProgram::setUniform(std::string uniform, int i, int i1)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform2i(id, i, i1);
	}
}

void ShaderProgram::setUniform(std::string uniform, int i, int i1, int i2)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform3i(id, i, i1, i2);
	}
}

void ShaderProgram::setUniform(std::string uniform, int i, int i1, int i2, int i3)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform4i(id, i, i1, i2, i3);
	}
}

void ShaderProgram::setUniform(std::string uniform, bool b)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniform1i(id, b);
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::vec2 v)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		setUniform(uniform, v.x, v.y);
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::vec3 v)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		setUniform(uniform, v.x, v.y, v.z);
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::vec4 v)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		setUniform(uniform, v.x, v.y, v.z, v.w);
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat2x2 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix2fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat2x3 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix2x3fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat2x4 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix2x4fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat3x2 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix3x2fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat3x3 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat3x4 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix3x4fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat4x2 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix4x2fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat4x3 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix4x3fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, glm::mat4x4 m)
{
	GLint id = getUniform(uniform);

	if (id >= 0)
	{
		glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(m));
	}
}

void ShaderProgram::setUniform(std::string uniform, Light* light)
{
//		VoxelGame::logger.info((char*)(std::string(uniform) + ".colour").c_str() + std::string(", ") + std::to_string(light->getColour().x) + ", " + std::to_string(light->getColour().y) + ", " + std::to_string(light->getColour().z));
		setUniform((char*)(std::string(uniform) + ".position").c_str(), light->getPosition());
		setUniform((char*)(std::string(uniform) + ".colour").c_str(), light->getColour());
//		VoxelGame::logger.info(std::to_string(light->getColour().x) + ", " + std::to_string(light->getColour().y) + ", " + std::to_string(light->getColour().z));
		setUniform((char*)(std::string(uniform) + ".attenuation").c_str(), light->getAttenuation());
		setUniform((char*)(std::string(uniform) + ".direction").c_str(), light->getDirection());
		setUniform((char*)(std::string(uniform) + ".innerRadius").c_str(), light->getInnerCone());
		setUniform((char*)(std::string(uniform) + ".outerRadius").c_str(), light->getOuterCone());
		setUniform((char*)(std::string(uniform) + ".type").c_str(), light->getType());
}

GLint ShaderProgram::getUniform(std::string uniform)
{
	GLint id = glGetUniformLocation(_programID, uniform.c_str());

//	if (id < 0 && _programID == 6)
//	{
//		GLenum err = glGetError();
//		std::cout << "Failed to load uniform loaction " << uniform << ", 0x" << std::hex << err << std::endl;
//	}

	return id;
//	GLint id = 0;
//	if (_uniforms.count(uniform) != 1)
//	{
//		id = glGetUniformLocation(_programID, uniform.c_str());
//
//		VoxelGame::logger.info(std::string("Loading uniform loaction ") + uniform + " id: " + std::to_string(id));
//
//		_uniforms.insert(std::make_pair(uniform, id));
//	} else
//	{
//		id = _uniforms.at(uniform);
//	}
//
//	return id;
}
