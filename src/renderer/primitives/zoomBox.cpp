/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  zoomBox.cpp
// Date:  5/5/2011
// Auth:  K. Loux
// Desc:  Logic for drawing the zoom box as the user moves the mouse.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/primitives/zoomBox.h"
#include "lp2d/renderer/renderWindow.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			ZoomBox
// Function:		ZoomBox
//
// Description:		Constructor for ZoomBox class.
//
// Input Arguments:
//		renderWindow	= RenderWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
ZoomBox::ZoomBox(RenderWindow &renderWindow) : Primitive(renderWindow),
	box(renderWindow)
{
	// Initially, we don't want to draw this
	isVisible = false;

	xAnchor = 0;
	yAnchor = 0;

	xFloat = 0;
	yFloat = 0;

	color = Color::ColorBlack;
}

//=============================================================================
// Class:			ZoomBox
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
void ZoomBox::Update(const unsigned int& /*i*/)
{
	std::vector<std::pair<double, double>> points;
	points.push_back(std::make_pair(xAnchor, yAnchor));
	points.push_back(std::make_pair(xFloat, yAnchor));
	points.push_back(std::make_pair(xFloat, yFloat));
	points.push_back(std::make_pair(xAnchor, yFloat));
	points.push_back(std::make_pair(xAnchor, yAnchor));
	box.Build(points, bufferInfo[0]);
}

//=============================================================================
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
//=============================================================================
void ZoomBox::GenerateGeometry()
{
	glBindVertexArray(bufferInfo[0].GetVertexArrayIndex());
	Line::DoPrettyDraw(bufferInfo[0].indexBuffer.size());
	glBindVertexArray(0);
}

//=============================================================================
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
//=============================================================================
bool ZoomBox::HasValidParameters()
{
	// Just make sure the box actually exists
	// It is up to the render window to ensure the coordinates are valid
	if (abs(int(xAnchor - xFloat)) > 0 && abs(int(yAnchor - yFloat)) > 0)
		return true;

	return false;
}

//=============================================================================
// Class:			ZoomBox
// Function:		SetAnchorCorner
//
// Description:		Sets the location of the box's fixed corner.
//
// Input Arguments:
//		xAnchor	= const unsigned int& (pixels)
//		yAnchor	= const unsigned int& (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void ZoomBox::SetAnchorCorner(const unsigned int &xAnchor, const unsigned int &yAnchor)
{
	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;

	mModified = true;
}

//=============================================================================
// Class:			ZoomBox
// Function:		SetFloatingCorner
//
// Description:		Sets the location of the box's floating corner.
//
// Input Arguments:
//		xFloat	= const unsigned int& (pixels)
//		yFloat	= const unsigned int& (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void ZoomBox::SetFloatingCorner(const unsigned int &xFloat, const unsigned int &yFloat)
{
	this->xFloat = xFloat;
	this->yFloat = yFloat;

	mModified = true;
}

}// namespace LibPlot2D
