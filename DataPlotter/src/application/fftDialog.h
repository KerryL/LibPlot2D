/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fftDialog.h
// Created:  8/15/2012
// Author:  K. Loux
// Description:  Dialog for specification of FFT options.
// History:

#ifndef _FFT_DIALOG_H_
#define _FFT_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/signals/fft.h"

class FFTDialog : public wxDialog
{
public:
	// Constructor
	FFTDialog(wxWindow *parent, const unsigned int &_dataPoints,
		const unsigned int &_zoomDataPoints, const double &_sampleTime);

	FastFourierTransform::FFTWindow GetFFTWindow(void) const;
	unsigned int GetWindowSize(void) const;
	double GetOverlap(void) const;
	bool GetUseZoomedData(void) const;

private:
	void CreateControls(void);
	wxSizer* CreateInputControls(void);
	wxSizer* CreateOutputControls(void);
	wxSizer* CreateButtons(void);
	void ConfigureControls(void);

	unsigned int dataPoints;
	unsigned int zoomDataPoints;
	double sampleTime;

	unsigned int GetPointCount(void) const;

	wxComboBox *windowSizeCombo;
	wxComboBox *windowCombo;
	wxTextCtrl *overlapTextBox;
	wxCheckBox *useZoomCheckBox;

	wxStaticText *frequencyRange;
	wxStaticText *frequencyResolution;
	wxStaticText *numberOfAverages;

	virtual bool TransferDataFromWindow(void);

	void OnCheckBoxEvent(wxCommandEvent &event);
	void OnComboBoxEvent(wxCommandEvent &event);
	void OnTextBoxEvent(wxCommandEvent &event);

	wxArrayString GetWindowList(void) const;
	void UpdateOutputControls(void);

	DECLARE_EVENT_TABLE()
};

#endif// _FFT_DIALOG_H_