/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotRenderer.h
// Date:  5/4/2011
// Auth:  K. Loux
// Desc:  Derived from RenderWindow, this class is used to display plots on
//        the screen.

#ifndef PLOT_RENDERER_H_
#define PLOT_RENDERER_H_

// Local headers
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/renderer/primitives/legend.h"
#include "lp2d/utilities/flagEnum.h"

// Eigen headers
#include <Eigen/Eigen>

// Standard C++ headers
#include <stack>
#include <vector>
#include <cmath>
#include <memory>

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

// Local forward declarations
class PlotObject;
class Dataset2D;
class ZoomBox;
class PlotCursor;
class GuiInterface;

/// Class for handling rendering of 2D plots.
class PlotRenderer : public RenderWindow
{
public:
	/// Constructor.
	///
	/// \param guiInterface Reference to the interface object.
	/// \param parent	    Window which owns this.
	/// \param id		    Window id.
	/// \param attr		    Requested OpenGL canvas/context attributes.
	PlotRenderer(GuiInterface& guiInterface, wxWindow &parent, wxWindowID id,
		const wxGLAttributes& attr);
	~PlotRenderer() = default;

	/// Enumeration of quality levels for drawing curves.
	enum class CurveQuality
	{
		AlwaysLow = 0,
		HighWrite = 1 << 0,
		HighDrag = 1 << 1,
		HighStatic = 1 << 2,
		AlwaysHigh = HighWrite | HighDrag | HighStatic
	};

	/// \name Getters
	/// @{

	bool GetBottomMajorGrid() const;
	bool GetLeftMajorGrid() const;
	bool GetRightMajorGrid() const;

	bool GetBottomMinorGrid() const;
	bool GetLeftMinorGrid() const;
	bool GetRightMinorGrid() const;

	Color GetGridColor() const;

	double GetXMin() const;
	double GetXMax() const;
	double GetLeftYMin() const;
	double GetLeftYMax() const;
	double GetRightYMin() const;
	double GetRightYMax() const;

	bool GetXLogarithmic() const;
	bool GetLeftLogarithmic() const;
	bool GetRightLogarithmic() const;

	wxString GetXLabel() const;
	wxString GetLeftYLabel() const;
	wxString GetRightYLabel() const;
	wxString GetTitle() const;

	bool GetMajorGridOn() const;
	bool GetMinorGridOn() const;

	CurveQuality GetCurveQuality() const { return mCurveQuality; }

	bool LegendIsVisible() const;

	bool GetLeftCursorVisible() const;
	bool GetRightCursorVisible() const;

	/// @}

	/// Checks to see if the x-axis is "zoomed" in or if it shows the full
	/// range of the available data.
	/// \returns True if the x-axis is zoomed in.
	bool GetXAxisZoomed() const;

	/// \name Setters
	/// @{

	void SetMajorGridOn();
	void SetMajorGridOff();
	void SetMinorGridOn();
	void SetMinorGridOff();

	void SetBottomMajorGrid(const bool &grid);
	void SetLeftMajorGrid(const bool &grid);
	void SetRightMajorGrid(const bool &grid);
	void SetBottomMinorGrid(const bool &grid);
	void SetLeftMinorGrid(const bool &grid);
	void SetRightMinorGrid(const bool &grid);

	void SetBottomMajorResolution(const double &resolution);
	void SetLeftMajorResolution(const double &resolution);
	void SetRightMajorResolution(const double &resolution);

	double GetBottomMajorResolution() const;
	double GetLeftMajorResolution() const;
	double GetRightMajorResolution() const;

	void SetGridColor(const Color &color);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	void SetCurveQuality(const CurveQuality& curveQuality);

	void SetLegendOn();
	void SetLegendOff();

	void SetXLimits(const double &min, const double &max);
	void SetLeftYLimits(const double &min, const double &max);
	void SetRightYLimits(const double &min, const double &max);

	void SetEqualScaling(const bool& equalScaling);

	/// @}

