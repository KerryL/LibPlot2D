/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotListGrid.cpp
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Derived from wxGrid, this class displays information about plot
//        curves and provides an interface (via right-clicks) for users to
//        modify and perform actions on curves.

// Local headers
#include "lp2d/gui/plotListGrid.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/renderer/color.h"

namespace LibPlot2D
{

//==========================================================================
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
//==========================================================================
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
	EVT_MENU(idButtonRemoveCurve,					PlotListGrid::ButtonRemoveCurveClickedEvent)
	EVT_MENU(idContextBitMask,						PlotListGrid::ContextBitMaskEvent)
	EVT_MENU(idContextTimeShift,					PlotListGrid::ContextTimeShiftEvent)

	EVT_MENU(idContextFilter,						PlotListGrid::ContextFilterEvent)
	EVT_MENU(idContextFitCurve,						PlotListGrid::ContextFitCurve)
END_EVENT_TABLE();

PlotListGrid::PlotListGrid(wxWindow *parent, wxWindowID id) : wxGrid(parent, id)
{
	Build();
}

void PlotListGrid::Build()
{
	BeginBatch();

	CreateGrid(0, colCount, wxGrid::wxGridSelectRows);
	SetRowLabelSize(0);
	SetColFormatNumber(colLineSize);
	SetColFormatNumber(colMarkerSize);
	SetColFormatFloat(colLeftCursor);
	SetColFormatFloat(colRightCursor);
	SetColFormatFloat(colDifference);
	SetColFormatBool(colVisible);
	SetColFormatBool(colRightAxis);

	SetColLabelValue(colName, _T("Curve"));
	SetColLabelValue(colColor, _T("Color"));
	SetColLabelValue(colLineSize, _T("Line"));
	SetColLabelValue(colMarkerSize, _T("Marker"));
	SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	SetColLabelValue(colRightCursor, _T("Right Cursor"));
	SetColLabelValue(colDifference, _T("Difference"));
	SetColLabelValue(colVisible, _T("Visible"));
	SetColLabelValue(colRightAxis, _T("Right Axis"));

	SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	EnableDragRowSize(false);

	unsigned int i;
	for (i = 1; i < colCount; i++)// Skip the name column
		AutoSizeColLabelSize(i);

	EndBatch();
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	wxMenu *contextMenu = new wxMenu();

	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));
	contextMenu->Append(idContextFRF, _T("Frequency Response"));
	//contextMenu->Append(idContextSetXData, _T("Use as X-Axis"));

	contextMenu->AppendSeparator();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));// FIXME:  Eventually, maybe this can be integrated into the "Add Math Channel" dialog?

	contextMenu->AppendSeparator();

	if (row == 0 && currentFileFormat == FormatGeneric)
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

		contextMenu->Append(idButtonRemoveCurve, _T("Remove Curve"));
	}

	PopupMenu(contextMenu, position);

	delete contextMenu;
	contextMenu = NULL;
}

