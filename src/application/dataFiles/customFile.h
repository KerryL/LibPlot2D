/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

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
#include "application/dataFiles/dataFile.h"
#include "application/dataFiles/customFileFormat.h"

class CustomFile : public DataFile
{
public:
	// Constructor
	CustomFile(const wxString& fileName) : DataFile(fileName), fileFormat(fileName) {};

	static bool IsType(const wxString &_fileName);

protected:
	CustomFileFormat fileFormat;

	virtual wxArrayString CreateDelimiterList() const;
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors, wxString &errorString) const;
	virtual void AssembleDatasets(const std::vector<double> *rawData, const unsigned int &dataSize);
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors, wxArrayInt &nonNumericColumns) const;
	virtual void DoTypeSpecificLoadTasks();
	virtual unsigned int GetRawDataSize(const unsigned int &selectedCount) const;

	bool ExtractSpecialData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors, wxString &errorString) const;
	bool ExtractAsynchronousData(double &timeZero, const wxArrayString &parsedLine,
		std::vector<double> *rawData, std::vector<double> &factors, const wxArrayInt &choices,
		wxString &errorString) const;
	bool ExtractSynchronousData(double &timeZero, const wxArrayString &parsedLine,
		std::vector<double> *rawData, std::vector<double> &factors, const wxArrayInt &choices,
		wxString &errorString) const;
	void AssembleAsynchronousDatasets(const std::vector<double> *rawData, const unsigned int &dataSize);
};

#endif//_CUSTOM_FILE_H_