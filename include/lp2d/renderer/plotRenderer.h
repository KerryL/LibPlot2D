/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotRenderer.h
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Derived from RenderWindow, this class is used to display plots on
//				 the screen.
// History:

#ifndef PLOT_RENDERER_H_
#define PLOT_RENDERER_H_

// Standard C++ headers
#include <stack>
#include <vector>
#include <cmath>
#include <memory>

// Local headers
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/renderer/primitives/legend.h"

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

class PlotRenderer : public RenderWindow
{
public:
	PlotRenderer(GuiInterface& guiInterface, wxWindow &wxParent, wxWindowID id,
		const wxGLAttributes& attr);
	~PlotRenderer() = default;

	// Gets properties for actors
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

	bool GetXAxisZoomed() const;

	// Sets properties for actors
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

	void SetCurveProperties(const unsigned int &index, const Color &color,
		const bool &visible, const bool &rightAxis, const double &lineSize,
		const int &markerSize);
	void SetXLimits(const double &min, const double &max);
	void SetLeftYLimits(const double &min, const double &max);
	void SetRightYLimits(const double &min, const double &max);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	wxString GetXLabel() const;
	wxString GetLeftYLabel() const;
	wxString GetRightYLabel() const;
	wxString GetTitle() const;

	void AddCurve(const Dataset2D &data);
	void RemoveAllCurves();
	void RemoveCurve(const unsigned int &index);

	void AutoScale();
	void AutoScaleBottom();
	void AutoScaleLeft();
	void AutoScaleRight();

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	bool GetMajorGridOn() const;
	bool GetMinorGridOn() const;
	
	bool LegendIsVisible() const;
	void SetLegendOn();
	void SetLegendOff();
	void UpdateLegend(const std::vector<Legend::LegendEntryInfo> &entries);

	// Called to update the screen
	void UpdateDisplay();

	bool GetLeftCursorVisible() const;
	bool GetRightCursorVisible() const;
	double GetLeftCursorValue() const;
	double GetRightCursorValue() const;

	void UpdateCursors();

	void SaveCurrentZoom();
	void ClearZoomStack();

	static const unsigned int maxXTicks;
	static const unsigned int maxYTicks;
	static double ComputeTickSpacing(const double &min, const double &max, const int &maxTicks);

	enum CurveQuality
	{
		QualityAlwaysLow = 0,
		QualityHighWrite = 1 << 0,
		QualityHighDrag = 1 << 1,
		QualityHighStatic = 1 << 2,
		QualityAlwaysHigh = QualityHighWrite | QualityHighDrag | QualityHighStatic
	};

	void SetCurveQuality(const CurveQuality& curveQuality);
	CurveQuality GetCurveQuality() const { return curveQuality; }

	unsigned long long GetTotalPointCount() const;

	wxImage GetImage() const;

	virtual unsigned int GetVertexDimension() const { return 2; }

	enum Modelview
	{
		ModelviewFixed,
		ModelviewLeft,
		ModelviewRight
	};

	void LoadModelviewUniform(const Modelview& mv);

	void SetLeftModelview(const Matrix& m) { leftModelview = m; }
	void SetRightModelview(const Matrix& m) { rightModelview = m; }

	// Scaling functions for handling differences in linear and logarithmic axis scales
	inline double DoXScale(const double& value) { return xScaleFunction(value); }
	inline double DoLeftYScale(const double& value) { return leftYScaleFunction(value); }
	inline double DoRightYScale(const double& value) { return rightYScaleFunction(value); }

	typedef double (*ScalingFunction)(const double&);
	ScalingFunction GetXScaleFunction() { return xScaleFunction; }
	ScalingFunction GetLeftYScaleFunction() { return leftYScaleFunction; }
	ScalingFunction GetRightYScaleFunction() { return rightYScaleFunction; }

	static inline double DoLineaerScale(const double& value) { return value; }
	static inline double DoLogarithmicScale(const double& value) { return log10(value); }

	enum PlotContext
	{
		PlotContextXAxis,
		PlotContextLeftYAxis,
		PlotContextRightYAxis,
		PlotContextPlotArea
	};

	bool GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const;
	void SetNewAxisRange(const PlotContext &axis, const double &min, const double &max);

private:
	static const std::string defaultVertexShader;

	// Called from the PlotRenderer constructor only in order to initialize the display
	void CreateActors();

	std::unique_ptr<PlotObject> plot;

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

	ZoomBox *zoomBox;
	PlotCursor *leftCursor;
	PlotCursor *rightCursor;
	Legend *legend;

	bool draggingLeftCursor;
	bool draggingRightCursor;
	bool draggingLegend;

	void ComputePrettyLimits(double &min, double &max, const unsigned int& maxTicks) const;
	void UpdateLegendAnchor();

	void CreatePlotContextMenu(const wxPoint &position, const PlotContext &context);

protected:
	GuiInterface& guiInterface;

	void ProcessZoom(wxMouseEvent &event);
	void ProcessZoomWithBox(wxMouseEvent &event);
	void ProcessPan(wxMouseEvent &event);

	void PanBottomXAxis(wxMouseEvent &event);
	void PanLeftYAxis(wxMouseEvent &event);
	void PanRightYAxis(wxMouseEvent &event);

	void ProcessPlotAreaDoubleClick(const unsigned int &x);
	void ProcessOffPlotDoubleClick(const unsigned int &x, const unsigned int &y);

	void ProcessRightClick(wxMouseEvent &event);
	void ProcessZoomBoxEnd();

	void ForcePointWithinPlotArea(unsigned int &x, unsigned int &y);

	virtual std::string GetDefaultVertexShader() const { return defaultVertexShader; }

	// Use RenderWindow::glModelViewMatrix for window-fixed things like axes and gridlines
	float glModelViewMatrixLeftAxis[16];
	float glModelViewMatrixRightAxis[16];

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

	std::stack<Zoom> zoom;
	void UndoZoom();
	bool ZoomChanged() const;

	bool ignoreNextMouseMove;
	CurveQuality curveQuality;

	Matrix leftModelview;
	Matrix rightModelview;

	// Function pointers for working with plot values
	ScalingFunction xScaleFunction;
	ScalingFunction leftYScaleFunction;
	ScalingFunction rightYScaleFunction;

	void DoCopy();
	void DoPaste();

	// The event IDs
	enum MainFrameEventID
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

	std::unique_ptr<wxMenu> CreateAxisContextMenu(const unsigned int &baseEventId) const;
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

}// namespace LibPlot2D

#endif// PLOT_RENDERER_H_
