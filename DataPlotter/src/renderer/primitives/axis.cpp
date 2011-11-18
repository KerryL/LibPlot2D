/*===================================================================================
                                    DataPlotter
                         Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  axis.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating axis objects on a plot.
// History:
//	11/17/2010	- Fixed some bugs related to rendering of ticks and grid lines, K. Loux.

// Local headers
#include "renderer/primitives/axis.h"
#include "renderer/renderWindow.h"
#include "utilities/math/plotMath.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			Axis
// Function:		Axis
//
// Description:		Constructor for the Axis class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Axis::Axis(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	color.Set(0.0, 0.0, 0.0, 1.0);

	orientation = OrientationBottom;
	tickStyle = TickStyleThrough;

	tickSize = 7;

	minimum = 0.0;
	maximum = 1.0;
	majorResolution = 1.0;
	minorResolution = 1.0;

	offsetFromWindowEdge = 75;// [pixels]

	grid = false;

	font = NULL;
	
	minAxis = NULL;
	maxAxis = NULL;
	oppositeAxis = NULL;

	gridColor.Set(0.8, 0.8, 0.8, 1.0);
}

//==========================================================================
// Class:			Axis
// Function:		~Axis
//
// Description:		Destructor for the Axis class.
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
Axis::~Axis()
{
}

//==========================================================================
// Class:			Axis
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
void Axis::GenerateGeometry(void)
{
	// Preliminary calculations
	int axisLength, tick;
	double outsideTick = 0.0, insideTick = 0.0;
	double tickSpacing;// To avoid rounding errors, we convert this back to an int after we do some math
	double gridSpacing;
	double sign = 1.0;
	int numberOfTicks = int((maximum - minimum) / minorResolution + 0.5) - 1;
	int numberOfGridLines = int((maximum - minimum) / majorResolution + 0.5) - 1;
	int mainAxisLocation;

	// Determine where the ticks should be drawn
	if (tickStyle == TickStyleInside)
		insideTick = 1.0;
	else if (tickStyle == TickStyleOutside)
		outsideTick = 1.0;
	else if (tickStyle == TickStyleThrough)
	{
		insideTick = 0.5;
		outsideTick = 0.5;
	}

	// Set the sign variable as necessary
	if (orientation == OrientationTop || orientation == OrientationRight)
		sign = -1.0;

	// Compute the mainAxisLocation (X for vertical axis, Y for horizontal axis)
	if (orientation == OrientationBottom || orientation == OrientationLeft)
		mainAxisLocation = offsetFromWindowEdge;
	else if (orientation == OrientationRight)
		mainAxisLocation = renderWindow.GetSize().GetWidth() - offsetFromWindowEdge;
	else// OrientationTop
		mainAxisLocation = renderWindow.GetSize().GetHeight() - offsetFromWindowEdge;

	// Set the line width
	glLineWidth(1.0f);

	// Create the axis
	glBegin(GL_LINES);

	// Draw the axis
	if (IsHorizontal())
	{
		axisLength = renderWindow.GetSize().GetWidth() -
				minAxis->GetOffsetFromWindowEdge() - maxAxis->GetOffsetFromWindowEdge();
		glVertex2i(minAxis->GetOffsetFromWindowEdge(), mainAxisLocation);
		glVertex2i(renderWindow.GetSize().GetWidth()
				- maxAxis->GetOffsetFromWindowEdge(), mainAxisLocation);
	}
	else
	{
		axisLength = renderWindow.GetSize().GetHeight() -
				minAxis->GetOffsetFromWindowEdge() - maxAxis->GetOffsetFromWindowEdge();
		glVertex2i(mainAxisLocation, minAxis->GetOffsetFromWindowEdge());
		glVertex2i(mainAxisLocation, renderWindow.GetSize().GetHeight() -
				maxAxis->GetOffsetFromWindowEdge());
	}

	// Compute the spacing for grids and ticks
	gridSpacing = (double)axisLength / double(numberOfGridLines + 1);
	tickSpacing = (double)axisLength / double(numberOfTicks + 1);

	// Draw the grids and tick marks
	if (IsHorizontal())
	{
		// Draw the grid (and check to make sure the opposite axis pointer was provided)
		if (grid && oppositeAxis)
		{
			glColor4d(gridColor.GetRed(), gridColor.GetGreen(), gridColor.GetBlue(), gridColor.GetAlpha());

			for (tick = 0; tick < numberOfGridLines; tick++)
			{
				glVertex2i(minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing,
						offsetFromWindowEdge);
				glVertex2i(minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing,
					renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge());
			}

			glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
		}

		// Draw the tick marks (if they're turned on)
		if (tickStyle != TickStyleNone)
		{
			// The first and last inside ticks do not need to be drawn, thus we start this loop with Tick = 1.
			for (tick = 1; tick <= numberOfTicks; tick++)
			{
				glVertex2i(minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing,
						mainAxisLocation - tickSize * outsideTick * sign);
				glVertex2i(minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing,
						mainAxisLocation + tickSize * insideTick * sign);
			}
		}
	}
	else
	{
		// Draw the grid
		if (grid && oppositeAxis)
		{
			glColor4d(gridColor.GetRed(), gridColor.GetGreen(), gridColor.GetBlue(), gridColor.GetAlpha());

			for (tick = 0; tick < numberOfGridLines; tick++)
			{
				glVertex2i(offsetFromWindowEdge, minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing);
				glVertex2i(renderWindow.GetSize().GetWidth() - oppositeAxis->GetOffsetFromWindowEdge(),
					minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing);
			}

			glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
		}

		// Draw the tick marks (if they're turned on)
		if (tickStyle != TickStyleNone)
		{
			for (tick = 1; tick <= numberOfTicks; tick++)
			{
				glVertex2i(mainAxisLocation - tickSize * outsideTick * sign,
						minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing);
				glVertex2i(mainAxisLocation + tickSize * insideTick * sign,
						minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing);
			}
		}
	}

	// Complete the axis
	glEnd();

	// Add the text
	// FIXME:  Should add something here to center text over the plot area, not the render window
	if (font)
	{
		FTBBox boundingBox;
		double yTranslation, xTranslation;

		// Add the label text
		if (!label.IsEmpty())
		{
			double fontOffsetFromWindowEdge = offsetFromWindowEdge / 3.0;

			// Vertical axes need more space for the numbers
			if (!IsHorizontal())
				fontOffsetFromWindowEdge /= 2.0;

			// FIXME:  Change plot dimension if there is a title? or if there is a title and a label for the top axis?

			// Get the bounding box
			boundingBox = font->BBox("H");// Some capital letter to assure uniform spacing

			switch (orientation)
			{
			case OrientationBottom:
				yTranslation = fontOffsetFromWindowEdge;
				break;

			case OrientationLeft:
				yTranslation = fontOffsetFromWindowEdge + boundingBox.Upper().Y();
				break;

			case OrientationTop:
				yTranslation = renderWindow.GetSize().GetHeight() - fontOffsetFromWindowEdge - boundingBox.Upper().Y();
				break;

			case OrientationRight:
				yTranslation = renderWindow.GetSize().GetWidth() - fontOffsetFromWindowEdge;
				break;

			default:
				assert(0);
				yTranslation = 0.0;// To avoid MSVC++ compiler warning C4701
				break;
			}

			glPushMatrix();
				glLoadIdentity();

				// Get the real bounding box
				boundingBox = font->BBox(label.c_str());

				if (IsHorizontal())
					glTranslated((renderWindow.GetSize().GetWidth() + boundingBox.Lower().X()
						- boundingBox.Upper().X()) / 2.0, yTranslation, 0.0);
				else
				{
					glRotated(90.0, 0.0, 0.0, 1.0);
					glTranslated((boundingBox.Lower().X() - boundingBox.Upper().X()
						+ renderWindow.GetSize().GetHeight()) / 2.0, -yTranslation, 0.0);
				}

				font->Render(label.c_str());
			glPopMatrix();
		}

		// Determine the precision to use for displaying the numbers
		// Use just enough precision so that two adjacent ticks are distinguishable
		// NOTE:  Changed 5/7/2011 to show 3 significant digits.  Fixes problem with apparent
		// accuracy of plot during panning (plot moves, grid lines don't, value appears to be wrong)
		unsigned int precision;
		if (log10(majorResolution) >= 0.0)
			precision = 0;
		else
			precision = -log10(majorResolution) + 1;
		precision += 2;// This is the change from 5/7/2011

		// Set the maximum and minimum to be exactly the values show after rounding
		wxString limit;
		limit.Printf("%0.*f", precision, minimum);
		if (!limit.ToDouble(&minimum))
		{
			// Warn the user?
			// FIXME:  Warn the user
		}

		// Add the number values text
		double textValue;
		wxString valueLabel;
		double valueOffsetFromEdge = offsetFromWindowEdge * 0.8;
		for (tick = 0; tick < numberOfGridLines + 2; tick++)
		{
			// Determine the label value
			textValue = minimum + (double)tick * majorResolution;

			// Assign the value to the string
			valueLabel.Printf("%0.*f", precision, textValue);

			glPushMatrix();
				glLoadIdentity();

				// Get the real bounding box
				boundingBox = font->BBox(valueLabel);

				if (IsHorizontal())
				{
					if (orientation == OrientationBottom)
						yTranslation = valueOffsetFromEdge - boundingBox.Upper().Y();
					else
						yTranslation = renderWindow.GetSize().GetHeight() - valueOffsetFromEdge;

					xTranslation = minAxis->GetOffsetFromWindowEdge() + tick * gridSpacing -
						(boundingBox.Upper().X() - boundingBox.Lower().X()) / 2.0;
				}
				else
				{
					if (orientation == OrientationLeft)
						xTranslation = valueOffsetFromEdge - boundingBox.Upper().X();
					else
						xTranslation = renderWindow.GetSize().GetWidth() - valueOffsetFromEdge;

					yTranslation = minAxis->GetOffsetFromWindowEdge() + tick * gridSpacing -
						(boundingBox.Upper().Y() - boundingBox.Lower().Y()) / 2.0;
				}

				glTranslated(xTranslation, yTranslation, 0.0);

				font->Render(valueLabel);
			glPopMatrix();
		}

		if (!valueLabel.ToDouble(&maximum))
		{
			// Warn the user?
			// FIXME:  Warn the user
		}
	}
}

//==========================================================================
// Class:			Axis
// Function:		IsHorizontal
//
// Description:		Checks to see if this object has horizontal orientation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for horizontal, false for vertical
//
//==========================================================================
bool Axis::IsHorizontal(void) const
{
	if (orientation == OrientationBottom || orientation == OrientationTop)
		return true;

	return false;
}

//==========================================================================
// Class:			Axis
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
bool Axis::HasValidParameters(void)
{
	// Don't draw if any of the limits are not numbers
	if (PlotMath::IsNaN(minimum) || PlotMath::IsNaN(maximum))
		return false;
	
	// Make sure the pointers to the perpendicular axes have been provided
	if (!minAxis || !maxAxis)
		return false;

	return true;
}