/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  frfDialog.h
// Created:  8/15/2012
// Author:  K. Loux
// Description:  Dialog for selection of frequency response function data.
// History:

#ifndef _FRF_DIALOG_H_
#define _FRF_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

class FRFDialog : public wxDialog
{
public:
	// Constructors
	FRFDialog(wxWindow *parent, const wxArrayString &descriptions);

	unsigned int GetInputIndex(void) const;
	unsigned int GetOutputIndex(void) const;
	unsigned int GetNumberOfAverages(void) const;

	bool GetComputePhase(void) const;
	bool GetComputeCoherence(void) const;
	bool GetModuloPhase(void) const;

private:
	void CreateControls(const wxArrayString &descriptions);
	wxSizer *CreateSelectionControls(const wxArrayString &descriptions);
	wxSizer *CreateTextBox(void);
	wxSizer *CreateCheckBoxes(void);

	wxListBox *inputList;
	wxListBox *outputList;

	wxCheckBox *phaseCheckBox;
	wxCheckBox *coherenceCheckBox;
	wxCheckBox *moduloPhaseCheckBox;

	wxTextCtrl *averagesTextBox;

	virtual bool TransferDataFromWindow(void);
};

#endif// _FRF_DIALOG_H_