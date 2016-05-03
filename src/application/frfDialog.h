/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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

	unsigned int GetInputIndex() const;
	unsigned int GetOutputIndex() const;
	unsigned int GetNumberOfAverages() const;

	bool GetComputePhase() const;
	bool GetComputeCoherence() const;
	bool GetModuloPhase() const;

private:
	void CreateControls(const wxArrayString &descriptions);
	wxSizer *CreateSelectionControls(const wxArrayString &descriptions);
	wxSizer *CreateTextBox();
	wxSizer *CreateCheckBoxes();

	wxListBox *inputList;
	wxListBox *outputList;

	wxCheckBox *phaseCheckBox;
	wxCheckBox *coherenceCheckBox;
	wxCheckBox *moduloPhaseCheckBox;

	wxTextCtrl *averagesTextBox;

	virtual bool TransferDataFromWindow();
};

#endif// _FRF_DIALOG_H_