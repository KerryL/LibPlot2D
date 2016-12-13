/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  dataFile.cpp
// Date:  10/2/2012
// Auth:  K. Loux
// Desc:  Base class for data file classes.

// Standard C++ headers
#include <locale>

// Local headers
#include "lp2d/parser/dataFile.h"
#include "lp2d/gui/multiChoiceDialog.h"
#include "lp2d/gui/guiInterface.h"
#include "lp2d/utilities/dataset2D.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			DataFile
// Function:		DataFile
//
// Desc:		Constructor for DataFile class.  Assigns arguments and
//					default values.
//
// Input Arguments:
//		fileName		= const wxString& file which this object represents
//		parent			= wxWindow* parent for centering curve choice dialog
//		selections		= wxArrayInt*, optional, default selections
//		removeExisting	= bool*, optional, default value of checkbox
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
DataFile::DataFile(const wxString& fileName) : fileName(fileName)
{
	headerLines = 0;

	ignoreConsecutiveDelimiters = true;
	timeIsFormatted = false;
}

//=============================================================================
// Class:			DataFile
// Function:		~DataFile
//
// Desc:		Destructor for DataFile class.
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
DataFile::~DataFile()
{
}

//=============================================================================
// Class:			DataFile
// Function:		Initialize
//
// Desc:		Initializes with type-specific class.
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
void DataFile::Initialize()
{
	delimiter = DetermineBestDelimiter();
	DoTypeSpecificLoadTasks();
	descriptions = GetCurveInformation(headerLines, scales, nonNumericColumns);
}

//=============================================================================
// Class:			DataFile
// Function:		GetSelectionsFromUser
//
// Desc:		Reads header info from the file and asks the user to
//					specify curves to plot.
//
// Input Arguments:
//		selectionInfo	= const SelectionData& indicating defaults
//
// Output Arguments:
//		selectionInfo	= const SelectionData& indicating user specified options
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
void DataFile::GetSelectionsFromUser(SelectionData &selectionInfo, wxWindow *parent)
{
	if (delimiter.IsEmpty())
	{
		wxMessageBox(_T("Could not find an appropriate delimiter."), _T("Error Parsing File"), wxICON_ERROR);
		return;
	}
	else if (descriptions.size() < 2)
	{
		wxMessageBox(_T("No plottable data found in file!"), _T("Error Generating Plot"), wxICON_ERROR);
		return;
	}

	MultiChoiceDialog dialog(parent, _T("Select data to plot:"), _T("Select Data"),
		wxArrayString(descriptions.begin() + 1, descriptions.end()), wxCHOICEDLG_STYLE,
		wxDefaultPosition, &selectionInfo.selections, &selectionInfo.removeExisting);
	if (dialog.ShowModal() == wxID_CANCEL)
	{
		selectionInfo.selections.Clear();
		return;
	}

	selectionInfo.selections = AdjustForSkippedColumns(dialog.GetSelections());
	if (selectionInfo.selections.Count() == 0)
	{
		wxMessageBox(_T("No data selected for plotting!"), _T("Error Generating Plot"), wxICON_ERROR);
		return;
	}
	selectionInfo.removeExisting = dialog.RemoveExistingCurves();
}

//=============================================================================
// Class:			DataFile
// Function:		AdjustForSkippedColumns
//
// Desc:		Adjusts the indices to account for columns that were not
//					displayed as allowable selections.  Index 0 is first data
//					column (not time column).
//
// Input Arguments:
//		selections	= const wxArrayInt&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayInt
//
//=============================================================================
wxArrayInt DataFile::AdjustForSkippedColumns(const wxArrayInt& selections) const
{
	wxArrayInt trueIndices;
	unsigned int i;
	for (i = 0; i < selections.size(); i++)
		trueIndices.Add(AdjustForSkippedColumns(selections[i]));

	assert(selections.size() == trueIndices.size());
	return trueIndices;
}

