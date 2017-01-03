/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  fontFinder.h
// Date:  11/17/2011
// Auth:  K. Loux
// Desc:  Cross-platform methods for working with fonts.  Builds on wxWidgets
//        objects.

#ifndef FONT_FINDER_H_
#define FONT_FINDER_H_

// wxWidgets forward declarations
class wxString;

// Local headers
#include "lp2d/utilities/machineDefinitions.h"

namespace LibPlot2D
{

class FontFinder
{
public:
	// Method for finding the best match on the system when given a list of desired font names
	static bool GetFontFaceName(wxFontEncoding encoding, const wxArrayString &preferredFonts,
		const bool &fixedWidth, wxString &fontName);

	// Method for retrieving the path to a font .ttf file given only the face name
	static wxString GetFontFileName(const wxString &fontName);
	static bool GetPreferredFontFileName(wxFontEncoding encoding,
		const wxArrayString &preferredFonts, const bool &fixedWidth, wxString &fontFile);

	// Method for retrieving the font name from a specific font file
	static bool GetFontName(const wxString &fontFile, wxString &fontName);

	/*enum class StylePreference
	{
		Regular,
		Italics,
		Bold,
		BoldItalics,
		NoPreference
	};*/

private:
	static const unsigned int mFamilyNameRecordId;
	static const unsigned int mSubFamilyNameRecordId;
	static const unsigned int mFullNameRecordId;

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
		uint32_t checkSum;
		uint32_t offset;
		uint32_t length;
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

	struct FontInformation
	{
		wxString fontFamily;
		wxString fontSubFamily;
		wxString fullName;
	};

	static bool RecordIsComplete(const FontInformation& info);

	inline static void SwapEndian(unsigned short& x)
	{
		x = (x >> 8) |
			(x << 8);
	}

	inline static void SwapEndian(uint32_t& x)
	{
		x = (x >> 24) |
			((x << 8) & 0x00FF0000) |
			((x >> 8) & 0x0000FF00) |
			(x << 24);
	}

	static TT_OFFSET_TABLE ReadOffsetTable(std::ifstream &file);
	static bool GetNameTable(std::ifstream &file,
		const TT_OFFSET_TABLE &offsetTable, TT_TABLE_DIRECTORY &table);
	static TT_NAME_TABLE_HEADER GetNameTableHeader(std::ifstream &file,
		const size_t &offset);
	static void CheckHeaderForName(std::ifstream &file, const size_t &offset,
		FontInformation& fontInfo);
};

}// namespace LibPlot2D

#endif// FONT_FINDER_H_
