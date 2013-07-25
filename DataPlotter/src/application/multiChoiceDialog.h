/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

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

// wxWidgets headers
#include <wx/wx.h>

class MultiChoiceDialog : public wxDialog
{
public:
	// Constructor
	MultiChoiceDialog(wxWindow* parent, const wxString& message, const wxString& caption,
		const wxArrayString& choices, long style = wxCHOICEDLG_STYLE, const wxPoint& pos = wxDefaultPosition);

	virtual  wxArrayInt GetSelections(void) const;

	bool RemoveExistingCurves(void) const;

private:
	void CreateControls(const wxString& message, const wxArrayString& choices);
	wxSizer* CreateButtons(void);

	wxCheckListBox *choiceListBox;
	wxCheckBox *removeCheckBox;

	// Object IDs
	enum
	{
		ID_SELECT_ALL = wxID_HIGHEST + 100
	};

	// Event handlers
	void OnSelectAllButton(wxCommandEvent &event);

	void SetAllChoices(const bool &selected);

	DECLARE_EVENT_TABLE();
};

#endif// _MULTI_CHOICE_DIALOG_H_