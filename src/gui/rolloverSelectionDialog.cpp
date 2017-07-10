/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  rolloverSelectionDialog.cpp
// Date:  7/8/2017
// Auth:  K. Loux
// Desc:  Dialog for specifying the point at which data wraps around.

// Local headers
#include "lp2d/gui/rolloverSelectionDialog.h"

// wxWidgets headers
#include <wx/wx.h>

namespace LibPlot2D
{

//=============================================================================
// Class:			RolloverSelectionDialog
// Function:		Event Table
//
// Description:		Event table for the CreateSignalDialog class.
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
BEGIN_EVENT_TABLE(RolloverSelectionDialog, wxDialog)
	EVT_RADIOBUTTON(wxID_ANY, RolloverSelectionDialog::OnRadioButtonChange)
END_EVENT_TABLE()

RolloverSelectionDialog::RolloverSelectionDialog(wxWindow* parent)
	: wxDialog(parent, wxID_ANY, _T("Select Rollover Point"))
{
	CreateControls();
}

void RolloverSelectionDialog::CreateControls()
{
	wxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL, 5);

	const unsigned int spacing(5);
	mRollover360Radio = new wxRadioButton(this, wxID_ANY, _T("360"));
	mainSizer->Add(mRollover360Radio, wxSizerFlags().Border(wxTOP | wxBOTTOM, spacing));
	mRollover2PiRadio = new wxRadioButton(this, wxID_ANY, _T("2 ") + wxString::FromUTF8("\xF0\x9D\x9C\x8B"));
	mainSizer->Add(mRollover2PiRadio, wxSizerFlags().Border(wxTOP | wxBOTTOM, spacing));

	wxSizer* customSizer = new wxBoxSizer(wxHORIZONTAL);
	mRolloverCustomRadio = new wxRadioButton(this, wxID_ANY, _T("Custom:"));
	customSizer->Add(mRolloverCustomRadio);
	customSizer->AddSpacer(5);
	mCustomTextCtrl = new wxTextCtrl(this, wxID_ANY, _T("360"));
	mCustomTextCtrl->Enable(false);
	customSizer->Add(mCustomTextCtrl);

	mainSizer->Add(customSizer, wxSizerFlags().Border(wxTOP | wxBOTTOM, spacing));

	mainSizer->AddSpacer(15);

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	SetSizerAndFit(topSizer);
	Center();
}

void RolloverSelectionDialog::OnRadioButtonChange(wxCommandEvent& WXUNUSED(event))
{
	mCustomTextCtrl->Enable(mRolloverCustomRadio->GetValue());
}

bool RolloverSelectionDialog::TransferDataFromWindow()
{
	if (mRolloverCustomRadio->GetValue())
	{
		if (!mCustomTextCtrl->GetValue().ToDouble(&mRolloverPoint))
			return false;
	}
	else if (mRollover360Radio->GetValue())
		mRolloverPoint = 360.0;
	else// if (mRollover2PiRadio->GetValue())
		mRolloverPoint = 2.0 * M_PI;

	return true;
}

}// namespace LibPlot2D
