/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  fftDialog.cpp
// Date:  8/15/2012
// Auth:  K. Loux
// Desc:  Dialog for specification of FFT options.

// wxWidgets headers
#include <wx/sizer.h>

// Local headers
#include "lp2d/gui/fftDialog.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			FFTDialog
// Function:		FFTDialog
//
// Description:		Constructor for FFTDialog class.
//
// Input Arguments:
//		parent			= wxWindow* that owns this object
//		_dataPoints		= const unsigned int&
//		_zoomDataPoints	= const unsigned int&
//		_sampleTime		= const double& [sec]
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
FFTDialog::FFTDialog(wxWindow *parent, const unsigned int &dataPoints,
	const unsigned int &zoomDataPoints, const double &sampleTime)
	: wxDialog(parent, wxID_ANY, _T("Fast Fourier Transform"),
	wxDefaultPosition), mDataPoints(dataPoints),
	mZoomDataPoints(zoomDataPoints), mSampleTime(sampleTime),
	mFrequencyRange(nullptr), mFrequencyResolution(nullptr),
	mNumberOfAverages(nullptr)
{
	CreateControls();
}

//=============================================================================
// Class:			FFTDialog
// Function:		Event Table
//
// Description:		Event table for FFTDialog class.
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
BEGIN_EVENT_TABLE(FFTDialog, wxDialog)
	EVT_CHECKBOX(wxID_ANY, FFTDialog::OnCheckBoxEvent)
	EVT_COMBOBOX(wxID_ANY, FFTDialog::OnComboBoxEvent)
	EVT_TEXT(wxID_ANY, FFTDialog::OnTextBoxEvent)
END_EVENT_TABLE()

//=============================================================================
// Class:			FFTDialog
// Function:		CreateControls
//
// Description:		Creates the dialog controls.
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
void FFTDialog::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 8);

	mainSizer->Add(CreateInputControls());
	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateOutputControls());
	mainSizer->AddSpacer(10);

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	SetSizerAndFit(topSizer);
	Center();
}

//=============================================================================
// Class:			FFTDialog
// Function:		CreateInputControls
//
// Description:		Creates the main input controls.
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
wxSizer* FFTDialog::CreateInputControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 5, 5);
	topSizer->Add(sizer);

	wxStaticText *windowLabel = new wxStaticText(this, wxID_ANY, _T("Window"));
	mWindowCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, GetWindowList(), wxCB_READONLY);
	mWindowCombo->SetSelection(static_cast<int>(FastFourierTransform::WindowType::Hann));
	sizer->Add(windowLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
	sizer->Add(mWindowCombo, 1, wxALL | wxGROW, 2);

	wxStaticText *windowSizeLabel = new wxStaticText(this, wxID_ANY, _T("Window Size"));
	wxArrayString empty;
	mWindowSizeCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, empty, wxCB_READONLY);
	sizer->Add(windowSizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
	sizer->Add(mWindowSizeCombo, 1, wxALL | wxGROW, 2);

	wxStaticText *overlapLabel = new wxStaticText(this, wxID_ANY, _T("Overlap"));
	mOverlapTextBox = new wxTextCtrl(this, wxID_ANY, _T("0.0"));
	sizer->Add(overlapLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
	sizer->Add(mOverlapTextBox, 1, wxALL | wxGROW, 2);

	topSizer->AddSpacer(5);
	mUseZoomCheckBox = new wxCheckBox(this, wxID_ANY, _T("Use Zoomed Region Only"));
	topSizer->Add(mUseZoomCheckBox, 0, wxALL, 2);
	mSubtractMeanCheckBox = new wxCheckBox(this, wxID_ANY, _T("Subtract Mean Value"));
	topSizer->Add(mSubtractMeanCheckBox, 0, wxALL, 2);

	SetCheckBoxDefaults();
	ConfigureControls();

	return topSizer;
}

//=============================================================================
// Class:			FFTDialog
// Function:		CreateOutputControls
//
// Description:		Creates the output controls.
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
wxSizer* FFTDialog::CreateOutputControls()
{
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 5, 5);

	wxStaticText *rangeLabel = new wxStaticText(this, wxID_ANY, _T("Range"));
	mFrequencyRange = new wxStaticText(this, wxID_ANY, wxEmptyString);

	wxStaticText *resolutionLabel = new wxStaticText(this, wxID_ANY, _T("Resolution"));
	mFrequencyResolution = new wxStaticText(this, wxID_ANY, wxEmptyString);

	wxStaticText *averagesLabel = new wxStaticText(this, wxID_ANY, _T("Averages"));
	mNumberOfAverages = new wxStaticText(this, wxID_ANY, wxEmptyString);

	sizer->Add(rangeLabel, 0, wxALL, 2);
	sizer->Add(mFrequencyRange, 1, wxGROW | wxALL, 2);

	sizer->Add(resolutionLabel, 0, wxALL, 2);
	sizer->Add(mFrequencyResolution, 1, wxGROW | wxALL, 2);

	sizer->Add(averagesLabel, 0, wxALL, 2);
	sizer->Add(mNumberOfAverages, 1, wxGROW | wxALL, 2);

	UpdateOutputControls();

	return sizer;
}

