/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  dataFile.h
// Date:  10/2/2012
// Auth:  K. Loux
// Desc:  Base class for data file classes.

#ifndef DATA_FILE_H_
#define DATA_FILE_H_

// Local headers
#include "lp2d/utilities/dataset2D.h"

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <vector>
#include <fstream>
#include <memory>

namespace LibPlot2D
{

/// Base class for data input files.  Classes derived from this must define a
/// method that has type `FileTypeManager::TypeCheckFunction`.  This method
/// must determine if the class is a good match for the file (i.e. can the
/// class open the specified file).  Derived classes must also declare
/// `std::unique_ptr<DataFile>
/// DataFile::Create<DerivedClassType>(const wxString&)` to be a friend.
///
/// Note that in order to use GuiInterface::RegisterFileType(), the name
///	chosen for the static method described above must be IsType().  Other names
/// may be used only if types are registered by calling
/// FileTypeManager::RegisterFileType() directly.
///
/// Also note that it is a requirement of base classes (in order for their
/// types to be registerable with FileTypeManager) that their constructors take
///	a single `const wxString&` argument.
///
/// \see FileTypeManager
/// \see GuiInterface
class DataFile
{
public:
	virtual ~DataFile() = default;

	/// Factory method for creating new DataFile objects.
	///
	/// \param fileName Path and file name of desired file.
	///
	/// \returns Pointer to a new DataFile of the appropriate type.
	template<typename T>
	static std::unique_ptr<DataFile> Create(const wxString& fileName);

	/// Performs initialization of this object.
	void Initialize();

	/// Container for storing information about user choices regarding
	/// extracting data from the file.
	struct SelectionData
	{
		wxArrayInt selections;///< Indices of channels selected by the user.

		/// Indicates whether or not existing curves should be removed.
		bool removeExisting = true;
	};
	
	/// Prompts the user to make selections regarding importing data from file.
	///
	/// \param [in, out] selectionInfo User selection information.  Use
	///                                non-empty input argument to specify
	///                                default selections.
	/// \param           parent        Pointer to parent window.
	void GetSelectionsFromUser(SelectionData &selectionInfo, wxWindow *parent);

	/// Loads the data from file according to the specified options.
	///
	/// \param selectionInfo User-specified options for importing new data.
	///
	/// \returns True if the data was successfully extracted from the file.
	bool Load(const SelectionData &selectionInfo);

	/// Returns a Dataset2D for the specified channel.
	///
	/// \param i Index of the channel to retrieve.
	///
	/// \returns Dataset2D for the specified channel.
	std::unique_ptr<Dataset2D>& GetDataset(const unsigned int &i) { return mData[i]; }

	/// Gets the description for the specified channel.
	///
	/// \param i Index of the channel to retrieve.
	///
	/// \returns The description for the specified channel.
	wxString GetDescription(const unsigned int &i) const { return mSelectedDescriptions[i]; }

	/// Gets the full set of descriptions for the available channels.
	/// \returns The full set of descriptions for the available channels.
	wxArrayString GetAllDescriptions() const { return mDescriptions; }

	/// Gets the number of available data channels.
	/// \returns The number of available data channels.
	unsigned int GetDataCount() { return mData.size(); }

	/// Checks to see if the descriptions are the same as the specified file.
	///
	/// \param file File against which the comparison will be made.
	///
	/// \returns True if the descriptions in the specified file match those in
	///          this file.
	bool DescriptionsMatch(const DataFile &file) const;

	/// Checks to see if the descriptions are the same as the specified list.
	///
	/// \param descriptions List against which the comparison will be made.
	///
	/// \returns True if the descriptions in the specified list match those in
	///          this file.
	bool DescriptionsMatch(const wxArrayString &descriptions) const;

protected:
	/// Constructor.
	///
	/// \param fileName Path and file name of desired file.
	explicit DataFile(const wxString& fileName);

	const wxString mFileName;///< Path and file name of desired file.

	std::vector<std::unique_ptr<Dataset2D>> mData;///< Extracted data.
	std::vector<double> mScales;///< Scale factors to use when extracting data.
	wxArrayString mDescriptions;///< For all channels.
	wxArrayString mSelectedDescriptions;///< For selected channels.
	wxString mDelimiter;///< Delimiter to use when parsing the file.
	wxArrayInt mNonNumericColumns;///< Columns containing unparsable data.

	unsigned int mHeaderLines = 0;///< Number of rows that do not contain data.

	/// Flag indicating that consecutive delimiters should be treated as a
	/// single delimiter.
	bool mIgnoreConsecutiveDelimiters = true;

	/// Flag indicating that the time data is formatted in a special way (as
	/// opposed to simply being represented by a floating-point value).
	bool mTimeIsFormatted = false;

	/// Parses the file to determine which delimiter is most likely to result
	/// in successfull data extraction.
	wxString DetermineBestDelimiter() const;

	/// Builds a list of potential delimiters to use for parsing the file.
	/// \returns List of potential delimiters.
	virtual wxArrayString CreateDelimiterList() const;

	/// Pulls the specified channels from the file.
	///
	/// \param                   file Stream from which the data should be
	///                          extracted.
	/// \param choices           List of channel indices to extract.
	/// \param rawData     [out] Data extracted from file.
	/// \param factors     [out] List of scale factors corresponding to
	///                          extracted data.
	/// \param errorString [out] Explaination of any parsing errors.
	///
	/// \returns True if data was successfully extracted.  If false, check
	///          \p errorString.
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<std::vector<double>>& rawData,
		std::vector<double> &factors, wxString &errorString) const;