//=============================================================================
// Class:			DataFile
// Function:		AdjustForSkippedColumns
//
// Desc:		Adjusts the indices to account for columns that were not
//					displayed as allowable selections.  Index 0 is first data
//					column (not time column).
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int DataFile::AdjustForSkippedColumns(const unsigned int &i) const
{
	unsigned int j, adjustment(0);
	for (j = 0; j < nonNumericColumns.size(); j++)
	{
		if (nonNumericColumns[j] - 1 <= (int)i)
			adjustment++;
		else
			break;
	}

	return i + adjustment;
}

//=============================================================================
// Class:			DataFile
// Function:		Load
//
// Desc:		Performs the actions necessary to load the file contents
//					into datasets.
//
// Input Arguments:
//		selectionInfo	= const SelectionData&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool DataFile::Load(const SelectionData &selectionInfo)
{
	selectedDescriptions = RemoveUnwantedDescriptions(descriptions, selectionInfo.selections);

	std::ifstream file(fileName.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"), _T("Error Reading File"), wxICON_ERROR);
		return false;
	}
	SkipLines(file, headerLines);
	DoTypeSpecificProcessTasks();

	std::vector<double> *rawData = new std::vector<double>[GetRawDataSize(selectionInfo.selections.size())];
	wxString errorString;
	if (!ExtractData(file, selectionInfo.selections, rawData, scales, errorString))
	{
		file.close();
		delete [] rawData;
		wxMessageBox(_T("Error during data extraction:\n") + errorString,
			_T("Error Reading File"), wxICON_ERROR);
		return false;
	}
	file.close();

	AssembleDatasets(rawData, GetRawDataSize(selectionInfo.selections.size()));
	delete [] rawData;

	return true;
}

//=============================================================================
// Class:			DataFile
// Function:		DetermineBestDelimiter
//
// Desc:		Tries each delimiter in the list until finding the best
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
//=============================================================================
wxString DataFile::DetermineBestDelimiter() const
{
	std::string nextLine;
	wxArrayString delimitedLine, delimiterList(CreateDelimiterList());
	unsigned int i, columnCount(0);

	if (delimiterList.size() == 1)
		return delimiterList[0];
	std::ifstream file(fileName.mb_str(), std::ios::in);
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
				// TODO:  This check could be more robust (what if header rows contain numberic label?)
				if (IsDataRow(delimitedLine)
					&& columnCount == delimitedLine.size())// Number of number columns == number of text columns
				{
					file.close();
					return delimiterList[i];
				}
				else
					columnCount = delimitedLine.size();
			}
		}
	}

	file.close();
	return wxEmptyString;
}

//=============================================================================
// Class:			DataFile
// Function:		CreateDelimiterList
//
// Desc:		Populates the list of delimiters to try when parsing this file.
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
//=============================================================================
wxArrayString DataFile::CreateDelimiterList() const
{
	// Don't use decimals because we're going to have those in regular numbers
	std::locale usersLocale = std::locale("");// get user's preferred locale settings
	wxChar point = std::use_facet< std::numpunct<wxChar> >(usersLocale).decimal_point();

	wxArrayString delimiterList;
	if (point != ',')
		delimiterList.Add(_T(","));

	if (point != '.')
		delimiterList.Add(_T("."));

	delimiterList.Add(_T(" "));
	delimiterList.Add(_T("\t"));
	delimiterList.Add(_T(";"));

	return delimiterList;
}

