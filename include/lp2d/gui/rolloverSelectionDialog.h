/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  rolloverSelectionDialog.h
// Date:  7/8/2017
// Auth:  K. Loux
// Desc:  Dialog for specifying the point at which data wraps around.

#ifndef ROLLOVER_SELECTION_DIALOG_H_
#define ROLLOVER_SELECTION_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

// wxWidgets forward declarations
class wxTextCtrl;
class wxRadioButton;

namespace LibPlot2D
{

/// Dialog for selecting a point at which data might "rollover."
class RolloverSelectionDialog : public wxDialog
{
public:
	/// Constructor.
	///
	/// \param parent     Pointer to the window that owns this.
	RolloverSelectionDialog(wxWindow* parent);

	/// Gets the selected rollover point.
	/// \returns The selected rollover point.
	double GetRolloverPoint() const { return mRolloverPoint; }

private:
	double mRolloverPoint = 360.0;

	wxRadioButton* mRollover360Radio;
	wxRadioButton* mRollover2PiRadio;
	wxRadioButton* mRolloverCustomRadio;
	wxTextCtrl* mCustomTextCtrl;

	void CreateControls();

	enum EventIDs
	{
		idRadioChange = wxID_HIGHEST + 500
	};

	void OnRadioButtonChange(wxCommandEvent& event);

	bool TransferDataFromWindow() override;

	DECLARE_EVENT_TABLE();
};

}// namespace LibPlot2D

#endif// ROLLOVER_SELECTION_DIALOG_H_
