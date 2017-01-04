/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  textRendering.cpp
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derived from Primitive for creating text objects on a plot.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/primitives/textRendering.h"
#include "lp2d/renderer/text.h"
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			TextRendering
// Function:		TextRendering
//
// Description:		Constructor for the TextRendering class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
TextRendering::TextRendering(RenderWindow &renderWindow)
	: Primitive(renderWindow), mFont(renderWindow)
{
	mColor = Color::ColorBlack;
}

//=============================================================================
// Class:			TextRendering
// Function:		Update
//
// Description:		Updates the GL buffers associated with this object.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void TextRendering::Update(const unsigned int& /*i*/)
{
	mFont.SetColor(mColor);
	mFont.SetOrientation(mAngle);
	mFont.SetText(mText.ToStdString());

	if (mCentered)
	{
		// TODO:  Is this correct?
		mFont.SetPosition(mX - GetTextWidth() * 0.5 * cos(mAngle)
			+ GetTextHeight() * 0.5 * sin(mAngle),
			mY - GetTextWidth() * 0.5 * sin(mAngle)
			- GetTextHeight() * 0.5 * cos(mAngle));
	}
	else
		mFont.SetPosition(mX, mY);

	mBufferInfo[0] = mFont.BuildText();
}

//=============================================================================
// Class:			TextRendering
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void TextRendering::GenerateGeometry()
{
	if (mFont.IsOK() && mBufferInfo[0].vertexCount > 0)
	{
		glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
		mFont.RenderBufferedGlyph(mBufferInfo[0].vertexCount);
	}
}

//=============================================================================
// Class:			TextRendering
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//=============================================================================
bool TextRendering::HasValidParameters()
{
	// Don't draw if the angle is not a number
	if (PlotMath::IsNaN(mAngle))
		return false;

	if (!mFont.IsOK() || !mText.IsEmpty())
		return false;

	return true;
}

//=============================================================================
// Class:			TextRendering
// Function:		GetTextHeight
//
// Description:		Returns the height of the bounding box for the current
//					text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double, height in pixels of the current text
//
//=============================================================================
double TextRendering::GetTextHeight()
{
	if (!mFont.IsOK())
		return 0.0;

	Text::BoundingBox boundingBox(mFont.GetBoundingBox(mText.ToStdString()));

	return boundingBox.yUp - boundingBox.yDown;
}

//=============================================================================
// Class:			TextRendering
// Function:		GetTextWidth
//
// Description:		Returns the width of the bounding box for the current
//					text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double, width in pixels of the current text
//
//=============================================================================
double TextRendering::GetTextWidth()
{
	if (!mFont.IsOK())
		return 0.0;

	Text::BoundingBox boundingBox(mFont.GetBoundingBox(mText.ToStdString()));

	return boundingBox.xRight - boundingBox.xLeft;
}

//=============================================================================
// Class:			TextRendering
// Function:		InitializeFonts
//
// Description:		Initializes the font object.
//
// Input Arguments:
//		fontFileName	= const std::string&
//		size			= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void TextRendering::InitializeFonts(const std::string& fontFileName,
	const double& size)
{
	if (!mFont.SetFace(fontFileName))
		return;

	mFont.SetColor(mColor);

	// For some reason, fonts tend to render more clearly at a larger size.  So
	// we up-scale to render the fonts then down-scale to achieve the desired
	// on-screen size.
	// TODO:  OGL4 Better to use a fixed large size and adjust scale accordingly?
	const double factor(3.0);
	mFont.SetSize(size * factor);
	mFont.SetScale(1.0 / factor);
}

}// namespace LibPlot2D
