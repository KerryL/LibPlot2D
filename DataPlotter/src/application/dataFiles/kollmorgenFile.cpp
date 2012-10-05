/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kollmorgenFile.cpp
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for Kollmorgen files.
// History:

// Local headers
#include "application/dataFiles/kollmorgenFile.h"

//==========================================================================
// Class:			KollmorgenFile
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
bool KollmorgenFile::IsType(const wxString &_fileName)
{
	std::ifstream file(_fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + _fileName + _T("'!"),
			_T("Error Reading File"), wxICON_ERROR);
		return false;
	}

	SkipLines(file, 1);

	std::string nextLine;
	std::getline(file, nextLine);// Read second line
	file.close();

	// Kollmorgen format from S600 series drives
	// There may be a better way to check this, but I haven't found it
	// Wrap in wxString for robustness against varying line endings
	if (wxString(nextLine).Trim().Mid(0, 7).Cmp(_T("MMI vom")) == 0)
		return true;

	return false;
}

//==========================================================================
// Class:			KollmorgenFile
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
wxArrayString KollmorgenFile::GetCurveInformation(unsigned int &headerLineCount,
	std::vector<double> &factors) const
{
	/*SkipLines(file, 2);

	std::string nextLine;
	std::getline(file, nextLine);

	// The third line contains the number of data points and the sampling period in msec
	// We use this information to generate the time series (file does not contain a time series)
	samplingPeriod = atof(nextLine.substr(nextLine.find_first_of(delimiter) + 1).c_str()) / 1000.0;// [sec]

	// The fourth line contains the data set labels (which also gives us the number of datasets we need)
	std::getline(file, nextLine);
	wxArrayString names = ParseLineIntoColumns(nextLine, delimiter);

	return names;*/
	wxArrayString a;
	return a;
}