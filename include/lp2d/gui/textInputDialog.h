/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  textInputDialog.h
// Date:  5/20/2013
// Auth:  K. Loux
// Desc:  Dialog box similar to ::wxGetTextFromUser() but allows
//        differentiation between canceling and returning an empty string.

#ifndef TEXT_INPUT_DIALOG_H_
#define TEXT_INPUT_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

namespace LibPlot2D
{

/// Dialog box similar to wxGetTextFromUser(), but allows differentiation
/// between canceling and returning an empty string.
class TextInputDialog : public wxDialog
{
public:
	/// Constructor.
	///
	/// \param message     Text to display.
	/// \param title       Text to display in title bar.
	/// \param defaultText Initial value of string in input text box.
	/// \param parent      Pointer to window that owns this.
	TextInputDialog(const wxString &message, const wxString &title,
		const wxString &defaultText, wxWindow *parent);
	~TextInputDialog() = default;

	/// Gets the contents of the input text box.
	/// \returns The contents of the input text box.
	wxString GetText() const { return mText->GetValue(); }

private:
	wxTextCtrl *mText;

	void CreateControls(const wxString &message, const wxString &defaultText);
};

}// namespace LibPlot2D

#endif// TEXT_INPUT_DIALOG_H_
