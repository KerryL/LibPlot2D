/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fftDialog.cpp
// Created:  8/15/2012
// Author:  K. Loux
// Description:  Dialog for specification of FFT options.
// History:

// wxWidgets headers
#include <wx/sizer.h>

// Local headers
#include "application/fftDialog.h"

//==========================================================================
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
//==========================================================================
FFTDialog::FFTDialog(wxWindow *parent, const unsigned int &_dataPoints,
	const unsigned int &_zoomDataPoints, const double &_sampleTime)
	: wxDialog(parent, wxID_ANY, _T("Fast Fourier Transform"), wxDefaultPosition)
{
	dataPoints = _dataPoints;
	zoomDataPoints = _zoomDataPoints;
	sampleTime = _sampleTime;

	frequencyRange = NULL;
	frequencyResolution = NULL;
	numberOfAverages = NULL;

	CreateControls();
}

//==========================================================================
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
//==========================================================================
BEGIN_EVENT_TABLE(FFTDialog, wxDialog)
	EVT_CHECKBOX(wxID_ANY, FFTDialog::OnCheckBoxEvent)
	EVT_COMBOBOX(wxID_ANY, FFTDialog::OnComboBoxEvent)
	EVT_TEXT(wxID_ANY, FFTDialog::OnTextBoxEvent)
END_EVENT_TABLE()

//==========================================================================
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
//==========================================================================
void FFTDialog::CreateControls(void)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 8);

	mainSizer->Add(CreateInputControls());
	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateOutputControls());
	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateButtons(), 1, wxGROW);

	SetSizerAndFit(topSizer);
	Center();
}

//==========================================================================
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
//==========================================================================
wxSizer* FFTDialog::CreateInputControls(void)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 5, 5);
	topSizer->Add(sizer);

	wxStaticText *windowLabel = new wxStaticText(this, wxID_ANY, _T("Window"));
	windowCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, GetWindowList(), wxCB_READONLY);
	windowCombo->SetSelection(FastFourierTransform::WindowHann);
	sizer->Add(windowLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
	sizer->Add(windowCombo, 1, wxALL | wxGROW, 2);

	wxStaticText *windowSizeLabel = new wxStaticText(this, wxID_ANY, _T("Window Size"));
	wxArrayString empty;
	windowSizeCombo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, empty, wxCB_READONLY);
	sizer->Add(windowSizeLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
	sizer->Add(windowSizeCombo, 1, wxALL | wxGROW, 2);

	wxStaticText *overlapLabel = new wxStaticText(this, wxID_ANY, _T("Overlap"));
	overlapTextBox = new wxTextCtrl(this, wxID_ANY, _T("0.0"));
	sizer->Add(overlapLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
	sizer->Add(overlapTextBox, 1, wxALL | wxGROW, 2);

	topSizer->AddSpacer(5);
	useZoomCheckBox = new wxCheckBox(this, wxID_ANY, _T("Use Zoomed Region Only"));
	topSizer->Add(useZoomCheckBox, 0, wxALL, 2);
	subtractMeanCheckBox = new wxCheckBox(this, wxID_ANY, _T("Subtract Mean Value"));
	topSizer->Add(subtractMeanCheckBox, 0, wxALL, 2);

	SetCheckBoxDefaults();
	ConfigureControls();

	return topSizer;
}

//==========================================================================
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
//==========================================================================
wxSizer* FFTDialog::CreateOutputControls(void)
{
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 5, 5);

	wxStaticText *rangeLabel = new wxStaticText(this, wxID_ANY, _T("Range"));
	frequencyRange = new wxStaticText(this, wxID_ANY, wxEmptyString);

	wxStaticText *resolutionLabel = new wxStaticText(this, wxID_ANY, _T("Resolution"));
	frequencyResolution = new wxStaticText(this, wxID_ANY, wxEmptyString);

	wxStaticText *averagesLabel = new wxStaticText(this, wxID_ANY, _T("Averages"));
	numberOfAverages = new wxStaticText(this, wxID_ANY, wxEmptyString);

	sizer->Add(rangeLabel, 0, wxALL, 2);
	sizer->Add(frequencyRange, 1, wxGROW | wxALL, 2);

	sizer->Add(resolutionLabel, 0, wxALL, 2);
	sizer->Add(frequencyResolution, 1, wxGROW | wxALL, 2);

	sizer->Add(averagesLabel, 0, wxALL, 2);
	sizer->Add(numberOfAverages, 1, wxGROW | wxALL, 2);

	UpdateOutputControls();

	return sizer;
}

