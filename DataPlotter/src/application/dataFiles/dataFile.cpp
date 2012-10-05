/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataFile.cpp
// Created:  10/2/2012
// Author:  K. Loux
// Description:  Base class for data file classes.
// History:

// Local headers
#include "application/dataFiles/dataFile.h"
#include "application/multiChoiceDialog.h"
#include "application/mainFrame.h"

//==========================================================================
// Class:			DataFile
// Function:		DataFile
//
// Description:		Constructor for DataFile class.  Assigns arguments and
//					default values.
//
// Input Arguments:
//		fileName	= const wxString& file which this object represents
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DataFile::DataFile(const wxString& _fileName)
{
	fileName = _fileName;
	headerLines = 0;
}

//==========================================================================
// Class:			DataFile
// Function:		~DataFile
//
// Description:		Destructor for DataFile class.
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
DataFile::~DataFile()
{
}

//==========================================================================
// Class:			DataFile
// Function:		Load
//
// Description:		Performs the actions necessary to load the file contents
//					into datasets.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool DataFile::Load(void)
{
	delimiter = DetermineBestDelimiter();
	if (delimiter.IsEmpty())
	{
		wxMessageBox(_T("Could not find an appropriate delimiter."), _T("Error Parsing File"), wxICON_ERROR);
		return false;
	}

	descriptions = GetCurveInformation(headerLines, scales);
	if (descriptions.size() < 2)
	{
		wxMessageBox(_T("No plottable data found in file!"), _T("Error Generating Plot"), wxICON_ERROR);
		return false;
	}

	if (!ProcessFile())
		return false;

	return true;
}

//==========================================================================
// Class:			DataFile
// Function:		DetermineBestDelimiter
//
// Description:		Tries each delimiter in the list until finding the best
//					one for this file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString indicating the delimiter to use; wxEmptyString on failure
//
//==========================================================================
wxString DataFile::DetermineBestDelimiter(void) const
{
	std::string nextLine;
	wxArrayString delimitedLine, delimiterList(CreateDelimiterList());
	unsigned int i, columnCount(0);

	if (delimiterList.size() == 1)
		return delimiterList[0];

	std::ifstream file(fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR);
		return wxEmptyString;
	}

	while (std::getline(file, nextLine))
	{
		for (i = 0; i < delimiterList.size(); i++)// Try all delimiters until we find one that works
		{
			delimitedLine = ParseLineIntoColumns(nextLine, delimiterList[i]);
			if (delimitedLine.size() > 1)
			{
				if (ListIsNumeric(delimitedLine) && columnCount == delimitedLine.size())// Number of number columns == number of text columns
					return delimiterList[i];
				else
					columnCount = delimitedLine.size();
			}
		}
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			DataFile
// Function:		CreateDelimiterList
//
// Description:		Populates the list of delimiters to try when parsing this file.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing the list
//
//==========================================================================
wxArrayString DataFile::CreateDelimiterList(void) const
{
	// Don't use periods ('.') because we're going to have those in regular numbers (switch this for other languages?)
	wxArrayString delimiterList;
	delimiterList.Add(_T(" "));
	delimiterList.Add(_T(","));
	delimiterList.Add(_T("\t"));
	delimiterList.Add(_T(";"));

	return delimiterList;
}

//==========================================================================
// Class:			DataFile
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
wxArrayString DataFile::GetCurveInformation(unsigned int &headerLineCount,
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
		delimitedLine = ParseLineIntoColumns(nextLine, delimiter);// FIXME:  Needs to know how to handle consecutive delimiters
		if (delimitedLine.size() > 1)
		{
			// FIXME:  Needs to know if we're allowing zero length and formatted time!
			if (ListIsNumeric(delimitedLine))// If not all columns are numeric, this isn't a data row
			{
				names = GenerateNames(previousLines, delimitedLine);
				headerLineCount = previousLines.size();
				if (names.size() == 0)
					names = GenerateDummyNames(delimitedLine.size());
				factors.resize(names.size(), 1.0);
				return names;
			}
		}
		previousLines.Add(nextLine);
	}

	names.Empty();
	return names;
}

//==========================================================================
// Class:			DataFile
// Function:		ParseLineIntoColumns
//
// Description:		Parses the specified line into pieces based on encountering
//					the specified delimiting character (or characters).
//
// Input Arguments:
//		line		= wxString containing the line to parse
//		delimiter	= const wxString& specifying the characters to assume for
//					  delimiting columns
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing one item for every column contained in the
//		original line
//
//==========================================================================
wxArrayString DataFile::ParseLineIntoColumns(wxString line,
	const wxString &delimiter, const bool &ignoreConsecutiveDelimiters) const
{
	// Remove \r character from end of line (required for GTK, etc.)
	line.Trim();

	wxArrayString parsed;

	size_t start(0);
	size_t end(0);

	while (end != std::string::npos && start < line.length())
	{
		// Find the next delimiting character
		end = line.find(delimiter.c_str(), start);

		// If the next delimiting character is right next to the previous character
		// (empty string between), ignore it (that is to say, we treat consecutive
		// delimiters as one)
		// Changed 4/29/2012 - For some Baumuller data, there are no units, which
		// results in consecutive delimiters that should NOT be treated as one
		if (end == start && ignoreConsecutiveDelimiters)
		{
			start++;
			continue;
		}

		// If no character was found, add the rest of the line to the list
		if (end == std::string::npos)
			parsed.Add(line.substr(start));
		else
			parsed.Add(line.substr(start, end - start));

		start = end + 1;
	}

	if (!ignoreConsecutiveDelimiters && line.Last() == delimiter)
		parsed.Add(wxEmptyString);

	return parsed;
}

