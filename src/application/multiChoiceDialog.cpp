/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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
//		defaultChoices	= wxArrayInt*, default choices
//		removeExisting	= bool*, default value of remove existing curves checkbox
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MultiChoiceDialog::MultiChoiceDialog(wxWindow* parent, const wxString& message, const wxString& caption,
		const wxArrayString& choices, long style, const wxPoint& pos,
		wxArrayInt *defaultChoices, bool *removeExisting)
		: wxDialog(parent, wxID_ANY, caption, pos, wxDefaultSize, style)
{
	CreateControls(message, choices);

	ApplyDefaults(defaultChoices, removeExisting);
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
	EVT_BUTTON(idSelectAll,	MultiChoiceDialog::OnSelectAllButton)
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
	topSizer->Add(mainSizer, 1, wxALL | wxEXPAND, 5);

	wxStaticText *instructions = new wxStaticText(this, wxID_ANY, message);
	mainSizer->Add(instructions, 0, wxALL, 8);

	choiceListBox = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition,
		wxSize(300, 200), choices, wxLB_ALWAYS_SB);
	SetAllChoices(true);
	mainSizer->Add(choiceListBox, 1, wxALL | wxEXPAND, 10);

	removeCheckBox = new wxCheckBox(this, wxID_ANY, _T("Remove Existing Curves"));
	mainSizer->Add(removeCheckBox, 0, wxALL & ~wxTOP, 10);
	removeCheckBox->SetValue(true);

	mainSizer->Add(new wxStaticLine(this), wxSizerFlags().Expand().DoubleBorder(wxLEFT | wxRIGHT));
	mainSizer->Add(CreateButtons(), 0, wxALL | wxEXPAND, 5);

	SetSizer(topSizer);
	topSizer->SetSizeHints(this);
	topSizer->Fit(this);

	Center();
	choiceListBox->SetFocus();
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		CreateButtons
//
// Description:		Creates the buttons.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* MultiChoiceDialog::CreateButtons(void)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *selectAllButton = new wxButton(this, idSelectAll, _T("Select All"));
	sizer->Add(selectAllButton, 0, wxALIGN_LEFT);
	sizer->AddStretchSpacer();

	wxSizer *okCancel = CreateButtonSizer(wxOK | wxCANCEL);
	if (okCancel)
		sizer->Add(okCancel);

	return sizer;
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

	SetAllChoices(!allSelected);
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		SetAllChoices
//
// Description:		Sets all choices to the specified value.
//
// Input Arguments:
//		selected	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MultiChoiceDialog::SetAllChoices(const bool &selected)
{
	unsigned int i;
	for (i = 0; i < choiceListBox->GetCount(); i++)
		choiceListBox->Check(i, selected);
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		RemoveExistingCurves
//
// Description:		Returns the value of the "Remove Existing Curves" checkbox.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool MultiChoiceDialog::RemoveExistingCurves(void) const
{
	return removeCheckBox->GetValue();
}

//==========================================================================
// Class:			MultiChoiceDialog
// Function:		ApplyDefaults
//
// Description:		Applies the specified defaults to the available user selections.
//
// Input Arguments:
//		defaultChoices	= wxArrayInt*
//		removeExisting	= bool*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MultiChoiceDialog::ApplyDefaults(wxArrayInt *defaultChoices, bool *removeExisting)
{
	if (defaultChoices)
	{
		unsigned int i;
		for (i = 0; i < choiceListBox->GetCount(); i++)
			choiceListBox->Check(i, false);

		for (i = 0; i < defaultChoices->Count(); i++)
			choiceListBox->Check((*defaultChoices)[i], true);
	}

	if (removeExisting)
		removeCheckBox->SetValue(*removeExisting);
}
