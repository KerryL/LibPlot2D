/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  rangeLimitsDialog.cpp
// Created:  5/12/2011
// Author:  K. Loux
// Description:  Dialog box for entering a minimum and maximum value for an axis.
// History:

// Local headers
#include "application/rangeLimitsDialog.h"

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
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 5);

	wxFlexGridSizer *inputSizer = new wxFlexGridSizer(2, 5, 5);
	mainSizer->Add(inputSizer, 0, wxEXPAND);
	inputSizer->AddGrowableCol(1);

	wxString valueString;
	wxStaticText *minLabel = new wxStaticText(this, wxID_ANY, _T("Minimum"));
	valueString.Printf("%f", min);
	minBox = new wxTextCtrl(this, wxID_ANY, valueString);
	inputSizer->Add(minLabel);
	inputSizer->Add(minBox, 0, wxEXPAND);

	wxStaticText *maxLabel = new wxStaticText(this, wxID_ANY, _T("Maximum"));
	valueString.Printf("%f", max);
	maxBox = new wxTextCtrl(this, wxID_ANY, valueString);
	inputSizer->Add(maxLabel);
	inputSizer->Add(maxBox, 0, wxEXPAND);

	mainSizer->AddSpacer(10);

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	minBox->SetFocus();

	SetSizerAndFit(topSizer);

	Center();
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
}