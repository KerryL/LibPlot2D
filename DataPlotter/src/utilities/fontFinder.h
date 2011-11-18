/*===================================================================================
                                    DataPlotter
                         Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fontFinder.h
// Created:  11/17/2011
// Author:  K. Loux
// Description:  Cross-platform methods for working with fonts.  Builds on wxWidgets
//				 objects.
// History:

#ifndef _FONT_FINDER_H_
#define _FONT_FINDER_H_

// wxWidgets forward declarations
class wxString;

// Local headers
#include "utilities/machineDefinitions.h"

class FontFinder
{
public:
	// Method for finding the best list on the system when given a list of desired font names
	static bool GetFontFaceName(wxFontEncoding encoding, const wxArrayString &preferredFonts,
		const bool &fixedWidth, wxString &fontName);

	// Method for retrieving the path to a font .ttf file given only the file name
	static wxString GetFontFileName(const wxString &fontName);
	static bool GetPreferredFontFileName(wxFontEncoding encoding,
		const wxArrayString &preferredFonts, const bool &fixedWidth, wxString &fontFile);

	// Method for retrieving the font name from a specific font file
	static bool GetFontName(const wxString &fontFile, wxString &fontName);

private:
	// TTF file header
	struct TT_OFFSET_TABLE
	{
		unsigned short majorVersion;
		unsigned short minorVersion;
		unsigned short numOfTables;
		unsigned short searchRange;
		unsigned short entrySelector;
		unsigned short rangeShift;
	};

	// Tables in TTF file and there placement and name (tag)
	struct TT_TABLE_DIRECTORY
	{
		char tag[4];
		DP_ULONG checkSum;
		DP_ULONG offset;
		DP_ULONG length;
	};

	// Header of names table
	struct TT_NAME_TABLE_HEADER
	{
		unsigned short fSelector;
		unsigned short nrCount;
		unsigned short storageOffset;
	};

	// Record in names table
	struct TT_NAME_RECORD
	{
		unsigned short platformID;
		unsigned short encodingID;
		unsigned short languageID;
		unsigned short nameID;
		unsigned short stringLength;
		unsigned short stringOffset;
	};

	inline static void SwapEndian(unsigned short& x)
	{
		x = (x >> 8) |
			(x << 8);
	}

	inline static void SwapEndian(DP_ULONG& x)
	{
		x = (x >> 24) |
			((x << 8) & 0x00FF0000) |
			((x >> 8) & 0x0000FF00) |
			(x << 24);
	}
};

#endif// _FONT_FINDER_H_