/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  guiUtilities.cpp
// Date:  12/9/2016
// Auth:  K. Loux
// Desc:  Utility methods for interacting with the user.

// Local headers
#include "lp2d/utilities/guiUtilities.h"

namespace LibPlot2D
{

//=============================================================================
// Namespace:		GuiUtilities
// Function:		GetFileNameFromUser
//
// Description:		Displays a dialog asking the user to specify a file name.
//					Arguments allow this to be for opening or saving files,
//					with different options for the wildcards.
//
// Input Arguments:
//		parent				= wxWindow*
//		dialogTitle			= wxString containing the title for the dialog
//		defaultDirectory	= wxString specifying the initial directory
//		defaultFileName		= wxString specifying the default file name
//		wildcard			= wxString specifying the list of file types to
//							  allow the user to select
//		style				= long integer specifying the type of dialog
//							  (this is usually wxFD_OPEN or wxFD_SAVE)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing the paths and file names of the specified files,
//		or and empty array if the user cancels
//
//=============================================================================
wxArrayString GuiUtilities::GetFileNameFromUser(wxWindow* parent,
	wxString dialogTitle, wxString defaultDirectory, wxString defaultFileName,
	wxString wildcard, long style)
{
	wxArrayString pathsAndFileNames;

	wxFileDialog dialog(parent, dialogTitle, defaultDirectory, defaultFileName,
		wildcard, style);

	dialog.CenterOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		// If this was an open dialog, we want to get all of the selected paths,
		// otherwise, just get the one selected path
		if (style & wxFD_OPEN)
			dialog.GetPaths(pathsAndFileNames);
		else
			pathsAndFileNames.Add(dialog.GetPath());
	}

	return pathsAndFileNames;
}

//=============================================================================
// Namespace:		GuiUtilities
// Function:		ExtractFileNameFromPath
//
// Description:		Removes the path from the path and file name.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString GuiUtilities::ExtractFileNameFromPath(const wxString &pathAndFileName)
{
	unsigned int start;
#ifdef __WXMSW__
	start = pathAndFileName.find_last_of(_T("\\")) + 1;
#else
	start = pathAndFileName.find_last_of(_T("/")) + 1;
#endif
	return pathAndFileName.Mid(start);
}

}// namespace LibPlot2D
