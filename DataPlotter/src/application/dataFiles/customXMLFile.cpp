/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  customXMLFile.cpp
// Created:  10/4/2012
// Author:  K. Loux
// Description:  File class for custom (XML) files defined by the user using an XML file.
// History:

// Local headers
#include "application/dataFiles/customXMLFile.h"

//==========================================================================
// Class:			CustomXMLFile
// Function:		IsType
//
// Description:		Method for determining if the specified file is this
//					type of file.
//
// Input Arguments:
//		_fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if this object can process the specified file, false otherwise
//
//==========================================================================
bool CustomXMLFile::IsType(const wxString &_fileName)
{
	/*CustomFileFormat format(_fileName);
	return format.IsCustomFormat();*/
	return false;
}