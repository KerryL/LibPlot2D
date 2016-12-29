/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  fileTypeManager.cpp
// Date:  12/15/2016
// Auth:  K. Loux
// Desc:  Object for managing various file types.

// Local headers
#include "lp2d/parser/fileTypeManager.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			FileTypeManager
// Function:		RegisterFileType
//
// Description:		Adds the specified file type to the list of registered
//					formats.
//
// Input Arguments:
//		typeFunction	= TypeCheckFunction
//		fileFactory		= FileFactory
//
// Output Arguments:
//		None
//
// Return Value:
//		void
//
//=============================================================================
void FileTypeManager::RegisterFileType(TypeCheckFunction typeFunction,
	FileFactory fileFactory)
{
	registeredTypes.push_back(std::make_pair(typeFunction, fileFactory));
}

//=============================================================================
// Class:			FileTypeManager
// Function:		GetDataFile
//
// Description:		Determines the correct DataFile object to use for the
//					specified file, and returns a pointer to an instance of that
//					object.
//
// Input Arguments:
//		fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::unique_ptr<DataFile>
//
//=============================================================================
std::unique_ptr<DataFile> FileTypeManager::GetDataFile(const wxString &fileName)
{
	for (const auto& type : registeredTypes)
	{
		if (type.first(fileName))
			return type.second(fileName);
	}

	return nullptr;
}

}// namespace LibPlot2D
