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

#ifndef PLOT_OBJECT_H_
#define PLOT_OBJECT_H_

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
	PlotObject(PlotRenderer &renderer);
	~PlotObject();

	void Update();

	void RemoveExistingPlots();
	void RemovePlot(const unsigned int &index);
	void AddCurve(const Dataset2D &data);

	// Accessors for the axes limits
	inline double GetXMin() const { return xMin; }
	inline double GetXMax() const { return xMax; }
	inline double GetLeftYMin() const { return yLeftMin; }
	inline double GetLeftYMax() const { return yLeftMax; }
	inline double GetRightYMin() const { return yRightMin; }
	inline double GetRightYMax() const { return yRightMax; }

	inline double GetXMinOriginal() const { return xMinOriginal; }
	inline double GetXMaxOriginal() const { return xMaxOriginal; }
	inline double GetLeftYMinOriginal() const { return yLeftMinOriginal; }
	inline double GetLeftYMaxOriginal() const { return yLeftMaxOriginal; }
	inline double GetRightYMinOriginal() const { return yRightMinOriginal; }
	inline double GetRightYMaxOriginal() const { return yRightMaxOriginal; }

	void SetXMin(const double &xMin);
	void SetXMax(const double &xMax);
	void SetLeftYMin(const double &yMin);
	void SetLeftYMax(const double &yMax);
	void SetRightYMin(const double &yMin);
	void SetRightYMax(const double &yMax);

	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const double &lineSize,
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

	void SetPrettyCurves(const bool &pretty) { this->pretty = pretty; }

	inline double GetXMajorResolution() const { return xMajorResolution; }
	inline double GetLeftYMajorResolution() const { return yLeftMajorResolution; }
	inline double GetRightYMajorResolution() const { return yRightMajorResolution; }

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	wxString GetXLabel() const;
	wxString GetLeftYLabel() const;
	wxString GetRightYLabel() const;
	wxString GetTitle() const;

	void SetGridColor(const Color &color);
	Color GetGridColor() const;

	void ResetAutoScaling();
	void SetAutoScaleBottom() { autoScaleX = true; }
	void SetAutoScaleLeft() { autoScaleLeftY = true; }
	void SetAutoScaleRight() { autoScaleRightY = true; }

	const Axis* GetBottomAxis() const { return axisBottom; }
	const Axis* GetTopAxis() const { return axisTop; }
	const Axis* GetLeftYAxis() const { return axisLeft; }
	const Axis* GetRightYAxis() const { return axisRight; }
	bool GetMajorGrid();
	bool GetMinorGrid();

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	bool GetXAxisAutoScaled() const { return autoScaleX; }

	unsigned int GetCurveCount() const { return plotList.size(); }
	unsigned long long GetTotalPointCount() const;

	FTFont* GetAxisFont() const { return axisFont; }

	unsigned int GetHorizontalAxisOffset(const bool &withLabel) const;
	unsigned int GetVerticalAxisOffset(const bool &withLabel) const;

private:
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

	FTFont *axisFont;
	FTFont *titleFont;

	// The minimums and maximums for the axis
	double xMin, xMax, yLeftMin, yLeftMax, yRightMin, yRightMax;
	double xMinOriginal, xMaxOriginal, yLeftMinOriginal, yLeftMaxOriginal,
		yRightMinOriginal, yRightMaxOriginal;

	bool autoScaleX;
	bool autoScaleLeftY;
	bool autoScaleRightY;

	bool leftUsed;
	bool rightUsed;

	bool pretty;

	double xMajorResolution;
	double yLeftMajorResolution;
	double yRightMajorResolution;

	// The actual plot objects
	std::vector<PlotCurve*> plotList;
	std::vector<const Dataset2D*> dataList;

	void CreateAxisObjects();
	void InitializeFonts();
	void CreateFontObjects(const wxString &fontFile);

	// Handles all of the formatting for the plot
	void FormatPlot();

	// Handles the spacing of the axis ticks
	void AutoScaleAxis(double &min, double &max, double &majorRes, const int &maxTicks, const bool &logarithmic,
		const bool &forceLimits) const;
	void AutoScaleLogAxis(double &min, double &max, double &majorRes, const bool &forceLimits) const;
	void AutoScaleLinearAxis(double &min, double &max, double &majorRes, const int &maxTicks,
		const bool &forceLimits) const;
	void RoundMinMax(double &min, double &max, const double &tickSpacing, const bool &forceLimits) const;
	double ComputeMinorResolution(const double &min, const double &max,
		const double &majorResolution, const double &axisLength) const;

	void FormatAxesBasics();
	void FormatBottomBasics(const Axis::TickStyle &tickStyle);
	void FormatTopBasics(const Axis::TickStyle &tickStyle);
	void FormatLeftBasics(const Axis::TickStyle &tickStyle);
	void FormatRightBasics(const Axis::TickStyle &tickStyle);
	void UpdateAxesOffsets();
	void SetAxesColor(const Color &color);
	void FormatTitle();

	void FormatCurves();

	void CheckForZeroRange();
	void HandleZeroRangeAxis(double &min, double &max) const;

	void ApplyRangeLimits(const double &xMinor, const double &xMajor, const double &yLeftMinor,
		const double &yLeftMajor, const double &yRightMinor, const double &yRightMajor);
	void CheckAutoScaling();
	void UpdateLimitValues();
	void ValidateRangeLimits(double &min, double &max, const bool &autoScale, double &major, double &minor) const;
	void ValidateLogarithmicLimits(Axis &axis, const double &min);
	void SetOriginalAxisLimits();
	void GetAxisExtremes(const Dataset2D &data, Axis *yAxis);
	void ResetOriginalLimits();
	void MatchYAxes();
	double GetFirstValidValue(const double* data, const unsigned int &size) const;
};

#endif// PLOT_OBJECT_H_