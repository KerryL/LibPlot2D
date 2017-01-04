/*=============================================================================
                                   LibPlot2D
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
// Description:		Constructor for the PlotCursor class.
//
// Input Arguments:
//		mRenderWindow	= RenderWindow&
//		axis			= Axis& with which we are associated
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
PlotCursor::PlotCursor(RenderWindow &mRenderWindow, const Axis &axis)
	: Primitive(mRenderWindow), mAxis(axis), mLine(mRenderWindow)
{
	mIsVisible = false;
	mColor = Color::ColorBlack;
	mLine.SetLineColor(mColor);

	SetDrawOrder(2800);
}

//=============================================================================
// Class:			PlotCursor
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
void PlotCursor::Update(const unsigned int& /*i*/)
{
	if (mAxis.IsHorizontal())
	{
		mLine.Build(mLocationAlongAxis, mAxis.GetOffsetFromWindowEdge(),
			mLocationAlongAxis, mRenderWindow.GetSize().GetHeight()
			- mAxis.GetOppositeAxis()->GetOffsetFromWindowEdge(), mBufferInfo[0]);
	}
	else
	{
		mLine.Build(mAxis.GetOffsetFromWindowEdge(), mLocationAlongAxis,
			mRenderWindow.GetSize().GetWidth()
			- mAxis.GetOppositeAxis()->GetOffsetFromWindowEdge(),
			mLocationAlongAxis, mBufferInfo[0]);
	}

	// Update the value of the cursor (required for accuracy when zoom changes, for example)
	value = mAxis.PixelToValue(mLocationAlongAxis);
}

//=============================================================================
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
//=============================================================================
void PlotCursor::GenerateGeometry()
{
	if (mBufferInfo.size() == 0)
		return;

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	Line::DoPrettyDraw(mBufferInfo[0].indexBuffer.size());
	glBindVertexArray(0);
}

//=============================================================================
// Class:			PlotCursor
// Function:		HasValidParameters
//
// Description:		Checks to see if we're OK to draw the cursor.
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
	if (value >= mAxis.GetMinimum() && value <= mAxis.GetMaximum() &&
		mAxis.GetAxisAtMaxEnd() && mAxis.GetAxisAtMinEnd())
		return true;

	// If the parameters aren't valid, also hide this to prevent the cursor values from updating
	mIsVisible = false;

	return false;
}

//=============================================================================
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
//		true is the cursor is under the specified point, false otherwise
//
//=============================================================================
bool PlotCursor::IsUnder(const unsigned int &pixel)
{
	if (!mIsVisible)
		return false;

	// Apparent line width for clicking
	int width = 2;// [pixels]

	if (abs(int(mLocationAlongAxis - pixel)) <= width && mIsVisible)
		return true;

	return false;
}

//=============================================================================
// Class:			PlotCursor
// Function:		SetLocation
//
// Description:		Sets the x position where the cursor should appear on the plot.
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
	mLocationAlongAxis = location;
	value = mAxis.PixelToValue(location);
	mModified = true;
}

}// namespace LibPlot2D
