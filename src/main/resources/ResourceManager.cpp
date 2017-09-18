#include <ostream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <GL/glew.h>
#include "../VoxelGame.h"
#include "ResourceManager.h"
#include "../Logger.h"

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

bool ResourceManager::loadFile(const char* file, std::string& dest) const
{
	std::ifstream fileStream(file, std::ios::in);

	if (!fileStream.is_open())
	{
		VoxelGame::logger.warn(std::string("Failed to load resource from file: ") + file);
		return false;
	}
	dest = std::string();

	while (!fileStream.eof())
	{
		std::string temp = "";
		getline(fileStream, temp);
		dest.append(temp + "\n");
	}

	fileStream.close();
	return true;
}

Shader* ResourceManager::loadShader(GLenum type, const char* file) const
{
	VoxelGame::logger.info(std::string("Loading shader file ") + file);
	if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER && type != GL_GEOMETRY_SHADER && type != GL_COMPUTE_SHADER && type != GL_TESS_EVALUATION_SHADER && type != GL_TESS_CONTROL_SHADER)
	{
		VoxelGame::logger.warn(std::string("The shader type passed to the loader was not valid or was not supported by this version. Shader file ") + file + std::string(" will not be loaded."));
		return nullptr;
	}

	GLint flag = GL_FALSE;
	int logLength;

	std::string fileRaw;
	if (!loadFile(file, fileRaw))
	{
		VoxelGame::logger.warn("An error occurred while loading this shader, this may cause errors");
		return nullptr;
	}
	const char* glslSrc = fileRaw.c_str();
	//const char* const* str;

	VoxelGame::logger.info("Compiling shader");
	GLuint shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, &glslSrc, nullptr);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &flag);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
	GLchar* shaderError = new GLchar[logLength];
	glGetShaderInfoLog(shaderID, logLength, nullptr, shaderError);

	if (flag == GL_FALSE)
	{
		VoxelGame::logger.error(std::string("Failed to compile shader: ") + shaderError);
		glDeleteShader(shaderID);
	}
	return new Shader(type, shaderID, file, glslSrc);
}

Texture* ResourceManager::loadTexture(const char* file) const
{
	return nullptr;
}
