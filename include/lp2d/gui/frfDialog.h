/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  frfDialog.h
// Date:  8/15/2012
// Auth:  K. Loux
// Desc:  Dialog for selection of frequency response function data.

#ifndef FRF_DIALOG_H_
#define FRF_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

class FRFDialog : public wxDialog
{
public:
	// Constructors
	FRFDialog(wxWindow *parent, const wxArrayString &descriptions);

	~FRFDialog() = default;

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

}// namespace LibPlot2D

#endif// FRF_DIALOG_H_
