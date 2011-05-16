/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  main_frame_class.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Main frame for the application.
// History:

#ifndef _MAIN_FRAME_CLASS_H_
#define _MAIN_FRAME_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/dataset2D.h"
#include "utilities/managed_list_class.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// Local forward declarations
class PlotRenderer;

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

	MANAGED_LIST<const Dataset2D> plotList;

	// The event IDs
	enum MainFrameEventID
	{
		// Menu bar
		idButtonOpen = wxID_HIGHEST + 100,
		idButtonAutoScale,
		idButtonRemoveCurve,

		idContextAddMathChannel,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,

		idContextFilterLowPass,
		idContextFilterHighPass,

		idContextFitCurve,

		idPlotContextToggleGridlines,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,

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

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);

	void ContextFilterLowPassEvent(wxCommandEvent &event);
	void ContextFilterHighPassEvent(wxCommandEvent &event);

	void ContextFitCurve(wxCommandEvent &event);

	void ContextToggleGridlines(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);

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

	// The main control
	PlotRenderer *plotArea;

	// The menu and status bars
	wxMenuBar *menuBar;

	// Load file methods
	bool LoadTxtFile(wxString pathAndFileName);
	bool LoadCsvFile(wxString pathAndFileName);
	wxArrayString ParseLineIntoColumns(const std::string& line, const wxString &delimiter);

	// For the event table
	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations(void);
};

#endif// _MAIN_FRAME_CLASS_H_