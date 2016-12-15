/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  plotListGrid.cpp
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Derived from wxGrid, this class displays information about plot
//        curves and provides an interface (via right-clicks) for users to
//        modify and perform actions on curves.

// Local headers
#include "lp2d/gui/plotListGrid.h"
#include "lp2d/gui/guiInterface.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/renderer/color.h"

// wxWidgets headers
#include <wx/colordlg.h>

// Standard C++ headers
#include <memory>

namespace LibPlot2D
{

//=============================================================================
// Class:			PlotListGrid
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
BEGIN_EVENT_TABLE(PlotListGrid, wxGrid)
	// Grid events
	EVT_GRID_CELL_RIGHT_CLICK(PlotListGrid::GridRightClickEvent)
	EVT_GRID_CELL_LEFT_DCLICK(PlotListGrid::GridDoubleClickEvent)
	EVT_GRID_CELL_LEFT_CLICK(PlotListGrid::GridLeftClickEvent)
	EVT_GRID_CELL_CHANGED(PlotListGrid::GridCellChangeEvent)
	EVT_GRID_LABEL_RIGHT_CLICK(PlotListGrid::GridLabelRightClickEvent)

	// Context menu
	EVT_MENU(idContextAddMathChannel,				PlotListGrid::ContextAddMathChannelEvent)
	EVT_MENU(idContextFRF,							PlotListGrid::ContextFRFEvent)
	EVT_MENU(idContextCreateSignal,					PlotListGrid::ContextCreateSignalEvent)
	EVT_MENU(idContextSetTimeUnits,					PlotListGrid::ContextSetTimeUnitsEvent)
	EVT_MENU(idContextScaleXData,					PlotListGrid::ContextScaleXDataEvent)
	EVT_MENU(idContextPlotDerivative,				PlotListGrid::ContextPlotDerivativeEvent)
	EVT_MENU(idContextPlotIntegral,					PlotListGrid::ContextPlotIntegralEvent)
	EVT_MENU(idContextPlotRMS,						PlotListGrid::ContextPlotRMSEvent)
	EVT_MENU(idContextPlotFFT,						PlotListGrid::ContextPlotFFTEvent)
	EVT_MENU(idContextRemoveCurve,					PlotListGrid::ContextRemoveCurveEvent)
	EVT_MENU(idContextBitMask,						PlotListGrid::ContextBitMaskEvent)
	EVT_MENU(idContextTimeShift,					PlotListGrid::ContextTimeShiftEvent)

