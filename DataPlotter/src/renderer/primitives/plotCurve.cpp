/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCurve.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating data curves on a plot.
// History:
//	11/9/2010	- Modified to accomodate 3D plots, K. Loux.

// Local headers
#include "renderer/primitives/plotCurve.h"
#include "renderer/renderWindow.h"
#include "renderer/primitives/axis.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			PlotCurve
// Function:		PlotCurve
//
// Description:		Constructor for the PLOT_CURVE class.
//
// Input Arguments:
//		_RenderWindow	= RenderWindow* pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotCurve::PlotCurve(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	xAxis = NULL;
	yAxis = NULL;

	size = 1;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotCurve
//
// Description:		Copy constructor for the PlotCurve class.
//
// Input Arguments:
//		plotCurve	= const PlotCurve& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotCurve::PlotCurve(const PlotCurve &plotCurve) : Primitive(plotCurve)
{
	// Do the copy
	*this = plotCurve;
}

//==========================================================================
// Class:			PlotCurve
// Function:		~PlotCurve
//
// Description:		Destructor for the PlotCurve class.
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
PlotCurve::~PlotCurve()
{
}

//==========================================================================
// Class:			PlotCurve
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
void PlotCurve::GenerateGeometry(void)
{
	// Set the line width
	glLineWidth((float)size);

	// Create the plot
	unsigned int i;
	int point[2];
	double doubPoint[2];
	double interpolatedPoint[2];
	double interpolatedPoint2[2];
	double temp[2];

	glBegin(GL_LINE_STRIP);

	// FIXME:  If there are more points than pixels, we should avoid sending single pixel
	// line-draw commands to OpenGL (doesn't seem to affect quality, but may affect performance - of course, if we
	// don't do this intelligently it'll end up being better just to let OpenGL deal with this)
	for (i = 0; i < data->GetNumberOfPoints(); i++)
	{
		doubPoint[0] = data->GetXData(i);
		doubPoint[1] = data->GetYData(i);

		// Clip data that lies outside of the plot range and interpolate to the edge of the plot
		// No interpolation necessary
		if (doubPoint[0] >= xAxis->GetMinimum() && doubPoint[0] <= xAxis->GetMaximum() &&
			doubPoint[1] >= yAxis->GetMinimum() && doubPoint[1] <= yAxis->GetMaximum())
		{
			// If the previous point was interpolated, then we need to add an additional point to make the plot draw correctly
			if (i > 0)
			{
				if (data->GetXData(i - 1) < xAxis->GetMinimum() || data->GetXData(i - 1) > xAxis->GetMaximum() ||
					data->GetYData(i - 1) < yAxis->GetMinimum() || data->GetYData(i - 1) > yAxis->GetMaximum())
				{
					interpolatedPoint[0] = data->GetXData(i - 1);
					interpolatedPoint[1] = data->GetYData(i - 1);

					// FIXME:  When we need to interpolate (if a point is off the screen) on a logarithmic-scaled axis, the current code is wrong (plot gets distorted)

					// Interpolate to find the correct point
					if (interpolatedPoint[0] < xAxis->GetMinimum())
					{
						interpolatedPoint[0] = xAxis->GetMinimum();
						interpolatedPoint[1] = doubPoint[1] +
							(interpolatedPoint[0] - doubPoint[0]) / (data->GetXData(i - 1) - doubPoint[0]) *
							(data->GetYData(i - 1) - doubPoint[1]);
					}
					else if (interpolatedPoint[0] > xAxis->GetMaximum())
					{
						interpolatedPoint[0] = xAxis->GetMaximum();
						interpolatedPoint[1] = doubPoint[1] +
							(interpolatedPoint[0] - doubPoint[0]) / (data->GetXData(i - 1) - doubPoint[0]) *
							(data->GetYData(i - 1) - doubPoint[1]);
					}

					if (interpolatedPoint[1] < yAxis->GetMinimum())
					{
						interpolatedPoint[1] = yAxis->GetMinimum();
						interpolatedPoint[0] = doubPoint[0] +
							(interpolatedPoint[1] - doubPoint[1]) / (data->GetYData(i - 1) - doubPoint[1]) *
							(data->GetXData(i - 1) - doubPoint[0]);
					}
					else if (interpolatedPoint[1] > yAxis->GetMaximum())
					{
						interpolatedPoint[1] = yAxis->GetMaximum();
						interpolatedPoint[0] = doubPoint[0] +
							(interpolatedPoint[1] - doubPoint[1]) / (data->GetYData(i - 1) - doubPoint[1]) *
							(data->GetXData(i - 1) - doubPoint[0]);
					}

					glBegin(GL_LINE_STRIP);
					RescalePoint(interpolatedPoint, point);
					glVertex2iv(point);
				}
			}

			RescalePoint(doubPoint, point);
			glVertex2iv(point);
		}
		else// Outside the plot area
		{
			// If the next point in the series is within the valid drawing area, interpolate
			if (i > 0)
			{
				// Check previous point
				if (data->GetXData(i - 1) >= xAxis->GetMinimum() && data->GetXData(i - 1) <= xAxis->GetMaximum() &&
					data->GetYData(i - 1) >= yAxis->GetMinimum() && data->GetYData(i - 1) <= yAxis->GetMaximum())
				{
					interpolatedPoint[0] = doubPoint[0];
					interpolatedPoint[1] = doubPoint[1];

					// FIXME:  When we need to interpolate (if a point is off the screen) on a logarithmic-scaled axis, the current code is wrong (plot gets distorted)

					// Interpolate to find the correct point
					if (interpolatedPoint[0] < xAxis->GetMinimum())
					{
						interpolatedPoint[0] = xAxis->GetMinimum();
						interpolatedPoint[1] = doubPoint[1] +
							(interpolatedPoint[0] - doubPoint[0]) / (data->GetXData(i - 1) - doubPoint[0]) *
							(data->GetYData(i - 1) - doubPoint[1]);
					}
					else if (interpolatedPoint[0] > xAxis->GetMaximum())
					{
						interpolatedPoint[0] = xAxis->GetMaximum();
						interpolatedPoint[1] = doubPoint[1] +
							(interpolatedPoint[0] - doubPoint[0]) / (data->GetXData(i - 1) - doubPoint[0]) *
							(data->GetYData(i - 1) - doubPoint[1]);
					}

					if (interpolatedPoint[1] < yAxis->GetMinimum())
					{
						interpolatedPoint[1] = yAxis->GetMinimum();
						interpolatedPoint[0] = doubPoint[0] +
							(interpolatedPoint[1] - doubPoint[1]) / (data->GetYData(i - 1) - doubPoint[1]) *
							(data->GetXData(i - 1) - doubPoint[0]);
					}
					else if (interpolatedPoint[1] > yAxis->GetMaximum())
					{
						interpolatedPoint[1] = yAxis->GetMaximum();
						interpolatedPoint[0] = doubPoint[0] +
							(interpolatedPoint[1] - doubPoint[1]) / (data->GetYData(i - 1) - doubPoint[1]) *
							(data->GetXData(i - 1) - doubPoint[0]);
					}

					RescalePoint(interpolatedPoint, point);
					glVertex2iv(point);
					glEnd();
					continue;
				}
			}

			// If we get here, the current point and the previous point were outside the plot area
			// were all outside the plot area

			// Check to see if we've "jumped" the plot area - if so, interpolate between
			// two points to draw a straight line on the screen
			if (i > 0)
			{
				// Check for plot "jumping" - defined by a stright line between two points crossing
				// any two plot axes within the range of the axes
				unsigned int crossings(0);

				// Left Y-Axis
				if ((data->GetXData(i - 1) < xAxis->GetMinimum() && doubPoint[0] > xAxis->GetMinimum()) ||// Crossed from left-to-right
					(data->GetXData(i - 1) > xAxis->GetMinimum() && doubPoint[0] < xAxis->GetMinimum()))// Crossed from right-to-left
				{
					// Find the interpolated point
					interpolatedPoint[0] = xAxis->GetMinimum();
					interpolatedPoint[1] = data->GetYData(i - 1) + (doubPoint[1] - data->GetYData(i - 1)) /
						(doubPoint[0] - data->GetXData(i - 1)) * (interpolatedPoint[0] - data->GetXData(i - 1));

					// Check that the interpolated point is within the axis limits
					if (interpolatedPoint[1] >= yAxis->GetMinimum() && interpolatedPoint[1] <= yAxis->GetMaximum())
						crossings++;
				}

				// Right Y-Axis
				if ((data->GetXData(i - 1) < xAxis->GetMaximum() && doubPoint[0] > xAxis->GetMaximum()) ||// Crossed from left-to-right
					(data->GetXData(i - 1) > xAxis->GetMaximum() && doubPoint[0] < xAxis->GetMaximum()))// Crossed from right-to-left
				{
					// Find the interpolated point
					if (crossings == 0)
					{
						interpolatedPoint[0] = xAxis->GetMaximum();
						interpolatedPoint[1] = data->GetYData(i - 1) + (doubPoint[1] - data->GetYData(i - 1)) /
							(doubPoint[0] - data->GetXData(i - 1)) * (interpolatedPoint[0] - data->GetXData(i - 1));

						// Check that the interpolated point is within the axis limits
						if (interpolatedPoint[1] >= yAxis->GetMinimum() && interpolatedPoint[1] <= yAxis->GetMaximum())
							crossings++;
					}
					else
					{
						temp[0] = xAxis->GetMaximum();
						temp[1] = data->GetYData(i - 1) + (doubPoint[1] - data->GetYData(i - 1)) /
							(doubPoint[0] - data->GetXData(i - 1)) * (temp[0] - data->GetXData(i - 1));

						// Check that the interpolated point is within the axis limits
						if (temp[1] >= yAxis->GetMinimum() && temp[1] <= yAxis->GetMaximum())
						{
							crossings++;
							interpolatedPoint2[0] = temp[0];
							interpolatedPoint2[1] = temp[1];
						}
					}
				}

				// Bottom X-Axis
				if ((data->GetYData(i - 1) < yAxis->GetMinimum() && doubPoint[1] > yAxis->GetMinimum()) ||// Crossed from bottom-to-top
					(data->GetYData(i - 1) > yAxis->GetMinimum() && doubPoint[1] < yAxis->GetMinimum()))// Crossed from top-to-bottom
				{
					// Find the interpolated point
					if (crossings == 0)
					{
						interpolatedPoint[1] = yAxis->GetMinimum();
						interpolatedPoint[0] = data->GetXData(i - 1) + (doubPoint[0] - data->GetXData(i - 1)) /
							(doubPoint[1] - data->GetYData(i - 1)) * (interpolatedPoint[1] - data->GetYData(i - 1));

						// Check that the interpolated point is within the axis limits
						if (interpolatedPoint[0] >= xAxis->GetMinimum() && interpolatedPoint[0] <= xAxis->GetMaximum())
							crossings++;
					}
					else
					{
						temp[1] = yAxis->GetMinimum();
						temp[0] = data->GetXData(i - 1) + (doubPoint[0] - data->GetXData(i - 1)) /
							(doubPoint[1] - data->GetYData(i - 1)) * (temp[1] - data->GetYData(i - 1));

						// Check that the interpolated point is within the axis limits
						if (temp[0] >= xAxis->GetMinimum() && temp[0] <= xAxis->GetMaximum())
						{
							crossings++;
							interpolatedPoint2[0] = temp[0];
							interpolatedPoint2[1] = temp[1];
						}
					}
				}

				// Top X-Axis
				if ((data->GetYData(i - 1) < yAxis->GetMaximum() && doubPoint[1] > yAxis->GetMaximum()) ||// Crossed from bottom-to-top
					(data->GetYData(i - 1) > yAxis->GetMaximum() && doubPoint[1] < yAxis->GetMaximum()))// Crossed from top-to-bottom
				{
					// Find the interpolated point
					if (crossings == 0)
					{
						interpolatedPoint[1] = yAxis->GetMaximum();
						interpolatedPoint[0] = data->GetXData(i - 1) + (doubPoint[0] - data->GetXData(i - 1)) /
							(doubPoint[1] - data->GetYData(i - 1)) * (interpolatedPoint[1] - data->GetYData(i - 1));

						// Check that the interpolated point is within the axis limits
						if (interpolatedPoint[0] >= xAxis->GetMinimum() && interpolatedPoint[0] <= xAxis->GetMaximum())
							crossings++;
					}
					else
					{
						temp[1] = yAxis->GetMaximum();
						temp[0] = data->GetXData(i - 1) + (doubPoint[0] - data->GetXData(i - 1)) /
							(doubPoint[1] - data->GetYData(i - 1)) * (temp[1] - data->GetYData(i - 1));

						// Check that the interpolated point is within the axis limits
						if (temp[0] >= xAxis->GetMinimum() && temp[0] <= xAxis->GetMaximum())
						{
							crossings++;
							interpolatedPoint2[0] = temp[0];
							interpolatedPoint2[1] = temp[1];
						}
					}
				}

				// Impossible to have a number of crossings other than zero or two
				assert(crossings == 0 || crossings == 2);

				// If we have two crossings, we have a jump
				if (crossings == 2)
				{
					// Depending on where the previously drawn point was, it is possible for the order of these
					// points to be opposite from the proper order of "connecting the dots."
					glEnd();
					glBegin(GL_LINE_STRIP);

					// Connect the points
					RescalePoint(interpolatedPoint, point);
					glVertex2iv(point);

					RescalePoint(interpolatedPoint2, point);
					glVertex2iv(point);

					glEnd();
					glBegin(GL_LINE_STRIP);
				}
			}
		}
	}

	glEnd();
}

