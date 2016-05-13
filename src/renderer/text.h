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
	Text(const RenderWindow& renderer);
	~Text();

	void SetSize(const double& height);
	void SetSize(const double& width, const double& height);

	bool SetFace(const std::string& fontFileName);

	inline void SetColor(const Color& color) { this->color = color; }
	// TODO:  Orientation?

	inline void SetPosition(const double& x, const double& y) { this->x = x; this->y = y; }
	inline void SetScale(const double& scale) { assert(scale > 0.0); this->scale = scale; }
	inline void SetText(const std::string& text) { this->text = text; }

	Primitive::BufferInfo BuildText();
	void RenderBufferedGlyph(const unsigned int& characterCount);

	struct BoundingBox
	{
		int xLeft;
		int xRight;
		int yUp;
		int yDown;
	};

	BoundingBox GetBoundingBox(const std::string& s);

	bool IsOK() const { return isOK; }

private:
	static const std::string vertexShader;
	static const std::string fragmentShader;

	const RenderWindow& renderer;

	static unsigned int program;
	static unsigned int colorLocation;

	static FT_Library ft;
	static unsigned int ftReferenceCount;

	Primitive::BufferInfo bufferInfo;

	FT_Face face;
	Color color;

	double x;
	double y;
	double scale;// TODO:  Do I really need scale?

	std::string text;

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

	void DoInternalInitialization();
	bool GenerateGlyphs();
	static bool initialized;
	bool glyphsGenerated;

	void Initialize();
	void FreeFTResources();
	bool isOK;
};

#endif// TEXT_H_
