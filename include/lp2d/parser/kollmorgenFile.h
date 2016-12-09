/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  kollmorgenFile.h
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for Kollmorgen files.
// History:

#ifndef _KOLLMORGEN_FILE_H_
#define _KOLLMORGEN_FILE_H_

// Local headers
#include "application/dataFiles/dataFile.h"

class KollmorgenFile : public DataFile
{
public:
	// Constructor
	KollmorgenFile(const wxString& fileName)
		: DataFile(fileName) {};

	static bool IsType(const wxString &_fileName);

protected:
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors, wxArrayInt &nonNumericColumns) const;
	virtual void DoTypeSpecificLoadTasks();
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors,
		wxString &errorString) const;

private:
	double timeStep;// [sec]
};

#endif//_KOLLMORGEN_FILE_H_