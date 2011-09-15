/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_curve.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating plot curves objects.
// History:

#ifndef _PLOT_CURVE_H_
#define _PLOT_CURVE_H_

// Local headers
#include "renderer/primitives/primitive.h"
#include "utilities/managed_list_class.h"

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

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	void SetModified(void) { modified = true; };

	void SetSize(const unsigned int &_size) { size = _size; modified = true; };

	// Remove all data from the plot
	void SetData(const Dataset2D *_data);
	void ClearData(void) { data = NULL; };

	// For setting up the plot
	void BindToXAxis(Axis *_xAxis) { xAxis = _xAxis; modified = true; };
	void BindToYAxis(Axis *_yAxis) { yAxis = _yAxis; modified = true; };

	Axis *GetYAxis(void) { return yAxis; };

	// Gets the Y-value that corresponds to the specified X value
	bool GetYAt(double &value);

	// Overloaded operators
	PlotCurve& operator = (const PlotCurve &plotCurve);

private:
	// The axes with which this object is associated
	Axis *xAxis;
	Axis *yAxis;

	unsigned int size;

	// For use when the axis scale changes
	void RescalePoint(const double *xyPoint, int *point);

	// For appearance
	static const int offsetFromWindowEdge;

	// The data to be plotted
	const Dataset2D *data;
};

#endif// _PLOT_CURVE_H_