/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  customFileFormat.cpp
// Date:  7/17/2012
// Auth:  K. Loux
// Desc:  Object for interpreting user-specified custom file formats from XML
//        file and reading them into Dataset2D objects.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/xml/xml.h>

// Local headers
#include "lp2d/parser/customFileFormat.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			CustomFileFormat
// Function:		Constant Declarations
//
// Description:		Constant declarations for the CustomFileFormat class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
const wxString CustomFileFormat::mCustomFormatsXMLFileName = _T("CustomFormats.xml");
const wxString CustomFileFormat::mCustomFormatsRootName = _T("CUSTOM_FORMATS");
const unsigned long CustomFileFormat::mCustomFormatsVersion = 1;

//=============================================================================
// Class:			CustomFileFormat
// Function:		CustomFileFormat
//
// Description:		Constructor for CustomFileFormat class.
//
// Input Arguments:
//		mPathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString, name of format if a match, wxEmptyString otherwise
//
//=============================================================================
CustomFileFormat::CustomFileFormat(const wxString &mPathAndFileName) : mPathAndFileName(mPathAndFileName)
{
	wxXmlDocument customFormatDefinitions;
	if (!wxFileExists(mCustomFormatsXMLFileName) ||
		!customFormatDefinitions.Load(mCustomFormatsXMLFileName))
	{
		ClearData();
		return;
	}

	if (!CheckRootAndVersion(customFormatDefinitions))
	{
		ClearData();
		return;
	}

	wxXmlNode *format = customFormatDefinitions.GetRoot()->GetChildren();
	while (format)
	{
		if (format->GetName() == _T("FORMAT"))
		{
			if (ReadFormatTag(*format))
				return;
			else
				ClearData();
		}

		format = format->GetNext();
	}
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadFormatTag
//
// Description:		Reads format tag (and children).
//
// Input Arguments:
//		formatNode	= wxXmlNode&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for match, false otherwise
//
//=============================================================================
bool CustomFileFormat::ReadFormatTag(wxXmlNode &formatNode)
{
	mChannels.clear();
	if (!formatNode.GetAttribute(_T("NAME"), &mFormatName))
	{
		wxMessageBox(_T("Ignoring custom file formats:  Each FORMAT tag must have NAME attribute."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	wxString extension;
	if (!formatNode.GetAttribute(_T("EXTENSION"), &extension))
		extension = _T("*");
	unsigned int lastDot = mPathAndFileName.find_last_of(_T("."));
	if (!extension.Cmp(_T("*")) && !mPathAndFileName.Mid(lastDot).CmpNoCase(extension))
		return false;

	Identifier id;
	if (!ProcessFormatChildren(formatNode.GetChildren(), id))
		return false;

	mIsXML = formatNode.GetAttribute(_T("XML"), "FALSE").CmpNoCase("TRUE") == 0;
	if (IsFormat(mPathAndFileName, id))
	{
		mDelimiter = formatNode.GetAttribute(_T("DELIMITER"), wxEmptyString);
		mEndIdentifier = formatNode.GetAttribute(_T("END_IDENTIFIER"), wxEmptyString);
		mTimeUnits = formatNode.GetAttribute(_T("TIME_UNITS"), wxEmptyString);
		mTimeFormat = formatNode.GetAttribute(_T("TIME_FORMAT"), wxEmptyString);
		mAsynchronous = formatNode.GetAttribute(_T("ASYNC"), "FALSE").CmpNoCase("TRUE") == 0;

		wxString timeColumnString(formatNode.GetAttribute(_T("TIME_COLUMN"), _T("0")));
		if (!timeColumnString.ToULong(&mTimeColumn))
		{
			wxMessageBox(_T("Failed to parse time column into integer."), _T("Error Reading Custom Format Definitions"));
			return false;
		}

		wxString startRowString(formatNode.GetAttribute(_T("START_ROW"), _T("0")));
		if (!startRowString.ToULong(&mStartRow))
		{
			wxMessageBox(_T("Failed to parse start row into integer."), _T("Error Reading Custom Format Definitions"));
			return false;
		}

		if (mIsXML && ReadAdditionalXMLProperties(formatNode))
			return true;
		return true;
	}

	return false;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadIdentifierTag
//
// Description:		Reads identifier tag and populates id argument with contents.
//
// Input Arguments:
//		idNode	= wxXmlNode&
//
// Output Arguments:
//		id		= const Identifier&
//
// Return Value:
//		bool, false for error, true otherwise
//
//=============================================================================
bool CustomFileFormat::ReadIdentifierTag(wxXmlNode &idNode, Identifier &id)
{
	wxString temp = idNode.GetAttribute(_T("MATCH_CASE"), _T("1"));
	if (temp.Cmp(_T("1")) == 0)
		id.matchCase = true;
	else
		id.matchCase = false;

	if (!idNode.GetAttribute(_T("LOCATION"), &temp))
	{
		wxMessageBox(_T("Ignoring custom file formats:  Each IDENTIFIER tag must have LOCATION attribute."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}
	else
	{
		if (!ProcessLocationID(temp, id))
		{
			wxMessageBox(_T("Ignoring custom file formats:  LOCATION attributes must have value 'BOF', 'BOL', or 'ROOT'."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}
	}

	id.textToMatch = idNode.GetNodeContent();
	if (id.textToMatch.IsEmpty())
	{
		wxMessageBox(_T("Ignoring custom file formats:  IDENTIFIER contents must not be empty."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	return true;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadChannelTag
//
// Description:		Reads channel tag and pushes channel info into vector.
//
// Input Arguments:
//		channelNode	= wxXmlNode&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, false for error, true otherwise
//
//=============================================================================
bool CustomFileFormat::ReadChannelTag(wxXmlNode &channelNode)
{
	wxString temp;
	Channel channel;
	if (!ReadCodeOrColumn(channelNode, channel))// Must have code or column(>0)
		return false;

	if (!channelNode.GetAttribute(_T("NAME"), &channel.name))
	{
		wxMessageBox(_T("Ignoring channel definition for '") + mFormatName + _T("' format:  NAME must not be empty."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	channel.discardCode = channelNode.GetAttribute(_T("DISCARD_CODE"), "FALSE").CmpNoCase(_T("TRUE")) == 0;
	channel.units = channelNode.GetAttribute(_T("UNITS"), wxEmptyString);
	temp = channelNode.GetAttribute(_T("SCALE"), _T("1"));
	if (!temp.ToDouble(&channel.scale))
	{
		wxMessageBox(_T("Could not set scale for ") + mFormatName + _T(":") + channel.name + _T(".  Using scale = 1."),
			_T("Error Reading Custom Format Definitions"));
		channel.scale = 1.0;
	}

	mChannels.push_back(channel);

	return true;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadAdditionalXMLProperties
//
// Description:		Reads XML-type specific format properties.
//
// Input Arguments:
//		formatNode	= wxXmlNode&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for read OK, false otherwise
//
//=============================================================================
bool CustomFileFormat::ReadAdditionalXMLProperties(wxXmlNode &formatNode)
{
	mXDataNode = formatNode.GetAttribute(_T("XDATA_NODE"), wxEmptyString);
	mXDataKey = formatNode.GetAttribute(_T("XDATA_KEY"), wxEmptyString);
	mYDataNode = formatNode.GetAttribute(_T("YDATA_NODE"), wxEmptyString);
	mYDataKey = formatNode.GetAttribute(_T("YDATA_KEY"), wxEmptyString);
	mChannelParentNode = formatNode.GetAttribute(_T("CHANNEL_PARENT_NODE"), wxEmptyString);
	mChannelNode = formatNode.GetAttribute(_T("CHANNEL_NODE"), wxEmptyString);
	mCodeKey = formatNode.GetAttribute(_T("CODE_KEY"), wxEmptyString);

	// Also check that a mDelimiter was specified - this is required for XML types
	// TODO:  What if instead of a string of values, the format is a new node for each data point?
	if (mDelimiter.IsEmpty())
	{
		wxMessageBox(_T("Delimiter not specified for ") + mFormatName + _T(".  Delimiter specification is required for XML types."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	// It's OK if we don't have an xDataNode or yDataNode - in that case, assume that
	// each channel has it's own x-data and that it's within the channel tag
	return !(mXDataKey.IsEmpty() ||
		mYDataKey.IsEmpty() ||
		mChannelParentNode.IsEmpty() ||
		mChannelNode.IsEmpty() ||
		mCodeKey.IsEmpty());
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		IsFormat
//
// Description:		Determines if the specified file matches the identifier.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//		id				= const Identifier&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for match, false otherwise
//
//=============================================================================
bool CustomFileFormat::IsFormat(const wxString &pathAndFileName, const Identifier &id)
{
	std::ifstream dataFile(pathAndFileName.mb_str(), std::ios::in);
	if (!dataFile.is_open() || !dataFile.good())
	{
		dataFile.close();
		return false;
	}

	bool formatMatches(false);
	wxXmlDocument document;
	switch (id.location)
	{
	case Identifier::Location::BOF:
		formatMatches = MatchNextLine(dataFile, id);
		break;

	case Identifier::Location::BOL:
		formatMatches = MatchSpecifiedLine(dataFile, id);
		break;

	case Identifier::Location::ROOT:
		formatMatches = mIsXML &&
			MatchNextLine(dataFile, _T("<?xml")) &&
			document.Load(mPathAndFileName) &&
			document.GetRoot()->GetName().Cmp(id.textToMatch) == 0;
		break;

	default:
		wxMessageBox(_T("Missing identifier tag for format '") + mFormatName + _T("'."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	dataFile.close();
	return formatMatches;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		MatchNextLine
//
// Description:		Determines if the next line matches the specified identifier.
//
// Input Arguments:
//		inFile	= std::ifstream&
//		id		= const Identifier&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for match, false otherwise
//
//=============================================================================
bool CustomFileFormat::MatchNextLine(std::ifstream &inFile, const Identifier &id) const
{
	return MatchNextLine(inFile, id.textToMatch, id.matchCase);
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		MatchNextLine
//
// Description:		Determines if the next line matches the specified text.
//
// Input Arguments:
//		inFile		= std::ifstream&
//		textTomatch	= const wxString&
//		matchCase	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for match, false otherwise
//
//=============================================================================
bool CustomFileFormat::MatchNextLine(std::ifstream &inFile, const wxString &textToMatch, const bool &matchCase) const
{
	std::string nextLine;
	std::getline(inFile, nextLine);

	if (nextLine.length() < textToMatch.Len())
		return false;

	if (matchCase)
	{
		if (textToMatch.Cmp(nextLine.substr(0, textToMatch.Len())) == 0)
			return true;
	}
	else
	{
		if (textToMatch.CmpNoCase(nextLine.substr(0, textToMatch.Len())) == 0)
			return true;
	}

	return false;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ClearData
//
// Description:		Clears out data (for returning on loading error, for example).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void CustomFileFormat::ClearData()
{
	mFormatName.Empty();
	mDelimiter.Empty();
	mTimeUnits.Empty();

	mXDataNode.Empty();
	mXDataKey.Empty();
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ProcessChannelNames
//
// Description:		Converts "normal" channel names into user-specified
//					custom names and adjust scaling as specified by the user.
//
// Input Arguments:
//		names	= wxArrayString& (also output argument)
//
// Output Arguments:
//		scales	= std::vector<double>
//
// Return Value:
//		None
//
//=============================================================================
void CustomFileFormat::ProcessChannels(wxArrayString &names, std::vector<double> &scales) const
{
	assert(scales.size() == names.size());

	unsigned int i;//, location
	for (i = 0; i < names.size(); ++i)
	{
		for (const auto& channel : mChannels)
		{
			if (channel.code.IsEmpty())
			{
				if (i == static_cast<unsigned int>(channel.column))
				{
					names[i] = channel.name;
					if (!channel.units.IsEmpty())
						names[i].Append(_T(", [") + channel.units + _T("]"));
					scales[i] = channel.scale;
				}
			}
			else if (names[i].Contains(channel.code))
			{
				/*location = names[i].Find(channel.code);
				names[i] = names[i].Mid(0, location) + _T(", ") + channel.name;*/
				if (channel.discardCode)
					names[i].Empty();
				else
					names[i].Append(_T(", "));
				names[i].Append(channel.name);
				if (!channel.units.IsEmpty())
					names[i].Append(_T(", [") + channel.units + _T("]"));
				scales[i] = channel.scale;
			}
		}
	}
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		CheckRootAndVersion
//
// Description:		Checks root node and XML version.
//
// Input Arguments:
//		document	= const wxXmlDocument&, previously opened XML document
//
// Output Arguments:
//		None
//
// Return Value:
//		true if checks are OK, false otherwise
//
//=============================================================================
bool CustomFileFormat::CheckRootAndVersion(const wxXmlDocument &document) const
{
	// Check that the root name matches
	if (document.GetRoot()->GetName().Cmp(mCustomFormatsRootName) != 0)
	{
		wxMessageBox(_T("Ignoring custom file formats:  XML root must be ")
			+ mCustomFormatsRootName + _T("."), _T("Error Reading Custom Format Definitions"));
		return false;
	}

	wxString temp;

	// Check file version
	unsigned long version;
	if (!document.GetRoot()->GetAttribute(_T("VERSION"), &temp))
	{
		wxMessageBox(_T("Ignoring custom file formats:  XML root must contain VERSION attribute."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}
	else
	{
		if (!temp.ToULong(&version))
		{
			wxMessageBox(_T("Ignoring custom file formats:  VERSION value must be an integer."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}

		// Implement any version checks here
	}

	return true;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ProcessLocationID
//
// Description:		Processes specified ID location tag.
//
// Input Arguments:
//		value	= const wxString& specified in the XML file
//
// Output Arguments:
//		id		= Identifier&
//
// Return Value:
//		bool, true if ID was processed sucessfully, false otherwise
//
//=============================================================================
bool CustomFileFormat::ProcessLocationID(const wxString &value, Identifier &id) const
{
	if (value.CmpNoCase(_T("BOF")) == 0)
	{
		id.location = Identifier::Location::BOF;
		id.bolNumber = 0;
		return true;
	}

	if (value.CmpNoCase(_T("BOL")) == 0)
	{
		id.location = Identifier::Location::BOL;

		if (value.Len() > 3)
		{
			if (!value.Mid(3).ToLong(&id.bolNumber))
				id.bolNumber = -1;
		}
		else
			id.bolNumber = -1;

		return true;
	}

	if (value.CmpNoCase(_T("ROOT")) == 0)
	{
		id.location = Identifier::Location::ROOT;
		id.bolNumber = -1;
		return true;
	}

	return false;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		MatchSpecifiedLine
//
// Description:		Attempts to match the file to the specified line text.
//
// Input Arguments:
//		inFile	= std::ifstream&
//		id		= const Identifier&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool true if the line matches, false otherwise
//
//=============================================================================
bool CustomFileFormat::MatchSpecifiedLine(std::ifstream &inFile, const Identifier &id) const
{
	if (id.bolNumber < 0)
	{
		// Check every line for a match
		while (!inFile.eof())
		{
			if (MatchNextLine(inFile, id))
				return true;
		}
	}

	int line(0);
	std::string nextLine;

	while (!inFile.eof() && line < id.bolNumber)
		std::getline(inFile, nextLine);
	return MatchNextLine(inFile, id);
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ProcessFormatChildren
//
// Description:		Processes the child nodes of the format node.
//
// Input Arguments:
//		formatChild	= wxXmlNode* pointing to the format node (parent)
//
// Output Arguments:
//		id			= Identifier&
//
// Return Value:
//		bool, true if nodes are read without errors, false otherwise
//
//=============================================================================
bool CustomFileFormat::ProcessFormatChildren(wxXmlNode *formatChild, Identifier &id)
{
	while (formatChild)
	{
		if (formatChild->GetName().Cmp(_T("IDENTIFIER")) == 0)
		{
			if (!ReadIdentifierTag(*formatChild, id))
				return false;
		}
		else if (formatChild->GetName().Cmp(_T("CHANNEL")) == 0)
		{
			/*if (!ReadChannelTag(*formatChild))
				return false;*/
			// We ignore the return value for now - if a channel has an error, we don't
			// want to prevent reading and using other channel descriptors
			ReadChannelTag(*formatChild);
		}

		formatChild = formatChild->GetNext();
	}

	return true;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadCodeOrColumn
//
// Description:		Reads the information pertaining to CODE or COLUMN tags.
//
// Input Arguments:
//		channelNode	= wxXmlNode& containing CODE or COLUMN tags
//
// Output Arguments:
//		channel	= Channel&
//
// Return Value:
//		bool, true if code/column is successfully read, false otherwise
//
//=============================================================================
bool CustomFileFormat::ReadCodeOrColumn(wxXmlNode &channelNode, Channel &channel) const
{
	if (!channelNode.GetAttribute(_T("CODE"), &channel.code))
		return ReadCode(channelNode, channel);

	return ReadColumn(channelNode, channel);
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadCode
//
// Description:		Reads the information pertaining to CODE tags.
//
// Input Arguments:
//		channelNode	= wxXmlNode& containing CODE or COLUMN tags
//
// Output Arguments:
//		channel	= Channel&
//
// Return Value:
//		bool, true if code is successfully read, false otherwise
//
//=============================================================================
bool CustomFileFormat::ReadCode(wxXmlNode &channelNode, Channel &channel) const
{
	wxString temp;
	if (mIsXML)
	{
		wxMessageBox(_T("Ignoring channel definition for '") + mFormatName + _T("' format:  XML types require that CODE is specified."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}
	else if (!channelNode.GetAttribute(_T("COLUMN"), &temp))
	{
		wxMessageBox(_T("Ignoring channel definition for '") + mFormatName + _T("' format:  CODE or COLUMN must be specified."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}
	else if (!temp.ToLong(&channel.column))
	{
		wxMessageBox(_T("Ignoring channel definition for '") + mFormatName + _T("' format:  COLUMN must have integer value."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}
	else if (channel.column <= 0)
	{
		wxMessageBox(_T("Ignoring channel definition for '") + mFormatName + _T("' format:  COLUMN must be greater than zero."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	return true;
}

//=============================================================================
// Class:			CustomFileFormat
// Function:		ReadColumn
//
// Description:		Reads the information pertaining to COLUMN tags.
//
// Input Arguments:
//		channelNode	= wxXmlNode& containing CODE or COLUMN tags
//
// Output Arguments:
//		channel	= Channel&
//
// Return Value:
//		bool, true if column is successfully read, false otherwise
//
//=============================================================================
bool CustomFileFormat::ReadColumn(wxXmlNode& WXUNUSED(channelNode), Channel &channel) const
{
	if (channel.code.IsEmpty())
	{
		wxMessageBox(_T("Ignoring channel definition for '") + mFormatName + _T("' format:  CODE must not be empty."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	return true;
}

}// namespace LibPlot2D
