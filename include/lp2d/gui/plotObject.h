/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotObject.h
// Date:  5/4/2011
// Auth:  K. Loux
// Desc:  Intermediate class for creating plots from arrays of data.

#ifndef PLOT_OBJECT_H_
#define PLOT_OBJECT_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/renderer/primitives/axis.h"

namespace LibPlot2D
{

// Local forward declarations
class PlotRenderer;
class TextRendering;
class PlotCurve;
class Dataset2D;
class Color;
class GuiInterface;

/// Class for formatting the plot area and controlling axis properties.
class PlotObject
{
public:
	/// Constructor.
	///
	/// \param renderer     Reference to owning renderer.
	/// \param guiInterface Reference to interface object.
	PlotObject(PlotRenderer &renderer, GuiInterface& guiInterface);

	/// Updates the plot formatting.
	void Update();

	/// Removes all existing plots.
	void RemoveExistingPlots();

	/// Removes the specified plot.
	///
	/// \param index Index of the plot to remove.
	void RemovePlot(const unsigned int &index);

	/// Adds the specified curve to the plot.
	///
	/// \param data Data set to add.
	void AddCurve(const Dataset2D &data);

	/// \name Accessors for the axes limits
	/// @{

	inline double GetXMin() const { return mXMin; }
	inline double GetXMax() const { return mXMax; }
	inline double GetLeftYMin() const { return mYLeftMin; }
	inline double GetLeftYMax() const { return mYLeftMax; }
	inline double GetRightYMin() const { return mYRightMin; }
	inline double GetRightYMax() const { return mYRightMax; }

	inline double GetXMinOriginal() const { return mXMinOriginal; }
	inline double GetXMaxOriginal() const { return mXMaxOriginal; }
	inline double GetLeftYMinOriginal() const { return mYLeftMinOriginal; }
	inline double GetLeftYMaxOriginal() const { return mYLeftMaxOriginal; }
	inline double GetRightYMinOriginal() const { return mYRightMinOriginal; }
	inline double GetRightYMaxOriginal() const { return mYRightMaxOriginal; }

	void SetXMin(const double &xMin);
	void SetXMax(const double &xMax);
	void SetLeftYMin(const double &yMin);
	void SetLeftYMax(const double &yMax);
	void SetRightYMin(const double &yMin);
	void SetRightYMax(const double &yMax);

	/// @}

	/// Sets the properties of the specified curve.
	///
	/// \param index	  Index of the curve to modify.
	/// \param color	  Specifies the color of the curve.
	/// \param visible	  Indicates if the curve should be plotted.
	/// \param rightAxis  Indicates if the curve is plotted against the left or
	///                   right axis.
	/// \param lineSize	  Size of the line to draw.
	/// \param markerSize Size of the marker to draw.
	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const double &lineSize,
		const int &markerSize);

	/// \name Grid line control
	/// @{

	void SetMajorGrid(const bool &gridOn);
	void SetMinorGrid(const bool &gridOn);
	void SetXMajorGrid(const bool &gridOn);
	void SetXMinorGrid(const bool &gridOn);
	void SetLeftYMajorGrid(const bool &gridOn);
	void SetLeftYMinorGrid(const bool &gridOn);
	void SetRightYMajorGrid(const bool &gridOn);
	void SetRightYMinorGrid(const bool &gridOn);

	void SetGridColor(const Color &color);
	Color GetGridColor() const;

	bool GetMajorGrid();
	bool GetMinorGrid();

	/// @}

	/// \name Axis resolution accessors
	/// @{

	void SetXMajorResolution(const double &resolution);
	void SetLeftYMajorResolution(const double &resolution);
	void SetRightYMajorResolution(const double &resolution);

	inline double GetXMajorResolution() const { return mXMajorResolution; }
	inline double GetLeftYMajorResolution() const { return mYLeftMajorResolution; }
	inline double GetRightYMajorResolution() const { return mYRightMajorResolution; }

	/// @}

	/// Sets the flag indicating whether or not to use the higher quality
	/// rendering algorithm.
	///
	/// \param pretty Set true to use the higher quality rendering algorithm.
	void SetPrettyCurves(const bool &pretty) { mPretty = pretty; }

	/// \name Text object controls
	/// @{

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	wxString GetXLabel() const;
	wxString GetLeftYLabel() const;
	wxString GetRightYLabel() const;
	wxString GetTitle() const;

	/// @}

	/// \name Scaling methods
	/// @{

	void ResetAutoScaling();
	void SetAutoScaleBottom() { mAutoScaleX = true; }
	void SetAutoScaleLeft() { mAutoScaleLeftY = true; }
	void SetAutoScaleRight() { mAutoScaleRightY = true; }

	bool GetXAxisAutoScaled() const { return mAutoScaleX; }

