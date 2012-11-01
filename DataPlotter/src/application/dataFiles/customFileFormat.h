/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  customFileFormat.h
// Created:  7/17/2012
// Author:  K. Loux
// Description:  Object for interpreting user-specified custom file formats from XML
//				 file and reading them into Dataset2D objects.
// History:

#ifndef _CUSTOM_FILE_FORMAT_H_
#define _CUSTOM_FILE_FORMAT_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxXmlNode;
class wxXmlDocument;

class CustomFileFormat
{
public:
	// Constructor
	CustomFileFormat(const wxString &_pathAndFileName);

	bool IsCustomFormat(void) const { return !formatName.IsEmpty(); };
	bool IsAsynchronous(void) const { return asynchronous; };

	wxString GetDelimiter(void) const { return delimiter; };
	wxString GetTimeUnits(void) const { return timeUnits; };
	wxString GetTimeFormat(void) const { return timeFormat; };
	wxString GetEndIdentifier(void) const { return endIdentifier; };

	bool IsXML(void) const { return isXML; };

	wxString GetXMLXDataNode(void) const { return xDataNode; };
	wxString GetXMLXDataKey(void) const { return xDataKey; };
	wxString GetXMLYDataNode(void) const { return yDataNode; };
	wxString GetXMLYDataKey(void) const { return yDataKey; };
	wxString GetXMLChannelParentNode(void) const { return channelParentNode; };
	wxString GetXMLChannelNode(void) const { return channelNode; };
	wxString GetXMLCodeKey(void) const { return codeKey; };

	void ProcessChannels(wxArrayString &names, std::vector<double> &scales) const;

	inline static bool CustomDefinitionsExist(void) { return wxFileExists(customFormatsXMLFileName); };

private:
	static const wxString customFormatsXMLFileName;
	static const wxString customFormatsRootName;
	static const unsigned long customFormatsVersion;

	const wxString pathAndFileName;

	wxString formatName;
	wxString delimiter;
	wxString timeUnits;
	wxString timeFormat;
	wxString endIdentifier;

	bool asynchronous;
	bool isXML;

	wxString xDataNode;
	wxString xDataKey;
	wxString yDataNode;
	wxString yDataKey;
	wxString channelParentNode;
	wxString channelNode;
	wxString codeKey;

	struct Identifier
	{
		enum Location
		{
			BOF,	// beginning of file
			BOL,	// beginning of any pre-data line
			ROOT	// XML types only (match root node)
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

		bool discardCode;
	};

	std::vector<Channel> channels;

	bool CheckRootAndVersion(const wxXmlDocument &document) const;

	bool IsFormat(const wxString &pathAndFileName, const Identifier &id);
	bool MatchNextLine(std::ifstream &inFile, const Identifier &id) const;
	bool MatchNextLine(std::ifstream &inFile, const wxString &textToMatch,
		const bool &matchCase = false) const;

	bool ReadFormatTag(wxXmlNode &formatNode);
	bool ReadIdentifierTag(wxXmlNode &idNode, Identifier &id);
	bool ReadChannelTag(wxXmlNode &channelNode);
	bool ReadAdditionalXMLProperties(wxXmlNode &formatNode);

	void ClearData(void);

	bool ProcessLocationID(const wxString &value, Identifier &id) const;
	bool ProcessFormatChildren(wxXmlNode *formatChild, Identifier &id);
	bool MatchSpecifiedLine(std::ifstream &inFile, const Identifier &id) const;

	bool ReadCodeOrColumn(wxXmlNode &channelNode, Channel &channel) const;
	bool ReadCode(wxXmlNode &channelNode, Channel &channel) const;
	bool ReadColumn(wxXmlNode &channelNode, Channel &channel) const;
};

#endif//_CUSTOM_FILE_FORMAT_H_