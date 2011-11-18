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

// Local headers
#include "application/drop_target_class.h"
#include "application/main_frame_class.h"

//==========================================================================
// Class:			DropTarget
// Function:		DropTarget
//
// Description:		Constructor for DROP_TARGET class.
//
// Input Arguments:
//		_MainFrame	= &MAIN_FRAME, reference to main application window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DropTarget::DropTarget(MainFrame &_mainFrame) : mainFrame(_mainFrame)
{
}

//==========================================================================
// Class:			DropTarget
// Function:		~DropTarget
//
// Description:		Destructor for DropTarget class.
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
DropTarget::~DropTarget()
{
}

//==========================================================================
// Class:			DropTarget
// Function:		OnDropFiles
//
// Description:		Overloaded virtual method from wxFileDropTarget.
//
// Input Arguments:
//		x			= wxCoord (unused)
//		y			= wxCoord (unused)
//		filenames	= const &wxArrayString containing the list of filenames
//					  being dropped
//
// Output Arguments:
//		None
//
// Return Value:
//		true to accept the data, false to veto
//
//==========================================================================
bool DropTarget::OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
							  const wxArrayString &filenames)
{
	// Load each file
	unsigned int i;
	for (i = 0; i < filenames.Count(); i++)
		mainFrame.LoadFile(filenames[i]);

	return true;
}