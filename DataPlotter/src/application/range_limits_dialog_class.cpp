/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  range_limits_dialog_class.cpp
// Created:  5/12/2011
// Author:  K. Loux
// Description:  Dialog box for entering a minimum and maximum value for an axis.
// History:

// Local headers
#include "application/range_limits_dialog_class.h"

//==========================================================================
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
//==========================================================================
RangeLimitsDialog::RangeLimitsDialog(wxWindow *parent, const double &min, const double &max)
									 : wxDialog(parent, wxID_ANY, _T("Set Axis Limits"), wxDefaultPosition)
{
	// Create controls
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL, 5);

	// Create the text boxes and their labels
	int labelWidth = 50;
	int textBoxWidth = 75;
	wxString valueString;

	wxBoxSizer *minSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(minSizer, 0, wxALL, 5);
	wxStaticText *minLabel = new wxStaticText(this, wxID_ANY, _T("Minimum"), wxDefaultPosition, wxSize(labelWidth, -1));
	valueString.Printf("%f", min);
	minBox = new wxTextCtrl(this, wxID_ANY, valueString, wxDefaultPosition, wxSize(textBoxWidth, -1));
	minSizer->Add(minLabel);
	minSizer->Add(minBox);

	wxBoxSizer *maxSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(maxSizer, 0, wxALL, 5);
	wxStaticText *maxLabel = new wxStaticText(this, wxID_ANY, _T("Maximum"), wxDefaultPosition, wxSize(labelWidth, -1));
	valueString.Printf("%f", max);
	maxBox = new wxTextCtrl(this, wxID_ANY, valueString, wxDefaultPosition, wxSize(textBoxWidth, -1));
	maxSizer->Add(maxLabel);
	maxSizer->Add(maxBox);

	// Create the dialog buttons
	int buttonWidth = 50;
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL);
	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"), wxDefaultPosition, wxSize(buttonWidth, -1));
	okButton->SetDefault();
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"), wxDefaultPosition, wxSize(buttonWidth, -1));
	buttonSizer->Add(okButton, 0, wxALL, 5);
	buttonSizer->Add(cancelButton, 0, wxALL, 5);

	// Set the sizer to this dialog
	SetSizerAndFit(topSizer);
}

//==========================================================================
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
//==========================================================================
BEGIN_EVENT_TABLE(RangeLimitsDialog, wxDialog)
	EVT_BUTTON(wxID_OK,	RangeLimitsDialog::OnOKButton)
END_EVENT_TABLE()

//==========================================================================
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
//==========================================================================
double RangeLimitsDialog::GetMinimum(void) const
{
	double value;
	if (!minBox->GetValue().ToDouble(&value))
		value = 0.0;

	return value;
}

//==========================================================================
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
//==========================================================================
double RangeLimitsDialog::GetMaximum(void) const
{
	double value;
	if (!maxBox->GetValue().ToDouble(&value))
		value = 0.0;

	return value;
}

//==========================================================================
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
//==========================================================================
void RangeLimitsDialog::OnOKButton(wxCommandEvent &event)
{
	// Validate the values in the min and max boxes
	double dummy;
	if (!minBox->GetValue().ToDouble(&dummy) || !maxBox->GetValue().ToDouble(&dummy))
	{
		::wxMessageBox(_T("ERROR:  Minimum and Maximum values must be numbers!"), _T("Error Setting Limits"));
		return;
	}

	// Skip this event if the data is valid
	event.Skip();

	return;
}