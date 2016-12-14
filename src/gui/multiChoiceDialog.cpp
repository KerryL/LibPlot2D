/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  multiChoiceDialog.cpp
// Date:  8/2/2012
// Auth:  K. Loux
// Desc:  Multiple choice dialog box.  Intended to function exaclty as a
//        wxMultiChoiceDialog, but with a select all button.

// Standard C++ headers
#include <algorithm>

// wxWidgets headers
#include <wx/statline.h>

// Local headers
#include "lp2d/gui/multiChoiceDialog.h"

namespace LibPlot2D
{

//=============================================================================
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
//=============================================================================
MultiChoiceDialog::MultiChoiceDialog(wxWindow* parent, const wxString& message, const wxString& caption,
		const wxArrayString& choices, long style, const wxPoint& pos,
		wxArrayInt *defaultChoices, bool *removeExisting)
		: wxDialog(parent, wxID_ANY, caption, pos, wxDefaultSize, style)
{
	descriptions = choices;
	shown.resize(choices.Count());
	unsigned int i;
	for (i = 0; i < choices.Count(); ++i)
		shown[i] = true;
	CreateControls(message, choices);
	ApplyDefaults(defaultChoices, removeExisting);
}

//=============================================================================
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
//=============================================================================
BEGIN_EVENT_TABLE(MultiChoiceDialog, wxDialog)
	EVT_BUTTON(idSelectAll,	MultiChoiceDialog::OnSelectAllButton)
	EVT_TEXT(idFilterText, MultiChoiceDialog::OnFilterTextChange)
	EVT_CHECKLISTBOX(wxID_ANY, MultiChoiceDialog::OnCheckListBoxSelection)
END_EVENT_TABLE()

//=============================================================================
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
//=============================================================================
void MultiChoiceDialog::CreateControls(const wxString& message, const wxArrayString& choices)
{
	wxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1);
	topSizer->Add(mainSizer, 1, wxALL | wxEXPAND, 5);
	mainSizer->AddGrowableCol(0, 1);

	wxSizer *headerSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(headerSizer, 1, wxGROW | wxLEFT | wxRIGHT | wxTOP, 10);
	headerSizer->Add(new wxStaticText(this, wxID_ANY, message));
	headerSizer->AddStretchSpacer();
	headerSizer->Add(new wxStaticText(this, wxID_ANY, _T("Filter:")));
	filterText = new wxTextCtrl(this, idFilterText);
	headerSizer->Add(filterText, 0, wxLEFT, 5);

	choiceListBox = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition,
		wxSize(400, ComputeListBoxHeight(choices)), choices, wxLB_ALWAYS_SB);
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

//=============================================================================
// Class:			MultiChoiceDialog
// Function:		ComputeListBoxHeight
//
// Description:		Computes the ideal height of the list box based on the number
//					of choices to display.
//
// Input Arguments:
//		choices	= const wxArrayString&
//
// Output Arguments:
//		None
//
// Return Value:
//		int
//
//=============================================================================
int MultiChoiceDialog::ComputeListBoxHeight(const wxArrayString& choices) const
{
	const double factor(0.6);
	int value(choices.Count() * 20);
	return std::min(std::max(200, value), (int)(wxGetClientDisplayRect().GetHeight() * factor));
}

//=============================================================================
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
//=============================================================================
wxSizer* MultiChoiceDialog::CreateButtons()
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

//=============================================================================
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
//		None
//
//=============================================================================
void MultiChoiceDialog::OnSelectAllButton(wxCommandEvent& WXUNUSED(event))
{
	bool allSelected(true);
	unsigned int i;
	for (i = 0; i < choiceListBox->GetCount(); ++i)
	{
		if (!choiceListBox->IsChecked(i))
		{
			allSelected = false;
			break;
		}
	}

	SetAllChoices(!allSelected);
}

