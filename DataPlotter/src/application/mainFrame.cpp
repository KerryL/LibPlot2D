/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainFrame.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MainFrame class.  Uses wxWidgets for the GUI components.
// History:

// Standard C++ headers
#include <algorithm>

// wxWidgets headers
#include <wx/grid.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>

// Local headers
#include "application/mainFrame.h"
#include "application/plotterApp.h"
#include "application/dropTarget.h"
#include "application/rangeLimitsDialog.h"
#include "application/filterDialog.h"
#include "application/customFileFormat.h"
#include "application/multiChoiceDialog.h"
#include "application/frfDialog.h"
#include "application/fftDialog.h"
#include "renderer/plotRenderer.h"
#include "renderer/color.h"
#include "utilities/dataset2D.h"
#include "utilities/math/plotMath.h"
#include "utilities/signals/integral.h"
#include "utilities/signals/derivative.h"
#include "utilities/signals/rms.h"
#include "utilities/signals/fft.h"
#include "utilities/math/expressionTree.h"
#include "utilities/signals/filters/lowPassOrder1.h"
#include "utilities/signals/filters/lowPassOrder2.h"
#include "utilities/signals/filters/highPassOrder1.h"

// *nix Icons
#ifdef __WXGTK__
#include "../../res/icons/plots16.xpm"
#include "../../res/icons/plots24.xpm"
#include "../../res/icons/plots32.xpm"
#include "../../res/icons/plots48.xpm"
#include "../../res/icons/plots64.xpm"
#include "../../res/icons/plots128.xpm"
#endif

//==========================================================================
// Class:			MainFrame
// Function:		MainFrame
//
// Description:		Constructor for MainFrame class.  Initializes the form
//					and creates the controls, etc.
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
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString, wxDefaultPosition,
								 wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
	CreateControls();
	SetProperties();

	currentFileFormat = FormatGeneric;
}

//==========================================================================
// Class:			MainFrame
// Function:		~MainFrame
//
// Description:		Denstructor for MainFrame class.  Frees memory and
//					releases GUI object managers.
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
MainFrame::~MainFrame()
{
}

//==========================================================================
// Class:			MainFrame
// Function:		Constant Declarations
//
// Description:		Constant declarations for the MainFrame class.
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
/*#ifdef __WXGTK__
const wxString MainFrame::pathToConfigFile = _T("dataplotter.rc");
#else
const wxString MainFrame::pathToConfigFile = _T("dataplotter.ini");
#endif*/

