/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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

namespace LibPlot2D
{

class CustomFileFormat
{
public:
	// Constructor
	CustomFileFormat(const wxString &pathAndFileName);

	bool IsCustomFormat() const { return !formatName.IsEmpty(); }
	bool IsAsynchronous() const { return asynchronous; }

	wxString GetDelimiter() const { return delimiter; }
	wxString GetTimeUnits() const { return timeUnits; }
	wxString GetTimeFormat() const { return timeFormat; }
	wxString GetEndIdentifier() const { return endIdentifier; }

	bool IsXML() const { return isXML; };

	wxString GetXMLXDataNode() const { return xDataNode; }
	wxString GetXMLXDataKey() const { return xDataKey; }
	wxString GetXMLYDataNode() const { return yDataNode; }
	wxString GetXMLYDataKey() const { return yDataKey; }
	wxString GetXMLChannelParentNode() const { return channelParentNode; }
	wxString GetXMLChannelNode() const { return channelNode; }
	wxString GetXMLCodeKey() const { return codeKey; }

	void ProcessChannels(wxArrayString &names, std::vector<double> &scales) const;

	inline static bool CustomDefinitionsExist() { return wxFileExists(customFormatsXMLFileName); }

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

	void ClearData();

	bool ProcessLocationID(const wxString &value, Identifier &id) const;
	bool ProcessFormatChildren(wxXmlNode *formatChild, Identifier &id);
	bool MatchSpecifiedLine(std::ifstream &inFile, const Identifier &id) const;

	bool ReadCodeOrColumn(wxXmlNode &channelNode, Channel &channel) const;
	bool ReadCode(wxXmlNode &channelNode, Channel &channel) const;
	bool ReadColumn(wxXmlNode &channelNode, Channel &channel) const;
};

}// namespace LibPlot2D

#endif//_CUSTOM_FILE_FORMAT_H_