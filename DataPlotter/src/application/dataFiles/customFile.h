/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

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
	CustomFile(const wxString& _fileName) : DataFile(_fileName), fileFormat(_fileName) {};

	static bool IsType(const wxString &_fileName);

protected:
	CustomFileFormat fileFormat;

	virtual wxArrayString CreateDelimiterList(void) const;
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors) const;
	virtual void AssembleDatasets(const std::vector<double> *rawData, const unsigned int &dataSize);
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount, std::vector<double> &factors) const;

	bool ExtractSpecialData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors) const;
	void AssembleAsynchronousDatasets(const std::vector<double> *rawData, const unsigned int &dataSize);
};

#endif//_CUSTOM_FILE_H_