/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  main_frame_class.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MainFrame class.  Uses wxWidgets for the GUI components.
// History:

// Standard C++ headers
#include <fstream>
#include <vector>
#include <algorithm>

// wxWidgets headers
#include <wx/grid.h>
#include <wx/colordlg.h>

// Local headers
#include "application/main_frame_class.h"
#include "application/plotter_app_class.h"
#include "application/drop_target_class.h"
#include "application/range_limits_dialog_class.h"
#include "renderer/plot_renderer_class.h"
#include "renderer/color_class.h"
#include "utilities/dataset2D.h"
#include "utilities/math/plot_math.h"
#include "utilities/signals/integral.h"
#include "utilities/signals/derivative.h"
#include "utilities/signals/rms.h"
#include "utilities/signals/fft.h"
#include "utilities/math/expression_tree_class.h"
#include "utilities/signals/filters/low_pass_order1_class.h"
#include "utilities/signals/filters/high_pass_order1_class.h"
#include "utilities/signals/curve_fit.h"

//==========================================================================
// Class:			MainFrame
// Function:		MainFrame
//
// Description:		Constructor for MainFrame class.  Initializes the form
//					and creates the controls, etc.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
								 wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
	DoLayout();
	SetProperties();

	// Initialize the file format flag
	currentFileFormat = FormatGeneric;
}

//==========================================================================
// Class:			MainFrame
// Function:		~MainFrame
//
// Description:		Denstructor for MainFrame class.  Frees memory and
//					releases GUI object managers.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainFrame::~MainFrame()
{
}

//==========================================================================
// Class:			MainFrame
// Function:		Constant Declarations
//
// Description:		Constant declarations for the MainFrame class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*#ifdef __WXGTK__
const wxString MainFrame::pathToConfigFile = _T("dataplotter.rc");
#else
const wxString MainFrame::pathToConfigFile = _T("dataplotter.ini");
#endif*/

//==========================================================================
// Class:			MainFrame
// Function:		DoLayout
//
// Description:		Creates sizers and controls and lays them out in the window.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::DoLayout(void)
{
	// Create the top sizer, and on inside of it just to pad the borders a bit
	topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 1, wxGROW | wxALL, 5);

	// Create the main control
	optionsGrid = NULL;// To avoid crashing in UpdateCursors
	plotArea = new PlotRenderer(*this, wxID_ANY);
	plotArea->SetSize(480, 320);
	plotArea->SetGridOn();
	mainSizer->Add(plotArea, 1, wxGROW);

	// Create the options controls and buttons to add/remove math channels
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(lowerSizer);

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);
	lowerSizer->Add(buttonSizer, 0, wxGROW | wxALL, 5);

	unsigned int buttonWidth = 80;
	unsigned int buttonHeight = 25;
	openButton = new wxButton(this, idButtonOpen, _T("Open"), wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	autoScaleButton = new wxButton(this, idButtonAutoScale, _T("Auto Scale"), wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	removeCurveButton = new wxButton(this, idButtonRemoveCurve, _T("Remove"), wxDefaultPosition, wxSize(buttonWidth, buttonHeight));
	buttonSizer->Add(openButton, 0, 0, 0);
	buttonSizer->Add(autoScaleButton, 0, 0, 0);
	buttonSizer->Add(removeCurveButton, 0, 0, 0);

	optionsGrid = new wxGrid(this, wxID_ANY);
	lowerSizer->Add(optionsGrid, 1, wxGROW | wxALL, 5);

	// Configure the grid
	optionsGrid->BeginBatch();

	optionsGrid->CreateGrid(0, colCount, wxGrid::wxGridSelectRows);
	optionsGrid->SetRowLabelSize(0);
	optionsGrid->SetColFormatNumber(colSize);
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colSize, _T("Size"));
	optionsGrid->SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	optionsGrid->SetColLabelValue(colRightCursor, _T("Right Cursor"));
	optionsGrid->SetColLabelValue(colDifference, _T("Difference"));
	optionsGrid->SetColLabelValue(colVisible, _T("Visible"));
	optionsGrid->SetColLabelValue(colRightAxis, _T("Right Axis"));

	optionsGrid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

	optionsGrid->EndBatch();

	// Assign sizers and resize the frame
	SetSizerAndFit(topSizer);
	SetAutoLayout(true);
	topSizer->SetSizeHints(this);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default fonts.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetProperties(void)
{
	SetTitle(DataPlotterApp::dataPlotterTitle);
	SetName(DataPlotterApp::dataPlotterName);
	Center();

	// Add the icon
#ifdef __WXMSW__
    SetIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE, 16, 16));
#endif
	// FIXME:  Linux icons

	// Allow draging-and-dropping of files onto this window to open them
	SetDropTarget(dynamic_cast<wxDropTarget*>(new DropTarget(*this)));

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	// Buttons
	EVT_BUTTON(idButtonOpen,						MainFrame::ButtonOpenClickedEvent)
	EVT_BUTTON(idButtonAutoScale,					MainFrame::ButtonAutoScaleClickedEvent)
	EVT_BUTTON(idButtonRemoveCurve,					MainFrame::ButtonRemoveCurveClickedEvent)

	// Grid control
	EVT_GRID_CELL_RIGHT_CLICK(MainFrame::GridRightClickEvent)
	EVT_GRID_CELL_LEFT_DCLICK(MainFrame::GridDoubleClickEvent)
	EVT_GRID_CELL_LEFT_CLICK(MainFrame::GridLeftClickEvent)
	EVT_GRID_CELL_CHANGE(MainFrame::GridCellChangeEvent)

	// Context menu
	EVT_MENU(idContextAddMathChannel,				MainFrame::ContextAddMathChannelEvent)
	EVT_MENU(idContextPlotDerivative,				MainFrame::ContextPlotDerivativeEvent)
	EVT_MENU(idContextPlotIntegral,					MainFrame::ContextPlotIntegralEvent)
	EVT_MENU(idContextPlotRMS,						MainFrame::ContextPlotRMSEvent)
	EVT_MENU(idContextPlotFFT,						MainFrame::ContextPlotFFTEvent)
	EVT_MENU(idButtonRemoveCurve,					MainFrame::ButtonRemoveCurveClickedEvent)

	EVT_MENU(idContextFilterLowPass,				MainFrame::ContextFilterLowPassEvent)
	EVT_MENU(idContextFilterHighPass,				MainFrame::ContextFilterHighPassEvent)

	EVT_MENU(idContextFitCurve,						MainFrame::ContextFitCurve)

	EVT_MENU(idPlotContextToggleGridlines,			MainFrame::ContextToggleGridlines)
	EVT_MENU(idPlotContextAutoScale,				MainFrame::ContextAutoScale)
	EVT_MENU(idPlotContextWriteImageFile,			MainFrame::ContextWriteImageFile)

	EVT_MENU(idPlotContextBGColor,					MainFrame::ContextPlotBGColor)
	EVT_MENU(idPlotContextGridColor,				MainFrame::ContextGridColor)

	EVT_MENU(idPlotContextToggleBottomGridlines,	MainFrame::ContextToggleGridlinesBottom)
	EVT_MENU(idPlotContextSetBottomRange,			MainFrame::ContextSetRangeBottom)
	EVT_MENU(idPlotContextAutoScaleBottom,			MainFrame::ContextAutoScaleBottom)

	//EVT_MENU(idPlotContextToggleTopGridlines,		MainFrame::)
	//EVT_MENU(idPlotContextSetTopRange,				MainFrame::)
	//EVT_MENU(idPlotContextAutoScaleTop,				MainFrame::)

	EVT_MENU(idPlotContextToggleLeftGridlines,		MainFrame::ContextToggleGridlinesLeft)
	EVT_MENU(idPlotContextSetLeftRange,				MainFrame::ContextSetRangeLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			MainFrame::ContextAutoScaleLeft)

	EVT_MENU(idPlotContextToggleRightGridlines,		MainFrame::ContextToggleGridlinesRight)
	EVT_MENU(idPlotContextSetRightRange,			MainFrame::ContextSetRangeRight)
	EVT_MENU(idPlotContextAutoScaleRight,			MainFrame::ContextAutoScaleRight)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		ButtonOpenClickedEvent
