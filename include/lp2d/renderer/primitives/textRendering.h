/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  textRendering.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derived from Primitive, this class is used to draw text.

#ifndef TEXT_RENDERING_H_
#define TEXT_RENDERING_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/text.h"

namespace LibPlot2D
{

/// Object representing text to be rendered using OpenGL.
class TextRendering : public Primitive
{
public:
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	explicit TextRendering(RenderWindow &renderWindow);
	~TextRendering() = default;

	/// Intializes the glyphs used to render this object.
	///
	/// \param fontFileName Path and file name of the TrueType font file.
	/// \param size         Size of the font in pixels.
	void InitializeFonts(const std::string& fontFileName, const double& size);

	/// \name Setters
	/// @{

	/// Sets the angle of the text.  Zero is horizontal, positive angles rotate
	/// the text counter-clockwise.
	///
	/// \param angle Angle at which to draw the text (in radians).
	void SetAngle(const double& angle) { mAngle = angle; mModified = true; }

	/// Sets the value of the string to render.
	///
	/// \param text Value of the string.
	void SetText(const wxString& text) { mText = text; mModified = true; }

	/// Sets the position of the text.
	///
	/// \param x X-position.
	/// \param y Y-position.
	void SetPosition(const double& x, const double& y) { mX = x; mY = y; mModified = true; }

	/// Sets a flag indicating whether or not the text should be centered at
	/// the specified location.  If false (default), text is positioned with
	/// respect to the lower left-hand corner of the bounding box.
	void SetCentered(const bool& centered) { mCentered = centered; mModified = true; }

	/// @}

	/// \name Getters
	/// @{

	double GetTextHeight();
	double GetTextWidth();
	wxString GetText() const { return mText; }

	/// @}

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void Update(const unsigned int& i) override;
	void GenerateGeometry() override;

private:
	double mAngle = 0.0;// 0 is horizontal, angle builds counter-clockwise about an axis out of the screen

	wxString mText;
	Text mFont;

	// Flag indicating whether the text is centered at (X, Y) or if, if false,
	// (0, 0) represents the lower left corner of the text bounding box
	bool mCentered = false;
	double mX = 0.0;
	double mY = 0.0;
};

}// namespace LibPlot2D

#endif// TEXT_RENDERING_H_
