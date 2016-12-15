/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  kollmorgenFile.h
// Date:  10/4/2012
// Auth:  K. Loux
// Desc:  File class for Kollmorgen files.

#ifndef KOLLMORGEN_FILE_H_
#define KOLLMORGEN_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"

namespace LibPlot2D
{

class KollmorgenFile final : public DataFile
{
public:
	// Constructor
	explicit KollmorgenFile(const wxString& fileName)
		: DataFile(fileName) {}

	~KollmorgenFile() = default;

	static bool IsType(const wxString &fileName);

protected:
	wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors, wxArrayInt &nonNumericColumns) const override;
	void DoTypeSpecificLoadTasks() override;
	bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<std::vector<double>>& rawData, std::vector<double> &factors,
		wxString &errorString) const override;

private:
	double timeStep;// [sec]

	friend std::unique_ptr<DataFile>
		DataFile::Create<KollmorgenFile>(const wxString&);
};

}// namespace LibPlot2D

#endif// KOLLMORGEN_FILE_H_
