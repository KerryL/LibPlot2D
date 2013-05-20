/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  textInputDialog.h
// Created:  5/20/2013
// Author:  K. Loux
// Description:  Dialog box similar to ::wxGetTextFromUser() but allows differentiation between
//				 canceling and returning an empty string.
// History:

#ifndef _TEXT_INPUT_DIALOG_H_
#define _TEXT_INPUT_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

class TextInputDialog : public wxDialog
{
public:
	TextInputDialog(const wxString &message, const wxString &title, const wxString &defaultText, wxWindow *parent);
	virtual ~TextInputDialog();

	wxString GetText(void) const { return text->GetValue(); };

private:
	wxTextCtrl *text;

	void CreateControls(const wxString &message, const wxString &defaultText);
};

#endif// _TEXT_INPUT_DIALOG_H_