//=============================================================================
// Class:			DataFile
// Function:		GetCurveInformation
//
// Desc:		Parses the file and assembles information for each column
//					based on the contents of the header rows.
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
//=============================================================================
wxArrayString DataFile::GetCurveInformation(unsigned int &headerLineCount,
	std::vector<double> &factors, wxArrayInt &nonNumericColumns) const
{
	std::ifstream file(fileName.mb_str(), std::ios::in);
	if (!file.is_open())
	{
		wxMessageBox(_T("Could not open file '") + fileName + _T("'!"),
			_T("Error Reading File"), wxICON_ERROR);
		return descriptions;
	}

	std::string nextLine;
	wxArrayString delimitedLine, previousLines, names;
	while (std::getline(file, nextLine))
	{
		delimitedLine = ParseLineIntoColumns(nextLine, delimiter);
		if (delimitedLine.size() > 1)
		{
			if (IsDataRow(delimitedLine))
			{
				names = GenerateNames(previousLines, delimitedLine, nonNumericColumns);
				headerLineCount = previousLines.size();
				if (names.size() == 0)
					names = GenerateDummyNames(delimitedLine, nonNumericColumns);
				factors.resize(names.size() + nonNumericColumns.size(), 1.0);
				file.close();
				return names;
			}
		}
		previousLines.Add(nextLine);
	}

	names.Empty();
	file.close();
	return names;
}

//=============================================================================
// Class:			DataFile
// Function:		ParseLineIntoColumns
//
// Desc:		Parses the specified line into pieces based on encountering
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
//=============================================================================
wxArrayString DataFile::ParseLineIntoColumns(wxString line,
	const wxString &delimiter) const
{
	line.Trim();// Remove \r character from end of line (required for GTK, etc.)

	wxArrayString parsed;

	size_t start(0);
	size_t end(0);

	while (end != std::string::npos && start < line.length())
	{
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

//=============================================================================
// Class:			DataFile
// Function:		GenerateNames
//
// Desc:		Creates the first part of the plot name for generic files.
//
// Input Arguments:
//		previousLines	= const wxArrayString&
//		currentLine		= const wxArrayString&
//
// Output Arguments:
//		nonNumericColumns	= wxArrayInt&
//
// Return Value:
//		wxArrayString
//
//=============================================================================
wxArrayString DataFile::GenerateNames(const wxArrayString &previousLines,
	const wxArrayString &currentLine, wxArrayInt &nonNumericColumns) const
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
				if (!currentLine[i].ToDouble(&value))
				{
					nonNumericColumns.Add(i);
					continue;
				}

				if (names.size() < i + 1 - nonNumericColumns.size())
					names.Add(delimitedPreviousLine[i]);
				else
					names[i - nonNumericColumns.size()].Prepend(delimitedPreviousLine[i] + _T(", "));
			}
		}
	}

	return names;
}

//=============================================================================
// Class:			DataFile
// Function:		GenerateDummyNames
//
// Desc:		Generates plot names for cases where no information was
//					provided by the data file.
//
// Input Arguments:
//		currentLine			= const wxArrayString&
//
// Output Arguments:
//		nonNumericColumns	= wxArrayInt&
//
// Return Value:
//		wxArrayString containing dummy names
//
//=============================================================================
wxArrayString DataFile::GenerateDummyNames(const wxArrayString &currentLine,
	wxArrayInt &nonNumericColumns) const
{
	unsigned int i;
	double value;
	wxArrayString names;
	for (i = 0; i < currentLine.size(); i++)
	{
		if (!currentLine[i].ToDouble(&value))
		{
			nonNumericColumns.Add(i);
			continue;
		}

		names.Add(wxString::Format("[%i]", i));
	}

	return names;
}

//=============================================================================
// Class:			DataFile
// Function:		RemoveUnwantedDescriptions
//
// Desc:		Returns the size to use when allocating the raw data array.
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
//=============================================================================
wxArrayString DataFile::RemoveUnwantedDescriptions(const wxArrayString &names,
	const wxArrayInt &choices) const
{
	if (names.Count() == 0)
		return names;

	wxArrayString selectedNames;
	selectedNames.Add(names[0]);
	unsigned int i;
	for (i = 1; i < names.size(); i++)
	{
		if (ArrayContainsValue(AdjustForSkippedColumns(i - 1), choices))
			selectedNames.Add(names[i]);
	}

	return selectedNames;
}

