/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  text.h
// Date:  5/12/2016
// Auth:  K. Loux
// Desc:  Helper object for building BufferInfo objects for text rendering.

#ifndef TEXT_H_
#define TEXT_H_

// Standard C++ headers
#include <string>
#include <map>
#include <limits>

// Freetype headers
#include <ft2build.h>
#include FT_FREETYPE_H

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/utilities/math/matrix.h"

namespace LibPlot2D
{

class Text
{
public:
	explicit Text(RenderWindow& renderer);
	~Text();

	// Enforce rule of 5
	Text(const Text&) = delete;
	Text(Text&&) = delete;
	Text& operator=(const Text&) = delete;
	Text& operator=(Text&&) = delete;

	void SetSize(const double& height);
	void SetSize(const double& width, const double& height);

	bool SetFace(const std::string& fontFileName);

	inline void SetColor(const Color& color) { this->color = color; }
	void SetOrientation(const double& angle);// [rad]

	inline void SetPosition(const double& x, const double& y) { this->x = x; this->y = y; }
	inline void SetScale(const double& scale) { assert(scale > 0.0); this->scale = scale; }

	inline void SetText(const std::string& text) { this->text = text; }
	void AppendText(const std::string& text);

	Primitive::BufferInfo BuildText();
	void RenderBufferedGlyph(const unsigned int& vertexCount);

	struct BoundingBox
	{
		int xLeft;
		int xRight;
		int yUp;
		int yDown;
	};

	BoundingBox GetBoundingBox(const std::string& s);

	bool IsOK() const { return isOK && (glyphsGenerated || face); }

private:
	static const std::string vertexShader;
	static const std::string fragmentShader;

	RenderWindow& renderer;

	static unsigned int program;
	static unsigned int vertexLocation;
	static unsigned int colorLocation;
	static unsigned int indexLocation;
	static unsigned int modelviewMatrixLocation;

	static FT_Library ft;
	static unsigned int ftReferenceCount;

	FT_Face face;
	Color color;

	double x;
	double y;
	double scale;

	unsigned int maxXSize;
	unsigned int maxYSize;

	std::string text;

	struct Glyph
	{
		unsigned int index;
		int xSize;
		int ySize;
		int xBearing;
		int yBearing;
		unsigned int advance;
	};

	std::map<char, Glyph> glyphs;
	unsigned int textureId = std::numeric_limits<unsigned int>::max();

	void DoInternalInitialization();
	bool GenerateGlyphs();
	static bool initialized;
	bool glyphsGenerated;

	void Initialize();
	void FreeFTResources();
	bool isOK;

	Matrix modelview;

	std::vector<Primitive::BufferInfo> bufferVector;
	Primitive::BufferInfo AssembleBuffers();
	Primitive::BufferInfo BuildLocalText();
	void ConfigureVertexArray(Primitive::BufferInfo& bufferInfo) const;
};

}// namespace LibPlot2D

#endif// TEXT_H_
