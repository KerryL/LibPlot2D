/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  multiChoiceDialog.h
// Date:  8/2/2012
// Auth:  K. Loux
// Desc:  Multiple choice dialog box.  Intended to function exaclty as a
//        wxMultiChoiceDialog, but with a select all button.

#ifndef MULTI_CHOICE_DIALOG_H_
#define MULTI_CHOICE_DIALOG_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

class MultiChoiceDialog : public wxDialog
{
public:
	// Constructor
	MultiChoiceDialog(wxWindow* parent, const wxString& message,
		const wxString& caption, const wxArrayString& choices,
		long style = wxCHOICEDLG_STYLE, const wxPoint& pos = wxDefaultPosition,
		wxArrayInt *defaultChoices = nullptr, bool *removeExisting = nullptr);

	~MultiChoiceDialog() = default;

	wxArrayInt GetSelections() const { return mSelections; }

	bool RemoveExistingCurves() const;

private:
	void CreateControls(const wxString& message, const wxArrayString& choices);
	wxSizer* CreateButtons();
	int ComputeListBoxHeight(const wxArrayString& choices) const;

	wxCheckListBox *mChoiceListBox;
	wxCheckBox *mRemoveCheckBox;
	wxTextCtrl *mFilterText;

	wxArrayString mDescriptions;
	wxArrayInt mSelections;
	std::vector<bool> mShown;

	unsigned int GetCorrectedIndex(const unsigned int &index) const;
	void UpdateSelectionList(const unsigned int &index);
	bool IsSelected(const int &i) const;

	// Object IDs
	enum EventIDs
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

}// namespace LibPlot2D

#endif// MULTI_CHOICE_DIALOG_H_
