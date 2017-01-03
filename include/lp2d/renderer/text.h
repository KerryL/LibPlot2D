/*=============================================================================
                                   LibPlot2D
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

// Local headers
#include "lp2d/renderer/primitives/primitive.h"

// Eigen headers
#include <Eigen/Eigen>

// Freetype headers
#include <ft2build.h>
#include FT_FREETYPE_H

// Standard C++ headers
#include <string>
#include <map>
#include <limits>

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

	inline void SetColor(const Color& color) { mColor = color; }
	void SetOrientation(const double& angle);// [rad]

	inline void SetPosition(const double& x, const double& y) { mX = x; mY = y; }
	inline void SetScale(const double& scale) { assert(scale > 0.0); mScale = scale; }

	inline void SetText(const std::string& text) { mText = text; }
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

	bool IsOK() const { return mIsOK && (mGlyphsGenerated || mFace); }

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	static const std::string mVertexShader;
	static const std::string mFragmentShader;

	RenderWindow& mRenderer;

	static unsigned int mProgram;
	static unsigned int mVertexLocation;
	static unsigned int mColorLocation;
	static unsigned int mIndexLocation;
	static unsigned int mModelviewMatrixLocation;

	static FT_Library mFt;
	static unsigned int mFtReferenceCount;

	FT_Face mFace = nullptr;
	Color mColor = Color::ColorBlack;

	double mX;
	double mY;
	double mScale = 1.0;

	unsigned int mMaxXSize;
	unsigned int mMaxYSize;

	std::string mText;

	struct Glyph
	{
		unsigned int index;
		int xSize;
		int ySize;
		int xBearing;
		int yBearing;
		unsigned int advance;
	};

	std::map<char, Glyph> mGlyphs;
	unsigned int mTextureId = std::numeric_limits<unsigned int>::max();

	void DoInternalInitialization();
	bool GenerateGlyphs();
	static bool mInitialized;
	bool mGlyphsGenerated = false;

	void Initialize();
	void FreeFTResources();
	bool mIsOK = true;

	Eigen::Matrix4d mModelview;

	std::vector<Primitive::BufferInfo> mBufferVector;
	Primitive::BufferInfo AssembleBuffers();
	Primitive::BufferInfo BuildLocalText();
	void ConfigureVertexArray(Primitive::BufferInfo& bufferInfo) const;
};

}// namespace LibPlot2D

#endif// TEXT_H_
