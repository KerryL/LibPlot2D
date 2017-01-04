/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  customFileFormat.h
// Date:  7/17/2012
// Auth:  K. Loux
// Desc:  Object for interpreting user-specified custom file formats from XML
//        file and reading them into Dataset2D objects.

#ifndef CUSTOM_FILE_FORMAT_H_
#define CUSTOM_FILE_FORMAT_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxXmlNode;
class wxXmlDocument;

namespace LibPlot2D
{

/// Class for storing information about interpreting custom (user-specified)
/// file formats.
class CustomFileFormat
{
public:
	/// Constructor.
	///
	/// \param fileName Path and file name of desired file.
	explicit CustomFileFormat(const wxString &pathAndFileName);

	bool IsCustomFormat() const { return !mFormatName.IsEmpty(); }
	bool IsAsynchronous() const { return mAsynchronous; }

	wxString GetDelimiter() const { return mDelimiter; }
	wxString GetTimeUnits() const { return mTimeUnits; }
	wxString GetTimeFormat() const { return mTimeFormat; }
	wxString GetEndIdentifier() const { return mEndIdentifier; }

	bool IsXML() const { return mIsXML; }

	wxString GetXMLXDataNode() const { return mXDataNode; }
	wxString GetXMLXDataKey() const { return mXDataKey; }
	wxString GetXMLYDataNode() const { return mYDataNode; }
	wxString GetXMLYDataKey() const { return mYDataKey; }
	wxString GetXMLChannelParentNode() const { return mChannelParentNode; }
	wxString GetXMLChannelNode() const { return mChannelNode; }
	wxString GetXMLCodeKey() const { return mCodeKey; }

	void ProcessChannels(wxArrayString &names,
		std::vector<double> &scales) const;

	inline static bool CustomDefinitionsExist()
	{ return wxFileExists(mCustomFormatsXMLFileName); }

private:
	static const wxString mCustomFormatsXMLFileName;
	static const wxString mCustomFormatsRootName;
	static const unsigned long mCustomFormatsVersion;

	const wxString mPathAndFileName;

	wxString mFormatName;
	wxString mDelimiter;
	wxString mTimeUnits;
	wxString mTimeFormat;
	wxString mEndIdentifier;

	bool mAsynchronous;
	bool mIsXML;

	wxString mXDataNode;
	wxString mXDataKey;
	wxString mYDataNode;
	wxString mYDataKey;
	wxString mChannelParentNode;
	wxString mChannelNode;
	wxString mCodeKey;

	struct Identifier
	{
		enum class Location
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

	std::vector<Channel> mChannels;

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

#endif// CUSTOM_FILE_FORMAT_H_
