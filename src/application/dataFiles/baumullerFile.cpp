/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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
//		fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if this object can process the specified file, false otherwise
//
//==========================================================================
bool BaumullerFile::IsType(const wxString &fileName)
{
	std::ifstream file(fileName.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"),
			_T("Error Reading File"), wxICON_ERROR);
		return false;
	}

	std::string nextLine;
	std::getline(file, nextLine);// Read first line
	file.close();

	// Wrap in wxString for robustness against varying line endings
	if (wxString(nextLine).Trim().Cmp(_T("WinBASS_II_Oscilloscope_Data")) == 0)
		return true;

	return false;
}

//==========================================================================
// Class:			BaumullerFile
// Function:		GetCurveInformation
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
	std::ifstream file(fileName.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR);
		return descriptions;
	}

	std::string nextLine;
	wxArrayString previousLines, names;
	while (std::getline(file, nextLine))
	{
		if (ConstructNames(nextLine, file, names, previousLines))
		{
			file.close();
			factors.resize(names.size(), 1.0);
			names[0] = _T("Time, [msec]");
			headerLineCount = previousLines.size() + 5;// Extra two for min/max rows
			return names;
		}
	}

	file.close();

	names.Empty();
	return names;
}

//==========================================================================
// Class:			BaumullerFile
// Function:		ConstructNames
//
// Description:		Generates names when the appropriate part of the file is
//					reached.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		names			= wxArrayString&
//		previousLines	= wxArrayString&
//
// Return Value:
//		wxArrayString containing the descriptions
//
//==========================================================================
bool BaumullerFile::ConstructNames(std::string &nextLine, std::ifstream &file,
	wxArrayString &names, wxArrayString &previousLines) const
{
	wxArrayString delimitedLine = ParseLineIntoColumns(nextLine, delimiter);
	if (delimitedLine.size() > 1 && delimitedLine[0].Cmp(_T("Par.number:")) == 0)
	{
		unsigned int i, j;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < delimitedLine.Count() - 1; j++)
			{
				if (i == 0)
					names.Add(delimitedLine[j]);
				else
					names[j].Append(_T(", ") + delimitedLine[j]);
			}
			if (!std::getline(file, nextLine))
				break;
			delimitedLine = ParseLineIntoColumns(nextLine, delimiter);
		}

		return true;
	}
	previousLines.Add(nextLine);

	return false;
}

//==========================================================================
// Class:			BaumullerFile
// Function:		DoTypeSpecificLoadTasks
//
// Description:		Sets flags to be used during initial loading operations
//					(like building curve names).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void BaumullerFile::DoTypeSpecificLoadTasks(void)
{
	ignoreConsecutiveDelimiters = false;
}

//==========================================================================
// Class:			BaumullerFile
// Function:		DoTypeSpecificProcessTasks
//
// Description:		Sets flags to be used during data extraction operations.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void BaumullerFile::DoTypeSpecificProcessTasks(void)
{
	ignoreConsecutiveDelimiters = true;
}