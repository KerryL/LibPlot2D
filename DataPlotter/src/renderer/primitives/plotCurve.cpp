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
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			PlotCurve
// Function:		PlotCurve
//
// Description:		Constructor for the PlotCurve class.
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

	lineSize = 1;
	markerSize = -1;
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
	if (lineSize > 0)
	{
		glLineWidth((float)lineSize);
		glBegin(GL_LINE_STRIP);

		unsigned int i;
		for (i = 0; i < data->GetNumberOfPoints(); i++)
		{
			if (PointIsWithinPlotArea(i))
			{
				if (i > 0 && !PointIsWithinPlotArea(i - 1))
					PlotInterpolatedPoint(i - 1, i, true);
				PlotPoint(i);
			}
			else if (i > 0 && PointIsWithinPlotArea(i - 1))
				PlotInterpolatedPoint(i - 1, i, false);
			else if (i > 0 && PointsJumpPlotArea(i - 1, i))
				PlotInterpolatedJumpPoints(i - 1, i);
		}

		glEnd();
	}

	if (markerSize > 0 || (markerSize < 0 && SmallRange()))
	{
		glBegin(GL_QUADS);
		PlotMarkers();
		glEnd();
	}
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointIsWithinPlotArea
//
// Description:		Checks to see if the point with the specified index is
//					within the plot area.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for points within the plot area, false otherwise
//
//==========================================================================
bool PlotCurve::PointIsWithinPlotArea(const unsigned int &i) const
{
	if (data->GetXData(i) >= xAxis->GetMinimum() &&
		data->GetXData(i) <= xAxis->GetMaximum() &&
		data->GetYData(i) >= yAxis->GetMinimum() &&
		data->GetYData(i) <= yAxis->GetMaximum())
		return true;

	return false;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotPoint
//
// Description:		Plots the coordinate with the specified data index.
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
//==========================================================================
void PlotCurve::PlotPoint(const unsigned int &i) const
{
	PlotPoint(data->GetXData(i), data->GetYData(i));
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotPoint
//
// Description:		Plots the coordinate with the specified coordinates.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::PlotPoint(const double &x, const double &y) const
{
	double doublePoint[2] = {x, y};
	int point[2];

	RescalePoint(doublePoint, point);
	glVertex2iv(point);
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotPoint
//
// Description:		Plots the coordinate where the line between the specified
//					indecies crosses out of/in to the plot area.
//
// Input Arguments:
//		first			= const unsigned int&
//		second			= const unsigned int&
//		startingPoint	= const bool& indicates whether this point is a
//						  continuation of an existing line strip, or if we
//						  are starting a new line strip
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::PlotInterpolatedPoint(const unsigned int &first, const unsigned int &second, const bool &startingPoint) const
{
	if (startingPoint)
	{
		glEnd();
		glBegin(GL_LINE_STRIP);
	}

	if (PointIsValid(first) && PointIsValid(second))
	{
		if (PointsCrossBottomAxis(first, second))
			PlotPoint(GetInterpolatedXOrdinate(first, second, yAxis->GetMinimum()), yAxis->GetMinimum());
		else if (PointsCrossTopAxis(first, second))
			PlotPoint(GetInterpolatedXOrdinate(first, second, yAxis->GetMaximum()), yAxis->GetMaximum());
		else if (PointsCrossLeftAxis(first, second))
			PlotPoint(xAxis->GetMinimum(), GetInterpolatedYOrdinate(first, second, xAxis->GetMinimum()));
		else if (PointsCrossRightAxis(first, second))
			PlotPoint(xAxis->GetMaximum(), GetInterpolatedYOrdinate(first, second, xAxis->GetMaximum()));
		else
			assert(false);
	}

	if (!startingPoint)
	{
		glEnd();
		glBegin(GL_LINE_STRIP);
	}
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotInterpolatedJumpPoints
//
// Description:		Plots the coordinate where the line between the specified
//					indecies crosses out of/in to the plot area.  It is assumed
//					that exactly two of the if clauses will evaluate true.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::PlotInterpolatedJumpPoints(const unsigned int &first, const unsigned int &second) const
{
	glEnd();

	if (PointIsValid(first) && PointIsValid(second))
	{
		glBegin(GL_LINE_STRIP);

		if (PointsCrossBottomAxis(first, second))
			PlotPoint(GetInterpolatedXOrdinate(first, second, yAxis->GetMinimum()), yAxis->GetMinimum());
		if (PointsCrossTopAxis(first, second))
			PlotPoint(GetInterpolatedXOrdinate(first, second, yAxis->GetMaximum()), yAxis->GetMaximum());
		if (PointsCrossLeftAxis(first, second))
			PlotPoint(xAxis->GetMinimum(), GetInterpolatedYOrdinate(first, second, xAxis->GetMinimum()));
		if (PointsCrossRightAxis(first, second))
			PlotPoint(xAxis->GetMaximum(), GetInterpolatedYOrdinate(first, second, xAxis->GetMaximum()));

		glEnd();
	}
	glBegin(GL_LINE_STRIP);
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointIsValid
//
// Description:		Checks to see if the specified point is a real, finite number.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for valid, false otherwise
//
//==========================================================================
bool PlotCurve::PointIsValid(const unsigned int &i) const
{
	assert(i < data->GetNumberOfPoints());

	return PlotMath::IsValid<double>(data->GetXData(i)) &&
		PlotMath::IsValid<double>(data->GetYData(i));
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
// Description:		Assignment operator for PlotCurve class.
//
// Input Arguments:
//		plotCurve	= const PlotCurve& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCurve&, reference to this object
//
//==========================================================================
PlotCurve& PlotCurve::operator=(const PlotCurve &plotCurve)
{
	if (this == &plotCurve)
		return *this;

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

//==========================================================================
// Class:			PlotCurve
// Function:		PointsCrossBottomAxis
//
// Description:		Determines whether or not the specified points span the
//					bottom axis.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for crossing, false otherwise
//
//==========================================================================
bool PlotCurve::PointsCrossBottomAxis(const unsigned int &first, const unsigned int &second) const
{
	if (!PointsCrossYOrdinate(first, second, yAxis->GetMinimum()))
		return false;

	double crossing = GetInterpolatedXOrdinate(first, second, yAxis->GetMinimum());
	if (crossing < xAxis->GetMinimum() || crossing > xAxis->GetMaximum())
		return false;

	return true;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointsCrossTopAxis
//
// Description:		Determines whether or not the specified points span the
//					top axis.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for crossing, false otherwise
//
//==========================================================================
bool PlotCurve::PointsCrossTopAxis(const unsigned int &first, const unsigned int &second) const
{
	if (!PointsCrossYOrdinate(first, second, yAxis->GetMaximum()))
		return false;

	double crossing = GetInterpolatedXOrdinate(first, second, yAxis->GetMaximum());
	if (crossing < xAxis->GetMinimum() || crossing > xAxis->GetMaximum())
		return false;

	return true;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointsCrossLeftAxis
//
// Description:		Determines whether or not the specified points span the
//					left axis.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for crossing, false otherwise
//
//==========================================================================
bool PlotCurve::PointsCrossLeftAxis(const unsigned int &first, const unsigned int &second) const
{
	if (!PointsCrossXOrdinate(first, second, xAxis->GetMinimum()))
		return false;

	double crossing = GetInterpolatedYOrdinate(first, second, xAxis->GetMinimum());
	if (crossing < yAxis->GetMinimum() || crossing > yAxis->GetMaximum())
		return false;

	return true;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointsCrossRightAxis
//
// Description:		Determines whether or not the specified points span the
//					right axis.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for crossing, false otherwise
//
//==========================================================================
bool PlotCurve::PointsCrossRightAxis(const unsigned int &first, const unsigned int &second) const
{
	if (!PointsCrossXOrdinate(first, second, xAxis->GetMaximum()))
		return false;

	double crossing = GetInterpolatedYOrdinate(first, second, xAxis->GetMaximum());
	if (crossing < yAxis->GetMinimum() || crossing > yAxis->GetMaximum())
		return false;

	return true;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointsCrossXOrdinate
//
// Description:		Determines whether or not the specified points span the
//					specified x-value.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for crossing, false otherwise
//
//==========================================================================
bool PlotCurve::PointsCrossXOrdinate(const unsigned int &first, const unsigned int &second, const double &value) const
{
	if ((data->GetXData(first) <= value && data->GetXData(second) >= value) ||
		(data->GetXData(first) >= value && data->GetXData(second) <= value))
		return true;

	return false;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointsCrossYOrdinate
//
// Description:		Determines whether or not the specified points span the
//					specified y-value.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for crossing, false otherwise
//
//==========================================================================
bool PlotCurve::PointsCrossYOrdinate(const unsigned int &first, const unsigned int &second, const double &value) const
{
	if ((data->GetYData(first) <= value && data->GetYData(second) >= value) ||
		(data->GetYData(first) >= value && data->GetYData(second) <= value))
		return true;

	return false;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointsJumpPlotArea
//
// Description:		Determines whether or not the specified points result in
//					a line through the plot area without either point lying
//					inside the plot area.  This assumes that neither point is
//					within the plot area (must have been previously determined).
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for jumping, false otherwise
//
//==========================================================================
bool PlotCurve::PointsJumpPlotArea(const unsigned int &first, const unsigned int &second) const
{
	unsigned int crossings(0);
	crossings += (unsigned int)PointsCrossBottomAxis(first, second);
	crossings += (unsigned int)PointsCrossTopAxis(first, second);
	crossings += (unsigned int)PointsCrossLeftAxis(first, second);
	crossings += (unsigned int)PointsCrossRightAxis(first, second);

	assert(crossings == 0 || crossings == 2);

	return crossings == 2;
}

//==========================================================================
// Class:			PlotCurve
// Function:		GetInterpolatedXOrdinate
//
// Description:		Interpolates to find the x-value most closesly matching
//					the specified y-value.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//		yValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotCurve::GetInterpolatedXOrdinate(const unsigned int &first,
	const unsigned int &second, const double &yValue) const
{
	double fraction;
	if (yAxis->IsLogarithmic())
		fraction = (log10(yValue) - log10(data->GetYData(first))) / (log10(data->GetYData(second)) - log10(data->GetYData(first)));
	else
		fraction = (yValue - data->GetYData(first)) / (data->GetYData(second) - data->GetYData(first));

	if (PlotMath::IsNaN(fraction))
		fraction = 1.0;

	if (xAxis->IsLogarithmic())
		return pow(data->GetXData(second), fraction) * pow(data->GetXData(first), 1.0 - fraction);
	return data->GetXData(first) + (data->GetXData(second) - data->GetXData(first)) * fraction;
}

//==========================================================================
// Class:			PlotCurve
// Function:		GetInterpolatedYOrdinate
//
// Description:		Interpolates to find the y-value most closesly matching
//					the specified x-value.
//
// Input Arguments:
//		first	= const unsigned int&
//		second	= const unsigned int&
//		xValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotCurve::GetInterpolatedYOrdinate(const unsigned int &first, const unsigned int &second, const double &xValue) const
{
	double fraction;
	if (xAxis->IsLogarithmic())
		fraction = (log10(xValue) - log10(data->GetXData(first))) / (log10(data->GetXData(second)) - log10(data->GetXData(first)));
	else
		fraction = (xValue - data->GetXData(first)) / (data->GetXData(second) - data->GetXData(first));

	if (PlotMath::IsNaN(fraction))
		fraction = 1.0;

	if (yAxis->IsLogarithmic())
		return pow(data->GetYData(second), fraction) * pow(data->GetYData(first), 1.0 - fraction);

	return data->GetYData(first) + (data->GetYData(second) - data->GetYData(first)) * fraction;
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotMarkers
//
// Description:		Plots markers at all un-interpolated points.
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
void PlotCurve::PlotMarkers(void) const
{
	unsigned int i;
	for (i = 0; i < data->GetNumberOfPoints(); i++)
	{
		if (PointIsWithinPlotArea(i))
			DrawMarker(data->GetXData(i), data->GetYData(i));
	}
}

//==========================================================================
// Class:			PlotCurve
// Function:		DrawMarker
//
// Description:		Draws a marker at the specified location.
//
// Input Arguments:
//		x	= const double&
//		y	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::DrawMarker(const double &x, const double &y) const
{
	double doublePoint[2] = {x, y};
	int point[2];
	RescalePoint(doublePoint, point);

	int halfMarkerSize = 2 * markerSize;

	glVertex2i(point[0] + halfMarkerSize, point[1] + halfMarkerSize);
	glVertex2i(point[0] + halfMarkerSize, point[1] - halfMarkerSize);
	glVertex2i(point[0] - halfMarkerSize, point[1] - halfMarkerSize);
	glVertex2i(point[0] - halfMarkerSize, point[1] + halfMarkerSize);
}

//==========================================================================
// Class:			PlotCurve
// Function:		SmallRange
//
// Description:		Determines if the range is small enough to warrant
//					drawing the point markers.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool PlotCurve::SmallRange(void) const
{
	if (data->GetNumberOfPoints() < 2)
		return false;

	switch (SmallXRange())
	{
	case RangeSizeSmall:
		return true;

	case RangeSizeLarge:
		return false;

	default:
	case RangeSizeUndetermined:
		break;
	}

	return SmallYRange() == RangeSizeSmall;
}

//==========================================================================
// Class:			PlotCurve
// Function:		SmallXRange
//
// Description:		Determines if the x-range is small enough to warrant
//					drawing the point markers.  A "small enough range" is
//					one where there are less than some number of pixels in the
//					x-direction between points (on average).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCurve::RangeSize
//
//==========================================================================
PlotCurve::RangeSize PlotCurve::SmallXRange(void) const
{
	double period = data->GetXData(1) - data->GetXData(0);
	if (period == 0.0)
		return RangeSizeUndetermined;

	unsigned int points = (unsigned int)floor((xAxis->GetMaximum() - xAxis->GetMinimum()) / period);
	if (points == 0)
		return RangeSizeSmall;

	unsigned int spacing = (renderWindow.GetSize().GetWidth()
		- xAxis->GetAxisAtMaxEnd()->GetOffsetFromWindowEdge()
		- xAxis->GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) / points;

	if (spacing > 7)
		return RangeSizeSmall;

	return RangeSizeLarge;
}

//==========================================================================
// Class:			PlotCurve
// Function:		SmallYRange
//
// Description:		Determines if the y-range is small enough to warrant
//					drawing the point markers.  A "small enough range" is
//					one where there are less than some number of pixels in the
//					y-direction between points (on average).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCurve::RangeSize
//
//==========================================================================
PlotCurve::RangeSize PlotCurve::SmallYRange(void) const
{
	double period = data->GetYData(1) - data->GetYData(0);
	if (period == 0.0)
		return RangeSizeUndetermined;

	unsigned int points = (unsigned int)floor((yAxis->GetMaximum() - yAxis->GetMinimum()) / period);
	if (points == 0)
		return RangeSizeSmall;

	unsigned int spacing = (renderWindow.GetSize().GetHeight()
		- yAxis->GetAxisAtMaxEnd()->GetOffsetFromWindowEdge()
		- yAxis->GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) / points;

	if (spacing > 7)
		return RangeSizeSmall;

	return RangeSizeLarge;
}