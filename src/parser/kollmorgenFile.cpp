/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  kollmorgenFile.cpp
// Date:  10/4/2012
// Auth:  K. Loux
// Desc:  File class for Kollmorgen files.

// Local headers
#include "lp2d/parser/kollmorgenFile.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			KollmorgenFile
// Function:		IsType
//
// Description:		Method for determining if the specified file is this
//					type of file.
//
// Input Arguments:
//		testFile	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if this object can process the specified file, false otherwise
//
//=============================================================================
bool KollmorgenFile::IsType(const wxString &testFile)
{
	std::ifstream file(testFile.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + testFile + _T("'!"),
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
	if (wxString(nextLine).Trim().Mid(0, 7).Cmp(_T("MMI vom")) == 0 ||// S600 drives
		wxString(nextLine).Trim().Mid(0, 10).Cmp(_T("DRIVE S300")) == 0)// S300 and S700 drives
		return true;

	return false;
}

//=============================================================================
// Class:			KollmorgenFile
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
//		nonNumericColumns	= wxArrayInt&
//
// Return Value:
//		wxArrayString containing the descriptions
//
//=============================================================================
wxArrayString KollmorgenFile::GetCurveInformation(unsigned int& headerLineCount,
	std::vector<double> &factors, wxArrayInt &/*nonNumericColumns*/) const
{
	std::ifstream file(mFileName.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + mFileName + _T("'!"),
			_T("Error Reading File"), wxICON_ERROR);
		return mDescriptions;
	}

	SkipLines(file, 3);
	headerLineCount = 4;

	std::string nextLine;
	std::getline(file, nextLine);// The fourth line contains the data set labels
	wxArrayString names = ParseLineIntoColumns(nextLine, mDelimiter);
	names.Insert(_T("Time [sec]"), 0);

	factors.resize(names.size(), 1.0);

	file.close();
	return names;
}

//=============================================================================
// Class:			KollmorgenFile
// Function:		DoTypeSpecificLoadTasks
//
// Description:		Finds and stores the sample rate for the file.
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
//=============================================================================
void KollmorgenFile::DoTypeSpecificLoadTasks()
{
	std::ifstream file(mFileName.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not determine sample rate!  Using 1 Hz."),
			_T("Error Reading File"), wxICON_ERROR);
		mTimeStep = 1.0;
		return;
	}

	SkipLines(file, 2);

	std::string nextLine;
	std::getline(file, nextLine);

	// The third line contains the number of data points and the sampling period in msec
	// We use this information to generate the time series (file does not contain a time series)
	mTimeStep = atof(nextLine.substr(nextLine.find_first_of(mDelimiter) + 1).c_str()) / 1000.0;// [sec]

	file.close();
}

//=============================================================================
// Class:			KollmorgenFile
// Function:		ExtractData
//
// Description:		Parses the file and reads data into vectors.  Only extracts
//					the data the user selected for display.  Also creates time
//					series based on timeStep.
//
// Input Arguments:
//		file	= std::ifstream& previously opened input stream to read from
//		choices	= const wxArrayInt& indicating the user's choices
//		factors	= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData	= std::vector<std::vector<double>>& containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool KollmorgenFile::ExtractData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<std::vector<double>>& rawData, std::vector<double> &factors, wxString& errorString) const
{
	std::string nextLine;
	wxArrayString parsed;
	unsigned int i, curveCount(choices.size() + 1);
	unsigned int lineNumber(mHeaderLines);
	double tempDouble, time(0.0);

	while (!file.eof())
	{
		++lineNumber;
		std::getline(file, nextLine);
		parsed = ParseLineIntoColumns(nextLine, mDelimiter);
		parsed.Insert(wxString::Format("%f", time), 0);
		if (parsed.size() < curveCount)
		{
			if (!file.eof())
				wxMessageBox(_T("Terminating data extraction prior to reaching end-of-file."),
					_T("Column Count Mismatch"), wxICON_WARNING);
			return true;
		}

		unsigned int set(0);
		for (i = 0; i < parsed.size(); ++i)
		{
			if (!parsed[i].ToDouble(&tempDouble))
			{
				errorString.Printf("Failed to convert entry at row %i, column %i, to a number.",
					lineNumber, i + 1);
				return false;
			}
			if (i == 0 || ArrayContainsValue(i, choices))// Always take the time column
			{
				rawData[set].push_back(tempDouble);
				factors[set] = factors[i];// Update scales for cases where user didn't select a column
				++set;
			}
		}
		time += mTimeStep;
	}
	return true;
}

}// namespace LibPlot2D
