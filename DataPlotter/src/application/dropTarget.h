/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  drop_target_class.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//				 when the user drags-and-drops them onto the main window.
// History:

#ifndef _DROP_TARGET_CLASS_H_
#define _DROP_TARGET_CLASS_H_

// wxWidgets headers
#include <wx/dnd.h>

// Local forward declarations
class MainFrame;

// The main class declaration
class DropTarget : public wxFileDropTarget
{
public:
	// Constructor
	DropTarget(MainFrame &_mainFrame);

	// Destructor
	~DropTarget();

private:
	// Reference to main frame
	MainFrame &mainFrame;

	// Required override of virtual OnDropFiles handler
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);
};

#endif//  _DROP_TARGET_CLASS_H_