/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  customXMLFile.h
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for custom (XML) files defined by the user using an XML file.
// History:

#ifndef _CUSTOM_XML_FILE_H_
#define _CUSTOM_XML_FILE_H_

// Local headers
#include "application/dataFiles/dataFile.h"
#include "application/dataFiles/customFileFormat.h"

// wxWidgets headers
#include <wx/xml/xml.h>

class CustomXMLFile : public DataFile
{
public:
	// Constructor
	CustomXMLFile(const wxString& _fileName, wxWindow *_parent = NULL) : DataFile(_fileName, _parent), fileFormat(_fileName) {};

	static bool IsType(const wxString &_fileName);

protected:
	CustomFileFormat fileFormat;

	virtual wxArrayString CreateDelimiterList(void) const;
	virtual bool ExtractData(std::ifstream &file, const wxArrayInt &choices,
		std::vector<double> *rawData, std::vector<double> &factors) const;
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors) const;

	wxArrayString SeparateNodes(const wxString &nodePath) const;
	wxXmlNode* FollowNodePath(const wxXmlDocument &document, const wxString &path) const;
	wxXmlNode* FollowNodePath(wxXmlNode *node, const wxString &path) const;

	bool DataStringToVector(const wxString &data, std::vector<double> &dataVector, const double &factor) const;

	bool ExtractXData(std::vector<double> *rawData, std::vector<double> &factors) const;
	bool ExtractYData(wxXmlNode *channel, std::vector<double> *rawData,
		std::vector<double> &factors, const unsigned int &set) const;
};

#endif//_CUSTOM_XML_FILE_H_