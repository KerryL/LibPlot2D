/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  text.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating text objects on a plot.
// History:

// Local headers
#include "renderer/primitives/text.h"
#include "renderer/renderWindow.h"
#include "utilities/math/plotMath.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			TextRendering
// Function:		TextRendering
//
// Description:		Constructor for the TextRendering class.
//
// Input Arguments:
//		_RenderWindow	= RENDER_WINDOW& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TextRendering::TextRendering(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	color.Set(0.0, 0.0, 0.0, 1.0);

	angle = 0.0;
	x = 0;
	y = 0;
	text = wxEmptyString;

	centered = false;

	font = NULL;
}

//==========================================================================
// Class:			TextRendering
// Function:		~TextRendering
//
// Description:		Destructor for the TextRendering class.
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
//==========================================================================
TextRendering::~TextRendering()
{
}

//==========================================================================
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
//==========================================================================
void TextRendering::GenerateGeometry(void)
{
	// Add the text
	if (font && !text.IsEmpty())
	{
		glPushMatrix();
			glLoadIdentity();

			// Position the text
			if (centered)
				glTranslated(x - GetTextWidth() / 2.0 * cos(angle * PlotMath::pi / 180.0)
					+ GetTextHeight() / 2.0 * sin(angle * PlotMath::pi / 180.0),
					y - GetTextWidth() / 2.0 * sin(angle * PlotMath::pi / 180.0)
					- GetTextHeight() / 2.0 * cos(angle * PlotMath::pi / 180.0), 0.0);
			else
				glTranslated(x, y, 0.0);
			glRotated(angle, 0.0, 0.0, 1.0);

			// Render the text
			font->Render(text.c_str());
		glPopMatrix();
	}
}

//==========================================================================
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
//==========================================================================
bool TextRendering::HasValidParameters(void)
{
	// Don't draw if the angle is not a number
	if (PlotMath::IsNaN(angle))
		return false;

	return true;
}

//==========================================================================
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
//==========================================================================
double TextRendering::GetTextHeight(void) const
{
	if (!font)
		return 0.0;

	FTBBox boundingBox = font->BBox(text.c_str());

	return boundingBox.Upper().Y() - boundingBox.Lower().Y();
}

//==========================================================================
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
//==========================================================================
double TextRendering::GetTextWidth(void) const
{
	if (!font)
		return 0.0;

	FTBBox boundingBox = font->BBox(text.c_str());

	return boundingBox.Upper().X() - boundingBox.Lower().X();
}