	/// Assigns the specified properties to the specified curve.
	///
	/// \param index      Index of the curve to update.
	/// \param color      Color to assign to the curve.
	/// \param visible    Flag indicating whether or not to render this curve.
	/// \param rightAxis  Flag indicating whether or not to plot this data
	///                   against the right axis.
	/// \param lineSize	  Width of the line in pixels.
	/// \param markerSize Size of the marker for the curve.
	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const double &lineSize,
		const int &markerSize);

	/// Adds the specified curve to the list of rendered curves.  Internally
	/// creates a PlotCurve object and adds it to this.
	///
	/// \param data Data set to add.
	void AddCurve(const Dataset2D &data);

	/// Removes all curves from the list.
	void RemoveAllCurves();

	/// Remvoes the specified curve from the list.
	///
	/// \param index Index of curve to remove.
	void RemoveCurve(const unsigned int &index);

	/// \name Autoscale methods.
	/// @{

	void AutoScale();///< Sets autoscaling on.
	void AutoScaleBottom();///< Sets autoscaling on for the bottom axis.
	void AutoScaleLeft();///< Sets autoscaling on for the left axis.
	void AutoScaleRight();///< Sets autoscaling on for the right axis.

	/// @}
	
	/// Updates the legend contents.
	///
	/// \param entries Information about how to list the legend entries.
	void UpdateLegend(const std::vector<Legend::LegendEntryInfo> &entries);

	void UpdateDisplay();///< Updates the rendered scene.

	/// Gets the value at which the cursor intersects the x-axis.
	/// \returns The value at which the cursor intersects the x-axis.
	double GetLeftCursorValue() const;

	/// Gets the value at which the cursor intersects the x-axis.
	/// \returns The value at which the cursor intersects the x-axis.
	double GetRightCursorValue() const;

	void UpdateCursors();///< Updates the cursor calculations.

	void SaveCurrentZoom();///< Adds the current zoom level to the stack.
	void ClearZoomStack();///< Empties the zoom stack.

	static const unsigned int mMaxXTicks;///< Max allowed number of x-ticks.
	static const unsigned int mMaxYTicks;///< Max allowed number of y-ticks.

	/// Computes the ideal number of ticks and space between them given the max
	/// and min values and the maximum number of ticks.
	///
	/// \param min      Minimum axis value.
	/// \param max      Maximum axis value.
	/// \param maxTicks Maximum number of ticks between \p min and \p max.
	///
	/// \returns The ideal tick spacing.
	static double ComputeTickSpacing(const double &min, const double &max,
		const int &maxTicks);

	/// Gets the total number of plotted points.
	/// \returns The total number of plotted points.
	unsigned long long GetTotalPointCount() const;

	wxImage GetImage() const override;

	unsigned int GetVertexDimension() const override { return 2; }

	/// Enumeration of available modelview matrices.
	enum class Modelview
	{
		Fixed,
		Left,
		Right
	};

	/// Loads the specified matrix into the modelview uniform for the current
	/// shader.
	///
	/// \param mv Source of matrix to load into the uniform.
	void LoadModelviewUniform(const Modelview& mv);

	/// Sets the valud of the modelview matrix.
	///
	/// \param m Value to assign to the modelview matrix.
	void SetLeftModelview(const Eigen::Matrix4d& m) { mLeftModelview = m; }

	/// Sets the valud of the modelview matrix.
	///
	/// \param m Value to assign to the modelview matrix.
	void SetRightModelview(const Eigen::Matrix4d& m) { mRightModelview = m; }

	/// \name Methods for applying scaling functions
	/// @{

	inline double DoXScale(const double& value)
	{ return mXScaleFunction(value); }
	inline double DoLeftYScale(const double& value)
	{ return mLeftYScaleFunction(value); }
	inline double DoRightYScale(const double& value)
	{ return mRightYScaleFunction(value); }

	/// @}

	/// Typedef for scaling functions.
	typedef double (*ScalingFunction)(const double&);

	/// \name Scaling function accessors
	/// @{

	ScalingFunction GetXScaleFunction() { return mXScaleFunction; }
	ScalingFunction GetLeftYScaleFunction() { return mLeftYScaleFunction; }
	ScalingFunction GetRightYScaleFunction() { return mRightYScaleFunction; }

	/// @}

	/// \name Scaling functions
	/// @{

	static inline double DoLineaerScale(const double& value) { return value; }
	static inline double DoLogarithmicScale(const double& value)
	{ return log10(value); }

	/// @}

	/// Enumeration of plot area context choices.
	enum class PlotContext
	{
		XAxis,
		LeftYAxis,
		RightYAxis,
		PlotArea
	};

	/// Gets the range of the specified axis.
	///
	/// \param axis      Indicates which axis to query.
	/// \param min [out] Minimum axis value.
	/// \param max [out] Maximum axis value.
	///
	/// \returns True if \p axis context is valid.
	bool GetCurrentAxisRange(const PlotContext &axis, double &min,
		double &max) const;

	/// Sets the range of the specified axis.
	///
	/// \param axis	Indicates which axis to set.
	/// \param min  Minimum axis value.
	/// \param max  Maximum axis value.
	///
	/// \returns True if \p axis context is valid.
	void SetNewAxisRange(const PlotContext &axis, const double &min,
		const double &max);

	void DoCopy();///< Pastes clipboard data into plot.
	void DoPaste();///< Copies plot image to clipboard.

	/// Checks for visibility of the markers for the specified curve.
	///
	/// \param i Index of curve of interest.
	///
	/// \returns True if curve \p i has visible markers.
	bool CurveMarkersVisible(const unsigned int& i) const;

