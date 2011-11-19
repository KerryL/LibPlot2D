/*===================================================================================
                                    DataPlotter
                         Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCursor.cpp
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Represents an oscilloscope cursor on-screen.
// History:
//  5/12/2011 - Renamed to PlotCursor from Cursor due to conflict in X.h, K. Loux

// Local headers
#include "renderer/primitives/plotCursor.h"
#include "renderer/primitives/axis.h"
#include "renderer/renderWindow.h"

//==========================================================================
// Class:			PlotCursor
// Function:		PlotCursor
//
// Description:		Constructor for the PlotCursor class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow&
//		_axis			= Axis& with which we are associated
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotCursor::PlotCursor(RenderWindow &_renderWindow, const Axis &_axis)
	: Primitive(_renderWindow), axis(_axis)
{
	// Start out invisible
	isVisible = false;

	color = Color::ColorBlack;
}

//==========================================================================
// Class:			PlotCursor
// Function:		GenerateGeometry
//
// Description:		Generates OpenGL commands to draw the cursor.
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
void PlotCursor::GenerateGeometry(void)
{
	// The on-screen representation of the cursor is just a line, either horizontal
	// or vertical, whichever the axis we're associated with is not

	// Set the line width
	glLineWidth(1.0f);

	// Create the axis
	glBegin(GL_LINES);

	// Draw the cursor
	unsigned int dimension;// [pixels]

	if (axis.IsHorizontal())
	{
		dimension = renderWindow.GetSize().GetWidth()
				- axis.GetAxisAtMinEnd()->GetOffsetFromWindowEdge()
				- axis.GetAxisAtMaxEnd()->GetOffsetFromWindowEdge();
		glVertex2i(locationAlongAxis, axis.GetOffsetFromWindowEdge());
		glVertex2i(locationAlongAxis, renderWindow.GetSize().GetHeight()
				- axis.GetOppositeAxis()->GetOffsetFromWindowEdge());
	}
	else
	{
		dimension = renderWindow.GetSize().GetHeight()
				- axis.GetAxisAtMinEnd()->GetOffsetFromWindowEdge()
				- axis.GetAxisAtMaxEnd()->GetOffsetFromWindowEdge();
		glVertex2i(axis.GetOffsetFromWindowEdge(), locationAlongAxis);
		glVertex2i(renderWindow.GetSize().GetWidth()
				- axis.GetOppositeAxis()->GetOffsetFromWindowEdge(), locationAlongAxis);
	}

	glEnd();

	// Update the value of the cursor (required for accuracy when zoom changes, for example)
	// This is the value where the cursor meets its associated axis
	value = axis.GetMinimum() + double(locationAlongAxis
			- axis.GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) /
			(double)dimension * (axis.GetMaximum() - axis.GetMinimum());
}

//==========================================================================
// Class:			PlotCursor
// Function:		HasValidParameters
//
// Description:		Checks to see if we're OK to drawy the cursor.
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
bool PlotCursor::HasValidParameters(void)
{
	// Make sure the value is within the axis limits
	if (value >= axis.GetMinimum() && value <= axis.GetMaximum() &&
		axis.GetAxisAtMaxEnd() && axis.GetAxisAtMinEnd())
		return true;

	// If the parameters aren't valid, also hide this to prevent the cursor values from updating
	isVisible = false;

	return false;
}

//==========================================================================
// Class:			PlotCursor
// Function:		RescalePoint
//
// Description:		Rescales the onscreen position of the point according to
//					the size of the axis with which this object is associated.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		point	= unsigned int& specifying the location of the object in screen coordinates
//
// Return Value:
//		None
//
//==========================================================================
void PlotCursor::RescalePoint(unsigned int &point)
{
	if (!axis.GetAxisAtMaxEnd() || !axis.GetAxisAtMinEnd())
		return;
			
	int plotDimension;
	if (axis.IsHorizontal())
		plotDimension = renderWindow.GetSize().GetWidth()
				- axis.GetAxisAtMinEnd()->GetOffsetFromWindowEdge()
				- axis.GetAxisAtMaxEnd()->GetOffsetFromWindowEdge();
	else
		plotDimension = renderWindow.GetSize().GetHeight()
				- axis.GetAxisAtMinEnd()->GetOffsetFromWindowEdge()
				- axis.GetAxisAtMaxEnd()->GetOffsetFromWindowEdge();

	// Do the scaling
	point = axis.GetAxisAtMinEnd()->GetOffsetFromWindowEdge()
			+ (value - axis.GetMinimum()) / (axis.GetMaximum() - axis.GetMinimum()) * plotDimension;

	return;
}

//==========================================================================
// Class:			PlotCursor
// Function:		IsUnder
//
// Description:		Checks to see if the cursor is located under the specified
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
//==========================================================================
bool PlotCursor::IsUnder(const unsigned int &pixel)
{
	// Apparent line width for clicking
	int width = 2;// [pixels]

	if (abs(int(locationAlongAxis - pixel)) <= width && isVisible)
		return true;

	return false;
}

//==========================================================================
// Class:			PlotCursor
// Function:		SetValue
//
// Description:		Sets the x-value where the cursor should appear on the plot.
//
// Input Arguments:
//		_value	= const double& value to set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCursor::SetValue(const double& _value)
{
	value = _value;
	RescalePoint(locationAlongAxis);
	modified = true;

	return;
}

//==========================================================================
// Class:			PlotCursor
// Function:		operator=
//
// Description:		Sets the x-value where the cursor should appear on the plot.
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
//==========================================================================
PlotCursor& PlotCursor::operator=(const PlotCursor &target)
{
	if (&target == this)
		return *this;

	value = target.value;
	locationAlongAxis = target.locationAlongAxis;

	return *this;
}