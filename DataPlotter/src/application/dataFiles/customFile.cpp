/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  customFile.cpp
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for custom (non-XML) files defined by the user using
//				 an XML file.
// History:

// Local headers
#include "application/dataFiles/customFile.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			CustomFile
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
bool CustomFile::IsType(const wxString &_fileName)
{
	CustomFileFormat format(_fileName);
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
wxArrayString CustomFile::CreateDelimiterList(void) const
{
	if (fileFormat.GetDelimiter().IsEmpty())
		return DataFile::CreateDelimiterList();

	wxArrayString delimiterList;
	delimiterList.Add(fileFormat.GetDelimiter());
	return delimiterList;
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
//		file	= std::ifstream& previously opened input stream to read from
//		choices	= const wxArrayInt& indicating the user's choices
//		factors	= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData	= std::vector<double>* containing the data
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CustomFile::ExtractData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors) const
{
	if (fileFormat.IsAsynchronous() || !fileFormat.GetTimeFormat().IsEmpty())
		return ExtractSpecialData(file, choices, rawData, factors);

	return DataFile::ExtractData(file, choices, rawData, factors);
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
//
// Return Value:
//		wxArrayString containing the descriptions
//
//==========================================================================
wxArrayString CustomFile::GetCurveInformation(unsigned int &headerLineCount,
	std::vector<double> &factors) const
{
	wxArrayString names = DataFile::GetCurveInformation(headerLineCount, factors);
	fileFormat.ProcessChannels(names, factors);

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
//		file	= std::ifstream& previously opened input stream to read from
//		choices	= const wxArrayInt& indicating the user's choices
//		factors	= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData	= std::vector<double>* containing the data
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool CustomFile::ExtractSpecialData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors) const
{
	std::string nextLine;
	wxArrayString parsed;
	unsigned int i, set, curveCount(choices.size() + 1);
	double tempDouble, time, timeZero(-1.0);

	while (!file.eof())
	{
		std::getline(file, nextLine);
		parsed = ParseLineIntoColumns(nextLine, delimiter);
		if (parsed.size() < curveCount && parsed.size() > 0)
		{
			wxMessageBox(_T("Terminating data extraction prior to reaching end-of-file."),
				_T("Column Count Mismatch"), wxICON_WARNING);
			return true;
		}

		set = 0;
		if (fileFormat.IsAsynchronous())
		{
			if (!fileFormat.GetTimeFormat().IsEmpty())
			{
				time = GetTimeValue(parsed[0], fileFormat.GetTimeFormat(), fileFormat.GetTimeUnits());

				if (timeZero < 0.0)
					timeZero = time;
			}
			else
			{
				parsed[0].ToDouble(&time);
				timeZero = 0.0;
			}

			bool gotValue(false);
			for (i = 1; i < parsed.size(); i++)
			{
				if (!parsed[i].ToDouble(&tempDouble))
				{
					set++;
					continue;
				}
				gotValue = true;

				if (!descriptions[i].IsEmpty())
				{
					rawData[set * 2].push_back(time - timeZero);
					rawData[set * 2 + 1].push_back(tempDouble);
					set++;
				}
			}

			if (!gotValue)
				return false;
		}
		else
		{
			for (i = 0; i < parsed.size(); i++)
			{
				if (i == 0 && !fileFormat.GetTimeFormat().IsEmpty())
				{
					time = GetTimeValue(parsed[i], fileFormat.GetTimeFormat(), fileFormat.GetTimeUnits());
					if (timeZero < 0.0)
						timeZero = time;
					tempDouble = time - timeZero;
				}
				else if (!parsed[i].ToDouble(&tempDouble))
					return false;

				if (!descriptions[i].IsEmpty())
				{
					rawData[set].push_back(tempDouble);
					set++;
				}
			}
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
void CustomFile::DoTypeSpecificLoadTasks(void)
{
	ignoreConsecutiveDelimiters = !fileFormat.IsAsynchronous();
	timeIsFormatted = !fileFormat.GetTimeFormat().IsEmpty();
}