//==========================================================================
// Class:			DataFile
// Function:		GenerateNames
//
// Description:		Creates the first part of the plot name for generic files.
//
// Input Arguments:
//		previousLines	= const wxArrayString&
//		currentLine		= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString
//
//==========================================================================
wxArrayString DataFile::GenerateNames(const wxArrayString &previousLines,
	const wxArrayString &currentLine) const
{
	unsigned int i;
	int line;
	wxArrayString delimitedPreviousLine, names;
	double value;
	for (line = previousLines.size() - 1; line >= 0; line--)
	{
		delimitedPreviousLine = ParseLineIntoColumns(previousLines[line].c_str(), delimiter);
		if (delimitedPreviousLine.size() != currentLine.size())
			break;

		bool prependText(true);
		for (i = 0; i < delimitedPreviousLine.size(); i++)
		{
			prependText = !delimitedPreviousLine[i].ToDouble(&value);
			if (!prependText)
				break;
		}

		if (prependText)
		{
			for (i = 0; i < delimitedPreviousLine.size(); i++)
			{
				if (names.size() < i + 1)
					names.Add(delimitedPreviousLine[i]);
				else
					names[i].Prepend(delimitedPreviousLine[i] + _T(", "));
			}
		}
	}

	return names;
}

//==========================================================================
// Class:			DataFile
// Function:		GenerateDummyNames
//
// Description:		Generates plot names for cases where no information was
//					provided by the data file.
//
// Input Arguments:
//		count	= const unsigned int& specifying the number of names to generate
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString containing dummy names
//
//==========================================================================
wxArrayString DataFile::GenerateDummyNames(const unsigned int &count) const
{
	unsigned int i;
	wxArrayString names;
	for (i = 0; i < count; i++)
		names.Add(wxString::Format("[%i]", i));

	return names;
}

//==========================================================================
// Class:			DataFile
// Function:		ProcessFile
//
// Description:		Performs actions from asking the user which data columns
//					to extract through extracting the data and putting it in
//					Dataset2D format.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool DataFile::ProcessFile(void)
{
	MultiChoiceDialog dialog(NULL, _T("Select data to plot:"), _T("Select Data"),
		wxArrayString(descriptions.begin() + 1, descriptions.end()));
	if (dialog.ShowModal() == wxID_CANCEL)
		return false;

	wxArrayInt choices = dialog.GetSelections();
	if (choices.size() == 0)
	{
		wxMessageBox(_T("No data selected for plotting!"), _T("Error Generating Plot"), wxICON_ERROR);
		return false;
	}

	std::ifstream file(fileName.c_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR);
		return false;
	}
	SkipLines(file, headerLines);

	std::vector<double> *rawData = new std::vector<double>[choices.size() + 1];// +1 for time column, which isn't displayed for user to select
	if (!ExtractData(file, choices, rawData, scales))
	{
		wxMessageBox(_T("Error during data extraction."), _T("Error Reading File"), wxICON_ERROR);
		return false;
	}
	file.close();

	AssembleDatasets(rawData, choices.size() + 1);
	delete [] rawData;

	return true;
}

//==========================================================================
// Class:			DataFile
// Function:		ExtractData
//
// Description:		Parses the file and reads data into vectors.  Only extracts
//					the data the user selected for display.
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
bool DataFile::ExtractData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors) const
{
	std::string nextLine;
	wxArrayString parsed;
	unsigned int i, set, curveCount(choices.size() + 1);
	double tempDouble;

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
		for (i = 0; i < parsed.size(); i++)
		{
			if (!parsed[i].ToDouble(&tempDouble))
				return false;

			if (i == 0 || ArrayContainsValue(i - 1, choices))// Always take the time column; +1 due to time column not included in choices
			{
				rawData[set].push_back(tempDouble);
				factors[set] = factors[i];// Update scales for cases where user didn't select a column
				set++;
			}
		}
	}

	return true;
}

//==========================================================================
// Class:			DataFile
// Function:		ArrayContainsValue
//
// Description:		Determines whether or not the specified value is contained in
//					the specified array.
//
// Input Arguments:
//		value	= const int&
//		a		= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for value found, false otherwise
//
//==========================================================================
bool DataFile::ArrayContainsValue(const int &value, const wxArrayInt &a) const
{
	unsigned int i;
	for (i = 0; i < a.size(); i++)
	{
		if (a[i] == value)
			return true;
	}

	return false;
}

