/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  dropTarget.cpp
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//        when the user drags-and-drops them onto the main window.

// Local headers
#include "lp2d/gui/dropTarget.h"
#include "lp2d/gui/guiInterface.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			DropTarget
// Function:		DropTarget
//
// Description:		Constructor for DropTarget class.
//
// Input Arguments:
//		guiInterface	= &GuiInterface, reference to main application window
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
DropTarget::DropTarget(GuiInterface &guiInterface)
	: mGuiInterface(guiInterface)
{
	wxDataObjectComposite *dataObject = new wxDataObjectComposite;

	dataObject->Add(new wxFileDataObject);
	dataObject->Add(new wxTextDataObject);

	SetDataObject(dataObject);
}

//=============================================================================
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
//=============================================================================
bool DropTarget::OnDropFiles(const wxArrayString &filenames)
{
	mGuiInterface.LoadFiles(filenames);
	return true;// TODO:  Should I ever return false?
}

//=============================================================================
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
//=============================================================================
bool DropTarget::OnDropText(const wxString& data)
{
	mGuiInterface.LoadText(data);
	return true;// TODO:  Should I ever return false?
}

//=============================================================================
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
//=============================================================================
wxDragResult DropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
	wxDragResult def)
{
	if (!GetData())
		return wxDragNone;

	wxDataObjectComposite *dataObject(
		static_cast<wxDataObjectComposite*>(m_dataObject));
	const size_t bufferSize(
		dataObject->GetDataSize(dataObject->GetReceivedFormat()));

	std::vector<char> buffer(bufferSize);
	if (!dataObject->GetDataHere(
		dataObject->GetReceivedFormat(), buffer.data()))
		return wxDragNone;

	if (dataObject->GetReceivedFormat().GetType() == wxDF_FILENAME)
	{
		wxFileDataObject fileData;
		fileData.SetData(bufferSize, buffer.data());
		return OnDropFiles(fileData.GetFilenames()) ? def : wxDragNone;
	}
	else if (dataObject->GetReceivedFormat().GetType() == wxDF_TEXT)
	{
		wxTextDataObject textData;
		textData.SetData(bufferSize, buffer.data());
		return OnDropText(textData.GetText()) ? wxDragCopy : wxDragNone;
	}

	assert(false);
	return wxDragNone;
}

}// namespace LibPlot2D
