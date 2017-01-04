/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  fftDialog.h
// Date:  8/15/2012
// Auth:  K. Loux
// Desc:  Dialog for specification of FFT options.

#ifndef FFT_DIALOG_H_
#define FFT_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/utilities/signals/fft.h"

namespace LibPlot2D
{

/// Dialog for specifying FFT options.
/// \see FastFourierTransform
class FFTDialog : public wxDialog
{
public:
	/// Constructor.
	///
	/// \param parent Pointer to window that owns this.
	/// \param dataPoints     Number of data points contained within the
	///                       signal.
	/// \param zoomDataPoints Number of data points within the zoomed region of
	///                       the signal.
	/// \param sampleTime     The mean sample time of the signal.
	FFTDialog(wxWindow *parent, const unsigned int &dataPoints,
		const unsigned int &zoomDataPoints, const double &sampleTime);
	~FFTDialog() = default;

	/// Gets the window type specified by the user.
	/// \returns The desired window type.
	FastFourierTransform::WindowType GetFFTWindow() const;

	/// Gets the user-specified window size.
	/// \returns The user-specified window size.
	unsigned int GetWindowSize() const;

	/// Gets the user-specified overlap distance.
	/// \returns The user-specified overlap distance.
	double GetOverlap() const;

	/// Gets the flag indicating whether the user wants to perform an FFT on
	/// the entire data set, or only the zoomed region.
	/// \returns True if the user only wants to consider the zooomed region.
	bool GetUseZoomedData() const;

	/// Gets the flag indicating whether the user wants to subtract the mean
	/// value from the data set.
	/// \returns True if the user wants to subtract the mean value.
	bool GetSubtractMean() const;

private:
	void CreateControls();
	wxSizer* CreateInputControls();
	wxSizer* CreateOutputControls();
	void ConfigureControls();
	void SetCheckBoxDefaults();

	unsigned int mDataPoints;
	unsigned int mZoomDataPoints;
	double mSampleTime;

	unsigned int GetPointCount() const;

	wxComboBox *mWindowSizeCombo;
	wxComboBox *mWindowCombo;
	wxTextCtrl *mOverlapTextBox;
	wxCheckBox *mUseZoomCheckBox;
	wxCheckBox *mSubtractMeanCheckBox;

	wxStaticText *mFrequencyRange;
	wxStaticText *mFrequencyResolution;
	wxStaticText *mNumberOfAverages;

	bool TransferDataFromWindow() override;

	void OnCheckBoxEvent(wxCommandEvent &event);
	void OnComboBoxEvent(wxCommandEvent &event);
	void OnTextBoxEvent(wxCommandEvent &event);

	wxArrayString GetWindowList() const;
	void UpdateOutputControls();

	DECLARE_EVENT_TABLE()
};

}// namespace LibPlot2D

#endif// FFT_DIALOG_H_
