#include "FontRenderer.h"
#include "../mesh/Mesh.h"
#include "../../Logger.h"
#include <algorithm>
#include <iostream>
#include "../WindowHandler.h"

FontRenderer::FontRenderer()
{

}

FontRenderer::~FontRenderer()
{
}

void FontRenderer::init()
{
	if (FT_Init_FreeType(&_library))
	{
		VoxelGame::logger.error("Failed to initialize FreeType library");
	}

	_shaderProgram = new ShaderProgram();
	_shaderProgram->addShader(GL_VERTEX_SHADER, "res/shaders/fontRenderer/VertexShader.glsl");
	_shaderProgram->addShader(GL_FRAGMENT_SHADER, "res/shaders/fontRenderer/FragmentShader.glsl");
	_shaderProgram->addAttribute(ATTRIBUTE_VERTEX_GEOMETRIC, "position");
	_shaderProgram->addAttribute(ATTRIBUTE_VERTEX_TEXTURE, "texture");
	_shaderProgram->addAttribute(ATTRIBUTE_VERTEX_COLOUR, "colour");
	_shaderProgram->completeProgram();
}

void FontRenderer::render()
{
	_shaderProgram->useProgram(true);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCullFace(GL_BACK);

	for (auto it0 = _screenText.begin(); it0 != _screenText.end(); ++it0)
	{
		FontObject* font = it0->first;
		std::vector<RenderableText*> texts = it0->second;

		for (auto it1 = texts.begin(); it1 != texts.end(); ++it1)
		{
			RenderableText* text = *it1;

			if (text->colour.a <= 0.0F)
				continue; //ignore invisible text

			for (const char* c = text->string; *c; c++)
			{
				Character character = font->characters[*c];

				float w = WindowHandler::getWidth();
				float h = WindowHandler::getHeight();

				float ch_w = ((character.characterSize.x / w) / PIXEL_SIZE) * text->fontSize * 2.0F;
				float ch_h = ((character.characterSize.y / h) / PIXEL_SIZE) * text->fontSize * 2.0F;

				float ch_g_x0 = (text->x / w - 1.0F);
				float ch_g_y0 = (text->y / h - 1.0F);
				float ch_g_x1 = ch_g_x0 + ch_w;
				float ch_g_y1 = ch_g_y0 + ch_h;

				float ch_t_x0 = character.textureCoord.x;
				float ch_t_y1 = character.textureCoord.y;
				float ch_t_x1 = character.textureCoord.x + character.textureSize.x;
				float ch_t_y0 = character.textureCoord.y + character.textureSize.y;

				float ch_c_r = text->colour.r;
				float ch_c_g = text->colour.g;
				float ch_c_b = text->colour.b;
				float ch_c_a = text->colour.a;

				text->x += character.advance / PIXEL_SIZE * text->fontSize * 2.0F;

				if (ch_w == 0 || ch_h == 0)
					continue;

				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);

				glActiveTexture(GL_TEXTURE0 + font->textureUnit);
				glBindTexture(GL_TEXTURE_2D, font->textureAtlas);
				glBindSampler(font->textureUnit, font->textureUnit);
				_shaderProgram->setUniform("textureUnit", static_cast<int>(font->textureUnit));

				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBegin(GL_QUADS);

				glVertexAttrib4f(ATTRIBUTE_VERTEX_COLOUR, ch_c_r, ch_c_g, ch_c_b, ch_c_a);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_TEXTURE, ch_t_x1, ch_t_y0);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_GEOMETRIC, ch_g_x1, ch_g_y0);

				glVertexAttrib4f(ATTRIBUTE_VERTEX_COLOUR, ch_c_r, ch_c_g, ch_c_b, ch_c_a);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_TEXTURE, ch_t_x1, ch_t_y1);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_GEOMETRIC, ch_g_x1, ch_g_y1);

				glVertexAttrib4f(ATTRIBUTE_VERTEX_COLOUR, ch_c_r, ch_c_g, ch_c_b, ch_c_a);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_TEXTURE, ch_t_x0, ch_t_y1);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_GEOMETRIC, ch_g_x0, ch_g_y1);

				glVertexAttrib4f(ATTRIBUTE_VERTEX_COLOUR, ch_c_r, ch_c_g, ch_c_b, ch_c_a);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_TEXTURE, ch_t_x0, ch_t_y0);
				glVertexAttrib2f(ATTRIBUTE_VERTEX_GEOMETRIC, ch_g_x0, ch_g_y0);

				glEnd();
			}
		}

		it0->second.clear();
	}
	_shaderProgram->useProgram(false);
}