	/// Builds Dataset2D objects from the \p rawData.
	///
	/// \param rawData Data extracted from file.
	virtual void AssembleDatasets(
		const std::vector<std::vector<double>>& rawData);

	/// Extracts the descriptions of the available data, as well as other
	/// information regarding the file contents.
	///
	/// \param headerLineCount   [out] The number of header lines in the file.
	/// \param factors           [out] The scaling factors to use when
	///                                extracting data from file (usually 1,
	///                                but may vary depending on file type).
	/// \param nonNumericColumns [out] Indices for channels contained in the
	///                                data that should be ignored (i.e. cannot
	///                                be plotted).
	///
	/// \returns List of curve descriptions.
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors, wxArrayInt &nonNumericColumns) const;

	/// Placeholder method to allow derived types to perform tasks immediately
	/// after loading the file.
	virtual void DoTypeSpecificLoadTasks() {}

	/// Placeholder method to allow derived types to perform tasks while
	/// extracting the data.
	virtual void DoTypeSpecificProcessTasks() {}

	/// Computes the required size of the raw data vector.
	///
	/// \param selectedCount Number of curves selected for plotting.
	///
	/// \returns The required size of the raw data vector.
	virtual unsigned int GetRawDataSize(
		const unsigned int &selectedCount) const;

	/// Parses the specified line using the specified delimiter to split the
	/// line into columns.
	///
	/// \param line      String to parse.
	/// \param delimiter String to use as the delimiter.
	///
	/// \return List of data parsed from \p line.
	wxArrayString ParseLineIntoColumns(wxString line,
		const wxString &delimiter) const;

	/// Converts the time string with the given format into the specified
	/// units.  Converts to seconds first, then applies a scaling factor.  Will
	/// recognize (case insensitive) H as hour, m as minute, s as second and x
	/// as millisecond.  Same formatting is assummed between delimiters (i.e.
	/// hm:s is interpreted as h:s).
	///
	/// \param timeString String to evaluate.
	/// \param timeFormat Instructions on how to interpret the \p timeString.
	/// \param timeUnits  Desired time units for returned value.
	///
	/// \returns Corresponding time value.
	double GetTimeValue(const wxString &timeString,
		const wxString &timeFormat, const wxString &timeUnits) const;

	/// Gets the appropriate scaling factor to convert from seconds into the
	/// specified units.
	///
	/// \param format String describing desired units.
	///
	/// \returns Scaling factor.
	double GetTimeScalingFactor(const wxString &format) const;

	/// Builds list of channel names based on parsed heading lines.
	///
	/// \param previousLines		   List of previously parsed lines.
	/// \param currentLine			   Current line being parsed.
	/// \param nonNumericColumns [out] If the current line contains data, this
	///                                is used to identify channels that do not
	///                                contain plottable data.
	///
	/// \returns List of channel names.
	wxArrayString GenerateNames(const wxArrayString &previousLines,
		const wxArrayString &currentLine, wxArrayInt &nonNumericColumns) const;

	/// Builds list of channels names when the data file does not contain any
	/// usable description.
	///
	/// \param currentLine             Current line being parsed.
	/// \param nonNumericColumns [out] If the current line contains data, this
	///                                is used to identify channels that do not
	///                                contain plottable data.
	///
	/// \returns List of channel names.
	wxArrayString GenerateDummyNames(const wxArrayString &currentLine,
		wxArrayInt &nonNumericColumns) const;

	/// Checks to see if the parsed line contains plottable data or not.
	///
	/// \param list List of parsed data (i.e. generated with
	///             ParseLineIntoColumns()).
	///
	/// \returns True if the \p list appears to contain plottable data.
	bool IsDataRow(const wxArrayString &list) const;

	/// Checks to see if the specified value appears within the specified
	/// array.
	///
	/// \param value Value for which we are testing.
	/// \param a     Array to search.
	///
	/// \returns True if \p a contains \p value.
	bool ArrayContainsValue(const int &value, const wxArrayInt &a) const;

	/// Generates a prunned list of channel names based on whether or not the
	/// user has selected the channel for plotting.
	///
	/// \param names   Full list of channel names.
	/// \param choices List of channel indices selected by the user.
	///
	/// \returns Prunned list of channel names.
	wxArrayString RemoveUnwantedDescriptions(const wxArrayString &names,
		const wxArrayInt &choices) const;

	/// Advances the get pointer in the stream by the specified number of
	/// newlines (or carriage returns/line feeds).
	///
	/// \param file	 Stream to manipulate.
	/// \param count Number of lines to skip.
	static void SkipLines(std::ifstream &file, const unsigned int &count);

	/// Adjusts the indices to account for columns that were not displayed as
	/// allowable selections.  Index 0 is first data column (not time column).
	///
	/// \param selections List of indices to adjust.
	///
	/// \returns List of adjusted indices.
	wxArrayInt AdjustForSkippedColumns(const wxArrayInt& selections) const;

	/// Adjusts the index to account for columns that were not displayed as
	/// allowable selections.  Index 0 is first data column (not time column).
	///
	/// \param i Index to adjust.
	///
	/// \returns Adjusted index.
	unsigned int AdjustForSkippedColumns(const unsigned int &i) const;
};

template<typename T>
std::unique_ptr<DataFile> DataFile::Create(const wxString& fileName)
{
	static_assert(std::is_base_of<DataFile, T>::value,
		"T must be a descendant of DataFile");
	return std::make_unique<T>(fileName);
}

}// namespace LibPlot2D

#endif// DATA_FILE_H_
