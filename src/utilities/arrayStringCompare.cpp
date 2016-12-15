/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2015

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  mainFrame.cpp
// Date:  1/27/2015
// Auth:  K. Loux
// Desc:  Comparison class for specializing std::maps on wxArrayString.

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/utilities/arrayStringCompare.h"

namespace LibPlot2D
{

bool ArrayStringCompare::operator()(const wxArrayString &a1, const wxArrayString &a2) const
{
	return GenerateSingleString(a1) < GenerateSingleString(a2);
}

wxString ArrayStringCompare::GenerateSingleString(const wxArrayString &a) const
{
	wxString s;
	for (const auto& v : a)
		s += v;

	return s;
}

}// namespace LibPlot2D
