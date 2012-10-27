/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataFile.h
// Created:  10/2/2012
// Author:  K. Loux
// Description:  Base class for data file classes.
// History:

#ifndef _DATA_FILE_H_
#define _DATA_FILE_H_

// Standard C++ headers
#include <vector>
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// Local forward declarations
class Dataset2D;

class DataFile
{
public:
	// Constructor
	DataFile(const wxString& _fileName);

	// Destructor
	virtual ~DataFile();

	bool Load(void);


	Dataset2D* GetDataset(const unsigned int &i) const { return data[i]; };
	wxString GetDescription(const unsigned int &i) const { return descriptions[i]; };
	unsigned int GetDataCount(void) { return data.size(); };

	// Classes derived from this should have this method:
	//static bool IsType(const wxWidgets& _fileName);

protected:
	wxString fileName;
	std::vector<Dataset2D*> data;
	std::vector<double> scales;
	wxArrayString descriptions;
	wxString delimiter;

	unsigned int headerLines;
	bool ignoreConsecutiveDelimiters;
	bool timeIsFormatted;

	wxString DetermineBestDelimiter(void) const;

	virtual wxArrayString CreateDelimiterList(void) const;
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors) const;
	virtual void AssembleDatasets(const std::vector<double> *rawData,
		const unsigned int &dataSize);
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors) const;
	virtual void DoTypeSpecificLoadTasks(void) {};
	virtual void DoTypeSpecificProcessTasks(void) {};
	virtual unsigned int GetRawDataSize(const unsigned int &selectedCount) const;

	wxArrayString ParseLineIntoColumns(wxString line, const wxString &delimiter) const;
	double GetTimeValue(const wxString &timeString,
		const wxString &timeFormat, const wxString &timeUnits) const;
	double GetTimeScalingFactor(const wxString &format) const;

	wxArrayString GenerateNames(const wxArrayString &previousLines,
		const wxArrayString &currentLine) const;
	wxArrayString GenerateDummyNames(const unsigned int &count) const;
	bool ListIsNumeric(const wxArrayString &list) const;

	bool ProcessFile(void);
	bool ArrayContainsValue(const int &value, const wxArrayInt &a) const;
	void TransferVectorToArray(const std::vector<double> &source, double *destination) const;
	wxArrayString RemoveUnwantedDescriptions(const wxArrayString &names, const wxArrayInt &choices) const;

	static void SkipLines(std::ifstream &file, const unsigned int &count);
};

#endif// _DATA_FILE_H_