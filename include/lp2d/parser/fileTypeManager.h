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

class FileTypeManager
{
public:
	typedef bool (*TypeCheckFunction)(const wxString &fileName);
	typedef std::unique_ptr<DataFile> (*FileFactory)(const wxString &fileName);
	void RegisterFileType(TypeCheckFunction typeFunction,
		FileFactory fileFactory);

	std::unique_ptr<DataFile> GetDataFile(const wxString &fileName);

private:
	std::vector<std::pair<TypeCheckFunction, FileFactory>> mRegisteredTypes;
};

}// namespace LibPlot2D

#endif// FILE_TYPE_MANAGER_H_