private:
	static const std::string mDefaultVertexShader;

	// Called from the PlotRenderer constructor only in order to initialize the display
	void CreateActors();

	std::unique_ptr<PlotObject> mPlot;

	// Overload of size event
	void OnSize(wxSizeEvent &event);

	// Overload of interaction events
	void OnMouseWheelEvent(wxMouseEvent &event);
	void OnMouseMoveEvent(wxMouseEvent &event);
	void OnRightButtonUpEvent(wxMouseEvent &event);
	void OnLeftButtonUpEvent(wxMouseEvent &event);
	void OnLeftButtonDownEvent(wxMouseEvent &event);
	void OnMiddleButtonUpEvent(wxMouseEvent &event);

	void OnMouseLeaveWindowEvent(wxMouseEvent &event);
	void OnDoubleClickEvent(wxMouseEvent &event);

	ZoomBox *mZoomBox;
	PlotCursor *mLeftCursor;
	PlotCursor *mRightCursor;
	Legend *mLegend;

	bool mDraggingLeftCursor = false;
	bool mDraggingRightCursor = false;
	bool mDraggingLegend = false;

	void ComputePrettyLimits(double &min, double &max,
		const unsigned int& maxTicks) const;
	void UpdateLegendAnchor();

	void CreatePlotContextMenu(const wxPoint &position,
		const PlotContext &context);

	GuiInterface& mGuiInterface;

	void ProcessZoom(wxMouseEvent &event);
	void ProcessZoomWithBox(wxMouseEvent &event);
	void ProcessPan(wxMouseEvent &event);

	void PanBottomXAxis(wxMouseEvent &event);
	void PanLeftYAxis(wxMouseEvent &event);
	void PanRightYAxis(wxMouseEvent &event);

	void ProcessPlotAreaDoubleClick(const unsigned int &x);
	void ProcessOffPlotDoubleClick(const unsigned int &x,
		const unsigned int &y);

	void ProcessRightClick(wxMouseEvent &event);
	void ProcessZoomBoxEnd();

	void ForcePointWithinPlotArea(unsigned int &x, unsigned int &y);

	std::string GetDefaultVertexShader() const override
	{ return mDefaultVertexShader; }

	struct Zoom
	{
		double xMin;
		double xMax;
		double xMajor;

		double leftYMin;
		double leftYMax;
		double leftYMajor;

		double rightYMin;
		double rightYMax;
		double rightYMajor;
	};

	std::stack<Zoom> mZoom;
	void UndoZoom();
	bool ZoomChanged() const;

	bool mIgnoreNextMouseMove = false;
	CurveQuality mCurveQuality = CurveQuality::AlwaysHigh;

	Eigen::Matrix4d mLeftModelview;
	Eigen::Matrix4d mRightModelview;

	// Function pointers for working with plot values
	ScalingFunction mXScaleFunction = DoLineaerScale;
	ScalingFunction mLeftYScaleFunction = DoLineaerScale;
	ScalingFunction mRightYScaleFunction = DoLineaerScale;

	// The event IDs
	enum EventIDs
	{
		idPlotContextCopy,
		idPlotContextPaste,
		idPlotContextMajorGridlines,
		idPlotContextMinorGridlines,
		idPlotContextShowLegend,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,
		idPlotContextExportData,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextBottomMajorGridlines,// Maintain this order for each axis' context IDs
		idPlotContextBottomMinorGridlines,
		idPlotContextAutoScaleBottom,
		idPlotContextSetBottomRange,
		idPlotContextSetBottomMajorResolution,
		idPlotContextBottomLogarithmic,
		idPlotContextEditBottomLabel,

		/*idPlotContextTopMajorGridlines,
		idPlotContextTopMinorGridlines,
		idPlotContextAutoScaleTop,
		idPlotContextSetTopRange,
		idPlotContextSetTopMajorResolution,
		idPlotContextTopLogarithmic,
		idPlotContextEditTopLabel,*/

		idPlotContextLeftMajorGridlines,
		idPlotContextLeftMinorGridlines,
		idPlotContextAutoScaleLeft,
		idPlotContextSetLeftRange,
		idPlotContextSetLeftMajorResolution,
		idPlotContextLeftLogarithmic,
		idPlotContextEditLeftLabel,

		idPlotContextRightMajorGridlines,
		idPlotContextRightMinorGridlines,
		idPlotContextAutoScaleRight,
		idPlotContextSetRightRange,
		idPlotContextSetRightMajorResolution,
		idPlotContextRightLogarithmic,
		idPlotContextEditRightLabel
	};

	std::unique_ptr<wxMenu> CreateAxisContextMenu(
		const unsigned int &baseEventId) const;
	std::unique_ptr<wxMenu> CreatePlotAreaContextMenu() const;

	// Context menu events
	void ContextCopy(wxCommandEvent &event);
	void ContextPaste(wxCommandEvent &event);
	void ContextToggleMajorGridlines(wxCommandEvent &event);
	void ContextToggleMinorGridlines(wxCommandEvent &event);
	void ContextToggleLegend(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);
	void ContextExportData(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleMajorGridlinesBottom(wxCommandEvent &event);
	void ContextToggleMinorGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);
	void ContextSetMajorResolutionBottom(wxCommandEvent &event);
	void ContextSetLogarithmicBottom(wxCommandEvent &event);
	void ContextEditBottomLabel(wxCommandEvent &event);

	void ContextToggleMajorGridlinesLeft(wxCommandEvent &event);
	void ContextToggleMinorGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);
	void ContextSetMajorResolutionLeft(wxCommandEvent &event);
	void ContextSetLogarithmicLeft(wxCommandEvent &event);
	void ContextEditLeftLabel(wxCommandEvent &event);

	void ContextToggleMajorGridlinesRight(wxCommandEvent &event);
	void ContextToggleMinorGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	void ContextSetMajorResolutionRight(wxCommandEvent &event);
	void ContextSetLogarithmicRight(wxCommandEvent &event);
	void ContextEditRightLabel(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE()
};

/// Specialization for enabling bitwise operations for the CurveQuality
/// enumeration.
template<>
struct EnableBitwiseOperators<PlotRenderer::CurveQuality>
{
	/// Flag that indicates that bitwise operators should be enabled.
	static constexpr bool mEnable = true;
};

}// namespace LibPlot2D

#endif// PLOT_RENDERER_H_
