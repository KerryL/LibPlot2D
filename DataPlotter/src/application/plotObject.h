/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

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

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/primitives/axis.h"

// Local forward declarations
class PlotRenderer;
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

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	bool GetXAxisAutoScaled(void) const { return autoScaleX; };

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
	std::vector<PlotCurve*> plotList;
	std::vector<const Dataset2D*> dataList;

	void CreateAxisObjects(void);
	void InitializeFonts(void);
	void CreateFontObjects(const wxString &fontFile);

	// Handles all of the formatting for the plot
	void FormatPlot(void);

	// Handles the spacing of the axis ticks
	double AutoScaleAxis(double &min, double &max, const int &maxTicks, const bool &logarithmic,
		const bool &forceLimits = false) const;
	double AutoScaleLogAxis(double &min, double &max, const bool &forceLimits) const;
	double AutoScaleLinearAxis(double &min, double &max, const int &maxTicks, const bool &forceLimits) const;
	void RoundMinMax(double &min, double &max, const double &tickSpacing, const bool &forceLimits) const;

	void FormatAxesBasics(void);
	void FormatBottomBasics(const Axis::TickStyle &tickStyle);
	void FormatTopBasics(const Axis::TickStyle &tickStyle);
	void FormatLeftBasics(const Axis::TickStyle &tickStyle);
	void FormatRightBasics(const Axis::TickStyle &tickStyle);
	void SetAxesColor(const Color &color);
	void FormatTitle(void);

	void CheckForZeroRange(void);
	void HandleZeroRangeAxis(double &min, double &max) const;

	void ApplyRangeLimits(const double &xMinor, const double &xMajor, const double &yLeftMinor,
		const double &yLeftMajor, const double &yRightMinor, const double &yRightMajor);
	void CheckAutoScaling(void);
	void UpdateLimitValues(void);
	void ValidateRangeLimits(double &min, double &max, const bool &autoScale, double &major, double &minor) const;
	void ValidateLogarithmicLimits(Axis &axis, const double &min);
	void SetOriginalAxisLimits(void);
	void GetAxisExtremes(const Dataset2D &data, Axis *yAxis);
	void ResetOriginalLimits(void);
	void MatchYAxes(const bool &leftFound, const bool &rightFound);
};

#endif// _PLOT_OBJECT_H_