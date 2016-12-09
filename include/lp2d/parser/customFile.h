/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  customFile.h
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for custom (non-XML) files defined by the user using an XML file.
// History:

#ifndef _CUSTOM_FILE_H_
#define _CUSTOM_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"
#include "lp2d/parser/customFileFormat.h"

namespace LibPlot2D
{

class CustomFile : public DataFile
{
public:
	// Constructor
	CustomFile(const wxString& fileName) : DataFile(fileName),
		fileFormat(fileName) {}

	static bool IsType(const wxString &testFile);

protected:
	CustomFileFormat fileFormat;

	wxArrayString CreateDelimiterList() const override;
	bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors,
		wxString &errorString) const override;
	void AssembleDatasets(const std::vector<double> *rawData,
		const unsigned int &dataSize) override;
	wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors,
		wxArrayInt &nonNumericColumns) const override;
	void DoTypeSpecificLoadTasks() override;
	unsigned int GetRawDataSize(
		const unsigned int &selectedCount) const override;

	bool ExtractSpecialData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors,
		wxString &errorString) const;
	bool ExtractAsynchronousData(double &timeZero,
		const wxArrayString &parsedLine, std::vector<double> *rawData,
		std::vector<double> &factors, const wxArrayInt &choices,
		wxString &errorString) const;
	bool ExtractSynchronousData(double &timeZero, const wxArrayString &parsedLine,
		std::vector<double> *rawData, std::vector<double> &factors,
		const wxArrayInt &choices, wxString &errorString) const;
	void AssembleAsynchronousDatasets(const std::vector<double> *rawData,
		const unsigned int &dataSize);
};

}// namespace LibPlot2D

#endif//_CUSTOM_FILE_H_