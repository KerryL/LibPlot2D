/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  multiChoiceDialog.h
// Created:  8/2/2012
// Author:  K. Loux
// Description:  Multiple choice dialog box.  Intended to function exaclty as a wxMultiChoiceDialog,
//				 but with a select all button.
// History:

#ifndef _MULTI_CHOICE_DIALOG_H_
#define _MULTI_CHOICE_DIALOG_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

class MultiChoiceDialog : public wxDialog
{
public:
	// Constructor
	MultiChoiceDialog(wxWindow* parent, const wxString& message, const wxString& caption,
		const wxArrayString& choices, long style = wxCHOICEDLG_STYLE, const wxPoint& pos = wxDefaultPosition,
		wxArrayInt *defaultChoices = NULL, bool *removeExisting = NULL);

	virtual wxArrayInt GetSelections() const { return selections; };

	bool RemoveExistingCurves() const;

private:
	void CreateControls(const wxString& message, const wxArrayString& choices);
	wxSizer* CreateButtons();
	int ComputeListBoxHeight(const wxArrayString& choices) const;

	wxCheckListBox *choiceListBox;
	wxCheckBox *removeCheckBox;
	wxTextCtrl *filterText;

	wxArrayString descriptions;
	wxArrayInt selections;
	std::vector<bool> shown;
	unsigned int GetCorrectedIndex(const unsigned int &index) const;
	void UpdateSelectionList(const unsigned int &index);
	bool IsSelected(const int &i) const;

	// Object IDs
	enum
	{
		idSelectAll = wxID_HIGHEST + 300,
		idFilterText
	};

	// Event handlers
	void OnSelectAllButton(wxCommandEvent &event);
	void OnFilterTextChange(wxCommandEvent &event);
	void OnCheckListBoxSelection(wxCommandEvent &event);

	void SetAllChoices(const bool &selected);
	void ApplyDefaults(wxArrayInt *defaultChoices, bool *removeExisting);

	DECLARE_EVENT_TABLE();
};

#endif// _MULTI_CHOICE_DIALOG_H_
