/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotterApp.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  The application class.
// History:

#ifndef _PLOTTER_APP_H_
#define _PLOTTER_APP_H_

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class MainFrame;

// The application class
class DataPlotterApp : public wxApp
{
public:
	// Initialization function
	bool OnInit();

	// The name of the application
	static const wxString dataPlotterTitle;// As displayed
	static const wxString dataPlotterName;// Internal
	static const wxString creator;
	static const wxString versionString;

private:
	// The main class for the application - this object is the parent for all other objects
	MainFrame *mainFrame;
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(DataPlotterApp);

#endif// _PLOTTER_APP_H_