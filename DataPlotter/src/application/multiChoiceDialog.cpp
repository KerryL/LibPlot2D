/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  multiChoiceDialog.cpp
// Created:  8/2/2012
// Author:  K. Loux
// Description:  Multiple choice dialog box.  Intended to function exaclty as a wxMultiChoiceDialog,
//				 but with a select all button.
// History:

// wxWidgets headers
#include <wx/statline.h>

// Local headers
#include "multiChoiceDialog.h"

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		MultiChoiceDialog
//
// Description:		Constructor for MultiChoiceDialog class.
//
// Input Arguments:
//		parent	= wxWindow* pointing to the dialog's parent window
//		message	= const wxString& specifying message (instructions) to user
//		caption	= const wxString& specifying the dialog caption
//		choices	= const wxArrayString& containing list of user choices
//		style	= long specifying dialog style flags
//		pos		= const wxPoint& specifying on-screen position
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MultiChoiceDialog::MultiChoiceDialog(wxWindow* parent, const wxString& message, const wxString& caption,
		const wxArrayString& choices, long style, const wxPoint& pos)
		: wxDialog(parent, wxID_ANY, caption, pos, wxDefaultSize, style)
{
	CreateControls(message, choices);
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		Event Table
//
// Description:		Event table for MultiChoiceDialog class.
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
BEGIN_EVENT_TABLE(MultiChoiceDialog, wxDialog)
	EVT_BUTTON(ID_SELECT_ALL,	MultiChoiceDialog::OnSelectAllButton)
END_EVENT_TABLE()

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		GetSelections
//
// Description:		Returns the user-selected items in the list.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayInt containing a list of checked items
//
//==========================================================================
wxArrayInt MultiChoiceDialog::GetSelections(void) const
{
	wxArrayInt selections;
	unsigned int i;
	for (i = 0; i < choiceListBox->GetCount(); i++)
	{
		if (choiceListBox->IsChecked(i))
			selections.Add(i);
	}

	return selections;
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		CreateControls
//
// Description:		Generates controls for the dialog.
//
// Input Arguments:
//		message	= const wxString& containing user instructions
//		choices	= const wxArrayString& containing list of user choices
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MultiChoiceDialog::CreateControls(const wxString& message, const wxArrayString& choices)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 5);

	wxStaticText *instructions = new wxStaticText(this, wxID_ANY, message);
	mainSizer->Add(instructions, 0, wxALL, 8);

	choiceListBox = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(300, 200), choices, wxLB_ALWAYS_SB);
	mainSizer->Add(choiceListBox, 0, wxALL, 10);

	mainSizer->Add(new wxStaticLine(this), wxSizerFlags().Expand().DoubleBorder(wxLEFT | wxRIGHT));

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *selectAllButton = new wxButton(this, ID_SELECT_ALL, _T("Select All"));
	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"));
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"));

	buttonSizer->Add(selectAllButton, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP, 3);
	buttonSizer->AddStretchSpacer();
	buttonSizer->Add(okButton, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxTOP, 3);
	buttonSizer->Add(cancelButton, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxTOP, 3);

	mainSizer->Add(buttonSizer, 1, wxGROW | wxALL, 5);

	SetSizer(topSizer);
	topSizer->SetSizeHints(this);
	topSizer->Fit(this);

	Center();
	okButton->SetDefault();
	choiceListBox->SetFocus();
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		OnSelectAllButton
//
// Description:		Event handler for Select All button clicks.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayInt containing a list of checked items
//
//==========================================================================
void MultiChoiceDialog::OnSelectAllButton(wxCommandEvent& WXUNUSED(event))
{
	bool allSelected(true);
	unsigned int i;
	for (i = 0; i < choiceListBox->GetCount(); i++)
	{
		if (!choiceListBox->IsChecked(i))
		{
			allSelected = false;
			break;
		}
	}

	for (i = 0; i < choiceListBox->GetCount(); i++)
		choiceListBox->Check(i, !allSelected);
}