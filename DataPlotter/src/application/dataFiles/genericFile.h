/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  genericFile.h
// Created:  10/3/2012
// Author:  K. Loux
// Description:  Generic delimited data file.
// History:

#ifndef _GENERIC_FILE_H_
#define _GENERIC_FILE_H_

// Local headers
#include "application/dataFiles/dataFile.h"

class GenericFile : public DataFile
{
public:
	// Constructor
	GenericFile(const wxString& _fileName) : DataFile(_fileName) {};

	static bool IsType(const wxString &_fileName);
};

#endif//_GENERIC_FILE_H_