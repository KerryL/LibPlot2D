/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  mainFrame.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MainFrame class.  Uses wxWidgets for the GUI components.
// History:

// Standard C++ headers
#include <algorithm>
#include <map>

// wxWidgets headers
#include <wx/grid.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>
#include <wx/file.h>
#include <wx/clipbrd.h>

// Local headers
#include "application/mainFrame.h"
#include "application/plotterApp.h"
#include "application/dropTarget.h"
#include "application/rangeLimitsDialog.h"
#include "application/filterDialog.h"
#include "application/createSignalDialog.h"
#include "application/dataFiles/genericFile.h"
#include "application/dataFiles/baumullerFile.h"
#include "application/dataFiles/kollmorgenFile.h"
#include "application/dataFiles/customFile.h"
#include "application/dataFiles/customXMLFile.h"
#include "application/dataFiles/customFileFormat.h"
#include "application/frfDialog.h"
#include "application/fftDialog.h"
#include "application/textInputDialog.h"
#include "renderer/plotRenderer.h"
#include "renderer/color.h"
#include "utilities/dataset2D.h"
#include "utilities/math/plotMath.h"
#include "utilities/signals/integral.h"
#include "utilities/signals/derivative.h"
#include "utilities/signals/rms.h"
#include "utilities/signals/fft.h"
#include "utilities/math/expressionTree.h"
#include "utilities/signals/filter.h"
#include "utilities/arrayStringCompare.h"

// *nix Icons
#ifdef __WXGTK__
#include "../../res/icons/plots16.xpm"
#include "../../res/icons/plots24.xpm"
#include "../../res/icons/plots32.xpm"
#include "../../res/icons/plots48.xpm"
#include "../../res/icons/plots64.xpm"
#include "../../res/icons/plots128.xpm"
#endif

// Testing prototypes
//void TestSignalOperations(void);

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

	if (!CustomFileFormat::CustomDefinitionsExist())
		wxMessageBox(_T("Warning:  Custom file definitions not found!"),
		_T("Custom File Formats"), wxICON_WARNING, this);

	currentFileFormat = FormatGeneric;

	// Also initialize the random number generator
	srand(time(NULL));

	//TestSignalOperations();
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
	splitter->SetSize(GetClientSize());
	splitter->SetSashGravity(1.0);
	splitter->SetMinimumPaneSize(150);

	SetSizerAndFit(topSizer);
	splitter->SetSashPosition(splitter->GetSashPosition(), false);
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

	plotArea->SetMinSize(wxSize(650, 320));
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
	optionsGrid->SetColFormatNumber(colLineSize);
	optionsGrid->SetColFormatNumber(colMarkerSize);
	optionsGrid->SetColFormatFloat(colLeftCursor);
	optionsGrid->SetColFormatFloat(colRightCursor);
	optionsGrid->SetColFormatFloat(colDifference);
	optionsGrid->SetColFormatBool(colVisible);
	optionsGrid->SetColFormatBool(colRightAxis);

	optionsGrid->SetColLabelValue(colName, _T("Curve"));
	optionsGrid->SetColLabelValue(colColor, _T("Color"));
	optionsGrid->SetColLabelValue(colLineSize, _T("Line"));
	optionsGrid->SetColLabelValue(colMarkerSize, _T("Marker"));
	optionsGrid->SetColLabelValue(colLeftCursor, _T("Left Cursor"));
	optionsGrid->SetColLabelValue(colRightCursor, _T("Right Cursor"));
	optionsGrid->SetColLabelValue(colDifference, _T("Difference"));
	optionsGrid->SetColLabelValue(colVisible, _T("Visible"));
	optionsGrid->SetColLabelValue(colRightAxis, _T("Right Axis"));

	optionsGrid->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);
	optionsGrid->EnableDragRowSize(false);

	unsigned int i;
	for (i = 1; i < colCount; i++)// Skip the name column
		optionsGrid->AutoSizeColLabelSize(i);

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

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer->Add(topSizer);

	topSizer->Add(new wxButton(parent, idButtonOpen, _T("&Open")), 1, wxGROW);
	topSizer->Add(new wxButton(parent, idButtonAutoScale, _T("&Auto Scale")), 1, wxGROW);
	topSizer->Add(new wxButton(parent, idButtonRemoveCurve, _T("&Remove")), 1, wxGROW);
	topSizer->Add(new wxButton(parent, idButtonReloadData, _T("Reload &Data")), 1, wxGROW);

	buttonSizer->AddStretchSpacer(1);
	buttonSizer->Add(new wxStaticText(parent, wxID_ANY, DataPlotterApp::versionString));

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
	SetIcon(wxIcon(plots16_xpm));
	SetIcon(wxIcon(plots24_xpm));
	SetIcon(wxIcon(plots32_xpm));
	SetIcon(wxIcon(plots48_xpm));
	SetIcon(wxIcon(plots64_xpm));
	SetIcon(wxIcon(plots128_xpm));
