/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  customFileFormat.h
// Created:  7/17/2012
// Author:  K. Loux
// Description:  Object for interpreting user-specified custom file formats from XML
//				 file and reading them into Dataset2D objects.
// History:

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxXmlNode;

class CustomFileFormat
{
public:
	// Constructor
	CustomFileFormat(const wxString &_pathAndFileName);

	bool IsCustomFormat(void) { return !formatName.IsEmpty(); };

	wxString GetDelimiter(void) { return delimiter; };
	wxString GetTimeUnits(void) { return timeUnits; };

	void ProcessChannels(wxArrayString &names, std::vector<double> &scales);

private:
	static const wxString customFormatsXMLFileName;
	static const wxString customFormatsRootName;
	static const unsigned long customFormatsVersion;

	const wxString pathAndFileName;

	wxString formatName;
	wxString delimiter;
	wxString timeUnits;

	struct Identifier
	{
		enum Location
		{
			BOF,	// beginning of file
			BOL		// beginning of any pre-data line
		} location;

		long bolNumber;

		wxString textToMatch;

		bool matchCase;
	};

	struct Channel
	{
		wxString code;
		wxString name;
		wxString units;

		long column;

		double scale;
	};

	std::vector<Channel> channels;

	bool IsFormat(const wxString &pathAndFileName, const Identifier &id);
	bool MatchNextLine(std::ifstream &inFile, const Identifier &id);

	bool ReadFormatTag(wxXmlNode &formatNode);
	bool ReadIdentifierTag(wxXmlNode &idNode, Identifier &id);
	bool ReadChannelTag(wxXmlNode &channelNode);

	void ClearData(void);
};