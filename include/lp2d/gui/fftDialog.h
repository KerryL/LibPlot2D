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

class FFTDialog : public wxDialog
{
public:
	// Constructor
	FFTDialog(wxWindow *parent, const unsigned int &dataPoints,
		const unsigned int &zoomDataPoints, const double &sampleTime);

	~FFTDialog() = default;

	FastFourierTransform::WindowType GetFFTWindow() const;
	unsigned int GetWindowSize() const;
	double GetOverlap() const;
	bool GetUseZoomedData() const;
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
