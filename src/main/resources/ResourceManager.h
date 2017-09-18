#pragma once

#define RESOURCE_PREFIX "res/"
#define RESOURCE_LOCATION_SHADER RESOURCE_PREFIX + "shaders/"
#include "../renderer/Shader.h"
#include "../renderer/mesh/Texture.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	bool loadFile(const char* file, std::string& dest) const;

	Shader* loadShader(GLenum type, const char* file) const;

	Texture* loadTexture(const char* file) const;
};