//=============================================================================
// Class:			DataFile
// Function:		GetRawDataSize
//
// Desc:		Returns the size to use when allocating the raw data array.
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
//=============================================================================
unsigned int DataFile::GetRawDataSize(const unsigned int &selectedCount) const
{
	return selectedCount + 1;// +1 for time data
}

//=============================================================================
// Class:			DataFile
// Function:		ExtractData
//
// Desc:		Parses the file and reads data into vectors.  Only extracts
//					the data the user selected for display.
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
//=============================================================================
bool DataFile::ExtractData(std::ifstream &file, const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors,
	wxString &errorString) const
{
	std::string nextLine;
	wxArrayString parsed;
	unsigned int i, set, curveCount(choices.size() + 1);
	unsigned int lineNumber(headerLines);
	double tempDouble;
	std::vector<double> newFactors(choices.size() + 1, 1.0);
	while (std::getline(file, nextLine))
	{
		lineNumber++;
		parsed = ParseLineIntoColumns(nextLine, delimiter);
		if (parsed.size() < curveCount)
		{
			if (!file.eof())
				wxMessageBox(_T("Terminating data extraction prior to reaching end-of-file."),
					_T("Column Count Mismatch"), wxICON_WARNING);
			return true;
		}

		set = 0;
		for (i = 0; i < parsed.size(); i++)
		{
			if (i == 0 || ArrayContainsValue(i - 1, choices))// Always take the time column; +1 due to time column not included in choices
			{
				if (!parsed[i].ToDouble(&tempDouble))
				{
					errorString.Printf("Failed to convert entry at row %i, column %i, to a number.",
						lineNumber, i + 1);
					return false;
				}

				rawData[set].push_back(tempDouble);
				newFactors[set] = factors[i];// Update scales for cases where user didn't select a column
				set++;
			}
		}
	}
	factors = newFactors;

	return true;
}

//=============================================================================
// Class:			DataFile
// Function:		ArrayContainsValue
//
// Desc:		Determines whether or not the specified value is contained in
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
//=============================================================================
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

//=============================================================================
// Class:			DataFile
// Function:		AssembleDatasets
//
// Desc:		Takes raw data in std::vector<double> format and populates
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
//=============================================================================
void DataFile::AssembleDatasets(const std::vector<double> *rawData, const unsigned int &dataSize)
{
	assert(dataSize > 1 && rawData);

	unsigned int i;
	for (i = 1; i < dataSize; i++)
	{
		std::unique_ptr<Dataset2D> dataset;
		if (i == 1)
		{
			dataset = std::make_unique<Dataset2D>(rawData[0].size());
			TransferVectorToArray(rawData[0], dataset->GetXPointer());
		}
		else
			dataset = std::make_unique<Dataset2D>(*data[0]);

		TransferVectorToArray(rawData[i], dataset->GetYPointer());
		*dataset *= scales[i];
		data.push_back(std::move(dataset));
	}
}

//=============================================================================
// Class:			DataFile
// Function:		TransferVectorToArray
//
// Desc:		Takes data in a std::vector<double> and moves it into a
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
//=============================================================================
void DataFile::TransferVectorToArray(const std::vector<double> &source,
	double *destination) const
{
	unsigned int i;
	for (i = 0; i < source.size(); i++)
		destination[i] = source[i];
}

//=============================================================================
// Class:			DataFile
// Function:		SkipLines
//
// Desc:		Reads and discards the specified number of lines from
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
//=============================================================================
void DataFile::SkipLines(std::ifstream &file, const unsigned int &count)
{
	std::string nextLine;
	unsigned int i;
	for (i = 0; i < count; i++)
		std::getline(file, nextLine);
}

//=============================================================================
// Class:			DataFile
// Function:		IsDataRow
//
// Desc:		Checks to see if the input array could be a valid data row.
//
// Input Arguments:
//		list	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool DataFile::IsDataRow(const wxArrayString &list) const
{
	double value;
	if (!timeIsFormatted)
	{
		if (list[0].IsEmpty() || !list[0].ToDouble(&value))
			return false;
	}
	// TODO:  Check that first column time format is OK?

	unsigned int j;
	for (j = 1; j < list.size(); j++)
	{
		if (!list[j].IsEmpty() && list[j].ToDouble(&value))
			return true;
	}

	return false;
}