//==========================================================================
// Class:			PlotCurve
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
bool PlotCurve::HasValidParameters(void)
{
	if (xAxis != NULL && yAxis != NULL && data->GetNumberOfPoints() > 1)
	{
		if (xAxis->IsHorizontal() && !yAxis->IsHorizontal())
			return true;
	}

	return false;
}

//==========================================================================
// Class:			PlotCurve
// Function:		operator=
//
// Description:		Assignment operator for PLOT_CURVE class.
//
// Input Arguments:
//		plotCurve	= const PlotCurve& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		PLOT_CURVE&, reference to this object
//
//==========================================================================
PlotCurve& PlotCurve::operator=(const PlotCurve &plotCurve)
{
	// Check for self-assignment
	if (this == &plotCurve)
		return *this;

	// Copy the important information using the base class's assignment operator
	this->Primitive::operator=(plotCurve);

	return *this;
}

//==========================================================================
// Class:			PlotCurve
// Function:		SetData
//
// Description:		Assigns data to the curve.
//
// Input Arguments:
//		_data	= const Dataset2D* to plot
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::SetData(const Dataset2D *_data)
{
	data = _data;
	modified = true;
}

//==========================================================================
// Class:			PlotCurve
// Function:		RescalePoint
//
// Description:		Rescales the onscreen position of the point according to
//					the size of the axis with which this object is associated.
//
// Input Arguments:
//		value	= const double* containing the location of the point in plot
//				  coordinates
//
// Output Arguments:
//		coordinate	= int* specifying the location of the object in screen coordinates
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::RescalePoint(const double *value, int *coordinate) const
{
	if (!value || !coordinate)
		return;

	coordinate[0] = xAxis->ValueToPixel(value[0]);
	coordinate[1] = yAxis->ValueToPixel(value[1]);
}