/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Main frame for the application.
// History:

#ifndef _MAIN_FRAME_H_
#define _MAIN_FRAME_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/dataset2D.h"
#include "utilities/managedList.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// Local forward declarations
class PlotRenderer;
struct FilterParameters;

// The main frame class
class MainFrame : public wxFrame
{
public:
	// Constructor
	MainFrame();

	// Destructor
	~MainFrame();

	// For getting a open/save file name from the user
	wxArrayString GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
		wxString defaultFileName, wxString wildcard, long style);

	// Load a plot from file
	bool LoadFile(wxString pathAndFileName);

	enum PlotContext
	{
		plotContextXAxis,
		plotContextLeftYAxis,
		plotContextRightYAxis,
		plotContextPlotArea
	};

	bool GetXAxisScalingFactor(double &factor, wxString *label = NULL);

	void CreatePlotContextMenu(const wxPoint &position, const PlotContext &context);
	void DisplayAxisRangeDialog(const PlotContext &axis);

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

private:
	// Functions that do some of the frame initialization and control positioning
	void DoLayout(void);
	void SetProperties(void);

	// Controls
	wxButton *openButton;
	wxButton *autoScaleButton;
	wxButton *removeCurveButton;
	wxGrid *optionsGrid;
	wxBoxSizer *topSizer;

	enum Columns
	{
		colName = 0,
		colColor,
		colSize,
		colLeftCursor,
		colRightCursor,
		colDifference,
		colVisible,
		colRightAxis,

		colCount
	};

	// For displaying a menu that was crated by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateGridContextMenu(const wxPoint &position, const unsigned int &row);

	void ClearAllCurves(void);
	void AddCurve(wxString mathString);
	void AddCurve(Dataset2D *data, wxString name);
	void RemoveCurve(const unsigned int &i);

	ManagedList<const Dataset2D> plotList;

	// The event IDs
	enum MainFrameEventID
	{
		// Menu bar
		idButtonOpen = wxID_HIGHEST + 100,
		idButtonAutoScale,
		idButtonRemoveCurve,

		idContextAddMathChannel,
		idContextSetTimeUnits,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,

		idContextFilter,

		idContextFitCurve,

		idPlotContextToggleGridlines,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextToggleBottomGridlines,
		idPlotContextSetBottomRange,
		idPlotContextAutoScaleBottom,

		/*idPlotContextToggleTopGridlines,
		idPlotContextSetTopRange,
		idPlotContextAutoScaleTop,*/

		idPlotContextToggleLeftGridlines,
		idPlotContextSetLeftRange,
		idPlotContextAutoScaleLeft,

		idPlotContextToggleRightGridlines,
		idPlotContextSetRightRange,
		idPlotContextAutoScaleRight
	};

	// Event handlers-----------------------------------------------------
	// Buttons
	void ButtonOpenClickedEvent(wxCommandEvent &event);
	void ButtonAutoScaleClickedEvent(wxCommandEvent &event);
	void ButtonRemoveCurveClickedEvent(wxCommandEvent &event);

	// Grid events
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextSetTimeUnitsEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);

	void ContextFilterEvent(wxCommandEvent &event);

	void ContextFitCurve(wxCommandEvent &event);

	void ContextToggleGridlines(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);

	void ContextToggleGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);

	void ContextToggleGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);
	FilterParameters DisplayFilterDialog(void);
	void ApplyFilter(const FilterParameters &parameters, Dataset2D &data);

	// The main control
	PlotRenderer *plotArea;

	// The menu and status bars
	wxMenuBar *menuBar;

	// Load file methods
	bool LoadTxtFile(wxString pathAndFileName);
	bool LoadCsvFile(wxString pathAndFileName);
	bool LoadGenericDelimitedFile(wxString pathAndFileName);
	bool LoadBaumullerFile(wxString pathAndFileName);
	bool LoadKollmorgenFile(wxString pathAndFileName);
	wxArrayString ParseLineIntoColumns(wxString line, const wxString &delimiter);

	enum FileFormat
	{
		FormatBaumuller,
		FormatKollmorgen,
		FormatFFT,
		FormatGeneric
	};

	FileFormat currentFileFormat;

	void SetTitleFromFileName(wxString pathAndFileName);
	void SetXDataLabel(wxString label);
	void SetXDataLabel(const FileFormat &format);
	wxString genericXAxisLabel;

	// For the event table
	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations(void);
};

#endif// _MAIN_FRAME_H_