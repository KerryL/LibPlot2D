/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  textInputDialog.cpp
// Created:  5/20/2013
// Author:  K. Loux
// Description:  Dialog box similar to ::wxGetTextFromUser() but allows differentiation between
//				 canceling and returning an empty string.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "application/textInputDialog.h"

//==========================================================================
// Class:			TextInputDialog
// Function:		TextInputDialog
//
// Description:		Constructor for TextInputDialog class.
//
// Input Arguments:
//		message		= const wxString&, propmt to display for user
//		title		= const wxString&, dialog title
//		defaultText	= const wxString&, default input text
//		parent		= const wxWindow*, pointing to owner
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TextInputDialog::TextInputDialog(const wxString &message, const wxString &title,
	const wxString &defaultText, wxWindow *parent) : wxDialog(parent, wxID_ANY, title)
{
	CreateControls(message, defaultText);
}

//==========================================================================
// Class:			TextInputDialog
// Function:		~TextInputDialog
//
// Description:		Destructor for TextInputDialog class.
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
TextInputDialog::~TextInputDialog()
{
}

//==========================================================================
// Class:			TextInputDialog
// Function:		CreateControls
//
// Description:		Constructor for TextInputDialog class.
//
// Input Arguments:
//		message		= const wxString&, propmt to display for user
//		defaultText	= const wxString&, default input text
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TextInputDialog::CreateControls(const wxString &message, const wxString &defaultText)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 8);

	mainSizer->Add(new wxStaticText(this, wxID_ANY, message), 0, wxALL, 5);
	text = new wxTextCtrl(this, wxID_ANY, defaultText);
	mainSizer->Add(text, 1, wxGROW | wxALL, 5);
	mainSizer->AddSpacer(10);

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	SetSizerAndFit(topSizer);
	Center();

	text->SetFocus();
}