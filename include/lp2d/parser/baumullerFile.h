/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  baumullerFile.h
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for Baumuller files.
// History:

#ifndef BAUMULLER_FILE_H_
#define BAUMULLER_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"

namespace LibPlot2D
{

class BaumullerFile final : public DataFile
{
public:
	// Constructor
	BaumullerFile(const wxString& fileName) : DataFile(fileName) {}

	static bool IsType(const wxString &fileName);

protected:
	wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors,
		wxArrayInt &nonNumericColumns) const override;
	void DoTypeSpecificLoadTasks() override;
	void DoTypeSpecificProcessTasks() override;

	bool ConstructNames(std::string &nextLine, std::ifstream &file,
		wxArrayString &names, wxArrayString &previousLines) const;
};

}// namespace LibPlot2D

#endif//BAUMULLER_FILE_H_