#endif

	SetDropTarget(dynamic_cast<wxDropTarget*>(new DropTarget(*this)));

	const int entryCount(5);
	wxAcceleratorEntry entries[entryCount];
	entries[0].Set(wxACCEL_CTRL, (int)'c', idPlotContextCopy);
	entries[1].Set(wxACCEL_CTRL, (int)'v', idPlotContextPaste);
	entries[2].Set(wxACCEL_CTRL, (int)'o', idButtonOpen);
	entries[3].Set(wxACCEL_CTRL, (int)'a', idButtonAutoScale);
	entries[4].Set(wxACCEL_CTRL, (int)'r', idButtonRemoveCurve);
	wxAcceleratorTable accel(entryCount, entries);
	SetAcceleratorTable(accel);
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
	EVT_BUTTON(idButtonReloadData,					MainFrame::ButtonReloadDataClickedEvent)

	// Grid control
	EVT_GRID_CELL_RIGHT_CLICK(MainFrame::GridRightClickEvent)
	EVT_GRID_CELL_LEFT_DCLICK(MainFrame::GridDoubleClickEvent)
	EVT_GRID_CELL_LEFT_CLICK(MainFrame::GridLeftClickEvent)
	EVT_GRID_CELL_CHANGE(MainFrame::GridCellChangeEvent)
	EVT_GRID_LABEL_RIGHT_CLICK(MainFrame::GridLabelRightClickEvent)

	// Context menu
	EVT_MENU(idContextAddMathChannel,				MainFrame::ContextAddMathChannelEvent)
	EVT_MENU(idContextFRF,							MainFrame::ContextFRFEvent)
	EVT_MENU(idContextCreateSignal,					MainFrame::ContextCreateSignalEvent)
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

	EVT_MENU(idPlotContextCopy,						MainFrame::ContextCopy)
	EVT_MENU(idPlotContextPaste,					MainFrame::ContextPaste)
	EVT_MENU(idPlotContextToggleGridlines,			MainFrame::ContextToggleGridlines)
	EVT_MENU(idPlotContextAutoScale,				MainFrame::ContextAutoScale)
	EVT_MENU(idPlotContextWriteImageFile,			MainFrame::ContextWriteImageFile)
	EVT_MENU(idPlotContextExportData,				MainFrame::ContextExportData)

	EVT_MENU(idPlotContextBGColor,					MainFrame::ContextPlotBGColor)
	EVT_MENU(idPlotContextGridColor,				MainFrame::ContextGridColor)

	EVT_MENU(idPlotContextToggleBottomGridlines,	MainFrame::ContextToggleGridlinesBottom)
	EVT_MENU(idPlotContextSetBottomRange,			MainFrame::ContextSetRangeBottom)
	EVT_MENU(idPlotContextSetBottomLogarithmic,		MainFrame::ContextSetLogarithmicBottom)
	EVT_MENU(idPlotContextAutoScaleBottom,			MainFrame::ContextAutoScaleBottom)
	EVT_MENU(idPlotContextEditBottomLabel,			MainFrame::ContextEditBottomLabel)

	//EVT_MENU(idPlotContextToggleTopGridlines,		MainFrame::)
	//EVT_MENU(idPlotContextSetTopRange,			MainFrame::)
	//EVT_MENU(idPlotContextSetTopLogarithmic,		MainFrame::)
	//EVT_MENU(idPlotContextAutoScaleTop,			MainFrame::)

	EVT_MENU(idPlotContextToggleLeftGridlines,		MainFrame::ContextToggleGridlinesLeft)
	EVT_MENU(idPlotContextSetLeftRange,				MainFrame::ContextSetRangeLeft)
	EVT_MENU(idPlotContextSetLeftLogarithmic,		MainFrame::ContextSetLogarithmicLeft)
	EVT_MENU(idPlotContextAutoScaleLeft,			MainFrame::ContextAutoScaleLeft)
	EVT_MENU(idPlotContextEditLeftLabel,			MainFrame::ContextEditLeftLabel)

	EVT_MENU(idPlotContextToggleRightGridlines,		MainFrame::ContextToggleGridlinesRight)
	EVT_MENU(idPlotContextSetRightRange,			MainFrame::ContextSetRangeRight)
	EVT_MENU(idPlotContextSetRightLogarithmic,		MainFrame::ContextSetLogarithmicRight)
	EVT_MENU(idPlotContextAutoScaleRight,			MainFrame::ContextAutoScaleRight)
	EVT_MENU(idPlotContextEditRightLabel,			MainFrame::ContextEditRightLabel)
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
	// Set up the wildcard specifications (done here for readability)
	wxString wildcard("All files (*.*)|*.*");
	wildcard.append("|Comma Separated (*.csv)|*.csv");
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	wxArrayString fileList = GetFileNameFromUser(_T("Open Data File"), wxEmptyString, wxEmptyString,
		wildcard, wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

	if (fileList.GetCount() == 0)
		return;
	else if (fileList.GetCount() > 1)
		LoadFiles(fileList);
	else
		LoadFile(fileList[0]);
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
	wxArrayString pathAndFileName = GetFileNameFromUser(_T("Save Image File"), wxEmptyString, wxEmptyString,
		_T("PNG Image (*.png)|*.png|Bitmap Image (*.bmp)|*.bmp|JPEG Image (*.jpg, *.jpeg)|*.jpg;*.jpeg|TIFF Image (*.tif)|*.tif"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (pathAndFileName.IsEmpty())
		return;

	plotArea->WriteImageToFile(pathAndFileName[0]);
}

//==========================================================================
// Class:			MainFrame
// Function:		ContexExportData
//
// Description:		Exports the data to file.
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
void MainFrame::ContextExportData(wxCommandEvent& WXUNUSED(event))
{
	wxString wildcard(_T("Comma Separated (*.csv)|*.csv"));
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	wxArrayString pathAndFileName = GetFileNameFromUser(_T("Save As"),
		wxEmptyString, wxEmptyString, wildcard, wxFD_SAVE);

	if (pathAndFileName.Count() == 0)
		return;

	if (wxFile::Exists(pathAndFileName[0]))
	{
		if (wxMessageBox(_T("File exists.  Overwrite?"), _T("Overwrite File?"), wxYES_NO, this) == wxNO)
			return;
	}

	wxString delimiter;
	if (pathAndFileName[0].Mid(pathAndFileName[0].Last('.')).CmpNoCase(_T(".txt")) == 0)
		delimiter = _T("\t");
	else
		delimiter = _T(",");// FIXME:  Need to handle descriptions containing commas so we don't have problems with import later on

	// Export both x and y data in case of asynchronous data or FFT, etc.
	std::ofstream outFile(pathAndFileName[0].mb_str(), std::ios::out);
	if (!outFile.is_open() || !outFile.good())
	{
		wxMessageBox(_T("Could not open '") + pathAndFileName[0] + _T("' for output."),
			_T("Error Writing File"), wxICON_ERROR, this);
		return;
	}

	unsigned int i, j(0);
	for (i = 1; i < plotList.GetCount() + 1; i++)
	{
		if (optionsGrid->GetCellValue(i, colName).Contains(_T("FFT")) ||
			optionsGrid->GetCellValue(i, colName).Contains(_T("FRF")))
			outFile << _T("Frequency [Hz]") << delimiter;
		else
			outFile << genericXAxisLabel << delimiter;

		outFile << optionsGrid->GetCellValue(i, colName);

		if (i == plotList.GetCount())
			outFile << endl;
		else
			outFile << delimiter;
	}

	bool done(false);
	while (!done)
	{
		done = true;
		for (i = 0; i < plotList.GetCount(); i++)
		{
			if (j < plotList[i]->GetNumberOfPoints())
				outFile << plotList[i]->GetXData(j) << delimiter << plotList[i]->GetYData(j);
			else
				outFile << delimiter;

			if (i == plotList.GetCount() - 1)
				outFile << endl;
			else
				outFile << delimiter;

			if (j + 1 < plotList[i]->GetNumberOfPoints())
				done = false;
		}

		j++;
	}

	outFile.close();
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
// Description:		Event fires when user clicks "Remove" button.
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
	// Actually - this method might require sorting and removing rows in reverse
	// order to avoide changing row indices during deletion process
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
	for (i = optionsGrid->GetRows() - 1; i > 0; i--)
	{
		if (optionsGrid->IsInSelection(i, 0))
			RemoveCurve(i - 1);
	}

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonReloadDataClickedEvent
//
// Description:		Event fires when user clicks "Reload Data" button.
//
// Input Arguments:
//		event	= &wxCommandEvent (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonReloadDataClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	if (lastFileLoaded.IsEmpty())
		return;

	LoadFile(lastFileLoaded/*, true*/);// TODO:  How does this work now?
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
	wxMenu *contextMenu = new wxMenu();

	contextMenu->Append(idContextAddMathChannel, _T("Add Math Channel"));
	contextMenu->Append(idContextFRF, _T("Frequency Response"));
	//contextMenu->Append(idContextSetXData, _T("Use as X-Axis"));

	contextMenu->AppendSeparator();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));// FIXME:  Eventually, maybe this can be integrated into the "Add Math Channel" dialog?

	contextMenu->AppendSeparator();

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

	PopupMenu(contextMenu, position);

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
	contextMenu->Append(idPlotContextCopy, _T("Copy"));
	contextMenu->Append(idPlotContextPaste, _T("Paste"));
	contextMenu->Append(idPlotContextWriteImageFile, _T("Write Image File"));
	contextMenu->Append(idPlotContextExportData, _T("Export Data"));
	contextMenu->AppendSeparator();
	contextMenu->Append(idPlotContextToggleGridlines, _T("Toggle Gridlines"));
	contextMenu->Append(idPlotContextAutoScale, _T("Auto Scale"));
	contextMenu->Append(idPlotContextBGColor, _T("Set Background Color"));
	contextMenu->Append(idPlotContextGridColor, _T("Set Gridline Color"));

	if (wxTheClipboard->Open())
	{
		if (!wxTheClipboard->IsSupported(wxDF_TEXT))
			contextMenu->Enable(idPlotContextPaste, false);
		wxTheClipboard->Close();
	}
	else
		contextMenu->Enable(idPlotContextPaste, false);

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
	contextMenu->Append(baseEventId + 4, _T("Edit Label"));

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
	wxArrayString pathsAndFileNames;

	wxFileDialog dialog(this, dialogTitle, defaultDirectory, defaultFileName,
		wildcard, style);

	dialog.CenterOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (style & wxFD_OPEN)
			dialog.GetPaths(pathsAndFileNames);
		else
			pathsAndFileNames.Add(dialog.GetPath());
	}

	return pathsAndFileNames;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadFile
