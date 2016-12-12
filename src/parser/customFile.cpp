/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  customFile.cpp
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for custom (non-XML) files defined by the user using
//				 an XML file.

// Local headers
#include "lp2d/parser/customFile.h"
#include "lp2d/utilities/dataset2D.h"

namespace LibPlot2D
{

//==========================================================================
// Class:			CustomFile
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
bool CustomFile::IsType(const wxString &fileName)
{
	CustomFileFormat format(fileName);
	return format.IsCustomFormat() && !format.IsXML();
}

//==========================================================================
// Class:			CustomFile
// Function:		CreateDelimiterList
//
// Description:		Overload.  Creates the delimiter list.  If no delimiter
//					is specified in the custom definition, this is the same
//					as DataFile, otherwise we limit the delimiter list to
//					contain only the delimiter specified.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString
//
//==========================================================================
wxArrayString CustomFile::CreateDelimiterList() const
{
	if (fileFormat.GetDelimiter().IsEmpty())
		return DataFile::CreateDelimiterList();

	wxArrayString delimiterList;
	delimiterList.Add(fileFormat.GetDelimiter());
	return delimiterList;
}

//==========================================================================
// Class:			CustomFile
// Function:		GetRawDataSize
//
// Description:		Returns the size to use when allocating the raw data array.
//
// Input Arguments:
//		selectedCount	= const unsigned int& indicating number of curves the
//						  user selected to display
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int CustomFile::GetRawDataSize(const unsigned int &selectedCount) const
{
	if (fileFormat.IsAsynchronous())
		return selectedCount * 2;

	return DataFile::GetRawDataSize(selectedCount);
}

//==========================================================================
// Class:			CustomFile
// Function:		ExtractData
//
// Description:		Parses the file and reads data into vectors.  Only extracts
//					the data the user selected for display.  Handles asynchronous
//					and time-formatted data as special cases.
//
// Input Arguments:
//		file		= std::ifstream& previously opened input stream to read from
//		choices		= const wxArrayInt& indicating the user's choices
//		factors		= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData		= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CustomFile::ExtractData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors,
	wxString &errorString) const
{
	if (fileFormat.IsAsynchronous() || !fileFormat.GetTimeFormat().IsEmpty())
		return ExtractSpecialData(file, choices, rawData, factors, errorString);

	return DataFile::ExtractData(file, choices, rawData, factors, errorString);
}

//==========================================================================
// Class:			CustomFile
// Function:		AssembleDatasets
//
// Description:		Takes raw data in std::vector<double> format and populates
//					this object's data member with it.  If the format is
//					asynchronous, it handles that as a special case.
//
// Input Arguments:
//		rawData		= const std::vector<double>*
//		dataSize	= const unsigned int& indicating the number of elements of the first argument
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CustomFile::AssembleDatasets(const std::vector<double> *rawData,
	const unsigned int &dataSize)
{
	if (fileFormat.IsAsynchronous())
		AssembleAsynchronousDatasets(rawData, dataSize);
	else
		DataFile::AssembleDatasets(rawData, dataSize);
}

//==========================================================================
// Class:			CustomFile
// Function:		GetCurveInformation
//
// Description:		Parses the file and assembles descriptions for each column
//					based on the contents of the header rows.  Also reports
//					the number of header rows back to the calling function.
//					If custom descriptions and scaling factors are specified,
//					these are used in place of the defaults.
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
//==========================================================================
wxArrayString CustomFile::GetCurveInformation(unsigned int &headerLineCount,
	std::vector<double> &factors, wxArrayInt &/*nonNumericColumns*/) const
{
	wxArrayInt nonNumericColumns;// TODO:  Use this
	wxArrayString names = DataFile::GetCurveInformation(headerLineCount, factors, nonNumericColumns);
	fileFormat.ProcessChannels(names, factors);

	if (!fileFormat.GetTimeUnits().IsEmpty())
		names[0] = _T("Time, [") + fileFormat.GetTimeUnits() + _T("]");

	return names;
}

//==========================================================================
// Class:			CustomFile
// Function:		ExtractSpecialData
//
// Description:		Parses the file and reads data into vectors.  Only extracts
//					the data the user selected for display.  This handles the
//					special cases of asynchronous and time-formatted data.
//
// Input Arguments:
//		file		= std::ifstream& previously opened input stream to read from
//		choices		= const wxArrayInt& indicating the user's choices
//		factors		= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData		= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CustomFile::ExtractSpecialData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors, wxString &errorString) const
{
	std::string nextLine;
	wxArrayString parsed;
	unsigned int curveCount(choices.size() + 1);
	unsigned int lineNumber(headerLines);
	double timeZero(-1.0);

	while (std::getline(file, nextLine))
	{
		lineNumber++;
		parsed = ParseLineIntoColumns(nextLine, delimiter);
		if (parsed.size() < curveCount && parsed.size() > 0)
		{
			if (!file.eof() &&
				(fileFormat.GetEndIdentifier().IsEmpty() ||
				(!fileFormat.GetEndIdentifier().IsEmpty() &&
				parsed[0].Cmp(fileFormat.GetEndIdentifier()) != 0)))
				wxMessageBox(_T("Terminating data extraction prior to reaching end-of-file."),
					_T("Column Count Mismatch"), wxICON_WARNING);
			return true;
		}

		if (fileFormat.IsAsynchronous())
		{
			if (!ExtractAsynchronousData(timeZero, parsed, rawData, factors, choices, errorString))
			{
				errorString.Append(wxString::Format(" at line %i", lineNumber));
				return false;
			}
		}
		else
		{
			if (!ExtractSynchronousData(timeZero, parsed, rawData, factors, choices, errorString))
			{
				errorString.Append(wxString::Format(" at line %i", lineNumber));
				return false;
			}
		}
	}

	return true;
}

