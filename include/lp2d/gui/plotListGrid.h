/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  plotListGrid.h
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Derived from wxGrid, this class displays information about plot
//        curves and provides an interface (via right-clicks) for users to
//        modify and perform actions on curves.

#ifndef PLOT_LIST_GRID_H_
#define PLOT_LIST_GRID_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/grid.h>

// Local headers
#include "lp2d/utilities/signals/curveFit.h"

namespace LibPlot2D
{

// Local forward declarations
class Color;
struct FilterParameters;
class Dataset2D;
class GuiInterface;

class PlotListGrid : public wxGrid
{
public:
	PlotListGrid(GuiInterface& guiInterface, wxWindow* parent,
		wxWindowID id = wxID_ANY);

	~PlotListGrid() = default;

	Color GetNextColor(const unsigned int& index) const;

	void AddTimeRow();
	unsigned int AddDataRow(const wxString& name);

	/// Enumeration of columns contained within the grid.
	enum Columns
	{
		ColName = 0,
		ColColor,
		ColLineSize,
		ColMarkerSize,
		ColLeftCursor,
		ColRightCursor,
		ColDifference,
		ColVisible,
		ColRightAxis,

		ColCount
	};

protected:
	GuiInterface& guiInterface;

	virtual void CreateGridContextMenu(const wxPoint& position, const unsigned int& row);

	// The event IDs
	enum MainFrameEventID
	{
		idContextAddMathChannel = wxID_HIGHEST + 100,
		idContextFRF,

		idContextCreateSignal,

		idContextSetTimeUnits,
		idContextScaleXData,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,
		idContextRemoveCurve,
		idContextTimeShift,
		idContextBitMask,

		idContextFilter,
		idContextFitCurve
	};

	// Grid events
	virtual void GridRightClickEvent(wxGridEvent &event);
	virtual void GridDoubleClickEvent(wxGridEvent &event);
	virtual void GridLeftClickEvent(wxGridEvent &event);
	virtual void GridCellChangeEvent(wxGridEvent &event);
	virtual void GridLabelRightClickEvent(wxGridEvent &event);

	// Context menu events
	virtual void ContextAddMathChannelEvent(wxCommandEvent &event);
	virtual void ContextFRFEvent(wxCommandEvent &event);
	virtual void ContextCreateSignalEvent(wxCommandEvent &event);
	virtual void ContextSetTimeUnitsEvent(wxCommandEvent &event);
	virtual void ContextScaleXDataEvent(wxCommandEvent &event);
	virtual void ContextPlotDerivativeEvent(wxCommandEvent &event);
	virtual void ContextPlotIntegralEvent(wxCommandEvent &event);
	virtual void ContextPlotRMSEvent(wxCommandEvent &event);
	virtual void ContextPlotFFTEvent(wxCommandEvent &event);
	virtual void ContextRemoveCurveEvent(wxCommandEvent &event);
	virtual void ContextTimeShiftEvent(wxCommandEvent &event);
	virtual void ContextBitMaskEvent(wxCommandEvent &event);

	virtual void ContextFilterEvent(wxCommandEvent &event);
	virtual void ContextFitCurve(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()

private:
	void Build();

	static bool ItemIsInArray(const wxArrayInt& a, const int& item);
};

}// namespace LibPlot2D

#endif// PLOT_LIST_GRID_H_
