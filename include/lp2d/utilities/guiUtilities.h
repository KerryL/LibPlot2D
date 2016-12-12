/*=============================================================================
                                    DataPlotter
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

namespace GuiUtilities
{

wxArrayString GetFileNameFromUser(wxWindow* parent, wxString dialogTitle,
	wxString defaultDirectory, wxString defaultFileName, wxString wildcard,
	long style);

wxString ExtractFileNameFromPath(const wxString &pathAndFileName);

}// namespace GuiUtilities

}// namespace LibPlot2D

#endif// GUI_UTILITIES_H_
