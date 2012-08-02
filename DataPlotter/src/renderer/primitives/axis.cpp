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
//	07/30/2012	- Added logarithmically-scalled plotting, K. Loux.

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

	logarithmic = false;

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

	// Handle logarithmically-scaled plots
	if (logarithmic)
	{
		numberOfTicks = ceil(log10(maximum)) - floor(log10(minimum)) - 1;
		numberOfGridLines = (numberOfTicks + 1) * 8 + numberOfTicks;
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
				if (logarithmic)
				{
					// Here, use gridSpacing as the axis value for the next gridline
					if (tick == 0)
					{
						// Find order of magnitude of minimum, then find smallest integer factor of that order greater than minimum
						gridSpacing = pow(10.0, floor(log10(minimum)));
						int scale(1);
						while (gridSpacing * scale <= minimum)
							scale++;
						gridSpacing *= scale;
					}
					else
					{
						// Add the next increment equal to the order we're within
						gridSpacing +=pow(10.0, floor(log10(gridSpacing)));

						// Fix rounding errors (this can actually lead to grid lines in erroneous locations during OOM calcs)
						double orderOfMagnitude(floor(log10(gridSpacing)));
						gridSpacing /= pow(10.0, orderOfMagnitude);
						gridSpacing = floor(gridSpacing + 0.5);
						gridSpacing *= pow(10.0, orderOfMagnitude);
					}

					if (gridSpacing >= maximum)
						break;

					glVertex2i(ValueToPixel(gridSpacing), offsetFromWindowEdge);
					glVertex2i(ValueToPixel(gridSpacing),
						renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge());
				}
				else
				{
					glVertex2i(minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing,
							offsetFromWindowEdge);
					glVertex2i(minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing,
						renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge());
				}
			}

			glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
		}

		// Draw the tick marks (if they're turned on)
		if (tickStyle != TickStyleNone)
		{
			// The first and last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
			for (tick = 1; tick <= numberOfTicks; tick++)
			{
				if (logarithmic)
				{
					unsigned int location = ValueToPixel(pow(10.0, floor(log10(minimum)) + tick));
					glVertex2i(location, mainAxisLocation - tickSize * outsideTick * sign);
					glVertex2i(location, mainAxisLocation + tickSize * insideTick * sign);
				}
				else
				{
					glVertex2i(minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing,
						mainAxisLocation - tickSize * outsideTick * sign);
					glVertex2i(minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing,
						mainAxisLocation + tickSize * insideTick * sign);
				}
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
				if (logarithmic)
				{
					// Here, use gridSpacing as the axis value for the next gridline
					if (tick == 0)
					{
						// Find order of magnitude of minimum, then find smallest integer factor of that order greater than minimum
						gridSpacing = pow(10.0, floor(log10(minimum)));
						int scale(1);
						while (gridSpacing * scale <= minimum)
							scale++;
						gridSpacing *= scale;
					}
					else
						// Add the next increment equal to the order we're within
						gridSpacing += pow(10.0, floor(log10(gridSpacing)));

					if (gridSpacing >= maximum)
						break;

					glVertex2i(offsetFromWindowEdge, ValueToPixel(gridSpacing));
					glVertex2i(renderWindow.GetSize().GetWidth() - oppositeAxis->GetOffsetFromWindowEdge(),
						ValueToPixel(gridSpacing));
				}
				else
				{
					glVertex2i(offsetFromWindowEdge, minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing);
					glVertex2i(renderWindow.GetSize().GetWidth() - oppositeAxis->GetOffsetFromWindowEdge(),
						minAxis->GetOffsetFromWindowEdge() + (tick + 1) * gridSpacing);
				}
			}

			glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
		}

		// Draw the tick marks (if they're turned on)
		if (tickStyle != TickStyleNone)
		{
			for (tick = 1; tick <= numberOfTicks; tick++)
			{
				if (logarithmic)
				{
					unsigned int location = ValueToPixel(pow(10.0, floor(log10(minimum)) + tick));
					glVertex2i(mainAxisLocation - tickSize * outsideTick * sign, location);
					glVertex2i(mainAxisLocation + tickSize * insideTick * sign, location);
				}
				else
				{
					glVertex2i(mainAxisLocation - tickSize * outsideTick * sign,
						minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing);
					glVertex2i(mainAxisLocation + tickSize * insideTick * sign,
						minAxis->GetOffsetFromWindowEdge() + tick * tickSpacing);
				}
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

		// Sometimes for log plots we need to re-reference the minimum after it goes to zero during rounding
		double originalMinimum(minimum);

		// Set the maximum and minimum to be exactly the values shown after rounding
		wxString limit;
		limit.Printf("%0.*f", precision, minimum);
		if (!limit.ToDouble(&minimum))
		{
			// Warn the user?
			// FIXME:  Warn the user
		}

		// Don't do this blindly for log plots - we can't have zero
		if (logarithmic)
		{
			while (minimum == 0.0)
			{
				precision++;
				limit.Printf("%0.*f", precision, originalMinimum);
				if (!limit.ToDouble(&minimum))
				{
					// Warn the user?
					// FIXME:  Warn the user
				}
			}
		}

		// Add the number values text
		double textValue;
		wxString valueLabel;
		double valueOffsetFromEdge = offsetFromWindowEdge * 0.8;
		for (tick = 0; tick < numberOfTicks + 2; tick++)
		{
			// Determine the label value
			if (logarithmic)
			{
				if (tick == 0)
					textValue = minimum;
				else if (tick == numberOfTicks + 1)
					textValue = maximum;
				else
					textValue = pow(10.0, floor(log10(minimum)) + tick);
			}
			else
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

					xTranslation = ValueToPixel(textValue) -
						(boundingBox.Upper().X() - boundingBox.Lower().X()) / 2.0;
				}
				else
				{
					if (orientation == OrientationLeft)
						xTranslation = valueOffsetFromEdge - boundingBox.Upper().X();
					else
						xTranslation = renderWindow.GetSize().GetWidth() - valueOffsetFromEdge;

					yTranslation = ValueToPixel(textValue) -
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

//==========================================================================
// Class:			Axis
// Function:		ValueToPixel
//
// Description:		Computes the pixel location along the axis that corresponds
//					to the specified plot-units value.
//
// Input Arguments:
//		value	= const double& containing the location of the point in plot
//				  coordinates
//
// Output Arguments:
//		None
//
// Return Value:
//		int specifying the location of the object in screen coordinates
//
//==========================================================================
int Axis::ValueToPixel(const double &value) const
{
	// Get the plot size
	int plotDimension;
	if (IsHorizontal())
		plotDimension = renderWindow.GetSize().GetWidth()
				- minAxis->GetOffsetFromWindowEdge()
				- maxAxis->GetOffsetFromWindowEdge();
	else
		plotDimension = renderWindow.GetSize().GetHeight()
				- minAxis->GetOffsetFromWindowEdge()
				- maxAxis->GetOffsetFromWindowEdge();

	// Do the scaling
	if (IsLogarithmic())
	{
		if (value <= 0.0 || minimum <= 0.0)
			return GetOffsetFromWindowEdge();
		else
			return minAxis->GetOffsetFromWindowEdge()
				+ (log10(value) - log10(minimum)) /
				(log10(maximum) - log10(minimum)) * plotDimension;
	}

	return minAxis->GetOffsetFromWindowEdge()
		+ (value - minimum) /
		(maximum - minimum) * plotDimension;
}

//==========================================================================
// Class:			Axis
// Function:		PixelToValue
//
// Description:		Computes the value (in plot-units) corresponding to the
//					specified on-screen pixel location.
//
// Input Arguments:
//		pixel	= const int& specifying desired screen-coordinate
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifing the corresponding value
//
//==========================================================================
double Axis::PixelToValue(const int &pixel) const
{
	// Get the plot size
	double fraction;
	if (IsHorizontal())
		fraction = double(pixel - minAxis->GetOffsetFromWindowEdge()) / double(renderWindow.GetSize().GetWidth()
				- minAxis->GetOffsetFromWindowEdge()
				- maxAxis->GetOffsetFromWindowEdge());
	else
		fraction = double(pixel - minAxis->GetOffsetFromWindowEdge()) / double(renderWindow.GetSize().GetHeight()
				- minAxis->GetOffsetFromWindowEdge()
				- maxAxis->GetOffsetFromWindowEdge());

	// Do the scaling
	if (IsLogarithmic())
		return pow(10.0, fraction * (log10(maximum) - log10(minimum)) + log10(minimum));

	return fraction * (maximum - minimum) + minimum;
}