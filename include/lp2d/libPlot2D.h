/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotterApp.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  The application class.

#ifndef LIBPLOT2D_H_
#define LIBPLOT2D_H_

// wxWidgets headers
#include <wx/wx.h>

/// Core namespace for the LibPlot2D library.
namespace LibPlot2D
{
	/// \name Version information strings.
	/// @{

	extern const wxString versionString;///< Latest git tag.
	extern const wxString gitHash;///< Short hash for current git commit.

	/// @}

}// namespace LibPlot2D

#endif// LIBPLOT2D_H_
