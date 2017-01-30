/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  createSignalDialog.cpp
// Date:  8/19/2013
// Auth:  K. Loux
// Desc:  Dialog for creating a variety of signals (not dependent on other
//        curves).

// Standard C++ headers
#include <cstdlib>
#include <climits>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/gui/createSignalDialog.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			CreateSignalDialog
// Function:		CreateSignalDialog
//
// Description:		Constructor for CreateSignalDialog class.
//
// Input Arguments:
//		parent		= wxWindow*
//		startTime	= const double&
//		duration	= const double&
//		sampleRate	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
CreateSignalDialog::CreateSignalDialog(wxWindow *parent, const double &startTime,
	const double &duration, const double &sampleRate) : wxDialog(parent, wxID_ANY,
	_T("Create Signal"), wxPoint(-1,-1))
{
	CreateControls(startTime, duration, sampleRate);
}

//=============================================================================
// Class:			CreateSignalDialog
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
BEGIN_EVENT_TABLE(CreateSignalDialog, wxDialog)
	EVT_COMBOBOX(wxID_ANY,			CreateSignalDialog::OnSignalTypeChangeEvent)
	EVT_TEXT(idInitialValue,		CreateSignalDialog::OnAmplitudeChangeEvent)
	EVT_TEXT(idSlope,				CreateSignalDialog::OnSlopeChangeEvent)
	EVT_TEXT(idFrequency,			CreateSignalDialog::OnFrequencyChangeEvent)
	EVT_TEXT(idPeriod,				CreateSignalDialog::OnPeriodChangeEvent)
	EVT_TEXT(idPhaseAngle,			CreateSignalDialog::OnPhaseAngleChangeEvent)
	EVT_TEXT(idPhaseTime,			CreateSignalDialog::OnPhaseTimeChangeEvent)
END_EVENT_TABLE()