void FontRenderer::drawText(const char* text, const char* fontName, float x, float y, float fontSize, glm::vec4 colour)
{
	FontObject* font = getFont(fontName);
	if (font != nullptr)
	{
		_screenText[font].push_back(new RenderableText{text, x * 2.0F, y * 2.0F, fontSize, colour});
	} else
	{
		VoxelGame::logger.error("Failed to load font object");
	}
}

glm::vec2 FontRenderer::getTextSize(const char* text, const char* fontName, float fontSize)
{
	FontObject* font = getFont(fontName);
	glm::vec2 size;

	if (font == nullptr)
	{
		return size;
	}

	float x = 0.0F;
	for (const char* c = text; *c; ++c)
	{
		Character character = font->characters[*c];

		size.x += character.advance / PIXEL_SIZE * fontSize;
		size.y = std::max(((character.characterSize.y) / PIXEL_SIZE) * fontSize, size.y);
	}

	return size;
}

FontObject* FontRenderer::loadFontObject(std::string name) const
{
	VoxelGame::logger.info(std::string("Loading font face \"") + name + "\"");
	FontObject* font = new FontObject();

	FT_Face face;

	if (FT_New_Face(_library, std::string(GET_FONT_RES(name)).c_str(), 0, &face))
	{
		VoxelGame::logger.error(std::string("Failed to load font face \"") + name + "\"");
		return nullptr;
	}

	FT_Set_Char_Size(face, PIXEL_SIZE * 64, PIXEL_SIZE * 64, 96, 96);

	FT_GlyphSlot glyph = face->glyph;

	for (int i = 32; i < 128; i++)
	{
		FT_Error error = FT_Load_Char(face, i, FT_LOAD_RENDER);
		if (error)
		{
			VoxelGame::logger.error(std::string("Failed to load ASCII character \'") + ((char)i) + "\' to font face \"" + name + "\", Error code " + std::to_string(error));
			continue;
		}

		font->textureSize.x += glyph->bitmap.width;
		font->textureSize.y = std::max(font->textureSize.y, (float)glyph->bitmap.rows);
	}

	font->textureUnit = 0;
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + font->textureUnit);
	glBindTexture(GL_TEXTURE_2D, font->textureAtlas);
	glBindSampler(font->textureUnit, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font->textureSize.x, font->textureSize.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);

	int x = 0;
	int y = 0;

	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			VoxelGame::logger.error(std::string("Failed to load ASCII character \'") + ((char)i) + "\' to font face \"" + name + "\"");
			continue;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, glyph->bitmap.width, glyph->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

		font->characters[i].advance = glyph->advance.x >> 6;
		font->characters[i].characterSize.x = (float) glyph->bitmap.width;
		font->characters[i].characterSize.y = (float) glyph->bitmap.rows;
		font->characters[i].characterOffset.x = (float) glyph->bitmap_left;
		font->characters[i].characterOffset.y = (float) glyph->bitmap_top;
		font->characters[i].textureCoord.x = (float)x / font->textureSize.x;
		font->characters[i].textureCoord.y = (float)y / font->textureSize.y;
		font->characters[i].textureSize.x = glyph->bitmap.width / font->textureSize.x;
		font->characters[i].textureSize.y = glyph->bitmap.rows / font->textureSize.y;
		x += glyph->bitmap.width;
	}

	return font;
}

FontObject* FontRenderer::getFont(std::string name)
{
	if (name.length() <= 0)
	{
		return nullptr;
	}

	if (_cachedFonts.count(name) == 0)
	{
		FontObject* font = loadFontObject(name);

		if (font != nullptr)
		{
			_cachedFonts.insert(std::make_pair(name, font));
		}

		return font;
	}

	return _cachedFonts[name];
}
