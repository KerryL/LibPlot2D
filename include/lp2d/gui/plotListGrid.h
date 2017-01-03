/*=============================================================================
                                   LibPlot2D
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
	enum class Column
	{
		Name = 0,
		Color,
		LineSize,
		MarkerSize,
		LeftCursor,
		RightCursor,
		Difference,
		Visible,
		RightAxis,

		Count
	};

protected:
	GuiInterface& mGuiInterface;

	void CreateGridContextMenu(const wxPoint& position,
		const unsigned int& row);

	// The event IDs
	enum EventIDs
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
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);
	void GridLabelRightClickEvent(wxGridEvent &event);

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextFRFEvent(wxCommandEvent &event);
	void ContextCreateSignalEvent(wxCommandEvent &event);
	void ContextSetTimeUnitsEvent(wxCommandEvent &event);
	void ContextScaleXDataEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);
	void ContextRemoveCurveEvent(wxCommandEvent &event);
	void ContextTimeShiftEvent(wxCommandEvent &event);
	void ContextBitMaskEvent(wxCommandEvent &event);

	void ContextFilterEvent(wxCommandEvent &event);
	void ContextFitCurve(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()

private:
	void Build();

	static bool ItemIsInArray(const wxArrayInt& a, const int& item);
};

}// namespace LibPlot2D

#endif// PLOT_LIST_GRID_H_
