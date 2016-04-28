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

	virtual ~PlotCurve();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual void GenerateGeometry();
	virtual bool HasValidParameters();

	inline void SetLineSize(const double &size) { lineSize = size; modified = true; }
	inline void SetMarkerSize(const int &size) { markerSize = size; modified = true; }
	inline void SetPretty(const bool &pretty) { line.SetPretty(pretty); modified = true; }

	void SetData(const Dataset2D *data);
	inline void ClearData() { data = NULL; }

	// For setting up the plot
	inline void BindToXAxis(Axis *xAxis) { this->xAxis = xAxis; modified = true; }
	inline void BindToYAxis(Axis *yAxis) { this->yAxis = yAxis; modified = true; }

	inline Axis *GetYAxis() { return yAxis; }

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

	//bool PointIsWithinPlotArea(const unsigned int &i) const;
	void PlotPoint(const unsigned int &i);
	void PlotPoint(const double &x, const double &y);

	bool PointIsValid(const unsigned int &i) const;

	void PlotMarkers() const;
	void DrawMarker(const double &x, const double &y) const;

	enum RangeSize
	{
		RangeSizeSmall,
		RangeSizeLarge,
		RangeSizeUndetermined
	};

	bool SmallRange() const;
	RangeSize SmallXRange() const;
	RangeSize SmallYRange() const;
};

#endif// PLOT_CURVE_H_