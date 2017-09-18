//#define cimg_use_jpeg
#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include <stb_image.h>
#include <string>
#include "../../VoxelGame.h"
#include "../../Logger.h"


void Texture::bind(GLuint textureUnit)
{
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glBindSampler(textureUnit, _samplerID);
}

void Texture::dispose()
{
	if (this != nullptr)
	{
		glDeleteSamplers(1, &_samplerID);
		glDeleteTextures(1, &_textureID);
	}
}

int Texture::getWidth() const
{
	return _width;
}

int Texture::getHeight() const
{
	return _height;
}

GLuint Texture::getTextureID() const
{
	return _textureID;
}

GLuint Texture::getSamplerID() const
{
	return _samplerID;
}

const char* Texture::getRawFile() const
{
	return _fileRaw;
}

Texture* Texture::loadTexture(const char* fileRaw, bool mipmaps)
{	
	int depth;
	Texture* texture = new Texture();
	unsigned char* image = stbi_load(fileRaw, &texture->_width, &texture->_height, &depth, STBI_rgb_alpha);

	if (image == nullptr)
	{
		VoxelGame::logger.info("Loading image without alpha channels");
		image = stbi_load(fileRaw, &texture->_width, &texture->_height, &depth, STBI_rgb);
		if (image == nullptr)
		{
			VoxelGame::logger.warn("An error occurred while loading the image file " + std::string(fileRaw));
			return nullptr;
		}
	}

	VoxelGame::logger.info(std::string("Loading image file ") + fileRaw + " with dimensions[" + std::to_string(texture->_width) + ", " + std::to_string(texture->_height) + "] bit-depth=" + std::to_string(depth));

	glGenTextures(1, &texture->_textureID);

	glBindTexture(GL_TEXTURE_2D, texture->_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	GLenum mode = GL_RGBA;// depth == 3 ? GL_RGB : depth == 4 ? GL_RGBA : GL_LUMINANCE;
	glTexImage2D(GL_TEXTURE_2D, 0, mode,  texture->_width, texture->_height, 0, mode, GL_UNSIGNED_BYTE, image);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(image);
	return texture;
}

//Texture* Texture::loadCImg(const char* rawFile, bool mipmaps)
//{
//	cimg_library::CImg<unsigned char> image;
//
//	try
//	{
//		image.load(rawFile);
//	} catch (cimg_library::CImgException exception)
//	{
//		VoxelGame::logger.warn(std::string("An error occurred while loading the image file ") + rawFile);
//		VoxelGame::logger.error(exception.what());
//	}
//
//	if (image)
//	{
//		Texture* texture = new Texture();
//
//		texture->_width = image.width();
//		texture->_height = image.height();
//		texture->_samplerID = 0;
//		int format = GL_RGB;
//		glGenTextures(1, &texture->_textureID);
//		glBindTexture(GL_TEXTURE_2D, texture->_textureID);
//		image.permute_axes("cxyz");
//		glTexImage2D(GL_TEXTURE_2D, 0, format, texture->_width, _texture->_height, 0, format, GL_UNSIGNED_BYTE, image);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//		if (mipmaps)
//		{
//			glGenerateMipmap(GL_TEXTURE_2D);
//		}
//		return texture;
//	}
//
//	return nullptr;
//}