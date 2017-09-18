#pragma once

#include <GL/glew.h>

class Shader
{
private:
	GLuint _program;
	GLuint _type;
	GLuint _id;
	const char* _file;
	const char* _source;

public:
	Shader(GLuint type, GLuint id, const char* file, const char* source);

	~Shader();

	GLuint getPorgram() const;

	GLuint getType() const;

	GLuint getID() const;

	/**
	 * The file path of this shader
	 */
	const char* getFile() const;

	/**
	 * The source code for this shader
	 */
	const char* getSource() const;

	void attachTo(GLuint program);

	static char* getShaderAsString(GLuint type);
};

