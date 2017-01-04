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

/// Dialog allowing the user to specify frequency response function parameters.
class FRFDialog : public wxDialog
{
public:
	/// Constructor.
	///
	/// \param parent       Pointer to window that owns this.
	/// \param descriptions Descriptions of the available signals.
	FRFDialog(wxWindow *parent, const wxArrayString &descriptions);
	~FRFDialog() = default;

	/// Gets the index of the user-specified input signal.
	/// \returns The associated index.
	unsigned int GetInputIndex() const;

	/// Gets the index of the user-specified output signal.
	/// \returns The associated index.
	unsigned int GetOutputIndex() const;

	/// Gets the user-specified number of averages to use.
	/// \returns The number of averages.
	unsigned int GetNumberOfAverages() const;

	/// Gets the flag indicating whether the user wants to compute the phase
	/// data or not.
	/// \returns True if the user specified that the phase data should be
	///          calculated.
	bool GetComputePhase() const;

	/// Gets the flag indicating whether the user wants to compute the
	/// coherence data or not.
	/// \returns True if the user specified that the coherence data should be
	///          calculated.
	bool GetComputeCoherence() const;

	/// Gets the flag indicating whether or not the user wants the phase data
	/// to be limited to the range <&plusmn> 180 deg.
	/// \returns True if the user specified that the phase data should be
	///          wrapped.
	bool GetModuloPhase() const;

private:
	void CreateControls(const wxArrayString &descriptions);
	wxSizer *CreateSelectionControls(const wxArrayString &descriptions);
	wxSizer *CreateTextBox();
	wxSizer *CreateCheckBoxes();

	wxListBox *mInputList;
	wxListBox *mOutputList;

	wxCheckBox *mPhaseCheckBox;
	wxCheckBox *mCoherenceCheckBox;
	wxCheckBox *mModuloPhaseCheckBox;

	wxTextCtrl *mAveragesTextBox;

	bool TransferDataFromWindow() override;
};

}// namespace LibPlot2D

#endif// FRF_DIALOG_H_
