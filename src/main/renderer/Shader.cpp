#include "Shader.h"

Shader::Shader(GLuint type, GLuint id, const char* file, const char* source): _program(0), _id(id), _type(type), _file(file), _source(source)
{
}

Shader::~Shader()
{
	glDetachShader(_program, _id);
	glDeleteShader(_id);
}

GLuint Shader::getPorgram() const
{
	return _program;
}

GLuint Shader::getType() const
{
	return _type;
}

GLuint Shader::getID() const
{
	return _id;
}

const char* Shader::getFile() const
{
	return _file;
}

const char* Shader::getSource() const
{
	return _source;
}

void Shader::attachTo(GLuint program)
{
	_program = program;
	glAttachShader(program, _id);
}

char* Shader::getShaderAsString(GLuint type)
{
	if (type == GL_VERTEX_SHADER) return "GL_VERTEX_SHADER";
	if (type == GL_FRAGMENT_SHADER) return "GL_FRAGMENT_SHADER";
	if (type == GL_GEOMETRY_SHADER) return "GL_GEOMETRY_SHADER";
	if (type == GL_COMPUTE_SHADER) return "GL_COMPUTE_SHADER";
	if (type == GL_TESS_EVALUATION_SHADER) return "GL_TESS_EVALUATION_SHADER";
	if (type == GL_TESS_CONTROL_SHADER) return "GL_TESS_CONTROL_SHADER";

	return "INVALID_SHADER_TYPE";
}
