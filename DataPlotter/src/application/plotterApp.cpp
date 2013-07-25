/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotterApp.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  The application class.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "application/plotterApp.h"
#include "application/mainFrame.h"

// Implement the application (have wxWidgets set up the appropriate entry points, etc.)
IMPLEMENT_APP(DataPlotterApp);

//==========================================================================
// Class:			DataPlotterApp
// Function:		Constant Declarations
//
// Description:		Constant declarations for the DataPlotterApp class.
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
const wxString DataPlotterApp::dataPlotterTitle = _T("Data Plotter");
const wxString DataPlotterApp::dataPlotterName = _T("DataPlotterApplication");
const wxString DataPlotterApp::creator = _T("Kerry Loux");
const wxString DataPlotterApp::versionString = _T("v3.10");

//==========================================================================
// Class:			DataPlotterApp
// Function:		OnInit
//
// Description:		Initializes the application window.
//
// Input Arguments:
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