//==========================================================================
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
//==========================================================================
unsigned int PlotListGrid::AddDataRow(const wxString &name)
{
	unsigned int index = GetNumberRows();
	AppendRows();

	unsigned int maxMarkerSize(5);

	SetCellRenderer(index, colVisible, new wxGridCellBoolRenderer);
	SetCellRenderer(index, colRightAxis, new wxGridCellBoolRenderer);
	SetCellEditor(index, colLineSize, new wxGridCellFloatEditor(1, 1));
	SetCellEditor(index, colMarkerSize, new wxGridCellNumberEditor(-1, maxMarkerSize));

	unsigned int i;
	for (i = 1; i < colCount; i++)
			SetReadOnly(index, i, true);
	SetReadOnly(index, colLineSize, false);
	SetReadOnly(index, colMarkerSize, false);
	SetCellValue(index, colName, name);

	Color color = GetNextColor(index);

	SetCellBackgroundColour(index, colColor, color.ToWxColor());
	SetCellValue(index, colLineSize, _T("1"));
	SetCellValue(index, colMarkerSize, _T("-1"));
	SetCellValue(index, colVisible, _T("1"));

	int width = GetColSize(colName);
	AutoSizeColumn(colName, false);
	if (GetColSize(colName) < width)
		SetColSize(colName, width);

	return index;
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::GridRightClickEvent(wxGridEvent &event)
{
	if (GetSelectedRows().Count() == 0 ||
		!ItemIsInArray(GetSelectedRows(), event.GetRow()))
		SelectRow(event.GetRow());

	CreateGridContextMenu(event.GetPosition() + GetPosition()
		+ GetParent()->GetPosition(), event.GetRow());
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::GridDoubleClickEvent(wxGridEvent &event)
{
	// No color for Time
	int row = event.GetRow();
	if (row == 0)
		return;

	if (event.GetCol() != colColor)
	{
		event.Skip();
		return;
	}

	wxColourData colorData;
	colorData.SetColour(GetCellBackgroundColour(row, colColor));

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
	dialog.SetTitle(_T("Choose Line Color"));
	if (dialog.ShowModal() == wxID_OK)
    {
        colorData = dialog.GetColourData();
		SetCellBackgroundColour(row, colColor, colorData.GetColour());
		UpdateCurveProperties(row - 1);
	}
}

//==========================================================================
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
//==========================================================================
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
	if (row == 0 || (event.GetCol() != colVisible && event.GetCol() != colRightAxis))
		return;

	if (GetCellValue(row, event.GetCol()).Cmp(_T("1")) == 0)
		SetCellValue(row, event.GetCol(), wxEmptyString);
	else
		SetCellValue(row, event.GetCol(), _T("1"));

	ShowAppropriateXLabel();

	UpdateCurveProperties(row - 1);
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::GridCellChangeEvent(wxGridEvent &event)
{
	unsigned int row(event.GetRow());
	if (row == 0 || (event.GetCol() != colLineSize && event.GetCol() != colMarkerSize))
	{
		event.Skip();
		UpdateLegend();// Included in case of text changes
		plotArea->Refresh();
		return;
	}

	UpdateCurveProperties(row - 1);
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::GridLabelRightClickEvent(wxGridEvent &event)
{
	wxMenu *contextMenu = new wxMenu();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));

	PopupMenu(contextMenu, event.GetPosition() + GetPosition()
		+ GetParent()->GetPosition());

	delete contextMenu;
	contextMenu = NULL;
}

//==========================================================================
// Class:			PlotListGrid
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties.
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotListGrid::UpdateCurveProperties(const unsigned int &index)
{
	Color color;
	color.Set(GetCellBackgroundColour(index + 1, colColor));
	UpdateCurveProperties(index, color,
		!GetCellValue(index + 1, colVisible).IsEmpty(),
		!GetCellValue(index + 1, colRightAxis).IsEmpty());
}

//==========================================================================
// Class:			PlotListGrid
// Function:		UpdateCurveProperties
//
// Description:		Updates the specified curve properties to match the arguments.
//
// Input Arguments:
//		index		= const unsigned int&
//		color		= const Color&
//		visible		= const bool&
//		rightAxis	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotListGrid::UpdateCurveProperties(const unsigned int &index,
	const Color &color, const bool &visible, const bool &rightAxis)
{
	double lineSize;
	long markerSize;
	UpdateLegend();// Must come first in order to be updated simultaneously with line
	GetCellValue(index + 1, colLineSize).ToDouble(&lineSize);
	GetCellValue(index + 1, colMarkerSize).ToLong(&markerSize);
	plotArea->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
	plotArea->SaveCurrentZoom();
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextAddMathChannelEvent(wxCommandEvent& WXUNUSED(event))
{
	DisplayMathChannelDialog(wxString::Format("[%i]", GetSelectedRows()[0]));
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextFRFEvent(wxCommandEvent& WXUNUSED(event))
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	wxArrayString descriptions;
	int i;
	for (i = 1; i < GetNumberRows(); i++)
		descriptions.Add(GetCellValue(i, 0));

	FRFDialog dialog(this, descriptions);
	if (dialog.ShowModal() != wxID_OK)
		return;

	Dataset2D *amplitude = new Dataset2D, *phase = NULL, *coherence = NULL;

	if (dialog.GetComputePhase())
		phase = new Dataset2D;
	if (dialog.GetComputeCoherence())
		coherence = new Dataset2D;

	if (!PlotMath::XDataConsistentlySpaced(*plotList[dialog.GetInputIndex()]) ||
		!PlotMath::XDataConsistentlySpaced(*plotList[dialog.GetOutputIndex()]))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	FastFourierTransform::ComputeFRF(*plotList[dialog.GetInputIndex()],
		*plotList[dialog.GetOutputIndex()], dialog.GetNumberOfAverages(),
		FastFourierTransform::WindowHann, dialog.GetModuloPhase(), *amplitude, phase, coherence);

	AddFFTCurves(factor, amplitude, phase, coherence, wxString::Format("[%u] to [%u]",
		dialog.GetInputIndex(), dialog.GetOutputIndex()));
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextCreateSignalEvent(wxCommandEvent& WXUNUSED(event))
{
	double startTime(0.0);// [sec]
	double duration(10.0);// [sec]
	double sampleRate(100.0);// [Hz]

	double factor(1.0);
	if (plotList.GetCount() > 0)
	{
		GetXAxisScalingFactor(factor);

		// Use first curve to pull time and frequency information
		sampleRate = 1.0 / PlotMath::GetAverageXSpacing(*plotList[0]) * factor;
		startTime = plotList[0]->GetXData(0) / factor;
		duration = plotList[0]->GetXData(plotList[0]->GetNumberOfPoints() - 1) / factor - startTime;
	}

	CreateSignalDialog dialog(this, startTime, duration, sampleRate);

	if (dialog.ShowModal() != wxID_OK)
		return;

	AddCurve(&dialog.GetSignal()->MultiplyXData(factor), dialog.GetSignalName());

	// Set time units if it hasn't been done already
	double dummy;
	if (!GetXAxisScalingFactor(dummy))
	{
		genericXAxisLabel = _T("Time [sec]");
		SetXDataLabel(genericXAxisLabel);
		//plotArea->SaveCurrentZoom();// TODO:  Is this necessary?
	}
}

//==========================================================================
// Class:			PlotListGrid
// Function:		AddFFTCurves
//
// Description:		Adds the FFT curves to the plot list.
//
// Input Arguments:
//		xFactor	= const double& scaling factor to convert X units to Hz
//		amplitude	= Dataset2D*
//		phase		= Dataset2D*
//		coherence	= Dataset2D*
//		namePortion	= const wxString& identifying the input/output signals
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotListGrid::AddFFTCurves(const double& xFactor,
	Dataset2D *amplitude, Dataset2D *phase,
	Dataset2D *coherence, const wxString &namePortion)
{
	AddCurve(&(amplitude->MultiplyXData(xFactor)), _T("FRF Amplitude, ") + namePortion + _T(", [dB]"));
	SetMarkerSize(GetNumberRows() - 2, 0);

	if (phase)
	{
		AddCurve(&(phase->MultiplyXData(xFactor)), _T("FRF Phase, ") + namePortion + _T(", [deg]"));
		SetMarkerSize(GetNumberRows() - 2, 0);
	}

	if (coherence)
	{
		AddCurve(&(coherence->MultiplyXData(xFactor)), _T("FRF Coherence, ") + namePortion + _T(", [-]"));
		SetMarkerSize(GetNumberRows() - 2, 0);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextSetTimeUnitsEvent(wxCommandEvent& WXUNUSED(event))
{
	double f;
	wxString units;

	// Check to see if we already have some confidence in our x-axis units
	if (GetXAxisScalingFactor(f, &units))
	{
		// Ask the user to confirm, since we don't think we need their help
		if (wxMessageBox(_T("Time units are being interpreted as ") + units +
			_T(", are you sure you want to change them?"), _T("Are You Sure?"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxNO)
			return;
	}

	// Ask the user to specify the correct units
	wxString userUnits;
	userUnits = ::wxGetTextFromUser(_T("Specify time units (e.g. \"msec\" or \"minutes\")"),
		_T("Specify Units"), _T("seconds"), this);

	// If the user cancelled, we will have a blank string
	if (userUnits.IsEmpty())
		return;

	// Check to make sure we understand what the user specified
	wxString currentLabel(GetCellValue(0, colName));
	genericXAxisLabel = _T("Time, [") + userUnits + _T("]");
	SetXDataLabel(genericXAxisLabel);
	if (!GetXAxisScalingFactor(f, &units))
	{
		// Set the label back to what it used to be and warn the user
		SetXDataLabel(currentLabel);
		wxMessageBox(_T("Could not understand units \"") + userUnits + _T("\"."), _T("Error Setting Units"), wxICON_ERROR, this);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextScaleXDataEvent(wxCommandEvent& WXUNUSED(event))
{
	double factor(0.0);
	wxString factorText(_T("0.0"));

	while (!factorText.ToDouble(&factor) || factor == 0.0)
	{
		factorText = ::wxGetTextFromUser(_T("Specify scaling factor:"),
		_T("Specify Factor"), _T("1"), this);
		if (factorText.IsEmpty())
			return;
	}

	wxArrayInt selectedRows = GetSelectedRows();
	unsigned int i;

	// If applied to the row 0, apply to all curves
	if (selectedRows.Count() == 1 && selectedRows[0] == 0)
	{
		unsigned int stopIndex(plotList.GetCount());
		for (i = 0; i < stopIndex; i++)
		{
			Dataset2D *scaledData = new Dataset2D(*plotList[i]);
			scaledData->MultiplyXData(factor);
			AddCurve(scaledData, GetCellValue(i + 1, colName));

			SetCellBackgroundColour(i + stopIndex + 1, colColor,
				GetCellBackgroundColour(i + 1, colColor));
			SetCellValue(i + stopIndex + 1, colLineSize,
				GetCellValue(i + 1, colLineSize));
			SetCellValue(i + stopIndex + 1, colMarkerSize,
				GetCellValue(i + 1, colMarkerSize));
			SetCellValue(i + stopIndex + 1, colVisible,
				GetCellValue(i + 1, colVisible));
			SetCellValue(i + stopIndex + 1, colRightAxis,
				GetCellValue(i + 1, colRightAxis));

			UpdateCurveProperties(i + stopIndex);
		}

		for (i = stopIndex; i > 0; i--)
			RemoveCurve(i - 1);
	}
	// If applied to any other row, apply only to that row (by duplicating curve)
	else
	{
		for (i = 0; i < selectedRows.Count(); i++)
		{
			Dataset2D *scaledData = new Dataset2D(*plotList[selectedRows[i] - 1]);
			scaledData->MultiplyXData(factor);
			AddCurve(scaledData, GetCellValue(selectedRows[i], colName)
				+ wxString::Format(", X-scaled by %f", factor));
		}
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextPlotDerivativeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the derivative of dataset and add it to the plot
	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		Dataset2D *newData = new Dataset2D(
			DiscreteDerivative::ComputeTimeHistory(*plotList[row - 1]));

		wxString name = _T("d/dt(") + GetCellValue(row, colName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextPlotIntegralEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the integral of dataset and add it to the plot
	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		Dataset2D *newData = new Dataset2D(
			DiscreteIntegral::ComputeTimeHistory(*plotList[row - 1]));

		wxString name = _T("integral(") + GetCellValue(row, colName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextPlotRMSEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		Dataset2D *newData = new Dataset2D(
			RootMeanSquare::ComputeTimeHistory(*plotList[row - 1]));

		wxString name = _T("RMS(") + GetCellValue(row, colName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextPlotFFTEvent(wxCommandEvent& WXUNUSED(event))
{
	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		Dataset2D *newData = GetFFTData(plotList[row - 1]);
		if (!newData)
			continue;

		wxString name = _T("FFT(") + GetCellValue(row, colName) + _T(")");
		AddCurve(newData, name);
		SetMarkerSize(GetNumberRows() - 2, 0);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextBitMaskEvent(wxCommandEvent& WXUNUSED(event))
{
	unsigned long bit;
	wxString bitString = wxGetTextFromUser(_T("Specify the bit to plot:"), _T("Bit Seleciton"), _T("0"), this);
	if (bitString.IsEmpty())
		return;
	else if (!bitString.ToULong(&bit))
	{
		wxMessageBox(_T("Bit value must be a positive integer."), _T("Bit Selection Error"), wxICON_ERROR, this);
		return;
	}

	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		Dataset2D *newData = new Dataset2D(
			PlotMath::ApplyBitMask(*plotList[row - 1], bit));

		wxString name = GetCellValue(row, colName) + _T(", Bit ") + wxString::Format("%lu", bit);
		AddCurve(newData, name);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextTimeShiftEvent(wxCommandEvent& WXUNUSED(event))
{
	double shift(0.0);
	wxString shiftText = ::wxGetTextFromUser(
		_T("Specify the time to add to time data in original data:\n")
		_T("Use same units as time series.  Positive values shift curve to the right."),
		_T("Time Shift"), _T("0"), this);

	if (!shiftText.ToDouble(&shift) || shift == 0.0)
		return;

	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		Dataset2D *newData = new Dataset2D(*plotList[row - 1]);

		newData->XShift(shift);

		wxString name = GetCellValue(row, colName) + _T(", t = t0 + ");
		name += shiftText;
		AddCurve(newData, name);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextFilterEvent(wxCommandEvent& WXUNUSED(event))
{
	// Display dialog
	FilterParameters filterParameters = DisplayFilterDialog();
	if (filterParameters.order == 0)
		return;

	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		const Dataset2D *currentData = plotList[row - 1];
		Dataset2D *newData = new Dataset2D(*currentData);

		ApplyFilter(filterParameters, *newData);

		wxString name = FilterDialog::GetFilterNamePrefix(filterParameters) + _T(" (") + GetCellValue(row, colName) + _T(")");
		AddCurve(newData, name);
	}
}

//==========================================================================
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
//==========================================================================
void PlotListGrid::ContextFitCurve(wxCommandEvent& WXUNUSED(event))
{
	// Ask the user what order to use for the polynomial
	unsigned long order;
	wxString orderString = ::wxGetTextFromUser(_T("Specify the order of the polynomial fit:"),
		_T("Polynomial Curve Fit"), _T("2"), this);

	// If cancelled, the orderString will be empty.  It is possible that the user cleared the textbox
	// and clicked OK, but we'll ignore this case since we can't tell the difference
	if (orderString.IsEmpty())
		return;

	if (!orderString.ToULong(&order))
	{
		wxMessageBox(_T("ERROR:  Order must be a positive integer!"), _T("Error Fitting Curve"), wxICON_ERROR, this);
		return;
	}

	unsigned int i, row;
	for (i = 0; i < GetSelectedRows().Count(); i++)
	{
		row = GetSelectedRows()[i];
		wxString name;
		Dataset2D* newData = GetCurveFitData(order, plotList[row - 1], name);

		AddCurve(newData, name);
	}
}

//==========================================================================
// Class:			PlotListGrid
// Function:		GetCurveFitData
//
// Description:		Fits a curve of the specified order to the specified data
//					and returns a dataset containing the curve.
//
// Input Arguments:
//		order	= const unsigned int&
//		data	= const Dataset2D*
//
// Output Arguments:
//		name	= wxString&
//
// Return Value:
//		::Dataset2D*
//
//==========================================================================
Dataset2D* PlotListGrid::GetCurveFitData(const unsigned int &order,
	const Dataset2D* data, wxString &name) const
{
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*data, order);

	Dataset2D *newData = new Dataset2D(*data);
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = CurveFit::EvaluateFit(newData->GetXData(i), fitData);

	name = GetCurveFitName(fitData, GetSelectedRows()[0]);

	delete [] fitData.coefficients;

	return newData;
}

//==========================================================================
// Class:			PlotListGrid
// Function:		GetCurveFitName
//
// Description:		Determines an appropriate name for a curve fit dataset.
//
// Input Arguments:
//		fitData	= const CurveFit::PolynomialFit&
//		row		= const unsigned int& specifying the dataset ID that was fit
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString indicating the name for the fit
//
//==========================================================================
wxString PlotListGrid::GetCurveFitName(const CurveFit::PolynomialFit &fitData,
	const unsigned int &row) const
{
	wxString name, termString;
	//name.Printf("Order %lu Fit([%i]), R^2 = %0.2f", order, row, fitData.rSquared);
	name.Printf("Fit [%i] (R^2 = %0.2f): ", row, fitData.rSquared);

	unsigned int i;
	for (i = 0; i <= fitData.order; i++)
	{
		if (i == 0)
			termString.Printf("%1.2e", fitData.coefficients[i]);
		else if (i == 1)
			termString.Printf("%0.2ex", fabs(fitData.coefficients[i]));
		else
			termString.Printf("%0.2ex^%i", fabs(fitData.coefficients[i]), i);

		if (i < fitData.order)
		{
			if (fitData.coefficients[i + 1] > 0.0)
				termString.Append(_T(" + "));
			else
				termString.Append(_T(" - "));
		}
		name.Append(termString);
	}

	return name;
}

//==========================================================================
// Class:			PlotListGrid
// Function:		AddTimeRowToGrid
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
//==========================================================================
void PlotListGrid::AddTimeRowToGrid()
{
	AppendRows();

	SetXDataLabel(currentFileFormat);

	unsigned int i;
	for (i = 0; i < colCount; i++)
		SetReadOnly(0, i, true);
}

//==========================================================================
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
//==========================================================================
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

//==========================================================================
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
//==========================================================================
bool PlotListGrid::ItemIsInArray(const wxArrayInt& a, const int& item)
{
	unsigned int i;
	for (i = 0; i < a.Count(); i++)
	{
		if (a[i] == item)
			return true;
	}

	return false;
}

//==========================================================================
// Class:			PlotListGrid
// Function:		DisplayMathChannelDialog
//
// Description:		Displays an input dialog that allows the user to enter a
//					math expression.  If an expression is entered, it attempts
//					to add the channel.
//
// Input Arguments:
//		defaultInput	= wxString (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotListGrid::DisplayMathChannelDialog(wxString defaultInput)
{
	// Display input dialog in which user can specify the math desired
	wxString message(_T("Enter the math you would like to perform:\n\n"));
	message.Append(_T("    Use [x] notation to specify channels, where x = 0 is Time, x = 1 is the first data channel, etc.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, %, ddt, int, fft and trigonometric functions.\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, this));
}

//==========================================================================
// Class:			PlotListGrid
// Function:		DisplayAxisRangeDialog
//
// Description:		Displays an input dialog that allows the user to set the
//					range for an axis.
//
// Input Arguments:
//		axis	= const PlotContext& specifying the axis which is to be resized
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotListGrid::DisplayAxisRangeDialog(const PlotContext &axis)
{
	double min, max;
	if (!GetCurrentAxisRange(axis, min, max))
		return;

	RangeLimitsDialog dialog(this, min, max);
	if (dialog.ShowModal() != wxID_OK)
		return;

	// Get the new limits (and correct if they entered the larger value in the min box)
	if (dialog.GetMinimum() < dialog.GetMaximum())
	{
		min = dialog.GetMinimum();
		max = dialog.GetMaximum();
	}
	else
	{
		max = dialog.GetMinimum();
		min = dialog.GetMaximum();
	}

	// Make sure the limits aren't equal
	if (min == max)
	{
		wxMessageBox(_T("ERROR:  Limits must unique!"), _T("Error Setting Limits"), wxICON_ERROR, this);
		return;
	}

	SetNewAxisRange(axis, min, max);
	plotArea->SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotListGrid
// Function:		DisplayFilterDialog
//
// Description:		Dispalys a dialog box allowing the user to specify a filter,
//					returns the specified parameters.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		FilterParameters describing the user-specified filter (order = 0 for cancelled dialog)
//
//==========================================================================
FilterParameters PlotListGrid::DisplayFilterDialog()
{
	FilterDialog dialog(this);
	if (dialog.ShowModal() != wxID_OK)
	{
		FilterParameters parameters;
		parameters.order = 0;
		return parameters;
	}

	return dialog.GetFilterParameters();
}

//==========================================================================
// Class:			PlotListGrid
// Function:		ApplyFilter
//
// Description:		Applies the specified filter to the specified dataset.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//		data		= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotListGrid::ApplyFilter(const FilterParameters &parameters,
	Dataset2D &data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	if (!PlotMath::XDataConsistentlySpaced(data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	Filter *filter = GetFilter(parameters, factor / data.GetAverageDeltaX(), data.GetYData(0));

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetYPointer()[i] = filter->Apply(data.GetYData(i));

	// For phaseless filter, re-apply the same filter backwards
	if (parameters.phaseless)
	{
		data.Reverse();
		filter->Initialize(data.GetYData(0));
		for (i = 0; i < data.GetNumberOfPoints(); i++)
			data.GetYPointer()[i] = filter->Apply(data.GetYData(i));
		data.Reverse();
	}

	delete filter;
}

//==========================================================================
// Class:			PlotListGrid
// Function:		GetFilter
//
// Description:		Returns a filter matching the specified parameters.
//
// Input Arguments:
//		parameters		= const FilterParameters&
//		sampleRate		= const double& [Hz]
//		initialValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Filter*
//
//==========================================================================
Filter* PlotListGrid::GetFilter(const FilterParameters &parameters,
	const double &sampleRate, const double &initialValue) const
{
	return new Filter(sampleRate,
		Filter::CoefficientsFromString(std::string(parameters.numerator.mb_str())),
		Filter::CoefficientsFromString(std::string(parameters.denominator.mb_str())),
		initialValue);
}

}// namespace LibPlot2D