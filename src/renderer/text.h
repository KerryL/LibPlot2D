/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  text.h
// Created:  5/12/2016
// Author:  K. Loux
// Description:  Helper object for building BufferInfo objects for text rendering.
// History:

#ifndef TEXT_H_
#define TEXT_H_

// Standard C++ headers
#include <string>
#include <map>

// Freetype headers
#include <ft2build.h>
#include FT_FREETYPE_H

// Local headers
#include "renderer/primitives/primitive.h"

class Text
{
public:
	Text();

	void SetSize(const double& height);
	void SetSize(const double& width, const double& height);

	bool SetFace(const std::string& fontFileName);

	// TODO:  Orientation?  Color?

	Primitive::BufferInfo SetText(const std::string& text);
	void RenderBufferedGlyph();

	static const std::string vertexShader;
	static const std::string fragmentShader;

private:
	FT_Library ft;
	Primitive::BufferInfo bufferInfo;

	FT_Face face;

	struct Glyph
	{
		unsigned int id;
		int xSize;
		int ySize;
		int xBearing;
		int yBearing;
		unsigned int advance;
	};

	std::map<char, Glyph> glyphs;

	bool GenerateGlyphs();
};

#endif// TEXT_H_