	EVT_MENU(idContextFilter,						PlotListGrid::ContextFilterEvent)
	EVT_MENU(idContextFitCurve,						PlotListGrid::ContextFitCurve)
END_EVENT_TABLE();

PlotListGrid::PlotListGrid(GuiInterface& guiInterface, wxWindow *parent,
	wxWindowID id) : wxGrid(parent, id), guiInterface(guiInterface)
{
	Build();
	guiInterface.SetPlotListGrid(this);
}

void PlotListGrid::Build()
{
	BeginBatch();

	CreateGrid(0, ColCount, wxGrid::wxGridSelectRows);
	SetRowLabelSize(0);
	SetColFormatNumber(ColLineSize);
	SetColFormatNumber(ColMarkerSize);
	SetColFormatFloat(ColLeftCursor);
	SetColFormatFloat(ColRightCursor);
	SetColFormatFloat(ColDifference);
	SetColFormatBool(ColVisible);
	SetColFormatBool(ColRightAxis);

	SetColLabelValue(ColName, _T("Curve"));
	SetColLabelValue(ColColor, _T("Color"));
	SetColLabelValue(ColLineSize, _T("Line"));
	SetColLabelValue(ColMarkerSize, _T("Marker"));
	SetColLabelValue(ColLeftCursor, _T("Left Cursor"));
	SetColLabelValue(ColRightCursor, _T("Right Cursor"));
	SetColLabelValue(ColDifference, _T("Difference"));
	SetColLabelValue(ColVisible, _T("Visible"));
	SetColLabelValue(ColRightAxis, _T("Right Axis"));

	SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	EnableDragRowSize(false);

	unsigned int i;
	for (i = 1; i < ColCount; ++i)// Skip the name column
		AutoSizeColLabelSize(i);

	EndBatch();
}

//=============================================================================
// Class:			PlotListGrid
// Function:		CreateGridContextMenu
//
// Description:		Displays a context menu for the grid control.
//
// Input Arguments:
//		position	= const wxPoint& specifying the position to display the menu
//		row			= const unsigned int& specifying the row that was clicked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	std::unique_ptr<wxMenu> contextMenu(std::make_unique<wxMenu>());

	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));
	contextMenu->Append(idContextFRF, _T("Frequency Response"));
	//contextMenu->Append(idContextSetXData, _T("Use as X-Axis"));

	contextMenu->AppendSeparator();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));// FIXME:  Eventually, maybe this can be integrated into the "Add Math Channel" dialog?

	contextMenu->AppendSeparator();

	if (row == 0 && guiInterface.GetCurrentFileFormat() == GuiInterface::FormatGeneric)
	{
		contextMenu->Append(idContextSetTimeUnits, _T("Set Time Units"));
		contextMenu->Append(idContextScaleXData, _T("Scale X-Data"));
	}
	else if (row > 0)
	{
		contextMenu->Append(idContextPlotDerivative, _T("Plot Derivative"));
		contextMenu->Append(idContextPlotIntegral, _T("Plot Integral"));
		contextMenu->Append(idContextPlotRMS, _T("Plot RMS"));
		contextMenu->Append(idContextPlotFFT, _T("Plot FFT"));
		contextMenu->Append(idContextTimeShift, _T("Plot Time-Shifted"));
		contextMenu->Append(idContextScaleXData, _T("Plot Time-Scaled"));
		contextMenu->Append(idContextBitMask, _T("Plot Bit"));

		contextMenu->AppendSeparator();

		contextMenu->Append(idContextFilter, _T("Filter Curve"));
		contextMenu->Append(idContextFitCurve, _T("Fit Curve"));

		contextMenu->AppendSeparator();

		contextMenu->Append(idContextRemoveCurve, _T("Remove Curve"));
	}

	PopupMenu(contextMenu.get(), position);
}

//=============================================================================
// Class:			PlotListGrid
// Function:		AddDataRowToGrid
//
// Description:		Adds the entry for the data to the options grid.
//
// Input Arguments:
//		name	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int specifying the index of the new data
//
//=============================================================================
unsigned int PlotListGrid::AddDataRow(const wxString &name)
{
	unsigned int index = GetNumberRows();
	AppendRows();

	unsigned int maxMarkerSize(5);

	SetCellRenderer(index, ColVisible, new wxGridCellBoolRenderer);
	SetCellRenderer(index, ColRightAxis, new wxGridCellBoolRenderer);
	SetCellEditor(index, ColLineSize, new wxGridCellFloatEditor(1, 1));
	SetCellEditor(index, ColMarkerSize, new wxGridCellNumberEditor(-1, maxMarkerSize));

	unsigned int i;
	for (i = 1; i < ColCount; ++i)
			SetReadOnly(index, i, true);
	SetReadOnly(index, ColLineSize, false);
	SetReadOnly(index, ColMarkerSize, false);
	SetCellValue(index, ColName, name);

	Color color = GetNextColor(index);

	SetCellBackgroundColour(index, ColColor, color.ToWxColor());
	SetCellValue(index, ColLineSize, _T("1"));
	SetCellValue(index, ColMarkerSize, _T("-1"));
	SetCellValue(index, ColVisible, _T("1"));

	int width = GetColSize(ColName);
	AutoSizeColumn(ColName, false);
	if (GetColSize(ColName) < width)
		SetColSize(ColName, width);

	return index;
}

