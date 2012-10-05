/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  baumullerFile.cpp
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for Baumuller files.
// History:

// Local headers
#include "application/dataFiles/baumullerFile.h"

//==========================================================================
// Class:			BaumullerFile
// Function:		IsType
//
// Description:		Method for determining if the specified file is this
//					type of file.
//
// Input Arguments:
//		_fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if this object can process the specified file, false otherwise
//
//==========================================================================
bool BaumullerFile::IsType(const wxString &_fileName)
{
	std::ifstream file(_fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + _fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR);
		return false;
	}

	std::string nextLine;
	std::getline(file, nextLine);// Read first line
	file.close();

	// Wrap in wxString for robustness against varying line endings
	if (wxString(nextLine).Trim().Cmp(_T("WinBASS_II_Oscilloscope_Data")) == 0)
		return true;

	// FIXME:  This can be handled with custom format: just needs time units specified

	return false;
}

//==========================================================================
// Class:			BaumullerFile
// Function:		GetDescriptions
//
// Description:		Parses the file and assembles descriptions for each column
//					based on the contents of the header rows.  Also reports
//					the number of header rows back to the calling function.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		headerLineCount	= unsigned int& number of lines making up this file's header
//		factors			= std::vector<double>& containing scaling factors
//
// Return Value:
//		wxArrayString containing the descriptions
//
//==========================================================================
wxArrayString BaumullerFile::GetCurveInformation(unsigned int &headerLineCount,
	std::vector<double> &factors) const
{
	std::ifstream file(fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR);
		return descriptions;
	}

	std::string nextLine;
	wxArrayString delimitedLine, previousLines, names;
	while (std::getline(file, nextLine))
	{
		delimitedLine = ParseLineIntoColumns(nextLine, delimiter);
		if (delimitedLine.size() > 1)
		{
			if (ListIsNumeric(delimitedLine))// If not all columns are numeric, this isn't a data row
			{
				names = GenerateNames(previousLines, delimitedLine);
				headerLineCount = previousLines.size();
				if (names.size() == 0)
					names = GenerateDummyNames(delimitedLine.size());
				factors.resize(names.size(), 1.0);
				names[0] = _T("Time, [msec]");
				return names;
			}
		}
		previousLines.Add(nextLine);
	}

	names.Empty();
	return names;
}