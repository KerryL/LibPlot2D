/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  dropTarget.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//				 when the user drags-and-drops them onto the main window.
// History:

#ifndef DROP_TARGET_H_
#define DROP_TARGET_H_

// wxWidgets headers
#include <wx/dnd.h>

namespace LibPlot2D
{

// Local forward declarations
class MainFrame;

// The main class declaration
class DropTarget : public wxDropTarget
{
public:
	// Constructor
	DropTarget(MainFrame &_mainFrame);

	// Destructor
	virtual ~DropTarget();

	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

private:
	// Reference to main frame
	MainFrame &mainFrame;

	bool OnDropFiles(const wxArrayString &filenames);
	bool OnDropText(const wxString& data);

	char *buffer;

	void ClearBuffer();
};

}// namespace LibPlot2D

#endif// DROP_TARGET_H_