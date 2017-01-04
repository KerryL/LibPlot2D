/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  rangeLimitsDialog.cpp
// Date:  5/12/2011
// Auth:  K. Loux
// Desc:  Dialog box for entering a minimum and maximum value for an axis.

// Local headers
#include "lp2d/gui/rangeLimitsDialog.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			RangeLimitsDialog
// Function:		RangeLimitsDialog
//
// Description:		Constructor for RangeLimitsDialog class.
//
// Input Arguments:
//		parent	= wxWindow* that owns this object
//		min		= const double& specifying current minimum axis limit
//		max		= const double& specifying current maximum axis limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
RangeLimitsDialog::RangeLimitsDialog(wxWindow *parent, const double &min,
	const double &max) : wxDialog(parent, wxID_ANY, _T("Set Axis Limits"),
	wxDefaultPosition)
{
	CreateControls(min, max);
	Center();
}

//=============================================================================
// Class:			RangeLimitsDialog
// Function:		CreateControls
//
// Description:		Builds the dialog.
//
// Input Arguments:
//		min	= const double&
//		max	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RangeLimitsDialog::CreateControls(const double& min, const double& max)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 5);

	wxFlexGridSizer *inputSizer = new wxFlexGridSizer(2, 5, 5);
	mainSizer->Add(inputSizer, 0, wxEXPAND);
	inputSizer->AddGrowableCol(1);

	wxString valueString;
	wxStaticText *minLabel = new wxStaticText(this, wxID_ANY, _T("Minimum"));
	valueString.Printf("%f", min);
	mMinBox = new wxTextCtrl(this, wxID_ANY, valueString);
	inputSizer->Add(minLabel);
	inputSizer->Add(mMinBox, 0, wxEXPAND);

	wxStaticText *maxLabel = new wxStaticText(this, wxID_ANY, _T("Maximum"));
	valueString.Printf("%f", max);
	mMaxBox = new wxTextCtrl(this, wxID_ANY, valueString);
	inputSizer->Add(maxLabel);
	inputSizer->Add(mMaxBox, 0, wxEXPAND);

	mainSizer->AddSpacer(10);

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	mMinBox->SetFocus();

	SetSizerAndFit(topSizer);
}

//=============================================================================
// Class:			RangeLimitsDialog
// Function:		Event Table
//
// Description:		Specifies event handlers for dialog events.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
BEGIN_EVENT_TABLE(RangeLimitsDialog, wxDialog)
	EVT_BUTTON(wxID_OK,	RangeLimitsDialog::OnOKButton)
END_EVENT_TABLE()

//=============================================================================
// Class:			RangeLimitsDialog
// Function:		GetMinimum
//
// Description:		Returns the value entered in the minimum box.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating current value in the minimum box
//
//=============================================================================
double RangeLimitsDialog::GetMinimum() const
{
	double value;
	if (!mMinBox->GetValue().ToDouble(&value))
		value = 0.0;

	return value;
}

//=============================================================================
// Class:			RangeLimitsDialog
// Function:		GetMaximum
//
// Description:		Returns the value entered in the maximum box.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating current value in the maximum box
//
//=============================================================================
double RangeLimitsDialog::GetMaximum() const
{
	double value;
	if (!mMaxBox->GetValue().ToDouble(&value))
		value = 0.0;

	return value;
}

//=============================================================================
// Class:			RangeLimitsDialog
// Function:		OnOKButton
//
// Description:		Validates min and max values when the user chooses OK.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RangeLimitsDialog::OnOKButton(wxCommandEvent &event)
{
	// Validate the values in the min and max boxes
	double dummy;
	if (!mMinBox->GetValue().ToDouble(&dummy) || !mMaxBox->GetValue().ToDouble(&dummy))
	{
		::wxMessageBox(_T("ERROR:  Minimum and Maximum values must be numbers!"), _T("Error Setting Limits"));
		return;
	}

	// Skip this event if the data is valid
	event.Skip();
}

}// namespace LibPlot2D
