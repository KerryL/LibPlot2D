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

/// Helper class for building BufferInfo objects for rendering text.
class Text
{
public:
	/// Constructor.
	///
	/// \param renderer The window in which the text will be rendered.
	explicit Text(RenderWindow& renderer);
	~Text();

	// Enforce rule of 5
	Text(const Text&) = delete;
	Text(Text&&) = delete;
	Text& operator=(const Text&) = delete;
	Text& operator=(Text&&) = delete;

	/// Sets the size of the text.  The width is automatically determined.
	///
	/// \param height Height of the text in pixels.
	void SetSize(const double& height);

	/// Sets the size of the text.  Allows intentional distortion of glyphs by
	/// stretching/squeezing the characters.
	///
	/// \param width  Width of the text in pixels.
	/// \param height Height of the text in pixels.
	void SetSize(const double& width, const double& height);

	/// Sets the TrueType font file to use for generating glyphs.
	///
	/// \param fontFileName Path and file name to the font file.
	///
	/// \returns True if the font file was successfully loaded.
	bool SetFace(const std::string& fontFileName);

	/// Sets the text color.
	///
	/// \param color Color to use for rendering the text.
	inline void SetColor(const Color& color) { mColor = color; }

	/// Sets the text orientation.  Zero is horizontal and positive angles
	/// rotate the text counter-clockwise.
	///
	/// \param angle Angle at which the text should be rendered. <b>[rad]</b>
	void SetOrientation(const double& angle);// [rad]

	/// Sets the position of the bounding box.
	///
	/// \param x X-location.
	/// \param y Y-location.
	inline void SetPosition(const double& x, const double& y) { mX = x; mY = y; }

	/// Sets the scale factor.
	///
	/// \param scale Factor to use.
	inline void SetScale(const double& scale) { assert(scale > 0.0); mScale = scale; }

	/// Sets the string to render.
	///
	/// \param text String to render.
	inline void SetText(const std::string& text) { mText = text; }

	/// Appends text to the existing string contents.
	///
	/// \param text Strint to append.
	void AppendText(const std::string& text);

	/// Builds a buffer for rendering the associated glyphs.
	/// \returns A buffer for rendering the associated glyphs.
	Primitive::BufferInfo BuildText();

	/// Function to render the buffered glyph (i.e. to be called from
	/// Primitive::GenerateGeometry()).
	void RenderBufferedGlyph(const unsigned int& vertexCount);

	/// Structure representing bounding box information.
	struct BoundingBox
	{
		int xLeft;///< Pixels to the left.
		int xRight;///< Pixels to the right.
		int yUp;///< Pixels up.
		int yDown;///< Pixels down.
	};

	/// Returns the bounding box associated with the specified string.
	///
	/// \param s String for which the bounding box is to be generated.
	///
	/// \returns The bounding box for the string.
	BoundingBox GetBoundingBox(const std::string& s);

	/// Checks to see if this object is ready to render.
	/// \returns True if this object was successfully initialized and is ready
	///          to render.
	bool IsOK() const { return mIsOK && (mGlyphsGenerated || mFace); }

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	static const std::string mVertexShader;
	static const std::string mFragmentShader;

	static const std::string mTextColorName;
	static const std::string mVertexName;
	static const std::string mTextureIndexName;

	RenderWindow& mRenderer;

	static GLuint mProgram;
	static GLint mVertexLocation;
	static GLint mIndexLocation;

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
