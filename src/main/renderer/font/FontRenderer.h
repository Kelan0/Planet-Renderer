#pragma once
#include "../../VoxelGame.h"
#include "../ShaderProgram.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#define GET_FONT_RES(font) "res/fonts/" + font + ".ttf"
#define PIXEL_SIZE 120

struct Character
{
	int ASCII_ID;
	float advance; //the number of pixels the cursor should advance after adding this character

	glm::vec2 textureCoord; //the texture x y on the texture atlas
	glm::vec2 textureSize; //the texture width and height on the texture atlas
	glm::vec2 characterOffset; //the distance from the cursor to the left and top of the character quad
	glm::vec2 characterSize; //the width and height of the character in screenspace
};

struct Word
{
private:
	std::vector<Character> characters;

public:
	float width;
	float fontSize;

	Word(float fontSize) : width(0), fontSize(fontSize) {}

	void addCharacter(Character character)
	{
		characters.push_back(character);
		width += character.advance * fontSize;
	}

	std::vector<Character> getCharacters() const
	{
		return characters;
	}
};

struct Line
{
	float width;
	float maxWidth;
	float spaceWidth;

	std::vector<Word> words;

	Line(float spaceWidth, float fontSize, float maxWidth) : width(0), maxWidth(maxWidth), spaceWidth(spaceWidth * fontSize) {}

	bool addWord(Word word)
	{
		float f = word.width + (words.size() == 0 ? 0 : spaceWidth);

		if (width + f <= maxWidth)
		{
			words.push_back(word);
			width += f;
			return true;
		}

		return false;
	}
};

struct FontObject
{
	GLuint textureAtlas;
	GLuint textureUnit;
	glm::vec2 textureSize;

	Character characters[128];
};

struct RenderableText
{
	const char* string;
	float x;
	float y;
	float fontSize;
	glm::vec4 colour;
};

class FontRenderer
{
private:
	ShaderProgram* _shaderProgram;
	FT_Library _library;

	std::map<FontObject*, std::vector<RenderableText*>> _screenText;
	std::map<std::string, FontObject*> _cachedFonts;
public:
	FontRenderer();
	~FontRenderer();

	void init();

	void render();

	void drawText(const char* text, const char* fontName, float x, float y, float fontSize, glm::vec4 colour = glm::vec4(1.0F));

	glm::vec2 getTextSize(const char* text, const char* fontName, float fontSize);

	FontObject* loadFontObject(std::string name) const;

	FontObject* getFont(std::string name);
};
