#pragma once

#include <GL/glew.h>

class ShaderProgram;

class Texture
{
private:
	int _width;
	int _height;

	GLuint _textureID;
	GLuint _samplerID;

	const char* _fileRaw;

	Texture(): _width(0), _height(0), _textureID(0), _samplerID(0), _fileRaw(nullptr) {}
public:
	void bind(GLuint textureUnit = 0);

	void dispose();

	int getWidth() const;

	int getHeight() const;

	GLuint getTextureID() const;

	GLuint getSamplerID() const;

	const char* getRawFile() const;

	static Texture* loadTexture(const char* fileRaw, bool mipmaps = false);

	static Texture* loadCImg(const char* rawFile, bool mipmaps = false);
};
