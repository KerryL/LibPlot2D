/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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
	GenericFile(const wxString& _fileName, wxWindow *_parent = NULL) : DataFile(_fileName, _parent) {};

	static bool IsType(const wxString &_fileName);
};

#endif//_GENERIC_FILE_H_