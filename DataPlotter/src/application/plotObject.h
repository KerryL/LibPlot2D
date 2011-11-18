/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotObject.h
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Intermediate class for creating plots from arrays of data.
// History:

#ifndef _PLOT_OBJECT_H_
#define _PLOT_OBJECT_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/objectList.h"

// Local forward declarations
class PlotRenderer;
class Axis;
class TextRendering;
class PlotCurve;
class Dataset2D;
class Color;

// FTGL forward declarations
class FTFont;

class PlotObject
{
public:
	// Constructor
	PlotObject(PlotRenderer &_renderer);

	// Destructor
	~PlotObject();

	// Updates the actors on the screen
	void Update(void);

	void RemoveExistingPlots(void);
	void RemovePlot(const unsigned int &index);
	void AddCurve(const Dataset2D &data);

	// Accessors for the axes limits
	inline double GetXMin(void) const { return xMin; };
	inline double GetXMax(void) const { return xMax; };
	inline double GetLeftYMin(void) const { return yLeftMin; };
	inline double GetLeftYMax(void) const { return yLeftMax; };
	inline double GetRightYMin(void) const { return yRightMin; };
	inline double GetRightYMax(void) const { return yRightMax; };

	inline double GetXMinOriginal(void) const { return xMinOriginal; };
	inline double GetXMaxOriginal(void) const { return xMaxOriginal; };
	inline double GetLeftYMinOriginal(void) const { return yLeftMinOriginal; };
	inline double GetLeftYMaxOriginal(void) const { return yLeftMaxOriginal; };
	inline double GetRightYMinOriginal(void) const { return yRightMinOriginal; };
	inline double GetRightYMaxOriginal(void) const { return yRightMaxOriginal; };

	void SetXMin(const double &_xMin);
	void SetXMax(const double &_xMax);
	void SetLeftYMin(const double &_yMin);
	void SetLeftYMax(const double &_yMax);
	void SetRightYMin(const double &_yMin);
	void SetRightYMax(const double &_yMax);

	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const unsigned int &size);
	void SetGrid(const bool &gridOn);
	void SetXGrid(const bool &gridOn);
	void SetLeftYGrid(const bool &gridOn);
	void SetRightYGrid(const bool &gridOn);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	void SetGridColor(const Color &color);
	Color GetGridColor(void) const;

	void ResetAutoScaling(void);
	void SetAutoScaleBottom(void) { autoScaleX = true; };
	void SetAutoScaleLeft(void) { autoScaleLeftY = true; };
	void SetAutoScaleRight(void) { autoScaleRightY = true; };

	const Axis* GetBottomAxis(void) const { return axisBottom; };
	const Axis* GetTopAxis(void) const { return axisTop; };
	const Axis* GetLeftYAxis(void) const { return axisLeft; };
	const Axis* GetRightYAxis(void) const { return axisRight; };
	bool GetGrid(void);

private:
	// The renderer object
	PlotRenderer &renderer;

	// The actors (the non-plot actors that are always present)
	Axis *axisTop;
	Axis *axisBottom;
	Axis *axisLeft;
	Axis *axisRight;

	TextRendering *titleObject;

	// The font objects
	FTFont *axisFont;
	FTFont *titleFont;

	// The minimums and maximums for the axis
	double xMin, xMax, yLeftMin, yLeftMax, yRightMin, yRightMax;
	double xMinOriginal, xMaxOriginal, yLeftMinOriginal, yLeftMaxOriginal,
		yRightMinOriginal, yRightMaxOriginal;

	// Flag to indicate how the limits are set
	bool autoScaleX;
	bool autoScaleLeftY;
	bool autoScaleRightY;

	// The actual plot objects
	ObjectList<PlotCurve> plotList;
	ObjectList<const Dataset2D> dataList;

	// Handles all of the formatting for the plot
	void FormatPlot(void);

	// Handles the spacing of the axis ticks
	double AutoScaleAxis(double &min, double &max, int maxTicks,
		const bool &forceLimits = false);
};

#endif// _PLOT_OBJECT_H_