//==========================================================================
// Class:			MainFrame
// Function:		CreateControls
//
// Description:		Creates sizers and controls and lays them out in the window.
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
void MainFrame::CreateControls(void)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	topSizer->Add(splitter, 1, wxGROW);

	wxPanel *lowerPanel = new wxPanel(splitter);
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	lowerSizer->Add(CreateButtons(lowerPanel), 0, wxGROW | wxALL, 5);
	lowerSizer->Add(CreateOptionsGrid(lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	CreatePlotArea(splitter);
	splitter->SplitHorizontally(plotArea, lowerPanel, plotArea->GetSize().GetHeight());
	splitter->SetSashGravity(1.0);
	splitter->SetMinimumPaneSize(150);

	SetSizerAndFit(topSizer);
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotArea
//
// Description:		Creates the main plot control.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotRenderer* pointing to plotArea
//
//==========================================================================
PlotRenderer* MainFrame::CreatePlotArea(wxWindow *parent)
{
#ifdef __WXGTK__
	// Under GTK, we get a segmentation fault or X error on call to SwapBuffers in RenderWindow.
	// Adding the double-buffer arugment fixes this.  Under windows, the double-buffer argument
	// causes the colors to go funky.  So we have this #if.
	int args[] = {WX_GL_DOUBLEBUFFER, 0};
	plotArea = new PlotRenderer(parent, wxID_ANY, args, *this);
#else
	plotArea = new PlotRenderer(parent, wxID_ANY, NULL, *this);
#endif

	plotArea->SetSize(480, 320);
	plotArea->SetGridOn();

	return plotArea;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateOptionsGrid
//
// Description:		Creates and formats the options grid.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxGrid* pointing to optionsGrid
//
//==========================================================================
wxGrid* MainFrame::CreateOptionsGrid(wxWindow *parent)
{
	optionsGrid = new wxGrid(parent, wxID_ANY);

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

	return optionsGrid;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateButtons
//
// Description:		Creates the buttons and returns the sizer pointer.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxBoxSizer*
//
//==========================================================================
wxBoxSizer* MainFrame::CreateButtons(wxWindow *parent)
{
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);

	openButton = new wxButton(parent, idButtonOpen, _T("Open"));
	autoScaleButton = new wxButton(parent, idButtonAutoScale, _T("Auto Scale"));
	removeCurveButton = new wxButton(parent, idButtonRemoveCurve, _T("Remove"));
	buttonSizer->Add(openButton, 0, wxEXPAND);
	buttonSizer->Add(autoScaleButton, 0, wxEXPAND);
	buttonSizer->Add(removeCurveButton, 0, wxEXPAND);

	wxStaticText *versionText = new wxStaticText(parent, wxID_ANY, DataPlotterApp::versionString);
	buttonSizer->Add(versionText, 0, wxEXPAND | wxALIGN_BOTTOM);

	return buttonSizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default fonts.
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
void MainFrame::SetProperties(void)
{
	SetTitle(DataPlotterApp::dataPlotterTitle);
	SetName(DataPlotterApp::dataPlotterName);
	Center();

#ifdef __WXMSW__
	SetIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE));
#elif __WXGTK__
	SetIcon(wxIcon(plots16_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(plots24_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(plots32_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(plots48_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(plots64_xpm, wxBITMAP_TYPE_XPM));
	SetIcon(wxIcon(plots128_xpm, wxBITMAP_TYPE_XPM));
#endif

	SetDropTarget(dynamic_cast<wxDropTarget*>(new DropTarget(*this)));
}

//==========================================================================
// Class:			MainFrame
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
	EVT_MENU(idContextFRF,							MainFrame::ContextFRFEvent)
	EVT_MENU(idContextSetTimeUnits,					MainFrame::ContextSetTimeUnitsEvent)
	EVT_MENU(idContextPlotDerivative,				MainFrame::ContextPlotDerivativeEvent)
	EVT_MENU(idContextPlotIntegral,					MainFrame::ContextPlotIntegralEvent)
	EVT_MENU(idContextPlotRMS,						MainFrame::ContextPlotRMSEvent)
	EVT_MENU(idContextPlotFFT,						MainFrame::ContextPlotFFTEvent)
	EVT_MENU(idButtonRemoveCurve,					MainFrame::ButtonRemoveCurveClickedEvent)
	EVT_MENU(idContextBitMask,						MainFrame::ContextBitMaskEvent)
	EVT_MENU(idContextTimeShift,					MainFrame::ContextTimeShiftEvent)

	EVT_MENU(idContextFilter,						MainFrame::ContextFilterEvent)
	EVT_MENU(idContextFitCurve,						MainFrame::ContextFitCurve)

	EVT_MENU(idPlotContextToggleGridlines,			MainFrame::ContextToggleGridlines)
	EVT_MENU(idPlotContextAutoScale,				MainFrame::ContextAutoScale)
	EVT_MENU(idPlotContextWriteImageFile,			MainFrame::ContextWriteImageFile)

	EVT_MENU(idPlotContextBGColor,					MainFrame::ContextPlotBGColor)
	EVT_MENU(idPlotContextGridColor,				MainFrame::ContextGridColor)

	EVT_MENU(idPlotContextToggleBottomGridlines,	MainFrame::ContextToggleGridlinesBottom)
	EVT_MENU(idPlotContextSetBottomRange,			MainFrame::ContextSetRangeBottom)
	EVT_MENU(idPlotContextSetBottomLogarithmic,		MainFrame::ContextSetLogarithmicBottom)
	EVT_MENU(idPlotContextAutoScaleBottom,			MainFrame::ContextAutoScaleBottom)

	//EVT_MENU(idPlotContextToggleTopGridlines,		MainFrame::)
	//EVT_MENU(idPlotContextSetTopRange,			MainFrame::)
	//EVT_MENU(idPlotContextSetTopLogarithmic,		MainFrame::)
	//EVT_MENU(idPlotContextAutoScaleTop,			MainFrame::)

	EVT_MENU(idPlotContextToggleLeftGridlines,		MainFrame::ContextToggleGridlinesLeft)
	EVT_MENU(idPlotContextSetLeftRange,				MainFrame::ContextSetRangeLeft)
	EVT_MENU(idPlotContextSetLeftLogarithmic,		MainFrame::ContextSetLogarithmicLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			MainFrame::ContextAutoScaleLeft)

	EVT_MENU(idPlotContextToggleRightGridlines,		MainFrame::ContextToggleGridlinesRight)
	EVT_MENU(idPlotContextSetRightRange,			MainFrame::ContextSetRangeRight)
	EVT_MENU(idPlotContextSetRightLogarithmic,		MainFrame::ContextSetLogarithmicRight)
	EVT_MENU(idPlotContextAutoScaleRight,			MainFrame::ContextAutoScaleRight)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		ButtonOpenClickedEvent
//
// Description:		Displays a dialog asking the user to specify the file to
//					read from.
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
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextWriteImageFile
//
// Description:		Calls the object of interest's write image file method.
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
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonAutoScaleClickedEvent
//
// Description:		Event fires when user clicks "AutoScale" button.
//
// Input Arguments:
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
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonRemoveCurveClickedEvent
//
// Description:		Event fires when user clicks "RemoveCurve" button.
//
// Input Arguments:
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
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::CreateGridContextMenu(const wxPoint &position, const unsigned int &row)
{
	// Declare the menu variable and get the position of the cursor
	wxMenu *contextMenu = new wxMenu();

	// Start building the context menu
	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));
	contextMenu->Append(idContextFRF, _T("Frequency Response"));

	if (row == 0 && currentFileFormat == FormatGeneric)
		contextMenu->Append(idContextSetTimeUnits, _T("Set Time Units"));
	else if (row > 0)
	{
		contextMenu->Append(idContextPlotDerivative, _T("Plot Derivative"));
		contextMenu->Append(idContextPlotIntegral, _T("Plot Integral"));
		contextMenu->Append(idContextPlotRMS, _T("Plot RMS"));
		contextMenu->Append(idContextPlotFFT, _T("Plot FFT"));
		contextMenu->Append(idContextTimeShift, _T("Plot Time-Shifted"));
		contextMenu->Append(idContextBitMask, _T("Plot Bit"));

		contextMenu->AppendSeparator();

		contextMenu->Append(idContextFilter, _T("Filter Curve"));
		contextMenu->Append(idContextFitCurve, _T("Fit Curve"));

		contextMenu->AppendSeparator();

		contextMenu->Append(idButtonRemoveCurve, _T("Remove Curve"));
	}

	// Show the menu
	PopupMenu(contextMenu, position);

	// Delete the context menu object
	delete contextMenu;
	contextMenu = NULL;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotContextMenu
//
// Description:		Displays a context menu for the plot.
//
// Input Arguments:
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
	wxMenu *contextMenu;

	switch (context)
	{
	case plotContextXAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleBottomGridlines);
		contextMenu->Check(idPlotContextSetBottomLogarithmic, plotArea->GetXLogarithmic());
		break;

	case plotContextLeftYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleLeftGridlines);
		contextMenu->Check(idPlotContextSetLeftLogarithmic, plotArea->GetLeftLogarithmic());
		break;

	case plotContextRightYAxis:
		contextMenu = CreateAxisContextMenu(idPlotContextToggleRightGridlines);
		contextMenu->Check(idPlotContextSetRightLogarithmic, plotArea->GetRightLogarithmic());
		break;

	default:
	case plotContextPlotArea:
		contextMenu = CreatePlotAreaContextMenu();
		break;
	}

	PopupMenu(contextMenu, position);

	delete contextMenu;
	contextMenu = NULL;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotAreaContextMenu
//
// Description:		Displays a context menu for the specified plot axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu*
//
//==========================================================================
wxMenu* MainFrame::CreatePlotAreaContextMenu(void) const
{
	wxMenu *contextMenu = new wxMenu();
	contextMenu->Append(idPlotContextToggleGridlines, _T("Toggle Gridlines"));
	contextMenu->Append(idPlotContextAutoScale, _T("Auto Scale"));
	contextMenu->Append(idPlotContextWriteImageFile, _T("Write Image File"));
	contextMenu->AppendSeparator();
	contextMenu->Append(idPlotContextBGColor, _T("Set Background Color"));
	contextMenu->Append(idPlotContextGridColor, _T("Set Gridline Color"));

	return contextMenu;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateAxisContextMenu
//
// Description:		Displays a context menu for the specified plot axis.
//
// Input Arguments:
//		baseEventId	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxMenu*
//
//==========================================================================
wxMenu* MainFrame::CreateAxisContextMenu(const unsigned int &baseEventId) const
{
	wxMenu* contextMenu = new wxMenu();

	contextMenu->Append(baseEventId, _T("Toggle Axis Gridlines"));
	contextMenu->Append(baseEventId + 1, _T("Auto Scale Axis"));
	contextMenu->Append(baseEventId + 2, _T("Set Range"));
	contextMenu->AppendCheckItem(baseEventId + 3, _T("Logarithmic Scale"));

	return contextMenu;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Arguments:
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
// Input Arguments:
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

	// NOTE:  If we ever choose to allow multiple files to be opened, this will need to go
	ClearAllCurves();

	startOfExtension = pathAndFileName.Last('.') + 1;
	fileExtension = pathAndFileName.Mid(startOfExtension);

	bool loadedOK(false);
	CustomFileFormat customFormat(pathAndFileName);
	if (customFormat.IsCustomFormat())
		loadedOK = LoadCustomFile(pathAndFileName, customFormat);
	else if (fileExtension.CmpNoCase("csv") == 0)
		loadedOK = LoadCsvFile(pathAndFileName);
	else if (fileExtension.CmpNoCase("txt") == 0)
		loadedOK = LoadTxtFile(pathAndFileName);
	else
		loadedOK = LoadGenericDelimitedFile(pathAndFileName);

	if (loadedOK)
	{
		SetTitleFromFileName(pathAndFileName);
		SetXDataLabel(currentFileFormat);
		plotArea->SaveCurrentZoom();
	}

	return loadedOK;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadCustomFile
//
// Description:		Method for loading a custom (defined in XML file) format.
//
// Input Arguments:
//		pathAndFileName	= wxString
//		customFormat	= CustomFileFormat
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadCustomFile(wxString pathAndFileName, CustomFileFormat &customFormat)
{
	return LoadGenericDelimitedFile(pathAndFileName, &customFormat);
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadTxtFile
//
// Description:		Method for loading a single object from a text file.
//
// Input Arguments:
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
	// Add any specific file formats with .txt extensions here

	// Try to load the file as a generic delimited file
	return LoadGenericDelimitedFile(pathAndFileName);
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadCsvFile
//
// Description:		Loads specific .csv file formats.
//
// Input Arguments:
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
	if (IsBaumullerFile(pathAndFileName))
		return LoadBaumullerFile(pathAndFileName);
	else if (IsKollmorgenFile(pathAndFileName))
		return LoadKollmorgenFile(pathAndFileName);
	// Add any other specific file formats with .csv extensions here

	return LoadGenericDelimitedFile(pathAndFileName);
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadBaumullerFile
//
// Description:		Loads Baumuller data trace (from BM4xxx series drive).
//
// Input Arguments:
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
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR, this);
		return false;
	}

	wxString delimiter(';');
	wxArrayString descriptions = GetBaumullerDescriptions(file, delimiter);

	std::vector<double> *data = new std::vector<double>[GetPopulatedCount(descriptions)];
	if (!ExtractData(file, delimiter, data, descriptions))
	{
		wxMessageBox(_T("ERROR:  Non-numeric entry encountered while parsing file!"),
			_T("Error Generating Plot"), wxICON_ERROR, this);
		delete [] data;
		return false;
	}
	file.close();

	AddData(data, descriptions);
	currentFileFormat = FormatBaumuller;

	delete [] data;

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetBaumullerDescriptions
//
// Description:		Parses the file and extracts curve descriptions.  This
//					assumes that the file has been opened, but not read from.
//					Also, prior to returning, all data prior to numeric data
//					is discarded.
//
// Input Arguments:
//		file		= std::ifstream&
//		delimiter	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing curve descriptions
//
//==========================================================================
wxArrayString MainFrame::GetBaumullerDescriptions(std::ifstream &file, const wxString &delimiter) const
{
	std::string nextLine;
	std::getline(file, nextLine);

	// wxString makes this robust against varying line endings
	while (wxString(nextLine).Trim().Mid(0, 11).Cmp(_T("Par.number:")) != 0)
		std::getline(file, nextLine);
	wxArrayString parameterNumbers = ParseLineIntoColumns(nextLine, delimiter);

	std::getline(file, nextLine);
	wxArrayString descriptions = ParseLineIntoColumns(nextLine, delimiter);

	std::getline(file, nextLine);
	wxArrayString units = ParseLineIntoColumns(nextLine, delimiter, false);

	SkipLines(file, 2);// Throw out max and min rows

	unsigned int i;
	for (i = 1; i < descriptions.GetCount(); i++)
		descriptions[i].Append(_T(" (") + parameterNumbers[i] + _T(") [") + units[i] + _T("]"));

	return descriptions;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadKollmorgenFile
//
// Description:		Loads Kollmorgen data trace (from S600 series drive).
//
// Input Arguments:
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
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR, this);
		return false;
	}

	wxString delimiter(',');
	double samplingPeriod;
	wxArrayString descriptions = GetKollmorgenDescriptions(file, delimiter, samplingPeriod);

	std::vector<double> *data = new std::vector<double>[GetPopulatedCount(descriptions)];
	if (!ExtractData(file, delimiter, data, descriptions))
	{
		wxMessageBox(_T("ERROR:  Non-numeric entry encountered while parsing file!"),
			_T("Error Generating Plot"), wxICON_ERROR, this);
		delete [] data;
		return false;
	}
	file.close();

	AddData(data, descriptions, &samplingPeriod);
	currentFileFormat = FormatKollmorgen;

	delete [] data;

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetKollmorgenDescriptions
//
// Description:		Parses the file and extracts curve descriptions.  This
//					assumes that the file has been opened, but not read from.
//					Also, prior to returning, all data prior to numeric data
//					is discarded.
//
// Input Arguments:
//		file			= std::ifstream&
//		delimiter		= const wxString&
//
// Output Arguments:
//		samplingPeriod	= double&
//
// Return Value:
//		wxArrayString containing curve descriptions
//
//==========================================================================
wxArrayString MainFrame::GetKollmorgenDescriptions(std::ifstream &file, const wxString &delimiter, double &samplingPeriod) const
{
	SkipLines(file, 2);

	std::string nextLine;
	std::getline(file, nextLine);

	// The third line contains the number of data points and the sampling period in msec
	// We use this information to generate the time series (file does not contain a time series)
	samplingPeriod = atof(nextLine.substr(nextLine.find_first_of(delimiter) + 1).c_str()) / 1000.0;// [sec]

	// The fourth line contains the data set labels (which also gives us the number of datasets we need)
	std::getline(file, nextLine);
	wxArrayString descriptions = ParseLineIntoColumns(nextLine, delimiter);

	return descriptions;
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
// Input Arguments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadGenericDelimitedFile(wxString pathAndFileName, CustomFileFormat *customFormat)
{
	wxString delimiter;
	unsigned int headerLines;
	wxArrayString descriptions = GetGenericDescriptions(pathAndFileName,
		GetDelimiterList(customFormat), delimiter, headerLines);
	if (descriptions.size() < 2)
	{
		wxMessageBox(_T("No plottable data found in file!"), _T("Error Generating Plot"), wxICON_ERROR, this);
		return false;
	}
	genericXAxisLabel = descriptions[0];

	std::vector<double> scales(descriptions.size(), 1.0);
	if (customFormat)
		customFormat->ProcessChannels(descriptions, scales);

	if (!ProcessGenericFile(pathAndFileName, descriptions, headerLines, delimiter, scales))
		return false;
	currentFileFormat = FormatGeneric;

	if (customFormat)
	{
		if (!customFormat->GetTimeUnits().IsEmpty())
			genericXAxisLabel = _T("Time [") + customFormat->GetTimeUnits() + _T("]");
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		CompensateGenericChoices
//
// Description:		Compensates for the method used (removing the x-data column
//					as a choice) for the user to identify data to plot.
//
// Input Arguments:
//		choices	= wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CompensateGenericChoices(wxArrayInt &choices) const
{
	unsigned int i;
	for (i = 0; i < choices.Count(); i++)
		choices[i]++;
	choices.Add(0);// Keep the x-axis data
}

//==========================================================================
// Class:			MainFrame
// Function:		ProcessGenericFile
//
// Description:		Performs necessary actions to extract desired data from
//					the specified file.
//
// Input Arguments:
//		fileName	= const wxString&
//		descriptions	= wxArrayString&
//		headerLines		= const unsigned int&
//		delimiter		= const wxString&
//		scales			= const std::vector<double>&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool MainFrame::ProcessGenericFile(const wxString &fileName, wxArrayString &descriptions,
	const unsigned int &headerLines, const wxString &delimiter, const std::vector<double> &scales)
{
	MultiChoiceDialog dialog(this, _T("Select data to plot:"), _T("Select Data"),
		wxArrayString(descriptions.begin() + 1, descriptions.end()));
	if (dialog.ShowModal() == wxID_CANCEL)
		return false;

	wxArrayInt choices = dialog.GetSelections();
	if (choices.size() == 0)
	{
		wxMessageBox(_T("No data selected for plotting!"), _T("Error Generating Plot"), wxICON_ERROR, this);
		return false;
	}

	std::ifstream file(fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR, this);
		return false;
	}
	SkipLines(file, headerLines);

	std::vector<double> *data = new std::vector<double>[choices.size() + 1];// +1 for time column, which isn't displayed for user to select
	CompensateGenericChoices(choices);
	RemoveUnwantedDescriptions(descriptions, choices);
	if (!ExtractData(file, delimiter, data, descriptions))
	{
		wxMessageBox(_T("Error during data extraction."), _T("Error Reading File"), wxICON_ERROR, this);
		return false;
	}
	file.close();

	AddData(data, descriptions, NULL, &scales);
	delete [] data;

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		SkipLines
//
// Description:		Reads and discards the specified number of lines from
//					the file.
//
// Input Arguments:
//		file	= std::ifstream&
//		count	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SkipLines(std::ifstream &file, const unsigned int &count) const
{
	std::string nextLine;
	unsigned int i;
	for (i = 0; i < count; i++)
		std::getline(file, nextLine);
}

//==========================================================================
// Class:			MainFrame
// Function:		GetDelimiterList
//
// Description:		Returns a list of delimiters to attempt to use when parsing
//					a file.
//
// Input Arguments:
//		customFormat	= const CustomFileFormat*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing all acceptable delimiters
//
//==========================================================================
wxArrayString MainFrame::GetDelimiterList(const CustomFileFormat *customFormat) const
{
	// Don't use periods ('.') because we're going to have those in regular numbers (switch this for other languages?)
	wxArrayString delimiterList;
	delimiterList.Add(_T(" "));
	delimiterList.Add(_T(","));
	delimiterList.Add(_T("\t"));
	delimiterList.Add(_T(";"));

	if (customFormat)
	{
		if (!customFormat->GetDelimiter().IsEmpty())
		{
			delimiterList.Clear();
			delimiterList.Add(customFormat->GetDelimiter());
		}
	}

	return delimiterList;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetGenericDescriptions
//
// Description:		Parses the file and extracts curve descriptions.  This
//					assumes that the file has been opened, but not read from.
//					Also, prior to returning, all data prior to numeric data
//					is discarded.
//
// Input Arguments:
//		fileName		= const wxString&
//		delimiterList	= const wxArrayString&
//
// Output Arguments:
//		delimiter		= wxString& indicating the delimiter to use for this file
//		headerLines		= unsigned int& indicating the number of lines read
//
// Return Value:
//		wxArrayString containing data descriptions
//
//==========================================================================
wxArrayString MainFrame::GetGenericDescriptions(const wxString &fileName, const wxArrayString &delimiterList,
	wxString &delimiter, unsigned int &headerLines)
{
	wxArrayString descriptions;
	std::ifstream file(fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR, this);
		return descriptions;
	}

	std::string nextLine;
	wxArrayString delimitedLine, previousLines;
	unsigned int i;
	while (std::getline(file, nextLine))
	{
		for (i = 0; i < delimiterList.size(); i++)// Try all delimiters until we find one that works
		{
			delimiter = delimiterList[i];
			delimitedLine = ParseLineIntoColumns(nextLine, delimiter);
			if (delimitedLine.size() > 1)
			{
				if (!ListIsNumeric(delimitedLine))// If not all columns are numeric, this isn't a data row
					break;

				GenerateGenericNames(previousLines, delimitedLine, delimiter, descriptions);
				headerLines = previousLines.size();
				if (descriptions.size() == 0)
					descriptions = GenerateDummyNames(delimitedLine.size());
				return descriptions;
			}
		}
		delimiter.Empty();
		previousLines.Add(nextLine);
	}

	return descriptions;
}

//==========================================================================
// Class:			MainFrame
// Function:		ListIsNumeric
//
// Description:		Checks to see if the input array contains only numeric values.
//
// Input Arguments:
//		list	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if all values are numeric, false otherwise
//
//==========================================================================
bool MainFrame::ListIsNumeric(const wxArrayString &list) const
{
	unsigned int j;
	double value;
	for (j = 0; j < list.size(); j++)
	{
		if (!list[j].ToDouble(&value))
			return false;
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		GenerateDummyNames
//
// Description:		Generates plot names for cases where no information was
//					provided by the data file.
//
// Input Arguments:
//		count	= const unsigned int& specifying the number of names to generate
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing dummy names
//
//==========================================================================
wxArrayString MainFrame::GenerateDummyNames(const unsigned int &count) const
{
	unsigned int i;
	wxString dummyPlotName;
	wxArrayString names;
	for (i = 0; i < count; i++)
	{
		dummyPlotName.Printf("[%i]", i);
		names.Add(dummyPlotName);
	}

	return names;
}

//==========================================================================
// Class:			MainFrame
// Function:		GenerateGenericNames
//
// Description:		Creates the first part of the plot name for generic files.
//
// Input Arguments:
//		previousLines	= const wxArrayString&
//		currentLin		= const wxArrayString&
//		delimiter		= const wxString&
//
// Output Arguments:
//		descriptions	= wxArrayString&
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::GenerateGenericNames(const wxArrayString &previousLines, const wxArrayString &currentLine,
	const wxString &delimiter, wxArrayString &descriptions) const
{
	unsigned int i;
	int line;
	wxArrayString delimitedPreviousLine;
	double value;
	for (line = previousLines.size() - 1; line >= 0; line--)
	{
		delimitedPreviousLine = ParseLineIntoColumns(previousLines[line].c_str(), delimiter);
		if (delimitedPreviousLine.size() != currentLine.size())
			break;

		bool prependText(true);
		for (i = 0; i < delimitedPreviousLine.size(); i++)
		{
			prependText = !delimitedPreviousLine[i].ToDouble(&value);
			if (!prependText)
				break;
		}

		if (prependText)
		{
			for (i = 0; i < delimitedPreviousLine.size(); i++)
			{
				if (descriptions.size() < i + 1)
					descriptions.Add(delimitedPreviousLine[i]);
				else
					descriptions[i].Prepend(delimitedPreviousLine[i] + _T(", "));
			}
		}
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveUnwantedDescriptions
//
// Description:		Makes descriptions for unselected items empty.
//
// Input Arguments:
//		choices			= const wxArrayInt&
//
// Output Arguments:
//		descriptions	= wxArrayString&
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::RemoveUnwantedDescriptions(wxArrayString &descriptions, const wxArrayInt &choices) const
{
	unsigned int i, j;
	bool remove;
	for (i = 0; i < descriptions.size(); i++)
	{
		remove = true;
		for (j = 0; j < choices.size(); j++)
		{
			if (i == (unsigned int)choices[j])
			{
				remove = false;
				break;
			}
		}

		if (remove)
			descriptions[i].Empty();
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		IsBaumullerFile
//
// Description:		Determines if the specified file is a Baumuller osilloscope trace.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for Baumuller files, false otherwise
//
//==========================================================================
bool MainFrame::IsBaumullerFile(const wxString &pathAndFileName)
{
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR, this);
		return false;
	}

	std::string nextLine;
	std::getline(file, nextLine);// Read first line
	file.close();

	// Wrap in wxString for robustness against varying line endings
	if (wxString(nextLine).Trim().Cmp(_T("WinBASS_II_Oscilloscope_Data")) == 0)
		return true;
	
	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		IsKollmorgenFile
//
// Description:		Determines if the specified file is a Baumuller osilloscope trace.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for Baumuller files, false otherwise
//
//==========================================================================
bool MainFrame::IsKollmorgenFile(const wxString &pathAndFileName)
{
	std::ifstream file(pathAndFileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + pathAndFileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR, this);
		return false;
	}

	SkipLines(file, 1);

	std::string nextLine;
	std::getline(file, nextLine);// Read second line
	file.close();

	// Kollmorgen format from S600 series drives
	// There may be a better way to check this, but I haven't found it
	// Wrap in wxString for robustness against varying line endings
	if (wxString(nextLine).Trim().Mid(0, 7).Cmp(_T("MMI vom")) == 0)
		return true;

	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		ExtractData
//
// Description:		Extracts numeric data from the file.  Columns with empty
//					descriptions are ignored.
//
// Input Arguments:
//		file			= std::ifstream&
//		delimiter		= const wxString&
//		data			= std::vector<double>*
//		descriptions	= const wxArrayString&
//
// Output Arguments:
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool MainFrame::ExtractData(std::ifstream &file, const wxString &delimiter,
	std::vector<double> *data, const wxArrayString &descriptions) const
{
	std::string nextLine;
	wxArrayString parsed;
	unsigned int i, set, curveCount(GetPopulatedCount(descriptions));
	double tempDouble;

	while (!file.eof())
	{
		std::getline(file, nextLine);
		parsed = ParseLineIntoColumns(nextLine, delimiter);

		if (parsed.size() < curveCount && parsed.size() > 0)
		{
			/*wxMessageBox(_T("Terminating data extraction prior to reaching end-of-file."),
				_T("Column Count Mismatch"), wxICON_WARNING, this);*/// No warning here due to const method
			return true;
		}

		set = 0;
		for (i = 0; i < parsed.size(); i++)
		{
			if (!parsed[i].ToDouble(&tempDouble))
				return false;

			if (!descriptions[i].IsEmpty())
			{
				data[set].push_back(tempDouble);
				set++;
			}
		}
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetPopulatedCount
//
// Description:		Determines the number of non-empty members of the array.
//
// Input Arguments:
//		list	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int MainFrame::GetPopulatedCount(const wxArrayString &list) const
{
	unsigned int count(0), i;
	for (i = 0; i < list.GetCount(); i++)
	{
		if (!list[i].IsEmpty())
			count ++;
	}

	return count;
}

//==========================================================================
// Class:			MainFrame
// Function:		AddData
//
// Description:		Creates datasets and adds the associated curves to the plot.
//
// Input Arguments:
//		data			= const std::vector<double>*
//		descriptions	= const wxArrayString&
//		timeStep		= const double* (optional)
//		scales			= const std::vector<double>* (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::AddData(const std::vector<double> *data, const wxArrayString &descriptions,
	const double *timeStep, const std::vector<double> *scales)
{
	assert(data);

	Dataset2D *dataSet;
	unsigned int i, j;

	for (i = 1; i < descriptions.size(); i++)
	{
		if (descriptions[i].IsEmpty())
			continue;

		dataSet = new Dataset2D(data[0].size());
		for (j = 0; j < data[0].size(); j++)
		{
			if (timeStep)
				dataSet->GetXPointer()[j] = *timeStep * (double)j;
			else
				dataSet->GetXPointer()[j] = data[0].at(j);
			dataSet->GetYPointer()[j] = data[plotList.GetCount() + 1].at(j);
		}

		if (scales)
			*dataSet *= (*scales)[i];

		AddCurve(dataSet, descriptions[i]);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		ParseLineIntoColumns
//
// Description:		Parses the specified line into pieces based on encountering
//					the specified delimiting character (or characters).
//
// Input Arguments:
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
wxArrayString MainFrame::ParseLineIntoColumns(wxString line,
	const wxString &delimiter, const bool &ignoreConsecutiveDelimiters) const
{
	// Remove \r character from end of line (required for GTK, etc.)
	line.Trim();

	wxArrayString parsed;

	size_t start(0);
	size_t end(0);

	while (end != std::string::npos && start < line.length())
	{
		// Find the next delimiting character
		end = line.find(delimiter.c_str(), start);

		// If the next delimiting character is right next to the previous character
		// (empty string between), ignore it (that is to say, we treat consecutive
		// delimiters as one)
		// Changed 4/29/2012 - For some Baumuller data, there are no units, which
		// results in consecutive delimiters that should NOT be treated as one
		if (end == start && ignoreConsecutiveDelimiters)
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
// Input Arguments:
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
	unsigned int start;
#ifdef __WXMSW__
	start = pathAndFileName.find_last_of(_T("\\")) + 1;
#else
	start = pathAndFileName.find_last_of(_T("/")) + 1;
#endif
	unsigned int end(pathAndFileName.find_last_of(_T(".")));
	SetTitle(pathAndFileName.Mid(start, end - start) + _T(" - ") + DataPlotterApp::dataPlotterTitle);
}

//==========================================================================
// Class:			MainFrame
// Function:		ClearAllCurves
//
// Description:		Removes all curves from the plot.
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
void MainFrame::ClearAllCurves(void)
{
	while (plotList.GetCount() > 0)
		RemoveCurve(0);
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels to the specified string.
//
// Input Arguments:
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
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels according to the opened file type.
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

	case FormatFrequency:
		SetXDataLabel(_T("Frequency [Hz]"));
		break;

	default:
	case FormatGeneric:
		SetXDataLabel(genericXAxisLabel);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds a new dataset to the plot, created by operating on
//					existing datasets.
//
// Input Arguments:
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
	GetXAxisScalingFactor(xAxisFactor);// No warning here:  it's only an issue for FFTs and filters; warning are generated then

	wxString errors = expression.Solve(mathString, *mathChannel, xAxisFactor);

	if (!errors.IsEmpty())
	{
		wxMessageBox(_T("Could not solve expression:\n\n") + errors, _T("Error Solving Expression"), wxICON_ERROR, this);

		DisplayMathChannelDialog(mathString);
		return;
	}

	AddCurve(mathChannel, mathString.Upper());// TODO:  Get better name from user
}

//==========================================================================
// Class:			MainFrame
// Function:		AddCurve
//
// Description:		Adds an existing dataset to the plot.
//
// Input Arguments:
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

	optionsGrid->BeginBatch();
	if (optionsGrid->GetNumberRows() == 0)
		AddTimeRowToGrid();
	unsigned int index = AddDataRowToGrid(name);
	optionsGrid->EndBatch();

	plotArea->AddCurve(*data);
	unsigned long size;
	optionsGrid->GetCellValue(index, colSize).ToULong(&size);
	plotArea->SetCurveProperties(index - 1, GetNextColor(index), true, false, size);
	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::AddTimeRowToGrid(void)
{
	optionsGrid->AppendRows();
		
	SetXDataLabel(currentFileFormat);

	unsigned int i;
	for (i = 0; i < colCount; i++)
		optionsGrid->SetReadOnly(0, i, true);
}

//==========================================================================
// Class:			MainFrame
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
unsigned int MainFrame::AddDataRowToGrid(const wxString &name)
{
	unsigned int index = optionsGrid->GetNumberRows();
	optionsGrid->AppendRows();

	unsigned int maxLineSize(5);

	optionsGrid->SetCellEditor(index, colVisible, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colRightAxis, new wxGridCellBoolEditor);
	optionsGrid->SetCellEditor(index, colSize, new wxGridCellNumberEditor(1, maxLineSize));

	unsigned int i;
	for (i = 0; i < colDifference; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colSize, false);
	optionsGrid->SetCellValue(index, colName, name);

	Color color = GetNextColor(index);

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());
	optionsGrid->SetCellValue(index, colSize, _T("1"));
	optionsGrid->SetCellValue(index, colVisible, _T("1"));
	optionsGrid->AutoSizeColumns();// FIXME:  This doesn't seem to fit to the X data label, if it is longer than the regular curve names

	return index;
}

//==========================================================================
// Class:			MainFrame
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
Color MainFrame::GetNextColor(const unsigned int &index) const
{
	unsigned int colorIndex = (index - 1) % 10;
	if (colorIndex == 0)
		return Color::ColorBlue;
	else if (colorIndex == 1)
		return Color::ColorRed;
	else if (colorIndex == 2)
		return Color::ColorGreen;
	else if (colorIndex == 3)
		return Color::ColorMagenta;
	else if (colorIndex == 4)
		return Color::ColorCyan;
	else if (colorIndex == 5)
		return Color::ColorOrange;
	else if (colorIndex == 6)
		return Color::ColorGray;
	else if (colorIndex == 7)
		return Color::ColorPurple;
	else if (colorIndex == 8)
		return Color::ColorLightBlue;
	else if (colorIndex == 9)
		return Color::ColorBlack;
	else
		assert(false);

	// The following colors we opt'ed not to use - either too hard to see or too similar to other colors
	// Color::ColorYellow
	// Color::ColorDrabGreen
	// Color::ColorPaleGreen
	// Color::ColorPink

	return Color::ColorBlack;
}

//==========================================================================
// Class:			MainFrame
// Function:		RemoveCurve
//
// Description:		Removes a curve from the plot.
//
// Input Arguments:
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
	optionsGrid->DeleteRows(i + 1);

	if (optionsGrid->GetNumberRows() == 1)
		optionsGrid->DeleteRows();

	optionsGrid->AutoSizeColumns();

	plotArea->RemoveCurve(i);
	plotList.Remove(i);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::GridRightClickEvent(wxGridEvent &event)
{
	optionsGrid->SelectRow(event.GetRow());
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition(), event.GetRow());
}

//==========================================================================
// Class:			MainFrame
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
}

//==========================================================================
// Class:			MainFrame
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

	ShowAppropriateXLabel();

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
// Function:		ShowAppropriateXLabel
//
// Description:		Updates the x-axis label as necessary.
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
void MainFrame::ShowAppropriateXLabel(void)
{
	// If the only visible curves are frequency plots, change the x-label
	int i;
	bool showFrequencyLabel(false);
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).Cmp(_T("1")) == 0)
		{
			if (optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FFT")) == 0 ||
				optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FRF")) == 0)
				showFrequencyLabel = true;
			else
			{
				showFrequencyLabel = false;
				break;
			}
		}
	}

	if (showFrequencyLabel)
		SetXDataLabel(FormatFrequency);
	else
		SetXDataLabel(currentFileFormat);
}

//==========================================================================
// Class:			MainFrame
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
// Input Arguments:
//		None
//
// Output Arguments:
//		factor	= double&, scaling factor
//		label	= wxString*, pointer to string, to be populated with the unit string
//
// Return Value:
//		bool; true for success, false otherwise
//
//==========================================================================
bool MainFrame::GetXAxisScalingFactor(double &factor, wxString *label)
{
	if (XScalingFactorIsKnown(factor, label))
		return true;

	wxString unit = ExtractUnitFromDescription(genericXAxisLabel);

	unit = unit.Trim().Trim(false);
	if (label)
		label->assign(unit);

	return UnitStringToFactor(unit, factor);
}

//==========================================================================
// Class:			MainFrame
// Function:		XScalingFactorIsKnown
//
// Description:		If the x-axis scaling factor is known, determines its value.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		factor	= double&
//		label	= wxString*
//
// Return Value:
//		bool, true if known, false otherwise
//
//==========================================================================
bool MainFrame::XScalingFactorIsKnown(double &factor, wxString *label) const
{
	if (currentFileFormat == FormatBaumuller)
	{
		factor = 1000.0;
		if (label)
			label->assign(_T("msec"));
		return true;
	}
	else if (currentFileFormat == FormatKollmorgen)
	{
		factor = 1.0;
		if (label)
			label->assign(_T("sec"));
		return true;
	}

	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		ExtractUnitFromDescription
//
// Description:		Parses the description looking for a unit string.  This
//					will recognize the following as unit strings:
//					X Series Name [unit]
//					X Series Name (unit)
//					X Series Name *delimiter* unit
//
// Input Arguments:
//		description	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the unit porition of the description
//
//==========================================================================
wxString MainFrame::ExtractUnitFromDescription(const wxString &description) const
{
	wxString unit;
	if (FindWrappedString(description, unit, '[', ']'))
		return unit;
	else if (FindWrappedString(description, unit, '(', ')'))
		return unit;

	// Check for last string following a delimiter
	wxArrayString delimiters;
	delimiters.Add(_T(","));
	delimiters.Add(_T(";"));
	delimiters.Add(_T("-"));
	delimiters.Add(_T(":"));

	int location;
	unsigned int i;
	for (i = 0; i < delimiters.size(); i++)
	{
		location = description.Find(delimiters[i].c_str());
		if (location != wxNOT_FOUND && location < (int)description.Len() - 1)
		{
			unit = description.Mid(location + 1);
			break;
		}
	}

	return unit;
}

//==========================================================================
// Class:			MainFrame
// Function:		FindWrappedString
//
// Description:		Determines if the specified string contains a string wrapped
//					with the specified characters.
//
// Input Arguments:
//		s		= const wxString&
//		open	= const wxChar& specifying the opening wrapping character
//		close	= const wxChar& specifying the closing warpping character
//
// Output Arguments:
//		contents	= wxString&
//
// Return Value:
//		bool, true if a wrapped string is found, false otherwise
//
//==========================================================================
bool MainFrame::FindWrappedString(const wxString &s, wxString &contents,
	const wxChar &open, const wxChar &close) const
{
	if (s.Last() == close)
	{
		int i;
		for (i = s.Len() - 2; i >= 0; i--)
		{
			if (s.at(i) == open)
			{
				contents = s.Mid(i + 1, s.Len() - i - 2);
				return true;
			}
		}
	}

	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		UnitStringToFactor
//
// Description:		Converts from a unit string to a factor value.
//
// Input Arguments:
//		unit	= const wxString&
//
// Output Arguments:
//		factor	= double&
//
// Return Value:
//		bool, true if unit can be converted, false otherwise
//
//==========================================================================
bool MainFrame::UnitStringToFactor(const wxString &unit, double &factor) const
{
	// We'll recognize the following units:
	// h, hr, hours -> factor = 1.0 / 3600.0
	// m, min, minutes -> factor = 1.0 / 60.0
	// s, sec, seconds -> factor = 1.0
	// ms, msec, milliseconds -> factor = 1000.0
	// us, usec, microseconds -> factor = 1000000.0

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
	{
		// Assume a factor of 1
		factor = 1.0;
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
void MainFrame::ContextAddMathChannelEvent(wxCommandEvent& WXUNUSED(event))
{
	DisplayMathChannelDialog();
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextFRFEvent(wxCommandEvent& WXUNUSED(event))
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	wxArrayString descriptions;
	int i;
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
		descriptions.Add(optionsGrid->GetCellValue(i, 0));

	FRFDialog dialog(this, descriptions);
	if (dialog.ShowModal() != wxID_OK)
		return;

	Dataset2D *amplitude = new Dataset2D, *phase = NULL, *coherence = NULL;

	if (dialog.GetComputePhase())
		phase = new Dataset2D;
	if (dialog.GetComputeCoherence())
		coherence = new Dataset2D;

	FastFourierTransform::ComputeFRF(*plotList[dialog.GetInputIndex()],
		*plotList[dialog.GetOutputIndex()], dialog.GetNumberOfAverages(),
		FastFourierTransform::WindowUniform, *amplitude, phase, coherence);

	AddCurve(&(amplitude->MultiplyXData(factor)), wxString::Format("FRF Amplitude, [%u] to [%u], [dB]",
		dialog.GetInputIndex(), dialog.GetOutputIndex()));
	if (dialog.GetComputePhase())
		AddCurve(&(phase->MultiplyXData(factor)), wxString::Format("FRF Phase, [%u] to [%u], [deg]",
			dialog.GetInputIndex(), dialog.GetOutputIndex()));
	if (dialog.GetComputeCoherence())
		AddCurve(&(coherence->MultiplyXData(factor)), wxString::Format("FRF Coherence, [%u] to [%u]",
			dialog.GetInputIndex(), dialog.GetOutputIndex()));
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextSetTimeUnitsEvent(wxCommandEvent& WXUNUSED(event))
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
	wxString currentLabel(optionsGrid->GetCellValue(0, colName));
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
// Class:			MainFrame
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
void MainFrame::ContextPlotDerivativeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the derivative of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(DiscreteDerivative::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("d/dt(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextPlotIntegralEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the integral of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(DiscreteIntegral::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("integral(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextPlotRMSEvent(wxCommandEvent& WXUNUSED(event))
{
	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(RootMeanSquare::ComputeTimeHistory(*plotList[row - 1]));

	wxString name = _T("RMS(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextPlotFFTEvent(wxCommandEvent& WXUNUSED(event))
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = GetFFTData(plotList[row - 1], factor);
	if (!newData)
		return;

	newData->MultiplyXData(factor);

	wxString name = _T("FFT(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextBitMaskEvent(wxCommandEvent& WXUNUSED(event))
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

	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(PlotMath::ApplyBitMask(*plotList[row - 1], bit));

	wxString name = optionsGrid->GetCellValue(row, colName) + _T(", Bit ") + wxString::Format("%lu", bit);
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
// Function:		GetFFTData
//
// Description:		Returns a dataset containing an FFT of the specified data.
//
// Input Arguments:
//		data				= const Dataset2D&
//		timeScalingFactor	= const double& factor required to get seconds
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D* pointing to a dataset contining the new FFT data
//
//==========================================================================
Dataset2D* MainFrame::GetFFTData(const Dataset2D* data, const double &timeScalingFactor)
{
	FFTDialog dialog(this, data->GetNumberOfPoints(),
		data->GetNumberOfZoomedPoints(plotArea->GetXMin(), plotArea->GetXMax()),
		(data->GetXData(1) - data->GetXData(0)) / timeScalingFactor);
	if (dialog.ShowModal() != wxID_OK)
		return NULL;

	if (dialog.GetUseZoomedData())
		return new Dataset2D(FastFourierTransform::ComputeFFT(GetXZoomedDataset(*data),
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap()));

	return new Dataset2D(FastFourierTransform::ComputeFFT(*data,
		dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap()));
}

//==========================================================================
// Class:			MainFrame
// Function:		GetXZoomedDataset
//
// Description:		Returns a dataset containing only the data within the
//					current zoomed x-limits.
//
// Input Arguments:
//		fullData	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D MainFrame::GetXZoomedDataset(const Dataset2D &fullData) const
{
	unsigned int i, startIndex(0), endIndex(0);
	while (fullData.GetXData(startIndex) < plotArea->GetXMin() &&
		startIndex < fullData.GetNumberOfPoints())
		startIndex++;
	endIndex = startIndex;
	while (fullData.GetXData(endIndex) < plotArea->GetXMax() &&
		endIndex < fullData.GetNumberOfPoints())
		endIndex++;

	Dataset2D data(endIndex - startIndex);
	for (i = startIndex; i < endIndex; i++)
	{
		data.GetXPointer()[i - startIndex] = fullData.GetXData(i);
		data.GetYPointer()[i - startIndex] = fullData.GetYData(i);
	}

	return data;
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextTimeShiftEvent(wxCommandEvent& WXUNUSED(event))
{	
	double shift(0.0);
	wxString shiftText = ::wxGetTextFromUser(
		_T("Specify the time to add to time data in original data:\n"
		"Use same units as time series.  Positive values shift curve to the right."),
		_T("Time Shift"), _T("0"), this);
	
	if (!shiftText.ToDouble(&shift) || shift == 0.0)
		return;
	
	// Create new dataset containing the RMS of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = new Dataset2D(*plotList[row - 1]);
	
	newData->XShift(shift);

	wxString name = optionsGrid->GetCellValue(row, colName) + _T(", t = t0 + ");
	name += shiftText;
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextFilterEvent(wxCommandEvent& WXUNUSED(event))
{
	// Display dialog
	FilterParameters filterParameters = DisplayFilterDialog();
	if (filterParameters.order == 0)
		return;

	// Create new dataset containing the FFT of dataset and add it to the plot
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	const Dataset2D *currentData = plotList[row - 1];
	Dataset2D *newData = new Dataset2D(*currentData);

	ApplyFilter(filterParameters, *newData);

	wxString name = FilterDialog::GetFilterNamePrefix(filterParameters) + _T("(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::ContextFitCurve(wxCommandEvent& WXUNUSED(event))
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

	wxString name;
	Dataset2D* newData = GetCurveFitData(order, plotList[optionsGrid->GetSelectedRows()[0] - 1], name);
	
	AddCurve(newData, name);
}

//==========================================================================
// Class:			MainFrame
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
//		None
//
//==========================================================================
Dataset2D* MainFrame::GetCurveFitData(const unsigned int &order,
	const Dataset2D* data, wxString &name) const
{
	CurveFit::PolynomialFit fitData = CurveFit::DoPolynomialFit(*data, order);

	Dataset2D *newData = new Dataset2D(*data);
	unsigned int i;
	for (i = 0; i < newData->GetNumberOfPoints(); i++)
		newData->GetYPointer()[i] = CurveFit::EvaluateFit(newData->GetXData(i), fitData);

	name = GetCurveFitName(fitData, optionsGrid->GetSelectedRows()[0]);

	delete [] fitData.coefficients;

	return newData;
}

//==========================================================================
// Class:			MainFrame
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
wxString MainFrame::GetCurveFitName(const CurveFit::PolynomialFit &fitData,
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
// Class:			MainFrame
// Function:		ContextToggleGridlines
//
// Description:		Toggles gridlines for the entire plot on and off.
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
void MainFrame::ContextToggleGridlines(wxCommandEvent& WXUNUSED(event))
{
	if (plotArea->GetGridOn())
		plotArea->SetGridOff();
	else
		plotArea->SetGridOn();

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScale
//
// Description:		Autoscales the plot.
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
void MainFrame::ContextAutoScale(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScale();
	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCursorValues
//
// Description:		Updates the values for the cursors and their differences
//					in the options grid.
//
// Input Arguments:
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

	// TODO:  This would be nicer with smart precision so we show enough digits but not too many

	// For each curve, update the cursor values
	int i;
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		UpdateSingleCursorValue(i, leftValue, colLeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, colRightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left(leftValue), right(rightValue);
			plotList[i - 1]->GetYAt(left);
			plotList[i - 1]->GetYAt(right);
			optionsGrid->SetCellValue(i, colDifference, wxString::Format("%f", right - left));
			optionsGrid->SetCellValue(0, colDifference, wxString::Format("%f", rightValue - leftValue));
		}
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row		= const unsigned int& specifying the grid row
//		value	= const double& specifying the value to populate
//		column	= const unsigned int& specifying which grid column to populate
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateSingleCursorValue(const unsigned int &row,
	double value, const unsigned int &column, const bool &isVisible)
{
	if (isVisible)
	{
		optionsGrid->SetCellValue(0, column, wxString::Format("%f", value));

		if (plotList[row - 1]->GetYAt(value))
			optionsGrid->SetCellValue(row, column, _T("*") + wxString::Format("%f", value));
		else
			optionsGrid->SetCellValue(row, column, wxString::Format("%f", value));
	}
	else
	{
		optionsGrid->SetCellValue(0, column, wxEmptyString);
		optionsGrid->SetCellValue(row, column, wxEmptyString);

		// The difference column only exists if both cursors are visible
		optionsGrid->SetCellValue(0, colDifference, wxEmptyString);
		optionsGrid->SetCellValue(row, colDifference, wxEmptyString);
	}
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::DisplayMathChannelDialog(wxString defaultInput)
{
	// Display input dialog in which user can specify the math desired
	wxString message(_T("Enter the math you would like to perform:\n\n"));
	message.Append(_T("    Use [x] notation to specify channels, where x = 0 is Time, x = 1 is the first data channel, etc.\n"));
	message.Append(_T("    Valid operations are: +, -, *, /, %, ddt, int and fft.\n"));
	message.Append(_T("    Use () to specify order of operations"));

	AddCurve(::wxGetTextFromUser(message, _T("Specify Math Channel"), defaultInput, this));
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::DisplayAxisRangeDialog(const PlotContext &axis)
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
// Class:			MainFrame
// Function:		GetCurrentAxisRange
//
// Description:		Returns the range for the specified axis.
//
// Input Arguments:
//		axis	= const PlotContext&
//
// Output Arguments:
//		min		= double&
//		max		= double&
//
// Return Value:
//		bool, true on success, false otherwise
//
//==========================================================================
bool MainFrame::GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const
{
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
		return false;
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetNewAxisRange
//
// Description:		Returns the range for the specified axis.
//
// Input Arguments:
//		axis	= const PlotContext&
//		min		= const double&
//		max		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetNewAxisRange(const PlotContext &axis, const double &min, const double &max)
{
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
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesBottom
//
// Description:		Toggles gridlines for the bottom axis.
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
void MainFrame::ContextToggleGridlinesBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetBottomGrid(!plotArea->GetBottomGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleBottom
//
// Description:		Auto-scales the bottom axis.
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
void MainFrame::ContextAutoScaleBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleBottom();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetRangeBottom
//
// Description:		Dispalys a dialog box for setting the axis range.
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
void MainFrame::ContextSetRangeBottom(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextXAxis);
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesLeft
//
// Description:		Toggles gridlines for the bottom axis.
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
void MainFrame::ContextToggleGridlinesLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftGrid(!plotArea->GetLeftGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleLeft
//
// Description:		Toggles gridlines for the bottom axis.
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
void MainFrame::ContextAutoScaleLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleLeft();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetRangeLeft
//
// Description:		Dispalys a dialog box for setting the axis range.
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
void MainFrame::ContextSetRangeLeft(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextLeftYAxis);
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextToggleGridlinesRight
//
// Description:		Toggles gridlines for the bottom axis.
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
void MainFrame::ContextToggleGridlinesRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightGrid(!plotArea->GetRightGrid());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextAutoScaleRight
//
// Description:		Toggles gridlines for the bottom axis.
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
void MainFrame::ContextAutoScaleRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScaleRight();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetRangeRight
//
// Description:		Dispalys a dialog box for setting the axis range.
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
void MainFrame::ContextSetRangeRight(wxCommandEvent& WXUNUSED(event))
{
	DisplayAxisRangeDialog(plotContextRightYAxis);
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPlotBGColor
//
// Description:		Displays a dialog allowing the user to specify the plot's
//					background color.
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
FilterParameters MainFrame::DisplayFilterDialog(void)
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
// Class:			MainFrame
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
void MainFrame::ApplyFilter(const FilterParameters &parameters, Dataset2D &data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Cutoff frequency may be incorrect!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	FilterBase *filter = GetFilter(parameters, factor / (data.GetXData(1) - data.GetXData(0)), data.GetYData(0));

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
// Class:			MainFrame
// Function:		GetFilter
//
// Description:		Returns a filter matching the specified parameters.
//
// Input Arguments:
//		parameters		= const FilterParameters&
//		sampleRate		= const double& [sec]
//		initialValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		FilterBase*
//
//==========================================================================
FilterBase* MainFrame::GetFilter(const FilterParameters &parameters,
	const double &sampleRate, const double &initialValue) const
{
	FilterBase *filter = NULL;
	switch (parameters.type)
	{
	case FilterParameters::TypeLowPass:
		if ((parameters.order == 1 && !parameters.phaseless) || (parameters.order == 2 && parameters.phaseless))
			filter = new LowPassFirstOrderFilter(parameters.cutoffFrequency, sampleRate, initialValue);
		else if ((parameters.order == 2 && !parameters.phaseless) || (parameters.order == 4 && parameters.phaseless))
			filter = new LowPassSecondOrderFilter(parameters.cutoffFrequency, parameters.dampingRatio, sampleRate, initialValue);
		else
			assert(false);
		break;

	case FilterParameters::TypeHighPass:
		assert(parameters.order == 1);
		filter = new HighPassFirstOrderFilter(parameters.cutoffFrequency, sampleRate, initialValue);
		break;

	default:
		assert(false);
	}

	return filter;
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetLogarithmicBottom
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
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
void MainFrame::ContextSetLogarithmicBottom(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetXLogarithmic(!plotArea->GetXLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetLogarithmicLeft
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
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
void MainFrame::ContextSetLogarithmicLeft(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetLeftLogarithmic(!plotArea->GetLeftLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextSetLogarithmicRight
//
// Description:		Event handler for right Y-axis context menu Set Logarithmic event.
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
void MainFrame::ContextSetLogarithmicRight(wxCommandEvent& WXUNUSED(event))
{
	plotArea->SetRightLogarithmic(!plotArea->GetRightLogarithmic());
	plotArea->ClearZoomStack();
}

//==========================================================================
// Class:			MainFrame
// Function:		TestSignalOperations
//
// Description:		Performs tests on signal operation classes.
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
