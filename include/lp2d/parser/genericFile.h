/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  genericFile.h
// Date:  10/3/2012
// Auth:  K. Loux
// Desc:  Generic delimited data file.

#ifndef GENERIC_FILE_H_
#define GENERIC_FILE_H_

// Local headers
#include "lp2d/parser/dataFile.h"

namespace LibPlot2D
{

/// Class representing a generic text file containing data separated by some
/// delimiter.
class GenericFile : public DataFile
{
public:
	/// Constructor.
	///
	/// \param fileName Path and file name of desired file.
	explicit GenericFile(const wxString& fileName) : DataFile(fileName) {}

	~GenericFile() = default;

	/// Checks to determine if the specified file can successfully be an object
	/// of this type.
	///
	/// \param fileName Path and file name of desired file.
	///
	/// \returns True if the specified file is likely to be compatible with an
	/// object of this type.
	static bool IsType(const wxString &fileName);

private:
	friend std::unique_ptr<DataFile>
		DataFile::Create<GenericFile>(const wxString&);
};

}// namespace LibPlot2D

#endif//GENERIC_FILE_H_