//==========================================================================
// Class:			DataFile
// Function:		AssembleDatasets
//
// Description:		Takes raw data in std::vector<double> format and populates
//					this object's data member with it.
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
void DataFile::AssembleDatasets(const std::vector<double> *rawData, const unsigned int &dataSize)
{
	assert(dataSize > 1 && rawData);

	Dataset2D *dataset;
	unsigned int i;
	for (i = 1; i < dataSize; i++)
	{
		if (i == 1)
		{
			dataset = new Dataset2D(rawData[0].size());
			TransferVectorToArray(rawData[0], dataset->GetXPointer());
		}
		else
			dataset = new Dataset2D(*data[0]);

		TransferVectorToArray(rawData[i], dataset->GetYPointer());
		*dataset *= scales[i];
		data.push_back(dataset);
	}
}

//==========================================================================
// Class:			DataFile
// Function:		TransferVectorToArray
//
// Description:		Takes data in a std::vector<double> and moves it into a
//					(previously allocated) double array.
//
// Input Arguments:
//		source		= const std::vector<double>
//		destination	= double*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DataFile::TransferVectorToArray(const std::vector<double> &source,
	double *destination) const
{
	unsigned int i;
	for (i = 0; i < source.size(); i++)
		destination[i] = source[i];
}

//==========================================================================
// Class:			DataFile
// Function:		SkipLines
//
// Description:		Reads and discards the specified number of lines from
//					the file.
//
// Input Arguments:
//		file	= std::ifstream&
//		count	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void DataFile::SkipLines(std::ifstream &file, const unsigned int &count)
{
	std::string nextLine;
	unsigned int i;
	for (i = 0; i < count; i++)
		std::getline(file, nextLine);
}

//==========================================================================
// Class:			DataFile
// Function:		ListIsNumeric
//
// Description:		Checks to see if the input array contains only numeric values.
//
// Input Arguments:
//		list				= const wxArrayString&
//		timeIsFormatted		= const bool& indicates time column should be treated as
//							  numeric, even when it's not
//		allowEmptyValues	= const bool&, indicating that consecutive delimiters
//							  should not be discarded
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if all values are numeric, false otherwise
//
//==========================================================================
bool DataFile::ListIsNumeric(const wxArrayString &list, const bool &timeIsFormatted,
	const bool &allowEmptyValues) const
{
	unsigned int j;
	double value;
	for (j = (unsigned int)timeIsFormatted; j < list.size(); j++)
	{
		if (!list[j].ToDouble(&value) && !(allowEmptyValues && list[j].IsEmpty()))
			return false;
	}

	return true;
}

//==========================================================================
// Class:			DataFile
// Function:		GetTimeValue
//
// Description:		Converts the time string with the given format into the
//					specified units.  Converts to seconds first, then applies
//					a scaling factor.
//
// Input Arguments:
//		timeString	= const wxString&
//		timeFormat	= const wxString& (i.e. something like hh:mm:ss)
//		timeUnits	= const wxString& (i.e. something like msec or hours)
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double DataFile::GetTimeValue(const wxString &timeString,
	const wxString &timeFormat, const wxString &timeUnits) const
{
	double factor;
	if (!MainFrame::UnitStringToFactor(timeUnits, factor))
		factor = 1.0;

	// Recognize (case insensitive) H as hour, m as minute, s as second and x as millisecond
	// Assume all the same formatting between delimiters
	wxChar delimiter(':');
	unsigned int formatStart(0), formatCount(0), timeStart(0), timeCount(0);
	double time(0.0), value;
	while (true)
	{
		formatCount = timeFormat.Mid(formatStart).Find(delimiter);
		timeCount = timeString.Mid(timeStart).Find(delimiter);
		if (!timeString.Mid(timeStart, timeCount).ToDouble(&value))
		{
			// FIXME:  What to do in this case?
			value = 0.0;
		}

		if (timeFormat.Mid(formatStart, 1).CmpNoCase(_T("H")) == 0)// Hour
			value *= 3600.0;
		else if (timeFormat.Mid(formatStart, 1).CmpNoCase(_T("M")) == 0)// Minute
			value *= 60.0;
		else if (timeFormat.Mid(formatStart, 1).CmpNoCase(_T("S")) == 0) {}// Second (Do nothing)
		else if (timeFormat.Mid(formatStart, 1).CmpNoCase(_T("X")) == 0)// Millisecond
			value /= 1000.0;
		else
		{
			// FIMXE:  Tell the user we don't understand their format?
		}

		// TODO:  Handle rollovers (i.e. going from 23:59:59 to 00:00:00)

		time += value;

		if (formatCount == wxNOT_FOUND || timeCount == wxNOT_FOUND)
			break;

		formatStart += formatCount + 1;
		timeStart += timeCount + 1;
	}

	return time * factor;
}

//==========================================================================
// Class:			DataFile
// Function:		GetPopulatedCount
//
// Description:		Determines the number of non-empty members of the array.
//
// Input Arguments:
//		list	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
/*unsigned int DataFile::GetPopulatedCount(const wxArrayString &list) const
{
	unsigned int count(0), i;
	for (i = 0; i < list.GetCount(); i++)
	{
		if (!list[i].IsEmpty())
			count ++;
	}

	return count;
}*/