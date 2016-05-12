/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  text.cpp
// Created:  5/12/2016
// Author:  K. Loux
// Description:  Helper object for building BufferInfo objects for text rendering.
// History:

// Standard C++ headers
#include <cassert>

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/text.h"

const std::string Text::vertexShader(
	"#version 330\n"
	"\n"
	"uniform mat4 modelviewMatrix;\n"
	"uniform mat4 projectionMatrix;\n"
	"\n"
	"layout(location = 0) in vec4 vertex;// <vec2 pos, vec2 tex>\n"
	"\n"
	"out vec2 texCoords;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    gl_Position = projectionMatrix * modelviewMatrix * vec4(vertex.xy, 0.0, 1.0);\n"
	"    texCoords = vertex.zw;\n"
	"}\n"
);

const std::string Text::fragmentShader(
	"#version 330\n"
	"\n"
	"uniform sampler2D text;\n"
	"uniform vec3 textColor;\n"
	"\n"
	"in vec2 texCoords;\n"
	"\n"
	"out vec4 color;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, texCoords).r);\n"
	"    color = vec4(textColor, 1.0) * sampled;\n"
	"}\n"
);

Text::Text()
{
	if (FT_Init_FreeType(&ft))
	{
		assert(false);
	}
}

bool Text::SetFace(const std::string& fontFileName)
{
	if (FT_New_Face(ft, fontFileName.c_str(), 0, &face))
		return false;
	return true;
}

void Text::SetSize(const double& height)
{
	SetSize(0, height);
}

void Text::SetSize(const double& width, const double& height)
{
	FT_Set_Pixel_Sizes(face, width, height);
}

bool Text::GenerateGlyphs()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	Glyph g;
  
	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
			return false;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
			GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		g.id = texture;
		g.xSize = face->glyph->bitmap.width;
		g.ySize = face->glyph->bitmap.rows;
		g.xBearing = face->glyph->bitmap_left;
		g.yBearing = face->glyph->bitmap_top;
		g.advance = face->glyph->advance.x;

		glyphs.insert(std::make_pair(c, g));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return true;
}
