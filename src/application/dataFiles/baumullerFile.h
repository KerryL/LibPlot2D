/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  baumullerFile.h
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for Baumuller files.
// History:

#ifndef _BAUMULLER_FILE_H_
#define _BAUMULLER_FILE_H_

// Local headers
#include "application/dataFiles/dataFile.h"

class BaumullerFile : public DataFile
{
public:
	// Constructor
	BaumullerFile(const wxString& fileName) : DataFile(fileName) {};

	static bool IsType(const wxString &_fileName);

protected:
	virtual wxArrayString GetCurveInformation(unsigned int &headerLineCount,
		std::vector<double> &factors) const;
	virtual void DoTypeSpecificLoadTasks(void);
	virtual void DoTypeSpecificProcessTasks(void);

	bool ConstructNames(std::string &nextLine, std::ifstream &file,
		wxArrayString &names, wxArrayString &previousLines) const;
};

#endif//_BAUMULLER_FILE_H_