/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotCurve.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating plot curves objects.
// History:

#ifndef PLOT_CURVE_H_
#define PLOT_CURVE_H_

// Local headers
#include "renderer/primitives/primitive.h"
#include "utilities/managedList.h"
#include "renderer/line.h"

// Local forward declarations
class Axis;
class Dataset2D;

class PlotCurve : public Primitive
{
public:
	PlotCurve(RenderWindow &renderWindow);
	PlotCurve(const PlotCurve &plotCurve);

	~PlotCurve();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();

	void SetModified() { modified = true; }

	void SetLineSize(const double &size) { lineSize = size; modified = true; }
	void SetMarkerSize(const int &size) { markerSize = size; modified = true; }
	void SetPretty(const bool &pretty) { line.SetPretty(pretty); modified = true; }

	// Remove all data from the plot
	void SetData(const Dataset2D *data);
	void ClearData() { data = NULL; }

	// For setting up the plot
	void BindToXAxis(Axis *xAxis) { this->xAxis = xAxis; modified = true; }
	void BindToYAxis(Axis *yAxis) { this->yAxis = yAxis; modified = true; }

	Axis *GetYAxis(void) { return yAxis; }

	// Overloaded operators
	PlotCurve& operator=(const PlotCurve &plotCurve);

private:
	// The axes with which this object is associated
	Axis *xAxis;
	Axis *yAxis;

	const Dataset2D *data;

	Line line;
	std::vector<std::pair<double, double> > points;

	double lineSize;
	int markerSize;

	void RescalePoint(const double *value, double *coordinate) const;

	bool PointIsWithinPlotArea(const unsigned int &i) const;
	void PlotPoint(const unsigned int &i);
	void PlotPoint(const double &x, const double &y);
	void PlotInterpolatedPoint(const unsigned int &first, const unsigned int &second, const bool &startingPoint);
	void PlotInterpolatedJumpPoints(const unsigned int &first, const unsigned int &second);

	bool PointsCrossBottomAxis(const unsigned int &first, const unsigned int &second) const;
	bool PointsCrossTopAxis(const unsigned int &first, const unsigned int &second) const;
	bool PointsCrossLeftAxis(const unsigned int &first, const unsigned int &second) const;
	bool PointsCrossRightAxis(const unsigned int &first, const unsigned int &second) const;

	bool PointsCrossXOrdinate(const unsigned int &first, const unsigned int &second, const double &value) const;
	bool PointsCrossYOrdinate(const unsigned int &first, const unsigned int &second, const double &value) const;

	bool PointsJumpPlotArea(const unsigned int &first, const unsigned int &second) const;

	bool PointIsValid(const unsigned int &i) const;

	double GetInterpolatedXOrdinate(const unsigned int &first, const unsigned int &second, const double &yValue) const;
	double GetInterpolatedYOrdinate(const unsigned int &first, const unsigned int &second, const double &xValue) const;

	void PlotMarkers(void) const;
	void DrawMarker(const double &x, const double &y) const;

	enum RangeSize
	{
		RangeSizeSmall,
		RangeSizeLarge,
		RangeSizeUndetermined
	};

	bool SmallRange(void) const;
	RangeSize SmallXRange(void) const;
	RangeSize SmallYRange(void) const;
};

#endif// PLOT_CURVE_H_