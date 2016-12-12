/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  plotCursor.cpp
// Date:  5/5/2011
// Auth:  K. Loux
// Desc:  Represents an oscilloscope cursor on-screen.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/primitives/plotCursor.h"
#include "lp2d/renderer/primitives/axis.h"
#include "lp2d/renderer/renderWindow.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			PlotCursor
// Function:		PlotCursor
//
// Desc:		Constructor for the PlotCursor class.
//
// Input Arguments:
//		renderWindow	= RenderWindow&
//		axis			= Axis& with which we are associated
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
PlotCursor::PlotCursor(RenderWindow &renderWindow, const Axis &axis)
	: Primitive(renderWindow), axis(axis), line(renderWindow)
{
	isVisible = false;
	color = Color::ColorBlack;
	line.SetLineColor(color);

	SetDrawOrder(2800);
}

//=============================================================================
// Class:			PlotCursor
// Function:		Update
//
// Desc:		Updates the GL buffers associated with this object.
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
void PlotCursor::Update(const unsigned int& /*i*/)
{
	if (axis.IsHorizontal())
	{
		line.Build(locationAlongAxis, axis.GetOffsetFromWindowEdge(),
			locationAlongAxis, renderWindow.GetSize().GetHeight()
			- axis.GetOppositeAxis()->GetOffsetFromWindowEdge());
	}
	else
	{
		line.Build(axis.GetOffsetFromWindowEdge(), locationAlongAxis,
			renderWindow.GetSize().GetWidth()
			- axis.GetOppositeAxis()->GetOffsetFromWindowEdge(), locationAlongAxis);
	}

	bufferInfo[0] = line.GetBufferInfo();

	// Update the value of the cursor (required for accuracy when zoom changes, for example)
	value = axis.PixelToValue(locationAlongAxis);
}

//=============================================================================
// Class:			PlotCursor
// Function:		GenerateGeometry
//
// Desc:		Generates OpenGL commands to draw the cursor.
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
void PlotCursor::GenerateGeometry()
{
	if (bufferInfo.size() == 0)
		return;

	glBindVertexArray(bufferInfo[0].vertexArrayIndex);
	Line::DoPrettyDraw(bufferInfo[0].indexCount);
	glBindVertexArray(0);
}

//=============================================================================
// Class:			PlotCursor
// Function:		HasValidParameters
//
// Desc:		Checks to see if we're OK to draw the cursor.
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
bool PlotCursor::HasValidParameters()
{
	// Make sure the value is within the axis limits
	if (value >= axis.GetMinimum() && value <= axis.GetMaximum() &&
		axis.GetAxisAtMaxEnd() && axis.GetAxisAtMinEnd())
		return true;

	// If the parameters aren't valid, also hide this to prevent the cursor values from updating
	isVisible = false;

	return false;
}

//=============================================================================
// Class:			PlotCursor
// Function:		IsUnder
//
// Desc:		Checks to see if the cursor is located under the specified
//					point.
//
// Input Arguments:
//		pixel	= const unsigned int& describing the location of the point
//				  in screen coordinates
//
// Output Arguments:
//		None
//
// Return Value:
//		True is the cursor is under the specified point, false otherwise
//
//=============================================================================
bool PlotCursor::IsUnder(const unsigned int &pixel)
{
	// Apparent line width for clicking
	int width = 2;// [pixels]

	if (abs(int(locationAlongAxis - pixel)) <= width && isVisible)
		return true;

	return false;
}

//=============================================================================
// Class:			PlotCursor
// Function:		SetLocation
//
// Desc:		Sets the x position where the cursor should appear on the plot.
//
// Input Arguments:
//		location	= const int& location to set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotCursor::SetLocation(const int& location)
{
	locationAlongAxis = location;
	value = axis.PixelToValue(location);
	modified = true;
}

//=============================================================================
// Class:			PlotCursor
// Function:		operator=
//
// Desc:		Sets the x-value where the cursor should appear on the plot.
//
// Input Arguments:
//		_value	= const double& value to set
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCursor& reference to this object
//
//=============================================================================
PlotCursor& PlotCursor::operator=(const PlotCursor &target)
{
	if (&target == this)
		return *this;

	value = target.value;
	locationAlongAxis = target.locationAlongAxis;

	return *this;
}

}// namespace LibPlot2D
