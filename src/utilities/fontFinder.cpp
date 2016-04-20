/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  fontFinder.cpp
// Created:  11/17/2011
// Author:  K. Loux
// Description:  Cross-platform methods for working with fonts.  Builds on wxWidgets
//				 objects.
// History:

// Standard C++ headers
#include <cstdlib>
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/fontenum.h>

// Local headers
#include "utilities/fontFinder.h"

//==========================================================================
// Class:			FontFinder
// Function:		GetFontFileName
//
// Description:		Searches the local hard drive (intelligently) and returns
//					the path and file name for a preferred font.
//
// Input Arguments:
//		fontName	= const wxString& name of the desired font
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the path to the font file, or an empty string
//		if the font could not be located
//
//==========================================================================
wxString FontFinder::GetFontFileName(const wxString &fontName)
{
	wxString fontDirectory;

#ifdef __WXMSW__
	fontDirectory = wxGetOSDirectory() + _T("\\Fonts\\");
#elif defined __WXGTK__
	fontDirectory = _T("/usr/share/fonts/");
#else
	// Unknown platform - warn the user
#	warning "Unrecognized platform - unable to locate font files!"
	return wxEmptyString;
#endif

	wxArrayString fontFiles;
	wxDir::GetAllFiles(fontDirectory, &fontFiles, _T("*.ttf"), wxDIR_FILES | wxDIR_DIRS);

	unsigned int i;
	wxString nameFromFile;
	for (i = 0; i < fontFiles.GetCount(); i++)
	{
		if (GetFontName(fontFiles[i], nameFromFile))
		{
			if (fontName.CmpNoCase(nameFromFile) == 0)
				return fontFiles[i];
		}
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetPreferredFontFileName
//
// Description:		Returns the file name for a best match for a font on the
//					system when given a list of acceptable fonts.
//
// Input Arguments:
//		encoding		= wxFontEncoding
//		preferredFonts	= const wxArrayString& list of preferred font faces
//		fixedWidth		= const bool&
//
// Output Arguments:
//		fontFile		= wxString& containing the name of the best match
//
// Return Value:
//		bool, true for found a match from the preferred list
//
//==========================================================================
bool FontFinder::GetPreferredFontFileName(wxFontEncoding encoding,
		const wxArrayString &preferredFonts, const bool &fixedWidth, wxString &fontFile)
{
	// Get a list of the fonts found on the system
	wxArrayString fontList = wxFontEnumerator::GetFacenames(encoding, fixedWidth);

	// See if any of the installed fonts matches our list of preferred fonts
	unsigned int i, j;
	for (i = 0; i < preferredFonts.GetCount(); i++)
	{
		for (j = 0; j < fontList.GetCount(); j++)
		{
			// If the system font matches
			if (preferredFonts[i].CmpNoCase(fontList[j]) == 0)
			{
				// See if we can find the file for this font
				fontFile = GetFontFileName(fontList[j]);
				if (!fontFile.IsEmpty())
					return true;
			}
		}
	}

	// We didn't find our preferred fonts, now let's just go down the list until we find ANY font file
	for (i = 0; i < fontList.GetCount(); i++)
	{
		fontFile = GetFontFileName(fontList[i]);
		if (!fontFile.IsEmpty())
			return true;
	}

	// Nothing found - return false with empty fontFile
	return false;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetFontFaceName
//
// Description:		Returns the best match for a font on the system when given
//					a list of acceptable fonts.
//
// Input Arguments:
//		encoding		= wxFontEncoding
//		preferredFonts	= const wxArrayString& list of preferred font faces
//		fixedWidth		= const bool&
//
// Output Arguments:
//		fontName		= wxString& containing the name of the best match
//
// Return Value:
//		bool, true for found a match
//
//==========================================================================
bool FontFinder::GetFontFaceName(wxFontEncoding encoding, const wxArrayString &preferredFonts,
	const bool &fixedWidth, wxString &fontName)
{
	// Get a list of the fonts found on the system
	wxArrayString fontList = wxFontEnumerator::GetFacenames(encoding, fixedWidth);

	// See if any of them are in our preferred fonts list
	// Assume list is organized with most desired fonts first
	unsigned int i, j;
	for (i = 0; i < preferredFonts.GetCount(); i++)
	{
		for (j = 0; j < fontList.GetCount(); j++)
		{
			if (preferredFonts[i].CmpNoCase(fontList[j]) == 0)
			{
				fontName = fontList[j];
				return true;
			}
		}
	}

	// If no fonts were found with the enumerator, return an empty string
	if (fontList.GetCount() > 0)
		fontName = fontList[0];
	else
		fontName.Empty();

	return false;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetFontName
//
// Description:		Returns name of the font for the specified .ttf file.
//
// Input Arguments:
//		fontFile	= const wxString& specifying the file location
//
// Output Arguments:
//		fontName	= wxString& containing the name of the font
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool FontFinder::GetFontName(const wxString &fontFile, wxString &fontName)
{
	if (fontFile.IsEmpty())
		return false;

	std::ifstream fontStream(fontFile.mb_str(), std::ios::in | std::ios::binary);
	if (!fontStream.good() || !fontStream.is_open())
		return false;

	TT_OFFSET_TABLE ttOffsetTable = ReadOffsetTable(fontStream);

	// Make sure this is a version 1.0 TrueType Font
	if(ttOffsetTable.majorVersion != 1 || ttOffsetTable.minorVersion != 0)
		return false;

	TT_TABLE_DIRECTORY tblDir;
	if (!GetNameTable(fontStream, ttOffsetTable, tblDir))
		return false;

	TT_NAME_TABLE_HEADER ttNTHeader = GetNameTableHeader(fontStream, tblDir.offset);

	unsigned int i;
	for (i = 0; i < ttNTHeader.nrCount; i++)
	{
		fontName = CheckHeaderForName(fontStream, tblDir.offset + ttNTHeader.storageOffset);
		if (!fontName.IsEmpty())
			return true;
	}

	return false;
}

//==========================================================================
// Class:			FontFinder
// Function:		ReadOffsetTable
//
// Description:		Reads the offset table from file.
//
// Input Arguments:
//		file	= std::ifstream& to read from
//
// Output Arguments:
//		None
//
// Return Value:
//		TT_OFFSET_TABLE containing endian-correct table information
//
//==========================================================================
FontFinder::TT_OFFSET_TABLE FontFinder::ReadOffsetTable(std::ifstream &file)
{
	TT_OFFSET_TABLE table;
	file.read((char*)&table, sizeof(TT_OFFSET_TABLE));

	// Rearrange the bytes for the important fields
	SwapEndian(table.numOfTables);
	SwapEndian(table.majorVersion);
	SwapEndian(table.minorVersion);

	return table;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetNameTable
//
// Description:		Finds and outputs the name table from the file.
//
// Input Arguments:
//		file		= std::ifstream& to read from
//		offsetTable	= const TT_OFFSET_TABLE&
//
// Output Arguments:
//		table	= TT_TABLE_DIRECTORY&
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool FontFinder::GetNameTable(std::ifstream &file, const TT_OFFSET_TABLE &offsetTable, TT_TABLE_DIRECTORY &table)
{
	table.offset = 0;// To avoid MSVC++ Warning C4701
	wxString tempTagString;
	wxString tableName("name");

	int i;
	for (i = 0; i < offsetTable.numOfTables; i++)
	{
		file.read((char*)&table, sizeof(TT_TABLE_DIRECTORY));

		// Table's tag is <= 4 characters
		tempTagString = table.tag[0];
		tempTagString += table.tag[1];
		tempTagString += table.tag[2];
		tempTagString += table.tag[3];
		if(tempTagString.CmpNoCase(tableName) == 0)
		{
			SwapEndian(table.length);
			SwapEndian(table.offset);
			return true;
		}
	}

	return false;
}

//==========================================================================
// Class:			FontFinder
// Function:		GetNameTableHeader
//
// Description:		Gets the header for the name table from file.
//
// Input Arguments:
//		file	= std::ifstream& to read from
//		offset	= const size_t& specifying the offset from the beginning of the file
//
// Output Arguments:
//		None
//
// Return Value:
//		TT_NAME_TABLE_HEADER filled with endian-correct header info
//
//==========================================================================
FontFinder::TT_NAME_TABLE_HEADER FontFinder::GetNameTableHeader(std::ifstream &file, const size_t &offset)
{
	TT_NAME_TABLE_HEADER header;

	file.seekg(offset, std::ios_base::beg);
	file.read((char*)&header, sizeof(TT_NAME_TABLE_HEADER));

	SwapEndian(header.nrCount);
	SwapEndian(header.storageOffset);

	return header;
}

//==========================================================================
// Class:			FontFinder
// Function:		CheckHeaderForName
//
// Description:		Checks the table record (in the ifstream) for a font name.
//
// Input Arguments:
//		file	= std::ifstream& to read from
//		offset	= const size_t& specifying the offset from the beginning of the file
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name, or an empty string if not found
//
//==========================================================================
wxString FontFinder::CheckHeaderForName(std::ifstream &file, const size_t &offset)
{
	TT_NAME_RECORD ttRecord;
	wxString name;

	file.read((char*)&ttRecord, sizeof(TT_NAME_RECORD));
	SwapEndian(ttRecord.nameID);

	// Name ID == 1 indicates font name
	if (ttRecord.nameID == 1)
	{
		SwapEndian(ttRecord.stringLength);
		SwapEndian(ttRecord.stringOffset);

		// Save file position, so we can return to continue with search
		int nPos = file.tellg();
		file.seekg(ttRecord.stringOffset + offset, std::ios_base::beg);

		char *nameBuffer = new char[ttRecord.stringLength];
		file.read(nameBuffer, ttRecord.stringLength);
		name.assign(nameBuffer);
		// Apparent bug with setting the string length every time:
		// When the string is empty, and we assign a length anyway, it is no
		// longer emtpy, even though it contains no valid data.  As a workaround,
		// we only assign the proper length if the string is not already empty
		if (!name.IsEmpty())
			name.resize(ttRecord.stringLength);
		delete [] nameBuffer;

		file.seekg(nPos, std::ios_base::beg);
	}

	return name;
}