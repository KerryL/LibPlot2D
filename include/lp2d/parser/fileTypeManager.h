/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  fileTypeManager.h
// Date:  12/15/2016
// Auth:  K. Loux
// Desc:  Object for managing various file types.

#ifndef FILE_TYPE_MANAGER_H_
#define FILE_TYPE_MANAGER_H_

// Local headers
#include "lp2d/parser/dataFile.h"

// Standard C++ headers
#include <vector>

namespace LibPlot2D
{

/// Class for managing available file data types.  Known file types can be
/// registered with an instance of this class, and then files can be opened
/// without any requirements on the user or calling software to know the type
/// of the file.  This object will handle identifying the type of file and
/// will return an instance of the appropriate type (assuming that one of the
/// registered types was capable of opening the specified file).
class FileTypeManager
{
public:
	/// Typedef for file type-checking functions.
	/// \see DataFile
	typedef bool (*TypeCheckFunction)(const wxString &fileName);

	/// Typedef for file creation methods.
	/// \see DataFile
	typedef std::unique_ptr<DataFile> (*FileFactory)(const wxString &fileName);

	/// Registers the specified pair of type-checking function and file factory
	/// function.
	///
	/// \param typeFunction Pointer to a method for determining if a file can
	///                     be opened with the corresponding \p fileFactory.
	/// \param fileFactory  Pointer to a method for opening a file for which
	///                     the \p typeFunction returns true.
	void RegisterFileType(TypeCheckFunction typeFunction,
		FileFactory fileFactory);

	/// Gets a new DataFile object of the appropriate type.
	///
	/// \param fileName Path and file name of desired file.
	///
	/// \returns A pointer to a new DataFile object.
	std::unique_ptr<DataFile> GetDataFile(const wxString &fileName);

private:
	std::vector<std::pair<TypeCheckFunction, FileFactory>> mRegisteredTypes;
};

}// namespace LibPlot2D

#endif// FILE_TYPE_MANAGER_H_
