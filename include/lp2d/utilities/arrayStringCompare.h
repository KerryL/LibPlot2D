/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  mainFrame.cpp
// Date:  1/27/2015
// Auth:  K. Loux
// Desc:  Comparison class for specializing std::maps on wxArrayString.

#ifndef ARRAY_STRING_COMPARE_H_
#define ARRAY_STRING_COMPARE_H_

// wxWidgets forward declarations
class wxArrayString;
class wxString;

namespace LibPlot2D
{

class ArrayStringCompare
{
public:
	bool operator()(const wxArrayString &a1, const wxArrayString &a2) const;

private:
	wxString GenerateSingleString(const wxArrayString &a) const;
};

}// namespace LibPlot2D

#endif// ARRAY_STRING_COMPARE_H_