//
// Description:		Displays a dialog asking the user to specify the file to
//					read from.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonOpenClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set up the wildcard specifications
	// (Done here for readability)
	wxString wildcard("All files (*.*)|*.*");
	wildcard.append("|Comma Separated (*.csv)|*.csv");
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	// Get the file name to open from the user
	wxArrayString fileList = GetFileNameFromUser(_T("Open Data File"), wxEmptyString, wxEmptyString,
		wildcard, wxFD_OPEN /*| wxFD_MULTIPLE*/ | wxFD_FILE_MUST_EXIST);

	// Make sure the user didn't cancel
	if (fileList.GetCount() == 0)
		return;

	// Loop to make sure we open all selected files
	unsigned int i;
	for (i = 0; i < fileList.GetCount(); i++)
		LoadFile(fileList[i]);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextWriteImageFile
//
// Description:		Calls the object of interest's write image file method.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextWriteImageFile(wxCommandEvent& WXUNUSED(event))
{
	// Get the file name to open from the user
	wxArrayString pathAndFileName = GetFileNameFromUser(_T("Save Image File"), wxEmptyString, wxEmptyString,
		_T("Bitmap Image (*.bmp)|*.bmp|JPEG Image (*.jpg, *.jpeg)|*.jpg;*.jpeg|PNG Image (*.png)|*.png|TIFF Image (*.tif)|*.tif"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// Make sure the user didn't cancel
	if (pathAndFileName.IsEmpty())
		return;

	// Call the object's write image file method
	plotArea->WriteImageToFile(pathAndFileName[0]);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonAutoScaleClickedEvent
//
// Description:		Event fires when user clicks "AutoScale" button.
//
// Input Argurments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonAutoScaleClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScale();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonRemoveCurveClickedEvent
//
// Description:		Event fires when user clicks "RemoveCurve" button.
//
// Input Argurments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonRemoveCurveClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Known bug with wxGrid::GetSelectedRows() - returns empty set
	// This is the cleanest way to do it, after the bug is fixed
	/*wxArrayInt rows = optionsGrid->GetSelectedRows();

	// Must have row selected
	unsigned int i;
	for (i = 0; i < rows.Count(); i++)
	{
		// Cannot remove time row
		if (rows[i] == 0)
			continue;

		RemoveCurve(rows[i] - 1);
	}*/

	// Workaround for now
	int i;
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (optionsGrid->IsInSelection(i, 0))
			RemoveCurve(i - 1);
	}

	plotArea->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateGridContextMenu
//
// Description:		Displays a context menu for the grid control.
//
// Input Argurments:
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
void MainFrame::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	// Declare the menu variable and get the position of the cursor
	wxMenu *contextMenu = new wxMenu();

	// Start building the context menu
	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));

	if (row > 0)
	{
		contextMenu->Append(idContextPlotDerivative, _T("Plot Derivative"));
		contextMenu->Append(idContextPlotIntegral, _T("Plot Integral"));
		contextMenu->Append(idContextPlotRMS, _T("Plot RMS"));
		contextMenu->Append(idContextPlotFFT, _T("Plot FFT"));

		contextMenu->AppendSeparator();

		wxMenu *filterMenu = new wxMenu();
		filterMenu->Append(idContextFilterLowPass, _T("Low-Pass"));
		filterMenu->Append(idContextFilterHighPass, _T("High-Pass"));
		contextMenu->AppendSubMenu(filterMenu, _T("Filter"));

		contextMenu->Append(idContextFitCurve, _T("Fit Curve"));

		contextMenu->AppendSeparator();

		contextMenu->Append(idButtonRemoveCurve, _T("Remove Curve"));
	}

	// Show the menu
	PopupMenu(contextMenu, position);

	// Delete the context menu object
	delete contextMenu;
	contextMenu = NULL;

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotContextMenu
//
// Description:		Displays a context menu for the plot.
//
// Input Argurments:
//		position	= const wxPoint& specifying the position to display the menu
//		context		= const PlotContext& describing the area of the plot
//					  on which the click occured
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreatePlotContextMenu(const wxPoint &position, const PlotContext &context)
{
	// Declare the menu variable and get the position of the cursor
	wxMenu *contextMenu = new wxMenu();

	// Build the menu
	switch (context)
	{
	case plotContextXAxis:
		contextMenu->Append(idPlotContextToggleBottomGridlines, _T("Toggle Axis Gridlines"));
		contextMenu->Append(idPlotContextAutoScaleBottom, _T("Auto Scale Axis"));
		contextMenu->Append(idPlotContextSetBottomRange, _T("Set Range"));
		break;

	case plotContextLeftYAxis:
		contextMenu->Append(idPlotContextToggleLeftGridlines, _T("Toggle Axis Gridlines"));
		contextMenu->Append(idPlotContextAutoScaleLeft, _T("Auto Scale Axis"));
		contextMenu->Append(idPlotContextSetLeftRange, _T("Set Range"));
		break;

	case plotContextRightYAxis:
		contextMenu->Append(idPlotContextToggleRightGridlines, _T("Toggle Axis Gridlines"));
		contextMenu->Append(idPlotContextAutoScaleRight, _T("Auto Scale Axis"));
		contextMenu->Append(idPlotContextSetRightRange, _T("Set Range"));
		break;

	default:
	case plotContextPlotArea:
		contextMenu->Append(idPlotContextToggleGridlines, _T("Toggle Gridlines"));
		contextMenu->Append(idPlotContextAutoScale, _T("Auto Scale"));
		contextMenu->Append(idPlotContextWriteImageFile, _T("Write Image File"));
		contextMenu->AppendSeparator();
		contextMenu->Append(idPlotContextBGColor, _T("Set Background Color"));
		contextMenu->Append(idPlotContextGridColor, _T("Set Gridline Color"));
		break;
	}

	// Show the menu
	PopupMenu(contextMenu, position);

	// Delete the context menu object
	delete contextMenu;
	contextMenu = NULL;

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Argurments:
//		dialogTitle			= wxString containing the title for the dialog
//		defaultDirectory	= wxString specifying the initial directory
//		defaultFileName		= wxString specifying the default file name
//		wildcard			= wxString specifying the list of file types to
//							  allow the user to select
//		style				= long integer specifying the type of dialog
//							  (this is usually wxFD_OPEN or wxFD_SAVE)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing the paths and file names of the specified files,
//		or and empty array if the user cancels
//
//==========================================================================
wxArrayString MainFrame::GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
										 wxString defaultFileName, wxString wildcard, long style)
{
	// Initialize the return variable
	wxArrayString pathsAndFileNames;

	// Step 1 is to ask the user to specify the file name
	wxFileDialog dialog(this, dialogTitle, defaultDirectory, defaultFileName,
		wildcard, style);

	// Set the dialog to display center screen at the user's home directory
	dialog.CenterOnParent();

	// Display the dialog and make sure the user clicked OK
	if (dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (style & wxFD_OPEN)
			dialog.GetPaths(pathsAndFileNames);
		else
			pathsAndFileNames.Add(dialog.GetPath());
	}

	// Return the path and file name
	return pathsAndFileNames;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadFile
//
// Description:		Public method for loading a single object from file.
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadFile(wxString pathAndFileName)
{
	int startOfExtension;
	wxString fileExtension;

	// Clear out any curves we already have
	// NOTE:  If we ever choose to allow multiple files to be opened, this will need to go
	ClearAllCurves();

	// Decipher the file name to figure out what kind of object this is
	startOfExtension = pathAndFileName.Last('.') + 1;
	fileExtension = pathAndFileName.Mid(startOfExtension);

	// Create the appropriate object
	bool loadedOK(false);
	if (fileExtension.CmpNoCase("csv") == 0)
		loadedOK = LoadCsvFile(pathAndFileName);
	else if (fileExtension.CmpNoCase("txt") == 0)
		loadedOK = LoadTxtFile(pathAndFileName);
	else
		// Attempt to load the file, even if we don't recognize its extension
		//::wxMessageBox(_T("ERROR:  Unrecognized file extension '") + fileExtension + _T("'!"), _T("Error Loading File"));
		loadedOK = LoadGenericDelimitedFile(pathAndFileName);

	// If we couldn't load the file, tell the user
	// Changed 8/4/2011 - Specific messages are displayed in LoadXXXFile() methods
	//if (!loadedOK && displayDialogOnError)
	//	::wxMessageBox(_T("ERROR:  Unable to read data from file!"), _T("Error Loading File"));

	if (loadedOK)
	{
		SetTitleFromFileName(pathAndFileName);
		SetXDataLabel(currentFileFormat);
	}

	return loadedOK;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadTxtFile
//
// Description:		Public method for loading a single object from file.
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadTxtFile(wxString pathAndFileName)
{
	// TODO:  Add any specific file formats here

	// Try to load the file as a generic delimited file
	return LoadGenericDelimitedFile(pathAndFileName);
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadCsvFile
//
// Description:		Loads specific .csv file formats.
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadCsvFile(wxString pathAndFileName)
{
	// Open the file
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);

	if (!file.is_open())
	{
		::wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"));
		return false;
	}

	// Determine the type of the file we're trying to open
	std::string nextLine;
	std::getline(file, nextLine);

	// For compatability with GTK - getline returns string ending with \r under GTK, which we can't have
	wxString compatableNextLine(nextLine);

	if (compatableNextLine.Trim().compare(_T("WinBASS_II_Oscilloscope_Data")) == 0)// Baumuller oscilloscope trace
	{
		file.close();
		return LoadBaumullerFile(pathAndFileName);
	}
	else// Formats the require reading further into the file than the first line
	{
		std::getline(file, nextLine);
		compatableNextLine = nextLine;
		// Formats requiring reading to the second line

		// Kollmorgen format from S600 series drives
		// There may be a better way to check this, but I haven't found it
		if (compatableNextLine.compare(0, 7, _T("MMI vom")) == 0)
		{
			file.close();
			return LoadKollmorgenFile(pathAndFileName);
		}
	}
	// FIXME:  Add Siemens format here?  Useful for assigning readable names instead of .rXX
	// TODO:  Add any other specific file formats here

	file.close();

	// Try to open the file as a generic delimited file
	return LoadGenericDelimitedFile(pathAndFileName);
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadBaumullerFile
//
// Description:		Loads Baumuller data trace (from BM4x00 series drive).
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadBaumullerFile(wxString pathAndFileName)
{
	// Open the file
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);

	if (!file.is_open())
	{
		::wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"));
		return false;
	}

	wxString delimiter(';');

	std::string nextLine;
	std::getline(file, nextLine);

	// For compatability with GTK - getline returns string ending with \r under GTK, which we can't have
	wxString compatableNextLine(nextLine);// FIXME:  I don't think this is used

	// Throw out everything up to "Par.number:"
	// This will give us the number of datasets we need
	while (compatableNextLine.Trim().substr(0, 11).compare(_T("Par.number:")) != 0)
	{
		std::getline(file, nextLine);
		compatableNextLine = nextLine;
	}
	wxArrayString parameterNumbers = ParseLineIntoColumns(nextLine, delimiter);

	std::getline(file, nextLine);
	wxArrayString descriptions = ParseLineIntoColumns(nextLine, delimiter);

	std::getline(file, nextLine);
	wxArrayString units = ParseLineIntoColumns(nextLine, delimiter);

	// Throw out the max and min rows
	std::getline(file, nextLine);
	std::getline(file, nextLine);

	// Allocate datasets
	std::vector<double> *data = new std::vector<double>[parameterNumbers.size()];

	// Start reading data
	wxArrayString parsed;
	unsigned int i;
	double tempDouble;
	while (!file.eof())
	{
		std::getline(file, nextLine);
		parsed = ParseLineIntoColumns(nextLine, delimiter);

		for (i = 0; i < parsed.size(); i++)
		{
			if (!parsed[i].ToDouble(&tempDouble))
			{
				delete [] data;

				::wxMessageBox(_T("ERROR:  Non-numeric entry encounted while parsing file!"), _T("Error Generating Plot"));
				return false;
			}

			data[i].push_back(tempDouble);
		}
	}

	// Put the data in datasets and add them to the plot
	Dataset2D *dataSet;
	unsigned int j;
	for (i = 0; i < parameterNumbers.size() - 1; i++)
	{
		dataSet = new Dataset2D(data[0].size());
		for (j = 0; j < data[0].size(); j++)
		{
			dataSet->GetXPointer()[j] = data[0].at(j);
			dataSet->GetYPointer()[j] = data[i + 1].at(j);
		}

		AddCurve(dataSet, descriptions[i + 1]
			+ _T(" (") + parameterNumbers[i + 1] + _T(") [")
			+ units[i + 1] + _T("]"));
	}

	// Clean up memory
	// Don't delete dataSet -> this is handled by the MANAGED_LIST object
	delete [] data;
	file.close();

	// Set the file format flag
	currentFileFormat = FormatBaumuller;

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadKollmorgenFile
//
// Description:		Loads Kollmorgen data trace (from S600 series drive).
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadKollmorgenFile(wxString pathAndFileName)
{
	// Open the file
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);

	if (!file.is_open())
	{
		::wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"));
		return false;
	}

	wxString delimiter(',');

	std::string nextLine;
	std::getline(file, nextLine);

	// For compatability with GTK - getline returns string ending with \r under GTK, which we can't have
	wxString compatableNextLine(nextLine);// FIXME:  I don't think this is used...

	// Throw out the first two lines
	std::getline(file, nextLine);
	std::getline(file, nextLine);
	compatableNextLine = nextLine;

	// The third line contains the number of datapoints and the samling period in msec
	// We use this information to generate the time series (file does not contain a time series)
	//unsigned int rows = atoi(nextLine.substr(0, nextLine.find_first_of(delimiter)).c_str());
	double samplingPeriod = atof(nextLine.substr(nextLine.find_first_of(delimiter) + 1).c_str()) / 1000.0;// [sec]

	// The fourth line contains the data set labels (which also gives us the number of datasets we need)
	std::getline(file, nextLine);
	wxArrayString descriptions = ParseLineIntoColumns(nextLine, delimiter);

	// Allocate datasets
	std::vector<double> *data = new std::vector<double>[descriptions.size()];

	// Start reading data
	wxArrayString parsed;
	unsigned int i;
	double tempDouble;
	while (!file.eof())
	{
		std::getline(file, nextLine);
		parsed = ParseLineIntoColumns(nextLine, delimiter);

		for (i = 0; i < parsed.size(); i++)
		{
			if (!parsed[i].ToDouble(&tempDouble))
			{
				delete [] data;

				::wxMessageBox(_T("ERROR:  Non-numeric entry encounted while parsing file!"), _T("Error Generating Plot"));
				return false;
			}

			data[i].push_back(tempDouble);
		}
	}

	// Put the data in datasets and add them to the plot
	Dataset2D *dataSet;
	unsigned int j;
	double time;
	for (i = 0; i < descriptions.size(); i++)
	{
		dataSet = new Dataset2D(data[0].size());
		time = 0.0;

		for (j = 0; j < data[0].size(); j++)
		{
			dataSet->GetXPointer()[j] = time;
			dataSet->GetYPointer()[j] = data[i].at(j);

			time += samplingPeriod;
		}

		AddCurve(dataSet, descriptions[i]);
	}
	// FIXME:  Add Siemens format here?  Useful for assigning readable names instead of .rXX
	// TODO:  Add any other specific file formats here

	// Clean up memory
	// Don't delete dataSet -> this is handled by the MANAGED_LIST object
	delete [] data;
	file.close();

	// Set the file format flag
	currentFileFormat = FormatKollmorgen;

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadGenericDelimitedFile
//
// Description:		Attempts to load a generic delimited file by following a
//					standard set of rules:
//					- Assume first column is X-data
//					- Assume there are some non-delimited rows at the top of the
//					  file (skip these)
//					- Assume that once the delimited rows begin, there may be
//					  column headers (sometimes multiple rows)
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadGenericDelimitedFile(wxString pathAndFileName)
{
	// Open the file
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);

	if (!file.is_open())
	{
		::wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"));
		return false;
	}

	// Create a list of acceptable delimiting characters
	// Don't use periods ('.') because we're going to have those in regular numbers
	wxArrayString delimiterList;
	delimiterList.Add(_T(" "));
	delimiterList.Add(_T(","));
	delimiterList.Add(_T("\t"));
	delimiterList.Add(_T(";"));

	// Use std::string to read the file line-by-line
	std::string nextLine;
	std::getline(file, nextLine);

	// Determine which character is the most likely delimiting character
	// Determine how many file descriptor rows there are (and skip them)
	wxString delimiter;
	wxArrayString delimitedLine, plotNameList, previousLines;
	unsigned int i, j;
	double value;
	while (!file.eof())
	{
		// Look for the first row starting with a number that contains at least one
		// delimiting character (check all delimiters in our list)
		for (i = 0; i < delimiterList.size(); i++)
		{
			delimitedLine = ParseLineIntoColumns(nextLine, delimiterList[i]);
			if (delimitedLine.size() > 1)// Delimiters were found
			{
				// If all columns are numeric, this is probably the first row of data
				delimiter = delimiterList[i];
				for (j = 0; j < delimitedLine.size(); j++)
				{
					// Try to convert the string to a double
					if (!delimitedLine[j].ToDouble(&value))
					{
						// Not the row we're looking for if the data isn't numeric
						delimiter.Empty();
						break;
					}
				}

				// If we've found our delimiter, generate names for each plot
				if (!delimiter.IsEmpty())
				{
					// Work backwards line-by-line
					int line;
					wxArrayString delimitedPreviousLine;
					for (line = previousLines.size() - 1; line >= 0; line--)
					{
						delimitedPreviousLine = ParseLineIntoColumns(previousLines[line].c_str(), delimiter);

						// Stop when the number of columns is different
						if (delimitedPreviousLine.size() != delimitedLine.size())
							break;

						// If none of the entries on the line are numeric, prepend the plot name with this text
						bool prependText(true);
						for (j = 0; j < delimitedPreviousLine.size(); j++)
						{
							prependText = !delimitedPreviousLine[j].ToDouble(&value);
							if (!prependText)
								break;
						}

						if (prependText)
						{
							for (j = 0; j < delimitedPreviousLine.size(); j++)
							{
								if (plotNameList.size() < j + 1)
									plotNameList.Add(delimitedPreviousLine[j]);
								else
									plotNameList[j].Prepend(delimitedPreviousLine[j] + _T(", "));
							}
						}
					}
				}
			}

			// Have we determined what the delimiter is?
			if (!delimiter.IsEmpty())
				break;
		}

		// Have we determined what the delimiter is?
		if (!delimiter.IsEmpty())
			break;

		// Store this line for later (for creating plot names)
		previousLines.Add(nextLine);

		// Read the next line
		std::getline(file, nextLine);
	}

	// Find the header rows (if any) and use them to construct plot names
	if (plotNameList.size() == 0)
	{
		wxString dummyPlotName;
		for (i = 0; i < delimitedLine.size(); i++)
		{
			dummyPlotName.Printf("[%i]", i);
			plotNameList.Add(dummyPlotName);
		}
	}
	genericXAxisLabel = plotNameList[0];
	plotNameList.RemoveAt(0);

	// If there are no names in the list at this point, it's because we didn't find any plottable data
	if (plotNameList.size() == 0)
	{
		file.close();
		::wxMessageBox(_T("No plottable data found in file!"), _T("Error Generating Plot"));
		return false;
	}

	// Display a choice dialog allowing the user to specify which plots to import (only if there are more than 6 channels?)
	wxMultiChoiceDialog dialog(this, _T("Select data to plot:"), _T("Select Data"), plotNameList);
	if (dialog.ShowModal() == wxID_CANCEL)
	{
		file.close();
		// No message box - user canceled
		return false;
	}

	// Get the selections and make sure the user selected something
	wxArrayInt choices = dialog.GetSelections();
	if (choices.size() == 0)
	{
		file.close();
		::wxMessageBox(_T("No data selected for plotting!"), _T("Error Generating Plot"));
		return false;
	}

	// Add the selected datasets to the plot
	// Allocate datasets
	std::vector<double> *data = new std::vector<double>[choices.size() + 1];// +1 for time column, which isn't displayed for user to select

	// Start reading data
	while (!file.eof() && !nextLine.empty())
	{
		// Get the X-data
		if (!delimitedLine[0].ToDouble(&value))
		{
			delete [] data;

			::wxMessageBox(_T("ERROR:  Non-numeric entry encounted while parsing file!"), _T("Error Reading File"));
			return false;
		}
		data[0].push_back(value);

		// Get the Y-data
		for (i = 0; i < choices.size(); i++)
		{
			if (!delimitedLine[choices[i] + 1].ToDouble(&value))
			{
				delete [] data;

				::wxMessageBox(_T("ERROR:  Non-numeric entry encounted while parsing file!"), _T("Error Reading File"));
				return false;
			}
			data[i + 1].push_back(value);
		}

		// Read the next line
		std::getline(file, nextLine);
		delimitedLine = ParseLineIntoColumns(nextLine, delimiter);
	}

	// Put the data in datasets and add them to the plot
	Dataset2D *dataSet;
	for (i = 0; i < choices.size(); i++)
	{
		dataSet = new Dataset2D(data[0].size());
		for (j = 0; j < data[0].size(); j++)
		{
			dataSet->GetXPointer()[j] = data[0].at(j);
			dataSet->GetYPointer()[j] = data[i + 1].at(j);
		}

		AddCurve(dataSet, plotNameList[choices[i]]);
	}

	// Clean up memory
	// Don't delete dataSet -> this is handled by the MANAGED_LIST object
	delete [] data;
	file.close();

	// Set the file format flag
	currentFileFormat = FormatGeneric;

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		ParseLineIntoColumns
//
// Description:		Parses the specified line into pieces based on encountering
//					the specified delimiting character (or characters).
//
// Input Argurments:
//		line		= wxString containing the line to parse
//		delimiter	= const wxString& specifying the characters to assume for
//					  delimiting columns
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing one item for every column contained in the
//		original line
//
//==========================================================================
wxArrayString MainFrame::ParseLineIntoColumns(wxString line, const wxString &delimiter)
{
	// Remove \r character from end of line (required for GTK, etc.)
	line.Trim();

	wxArrayString parsed;

	unsigned int start(0);
	unsigned int end(0);

	while (end != std::string::npos && start < line.length())
	{
		// Find the next delimiting character
		end = line.find(delimiter.c_str(), start);

		// If the next delimiting character is right next to the previous character
		// (empty string between), ignore it (that is to say, we treat consecutive
		// delimiters as one)
		if (end == start)
		{
			start++;
			continue;
		}

		// If no character was found, add the rest of the line to the list
		if (end == std::string::npos)
			parsed.Add(line.substr(start));
		else
			parsed.Add(line.substr(start, end - start));

		start = end + 1;
	}

	return parsed;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetTitleFromFileName
//
// Description:		Sets the frame's title according to the specified file name.
//
// Input Argurments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetTitleFromFileName(wxString pathAndFileName)
{
	// Set the name of the form to contain the file name
	unsigned int start;
#ifdef __WXMSW__
	start = pathAndFileName.find_last_of(_T("\\")) + 1;
#else
	start = pathAndFileName.find_last_of(_T("/")) + 1;
#endif
	unsigned int end(pathAndFileName.find_last_of(_T(".")));
	SetTitle(pathAndFileName.Mid(start, end - start) + _T(" - ") + DataPlotterApp::dataPlotterTitle);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ClearAllCurves
//
// Description:		Removes all curves from the plot.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ClearAllCurves(void)
{
	// Remove the curves locally
	while (plotList.GetCount() > 0)
		RemoveCurve(0);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels to the specified string.
//
// Input Argurments:
//		label	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetXDataLabel(wxString label)
{
	optionsGrid->SetCellValue(0, colName, label);
	plotArea->SetXLabel(label);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels according to the opened file type.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetXDataLabel(const FileFormat &format)
{
	switch (format)
	{
	case FormatBaumuller:
		SetXDataLabel(_T("Time [msec]"));
		break;

	case FormatKollmorgen:
		SetXDataLabel(_T("Time [sec]"));
		break;

	case FormatFFT:
		SetXDataLabel(_T("Frequency [Hz]"));
		break;

	default:
	case FormatGeneric:
		SetXDataLabel(genericXAxisLabel);
	}

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds a new dataset to the plot, created by operating on
//					existing datasets.
//
// Input Argurments:
//		mathString	= wxString describing the desired math operations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddCurve(wxString mathString)
{
	// String will be empty if the user cancelled
	if (mathString.IsEmpty())
		return;

	// Parse string and determine what the new dataset should look like
	ExpressionTree expression(plotList);
	Dataset2D *mathChannel = new Dataset2D;

	double xAxisFactor;
	if (!GetXAxisScalingFactor(xAxisFactor))
	{
		// FIXME:  Do we warn the user or not?  This is really only a problem if the user
		// specified FFT or a filter
	}

	wxString errors = expression.Solve(mathString, *mathChannel, xAxisFactor);

	// Check to see if there were any problems solving the tree
	if (!errors.IsEmpty())
	{
		// Tell the user about the errors
		::wxMessageBox(_T("Could not solve expression:\n\n") + errors, _T("Error Solving Expression"));

		DisplayMathChannelDialog(mathString);
		return;
	}

	// Then, add the new dataset to the plot
	AddCurve(mathChannel, mathString.Upper());// FIXME:  Get better name from user

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds an existing dataset to the plot.
//
// Input Argurments:
//		data	= Dataset2D* to add
//		name	= wxString specifying the label for the curve
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddCurve(Dataset2D *data, wxString name)
{
	plotList.Add(data);

	// Handle adding to the grid control
	optionsGrid->BeginBatch();

	// If this is the first curve to be added, add a row for the time, too
	unsigned int i;
	if (optionsGrid->GetNumberRows() == 0)
	{
		optionsGrid->AppendRows();
		
		SetXDataLabel(currentFileFormat);

		for (i = 0; i < colCount; i++)
			optionsGrid->SetReadOnly(0, i, true);
	}
	unsigned int index = optionsGrid->GetNumberRows();
	optionsGrid->AppendRows();

	unsigned int maxLineSize(5);

	optionsGrid->SetCellEditor(index, colVisible, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colRightAxis, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colSize, new wxGridCellNumberEditor(1, maxLineSize));

	// Don't allow the user to change the contents of anything except the boolean cells
	for (i = 0; i < colDifference; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colSize, false);

	// Populate cell values
	optionsGrid->SetCellValue(index, colName, name);

	// Choose next color and set background color of appropriate cell
	unsigned int colorIndex = (index - 1) % 8;
	Color color;
	switch (colorIndex)
	{
	case 0:
		color = Color::ColorBlue;
		break;

	case 1:
		color = Color::ColorRed;
		break;

	case 2:
		color = Color::ColorGreen;
		break;

	case 3:
		color = Color::ColorMagenta;
		break;

	case 4:
		color = Color::ColorCyan;
		break;

	case 5:
		color = Color::ColorYellow;
		break;

	case 6:
		color = Color::ColorGray;
		break;

	default:
	case 7:
		color = Color::ColorBlack;
		break;
	}

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());

	optionsGrid->SetCellValue(index, colSize, _T("1"));

	// Set default boolean values
	optionsGrid->SetCellValue(index, colVisible, _T("1"));

	optionsGrid->AutoSizeColumns();

	optionsGrid->EndBatch();

	// Add the curve to the plot
	plotArea->AddCurve(*data);
	unsigned long size;
	optionsGrid->GetCellValue(index, colSize).ToULong(&size);
	plotArea->SetCurveProperties(index - 1, color, true, false, size);

	plotArea->UpdateDisplay();

	// Resize to prevent scrollbars and hidden values in the grid control
	topSizer->Layout();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveCurve
//
// Description:		Removes a curve from the plot.
//
// Input Argurments:
//		i	= const unsigned int& specifying curve to remove
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveCurve(const unsigned int &i)
{
	// Remove from grid control
	optionsGrid->DeleteRows(i + 1);

	// If this was the last data row, also remove the time row
	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	// Also remove the curve from the plot
	plotArea->RemoveCurve(i);

	// And remove from our local list (calls destructor for the dataset)
	plotList.Remove(i);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		GridRightClickEvent
//
// Description:		Handles right-click events on the grid control.  Displays
//					context menu.
//
// Input Argurments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridRightClickEvent(wxGridEvent &event)
{
	optionsGrid->SelectRow(event.GetRow());
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition(), event.GetRow());

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		GridDoubleClickEvent
//
// Description:		Handles double click event for the grid control.  If the
//					click occurs on the color box, display the color dialog.
//
// Input Argurments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridDoubleClickEvent(wxGridEvent &event)
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
	colorData.SetColour(optionsGrid->GetCellBackgroundColour(row, colColor));

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
    dialog.SetTitle(_T("Choose Line Color"));
    if (dialog.ShowModal() == wxID_OK)
    {
        colorData = dialog.GetColourData();
		optionsGrid->SetCellBackgroundColour(row, colColor, colorData.GetColour());
		Color color;
		color.Set(colorData.GetColour());
		unsigned long size;
		optionsGrid->GetCellValue(row, colSize).ToULong(&size);
		plotArea->SetCurveProperties(row - 1, color,
			!optionsGrid->GetCellValue(row, colVisible).IsEmpty(),
			!optionsGrid->GetCellValue(row, colRightAxis).IsEmpty(), size);
    }

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for boolean controls).
//
// Input Argurments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridLeftClickEvent(wxGridEvent &event)
{
	unsigned int row = event.GetRow();

	// This stuff may be necessary after bug is fixed with wxGrid::GetSelectedRows()?
	// See ButtonRemoveCurveClickedEvent() for details
	//optionsGrid->SetSelectionMode(wxGrid::wxGridSelectRows);
	//optionsGrid->SelectRow(row, event.ControlDown());

	// Skip to handle row selection (with SHIFT and CTRL) and also boolean column click handlers
	event.Skip();

	// Was this click in one of the boolean columns and not in the time row?
	if (row == 0 || (event.GetCol() != colVisible && event.GetCol() != colRightAxis))
		return;

	if (optionsGrid->GetCellValue(row, event.GetCol()).Cmp(_T("1")) == 0)
		optionsGrid->SetCellValue(row, event.GetCol(), wxEmptyString);
	else
		optionsGrid->SetCellValue(row, event.GetCol(), _T("1"));

	// If the only visible curves are FFTs, change the x-label
	int i;
	bool showFFTLabel(false);
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).Cmp(_T("1")) == 0)
		{
			if (optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FFT")) == 0)
				showFFTLabel = true;
			else
			{
				showFFTLabel = false;
				break;
			}
		}
	}
	if (showFFTLabel)
		SetXDataLabel(FormatFFT);
	else
		SetXDataLabel(currentFileFormat);

	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(row, colColor));
	unsigned long size;
	optionsGrid->GetCellValue(row, colSize).ToULong(&size);
	plotArea->SetCurveProperties(row - 1, color,
		!optionsGrid->GetCellValue(row, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(row, colRightAxis).IsEmpty(), size);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		GridLeftClickEvent
//
// Description:		Handles grid cell change events (for text controls).
//
// Input Argurments:
//		event	= wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GridCellChangeEvent(wxGridEvent &event)
{
	// This event is only valid for one column, and then not in the first row
	unsigned int row(event.GetRow());
	if (row == 0 || event.GetCol() != colSize)
		event.Skip();

	// Update all of the line parameters
	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(row, colColor));
	unsigned long size;
	optionsGrid->GetCellValue(row, colSize).ToULong(&size);
	plotArea->SetCurveProperties(row - 1, color,
		!optionsGrid->GetCellValue(row, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(row, colRightAxis).IsEmpty(), size);
}

//==========================================================================
// Class:			MainFrame
// Function:		GetXAxisScalingFactor
//
// Description:		Attempts to determine the scaling factor required to convert
//					the X-axis into seconds (assuming X-axis has units of time).
//
// Input Argurments:
//		None
//
// Output Arguments:
//		factor	= double&, scaling factor
//
// Return Value:
//		bool; true for success, false otherwise
//
//==========================================================================
bool MainFrame::GetXAxisScalingFactor(double &factor)
{
	// Put known file formats here (at the top), to save time and eliminate possibility for error

	// For Baumulelr datasets, multiply x data by 1000 in order to have seconds
	if (currentFileFormat == FormatBaumuller)
		factor = 1000.0;
	else if (currentFileFormat == FormatKollmorgen)
		factor = 1.0;
	else
	{
		// Use time series label to determine units and decide if scaling is necessary
		wxString xSeriesLabel(genericXAxisLabel);

		// We'll recognize the following label formats:
		// X Series Name [unit]
		// X Series Name (unit)
		// X Series Name *delimiter* unit

		// Can we find an appropriate label format?
		unsigned int i;
		wxString unit;
		if (xSeriesLabel.Last() == ']')
		{
			// Step backwards until we find the matching bracket
			for (i = xSeriesLabel.Len() - 2; i >= 0; i--)
			{
				if (xSeriesLabel.at(i) == '[')
				{
					unit = xSeriesLabel.Mid(i + 1, xSeriesLabel.Len() - i - 2);
					break;
				}
			}
		}
		else if (xSeriesLabel.Last() == ')')
		{
			// Step backwards until we find the matching parenthese
			for (i = xSeriesLabel.Len() - 2; i >= 0; i--)
			{
				if (xSeriesLabel.at(i) == '(')
				{
					unit = xSeriesLabel.Mid(i + 1, xSeriesLabel.Len() - i - 2);
					break;
				}
			}
		}
		else// Check for delimiters
		{
			wxArrayString delimiters;
			delimiters.Add(_T(","));
			delimiters.Add(_T(";"));
			delimiters.Add(_T("-"));
			delimiters.Add(_T(":"));

			int location;
			for (i = 0; i < delimiters.size(); i++)
			{
				location = xSeriesLabel.Find(delimiters[i].c_str());
				if (location != wxNOT_FOUND && location < (int)xSeriesLabel.Len() - 1)
				{
					unit = xSeriesLabel.Mid(location + 1);
					break;
				}
			}
		}

		// We'll recognize the following units:
		// h, hr, hours -> factor = 1.0 / 3600.0
		// m, min, minutes -> factor = 1.0 / 60.0
		// s, sec, seconds -> factor = 1.0
		// ms, msec, milliseconds -> factor = 1000.0
		// us, usec, microseconds -> factor = 1000000.0

		// Do we recognize the units?
		if (unit.CmpNoCase(_T("h")) == 0 || unit.CmpNoCase(_T("hr")) == 0 || unit.CmpNoCase(_T("hours")) == 0)
			factor = 1.0 / 3600.0;
		else if (unit.CmpNoCase(_T("m")) == 0 || unit.CmpNoCase(_T("min")) == 0 || unit.CmpNoCase(_T("minutes")) == 0)
			factor = 1.0 / 60.0;
		else if (unit.CmpNoCase(_T("s")) == 0 || unit.CmpNoCase(_T("sec")) == 0 || unit.CmpNoCase(_T("seconds")) == 0)
			factor = 1.0;
		else if (unit.CmpNoCase(_T("ms")) == 0 || unit.CmpNoCase(_T("msec")) == 0 || unit.CmpNoCase(_T("milliseconds")) == 0)
			factor = 1000.0;
		else if (unit.CmpNoCase(_T("us")) == 0 || unit.CmpNoCase(_T("usec")) == 0 || unit.CmpNoCase(_T("microseconds")) == 0)
			factor = 1000000.0;
		else
			return false;
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAddMathChannelEvent
//
// Description:		Adds a user-defined math channel to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAddMathChannelEvent(wxCommandEvent& WXUNUSED(event))
{
	DisplayMathChannelDialog();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the derivative of the selected grid
//					row to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextPlotDerivativeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the derivative of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(DiscreteDerivative::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("d/dt(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the integral of the selected grid
//					row to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextPlotIntegralEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the integral of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(DiscreteIntegral::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("integral(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the RMS of the selected grid
//					row to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextPlotRMSEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(RootMeanSquare::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("RMS(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotDerivativeEvent
//
// Description:		Adds a curve showing the FFT of the selected grid
//					row to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextPlotFFTEvent(wxCommandEvent& WXUNUSED(event))
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"), _T("Accuracy Warning"));

	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(FastFourierTransform::Compute(*plotList[row - 1]));

	// Scale as required
	newData->MultiplyXData(factor);

	wxString name = _T("FFT(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextFilterLowPassEvent
//
// Description:		Adds a curve showing the filtered signal to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextFilterLowPassEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	const Dataset2D *currentData = plotList[row - 1];
	Dataset2D *newData = new Dataset2D(*currentData);

	// Display a dialog asking for the cutoff frequency
	wxString cutoffString = ::wxGetTextFromUser(_T("Specify the cutoff frequency in Hertz:"),
		_T("Filter Cutoff Frequency"), _T("1.0"), this);
	double cutoff;
	if (!cutoffString.ToDouble(&cutoff))
	{
		::wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Filter Error"));
		return;
	}

	// Create the filter
	double sampleRate = 1.0 / (currentData->GetXData(1) - currentData->GetXData(0));// [Hz]

	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"), _T("Accuracy Warning"));

	sampleRate *= factor;
	LowPassFirstOrderFilter filter(cutoff, sampleRate, currentData->GetYData(0));

	// Apply the filter
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = filter.Apply(currentData->GetYData(i));

	wxString name = cutoffString.Trim() + _T(" Hz low-pass(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextFilterHighPassEvent
//
// Description:		Adds a curve showing the filtered signal to the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextFilterHighPassEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the filtered dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	const Dataset2D *currentData = plotList[row - 1];
	Dataset2D *newData = new Dataset2D(*currentData);

	// Display a dialog asking for the cutoff frequency
	wxString cutoffString = ::wxGetTextFromUser(_T("Specify the cutoff frequency in Hertz:"),
		_T("Filter Cutoff Frequency"), _T("1.0"), this);
	double cutoff;
	if (!cutoffString.ToDouble(&cutoff))
	{
		::wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Filter Error"));
		return;
	}

	// Create the filter
	double sampleRate = 1.0 / (currentData->GetXData(1) - currentData->GetXData(0));// [Hz]

	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"), _T("Accuracy Warning"));

	sampleRate *= factor;
	HighPassFirstOrderFilter filter(cutoff, sampleRate, currentData->GetYData(0));

	// Apply the filter
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = filter.Apply(currentData->GetYData(i));

	wxString name = cutoffString.Trim() + _T(" Hz high-pass(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextFitCurve
//
// Description:		Fits a curve to the dataset selected in the grid control.
//					User is asked to specify the order of the fit.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextFitCurve(wxCommandEvent& WXUNUSED(event))
{
	// Ask the user what order to use for the polynomial
	unsigned long order;
	wxString orderString = ::wxGetTextFromUser(_T("Specify the order of the polynomial fit:"),
		_T("Polynomial Curve Fit"), _T("2"), this);

	if (!orderString.ToULong(&order))
	{
		::wxMessageBox(_T("ERROR:  Order must be a positive integer!"), _T("Error Fitting Curve"));
		return;
	}

	// Fit the data
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*plotList[row - 1], order);

	// Create a data set to draw the fit and add it to the plot
	Dataset2D *newData = new Dataset2D(*plotList[row - 1]);
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = CurveFit::EvaluateFit(newData->GetXData(i), fitData);

	// Create discriptive string to use as the name
	wxString name, termString;
	//name.Printf("Order %lu Fit([%i]), R^2 = %0.2f", order, row, fitData.rSquared);
	name.Printf("Fit [%i] (R^2 = %0.2f): ", row, fitData.rSquared);
	for (i = 0; i <= order; i++)
	{
		if (i == 0)
			termString.Printf("%1.2e", fitData.coefficients[i]);
		else if (i == 1)
			termString.Printf("%0.2ex", fabs(fitData.coefficients[i]));
		else
			termString.Printf("%0.2ex^%i", fabs(fitData.coefficients[i]), i);

		if (i < order)
		{
			if (fitData.coefficients[i] > 0.0)
				termString.Append(_T(" + "));
			else
				termString.Append(_T(" - "));
		}
		name.Append(termString);
	}
	AddCurve(newData, name);

	// Free the coefficient data
	delete [] fitData.coefficients;

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlines
//
// Description:		Toggles gridlines for the entire plot on and off.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlines(wxCommandEvent& WXUNUSED(event))
{
	if (plotArea->GetGridOn())
		plotArea->SetGridOff();
	else
		plotArea->SetGridOn();

	plotArea->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScale
//
// Description:		Autoscales the plot.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScale(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScale();
	plotArea->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCursorValues
//
// Description:		Updates the values for the cursors and their differences
//					in the options grid.
//
// Input Argurments:
//		leftVisible		= const bool& indicating whether or not the left
//						  cursor is visible
//		rightVisible	= const bool& indicating whether or not the right
//						  cursor is visible
//		leftValue		= const double& giving the value of the left cursor
//		rightValue		= const double& giving the value of the right cursor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue)
{
	if (optionsGrid == NULL)
		return;

	// FIXME:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	double value;
	wxString valueString;
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (leftVisible)
		{
			valueString.Printf("%f", leftValue);
			optionsGrid->SetCellValue(0, colLeftCursor, valueString);

			value = leftValue;
			if (plotList[i - 1]->GetYAt(value))
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colLeftCursor, _T("*") + valueString);
			}
			else
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colLeftCursor, valueString);
			}
		}
		else
		{
			optionsGrid->SetCellValue(0, colLeftCursor, wxEmptyString);
			optionsGrid->SetCellValue(i, colLeftCursor, wxEmptyString);

			// The difference column only exists if both cursors are visible
			optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
			optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}

		if (rightVisible)
		{
			valueString.Printf("%f", rightValue);
			optionsGrid->SetCellValue(0, colRightCursor, valueString);

			value = rightValue;
			if (plotList[i - 1]->GetYAt(value))
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colRightCursor, _T("*") + valueString);
			}
			else
			{
				valueString.Printf("%f", value);
				optionsGrid->SetCellValue(i, colRightCursor, valueString);
			}

			// Update the difference cells if the left cursor is visible, too
			if (leftVisible)
			{
				double left = leftValue;
				plotList[i - 1]->GetYAt(left);
				valueString.Printf("%f", value - left);
				optionsGrid->SetCellValue(i, colDifference, valueString);

				valueString.Printf("%f", rightValue - leftValue);
				optionsGrid->SetCellValue(0, colDifference, valueString);
			}
		}
		else
		{
			optionsGrid->SetCellValue(0, colRightCursor, wxEmptyString);
			optionsGrid->SetCellValue(i, colRightCursor, wxEmptyString);

			// The difference column only exists if both cursors are visible
			optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
			optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}
	}

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		DisplayMathChannelDialog
//
// Description:		Displays an input dialog that allows the user to enter a
//					math expression.  If an expression is entered, it attempts
//					to add the channel.
//
// Input Argurments:
//		defaultInput	= wxString (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::DisplayMathChannelDialog(wxString defaultInput)
{
	// Display input dialog in which user can specify the math desired
	wxString message(_T("Enter the math you would like to perform:\n\n"));
	message.Append(_T("    Use [x] notation to specify channels, where x = 0 is Time, x = 1 is the first data channel, etc.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, ddt, and int\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, this));

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		DisplayAxisRangeDialog
//
// Description:		Displays an input dialog that allows the user to set the
//					range for an axis.
//
// Input Argurments:
//		axis	= const PlotContext& specifying the axis which is to be resized
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::DisplayAxisRangeDialog(const PlotContext &axis)
{
	// Assign min and max to current axis limits
	double min, max;
	switch (axis)
	{
	case plotContextXAxis:
		min = plotArea->GetXMin();
		max = plotArea->GetXMax();
		break;

	case plotContextLeftYAxis:
		min = plotArea->GetLeftYMin();
		max = plotArea->GetLeftYMax();
		break;

	case plotContextRightYAxis:
		min = plotArea->GetRightYMin();
		max = plotArea->GetRightYMax();
		break;

	default:
	case plotContextPlotArea:
		// Plot area is not a valid context in which we can set axis limits
		return;
	}

	// Display the dialog and make sure the user doesn't cancel
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
		::wxMessageBox(_T("ERROR:  Limits must unique!"), _T("Error Setting Limits"));
		return;
	}

	switch (axis)
	{
	case plotContextLeftYAxis:
		plotArea->SetLeftYLimits(min, max);
		break;

	case plotContextRightYAxis:
		plotArea->SetRightYLimits(min, max);
		break;

	default:
	case plotContextXAxis:
		plotArea->SetXLimits(min, max);
	}

	plotArea->UpdateDisplay();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesBottom
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetBottomGrid(!plotArea->GetBottomGrid());

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleBottom
//
// Description:		Auto-scales the bottom axis.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScaleBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleBottom();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetRangeBottom
//
// Description:		Dispalys a dialog box for setting the axis range.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextSetRangeBottom(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextXAxis);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesLeft
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlinesLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftGrid(!plotArea->GetLeftGrid());

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleLeft
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScaleLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleLeft();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetRangeLeft
//
// Description:		Dispalys a dialog box for setting the axis range.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextSetRangeLeft(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextLeftYAxis);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesRight
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextToggleGridlinesRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightGrid(!plotArea->GetRightGrid());

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleRight
//
// Description:		Toggles gridlines for the bottom axis.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextAutoScaleRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleRight();

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetRangeRight
//
// Description:		Dispalys a dialog box for setting the axis range.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextSetRangeRight(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextRightYAxis);

	return;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotBGColor
//
// Description:		Displays a dialog allowing the user to specify the plot's
//					background color.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextPlotBGColor(wxCommandEvent& WXUNUSED(event))
{
	wxColourData colorData;
	colorData.SetColour(plotArea->GetBackgroundColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
    dialog.SetTitle(_T("Choose Background Color"));
    if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		plotArea->SetBackgroundColor(color);
		plotArea->UpdateDisplay();
    }
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextGridColor
//
// Description:		Dispalys a dialog box allowing the user to specify the
//					gridline color.
//
// Input Argurments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ContextGridColor(wxCommandEvent& WXUNUSED(event))
{
	wxColourData colorData;
	colorData.SetColour(plotArea->GetGridColor().ToWxColor());

	wxColourDialog dialog(this, &colorData);
	dialog.CenterOnParent();
    dialog.SetTitle(_T("Choose Background Color"));
    if (dialog.ShowModal() == wxID_OK)
    {
		Color color;
		color.Set(dialog.GetColourData().GetColour());
		plotArea->SetGridColor(color);
		plotArea->UpdateDisplay();
    }
}

//==========================================================================
// Class:			MainFrame
// Function:		TestSignalOperations
//
// Description:		Performs tests on signal operation classes.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*void MainFrame::TestSignalOperations(void)
{
	// Create test data:
	Dataset2D set1(500);
	unsigned int i;
	double dt = 0.01;
	for (i = 0; i < set1.GetNumberOfPoints(); i++)
		set1.GetXPointer()[i] = i * dt;

	double f1 = 5.0 * PlotMath::PI * 2.0;
	double f2 = 2.0 * PlotMath::PI * 2.0;

	Dataset2D set2(set1);
	Dataset2D set3(set1);
	Dataset2D set4(set1);
	Dataset2D set5(set1);
	double t;
	for (i = 0; i < set1.GetNumberOfPoints(); i++)
	{
		t = set1.GetXData(i);
		set1.GetYPointer()[i] = 2.0 * sin(f1 * t);
		set2.GetYPointer()[i] = 3.5 * sin(f2 * t);
		set3.GetYPointer()[i] = set1.GetYData(i) + set2.GetYData(i);
		set4.GetYPointer()[i] = set1.GetYData(i) * set2.GetYData(i);
		set5.GetYPointer()[i] = 3.0 + 0.45 * t;
	}

	// Derivative	= OK
	// RMS			= OK
	// Integral		= OK
	// FFT			= OK

	// Save data to file
	set1.ExportDataToFile(_T("set1.txt"));
	set2.ExportDataToFile(_T("set2.txt"));
	set3.ExportDataToFile(_T("set3.txt"));
	set4.ExportDataToFile(_T("set4.txt"));
	set5.ExportDataToFile(_T("set5.txt"));

	// Test integrals and derivatives on sets 1 and 2 and 5
	Dataset2D intTest1 = DiscreteIntegral::ComputeTimeHistory(set1);
	Dataset2D intTest2 = DiscreteIntegral::ComputeTimeHistory(set2);
	Dataset2D derTest1 = DiscreteDerivative::ComputeTimeHistory(set1);
	Dataset2D derTest2 = DiscreteDerivative::ComputeTimeHistory(set2);
	Dataset2D intTest5 = DiscreteIntegral::ComputeTimeHistory(set5);

	// Save results to file
	intTest1.ExportDataToFile(_T("integral set 1.txt"));
	intTest2.ExportDataToFile(_T("integral set 2.txt"));
	derTest1.ExportDataToFile(_T("derivative set 1.txt"));
	derTest2.ExportDataToFile(_T("derivative set 2.txt"));
	intTest5.ExportDataToFile(_T("integral set 5.txt"));
	DiscreteDerivative::ComputeTimeHistory(intTest1).ExportDataToFile(_T("dofint1.txt"));

	// Test RMS and FFT on all four sets
	Dataset2D rms1 = RootMeanSquare::ComputeTimeHistory(set1);
	Dataset2D rms2 = RootMeanSquare::ComputeTimeHistory(set2);
	Dataset2D rms3 = RootMeanSquare::ComputeTimeHistory(set3);
	Dataset2D rms4 = RootMeanSquare::ComputeTimeHistory(set4);
	Dataset2D fft1 = FastFourierTransform::Compute(set1);
	Dataset2D fft2 = FastFourierTransform::Compute(set2);
	Dataset2D fft3 = FastFourierTransform::Compute(set3);
	Dataset2D fft4 = FastFourierTransform::Compute(set4);

	// Save results to file
	rms1.ExportDataToFile(_T("rms1.txt"));
	rms2.ExportDataToFile(_T("rms2.txt"));
	rms3.ExportDataToFile(_T("rms3.txt"));
	rms4.ExportDataToFile(_T("rms4.txt"));
	fft1.ExportDataToFile(_T("fft1.txt"));
	fft2.ExportDataToFile(_T("fft2.txt"));
	fft3.ExportDataToFile(_T("fft3.txt"));
	fft4.ExportDataToFile(_T("fft4.txt"));
}*/