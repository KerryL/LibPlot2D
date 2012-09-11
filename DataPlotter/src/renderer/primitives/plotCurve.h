/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCurve.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating plot curves objects.
// History:

#ifndef _PLOT_CURVE_H_
#define _PLOT_CURVE_H_

// Local headers
#include "renderer/primitives/primitive.h"
#include "utilities/managedList.h"

// Local forward declarations
class Axis;
class Dataset2D;

class PlotCurve : public Primitive
{
public:
	// Constructor
	PlotCurve(RenderWindow &_renderWindow);
	PlotCurve(const PlotCurve &plotCurve);

	// Destructor
	~PlotCurve();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	void SetModified(void) { modified = true; };

	void SetLineSize(const unsigned int &size) { lineSize = size; modified = true; };
	void SetMarkerSize(const int &size) { markerSize = size; modified = true; };

	// Remove all data from the plot
	void SetData(const Dataset2D *_data);
	void ClearData(void) { data = NULL; };

	// For setting up the plot
	void BindToXAxis(Axis *_xAxis) { xAxis = _xAxis; modified = true; };
	void BindToYAxis(Axis *_yAxis) { yAxis = _yAxis; modified = true; };

	Axis *GetYAxis(void) { return yAxis; };

	// Overloaded operators
	PlotCurve& operator = (const PlotCurve &plotCurve);

private:
	// The axes with which this object is associated
	Axis *xAxis;
	Axis *yAxis;

	const Dataset2D *data;

	unsigned int lineSize;
	int markerSize;

	void RescalePoint(const double *value, int *coordinate) const;

	bool PointIsWithinPlotArea(const unsigned int &i) const;
	void PlotPoint(const unsigned int &i) const;
	void PlotPoint(const double &x, const double &y) const;
	void PlotInterpolatedPoint(const unsigned int &first, const unsigned int &second, const bool &startingPoint) const;
	void PlotInterpolatedJumpPoints(const unsigned int &first, const unsigned int &second) const;

	bool PointsCrossBottomAxis(const unsigned int &first, const unsigned int &second) const;
	bool PointsCrossTopAxis(const unsigned int &first, const unsigned int &second) const;
	bool PointsCrossLeftAxis(const unsigned int &first, const unsigned int &second) const;
	bool PointsCrossRightAxis(const unsigned int &first, const unsigned int &second) const;

	bool PointsCrossXOrdinate(const unsigned int &first, const unsigned int &second, const double &value) const;
	bool PointsCrossYOrdinate(const unsigned int &first, const unsigned int &second, const double &value) const;

	bool PointsJumpPlotArea(const unsigned int &first, const unsigned int &second) const;

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

#endif// _PLOT_CURVE_H_