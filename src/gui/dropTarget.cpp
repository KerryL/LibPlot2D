/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  dropTarget.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//				 when the user drags-and-drops them onto the main window.
// History:

// Local headers
#include "lp2d/gui/dropTarget.h"

namespace LibPlot2D
{

//==========================================================================
// Class:			DropTarget
// Function:		DropTarget
//
// Description:		Constructor for DropTarget class.
//
// Input Arguments:
//		_mainFrame	= &MainFrame, reference to main application window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
DropTarget::DropTarget(MainFrame &_mainFrame) : mainFrame(_mainFrame)
{
	wxDataObjectComposite *dataObject = new wxDataObjectComposite;

	dataObject->Add(new wxFileDataObject);
	dataObject->Add(new wxTextDataObject);

	SetDataObject(dataObject);

	buffer = NULL;
}

//==========================================================================
// Class:			DropTarget
// Function:		~DropTarget
//
// Description:		Destructor for DropTarget class.
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
DropTarget::~DropTarget()
{
	ClearBuffer();
}

//==========================================================================
// Class:			DropTarget
// Function:		OnDropFiles
//
// Description:		Handles dragging and dropping of files.
//
// Input Arguments:
//		filenames	= const &wxArrayString containing the list of filenames
//					  being dropped
//
// Output Arguments:
//		None
//
// Return Value:
//		true to accept the data, false to veto
//
//==========================================================================
bool DropTarget::OnDropFiles(const wxArrayString &filenames)
{
	mainFrame.LoadFiles(filenames);
	return true;
}

//==========================================================================
// Class:			DropTarget
// Function:		OnDropText
//
// Description:		Handles dragging and dropping text.
//
// Input Arguments:
//		data	= const &wxString containing the text being dropped
//
// Output Arguments:
//		None
//
// Return Value:
//		true to accept the data, false to veto
//
//==========================================================================
bool DropTarget::OnDropText(const wxString& data)
{
	mainFrame.LoadText(data);

	return true;
}

//==========================================================================
// Class:			DropTarget
// Function:		OnData
//
// Description:		Overloaded virtual method from wxTextDropTarget.
//
// Input Arguments:
//		x		= wxCoord (unused)
//		y		= wxCoord (unused)
//		def		= wxDragResult
//
// Output Arguments:
//		None
//
// Return Value:
//		wxDragResult
//
//==========================================================================
wxDragResult DropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def)
{
	if (!GetData())
		return wxDragNone;

	ClearBuffer();

	wxDataObjectComposite *dataObject = static_cast<wxDataObjectComposite*>(m_dataObject);
	size_t bufferSize = dataObject->GetDataSize(dataObject->GetReceivedFormat());

	buffer = new char[bufferSize];
	if (!dataObject->GetDataHere(dataObject->GetReceivedFormat(), buffer))
		return wxDragNone;

	if (dataObject->GetReceivedFormat().GetType() == wxDF_FILENAME)
	{
		wxFileDataObject fileData;
		fileData.SetData(bufferSize, buffer);
		return OnDropFiles(fileData.GetFilenames()) ? def : wxDragNone;
	}
	else if (dataObject->GetReceivedFormat().GetType() == wxDF_TEXT)
	{
		wxTextDataObject textData;
		textData.SetData(bufferSize, buffer);
		return OnDropText(textData.GetText()) ? wxDragCopy : wxDragNone;
	}

	assert(false);
	return wxDragNone;
}

//==========================================================================
// Class:			DropTarget
// Function:		ClearBuffer
//
// Description:		Safely deletes the buffer contents.
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
void DropTarget::ClearBuffer()
{
	if (buffer)
		delete[] buffer;
	buffer = NULL;
}

}// namespace LibPlot2D
