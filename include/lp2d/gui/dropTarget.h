/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  dropTarget.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//        when the user drags-and-drops them onto the main window.

#ifndef DROP_TARGET_H_
#define DROP_TARGET_H_

// wxWidgets headers
#include <wx/dnd.h>

namespace LibPlot2D
{

// Local forward declarations
class GuiInterface;

// The main class declaration
class DropTarget : public wxDropTarget
{
public:
	// Constructor
	explicit DropTarget(GuiInterface &guiInterface);
	~DropTarget() = default;

	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

private:
	GuiInterface &guiInterface;

	bool OnDropFiles(const wxArrayString &filenames);
	bool OnDropText(const wxString& data);
};

}// namespace LibPlot2D

#endif// DROP_TARGET_H_