	/// @}

	/// \name Axis object accessors
	/// @{

	const Axis* GetBottomAxis() const { return mAxisBottom; }
	const Axis* GetTopAxis() const { return mAxisTop; }
	const Axis* GetLeftYAxis() const { return mAxisLeft; }
	const Axis* GetRightYAxis() const { return mAxisRight; }

	/// @}

	/// \name Logarithmic scaling toggles
	/// @{

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	/// @}

	/// Gets the total number of curves.
	/// \returns The total number of curves.
	unsigned int GetCurveCount() const { return mPlotList.size(); }

	/// Gets the total number of points.
	/// \returns The total number of points.
	unsigned long long GetTotalPointCount() const;

	bool CurveMarkersVisible(const unsigned int& i) const;

	/// Gets the offset from the side of the window for horizontal axes.
	///
	/// \param withLabel Indicates if the calculation should allow room for a
	///                  label.
	///
	/// \returns The number of pixels between the window edge and the rendered
	///          axis.
	unsigned int GetHorizontalAxisOffset(const bool &withLabel) const;

	/// Gets the offset from the side of the window for vertical axes.
	///
	/// \param withLabel Indicates if the calculation should allow room for a
	///                  label.
	///
	/// \returns The number of pixels between the window edge and the rendered
	///          axis.
	unsigned int GetVerticalAxisOffset(const bool &withLabel) const;

	/// Sets a flag indicating that the plot area size needs to be updated
	/// during the next render cycle.
	inline void UpdatePlotAreaSize() { mNeedScissorUpdate = true; }

	/// Gets the file name for the TrueType font file used to render axis text.
	/// \returns The file name for the axis font.
	inline std::string GetAxisFont() const { return mFontFileName; }

private:
	PlotRenderer &mRenderer;
	GuiInterface &mGuiInterface;

	static const unsigned int mHorizontalOffsetWithLabel;
	static const unsigned int mHorizontalOffsetWithoutLabel;
	static const unsigned int mVerticalOffsetWithLabel;
	static const unsigned int mVerticalOffsetWithoutLabel;

	// The actors (the non-plot actors that are always present)
	Axis *mAxisTop;
	Axis *mAxisBottom;
	Axis *mAxisLeft;
	Axis *mAxisRight;

	TextRendering *mTitleObject;

	// The minimums and maximums for the axis
	double mXMin, mXMax, mYLeftMin, mYLeftMax, mYRightMin, mYRightMax;
	double mXMinOriginal = 0.0;
	double mXMaxOriginal = 0.0;
	double mYLeftMinOriginal = 0.0;
	double mYLeftMaxOriginal = 0.0;
	double mYRightMinOriginal = 0.0;
	double mYRightMaxOriginal = 0.0;

	bool mAutoScaleX;
	bool mAutoScaleLeftY;
	bool mAutoScaleRightY;

	bool mLeftUsed;
	bool mRightUsed;

	bool mPretty;

	double mXMajorResolution;
	double mYLeftMajorResolution;
	double mYRightMajorResolution;

	bool mNeedScissorUpdate = true;

	// The actual plot objects
	std::vector<PlotCurve*> mPlotList;
	std::vector<const Dataset2D*> mDataList;

	std::string mFontFileName;
	void CreateAxisObjects();
	void InitializeFonts();

	// Handles all of the formatting for the plot
	void FormatPlot();
	void ComputeTransformationMatrices();

	// Handles the spacing of the axis ticks
	void AutoScaleAxis(double &min, double &max, double &majorRes,
		const int &maxTicks, const bool &logarithmic,
		const bool &forceLimits) const;
	void AutoScaleLogAxis(double &min, double &max, double &majorRes,
		const bool &forceLimits) const;
	void AutoScaleLinearAxis(double &min, double &max, double &majorRes,
		const int &maxTicks, const bool &forceLimits) const;
	void RoundMinMax(double &min, double &max, const double &tickSpacing,
		const bool &forceLimits) const;
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

	void ApplyRangeLimits(const double &xMinor, const double &xMajor,
		const double &yLeftMinor, const double &yLeftMajor,
		const double &yRightMinor, const double &yRightMajor);
	void CheckAutoScaling();
	void UpdateLimitValues();
	void ValidateRangeLimits(double &min, double &max, const bool &autoScale,
		double &major, double &minor) const;
	void ValidateLogarithmicLimits(Axis &axis, const double &min);
	void SetOriginalAxisLimits();
	void GetAxisExtremes(const Dataset2D &data, Axis *yAxis);
	void ResetOriginalLimits();
	void MatchYAxes();
	double GetFirstValidValue(const std::vector<double>& data) const;

	void UpdateScissorArea() const;
};

}// namespace LibPlot2D

#endif// PLOT_OBJECT_H_
