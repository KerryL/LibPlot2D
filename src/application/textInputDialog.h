/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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

	wxString GetText() const { return text->GetValue(); };

private:
	wxTextCtrl *text;

	void CreateControls(const wxString &message, const wxString &defaultText);
};

#endif// _TEXT_INPUT_DIALOG_H_