//=============================================================================
// Class:			DataFile
// Function:		GetTimeValue
//
// Desc:		Converts the time string with the given format into the
//					specified units.  Converts to seconds first, then applies
//					a scaling factor.
//					Will recognize (case insensitive) H as hour, m as minute,
//					s as second and x as millisecond.
//					Same formatting is assummed between delimiters (i.e. hm:s
//					is interpreted as h:s).
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
//=============================================================================
double DataFile::GetTimeValue(const wxString &timeString,
	const wxString &timeFormat, const wxString &timeUnits) const
{
	double factor;
	if (!GuiInterface::UnitStringToFactor(timeUnits, factor))
		factor = 1.0;

	wxChar delimiter(':');
	unsigned int formatStart(0), formatCount(0), timeStart(0), timeCount(0);
	double time(0.0), value;
	while (true)
	{
		formatCount = timeFormat.Mid(formatStart).Find(delimiter);
		timeCount = timeString.Mid(timeStart).Find(delimiter);
		if (!timeString.Mid(timeStart, timeCount).ToDouble(&value))
		{
			// TODO:  Add warning for user in this case?
			// What if user opens file with 100,000 data points, all containing
			// the same error - don't want to spam them with warnings, though.
			value = 0.0;
		}
		value *= GetTimeScalingFactor(timeFormat.Mid(formatStart, 1));

		// TODO:  Handle rollovers (i.e. going from 23:59:59 to 00:00:00)
		time += value;
		if ((int)formatCount == wxNOT_FOUND || (int)timeCount == wxNOT_FOUND)
			break;

		formatStart += formatCount + 1;
		timeStart += timeCount + 1;
	}

	return time * factor;
}

//=============================================================================
// Class:			DataFile
// Function:		GetTimeScalingFactor
//
// Desc:		Returns the proper scaling factor for the specified
//					format code.
//
// Input Arguments:
//		format	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double DataFile::GetTimeScalingFactor(const wxString &format) const
{
	if (format.CmpNoCase(_T("H")) == 0)// Hour
		return 3600.0;
	else if (format.CmpNoCase(_T("M")) == 0)// Minute
		return 60.0;
	else if (format.CmpNoCase(_T("S")) == 0)// Second (Do nothing)
		return 1.0;
	else if (format.CmpNoCase(_T("X")) == 0)// Millisecond
		return 0.001;

	// TODO:  Generate a warning to tell the user we didn't understand their format

	return 0.0;
}

//=============================================================================
// Class:			DataFile
// Function:		DescriptionsMatch
//
// Desc:		Determines if the list of descriptions in this file match
//					the descriptions given in the specified file.
//
// Input Arguments:
//		file	= const DataFile&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool DataFile::DescriptionsMatch(const DataFile &file) const
{
	return DescriptionsMatch(file.descriptions);
}

//=============================================================================
// Class:			DataFile
// Function:		DescriptionsMatch
//
// Desc:		Determines if the list of descriptions in this file match
//					the descriptions given in the specified file.
//
// Input Arguments:
//		descriptions	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool DataFile::DescriptionsMatch(const wxArrayString &descriptions) const
{
	if (this->descriptions.Count() != descriptions.Count())
		return false;

	unsigned int i;
	for (i = 0; i < descriptions.Count(); i++)
	{
		if (this->descriptions[i].compare(descriptions[i]) != 0)
			return false;
	}

	return true;
}

//=============================================================================
// Class:			DataFile::SelectionData
// Function:		SelectionData
//
// Desc:		Constructor for DataFile::SelectionData struct.
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
DataFile::SelectionData::SelectionData()
{
	removeExisting = true;
}

}// namespace LibPlot2D
