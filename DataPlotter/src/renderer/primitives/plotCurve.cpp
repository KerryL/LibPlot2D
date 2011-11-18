/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_curve.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating data curves on a plot.
// History:
//	11/9/2010	- Modified to accomodate 3D plots, K. Loux.

// Local headers
#include "renderer/primitives/plot_curve.h"
#include "renderer/render_window_class.h"
#include "renderer/primitives/axis.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			PlotCurve
// Function:		PlotCurve
//
// Description:		Constructor for the PLOT_CURVE class.
//
// Input Arguments:
//		_RenderWindow	= RENDER_WINDOW* pointing to the object that owns this
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
	// FIXME:  Need to copy the PlotData?
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
// Function:		Constant Declarations
//
// Description:		Declare class constants for the PlotCurve class.
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
const int PlotCurve::offsetFromWindowEdge = 75;

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
	glBegin(GL_LINE_STRIP);

	// Create the plot
	unsigned int i;
	int point[2];
	double doubPoint[2];
	// FIXME:  If there are more points than pixels, we should avoid sending single pixel
	// line-draw commands to openGL
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
					double interpolatedPoint[2] = { data->GetXData(i - 1), data->GetYData(i - 1) };

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
				}
			}

			RescalePoint(doubPoint, point);
			glVertex2iv(point);
		}
		else
		{
			// If the next point in the series (or the previous point in the series) is within
			// the valid drawing area, interpolate on one end, otherwise, interpolate on both ends
			if (i > 0)
			{
				if (data->GetXData(i - 1) >= xAxis->GetMinimum() && data->GetXData(i - 1) <= xAxis->GetMaximum() &&
					data->GetYData(i - 1) >= yAxis->GetMinimum() && data->GetYData(i - 1) <= yAxis->GetMaximum())
				{
					double interpolatedPoint[2] = { doubPoint[0], doubPoint[1] };

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
					continue;
				}
			}
			if (i < data->GetNumberOfPoints() - 1)
			{
				if (data->GetXData(i + 1) >= xAxis->GetMinimum() && data->GetXData(i + 1) <= xAxis->GetMaximum() &&
					data->GetYData(i + 1) >= yAxis->GetMinimum() && data->GetYData(i + 1) <= yAxis->GetMaximum())
				{
					double interpolatedPoint[2] = { doubPoint[0], doubPoint[1] };

					// Interpolate to find the correct point
					if (interpolatedPoint[0] < xAxis->GetMinimum())
					{
						interpolatedPoint[0] = xAxis->GetMinimum();
						interpolatedPoint[1] = doubPoint[1] +
							(interpolatedPoint[0] - doubPoint[0]) / (data->GetXData(i + 1) - doubPoint[0]) *
							(data->GetYData(i + 1) - doubPoint[1]);
					}
					else if (interpolatedPoint[0] > xAxis->GetMaximum())
					{
						interpolatedPoint[0] = xAxis->GetMaximum();
						interpolatedPoint[1] = doubPoint[1] +
							(interpolatedPoint[0] - doubPoint[0]) / (data->GetXData(i + 1) - doubPoint[0]) *
							(data->GetYData(i + 1) - doubPoint[1]);
					}
					
					if (interpolatedPoint[1] < yAxis->GetMinimum())
					{
						interpolatedPoint[1] = yAxis->GetMinimum();
						interpolatedPoint[0] = doubPoint[0] +
							(interpolatedPoint[1] - doubPoint[1]) / (data->GetYData(i + 1) - doubPoint[1]) *
							(data->GetXData(i + 1) - doubPoint[0]);
					}
					else if (interpolatedPoint[1] > yAxis->GetMaximum())
					{
						interpolatedPoint[1] = yAxis->GetMaximum();
						interpolatedPoint[0] = doubPoint[0] +
							(interpolatedPoint[1] - doubPoint[1]) / (data->GetYData(i + 1) - doubPoint[1]) *
							(data->GetXData(i + 1) - doubPoint[0]);
					}

					RescalePoint(interpolatedPoint, point);
					glVertex2iv(point);
				}
			}
			// FIXME:  If the user zooms in too much, the plot disappears - there must be some way to
			// interpolate between just two points to draw a representative line
		}
	}

	glEnd();

	return;
}

//==========================================================================
// Class:			PlotCurve
// Function:		RescalePoint
//
// Description:		Rescales the onscreen position of the point according to
//					the size of the axis with which this object is associated.
//
// Input Arguments:
//		xyPoint	= const double* containing the location of the point in plot
//					  coordinates
//
// Output Arguments:
//		point	= int* specifying the location of the object in screen coordinates
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::RescalePoint(const double *xyPoint, int *point)
{
	if (!xyPoint || !point)
		return;

	// Get the plot size
	int plotHeight = renderWindow.GetSize().GetHeight() - 2 * offsetFromWindowEdge;
	int plotWidth = renderWindow.GetSize().GetWidth() - 2 * offsetFromWindowEdge;

	// Do the scaling
	point[0] = offsetFromWindowEdge + (xyPoint[0] - xAxis->GetMinimum()) /
		(xAxis->GetMaximum() - xAxis->GetMinimum()) * plotWidth;
	point[1] = offsetFromWindowEdge + (xyPoint[1]- yAxis->GetMinimum()) /
		(yAxis->GetMaximum() - yAxis->GetMinimum()) * plotHeight;

	return;
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
// Function:		operator =
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
PlotCurve& PlotCurve::operator = (const PlotCurve &plotCurve)
{
	// Check for self-assignment
	if (this == &plotCurve)
		return *this;

	// Copy the important information using the base class's assignment operator
	this->Primitive::operator=(plotCurve);

	// FIMXE:  Need to handle copy of PlotData?

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

	return;
}