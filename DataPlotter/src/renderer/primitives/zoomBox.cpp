/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  zoomBox.cpp
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Logic for drawing the zoom box as the user moves the mouse.
// History:

// Local headers
#include "renderer/primitives/zoomBox.h"
#include "renderer/renderWindow.h"

//==========================================================================
// Class:			ZoomBox
// Function:		ZoomBox
//
// Description:		Constructor for ZoomBox class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
ZoomBox::ZoomBox(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	// Initially, we don't want to draw this
	isVisible = false;

	xAnchor = 0;
	yAnchor = 0;

	xFloat = 0;
	yFloat = 0;

	color = Color::ColorBlack;
}

//==========================================================================
// Class:			ZoomBox
// Function:		GenerateGeometry
//
// Description:		Generates OpenGL commands to draw the box.
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
void ZoomBox::GenerateGeometry(void)
{
	// Set the line width
	glLineWidth(1.0f);

	// Create the box
	glBegin(GL_LINE_STRIP);

	// Draw the axis
	glVertex2i(xAnchor, yAnchor);
	glVertex2i(xFloat, yAnchor);
	glVertex2i(xFloat, yFloat);
	glVertex2i(xAnchor, yFloat);
	glVertex2i(xAnchor, yAnchor);

	glEnd();
}

//==========================================================================
// Class:			ZoomBox
// Function:		HasValidParameters
//
// Description:		Checks to see if we're OK to draw the box.
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
bool ZoomBox::HasValidParameters(void)
{
	// Just make sure the box actually exists
	// It is up to the render window to ensure the coordinates are valid
	if (abs(int(xAnchor - xFloat)) > 0 && abs(int(yAnchor - yFloat)) > 0)
		return true;

	return false;
}

//==========================================================================
// Class:			ZoomBox
// Function:		SetAnchorCorner
//
// Description:		Sets the location of the box's fixed corner.
//
// Input Arguments:
//		_xAnchor	= const unsigned int& (pixels)
//		_yAnchor	= const unsigned int& (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ZoomBox::SetAnchorCorner(const unsigned int &_xAnchor, const unsigned int &_yAnchor)
{
	xAnchor = _xAnchor;
	yAnchor = _yAnchor;

	modified = true;
}

//==========================================================================
// Class:			ZoomBox
// Function:		SetFloatingCorner
//
// Description:		Sets the location of the box's floating corner.
//
// Input Arguments:
//		_xFloat	= const unsigned int& (pixels)
//		_yFloat	= const unsigned int& (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void ZoomBox::SetFloatingCorner(const unsigned int &_xFloat, const unsigned int &_yFloat)
{
	xFloat = _xFloat;
	yFloat = _yFloat;

	modified = true;
}