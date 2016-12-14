/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  baumullerFile.h
// Date:  10/4/2012
// Auth:  K. Loux
// Desc:  File class for Baumuller files.

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
	explicit BaumullerFile(const wxString& fileName) : DataFile(fileName) {}
	~BaumullerFile() = default;

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
