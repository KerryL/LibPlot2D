/*===================================================================================
                                    DataPlotter
                         Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

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
	// We will need to search through all the font files on the system, open
	// each one and get the name from file to know if we have the correct file
	wxString fontDirectory;

	// Search through the fonts directory, checking font names
	// until we find the one we're looking for

#ifdef __WXMSW__
	// Get the normal MSW font directory (FIXME:  Test this under Win 7, too!)
	fontDirectory = wxGetOSDirectory() + _T("\\Fonts\\");

#elif defined __WXGTK__
	//return _T("/usr/share/fonts/dejavu/DejaVuSans.ttf");// Fedora 13
	//return _T("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf");// Ubuntu 11.10

	// Get the normal *nix font directory (FIXME:  Test this!)
	fontDirectory = _T("/usr/share/fonts/");

#else
	// Unknown platform - warn the user
#	warning "Unrecognized platform - unable to locate font files!"
	return wxEmptyString
#endif

	// Grab file names for all .ttf files in that directory
	wxArrayString fontFiles;
	wxDir::GetAllFiles(fontDirectory, &fontFiles, _T("*.ttf"), wxDIR_FILES | wxDIR_DIRS);

	// Check the name of each file against our desired font face name
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
			return false;
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
	
	// Open the font file
	std::ifstream fontStream(fontFile.c_str(), std::ios::in | std::ios::binary);

	// Make sure it opened OK
	if (!fontStream.good())
		return false;

	// Read the offset table
	TT_OFFSET_TABLE ttOffsetTable;
	fontStream.read((char*)&ttOffsetTable, sizeof(TT_OFFSET_TABLE));

	// Rearrange the bytes for the important fields
	SwapEndian(ttOffsetTable.numOfTables);
	SwapEndian(ttOffsetTable.majorVersion);
	SwapEndian(ttOffsetTable.minorVersion);

	// Make sure this is a version 1.0 TrueType Font
	if(ttOffsetTable.majorVersion != 1 || ttOffsetTable.minorVersion != 0)
		return false;

	TT_TABLE_DIRECTORY tblDir;
	tblDir.offset = 0;// To avoid MSVC++ Warning C4701
	bool found = false;
	wxString tempTagString;
	wxString tableName("name");

	int i;
	for (i = 0; i < ttOffsetTable.numOfTables; i++)
	{
		fontStream.read((char*)&tblDir, sizeof(TT_TABLE_DIRECTORY));
		tempTagString.clear();

		// Table's tag is <= 4 characters
		tempTagString.assign(tblDir.tag);
		tempTagString.resize(4);
		if(tempTagString.CmpNoCase(tableName) == 0)
		{
			// Found the table, make sure to swap the bytes for the offset and length
			found = true;
			SwapEndian(tblDir.length);
			SwapEndian(tblDir.offset);
			break;
		}
	}

	// If we didnt' find the name, stop now
	if (!found)
		return false;

	// Go to the offset we found above
	fontStream.seekg(tblDir.offset, std::ios_base::beg);
	TT_NAME_TABLE_HEADER ttNTHeader;
	fontStream.read((char*)&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));

	// Swap the bytes again
	SwapEndian(ttNTHeader.nrCount);
	SwapEndian(ttNTHeader.storageOffset);

	TT_NAME_RECORD ttRecord;
	found = false;

	for (i = 0; i < ttNTHeader.nrCount; i++)
	{
		fontStream.read((char*)&ttRecord, sizeof(TT_NAME_RECORD));
		SwapEndian(ttRecord.nameID);

		// Name ID == 1 indicates font name
		if (ttRecord.nameID == 1)
		{
			SwapEndian(ttRecord.stringLength);
			SwapEndian(ttRecord.stringOffset);

			// Save file position, so we can return to continue with search
			int nPos = fontStream.tellg();
			fontStream.seekg(tblDir.offset + ttRecord.stringOffset +
				ttNTHeader.storageOffset, std::ios_base::beg);

			char *nameBuffer = new char[ttRecord.stringLength];
			fontStream.read(nameBuffer, ttRecord.stringLength);
			fontName.assign(nameBuffer);
			// Apparent bug with setting the string length every time:
			// When the string is empty, and we assign a length anyway, it is no
			// longer emtpy, even though it contains no valid data.  As a workaround,
			// we only assign the proper length if the string is not already empty
			if (!fontName.IsEmpty())
				fontName.resize(ttRecord.stringLength);
			delete [] nameBuffer;

			// Check to make sure the name isn't empty - if it is, continue searching
			if (!fontName.IsEmpty())
				break;

			fontStream.seekg(nPos, std::ios_base::beg);
		}
	}

	if (fontName.Length() > 0)
		return true;

	return false;
}