//
// Description:		Public method for loading a single object from file.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadFile(const wxString &pathAndFileName)
{
	DataFile *file = GetDataFile(pathAndFileName);

	DataFile::SelectionData selectionInfo;
	if (file->DescriptionsMatch(lastDescriptions))
		selectionInfo = lastSelectionInfo;

	file->GetSelectionsFromUser(selectionInfo, this);
	if (selectionInfo.selections.Count() < 1 || !file->Load(selectionInfo))
	{
		delete file;
		return false;
	}

	if (selectionInfo.removeExisting)
		ClearAllCurves();

	unsigned int i;
	for (i = 0; i < file->GetDataCount(); i++)
		AddCurve(file->GetDataset(i), file->GetDescription(i + 1));

	SetTitleFromFileName(pathAndFileName);
	genericXAxisLabel = file->GetDescription(0);
	SetXDataLabel(genericXAxisLabel);
	plotArea->SaveCurrentZoom();

	lastFileLoaded = pathAndFileName;
	lastSelectionInfo = selectionInfo;
	lastDescriptions = file->GetAllDescriptions();

	delete file;
	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadFiles
//
// Description:		Method for loading a multiple files.
//
// Input Arguments:
//		fileList	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		true for files successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadFiles(const wxArrayString &fileList)
{
	unsigned int i, j;
	std::vector<bool> loaded(fileList.size());
	std::vector<DataFile*> files(fileList.size());
	typedef std::map<wxArrayString, DataFile::SelectionData, ArrayStringCompare> SelectionMap;
	SelectionMap selectionInfoMap;
	SelectionMap::const_iterator it;
	DataFile::SelectionData selectionInfo;
	for (i = 0; i < fileList.Count(); i++)
	{
		files[i] = GetDataFile(fileList[i]);
		it = selectionInfoMap.find(files[i]->GetAllDescriptions());
		if (it == selectionInfoMap.end())
		{
			if (files[i]->DescriptionsMatch(lastDescriptions))
				selectionInfo = lastSelectionInfo;
			else
				selectionInfo.selections.Clear();
			files[i]->GetSelectionsFromUser(selectionInfo, this);
			if (selectionInfo.selections.Count() < 1)
			{
				for (j = 0; j <= i; j++)
					delete files[j];
				return false;
			}
			selectionInfoMap[files[i]->GetAllDescriptions()] = selectionInfo;
		}
		else
			selectionInfo = it->second;

		loaded[i] = selectionInfo.selections.Count() > 0 && files[i]->Load(selectionInfo);
	}

	if (selectionInfo.removeExisting)
		ClearAllCurves();

	for (i = 0; i < fileList.Count(); i++)
	{
		if (!loaded[i])
			continue;

		for (j = 0; j < files[i]->GetDataCount(); j++)
			AddCurve(files[i]->GetDataset(j), files[i]->GetDescription(j + 1) + _T(" : ") + ExtractFileNameFromPath(fileList[i]));
	}

	SetTitle(_T("Multiple Files - ") + DataPlotterApp::dataPlotterTitle);
	genericXAxisLabel = files[0]->GetDescription(0);
	SetXDataLabel(genericXAxisLabel);
	plotArea->SaveCurrentZoom();

	lastFileLoaded = fileList[fileList.Count() - 1];
	lastSelectionInfo = selectionInfo;
	lastDescriptions = files[files.size() - 1]->GetAllDescriptions();

	for (i = 0; i < files.size(); i++)
		delete files[i];

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		LoadText
//
// Description:		Public method for loading a single object from text.
//					This writes the text to a temporary file, then tries to
//					open it using normal methods.
//
// Input Arguments:
//		textData	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		true for file successfully loaded, false otherwise
//
//==========================================================================
bool MainFrame::LoadText(const wxString &textData)
{
	wxString tempFileName(GenerateTemporaryFileName());
	while (wxFile::Exists(tempFileName))
		tempFileName = GenerateTemporaryFileName();

	std::ofstream tempFile(tempFileName.mb_str(), std::ios::out);
	if (!tempFile.good() || !tempFile.is_open())
	{
		tempFile.close();
		if (remove(tempFileName.mb_str()) != 0)
			wxMessageBox(_T("Error deleting temporary file '") + tempFileName + _T("'."),
			_T("Could Not Delete File"), wxICON_ERROR, this);
		return false;
	}

	tempFile << textData;
	tempFile.close();

	bool fileLoaded = LoadFile(tempFileName);
	if (remove(tempFileName.mb_str()) != 0)
		wxMessageBox(_T("Error deleting temporary file '") + tempFileName + _T("'."),
		_T("Could Not Delete File"), wxICON_ERROR, this);

	if (fileLoaded)
		SetTitle(_T("Clipboard Data - ") + DataPlotterApp::dataPlotterTitle);

	return fileLoaded;
}

//==========================================================================
// Class:			MainFrame
// Function:		GenerateTemporaryFileName
//
// Description:		Generates a random string of characters to use as a file
//					name (always ends with .tmp).
//
// Input Arguments:
//		length	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString MainFrame::GenerateTemporaryFileName(const unsigned int &length) const
{
	wxString name;
	unsigned int i;
	for (i = 0; i < length; i++)
		name.Append((char)((rand() % 52) + 65));

	name.Append(_T(".tmp"));

	return name;
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
	wxString fileName(ExtractFileNameFromPath(pathAndFileName));
	unsigned int end(fileName.find_last_of(_T(".")));
	SetTitle(fileName.Mid(0, end) + _T(" - ") + DataPlotterApp::dataPlotterTitle);
}

//==========================================================================
// Class:			MainFrame
// Function:		ExtractFileNameFromPath
//
// Description:		Removes the path from the path and file name.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString MainFrame::ExtractFileNameFromPath(const wxString &pathAndFileName) const
{
	unsigned int start;
#ifdef __WXMSW__
	start = pathAndFileName.find_last_of(_T("\\")) + 1;
#else
	start = pathAndFileName.find_last_of(_T("/")) + 1;
#endif
	return pathAndFileName.Mid(start);
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
	ExpressionTree expression(&plotList);
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

	AddCurve(mathChannel, mathString.Upper());
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

	optionsGrid->Scroll(-1, optionsGrid->GetNumberRows());

	plotArea->AddCurve(*data);
	UpdateCurveProperties(index - 1, GetNextColor(index), true, false);
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
	unsigned int maxMarkerSize(5);

	optionsGrid->SetCellRenderer(index, colVisible, new wxGridCellBoolRenderer);
	optionsGrid->SetCellRenderer(index, colRightAxis, new wxGridCellBoolRenderer);
	optionsGrid->SetCellEditor(index, colLineSize, new wxGridCellNumberEditor(0, maxLineSize));
	optionsGrid->SetCellEditor(index, colMarkerSize, new wxGridCellNumberEditor(-1, maxMarkerSize));

	unsigned int i;
	for (i = 1; i < colCount; i++)
			optionsGrid->SetReadOnly(index, i, true);
	optionsGrid->SetReadOnly(index, colLineSize, false);
	optionsGrid->SetReadOnly(index, colMarkerSize, false);
	optionsGrid->SetCellValue(index, colName, name);

	Color color = GetNextColor(index);

	optionsGrid->SetCellBackgroundColour(index, colColor, color.ToWxColor());
	optionsGrid->SetCellValue(index, colLineSize, _T("1"));
	optionsGrid->SetCellValue(index, colMarkerSize, _T("-1"));
	optionsGrid->SetCellValue(index, colVisible, _T("1"));

	int width = optionsGrid->GetColumnWidth(colName);
	optionsGrid->AutoSizeColumn(colName, false);
	if (optionsGrid->GetColumnWidth(colName) < width)
		optionsGrid->SetColumnWidth(colName, width);

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
	CreateGridContextMenu(event.GetPosition() + optionsGrid->GetPosition()
		+ optionsGrid->GetParent()->GetPosition(), event.GetRow());
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
		UpdateCurveProperties(row - 1);
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

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::UpdateCurveProperties(const unsigned int &index)
{
	Color color;
	color.Set(optionsGrid->GetCellBackgroundColour(index + 1, colColor));
	UpdateCurveProperties(index, color,
		!optionsGrid->GetCellValue(index + 1, colVisible).IsEmpty(),
		!optionsGrid->GetCellValue(index + 1, colRightAxis).IsEmpty());
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::UpdateCurveProperties(const unsigned int &index, const Color &color,
	const bool &visible, const bool &rightAxis)
{
	unsigned long lineSize;
	long markerSize;
	optionsGrid->GetCellValue(index + 1, colLineSize).ToULong(&lineSize);
	optionsGrid->GetCellValue(index + 1, colMarkerSize).ToLong(&markerSize);
	plotArea->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
	plotArea->SaveCurrentZoom();
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
	unsigned int row(event.GetRow());
	if (row == 0 || (event.GetCol() != colLineSize && event.GetCol() != colMarkerSize))
	{
		event.Skip();
		return;
	}

	UpdateCurveProperties(row - 1);
}

//==========================================================================
// Class:			MainFrame
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
void MainFrame::GridLabelRightClickEvent(wxGridEvent &event)
{
	wxMenu *contextMenu = new wxMenu();

	contextMenu->Append(idContextCreateSignal, _T("Create Signal"));

	PopupMenu(contextMenu, event.GetPosition() + optionsGrid->GetPosition()
		+ optionsGrid->GetParent()->GetPosition());

	delete contextMenu;
	contextMenu = NULL;
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
wxString MainFrame::ExtractUnitFromDescription(const wxString &description)
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
		location = description.Find(delimiters[i].mb_str());
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
	const wxChar &open, const wxChar &close)
{
	if (s.Len() < 3)
		return false;

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
bool MainFrame::UnitStringToFactor(const wxString &unit, double &factor)
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
	DisplayMathChannelDialog(wxString::Format("[%i]", optionsGrid->GetSelectedRows()[0]));
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
// Class:			MainFrame
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
void MainFrame::ContextCreateSignalEvent(wxCommandEvent& WXUNUSED(event))
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
// Class:			MainFrame
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
void MainFrame::AddFFTCurves(const double& xFactor, Dataset2D *amplitude, Dataset2D *phase,
	Dataset2D *coherence, const wxString &namePortion)
{
	AddCurve(&(amplitude->MultiplyXData(xFactor)), _T("FRF Amplitude, ") + namePortion + _T(", [dB]"));
	SetMarkerSize(optionsGrid->GetRows() - 2, 0);

	if (phase)
	{
		AddCurve(&(phase->MultiplyXData(xFactor)), _T("FRF Phase, ") + namePortion + _T(", [deg]"));
		SetMarkerSize(optionsGrid->GetRows() - 2, 0);
	}

	if (coherence)
	{
		AddCurve(&(coherence->MultiplyXData(xFactor)), _T("FRF Coherence, ") + namePortion + _T(", [-]"));
		SetMarkerSize(optionsGrid->GetRows() - 2, 0);
	}
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
	unsigned int row = optionsGrid->GetSelectedRows()[0];
	Dataset2D *newData = GetFFTData(plotList[row - 1]);
	if (!newData)
		return;

	wxString name = _T("FFT(") + optionsGrid->GetCellValue(row, colName) + _T(")");
	AddCurve(newData, name);
	SetMarkerSize(optionsGrid->GetRows() - 2, 0);
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
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D* pointing to a dataset contining the new FFT data
//
//==========================================================================
Dataset2D* MainFrame::GetFFTData(const Dataset2D* data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	FFTDialog dialog(this, data->GetNumberOfPoints(),
		data->GetNumberOfZoomedPoints(plotArea->GetXMin(), plotArea->GetXMax()),
		data->GetAverageDeltaX() / factor);

	if (dialog.ShowModal() != wxID_OK)
		return NULL;

	if (!PlotMath::XDataConsistentlySpaced(*data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	Dataset2D *newData;

	if (dialog.GetUseZoomedData())
		newData = new Dataset2D(FastFourierTransform::ComputeFFT(GetXZoomedDataset(*data),
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));
	else
		newData = new Dataset2D(FastFourierTransform::ComputeFFT(*data,
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));

	newData->MultiplyXData(factor);

	return newData;
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
		_T("Specify the time to add to time data in original data:\n")
		_T("Use same units as time series.  Positive values shift curve to the right."),
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
// Function:		ContextCopy
//
// Description:		Handles context menu copy command events.
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
void MainFrame::ContextCopy(wxCommandEvent& WXUNUSED(event))
{
	DoCopy();
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextPaste
//
// Description:		Handles context menu paste command events.
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
void MainFrame::ContextPaste(wxCommandEvent& WXUNUSED(event))
{
	DoPaste();
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
	bool showXDifference(false);
	for (i = 1; i < optionsGrid->GetRows(); i++)
	{
		UpdateSingleCursorValue(i, leftValue, colLeftCursor, leftVisible);
		UpdateSingleCursorValue(i, rightValue, colRightCursor, rightVisible);

		if (leftVisible && rightVisible)
		{
			double left(leftValue), right(rightValue);
			if (plotList[i - 1]->GetYAt(left) && plotList[i - 1]->GetYAt(right))
			{
				optionsGrid->SetCellValue(i, colDifference, wxString::Format("%f", right - left));
				showXDifference = true;
			}
			else
				optionsGrid->SetCellValue(i, colDifference, wxEmptyString);
		}
	}

	if (showXDifference)
		optionsGrid->SetCellValue(0, colDifference, wxString::Format("%f", rightValue - leftValue));
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateSingleCursorValue
//
// Description:		Updates a single cursor value.
//
// Input Arguments:
//		row			= const unsigned int& specifying the grid row
//		value		= const double& specifying the value to populate
//		column		= const unsigned int& specifying which grid column to populate
//		isVisible	= const bool& indicating whether or not the cursor is visible
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

		bool exact;
		if (plotList[row - 1]->GetYAt(value, &exact))
		{
			if (exact)
				optionsGrid->SetCellValue(row, column, _T("*") + wxString::Format("%f", value));
			else
				optionsGrid->SetCellValue(row, column, wxString::Format("%f", value));
		}
		else
			optionsGrid->SetCellValue(row, column, wxEmptyString);
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
// Class:			MainFrame
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
Filter* MainFrame::GetFilter(const FilterParameters &parameters,
	const double &sampleRate, const double &initialValue) const
{
	return new Filter(sampleRate, Filter::CoefficientsFromString(std::string(parameters.numerator.mb_str())),
		Filter::CoefficientsFromString(std::string(parameters.denominator.mb_str())), initialValue);
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
// Function:		ContextEditBottomLabel
//
// Description:		Displays a message box asking the user to specify the text
//					for the label.
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
void MainFrame::ContextEditBottomLabel(wxCommandEvent& WXUNUSED(event))
{
	TextInputDialog dialog(_T("Specify label text:"), _T("Edit Label"), plotArea->GetXLabel(), this);
	if (dialog.ShowModal() == wxID_OK)
		plotArea->SetXLabel(dialog.GetText());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextEditLeftLabel
//
// Description:		Displays a message box asking the user to specify the text
//					for the label.
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
void MainFrame::ContextEditLeftLabel(wxCommandEvent& WXUNUSED(event))
{
	TextInputDialog dialog(_T("Specify label text:"), _T("Edit Label"), plotArea->GetLeftYLabel(), this);
	if (dialog.ShowModal() == wxID_OK)
		plotArea->SetLeftYLabel(dialog.GetText());
}

//==========================================================================
// Class:			MainFrame
// Function:		ContextEditRightLabel
//
// Description:		Displays a message box asking the user to specify the text
//					for the label.
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
void MainFrame::ContextEditRightLabel(wxCommandEvent& WXUNUSED(event))
{
	TextInputDialog dialog(_T("Specify label text:"), _T("Edit Label"), plotArea->GetRightYLabel(), this);
	if (dialog.ShowModal() == wxID_OK)
		plotArea->SetRightYLabel(dialog.GetText());
}

//==========================================================================
// Class:			MainFrame
// Function:		SetMarkerSize
//
// Description:		Sets the marker size for the specified curve.
//
// Input Arguments:
//		curve	= const unsigned int&
//		size	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetMarkerSize(const unsigned int &curve, const int &size)
{
	optionsGrid->SetCellValue(curve + 1, colMarkerSize, wxString::Format("%i", size));
	UpdateCurveProperties(curve);
}

//==========================================================================
// Class:			MainFrame
// Function:		GetDataFile
//
// Description:		Determines the correct DataFile object to use for the
//					specified file, and returns a pointer to an instance of that
//					object.
//
// Input Arguments:
//		fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		DataFile*
//
//==========================================================================
DataFile* MainFrame::GetDataFile(const wxString &fileName)
{
	if (BaumullerFile::IsType(fileName))
		return new BaumullerFile(fileName);
	else if (KollmorgenFile::IsType(fileName))
		return new KollmorgenFile(fileName);
	else if (CustomFile::IsType(fileName))
		return new CustomFile(fileName);
	else if (CustomXMLFile::IsType(fileName))
		return new CustomXMLFile(fileName);

	// Don't even check - if we can't open it with any other types,
	// always try to open it with a generic type
	return new GenericFile(fileName);
}

//==========================================================================
// Class:			MainFrame
// Function:		DoCopy
//
// Description:		Handles "copy to clipboard" actions.
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
void MainFrame::DoCopy(void)
{
	if (wxTheClipboard->Open())
	{
		// TODO:  Can we also copy text data here, and choose which to paste depending on context later?  What about copy from one window/paste to another of DataPlotter?
		wxTheClipboard->SetData(new wxBitmapDataObject(plotArea->GetImage()));
		wxTheClipboard->Close();
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		DoPaste
//
// Description:		Handles "paste from clipboard" actions.
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
void MainFrame::DoPaste(void)
{
	if (wxTheClipboard->Open())
	{
		if (wxTheClipboard->IsSupported(wxDF_TEXT))
		{
			wxTextDataObject data;
			wxTheClipboard->GetData(data);
			LoadText(data.GetText());
		}
		wxTheClipboard->Close();
	}
}

//==========================================================================
// Class:			None
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
/*void TestSignalOperations(void)
{
	// Create test data:
	Dataset2D set1(500);
	unsigned int i;
	double dt = 0.01;
	for (i = 0; i < set1.GetNumberOfPoints(); i++)
		set1.GetXPointer()[i] = i * dt;

	double f1 = 5.0 * M_PI * 2.0;
	double f2 = 2.0 * M_PI * 2.0;

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

	// Save original data to file
	set1.ExportDataToFile(_T("set1.txt"));
	set2.ExportDataToFile(_T("set2.txt"));
	set3.ExportDataToFile(_T("set3.txt"));
	set4.ExportDataToFile(_T("set4.txt"));
	set5.ExportDataToFile(_T("set5.txt"));

	// Integral
	Dataset2D intTest1 = DiscreteIntegral::ComputeTimeHistory(set1);
	Dataset2D intTest2 = DiscreteIntegral::ComputeTimeHistory(set2);
	Dataset2D intTest5 = DiscreteIntegral::ComputeTimeHistory(set5);
	intTest1.ExportDataToFile(_T("integral set 1.txt"));
	intTest2.ExportDataToFile(_T("integral set 2.txt"));
	intTest5.ExportDataToFile(_T("integral set 5.txt"));

	// Derivative
	DiscreteDerivative::ComputeTimeHistory(set1).ExportDataToFile(_T("derivative set 1.txt"));
	DiscreteDerivative::ComputeTimeHistory(set2).ExportDataToFile(_T("derivative set 2.txt"));
	DiscreteDerivative::ComputeTimeHistory(intTest1).ExportDataToFile(_T("dofint1.txt"));

	// Root Mean Square
	Dataset2D rms1 = RootMeanSquare::ComputeTimeHistory(set1);
	Dataset2D rms2 = RootMeanSquare::ComputeTimeHistory(set2);
	Dataset2D rms3 = RootMeanSquare::ComputeTimeHistory(set3);
	Dataset2D rms4 = RootMeanSquare::ComputeTimeHistory(set4);
	rms1.ExportDataToFile(_T("rms1.txt"));
	rms2.ExportDataToFile(_T("rms2.txt"));
	rms3.ExportDataToFile(_T("rms3.txt"));
	rms4.ExportDataToFile(_T("rms4.txt"));

	// Fast Fourier Transform
	Dataset2D fft1 = FastFourierTransform::ComputeFFT(set1);
	Dataset2D fft2 = FastFourierTransform::ComputeFFT(set2);
	Dataset2D fft3 = FastFourierTransform::ComputeFFT(set3);
	Dataset2D fft4 = FastFourierTransform::ComputeFFT(set4);
	fft1.ExportDataToFile(_T("fft1.txt"));
	fft2.ExportDataToFile(_T("fft2.txt"));
	fft3.ExportDataToFile(_T("fft3.txt"));
	fft4.ExportDataToFile(_T("fft4.txt"));

	// Filters
	std::vector<double> num, den;
	double wc(5.0), z(1.0);
	num.push_back(wc * wc);
	den.push_back(1.0);
	den.push_back(2.0 * z * wc);
	den.push_back(wc * wc);
	Filter f(100.0, num, den);
	// Coefficients should be:
	// a[0] = 0.00059488399762046393
	// a[1] = 0.0011897679952409279
	// a[2] = 0.00059488399762046393
	// b[0] = -1.9024390243902431
	// b[1] = 0.90481856038072561

	num.clear();
	num.push_back(1.0);
	num.push_back(0.0);
	num.push_back(0.0);
	Filter g(100.0, num, den);
	// Coefficients should be:
	// a[0] = 0.95181439619274233
	// a[1] = -1.9036287923854847
	// a[2] = 0.95181439619274233
	// b[0] = -1.9024390243902431
	// b[1] = 0.90481856038072561
}//*/
