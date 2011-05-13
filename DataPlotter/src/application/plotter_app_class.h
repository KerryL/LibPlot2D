/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotter_app_class.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  The application class.
// History:

#ifndef _PLOTTER_APP_CLASS_H_
#define _PLOTTER_APP_CLASS_H_

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

private:
	// The main class for the application - this object is the parent for all other objects
	MainFrame *mainFrame;
};

// Declare the application object (have wxWidgets create the wxGetApp() function)
DECLARE_APP(DataPlotterApp);

#endif// _PLOTTER_APP_CLASS_H_