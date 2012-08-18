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

class FFTDialog : public wxDialog
{
public:
	// Constructor
	FFTDialog();

private:
	virtual void OnOKButton(wxCommandEvent &event);

	virtual bool TransferDataFromWindow(void);

	DECLARE_EVENT_TABLE()
};

#endif// _FFT_DIALOG_H_