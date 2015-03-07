/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotRenderer.h
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Derived from RenderWindow, this class is used to display plots on
//				 the screen.
// History:

#ifndef _PLOT_RENDERER_H_
#define _PLOT_RENDERER_H_

// Standard C++ headers
#include <stack>
#include <vector>

// Local headers
#include "renderer/renderWindow.h"
#include "renderer/primitives/legend.h"

// wxWidgets forward declarations
class wxString;

// Local forward declarations
class PlotObject;
class MainFrame;
class Dataset2D;
class ZoomBox;
class PlotCursor;

class PlotRenderer : public RenderWindow
{
public:
	// Constructor
	PlotRenderer(wxWindow *parent, wxWindowID id, int args[], MainFrame &_mainFrame);

	// Destructor
	~PlotRenderer();

	// Gets properties for actors
	bool GetBottomMajorGrid(void) const;
	bool GetLeftMajorGrid(void) const;
	bool GetRightMajorGrid(void) const;

	bool GetBottomMinorGrid(void) const;
	bool GetLeftMinorGrid(void) const;
	bool GetRightMinorGrid(void) const;

	Color GetGridColor(void) const;

	double GetXMin(void) const;
	double GetXMax(void) const;
	double GetLeftYMin(void) const;
	double GetLeftYMax(void) const;
	double GetRightYMin(void) const;
	double GetRightYMax(void) const;

	bool GetXLogarithmic(void) const;
	bool GetLeftLogarithmic(void) const;
	bool GetRightLogarithmic(void) const;

	bool GetXAxisZoomed(void) const;

	// Sets properties for actors
	void SetMajorGridOn(void);
	void SetMajorGridOff(void);
	void SetMinorGridOn(void);
	void SetMinorGridOff(void);

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
		const bool &visible, const bool &rightAxis, const unsigned int &lineSize,
		const int &markerSize);
	void SetXLimits(const double &min, const double &max);
	void SetLeftYLimits(const double &min, const double &max);
	void SetRightYLimits(const double &min, const double &max);

	void SetXLabel(wxString text);
	void SetLeftYLabel(wxString text);
	void SetRightYLabel(wxString text);
	void SetTitle(wxString text);

	wxString GetXLabel(void) const;
	wxString GetLeftYLabel(void) const;
	wxString GetRightYLabel(void) const;
	wxString GetTitle(void) const;

	void AddCurve(const Dataset2D &data);
	void RemoveAllCurves(void);
	void RemoveCurve(const unsigned int &index);

	void AutoScale(void);
	void AutoScaleBottom(void);
	void AutoScaleLeft(void);
	void AutoScaleRight(void);

	void SetXLogarithmic(const bool &log);
	void SetLeftLogarithmic(const bool &log);
	void SetRightLogarithmic(const bool &log);

	bool GetMajorGridOn(void);
	bool GetMinorGridOn(void);
	
	bool LegendIsVisible(void);
	void SetLegendOn(void);
	void SetLegendOff(void);
	void UpdateLegend(const std::vector<Legend::LegendEntryInfo> &entries);

	// Called to update the screen
	void UpdateDisplay(void);

	bool GetLeftCursorVisible(void) const;
	bool GetRightCursorVisible(void) const;
	double GetLeftCursorValue(void) const;
	double GetRightCursorValue(void) const;

	void UpdateCursors(void);

	MainFrame *GetMainFrame(void) { return &mainFrame; };

	void SaveCurrentZoom(void);
	void ClearZoomStack(void);

private:
	// Called from the PlotRenderer constructor only in order to initialize the display
	void CreateActors(void);

	// The actors necessary to create the plot
	PlotObject *plot;

	// Parent window
	MainFrame &mainFrame;

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

protected:
	void ProcessZoom(wxMouseEvent &event);
	void ProcessZoomWithBox(wxMouseEvent &event);
	void ProcessPan(wxMouseEvent &event);

	void PanBottomXAxis(wxMouseEvent &event);
	void PanLeftYAxis(wxMouseEvent &event);
	void PanRightYAxis(wxMouseEvent &event);

	void ProcessPlotAreaDoubleClick(const unsigned int &x);
	void ProcessOffPlotDoubleClick(const unsigned int &x, const unsigned int &y);

	void ProcessRightClick(wxMouseEvent &event);
	void ProcessZoomBoxEnd(void);

	void ForcePointWithinPlotArea(unsigned int &x, unsigned int &y);

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
	void UndoZoom(void);
	bool ZoomChanged(void) const;

	bool ignoreNextMouseMove;

	// For the event table
	DECLARE_EVENT_TABLE()
};

#endif// _PLOT_RENDERER_H_