//=============================================================================
// Class:			FFTDialog
// Function:		ConfigureControls
//
// Description:		Configures (populates) the controls with available options.
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
void FFTDialog::ConfigureControls()
{
	unsigned int maxPower(
		FastFourierTransform::GetMaxPowerOfTwo(GetPointCount()));
	mWindowSizeCombo->Clear();

	unsigned int i;
	for (i = 1; i <= maxPower; ++i)
		mWindowSizeCombo->Append(wxString::Format("%u",
			static_cast<unsigned int>(pow(2, i))));
	mWindowSizeCombo->SetSelection(mWindowSizeCombo->GetCount() - 1);
}

//=============================================================================
// Class:			FFTDialog
// Function:		SetCheckBoxDefaults
//
// Description:		Configures the zoom check box (decides whether or not it
//					is enabled).
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
void FFTDialog::SetCheckBoxDefaults()
{
	if (mZoomDataPoints == mDataPoints || mZoomDataPoints == 0)
	{
		mUseZoomCheckBox->SetValue(false);
		mUseZoomCheckBox->Enable(false);
	}
	else
	{
		mUseZoomCheckBox->Enable(true);
		mUseZoomCheckBox->SetValue(true);
	}

	mSubtractMeanCheckBox->SetValue(true);
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetWindowList
//
// Description:		Generates the list of available FFT windows.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxArrayString
//
//=============================================================================
wxArrayString FFTDialog::GetWindowList() const
{
	wxArrayString list;

	unsigned int i;
	for (i = 0; i < static_cast<unsigned int>(
		FastFourierTransform::WindowType::Count); ++i)
		list.Add(FastFourierTransform::GetWindowName(
			static_cast<FastFourierTransform::WindowType>(i)));

	return list;
}

//=============================================================================
// Class:			FFTDialog
// Function:		OnCheckBoxEvent
//
// Description:		Event handler for check box events.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void FFTDialog::OnCheckBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	ConfigureControls();
}

//=============================================================================
// Class:			FFTDialog
// Function:		OnComboBoxEvent
//
// Description:		Event handler for combo box events.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void FFTDialog::OnComboBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdateOutputControls();
}

//=============================================================================
// Class:			FFTDialog
// Function:		OnTextBoxEvent
//
// Description:		Event handler for text box events.
//
// Input Arguments:
//		event	= wxCommandEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void FFTDialog::OnTextBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdateOutputControls();
}

//=============================================================================
// Class:			FFTDialog
// Function:		TransferDataFromWindow
//
// Description:		Validates the options specified by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if all data is valid, false otherwise
//
//=============================================================================
bool FFTDialog::TransferDataFromWindow()
{
	double value;
	if (!mOverlapTextBox->GetValue().ToDouble(&value)
		|| value < 0.0 || value > 1.0)
	{
		wxMessageBox(_T("Overlap value must be a number between 0.0 and 1.0."),
			_T("Value Error"), wxICON_ERROR, this);
		return false;
	}

	return true;
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetFFTWindow
//
// Description:		Returns the selected FFT window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		FastFourierTransform::WindowType
//
//=============================================================================
FastFourierTransform::WindowType FFTDialog::GetFFTWindow() const
{
	return static_cast<FastFourierTransform::WindowType>(
		mWindowCombo->GetSelection());
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetWindowSize
//
// Description:		Returns the specified window size.
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
unsigned int FFTDialog::GetWindowSize() const
{
	unsigned long value;
	mWindowSizeCombo->GetValue().ToULong(&value);

	return static_cast<unsigned int>(value);
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetOverlap
//
// Description:		Returns the specified overlap.
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
double FFTDialog::GetOverlap() const
{
	double value;
	mOverlapTextBox->GetValue().ToDouble(&value);

	return value;
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetUseZoomedData
//
// Description:		Indicates whether the user specified the use of the zoomed
//					portion or the full data set.
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
bool FFTDialog::GetUseZoomedData() const
{
	return mUseZoomCheckBox->GetValue();
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetSubtractMean
//
// Description:		Indicates whether the user specified that the data's mean
//					should be subtracted prior to evaluating the FFT.
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
bool FFTDialog::GetSubtractMean() const
{
	return mSubtractMeanCheckBox->GetValue();
}

//=============================================================================
// Class:			FFTDialog
// Function:		UpdateOutputControls
//
// Description:		Updates the output controls.
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
void FFTDialog::UpdateOutputControls()
{
	if (!mFrequencyRange || !mFrequencyResolution || !mNumberOfAverages)
		return;

	mFrequencyRange->SetLabel(wxString::Format("%0.3f Hz", 0.5 / mSampleTime));
	mFrequencyResolution->SetLabel(wxString::Format("%0.3f Hz",
		1.0 / (mSampleTime * GetWindowSize())));
	mNumberOfAverages->SetLabel(wxString::Format("%i",
		FastFourierTransform::GetNumberOfAverages(GetWindowSize(),
			GetOverlap(), GetPointCount())));
}

//=============================================================================
// Class:			FFTDialog
// Function:		GetPointCount
//
// Description:		Returns the number of points available for analysis.
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
unsigned int FFTDialog::GetPointCount() const
{
	if (mUseZoomCheckBox->GetValue())
		return mZoomDataPoints;

	return mDataPoints;
}

}// namespace LibPlot2D