//==========================================================================
// Class:			CustomFile
// Function:		ExtractAsynchronousData
//
// Description:		Extracts data from asynchronous data line.
//
// Input Arguments:
//		timeZero	= double&
//		parsedLine	= const wxArrayString&
//		factors		= std::vector<double>&
//		choices		= const wxArrayInt&
//
// Output Arguments:
//		rawData		= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CustomFile::ExtractAsynchronousData(double &timeZero, const wxArrayString &parsedLine,
	std::vector<double> *rawData, std::vector<double> &factors, const wxArrayInt &choices,
	wxString& WXUNUSED(errorString)) const
{
	double time, value;
	unsigned int set(0);
	if (!fileFormat.GetTimeFormat().IsEmpty())
	{
		time = GetTimeValue(parsedLine[0], fileFormat.GetTimeFormat(), fileFormat.GetTimeUnits());
		if (timeZero < 0.0)
			timeZero = time;
	}
	else
	{
		parsedLine[0].ToDouble(&time);
		timeZero = 0.0;
	}

	unsigned int i;
	for (i = 1; i < parsedLine.size(); i++)
	{
		if (!ArrayContainsValue(i - 1, choices))
			continue;

		if (!parsedLine[i].ToDouble(&value))
		{
			set++;
			continue;
		}
		rawData[set * 2].push_back((time - timeZero) * factors[0]);
		rawData[set * 2 + 1].push_back(value * factors[i]);
		set++;
	}

	return true;
}

//==========================================================================
// Class:			CustomFile
// Function:		ExtractSynchronousData
//
// Description:		Extracts data from synchronous data line.
//
// Input Arguments:
//		timeZero	= double&
//		parsedLine	= const wxArrayString&
//		factors		= std::vector<double>&
//		choices		= const wxArrayInt&
//
// Output Arguments:
//		rawData		= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CustomFile::ExtractSynchronousData(double &timeZero, const wxArrayString &parsedLine,
	std::vector<double> *rawData, std::vector<double> &factors, const wxArrayInt &choices,
	wxString &errorString) const
{
	double time, value;
	unsigned int i, set(0);
	for (i = 0; i < parsedLine.size(); i++)
	{
		if (i == 0 && !fileFormat.GetTimeFormat().IsEmpty())
		{
			time = GetTimeValue(parsedLine[i], fileFormat.GetTimeFormat(), fileFormat.GetTimeUnits());
			if (timeZero < 0.0)
				timeZero = time;
			value = time - timeZero;
		}
		else if (!parsedLine[i].ToDouble(&value))
		{
			errorString = _T("Failed to convert string to number");
			return false;
		}

		if (i == 0 || ArrayContainsValue(i - 1, choices))
		{
			rawData[set].push_back(value * factors[i]);
			set++;
		}
	}

	return true;
}

//==========================================================================
// Class:			CustomFile
// Function:		AssembleAsynchronousDatasets
//
// Description:		Takes raw data in std::vector<double> format and populates
//					this object's data member with it.  This handles the special
//					case of asynchronous data.
//
// Input Arguments:
//		rawData		= const std::vector<double>*
//		dataSize	= const unsigned int& indicating the number of elements of the first argument
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CustomFile::AssembleAsynchronousDatasets(const std::vector<double> *rawData,
	const unsigned int &dataSize)
{
	assert(dataSize > 1 && rawData && dataSize % 2 == 0);

	Dataset2D *dataset;
	unsigned int i;
	for (i = 0; i < dataSize; i += 2)
	{
		dataset = new Dataset2D(rawData[i].size());
		TransferVectorToArray(rawData[i], dataset->GetXPointer());
		TransferVectorToArray(rawData[i + 1], dataset->GetYPointer());
		*dataset *= scales[i / 2];
		data.push_back(dataset);
	}
}

//==========================================================================
// Class:			CustomFile
// Function:		DoTypeSpecificLoadTasks
//
// Description:		Sets non-standard flags for this class.
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
void CustomFile::DoTypeSpecificLoadTasks()
{
	ignoreConsecutiveDelimiters = !fileFormat.IsAsynchronous();
	timeIsFormatted = !fileFormat.GetTimeFormat().IsEmpty();
}

}// namespace LibPlot2D
