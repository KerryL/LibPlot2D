/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  genericFile.h
// Created:  10/3/2012
// Author:  K. Loux
// Description:  Generic delimited data file.

#ifndef GENERIC_FILE_H_
#define GENERIC_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"

namespace LibPlot2D
{

class GenericFile : public DataFile
{
public:
	// Constructor
	GenericFile(const wxString& fileName) : DataFile(fileName) {}

	static bool IsType(const wxString &fileName);
};

}// namespace LibPlot2D

#endif//GENERIC_FILE_H_