//==========================================================================
// Class:			FFTDialog
// Function:		CreateButtons
//
// Description:		Creates the dialog buttons.
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
//==========================================================================
wxSizer* FFTDialog::CreateButtons(void)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"));
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"));

	sizer->AddStretchSpacer();
	sizer->Add(okButton, 0, wxALIGN_RIGHT | wxALL, 2);
	sizer->Add(cancelButton, 0, wxALIGN_RIGHT | wxALL, 2);

	okButton->SetDefault();

	return sizer;
}

//==========================================================================
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
//==========================================================================
void FFTDialog::ConfigureControls(void)
{
	unsigned int maxPower(FastFourierTransform::GetMaxPowerOfTwo(GetPointCount()));
	windowSizeCombo->Clear();

	unsigned int i;
	for (i = 1; i <= maxPower; i++)
		windowSizeCombo->Append(wxString::Format("%u", (unsigned int)pow(2, (double)i)));
	windowSizeCombo->SetSelection(windowSizeCombo->GetCount() - 1);
}

//==========================================================================
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
//==========================================================================
void FFTDialog::SetCheckBoxDefaults(void)
{
	if (zoomDataPoints == dataPoints || zoomDataPoints == 0)
	{
		useZoomCheckBox->SetValue(false);
		useZoomCheckBox->Enable(false);
	}
	else
	{
		useZoomCheckBox->Enable(true);
		useZoomCheckBox->SetValue(true);
	}

	subtractMeanCheckBox->SetValue(true);
}

//==========================================================================
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
//==========================================================================
wxArrayString FFTDialog::GetWindowList(void) const
{
	wxArrayString list;

	unsigned int i;
	for (i = 0; i < FastFourierTransform::WindowCount; i++)
		list.Add(FastFourierTransform::GetWindowName((FastFourierTransform::FFTWindow)i));

	return list;
}

//==========================================================================
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
//==========================================================================
void FFTDialog::OnCheckBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	ConfigureControls();
}

//==========================================================================
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
//==========================================================================
void FFTDialog::OnComboBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdateOutputControls();
}

//==========================================================================
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
//==========================================================================
void FFTDialog::OnTextBoxEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdateOutputControls();
}

//==========================================================================
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
//==========================================================================
bool FFTDialog::TransferDataFromWindow(void)
{
	double value;
	if (!overlapTextBox->GetValue().ToDouble(&value) || value < 0.0 || value > 1.0)
	{
		wxMessageBox(_T("Overlap value must be a number between 0.0 and 1.0."), _T("Value Error"), wxICON_ERROR, this);
		return false;
	}

	return true;
}

//==========================================================================
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
//		FastFourierTransform::FFTWindow
//
//==========================================================================
FastFourierTransform::FFTWindow FFTDialog::GetFFTWindow(void) const
{
	return (FastFourierTransform::FFTWindow)windowCombo->GetSelection();
}

//==========================================================================
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
//==========================================================================
unsigned int FFTDialog::GetWindowSize(void) const
{
	unsigned long value;
	windowSizeCombo->GetValue().ToULong(&value);

	return (unsigned int)value;
}

//==========================================================================
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
//==========================================================================
double FFTDialog::GetOverlap(void) const
{
	double value;
	overlapTextBox->GetValue().ToDouble(&value);

	return value;
}

//==========================================================================
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
//==========================================================================
bool FFTDialog::GetUseZoomedData(void) const
{
	return useZoomCheckBox->GetValue();
}

//==========================================================================
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
//==========================================================================
bool FFTDialog::GetSubtractMean(void) const
{
	return subtractMeanCheckBox->GetValue();
}

//==========================================================================
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
//==========================================================================
void FFTDialog::UpdateOutputControls(void)
{
	if (!frequencyRange || !frequencyResolution || !numberOfAverages)
		return;

	frequencyRange->SetLabel(wxString::Format("%0.3f Hz", 0.5 / sampleTime));
	frequencyResolution->SetLabel(wxString::Format("%0.3f Hz", 1.0 / (sampleTime * (double)GetWindowSize())));
	numberOfAverages->SetLabel(wxString::Format("%i",
		FastFourierTransform::GetNumberOfAverages(GetWindowSize(), GetOverlap(), GetPointCount())));
}

//==========================================================================
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
//==========================================================================
unsigned int FFTDialog::GetPointCount(void) const
{
	if (useZoomCheckBox->GetValue())
		return zoomDataPoints;

	return dataPoints;
}