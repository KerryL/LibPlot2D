/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotter_app_class.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  The application class.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "application/plotter_app_class.h"
#include "application/main_frame_class.h"

// Implement the application (have wxWidgets set up the appropriate entry points, etc.)
IMPLEMENT_APP(DataPlotterApp);

//==========================================================================
// Class:			DataPlotterApp
// Function:		Constant Declarations
//
// Description:		Constant declarations for the DataPlotterApp class.
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
const wxString DataPlotterApp::dataPlotterTitle = _T("Data Plotter");
const wxString DataPlotterApp::dataPlotterName = _T("DataPlotterApplication");
const wxString DataPlotterApp::creator = _T("Kerry Loux");

//==========================================================================
// Class:			DataPlotterApp
// Function:		OnInit
//
// Description:		Initializes the application window.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool = true for successful window initialization, false for error
//
//==========================================================================
bool DataPlotterApp::OnInit()
{
	// Set the application's name and the vendor's name
	SetAppName(dataPlotterName);
	SetVendorName(creator);

	// Create the MainFrame object - this is the parent for all other objects
	mainFrame = new MainFrame();

	// Make sure the MainFrame was successfully created
	if (mainFrame == NULL)
		return false;

	// Make the window visible
	mainFrame->Show(true);

	// Bring the window to the top
	//SetTopWindow(mainFrame);

	return true;
}
