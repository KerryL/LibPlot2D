/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  frfDialog.cpp
// Date:  8/15/2012
// Auth:  K. Loux
// Desc:  Dialog for selection of frequency response function data.

// Standard C++ headers
#include <algorithm>

// Local headers
#include "lp2d/gui/frfDialog.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			FRFDialog
// Function:		FRFDialog
//
// Description:		Constructor for FRFDialog class.
//
// Input Arguments:
//		parent		= wxWindow* that owns this object
//		descriptions	= const wxArrayString& containing curve descriptions
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
FRFDialog::FRFDialog(wxWindow *parent, const wxArrayString &descriptions)
									 : wxDialog(parent, wxID_ANY, _T("Frequency Response Function"), wxDefaultPosition)
{
	CreateControls(descriptions);
}

//=============================================================================
// Class:			FRFDialog
// Function:		CreateControls
//
// Description:		Populates the dialog with controls.
//
// Input Arguments:
//		descriptions	= const wxArrayString& containing curve descriptions
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void FRFDialog::CreateControls(const wxArrayString &descriptions)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 5);

	mainSizer->Add(CreateSelectionControls(descriptions));
	mainSizer->Add(CreateTextBox());
	mainSizer->Add(CreateCheckBoxes());

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	SetSizerAndFit(topSizer);
	Center();
}

//=============================================================================
// Class:			FRFDialog
// Function:		CreateSelectionControls
//
// Description:		Returns a sizer containing the selection controls.
//
// Input Arguments:
///		descriptions	= const wxArrayString& containing curve descriptions
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//=============================================================================
wxSizer* FRFDialog::CreateSelectionControls(const wxArrayString &descriptions)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(leftSizer, 1, wxGROW);
	sizer->Add(rightSizer, 1, wxGROW);

	wxStaticText *inputText = new wxStaticText(this, wxID_ANY, _T("Specify stimulus data:"));
	mInputList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, descriptions, wxLB_SINGLE);
	leftSizer->Add(inputText);
	leftSizer->Add(mInputList, 0, wxGROW | wxALL, 5);

	wxStaticText *outputText = new wxStaticText(this, wxID_ANY, _T("Specify response data:"));
	mOutputList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, descriptions, wxLB_SINGLE);
	rightSizer->Add(outputText);
	rightSizer->Add(mOutputList, 0, wxGROW | wxALL, 5);

	return sizer;
}

//=============================================================================
// Class:			FRFDialog
// Function:		CreateTextBox
//
// Description:		Returns a sizer containing the text box control.
//
// Input Arguments:
///		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//=============================================================================
wxSizer* FRFDialog::CreateTextBox()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *averagesLabel = new wxStaticText(this, wxID_ANY, _T("Number of Averages"));
	mAveragesTextBox = new wxTextCtrl(this, wxID_ANY, _T("1"));
	sizer->Add(averagesLabel, 0, wxALL, 5);
	sizer->Add(mAveragesTextBox, 1, wxGROW | wxALL, 5);

	return sizer;
}

//=============================================================================
// Class:			FRFDialog
// Function:		CreateCheckBoxes
//
// Description:		Returns a sizer containing the check box controls.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//=============================================================================
wxSizer *FRFDialog::CreateCheckBoxes()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	mPhaseCheckBox = new wxCheckBox(this, wxID_ANY, _T("Include Phase Data"));
	mModuloPhaseCheckBox = new wxCheckBox(this, wxID_ANY, _T("Keep Phase Data Within \261") _T("180 deg"));
	mCoherenceCheckBox = new wxCheckBox(this, wxID_ANY, _T("Include Coherence Data"));

	mPhaseCheckBox->SetValue(true);

	wxBoxSizer *moduloSizer = new wxBoxSizer(wxHORIZONTAL);
	moduloSizer->AddSpacer(mPhaseCheckBox->GetSize().GetHeight());
	moduloSizer->Add(mModuloPhaseCheckBox);

	sizer->Add(mPhaseCheckBox, 0, wxALL, 5);
	sizer->Add(moduloSizer, 0, wxALL, 5);
	sizer->Add(mCoherenceCheckBox, 0, wxALL, 5);

	return sizer;
}

//=============================================================================
// Class:			FRFDialog
// Function:		TransferDataFromWindow
//
// Description:		Validates selections (ensures there are selections).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if transfer is valid, false otherwise
//
//=============================================================================
bool FRFDialog::TransferDataFromWindow()
{
	if (mInputList->GetSelection() == wxNOT_FOUND || mOutputList->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox(_T("Please select one stimulus signal and one response signal."),
			_T("Transfer Function"), wxICON_ERROR);
		return false;
	}

	unsigned long value;
	if (!mAveragesTextBox->GetValue().ToULong(&value))
	{
		wxMessageBox(_T("Number of averages must be an integer."),
			_T("Transfer Function"), wxICON_ERROR);
		return false;
	}

	return true;
}

//=============================================================================
// Class:			FRFDialog
// Function:		GetInputIndex
//
// Description:		Returns the input data index.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int FRFDialog::GetInputIndex() const
{
	return mInputList->GetSelection();
}

//=============================================================================
// Class:			FRFDialog
// Function:		GetOutputIndex
//
// Description:		Returns the output data index.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int FRFDialog::GetOutputIndex() const
{
	return mOutputList->GetSelection();
}

//=============================================================================
// Class:			FRFDialog
// Function:		GetNumberOfAverages
//
// Description:		Returns the number of averages specified by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int FRFDialog::GetNumberOfAverages() const
{
	unsigned long value;
	mAveragesTextBox->GetValue().ToULong(&value);
	return std::max<unsigned int>((unsigned int)value, 1);
}

//=============================================================================
// Class:			FRFDialog
// Function:		GetComputePhase
//
// Description:		Indicates whether or not the user selected phase computation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool FRFDialog::GetComputePhase() const
{
	return mPhaseCheckBox->GetValue();
}

//=============================================================================
// Class:			FRFDialog
// Function:		GetComputeCoherence
//
// Description:		Indicates whether or not the user selected coherence computation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool FRFDialog::GetComputeCoherence() const
{
	return mCoherenceCheckBox->GetValue();
}

//=============================================================================
// Class:			FRFDialog
// Function:		GetModuloPhase
//
// Description:		Indicates whether or not the user selected to modulo phase data.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool FRFDialog::GetModuloPhase() const
{
	return mModuloPhaseCheckBox->GetValue();
}

}// namespace LibPlot2D
