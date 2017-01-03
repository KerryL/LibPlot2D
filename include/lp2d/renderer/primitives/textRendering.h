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

class TextRendering : public Primitive
{
public:
	explicit TextRendering(RenderWindow &renderWindow);
	~TextRendering() = default;

	// Set option methods
	void SetAngle(const double& angle) { mAngle = angle; mModified = true; }// [rad]
	void InitializeFonts(const std::string& fontFileName, const double& size);
	void SetText(const wxString& text) { mText = text; mModified = true; }
	void SetPosition(const double& x, const double& y) { mX = x; mY = y; mModified = true; }
	void SetCentered(const bool& centered) { mCentered = centered; mModified = true; }

	double GetTextHeight();
	double GetTextWidth();
	wxString GetText() const { return mText; }

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
