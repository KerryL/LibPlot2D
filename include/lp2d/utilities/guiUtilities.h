/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  guiUtilities.h
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Utility methods for interacting with the user.

#ifndef GUI_UTILITIES_H_

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

/// Collection of utilities for performing common user interaction tasks.
namespace GuiUtilities
{

/// Displays a dialog from which the user can select files.
///
/// \param parent           Window over which the new dialog should be
///                         centered.
/// \param dialogTitle	    Message to display in dialog title bar.
/// \param defaultDirectory Initial directory to display.
/// \param defaultFileName  Initial file name to display.
/// \param wildcard			List of patterns to allow the user to select.
///                         Typically this will look something like "File type
///                         1 (*.type1)|*.type1|File type 2 (*.type2)|*.type2".
/// \param style            Indicates the type of dialog to display (this is
///                         usually wxFD_OPEN or wxFD_SAVE).  See
///                         http://docs.wxwidgets.org/3.1/filedlg_8h.html for
///                         more information.
wxArrayString GetFileNameFromUser(wxWindow* parent, wxString dialogTitle,
	wxString defaultDirectory, wxString defaultFileName, wxString wildcard,
	long style);

/// Extracts the file name from the specified path.
///
/// \param pathAndFileName Combined path and file name.
///
/// \returns The file name only.
wxString ExtractFileNameFromPath(const wxString &pathAndFileName);

}// namespace GuiUtilities

}// namespace LibPlot2D

#endif// GUI_UTILITIES_H_
