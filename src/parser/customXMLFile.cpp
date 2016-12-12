/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  customXMLFile.cpp
// Date:  10/4/2012
// Auth:  K. Loux
// Desc:  File class for custom (XML) files defined by the user using an XML file.

// Local headers
#include "lp2d/parser/customXMLFile.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			CustomXMLFile
// Function:		IsType
//
// Desc:		Method for determining if the specified file is this
//					type of file.
//
// Input Arguments:
//		fileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if this object can process the specified file, false otherwise
//
//=============================================================================
bool CustomXMLFile::IsType(const wxString &fileName)
{
	CustomFileFormat format(fileName);
	return format.IsCustomFormat() && format.IsXML();
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		CreateDelimiterList
//
// Desc:		Overload.  Creates the delimiter list.  If no delimiter
//					is specified in the custom definition, this is the same
//					as DataFile, otherwise we limit the delimiter list to
//					contain only the delimiter specified.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString
//
//=============================================================================
wxArrayString CustomXMLFile::CreateDelimiterList() const
{
	if (fileFormat.GetDelimiter().IsEmpty())
		return DataFile::CreateDelimiterList();

	wxArrayString delimiterList;
	delimiterList.Add(fileFormat.GetDelimiter());
	return delimiterList;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		ExtractData
//
// Desc:		Parses the file and reads data into vectors.  Only extracts
//					the data the user selected for display.
//
// Input Arguments:
//		file	= std::ifstream& previously opened input stream to read from
//		choices	= const wxArrayInt& indicating the user's choices
//		factors	= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData	= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool CustomXMLFile::ExtractData(std::ifstream& WXUNUSED(file), const wxArrayInt &choices,
	std::vector<double> *rawData, std::vector<double> &factors, wxString &errorString) const
{
	if (!ExtractXData(rawData, factors, errorString))
		return false;

	wxXmlDocument document(fileName);
	unsigned int channelCount(0), set(1);
	wxXmlNode *node = FollowNodePath(document, fileFormat.GetXMLChannelParentNode());
	if (!node)
	{
		errorString = _T("Could not follow path to channel parent node:  ") + fileFormat.GetXMLChannelParentNode();
		return false;
	}

	node = node->GetChildren();
	while (node)
	{
		if (node->GetName() == fileFormat.GetXMLChannelNode())
		{
			if (!ArrayContainsValue(channelCount++, choices))
			{
				node = node->GetNext();
				continue;
			}

			if (ExtractYData(node, rawData, factors, set, errorString))
				set++;
			else
			{
				return false;
			}
		}
		node = node->GetNext();
	}

	return true;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		ExtractXData
//
// Desc:		Reads the X-data into the rawData array.
//
// Input Arguments:
//		factors		= std::vector<double>& containing the list of scaling factors
//
// Output Arguments:
//		rawData		= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool CustomXMLFile::ExtractXData(std::vector<double> *rawData, std::vector<double> &factors,
	wxString& errorString) const
{
	wxXmlDocument document(fileName);
	wxXmlNode *node = FollowNodePath(document, fileFormat.GetXMLXDataNode());
	if (!node)
	{
		errorString = _T("Could not follow path to x-data node:  ") + fileFormat.GetXMLXDataNode();
		return false;
	}

	wxString data = node->GetAttribute(fileFormat.GetXMLXDataKey(), wxEmptyString);
	if (data.IsEmpty())
	{
		errorString = _T("Could not read x-data!");
		return false;
	}
	if (!DataStringToVector(data, rawData[0], factors[0], errorString))
		return false;

	return true;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		ExtractYData
//
// Desc:		Reads the Y-data into the rawData array.
//
// Input Arguments:
//		channel		= wxXmlNode*
//		factors		= std::vector<double>& containing the list of scaling factors
//		set			= const unsigned int&
//
// Output Arguments:
//		rawData		= std::vector<double>* containing the data
//		errorString	= wxString&
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool CustomXMLFile::ExtractYData(wxXmlNode *channel, std::vector<double> *rawData,
	std::vector<double> &factors, const unsigned int &set, wxString& errorString) const
{
	channel = FollowNodePath(channel, fileFormat.GetXMLYDataNode());
	if (!channel)
	{
		errorString = _T("Could not find y-data node!");
		return false;
	}

	wxString data = channel->GetAttribute(fileFormat.GetXMLYDataKey(), wxEmptyString);
	if (data.IsEmpty())
	{
		errorString = _T("Could not read y-data!");
		return false;
	}
	if (!DataStringToVector(data, rawData[set], factors[set], errorString))
		return false;
	if (rawData[set].size() != rawData[0].size())
	{
		errorString = _T("Number of y-data points differs from number of x-data points!");
		return false;
	}
	return true;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		GetCurveInformation
//
// Desc:		Parses the file and assembles descriptions for each column
//					based on the contents of the header rows.  Also reports
//					the number of header rows back to the calling function.
//					If custom descriptions and scaling factors are specified,
//					these are used in place of the defaults.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		headerLineCount	= unsigned int& number of lines making up this file's header
//		factors			= std::vector<double>& containing scaling factors
//		nonNumericColumns	= wxArrayInt&
//
// Return Value:
//		wxArrayString containing the descriptions
//
//=============================================================================
wxArrayString CustomXMLFile::GetCurveInformation(unsigned int &headerLineCount,
	std::vector<double> &factors, wxArrayInt &/*nonNumericColumns*/) const
{
	wxArrayString names;
	headerLineCount = 0;// Unused for XML types

	wxXmlDocument document(fileName);
	wxXmlNode *channel, *channelParent = FollowNodePath(document, fileFormat.GetXMLChannelParentNode());
	if (!channelParent)
	{
		wxMessageBox(_T("Could not follow path to channel nodes:  ") + fileFormat.GetXMLChannelParentNode(),
			_T("Error Reading File"), wxICON_ERROR);
		return names;
	}

	if (!fileFormat.GetTimeUnits().IsEmpty())
		names.Add(_T("Time, [") + fileFormat.GetTimeUnits() + _T("]"));
	else
		names.Add(_T("Time"));

	channel = channelParent->GetChildren();
	while (channel)
	{
		if (channel->GetName() == fileFormat.GetXMLChannelNode())
			names.Add(channel->GetAttribute(fileFormat.GetXMLCodeKey(), _T("Unnamed Channel")));
		channel = channel->GetNext();
	}

	factors.resize(names.size(), 1.0);
	fileFormat.ProcessChannels(names, factors);

	if (!fileFormat.GetTimeUnits().IsEmpty())
		names[0] = _T("Time, [") + fileFormat.GetTimeUnits() + _T("]");

	return names;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		FollowNodePath
//
// Desc:		Follows the specified node path and returns the final node.
//
// Input Arguments:
//		document	= const wxXmlDocument&
//		path		= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxXmlNode*
//
//=============================================================================
wxXmlNode* CustomXMLFile::FollowNodePath(const wxXmlDocument &document, const wxString &path) const
{
	return FollowNodePath(document.GetRoot(), path);
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		FollowNodePath
//
// Desc:		Follows the specified node path and returns the final node.
//
// Input Arguments:
//		node	= const wxXmlNode*
//		path	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxXmlNode*
//
//=============================================================================
wxXmlNode* CustomXMLFile::FollowNodePath(wxXmlNode *node, const wxString &path) const
{
	node = node->GetChildren();
	wxArrayString nodeStrings(SeparateNodes(path));
	unsigned int i(0);

	while (node && i < nodeStrings.Count())
	{
		if (node->GetName() == nodeStrings[i])
		{
			i++;
			if (i < nodeStrings.Count())
				node = node->GetChildren();
		}
		else
			node = node->GetNext();
	}

	return node;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		SeparateNodes
//
// Desc:		Takes a node path string (slash-separated) and chops it
//					into separate strings for each XML node along the path.
//
// Input Arguments:
//		nodePath	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString
//
//=============================================================================
wxArrayString CustomXMLFile::SeparateNodes(const wxString &nodePath) const
{
	wxArrayString nodes;
	int start(0), end(0);

	while (end != wxNOT_FOUND)
	{
		end = nodePath.Mid(start).Find(_T("/"));
		if (end == wxNOT_FOUND)
			end = nodePath.Mid(start).Find(_T("\\"));
		nodes.Add(nodePath.Mid(start, end));

		start += end + 1;
	}

	return nodes;
}

//=============================================================================
// Class:			CustomXMLFile
// Function:		DataStringToVector
//
// Desc:		Parses the contents of the string and separates discrete
//					data values.  Copies these values into the specified vector.
//
// Input Arguments:
//		data		= const wxString&
//		dataVector	= const std::vector<double>&
//		factor		= const double&
//
// Output Arguments:
//		errorString&	= wxString
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool CustomXMLFile::DataStringToVector(const wxString &data,
	std::vector<double> &dataVector, const double &factor, wxString& errorString) const
{
	unsigned int i;
	double value;
	wxArrayString parsed(ParseLineIntoColumns(data, fileFormat.GetDelimiter()));
	for (i = 0; i < parsed.Count(); i++)
	{
		if (!parsed[i].ToDouble(&value))
		{
			errorString = _T("Error processing XML Data");
			return false;
		}
		dataVector.push_back(value * factor);
	}

	return true;
}

}// namespace LibPlot2D
