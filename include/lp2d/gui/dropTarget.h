/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  dropTarget.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derives from wxDropTarget and overrides OnDropFiles to load files
//        when the user drags-and-drops them onto the main window.

#ifndef DROP_TARGET_H_
#define DROP_TARGET_H_

// wxWidgets headers
#include <wx/dnd.h>

namespace LibPlot2D
{

// Local forward declarations
class GuiInterface;

/// Class derived from wxDropTarget in order to allow drag-and-drop operations.
class DropTarget : public wxDropTarget
{
public:
	/// Constructor.
	///
	/// \param guiInterface Reference to the object that is used to load files.
	explicit DropTarget(GuiInterface &guiInterface);
	~DropTarget() = default;

	/// Method that fires when objects are dragged and dropped onto the
	/// associated window.
	///
	/// \param x   X-coordinate of the drop.
	/// \param y   Y-coordinate of the drop.
	/// \param def Type of drag operation.
	///
	/// \returns The result of the drag operation.
	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def) override;

private:
	GuiInterface &mGuiInterface;

	bool OnDropFiles(const wxArrayString &filenames);
	bool OnDropText(const wxString& data);
};

}// namespace LibPlot2D

#endif// DROP_TARGET_H_