//=============================================================================
// Class:			MultiChoiceDialog
// Function:		OnFilterTextChange
//
// Description:		Event handler for text change in the filter text box.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void MultiChoiceDialog::OnFilterTextChange(wxCommandEvent& WXUNUSED(event))
{
	choiceListBox->Clear();
	wxString filter(filterText->GetValue().Lower());
	unsigned int i;
	for (i = 0; i < shown.size(); ++i)
	{
		if (filter.IsEmpty() || descriptions[i].Lower().Contains(filter))
		{
			choiceListBox->Insert(descriptions[i], choiceListBox->GetCount());
			shown[i] = true;

			choiceListBox->Check(choiceListBox->GetCount() - 1, IsSelected(i));
		}
		else
			shown[i] = false;
	}
}

//=============================================================================
// Class:			MultiChoiceDialog
// Function:		OnCheckListBoxSelection
//
// Description:		Event handler for check list box selection changes.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void MultiChoiceDialog::OnCheckListBoxSelection(wxCommandEvent &event)
{
	UpdateSelectionList(event.GetInt());
}

//=============================================================================
// Class:			MultiChoiceDialog
// Function:		UpdateSelectionList
//
// Description:		Updates the list of selected items making corrections for
//					hidden items.
//
// Input Arguments:
//		index	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void MultiChoiceDialog::UpdateSelectionList(const unsigned int &index)
{
	if (choiceListBox->IsChecked(index) && !IsSelected(GetCorrectedIndex(index)))
		selections.Add(GetCorrectedIndex(index));
	else if (!choiceListBox->IsChecked(index) && IsSelected(GetCorrectedIndex(index)))
		selections.Remove(GetCorrectedIndex(index));
}

//=============================================================================
// Class:			MultiChoiceDialog
// Function:		GetCorrectedIndex
//
// Description:		Corrects the index based on which items are hidden.
//
// Input Arguments:
//		index	= cosnt unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int MultiChoiceDialog::GetCorrectedIndex(const unsigned int &index) const
{
	unsigned int i, trueIndex(0), fakeIndex(0);
	for (i = 0; i < shown.size(); ++i)
	{
		if (shown[i])
		{
			if (index == fakeIndex)
				break;
			++fakeIndex;
		}
		++trueIndex;
	}

	return i;
}

//=============================================================================
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
//=============================================================================
void MultiChoiceDialog::SetAllChoices(const bool &selected)
{
	unsigned int i;
	for (i = 0; i < choiceListBox->GetCount(); ++i)
	{
		choiceListBox->Check(i, selected);
		UpdateSelectionList(i);
	}
}

//=============================================================================
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
//=============================================================================
bool MultiChoiceDialog::RemoveExistingCurves() const
{
	return removeCheckBox->GetValue();
}

//=============================================================================
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
//=============================================================================
void MultiChoiceDialog::ApplyDefaults(wxArrayInt *defaultChoices, bool *removeExisting)
{
	unsigned int i;
	if (defaultChoices && defaultChoices->size() > 0)
	{
		selections = *defaultChoices;
		for (i = 0; i < choiceListBox->GetCount(); ++i)
		{
			choiceListBox->Check(i, false);
			shown[i] = true;
		}

		for (i = 0; i < defaultChoices->Count(); ++i)
			choiceListBox->Check((*defaultChoices)[i], true);
	}

	if (removeExisting)
		removeCheckBox->SetValue(*removeExisting);
}

//=============================================================================
// Class:			MultiChoiceDialog
// Function:		IsSelected
//
// Description:		Checks to see if the specified index is contained in the
//					list of selected items.
//
// Input Arguments:
//		i	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if selected, false otherwise
//
//=============================================================================
bool MultiChoiceDialog::IsSelected(const int &i) const
{
	unsigned int j;
	for (j = 0; j < selections.GetCount(); ++j)
	{
		if (selections[j] == i)
			return true;
	}

	return false;
}

}// namespace LibPlot2D