//=============================================================================
// Class:			PlotListGrid
// Function:		GridRightClickEvent
//
// Description:		Handles right-click events on the grid control.  Displays
//					context menu.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::GridRightClickEvent(wxGridEvent &event)
{
	if (GetSelectedRows().Count() == 0 ||
		!ItemIsInArray(GetSelectedRows(), event.GetRow()))
		SelectRow(event.GetRow());

	CreateGridContextMenu(event.GetPosition(), event.GetRow());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		GridDoubleClickEvent
//
// Description:		Handles double click event for the grid control.  If the
//					click occurs on the color box, display the color dialog.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::GridDoubleClickEvent(wxGridEvent &event)
{
	// No color for Time
	int row = event.GetRow();
	if (row == 0)
		return;

	if (event.GetCol() != ColColor)
	{
		event.Skip();
		return;
	}

	wxColourData colorData;
	colorData.SetColour(GetCellBackgroundColour(row, ColColor));

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Line Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
        colorData = dialog.GetColourData();
		SetCellBackgroundColour(row, ColColor, colorData.GetColour());
		guiInterface.UpdateCurveProperties(row - 1);
	}
}

//=============================================================================
// Class:			PlotListGrid
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for boolean controls).
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::GridLeftClickEvent(wxGridEvent &event)
{
	unsigned int row = event.GetRow();

	// This stuff may be necessary after bug is fixed with wxGrid::GetSelectedRows()?
	// See ButtonRemoveCurveClickedEvent() for details
	//SetSelectionMode(wxGrid::wxGridSelectRows);
	//SelectRow(row, event.ControlDown());

	// Skip to handle row selection (with SHIFT and CTRL) and also boolean column click handlers
	event.Skip();

	// Was this click in one of the boolean columns and not in the time row?
	if (row == 0 || (event.GetCol() != ColVisible && event.GetCol() != ColRightAxis))
		return;

	if (GetCellValue(row, event.GetCol()).Cmp(_T("1")) == 0)
		SetCellValue(row, event.GetCol(), wxEmptyString);
	else
		SetCellValue(row, event.GetCol(), _T("1"));

	guiInterface.ShowAppropriateXLabel();

	guiInterface.UpdateCurveProperties(row - 1);
}

//=============================================================================
// Class:			PlotListGrid
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for text controls).
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::GridCellChangeEvent(wxGridEvent &event)
{
	unsigned int row(event.GetRow());
	if (row == 0 || (event.GetCol() != ColLineSize && event.GetCol() != ColMarkerSize))
	{
		event.Skip();
		guiInterface.UpdateLegend();// Included in case of text changes
		guiInterface.RefreshRenderer();
		return;
	}

	guiInterface.UpdateCurveProperties(row - 1);
}

//=============================================================================
// Class:			PlotListGrid
// Function:		GridLabelRightClickEvent
//
// Description:		Handles right-click events in blank areas of grid control.
//
// Input Arguments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::GridLabelRightClickEvent(wxGridEvent &event)
{
	std::unique_ptr<wxMenu> contextMenu(std::make_unique<wxMenu>());

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));

	PopupMenu(contextMenu.get(), event.GetPosition());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextAddMathChannelEvent
