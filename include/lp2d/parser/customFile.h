/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  customFile.h
// Date:  10/4/2012
// Auth:  K. Loux
// Desc:  File class for custom (non-XML) files defined by the user using an
//        XML file.

#ifndef CUSTOM_FILE_H_
#define CUSTOM_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"
#include "lp2d/parser/customFileFormat.h"

namespace LibPlot2D
{

/// Class representing user-defined text-delimited data files.
class CustomFile : public DataFile
{
public:
	/// Constructor.
	///
	/// \param fileName Path and file name of desired file.
	explicit CustomFile(const wxString& fileName) : DataFile(fileName),
		mFileFormat(fileName) {}

	~CustomFile() = default;

	/// Checks to determine if the specified file can successfully be an object
	/// of this type.
	///
	/// \param fileName Path and file name of desired file.
	///
	/// \returns True if the specified file is likely to be compatible with an
	/// object of this type.
	static bool IsType(const wxString &fileName);

protected:
	CustomFileFormat mFileFormat;///< Information about the custom format.

	wxArrayString CreateDelimiterList() const override;
	bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<std::vector<double>>& rawData,
		std::vector<double> &factors, wxString &errorString) const override;
	void AssembleDatasets(
		const std::vector<std::vector<double>>& rawData) override;
	wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors,
		wxArrayInt &nonNumericColumns) const override;
	void DoTypeSpecificLoadTasks() override;
	unsigned int GetRawDataSize(
		const unsigned int &selectedCount) const override;

	friend std::unique_ptr<DataFile>
		DataFile::Create<CustomFile>(const wxString&);

private:
	bool ExtractSpecialData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<std::vector<double>>& rawData,
		std::vector<double> &factors, wxString &errorString) const;
	bool ExtractAsynchronousData(double &timeZero,
		const wxArrayString &parsedLine,
		std::vector<std::vector<double>>& rawData,
		std::vector<double> &factors, const wxArrayInt &choices,
		wxString &errorString) const;
	bool ExtractSynchronousData(double &timeZero,
		const wxArrayString &parsedLine,
		std::vector<std::vector<double>>& rawData,
		std::vector<double> &factors, const wxArrayInt &choices,
		wxString &errorString) const;
	void AssembleAsynchronousDatasets(
		const std::vector<std::vector<double>>& rawData);
};

}// namespace LibPlot2D

#endif// CUSTOM_FILE_H_
