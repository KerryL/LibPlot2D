/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

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

class CustomXMLFile : public DataFile
{
public:
	// Constructor
	CustomXMLFile(const wxString& _fileName) : DataFile(_fileName), fileFormat(_fileName) {};

	static bool IsType(const wxString &_fileName);

protected:
	CustomFileFormat fileFormat;
	//virtual wxArrayString GetDescriptions(unsigned int &headerLineCount) const;
};

#endif//_CUSTOM_XML_FILE_H_