//
// Description:		Adds a user-defined math channel to the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextAddMathChannelEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.DisplayMathChannelDialog(wxString::Format("[%i]", GetSelectedRows()[0]));
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextFRFEvent
//
// Description:		Event handler for context menu transfer function events.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextFRFEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.GenerateFRF();
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextCreateSignalEvent
//
// Description:		Displays dialog for creating various signals.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextCreateSignalEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.CreateSignal();
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextSetTimeUnitsEvent
//
// Description:		Available for the user to clarify the time units when we
//					are unable to determine them easily from the input file.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextSetTimeUnitsEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.SetTimeUnits();
}
//=============================================================================
// Class:			PlotListGrid
// Function:		ContextScaleXDataEvent
//
// Description:		Scales the X-data by the specified factor.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextScaleXDataEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.ScaleXData(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the derivative of the selected grid
//					row to the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextPlotDerivativeEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.PlotDerivative(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextPlotIntegralEvent
//
// Description:		Adds a curve showing the integral of the selected grid
//					row to the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextPlotIntegralEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.PlotIntegral(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextPlotRMSEvent
//
// Description:		Adds a curve showing the RMS of the selected grid
//					row to the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextPlotRMSEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.PlotRMS(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextPlotFFTEvent
//
// Description:		Adds a curve showing the FFT of the selected grid
//					row to the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextPlotFFTEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.PlotFFT(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextRemoveCurveEvent
//
// Description:		Removes the curve from the list.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextRemoveCurveEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.RemoveCurves(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextBitMaskEvent
//
// Description:		Creates bit mask for the specified curve.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextBitMaskEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.BitMask(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextTimeShiftEvent
//
// Description:		Adds a new curve equivalent to the selected curve shifted
//					by the specified amount.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextTimeShiftEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.TimeShift(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextFilterEvent
//
// Description:		Displays a dialog allowing the user to specify the filter,
//					and adds the filtered curve to the plot.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextFilterEvent(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.FilterCurves(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ContextFitCurve
//
// Description:		Fits a curve to the dataset selected in the grid control.
//					User is asked to specify the order of the fit.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::ContextFitCurve(wxCommandEvent& WXUNUSED(event))
{
	guiInterface.FitCurves(GetSelectedRows());
}

//=============================================================================
// Class:			PlotListGrid
// Function:		AddTimeRow
//
// Description:		Adds the entry for the time data to the options grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotListGrid::AddTimeRow()
{
	AppendRows();

	guiInterface.SetXDataLabel(guiInterface.GetCurrentFileFormat());

	unsigned int i;
	for (i = 0; i < ColCount; ++i)
		SetReadOnly(0, i, true);
}

//=============================================================================
// Class:			PlotListGrid
// Function:		GetNextColor
//
// Description:		Determines the next color to use (cycles through all the
//					pre-defined colors).
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Color to sue
//
//=============================================================================
Color PlotListGrid::GetNextColor(const unsigned int &index) const
{
	unsigned int colorIndex = (index - 1) % 12;
	if (colorIndex == 0)
		return Color::GetColorHSL(0.0, 1.0, 0.5);// (red)
	else if (colorIndex == 1)
		return Color::GetColorHSL(2.0 / 3.0, 1.0, 0.5);// (blue)
	else if (colorIndex == 2)
		return Color::GetColorHSL(1.0 / 3.0, 1.0, 0.5);// (green)
	else if (colorIndex == 3)
		return Color::GetColorHSL(0.125, 1.0, 0.5);// (gold)
	else if (colorIndex == 4)
		return Color::GetColorHSL(0.5, 0.5, 0.5);// (teal)
	else if (colorIndex == 5)
		return Color::GetColorHSL(5.0 / 6.0, 1.0, 0.5);// (magenta)
	else if (colorIndex == 6)
		return Color::GetColorHSL(0.0, 0.5, 0.6);// (reddish brown)
	else if (colorIndex == 7)
		return Color::GetColorHSL(0.73, 0.5, 0.5);// (purple)
	else if (colorIndex == 8)
		return Color::GetColorHSL(1.0 / 3.0, 0.5, 0.5);// (dark green)
	else if (colorIndex == 9)
		return Color::GetColorHSL(1.0 / 6.0, 0.3, 0.5);// (gold brown)
	else if (colorIndex == 10)
		return Color::GetColorHSL(0.875, 0.5, 0.5);// (light purple)
	else if (colorIndex == 11)
		return Color::ColorBlack;
	else
		assert(false);

	return Color::ColorBlack;
}

//=============================================================================
// Class:			PlotListGrid
// Function:		ItemIsInArray
//
// Description:		Checks to see if the specified value is contained in the
//					referenced array.
//
// Input Arguments:
//		a		= const wxArrayInt&
//		item	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if item is in the array, false otherwise
//
//=============================================================================
bool PlotListGrid::ItemIsInArray(const wxArrayInt& a, const int& item)
{
	for (const auto& i : a)
	{
		if (i == item)
			return true;
	}

	return false;
}

}// namespace LibPlot2D
