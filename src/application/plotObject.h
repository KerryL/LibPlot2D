/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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
		const bool &visible, const bool &rightAxis, const unsigned int &size,
		const int &markerSize);

	void SetMajorGrid(const bool &gridOn);
	void SetMinorGrid(const bool &gridOn);
	void SetXMajorGrid(const bool &gridOn);
	void SetXMinorGrid(const bool &gridOn);
	void SetLeftYMajorGrid(const bool &gridOn);
	void SetLeftYMinorGrid(const bool &gridOn);
	void SetRightYMajorGrid(const bool &gridOn);
	void SetRightYMinorGrid(const bool &gridOn);

	void SetXMajorResolution(const double &resolution);
	void SetLeftYMajorResolution(const double &resolution);
	void SetRightYMajorResolution(const double &resolution);

	inline double GetXMajorResolution() const { return xMajorResolution; }
	inline double GetLeftYMajorResolution() const { return yLeftMajorResolution; }
	inline double GetRightYMajorResolution() const { return yRightMajorResolution; }

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	wxString GetXLabel(void) const;
	wxString GetLeftYLabel(void) const;
	wxString GetRightYLabel(void) const;
	wxString GetTitle(void) const;

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
	bool GetMajorGrid(void);
	bool GetMinorGrid(void);

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	bool GetXAxisAutoScaled(void) const { return autoScaleX; };

	unsigned int GetCurveCount(void) const { return plotList.size(); };

	FTFont* GetAxisFont() const { return axisFont; }

private:
	// The renderer object
	PlotRenderer &renderer;

	static const unsigned int horizontalOffsetWithLabel;
	static const unsigned int horizontalOffsetWithoutLabel;
	static const unsigned int verticalOffsetWithLabel;
	static const unsigned int verticalOffsetWithoutLabel;

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

	bool leftUsed;
	bool rightUsed;

	double xMajorResolution;
	double yLeftMajorResolution;
	double yRightMajorResolution;

	// The actual plot objects
	std::vector<PlotCurve*> plotList;
	std::vector<const Dataset2D*> dataList;

	void CreateAxisObjects(void);
	void InitializeFonts(void);
	void CreateFontObjects(const wxString &fontFile);

	// Handles all of the formatting for the plot
	void FormatPlot(void);

	// Handles the spacing of the axis ticks
	void AutoScaleAxis(double &min, double &max, double &majorRes, const int &maxTicks, const bool &logarithmic,
		const bool &forceLimits) const;
	void AutoScaleLogAxis(double &min, double &max, double &majorRes, const bool &forceLimits) const;
	void AutoScaleLinearAxis(double &min, double &max, double &majorRes, const int &maxTicks,
		const bool &forceLimits) const;
	void RoundMinMax(double &min, double &max, const double &tickSpacing, const bool &forceLimits) const;
	double ComputeMinorResolution(const double &min, const double &max,
		const double &majorResolution, const double &axisLength) const;

	void FormatAxesBasics(void);
	void FormatBottomBasics(const Axis::TickStyle &tickStyle);
	void FormatTopBasics(const Axis::TickStyle &tickStyle);
	void FormatLeftBasics(const Axis::TickStyle &tickStyle);
	void FormatRightBasics(const Axis::TickStyle &tickStyle);
	void SetAxesColor(const Color &color);
	void FormatTitle(void);

	void FormatCurves(void);

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
	void MatchYAxes(void);
	double GetFirstValidValue(const double* data, const unsigned int &size) const;
};

#endif// _PLOT_OBJECT_H_