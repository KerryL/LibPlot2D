/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  rangeLimitsDialog.h
// Date:  5/12/2011
// Auth:  K. Loux
// Desc:  Dialog box for entering a minimum and maximum value for an axis.

#ifndef RANGE_LIMITS_DIALOG_H_
#define RANGE_LIMITS_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

/// Dialog box for entering a minimum and maximum value to define the range of
/// an axis.
class RangeLimitsDialog : public wxDialog
{
public:
	/// Constructor.
	///
	/// \param parent Pointer to the window that owns this.
	/// \param min    Initial value for the minimum box.
	/// \param max    Initial value for the maximum box.
	RangeLimitsDialog(wxWindow *parent, const double &min, const double &max);
	~RangeLimitsDialog() = default;

	/// Gets the minimum value.
	/// \returns The value entered in the "minimum" box.
	double GetMinimum() const;

	/// Gets the maximum value.
	/// \returns The value entered in the "maximum" box.
	double GetMaximum() const;

private:
	wxTextCtrl *mMinBox;
	wxTextCtrl *mMaxBox;

	void CreateControls(const double& min, const double& max);

	// Overload from wxDialog
	void OnOKButton(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};

}// namespace LibPlot2D

#endif// RANGE_LIMITS_DIALOG_H_
