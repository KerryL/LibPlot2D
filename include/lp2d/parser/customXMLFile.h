/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  customXMLFile.h
// Date:  10/4/2012
// Auth:  K. Loux
// Desc:  File class for custom (XML) files defined by the user using an XML file.

#ifndef CUSTOM_XML_FILE_H_
#define CUSTOM_XML_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"
#include "lp2d/parser/customFileFormat.h"

// wxWidgets headers
#include <wx/xml/xml.h>

namespace LibPlot2D
{

/// Class representing user-defined data files using an XML-based format.
class CustomXMLFile : public DataFile
{
public:
	/// Constructor.
	///
	/// \param fileName Path and file name of desired file.
	explicit CustomXMLFile(const wxString& fileName) : DataFile(fileName),
		mFileFormat(fileName) {}

	~CustomXMLFile() = default;

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
		std::vector<std::vector<double>>& rawData, std::vector<double> &factors,
		wxString &errorString) const override;
	wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors, wxArrayInt &nonNumericColumns) const override;

private:
	wxArrayString SeparateNodes(const wxString &nodePath) const;
	wxXmlNode* FollowNodePath(const wxXmlDocument &document, const wxString &path) const;
	wxXmlNode* FollowNodePath(wxXmlNode *node, const wxString &path) const;

	bool DataStringToVector(const wxString &data, std::vector<double> &dataVector,
		const double &factor, wxString& errorString) const;

	bool ExtractXData(std::vector<std::vector<double>>& rawData, std::vector<double> &factors,
		wxString& errorString) const;
	bool ExtractYData(wxXmlNode *channel, std::vector<std::vector<double>>& rawData,
		std::vector<double> &factors, const unsigned int &set, wxString& errorString) const;

	friend std::unique_ptr<DataFile>
		DataFile::Create<CustomXMLFile>(const wxString&);
};

}// namespace LibPlot2D

#endif// CUSTOM_XML_FILE_H_
