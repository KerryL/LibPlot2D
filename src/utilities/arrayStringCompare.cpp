/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2015

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  mainFrame.cpp
// Created:  1/27/2015
// Author:  K. Loux
// Description:  Comparison class for specializing std::maps on wxArrayString.

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
	unsigned int i;
	for (i = 0; i < a.Count(); i++)
		s += a[i];

	return s;
}

}// namespace LibPlot2D