//=============================================================================
// Class:			CreateSignalDialog
// Function:		GetSignalName
//
// Description:		Returns the user-specified name for the signal.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString CreateSignalDialog::GetSignalName() const
{
	return mSignalNameTextBox->GetValue();
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		CreateControls
//
// Description:		Creates the controls and populates with default values.
//
// Input Arguments:
//		startTime	= const double&
//		duration	= const double&
//		sampleRate	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void CreateSignalDialog::CreateControls(const double &startTime, const double &duration,
	const double &sampleRate)
{
	wxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL, 5);

	const unsigned int spacing(5);
	wxFlexGridSizer *inputSizer = new wxFlexGridSizer(3, spacing, spacing);
	inputSizer->AddGrowableCol(1);
	mainSizer->Add(inputSizer);

	mLastSelection = (SignalType)0;

	wxStaticText *signalNameLabel = new wxStaticText(this, wxID_ANY, _T("Signal Name"));
	mSignalNameTextBox = new wxTextCtrl(this, wxID_ANY, GetSignalName(mLastSelection));
	inputSizer->Add(signalNameLabel);
	inputSizer->Add(mSignalNameTextBox, 0, wxGROW);
	inputSizer->AddStretchSpacer();

	wxStaticText *startTimeLabel = new wxStaticText(this, wxID_ANY, _T("Start Time"));
	mStartTimeTextBox = new wxTextCtrl(this, wxID_ANY, wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(startTime), startTime));
	wxStaticText *startTimeUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(startTimeLabel);
	inputSizer->Add(mStartTimeTextBox, 0, wxGROW);
	inputSizer->Add(startTimeUnits);

	wxStaticText *durationLabel = new wxStaticText(this, wxID_ANY, _T("Duration"));
	mDurationTextBox = new wxTextCtrl(this, wxID_ANY, wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(duration), duration));
	wxStaticText *durationUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(durationLabel);
	inputSizer->Add(mDurationTextBox, 0, wxGROW);
	inputSizer->Add(durationUnits);

	wxStaticText *sampleRateLabel = new wxStaticText(this, wxID_ANY, _T("Sample Rate"));
	mSampleRateTextBox = new wxTextCtrl(this, wxID_ANY, wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(sampleRate), sampleRate));
	wxStaticText *sampleRateUnits = new wxStaticText(this, wxID_ANY, _T("Hz"));
	inputSizer->Add(sampleRateLabel);
	inputSizer->Add(mSampleRateTextBox, 0, wxGROW);
	inputSizer->Add(sampleRateUnits);

	wxArrayString signalList;
	unsigned int i;
	for (i = 0; i < static_cast<unsigned int>(SignalType::Count); ++i)
		signalList.Add(GetSignalName(static_cast<SignalType>(i)));

	wxStaticText *typeLabel = new wxStaticText(this, wxID_ANY, _T("Signal Type"));
	mSignalTypeComboBox = new wxComboBox(this, wxID_ANY, GetSignalName(mLastSelection),
		wxDefaultPosition, wxDefaultSize, signalList, wxCB_READONLY);
	inputSizer->Add(typeLabel);
	inputSizer->Add(mSignalTypeComboBox);
	inputSizer->AddStretchSpacer();

	mInitialValueLabel = new wxStaticText(this, wxID_ANY, _T("Initial Value"));
	mInitialValueTextBox = new wxTextCtrl(this, idInitialValue);
	inputSizer->Add(mInitialValueLabel);
	inputSizer->Add(mInitialValueTextBox, 0, wxGROW);
	inputSizer->AddStretchSpacer();

	mFinalValueLabel = new wxStaticText(this, wxID_ANY, _T("Final Value"));
	mFinalValueTextBox = new wxTextCtrl(this, idFinalValue);
	inputSizer->Add(mFinalValueLabel);
	inputSizer->Add(mFinalValueTextBox, 0, wxGROW);
	inputSizer->AddStretchSpacer();

	mSlopeLabel = new wxStaticText(this, wxID_ANY, _T("Slope"));
	mSlopeTextBox = new wxTextCtrl(this, idSlope, _T("1.0"));
	mSlopeUnits = new wxStaticText(this, wxID_ANY, _T("1 / seconds"));
	inputSizer->Add(mSlopeLabel);
	inputSizer->Add(mSlopeTextBox, 0, wxGROW);
	inputSizer->Add(mSlopeUnits);

	mFrequencyLabel = new wxStaticText(this, wxID_ANY, _T("Frequency"));
	mFrequencyTextBox = new wxTextCtrl(this, idFrequency);
	mFrequencyUnits = new wxStaticText(this, wxID_ANY, _T("Hz"));
	inputSizer->Add(mFrequencyLabel);
	inputSizer->Add(mFrequencyTextBox, 0, wxGROW);
	inputSizer->Add(mFrequencyUnits);

	wxStaticText *periodLabel = new wxStaticText(this, wxID_ANY, _T("Period"));
	mPeriodTextBox = new wxTextCtrl(this, idPeriod, _T("0.0"));
	wxStaticText *periodUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(periodLabel);
	inputSizer->Add(mPeriodTextBox, 0, wxGROW);
	inputSizer->Add(periodUnits);

	wxStaticText *phaseAngleLabel = new wxStaticText(this, wxID_ANY, _T("Phase Angle"));
	mPhaseAngleTextBox = new wxTextCtrl(this, idPhaseAngle, _T("0.0"));
	wxStaticText *phaseAngleUnits = new wxStaticText(this, wxID_ANY, _T("deg"));
	inputSizer->Add(phaseAngleLabel);
	inputSizer->Add(mPhaseAngleTextBox, 0, wxGROW);
	inputSizer->Add(phaseAngleUnits);

	wxStaticText *phaseTimeLabel = new wxStaticText(this, wxID_ANY, _T("Phase Delay"));
	mPhaseTimeTextBox = new wxTextCtrl(this, idPhaseTime, _T("0.0"));
	wxStaticText *phaseTimeUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(phaseTimeLabel);
	inputSizer->Add(mPhaseTimeTextBox, 0, wxGROW);
	inputSizer->Add(phaseTimeUnits);

	SetTextBoxLabelsAndEnables();
	SetDefaultInputs();

	mainSizer->AddSpacer(15);

	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 1, wxGROW);

	SetSizerAndFit(topSizer);
	Center();
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		GetSignalName
//
// Description:		Returns the name of the signal specified by the enumeration.
//
// Input Arguments:
//		type	= const SignalType&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString CreateSignalDialog::GetSignalName(const SignalType &type)
{
	assert(static_cast<typename std::underlying_type<SignalType>::type>(
		type) >= 0 && type < SignalType::Count);

	if (type == SignalType::Step)
		return _T("Step");
	else if (type == SignalType::Ramp)
		return _T("Ramp");
	else if (type == SignalType::Sinusoid)
		return _T("Sinusoid");
	else if (type == SignalType::Square)
		return _T("Square");
	else if (type == SignalType::Triangle)
		return _T("Triangle");
	else if (type == SignalType::Sawtooth)
		return _T("Sawtooth");
	else if (type == SignalType::Chirp)
		return _T("Chirp");
	else if (type == SignalType::WhiteNoise)
		return _T("White Noise");

	return _T("Unknown signal type");
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		TransferDataFromWindow
//
// Description:		Validates inputs to the dialog.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for all inputs valid, false otherwise
//
//=============================================================================
bool CreateSignalDialog::TransferDataFromWindow()
{
	if (mSignalNameTextBox->GetValue().IsEmpty())
	{
		wxMessageBox(_T("Signal Name must not be empty."), _T("Error"), 5L, this);
		return false;
	}

	double startTime, duration, sampleRate;
	if (!mStartTimeTextBox->GetValue().ToDouble(&startTime))
	{
		wxMessageBox(_T("Start Time must be numeric."), _T("Error"), 5L, this);
		return false;
	}

	if (!mDurationTextBox->GetValue().ToDouble(&duration))
	{
		wxMessageBox(_T("Duration must be numeric."), _T("Error"), 5L, this);
		return false;
	}
	else if (duration <= 0.0)
	{
		wxMessageBox(_T("Duration must be strictly positive."), _T("Error"), 5L, this);
		return false;
	}

	if (!mSampleRateTextBox->GetValue().ToDouble(&sampleRate))
	{
		wxMessageBox(_T("Sample Rate must be numeric."), _T("Error"), 5L, this);
		return false;
	}
	else if (sampleRate <= 0.0)
	{
		wxMessageBox(_T("Sample Rate must be strictly positive."), _T("Error"), 5L, this);
		return false;
	}

	double value;
	if (!mInitialValueTextBox->GetValue().ToDouble(&value))
	{
		wxMessageBox(mInitialValueLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
		return false;
	}
	else if (mLastSelection == SignalType::Sinusoid ||
		mLastSelection == SignalType::Square ||
		mLastSelection == SignalType::Triangle ||
		mLastSelection == SignalType::Sawtooth ||
		mLastSelection == SignalType::Chirp)
	{
		if (PlotMath::IsZero(value) && wxMessageBox(_T("Amplitude is zero.  Continue anyway?"), _T("Warning"), wxYES_NO, this) == wxNO)
			return false;
	}

	if (!mFinalValueTextBox->GetValue().ToDouble(&value))
	{
		wxMessageBox(mFinalValueLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
		return false;
	}

	if (mLastSelection != SignalType::WhiteNoise)
	{
		if (!mFrequencyTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(mFrequencyLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (mLastSelection != SignalType::Step && mLastSelection != SignalType::Ramp && value < 0.0)
		{
			if (wxMessageBox(_T("Frequency is negative.  Continue anyway?"), _T("Warning"), wxYES_NO, this) == wxNO)
				return false;
		}
	}

	if (mLastSelection == SignalType::Ramp ||
		mLastSelection == SignalType::Square ||
		mLastSelection == SignalType::Triangle ||
		mLastSelection == SignalType::Sawtooth ||
		mLastSelection == SignalType::Chirp)
	{
		if (!mSlopeTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(mSlopeLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (mLastSelection == SignalType::Square)
		{
			if (value < 0.0 || value > 1.0)
			{
				wxMessageBox(mSlopeLabel->GetLabel() + _T(" must be between 0.0 and 1.0."), _T("Error"), 5L, this);
				return false;
			}
		}
	}

	if (mLastSelection != SignalType::Step &&
		mLastSelection != SignalType::Ramp &&
		mLastSelection != SignalType::WhiteNoise)
	{
		if (!mPeriodTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(_T("Period must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (!mPhaseAngleTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(_T("Phase Angle must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (!mPhaseTimeTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(_T("Phase Delay must be numeric."), _T("Error"), 5L, this);
			return false;
		}
	}

	CreateSignal(startTime, duration, sampleRate);

	return true;
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		CreateSignal
//
// Description:		Creates the signal dataset to the user specifications.
//
// Input Arguments:
//		startTime	= const double&
//		duration	= const double&
//		sampleRate	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void CreateSignalDialog::CreateSignal(const double &startTime, const double &duration,
	const double &sampleRate)
{
	mDataset = std::make_unique<Dataset2D>(duration * sampleRate + 1);

	unsigned int i;
	for (i = 0; i < mDataset->GetNumberOfPoints(); ++i)
	{
		const double time(startTime + i / sampleRate);
		mDataset->GetX()[i] = time;
		mDataset->GetY()[i] = GetValue(time);
	}
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		GetValue
//
// Description:		Computes the value at the specified time for the user-specified
//					function.
//
// Input Arguments:
//		time	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double CreateSignalDialog::GetValue(const double &time)
{
	if (mLastSelection == SignalType::Step)
	{
		double eventTime(0.0), initialValue(0.0), finalValue(0.0);
		if (!mFrequencyTextBox->GetValue().ToDouble(&eventTime) ||
			!mInitialValueTextBox->GetValue().ToDouble(&initialValue) ||
			!mFinalValueTextBox->GetValue().ToDouble(&finalValue))
			return 0.0;

		if (time < eventTime)
			return initialValue;
		else
			return finalValue;
	}
	else if (mLastSelection == SignalType::Ramp)
	{
		double eventTime(0.0), initialValue(0.0), finalValue(0.0), slope(0.0);
		if (!mFrequencyTextBox->GetValue().ToDouble(&eventTime) ||
			!mInitialValueTextBox->GetValue().ToDouble(&initialValue) ||
			!mFinalValueTextBox->GetValue().ToDouble(&finalValue) ||
			!mSlopeTextBox->GetValue().ToDouble(&slope))
			return 0.0;

		if (finalValue > initialValue && slope < 0.0)
			slope = -slope;
		else if (finalValue < initialValue && slope > 0.0)
			slope = -slope;

		double eventEndTime((finalValue - initialValue) / slope + eventTime);
		if (time < eventTime)
			return initialValue;
		else if (time >= eventEndTime)
			return finalValue;
		else
			return initialValue + slope * (time - eventTime);
	}
	else if (mLastSelection == SignalType::Sinusoid)
	{
		double amplitude(0.0), offset(0.0), frequency(0.0), phase(0.0);
		if (!mFrequencyTextBox->GetValue().ToDouble(&frequency) ||
			!mInitialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!mFinalValueTextBox->GetValue().ToDouble(&offset) ||
			!mPhaseAngleTextBox->GetValue().ToDouble(&phase))
			return 0.0;

		frequency *= 2.0 * M_PI;
		phase *= M_PI / 180.0;

		return amplitude * sin(frequency * time + phase) + offset;
	}
	else if (mLastSelection == SignalType::Square)
	{
		double amplitude(0.0), offset(0.0), period(0.0), phase(0.0), duty(0.0);
		if (!mPeriodTextBox->GetValue().ToDouble(&period) ||
			!mInitialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!mFinalValueTextBox->GetValue().ToDouble(&offset) ||
			!mPhaseTimeTextBox->GetValue().ToDouble(&phase) ||
			!mSlopeTextBox->GetValue().ToDouble(&duty))
			return 0.0;

		double counter(fmod(time + phase, period));

		if (counter < duty * period)
			return offset + amplitude * 0.5;
		else
			return offset - amplitude * 0.5;
	}
	else if (mLastSelection == SignalType::Triangle)
	{
		double amplitude(0.0), offset(0.0), period(0.0), phase(0.0);
		if (!mPeriodTextBox->GetValue().ToDouble(&period) ||
			!mInitialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!mFinalValueTextBox->GetValue().ToDouble(&offset) ||
			!mPhaseTimeTextBox->GetValue().ToDouble(&phase))
			return 0.0;

		if (fmod(phase + time, period) < period * 0.5)
			return 2.0 * amplitude * fmod(phase + time, period * 0.5) / period + offset - amplitude * 0.5;
		else
			return -2.0 * amplitude * fmod(phase + time, period * 0.5) / period + offset + amplitude * 0.5;
	}
	else if (mLastSelection == SignalType::Sawtooth)
	{
		double amplitude(0.0), offset(0.0), period(0.0), phase(0.0);
		if (!mPeriodTextBox->GetValue().ToDouble(&period) ||
			!mInitialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!mFinalValueTextBox->GetValue().ToDouble(&offset) ||
			!mPhaseTimeTextBox->GetValue().ToDouble(&phase))
			return 0.0;

		return amplitude * fmod(phase + time, period) + offset - amplitude * 0.5;
	}
	else if (mLastSelection == SignalType::Chirp)
	{
		double amplitude(0.0), offset(0.0), frequency(0.0), phase(0.0), slope(0.0);
		if (!mFrequencyTextBox->GetValue().ToDouble(&frequency) ||
			!mInitialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!mFinalValueTextBox->GetValue().ToDouble(&offset) ||
			!mPhaseAngleTextBox->GetValue().ToDouble(&phase) ||
			!mSlopeTextBox->GetValue().ToDouble(&slope))
			return 0.0;

		frequency = frequency + slope * time;
		frequency *= 2.0 * M_PI;
		phase *= M_PI / 180.0;

		return amplitude * sin(frequency * time + phase) + offset;
	}
	else if (mLastSelection == SignalType::WhiteNoise)
	{
		double amplitude(0.0), offset(0.0);
		if (!mInitialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!mFinalValueTextBox->GetValue().ToDouble(&offset))
			return 0.0;

		return fmod(rand() * amplitude / RAND_MAX, amplitude) + offset - amplitude * 0.5;
	}

	assert(false);
	return 0.0;
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnSignalTypeChangeEvent
//
// Description:		Event handler for combo box selection events.
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
void CreateSignalDialog::OnSignalTypeChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	SignalType newType = (SignalType)mSignalTypeComboBox->GetSelection();
	if (GetSignalName(mLastSelection).Cmp(mSignalNameTextBox->GetValue()) == 0 ||
		mSignalNameTextBox->GetValue().IsEmpty())
		mSignalNameTextBox->ChangeValue(GetSignalName(newType));
	mLastSelection = newType;

	SetTextBoxLabelsAndEnables();
	SetDefaultInputs();
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnSignalTypeChangeEvent
//
// Description:		Event handler for combo box selection events.
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
void CreateSignalDialog::SetTextBoxLabelsAndEnables()
{
	// Prepare inputs for non-periodic functions
	if (mLastSelection == SignalType::Step ||
		mLastSelection == SignalType::Ramp)
	{
		mSlopeTextBox->Enable(mLastSelection == SignalType::Ramp);
		mFrequencyTextBox->Enable(true);
		mPeriodTextBox->Enable(false);
		mPhaseAngleTextBox->Enable(false);
		mPhaseTimeTextBox->Enable(false);

		mInitialValueLabel->SetLabel(_T("Initial Value"));
		mFinalValueLabel->SetLabel(_T("Final Value"));

		mSlopeLabel->SetLabel(_T("Slope"));
		mSlopeUnits->SetLabel(_T("1 / seconds"));

		mFrequencyLabel->SetLabel(_T("Event Time"));
		mFrequencyUnits->SetLabel(_T("seconds"));
	}
	// Prepare inputs for periodic functions
	else
	{
		mSlopeTextBox->Enable(mLastSelection != SignalType::Sinusoid && mLastSelection != SignalType::WhiteNoise);
		mFrequencyTextBox->Enable(mLastSelection != SignalType::WhiteNoise);
		mPeriodTextBox->Enable(mLastSelection != SignalType::WhiteNoise);
		mPhaseAngleTextBox->Enable(mLastSelection != SignalType::WhiteNoise);
		mPhaseTimeTextBox->Enable(mLastSelection != SignalType::WhiteNoise);

		mInitialValueLabel->SetLabel(_T("Amplitude"));
		mFinalValueLabel->SetLabel(_T("Offset"));

		if (mLastSelection == SignalType::Chirp)
		{
			mSlopeLabel->SetLabel(_T("Frequency Rate"));
			mSlopeUnits->SetLabel(_T("Hz / seconds"));
			mFrequencyLabel->SetLabel(_T("Initial Frequency"));
		}
		else if (mLastSelection == SignalType::Square)
		{
			mSlopeLabel->SetLabel(_T("Duty Cycle"));
			mSlopeUnits->SetLabel(_T("%"));
			mFrequencyLabel->SetLabel(_T("Frequency"));
		}
		else
		{
			mSlopeLabel->SetLabel(_T("Slope"));
			mSlopeUnits->SetLabel(_T("1 / seconds"));
			mFrequencyLabel->SetLabel(_T("Frequency"));
		}

		mFrequencyUnits->SetLabel(_T("Hz"));
	}
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		SetDefaultInputs
//
// Description:		Sets default inputs depending on the specified signal type.
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
void CreateSignalDialog::SetDefaultInputs()
{
	// Defaults for non-periodic functions
	if (mLastSelection == SignalType::Step ||
		mLastSelection == SignalType::Ramp)
	{
		mInitialValueTextBox->SetValue(_T("0.0"));
		mFinalValueTextBox->SetValue(_T("1.0"));

		if (mLastSelection == SignalType::Ramp)
			mSlopeTextBox->SetValue(_T("1.0"));

		mFrequencyTextBox->SetValue(_T("1.0"));
	}
	// Defaults for periodic functions
	else
	{
		mInitialValueTextBox->SetValue(_T("1.0"));
		mFinalValueTextBox->SetValue(_T("0.0"));
		mFrequencyTextBox->SetValue(_T("1.0"));
		mPhaseAngleTextBox->SetValue(_T("0.0"));

		if (mLastSelection == SignalType::Chirp)
			mSlopeTextBox->SetValue(_T("1.0"));
		else if (mLastSelection == SignalType::Square)
			mSlopeTextBox->SetValue(_T("0.5"));
	}
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnAmplitudeChangeEvent
//
// Description:		Event handler for amplitude text box change events.
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
void CreateSignalDialog::OnAmplitudeChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	if (mLastSelection != SignalType::Triangle &&
		mLastSelection != SignalType::Sawtooth)
		return;

	UpdateSlope();
	mKeepAmplitude = true;
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnSlopeChangeEvent
//
// Description:		Event handler for slope text box change events.
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
void CreateSignalDialog::OnSlopeChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	if (mLastSelection != SignalType::Triangle &&
		mLastSelection != SignalType::Sawtooth)
		return;

	UpdateAmplitude();
	mKeepAmplitude = false;
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnFrequencyChangeEvent
//
// Description:		Event handler for frequency text box change events.
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
void CreateSignalDialog::OnFrequencyChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	if (mLastSelection == SignalType::Step || mLastSelection == SignalType::Ramp)
		return;

	double value;
	if (!mFrequencyTextBox->GetValue().ToDouble(&value))
		return;
	mPeriodTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(1.0 / value), 1.0 / value));

	if (mKeepPhaseAngle)
		UpdatePhaseTime();
	else
		UpdatePhaseAngle();

	if (mKeepAmplitude)
		UpdateSlope();
	else
		UpdateAmplitude();
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnPeriodChangeEvent
//
// Description:		Event handler for period text box change events.
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
void CreateSignalDialog::OnPeriodChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	// Don't execute until we've been fully created
	if (!mPeriodTextBox)
		return;

	double value;
	if (!mPeriodTextBox->GetValue().ToDouble(&value))
		return;
	mFrequencyTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(1.0 / value), 1.0 / value));

	if (mKeepPhaseAngle)
		UpdatePhaseTime();
	else
		UpdatePhaseAngle();
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnPhaseAngleChangeEvent
//
// Description:		Event handler for phase angle text box change events.
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
void CreateSignalDialog::OnPhaseAngleChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdatePhaseTime();
	mKeepPhaseAngle = true;
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		OnPhaseTimeChangeEvent
//
// Description:		Event handler for phase time text box change events.
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
void CreateSignalDialog::OnPhaseTimeChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	UpdatePhaseAngle();
	mKeepPhaseAngle = false;
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		UpdatePhaseAngle
//
// Description:		Updates the phase angle based on frequency and phase time.
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
void CreateSignalDialog::UpdatePhaseAngle()
{
	// Don't execute until we've been fully created
	if (!mFrequencyTextBox || !mPhaseTimeTextBox)
		return;

	double phase(0.0), frequency(0.0);
	if (!mFrequencyTextBox->GetValue().ToDouble(&frequency) ||
		!mPhaseTimeTextBox->GetValue().ToDouble(&phase))
		return;

	mPhaseAngleTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(phase * 360.0 * frequency), phase * 360.0 * frequency));
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		UpdatePhaseTime
//
// Description:		Updates the phase time based on frequency and phase angle.
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
void CreateSignalDialog::UpdatePhaseTime()
{
	// Don't execute until we've been fully created
	if (!mFrequencyTextBox || !mPhaseTimeTextBox)
		return;

	double phase(0.0), frequency(0.0);
	if (!mFrequencyTextBox->GetValue().ToDouble(&frequency) ||
		!mPhaseAngleTextBox->GetValue().ToDouble(&phase))
		return;

	mPhaseTimeTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(phase / 360.0 / frequency), phase / 360.0 / frequency));
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		UpdateAmplitude
//
// Description:		Updates the amplitude based on frequency and slope.
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
void CreateSignalDialog::UpdateAmplitude()
{
	// Don't execute until we've been fully created
	if (!mPeriodTextBox || !mSlopeTextBox)
		return;

	double period(0.0), slope(0.0);
	if (!mPeriodTextBox->GetValue().ToDouble(&period) ||
		!mSlopeTextBox->GetValue().ToDouble(&slope))
		return;

	double factor(1.0);
	if (mLastSelection == SignalType::Triangle)
		factor = 2.0;

	mInitialValueTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(period * slope / factor), period * slope / factor));
}

//=============================================================================
// Class:			CreateSignalDialog
// Function:		UpdateSlope
//
// Description:		Updates the slope based on frequency and amplitude.
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
void CreateSignalDialog::UpdateSlope()
{
	// Don't execute until we've been fully created
	if (!mPeriodTextBox || !mInitialValueTextBox)
		return;

	double period(0.0), amplitude(0.0);
	if (!mPeriodTextBox->GetValue().ToDouble(&period) ||
		!mInitialValueTextBox->GetValue().ToDouble(&amplitude))
		return;

	double factor(1.0);
	if (mLastSelection == SignalType::Triangle)
		factor = 2.0;

	mSlopeTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(amplitude / period * factor), amplitude / period * factor));
}

}// namespace LibPlot2D
