/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotListGrid.h
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Derived from wxGrid, this class displays information about plot
//        curves and provides an interface (via right-clicks) for users to
//        modify and perform actions on curves.

#ifndef PLOT_LIST_GRID_H_

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

class PlotListGrid : public wxGrid
{
public:
	PlotListGrid(wxWindow *parent, wxWindowID id = wxID_ANY);

	// For displaying a menu that was crated by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateGridContextMenu(const wxPoint &position, const unsigned int &row);

	Color GetNextColor(const unsigned int &index) const;
	void AddTimeRow();
	unsigned int AddDataRow(const wxString &name);

private:
	void Build();

	void UpdateCurveProperties(const unsigned int &index,
		const LibPlot2D::Color &color, const bool &visible,
		const bool &rightAxis);
	void UpdateCurveProperties(const unsigned int &index);

	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);
	FilterParameters DisplayFilterDialog();
	void ApplyFilter(const FilterParameters &parameters, Dataset2D &data);

	Dataset2D *GetCurveFitData(const unsigned int &order, const Dataset2D* data, wxString &name) const;
	wxString GetCurveFitName(const CurveFit::PolynomialFit &fitData, const unsigned int &row) const;

	static bool ItemIsInArray(const wxArrayInt& a, const int& item);

	enum Columns
	{
		colName = 0,
		colColor,
		colLineSize,
		colMarkerSize,
		colLeftCursor,
		colRightCursor,
		colDifference,
		colVisible,
		colRightAxis,

		colCount
	};

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
		idContextTimeShift,
		idContextBitMask,

		idContextFilter,
		idContextFitCurve,
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
	void ContextTimeShiftEvent(wxCommandEvent &event);
	void ContextBitMaskEvent(wxCommandEvent &event);

	void ContextFilterEvent(wxCommandEvent &event);
	void ContextFitCurve(wxCommandEvent &event);
};

}// namespace LibPlot2D

#endif// PLOT_LIST_GRID_H_
