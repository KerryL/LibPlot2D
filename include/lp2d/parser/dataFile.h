/*=============================================================================
                                    DataPlotter
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

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <vector>
#include <fstream>
#include <memory>

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

class DataFile
{
public:
	// Constructor
	DataFile(const wxString& fileName);

	// Destructor
	virtual ~DataFile();

	void Initialize();

	struct SelectionData
	{
		SelectionData();
		wxArrayInt selections;
		bool removeExisting;
	};
	
	void GetSelectionsFromUser(SelectionData &selectionInfo, wxWindow *parent);

	bool Load(const SelectionData &selectionInfo);

	std::unique_ptr<Dataset2D>& GetDataset(const unsigned int &i) { return data[i]; }
	wxString GetDescription(const unsigned int &i) const { return selectedDescriptions[i]; }
	wxArrayString GetAllDescriptions() const { return descriptions; }
	unsigned int GetDataCount() { return data.size(); }

	bool DescriptionsMatch(const DataFile &file) const;
	bool DescriptionsMatch(const wxArrayString &descriptions) const;

	// Classes derived from this should have this method:
	//static bool IsType(const wxString& testFile);

protected:
	const wxString fileName;

	std::vector<std::unique_ptr<Dataset2D>> data;
	std::vector<double> scales;
	wxArrayString descriptions, selectedDescriptions;
	wxString delimiter;
	wxArrayInt nonNumericColumns;

	unsigned int headerLines;
	bool ignoreConsecutiveDelimiters;
	bool timeIsFormatted;

	wxString DetermineBestDelimiter() const;

	virtual wxArrayString CreateDelimiterList() const;
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<std::vector<double>>& rawData, std::vector<double> &factors,
		wxString &errorString) const;
	virtual void AssembleDatasets(const std::vector<std::vector<double>>& rawData);
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors, wxArrayInt &nonNumericColumns) const;
	virtual void DoTypeSpecificLoadTasks() {};
	virtual void DoTypeSpecificProcessTasks() {};
	virtual unsigned int GetRawDataSize(const unsigned int &selectedCount) const;

	wxArrayString ParseLineIntoColumns(wxString line, const wxString &delimiter) const;
	double GetTimeValue(const wxString &timeString,
		const wxString &timeFormat, const wxString &timeUnits) const;
	double GetTimeScalingFactor(const wxString &format) const;

	wxArrayString GenerateNames(const wxArrayString &previousLines,
		const wxArrayString &currentLine, wxArrayInt &nonNumericColumns) const;
	wxArrayString GenerateDummyNames(const wxArrayString &currentLine,
		wxArrayInt &nonNumericColumns) const;
	bool IsDataRow(const wxArrayString &list) const;

	bool ArrayContainsValue(const int &value, const wxArrayInt &a) const;
	void TransferVectorToArray(const std::vector<double> &source, double *destination) const;
	wxArrayString RemoveUnwantedDescriptions(const wxArrayString &names, const wxArrayInt &choices) const;

	static void SkipLines(std::ifstream &file, const unsigned int &count);

	wxArrayInt AdjustForSkippedColumns(const wxArrayInt& selections) const;
	unsigned int AdjustForSkippedColumns(const unsigned int &i) const;
};

}// namespace LibPlot2D

#endif// DATA_FILE_H_
