/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  customFileFormat.cpp
// Created:  7/17/2012
// Author:  K. Loux
// Description:  Object for interpreting user-specified custom file formats from XML
//				 file and reading them into Dataset2D objects.
// History:

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/xml/xml.h>

// Local headers
#include "application/customFileFormat.h"

//==========================================================================
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
//==========================================================================
const wxString CustomFileFormat::customFormatsXMLFileName = _T("CustomFormats.xml");
const wxString CustomFileFormat::customFormatsRootName = _T("CUSTOM_FORMATS");
const unsigned long CustomFileFormat::customFormatsVersion = 1;

//==========================================================================
// Class:			CustomFileFormat
// Function:		CustomFileFormat
//
// Description:		Constructor for CustomFileFormat class.
//
// Input Arguments:
//		_pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString, name of format if a match, wxEmptyString otherwise
//
//==========================================================================
CustomFileFormat::CustomFileFormat(const wxString &_pathAndFileName) : pathAndFileName(_pathAndFileName)
{
	// Load the XML file
	wxXmlDocument customFormatDefinitions;
	if (!customFormatDefinitions.Load(customFormatsXMLFileName))
	{
		// Load method above will warn user - no need for explicit warning here
		ClearData();
		return;
	}

	// Check that the root name matches
	if (customFormatDefinitions.GetRoot()->GetName().Cmp(customFormatsRootName) != 0)
	{
		::wxMessageBox(_T("Ignoring custom file formats:  XML root must be ")
			+ customFormatsRootName + _T("."), _T("Error Reading Custom Format Definitions"));
		ClearData();
		return;
	}

	wxString temp;

	// Check file version
	unsigned long version;
	if (!customFormatDefinitions.GetRoot()->GetPropVal(_T("VERSION"), &temp))
	{
		::wxMessageBox(_T("Ignoring custom file formats:  XML root must contain VERSION attribute."),
			_T("Error Reading Custom Format Definitions"));
		ClearData();
		return;
	}
	else
	{
		if (!temp.ToULong(&version))
		{
			::wxMessageBox(_T("Ignoring custom file formats:  VERSION value must be an integer."),
				_T("Error Reading Custom Format Definitions"));
			ClearData();
			return;
		}

		// FIXME:  Any version checks necessary?
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

//==========================================================================
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
//==========================================================================
bool CustomFileFormat::ReadFormatTag(wxXmlNode &formatNode)
{
	channels.empty();

	if (!formatNode.GetPropVal(_T("NAME"), &formatName))
	{
		::wxMessageBox(_T("Ignoring custom file formats:  Each FORMAT tag must have NAME attribute."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	wxString extension;
	if (!formatNode.GetPropVal(_T("EXTENSION"), &extension))
		extension = _T("*");
	// FIXME:  Make sure that our extension matches one of the ones in the list
	if (false)
		return false;

	Identifier id;
	wxXmlNode *formatChild = formatNode.GetChildren();
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

	if (IsFormat(pathAndFileName, id))
	{
		delimiter = formatNode.GetPropVal(_T("DELIMITER"), wxEmptyString);
		timeUnits = formatNode.GetPropVal(_T("TIME_UNITS"), wxEmptyString);
		return true;
	}

	return false;
}

//==========================================================================
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
//==========================================================================
bool CustomFileFormat::ReadIdentifierTag(wxXmlNode &idNode, Identifier &id)
{
	wxString temp = idNode.GetPropVal(_T("MATCH_CASE"), _T("1"));
	if (temp.Cmp(_T("1")) == 0)
		id.matchCase = true;
	else
		id.matchCase = false;

	if (!idNode.GetPropVal(_T("LOCATION"), &temp))
	{
		::wxMessageBox(_T("Ignoring custom file formats:  Each IDENTIFIER tag must have LOCATION attribute."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}
	else
	{
		if (temp.CmpNoCase(_T("BOF")) == 0)
		{
			id.location = Identifier::BOF;
			id.bolNumber = 0;
		}
		else if (temp.Mid(0, 3).CmpNoCase(_T("BOL")) == 0)
		{
			id.location = Identifier::BOL;

			if (temp.Len() > 3)
			{
				if (!temp.Mid(3).ToLong(&id.bolNumber))
					id.bolNumber = -1;
			}
			else
				id.bolNumber = -1;
		}
		else
		{
			::wxMessageBox(_T("Ignoring custom file formats:  LOCATION attributes must have value 'BOF' or 'BOL'."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}
	}

	id.textToMatch = idNode.GetNodeContent();
	if (id.textToMatch.IsEmpty())
	{
		::wxMessageBox(_T("Ignoring custom file formats:  IDENTIFIER contents must not be empty."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	return true;
}

//==========================================================================
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
//==========================================================================
bool CustomFileFormat::ReadChannelTag(wxXmlNode &channelNode)
{
	// Must have either column (<0) or code
	wxString temp;
	Channel channel;
	if (!channelNode.GetPropVal(_T("CODE"), &channel.code))
	{
		if (!channelNode.GetPropVal(_T("COLUMN"), &temp))
		{
			::wxMessageBox(_T("Ignoring channel definition for '") + formatName + _T("' format:  CODE or COLUMN must be specified."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}
		else if (!temp.ToLong(&channel.column))
		{
			::wxMessageBox(_T("Ignoring channel definition for '") + formatName + _T("' format:  COLUMN must have integer value."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}
		else if (channel.column <= 0)
		{
			::wxMessageBox(_T("Ignoring channel definition for '") + formatName + _T("' format:  COLUMN must be greater than zero."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}
	}
	else
	{
		if (channel.code.IsEmpty())
		{
			::wxMessageBox(_T("Ignoring channel definition for '") + formatName + _T("' format:  CODE must not be empty."),
				_T("Error Reading Custom Format Definitions"));
			return false;
		}
	}

	if (!channelNode.GetPropVal(_T("NAME"), &channel.name))
	{
		::wxMessageBox(_T("Ignoring channel definition for '") + formatName + _T("' format:  NAME must not be empty."),
			_T("Error Reading Custom Format Definitions"));
		return false;
	}

	channel.units = channelNode.GetPropVal(_T("UNITS"), wxEmptyString);
	temp = channelNode.GetPropVal(_T("SCALE"), _T("1"));
	if (!temp.ToDouble(&channel.scale))
	{
		::wxMessageBox(_T("Could not set scale for ") + formatName + _T(":") + channel.name + _T(".  Using scale = 1."),
			_T("Error Reading Custom Format Definitions"));
		channel.scale = 1.0;
	}

	channels.push_back(channel);

	return true;
}

//==========================================================================
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
//==========================================================================
bool CustomFileFormat::IsFormat(const wxString &pathAndFileName, const Identifier &id)
{
	std::ifstream dataFile(pathAndFileName.c_str(), std::ios::in);
	if (!dataFile.is_open() || !dataFile.good())
	{
		dataFile.close();
		return false;
	}

	int line(0);
	std::string nextLine;

	switch (id.location)
	{
	case Identifier::BOF:
		// Must match this line
		return MatchNextLine(dataFile, id);
		break;

	case Identifier::BOL:
		if (id.bolNumber < 0)
		{
			// Check every line for a match
			while (!dataFile.eof())
			{
				if (MatchNextLine(dataFile, id))
					return true;
			}
		}
		else
		{
			// Go to the nth line and check
			while (!dataFile.eof() && line < id.bolNumber)
				std::getline(dataFile, nextLine);
			return MatchNextLine(dataFile, id);
		}
		break;
	}

	return false;
}

//==========================================================================
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
//==========================================================================
bool CustomFileFormat::MatchNextLine(std::ifstream &inFile, const Identifier &id)
{
	std::string nextLine;
	std::getline(inFile, nextLine);
	if (id.matchCase)
	{
		if (id.textToMatch.Cmp(nextLine.substr(0, id.textToMatch.Len())) == 0)
			return true;
	}
	else
	{
		if (id.textToMatch.CmpNoCase(nextLine.substr(0, id.textToMatch.Len())) == 0)
			return true;
	}

	return false;
}

//==========================================================================
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
//==========================================================================
void CustomFileFormat::ClearData(void)
{
	formatName.Empty();
	delimiter.Empty();
	timeUnits.Empty();
}

//==========================================================================
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
//==========================================================================
void CustomFileFormat::ProcessChannels(wxArrayString &names, std::vector<double> &scales)
{
	// Check each name against each channel definition (or until we find a match)
	unsigned int i, j;//, location;
	for (i = 0; i < names.size(); i++)
	{
		for (j = 0; j < channels.size(); j++)
		{
			// Check for a column match
			if (channels[j].code.IsEmpty())
			{
				if (i == (unsigned int)channels[j].column)
				{
					names[i] = channels[j].name;
					if (!channels[j].units.IsEmpty())
						names[i].Append(_T(", [") + channels[j].units + _T("]"));
					scales[i] = channels[j].scale;
				}
			}
			// Check for a code match
			else if (names[i].Contains(channels[j].code))
			{
				/*location = names[i].Find(channels[j].code);
				names[i] = names[i].Mid(0, location) + _T(", ") + channels[j].name;*/
				names[i].Append(_T(", ") + channels[j].name);
				if (!channels[j].units.IsEmpty())
					names[i].Append(_T(", [") + channels[j].units + _T("]"));
				scales[i] = channels[j].scale;
			}
		}
	}
}