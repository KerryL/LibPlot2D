/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  transferFunctionDialog.h
// Created:  8/15/2012
// Author:  K. Loux
// Description:  Dialog for selection of transfer function data
// History:

#ifndef _TRANSFER_FUNCTION_DIALOG_H_
#define _TRANSFER_FUNCTION_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

class TransferFunctionDialog : public wxDialog
{
public:
	// Constructors
	TransferFunctionDialog(wxWindow *parent, const wxArrayString &descriptions);

	unsigned int GetInputIndex(void) const;
	unsigned int GetOutputIndex(void) const;

private:
	void CreateControls(const wxArrayString &descriptions);
	wxSizer *CreateSelectionControls(const wxArrayString &descriptions);
	wxSizer *CreateButtons(void);

	wxListBox *inputList;
	wxListBox *outputList;

	virtual bool TransferDataFromWindow(void);
};

#endif// _TRANSFER_FUNCTION_DIALOG_H_