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
	initialValueTextBox = nullptr;
	finalValueTextBox = nullptr;
	slopeTextBox = nullptr;
	frequencyTextBox = nullptr;
	periodTextBox = nullptr;
	phaseAngleTextBox = nullptr;
	phaseTimeTextBox = nullptr;

	dataset = nullptr;
	keepPhaseAngle = true;
	keepAmplitude = true;
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
	return signalNameTextBox->GetValue();
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

	lastSelection = (SignalType)0;

	wxStaticText *signalNameLabel = new wxStaticText(this, wxID_ANY, _T("Signal Name"));
	signalNameTextBox = new wxTextCtrl(this, wxID_ANY, GetSignalName(lastSelection));
	inputSizer->Add(signalNameLabel);
	inputSizer->Add(signalNameTextBox, 0, wxGROW);
	inputSizer->AddStretchSpacer();

	wxStaticText *startTimeLabel = new wxStaticText(this, wxID_ANY, _T("Start Time"));
	startTimeTextBox = new wxTextCtrl(this, wxID_ANY, wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(startTime), startTime));
	wxStaticText *startTimeUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(startTimeLabel);
	inputSizer->Add(startTimeTextBox, 0, wxGROW);
	inputSizer->Add(startTimeUnits);

	wxStaticText *durationLabel = new wxStaticText(this, wxID_ANY, _T("Duration"));
	durationTextBox = new wxTextCtrl(this, wxID_ANY, wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(duration), duration));
	wxStaticText *durationUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(durationLabel);
	inputSizer->Add(durationTextBox, 0, wxGROW);
	inputSizer->Add(durationUnits);

	wxStaticText *sampleRateLabel = new wxStaticText(this, wxID_ANY, _T("Sample Rate"));
	sampleRateTextBox = new wxTextCtrl(this, wxID_ANY, wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(sampleRate), sampleRate));
	wxStaticText *sampleRateUnits = new wxStaticText(this, wxID_ANY, _T("Hz"));
	inputSizer->Add(sampleRateLabel);
	inputSizer->Add(sampleRateTextBox, 0, wxGROW);
	inputSizer->Add(sampleRateUnits);

	wxArrayString signalList;
	unsigned int i;
	for (i = 0; i < static_cast<unsigned int>(SignalType::Count); ++i)
		signalList.Add(GetSignalName((SignalType)i));

	wxStaticText *typeLabel = new wxStaticText(this, wxID_ANY, _T("Signal Type"));
	signalTypeComboBox = new wxComboBox(this, wxID_ANY, GetSignalName(lastSelection),
		wxDefaultPosition, wxDefaultSize, signalList, wxCB_READONLY);
	inputSizer->Add(typeLabel);
	inputSizer->Add(signalTypeComboBox);
	inputSizer->AddStretchSpacer();

	initialValueLabel = new wxStaticText(this, wxID_ANY, _T("Initial Value"));
	initialValueTextBox = new wxTextCtrl(this, idInitialValue);
	inputSizer->Add(initialValueLabel);
	inputSizer->Add(initialValueTextBox, 0, wxGROW);
	inputSizer->AddStretchSpacer();

	finalValueLabel = new wxStaticText(this, wxID_ANY, _T("Final Value"));
	finalValueTextBox = new wxTextCtrl(this, idFinalValue);
	inputSizer->Add(finalValueLabel);
	inputSizer->Add(finalValueTextBox, 0, wxGROW);
	inputSizer->AddStretchSpacer();

	slopeLabel = new wxStaticText(this, wxID_ANY, _T("Slope"));
	slopeTextBox = new wxTextCtrl(this, idSlope, _T("1.0"));
	slopeUnits = new wxStaticText(this, wxID_ANY, _T("1 / seconds"));
	inputSizer->Add(slopeLabel);
	inputSizer->Add(slopeTextBox, 0, wxGROW);
	inputSizer->Add(slopeUnits);

	frequencyLabel = new wxStaticText(this, wxID_ANY, _T("Frequency"));
	frequencyTextBox = new wxTextCtrl(this, idFrequency);
	frequencyUnits = new wxStaticText(this, wxID_ANY, _T("Hz"));
	inputSizer->Add(frequencyLabel);
	inputSizer->Add(frequencyTextBox, 0, wxGROW);
	inputSizer->Add(frequencyUnits);

	wxStaticText *periodLabel = new wxStaticText(this, wxID_ANY, _T("Period"));
	periodTextBox = new wxTextCtrl(this, idPeriod, _T("0.0"));
	wxStaticText *periodUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(periodLabel);
	inputSizer->Add(periodTextBox, 0, wxGROW);
	inputSizer->Add(periodUnits);

	wxStaticText *phaseAngleLabel = new wxStaticText(this, wxID_ANY, _T("Phase Angle"));
	phaseAngleTextBox = new wxTextCtrl(this, idPhaseAngle, _T("0.0"));
	wxStaticText *phaseAngleUnits = new wxStaticText(this, wxID_ANY, _T("deg"));
	inputSizer->Add(phaseAngleLabel);
	inputSizer->Add(phaseAngleTextBox, 0, wxGROW);
	inputSizer->Add(phaseAngleUnits);

	wxStaticText *phaseTimeLabel = new wxStaticText(this, wxID_ANY, _T("Phase Delay"));
	phaseTimeTextBox = new wxTextCtrl(this, idPhaseTime, _T("0.0"));
	wxStaticText *phaseTimeUnits = new wxStaticText(this, wxID_ANY, _T("seconds"));
	inputSizer->Add(phaseTimeLabel);
	inputSizer->Add(phaseTimeTextBox, 0, wxGROW);
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
	if (signalNameTextBox->GetValue().IsEmpty())
	{
		wxMessageBox(_T("Signal Name must not be empty."), _T("Error"), 5L, this);
		return false;
	}

	double startTime, duration, sampleRate;
	if (!startTimeTextBox->GetValue().ToDouble(&startTime))
	{
		wxMessageBox(_T("Start Time must be numeric."), _T("Error"), 5L, this);
		return false;
	}

	if (!durationTextBox->GetValue().ToDouble(&duration))
	{
		wxMessageBox(_T("Duration must be numeric."), _T("Error"), 5L, this);
		return false;
	}
	else if (duration <= 0.0)
	{
		wxMessageBox(_T("Duration must be strictly positive."), _T("Error"), 5L, this);
		return false;
	}

	if (!sampleRateTextBox->GetValue().ToDouble(&sampleRate))
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
	if (!initialValueTextBox->GetValue().ToDouble(&value))
	{
		wxMessageBox(initialValueLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
		return false;
	}
	else if (lastSelection == SignalType::Sinusoid ||
		lastSelection == SignalType::Square ||
		lastSelection == SignalType::Triangle ||
		lastSelection == SignalType::Sawtooth ||
		lastSelection == SignalType::Chirp)
	{
		if (PlotMath::IsZero(value) && wxMessageBox(_T("Amplitude is zero.  Continue anyway?"), _T("Warning"), wxYES_NO, this) == wxNO)
			return false;
	}

	if (!finalValueTextBox->GetValue().ToDouble(&value))
	{
		wxMessageBox(finalValueLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
		return false;
	}

	if (lastSelection != SignalType::WhiteNoise)
	{
		if (!frequencyTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(frequencyLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (lastSelection != SignalType::Step && lastSelection != SignalType::Ramp && value < 0.0)
		{
			if (wxMessageBox(_T("Frequency is negative.  Continue anyway?"), _T("Warning"), wxYES_NO, this) == wxNO)
				return false;
		}
	}

	if (lastSelection == SignalType::Ramp ||
		lastSelection == SignalType::Square ||
		lastSelection == SignalType::Triangle ||
		lastSelection == SignalType::Sawtooth ||
		lastSelection == SignalType::Chirp)
	{
		if (!slopeTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(slopeLabel->GetLabel() + _T(" must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (lastSelection == SignalType::Square)
		{
			if (value < 0.0 || value > 1.0)
			{
				wxMessageBox(slopeLabel->GetLabel() + _T(" must be between 0.0 and 1.0."), _T("Error"), 5L, this);
				return false;
			}
		}
	}

	if (lastSelection != SignalType::Step &&
		lastSelection != SignalType::Ramp &&
		lastSelection != SignalType::WhiteNoise)
	{
		if (!periodTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(_T("Period must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (!phaseAngleTextBox->GetValue().ToDouble(&value))
		{
			wxMessageBox(_T("Phase Angle must be numeric."), _T("Error"), 5L, this);
			return false;
		}

		if (!phaseTimeTextBox->GetValue().ToDouble(&value))
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
	dataset = std::make_unique<Dataset2D>(duration * sampleRate + 1);

	unsigned int i;
	for (i = 0; i < dataset->GetNumberOfPoints(); ++i)
	{
		double time(startTime + (double)i / sampleRate);
		dataset->GetX()[i] = time;
		dataset->GetY()[i] = GetValue(time);
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
	if (lastSelection == SignalType::Step)
	{
		double eventTime(0.0), initialValue(0.0), finalValue(0.0);
		if (!frequencyTextBox->GetValue().ToDouble(&eventTime) ||
			!initialValueTextBox->GetValue().ToDouble(&initialValue) ||
			!finalValueTextBox->GetValue().ToDouble(&finalValue))
			return 0.0;

		if (time < eventTime)
			return initialValue;
		else
			return finalValue;
	}
	else if (lastSelection == SignalType::Ramp)
	{
		double eventTime(0.0), initialValue(0.0), finalValue(0.0), slope(0.0);
		if (!frequencyTextBox->GetValue().ToDouble(&eventTime) ||
			!initialValueTextBox->GetValue().ToDouble(&initialValue) ||
			!finalValueTextBox->GetValue().ToDouble(&finalValue) ||
			!slopeTextBox->GetValue().ToDouble(&slope))
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
	else if (lastSelection == SignalType::Sinusoid)
	{
		double amplitude(0.0), offset(0.0), frequency(0.0), phase(0.0);
		if (!frequencyTextBox->GetValue().ToDouble(&frequency) ||
			!initialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!finalValueTextBox->GetValue().ToDouble(&offset) ||
			!phaseAngleTextBox->GetValue().ToDouble(&phase))
			return 0.0;

		frequency *= 2.0 * PlotMath::pi;
		phase *= PlotMath::pi / 180.0;

		return amplitude * sin(frequency * time + phase) + offset;
	}
	else if (lastSelection == SignalType::Square)
	{
		double amplitude(0.0), offset(0.0), period(0.0), phase(0.0), duty(0.0);
		if (!periodTextBox->GetValue().ToDouble(&period) ||
			!initialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!finalValueTextBox->GetValue().ToDouble(&offset) ||
			!phaseTimeTextBox->GetValue().ToDouble(&phase) ||
			!slopeTextBox->GetValue().ToDouble(&duty))
			return 0.0;

		double counter(fmod(time + phase, period));

		if (counter < duty * period)
			return offset + amplitude * 0.5;
		else
			return offset - amplitude * 0.5;
	}
	else if (lastSelection == SignalType::Triangle)
	{
		double amplitude(0.0), offset(0.0), period(0.0), phase(0.0);
		if (!periodTextBox->GetValue().ToDouble(&period) ||
			!initialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!finalValueTextBox->GetValue().ToDouble(&offset) ||
			!phaseTimeTextBox->GetValue().ToDouble(&phase))
			return 0.0;

		if (fmod(phase + time, period) < period * 0.5)
			return 2.0 * amplitude * fmod(phase + time, period * 0.5) / period + offset - amplitude * 0.5;
		else
			return -2.0 * amplitude * fmod(phase + time, period * 0.5) / period + offset + amplitude * 0.5;
	}
	else if (lastSelection == SignalType::Sawtooth)
	{
		double amplitude(0.0), offset(0.0), period(0.0), phase(0.0);
		if (!periodTextBox->GetValue().ToDouble(&period) ||
			!initialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!finalValueTextBox->GetValue().ToDouble(&offset) ||
			!phaseTimeTextBox->GetValue().ToDouble(&phase))
			return 0.0;

		return amplitude * fmod(phase + time, period) + offset - amplitude * 0.5;
	}
	else if (lastSelection == SignalType::Chirp)
	{
		double amplitude(0.0), offset(0.0), frequency(0.0), phase(0.0), slope(0.0);
		if (!frequencyTextBox->GetValue().ToDouble(&frequency) ||
			!initialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!finalValueTextBox->GetValue().ToDouble(&offset) ||
			!phaseAngleTextBox->GetValue().ToDouble(&phase) ||
			!slopeTextBox->GetValue().ToDouble(&slope))
			return 0.0;

		frequency = frequency + slope * time;
		frequency *= 2.0 * PlotMath::pi;
		phase *= PlotMath::pi / 180.0;

		return amplitude * sin(frequency * time + phase) + offset;
	}
	else if (lastSelection == SignalType::WhiteNoise)
	{
		double amplitude(0.0), offset(0.0);
		if (!initialValueTextBox->GetValue().ToDouble(&amplitude) ||
			!finalValueTextBox->GetValue().ToDouble(&offset))
			return 0.0;

		return fmod((double)rand() / RAND_MAX * amplitude, amplitude) + offset - amplitude * 0.5;
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
	SignalType newType = (SignalType)signalTypeComboBox->GetSelection();
	if (GetSignalName(lastSelection).Cmp(signalNameTextBox->GetValue()) == 0 ||
		signalNameTextBox->GetValue().IsEmpty())
		signalNameTextBox->ChangeValue(GetSignalName(newType));
	lastSelection = newType;

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
	if (lastSelection == SignalType::Step ||
		lastSelection == SignalType::Ramp)
	{
		slopeTextBox->Enable(lastSelection == SignalType::Ramp);
		frequencyTextBox->Enable(true);
		periodTextBox->Enable(false);
		phaseAngleTextBox->Enable(false);
		phaseTimeTextBox->Enable(false);

		initialValueLabel->SetLabel(_T("Initial Value"));
		finalValueLabel->SetLabel(_T("Final Value"));

		slopeLabel->SetLabel(_T("Slope"));
		slopeUnits->SetLabel(_T("1 / seconds"));

		frequencyLabel->SetLabel(_T("Event Time"));
		frequencyUnits->SetLabel(_T("seconds"));
	}
	// Prepare inputs for periodic functions
	else
	{
		slopeTextBox->Enable(lastSelection != SignalType::Sinusoid && lastSelection != SignalType::WhiteNoise);
		frequencyTextBox->Enable(lastSelection != SignalType::WhiteNoise);
		periodTextBox->Enable(lastSelection != SignalType::WhiteNoise);
		phaseAngleTextBox->Enable(lastSelection != SignalType::WhiteNoise);
		phaseTimeTextBox->Enable(lastSelection != SignalType::WhiteNoise);

		initialValueLabel->SetLabel(_T("Amplitude"));
		finalValueLabel->SetLabel(_T("Offset"));

		if (lastSelection == SignalType::Chirp)
		{
			slopeLabel->SetLabel(_T("Frequency Rate"));
			slopeUnits->SetLabel(_T("Hz / seconds"));
			frequencyLabel->SetLabel(_T("Initial Frequency"));
		}
		else if (lastSelection == SignalType::Square)
		{
			slopeLabel->SetLabel(_T("Duty Cycle"));
			slopeUnits->SetLabel(_T("%"));
			frequencyLabel->SetLabel(_T("Frequency"));
		}
		else
		{
			slopeLabel->SetLabel(_T("Slope"));
			slopeUnits->SetLabel(_T("1 / seconds"));
			frequencyLabel->SetLabel(_T("Frequency"));
		}

		frequencyUnits->SetLabel(_T("Hz"));
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
	if (lastSelection == SignalType::Step ||
		lastSelection == SignalType::Ramp)
	{
		initialValueTextBox->SetValue(_T("0.0"));
		finalValueTextBox->SetValue(_T("1.0"));

		if (lastSelection == SignalType::Ramp)
			slopeTextBox->SetValue(_T("1.0"));

		frequencyTextBox->SetValue(_T("1.0"));
	}
	// Defaults for periodic functions
	else
	{
		initialValueTextBox->SetValue(_T("1.0"));
		finalValueTextBox->SetValue(_T("0.0"));
		frequencyTextBox->SetValue(_T("1.0"));
		phaseAngleTextBox->SetValue(_T("0.0"));

		if (lastSelection == SignalType::Chirp)
			slopeTextBox->SetValue(_T("1.0"));
		else if (lastSelection == SignalType::Square)
			slopeTextBox->SetValue(_T("0.5"));
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
	if (lastSelection != SignalType::Triangle &&
		lastSelection != SignalType::Sawtooth)
		return;

	UpdateSlope();
	keepAmplitude = true;
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
	if (lastSelection != SignalType::Triangle &&
		lastSelection != SignalType::Sawtooth)
		return;

	UpdateAmplitude();
	keepAmplitude = false;
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
	if (lastSelection == SignalType::Step || lastSelection == SignalType::Ramp)
		return;

	double value;
	if (!frequencyTextBox->GetValue().ToDouble(&value))
		return;
	periodTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(1.0 / value), 1.0 / value));

	if (keepPhaseAngle)
		UpdatePhaseTime();
	else
		UpdatePhaseAngle();

	if (keepAmplitude)
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
	if (!periodTextBox)
		return;

	double value;
	if (!periodTextBox->GetValue().ToDouble(&value))
		return;
	frequencyTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(1.0 / value), 1.0 / value));

	if (keepPhaseAngle)
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
	keepPhaseAngle = true;
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
	keepPhaseAngle = false;
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
	if (!frequencyTextBox || !phaseTimeTextBox)
		return;

	double phase(0.0), frequency(0.0);
	if (!frequencyTextBox->GetValue().ToDouble(&frequency) ||
		!phaseTimeTextBox->GetValue().ToDouble(&phase))
		return;

	phaseAngleTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(phase * 360.0 * frequency), phase * 360.0 * frequency));
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
	if (!frequencyTextBox || !phaseTimeTextBox)
		return;

	double phase(0.0), frequency(0.0);
	if (!frequencyTextBox->GetValue().ToDouble(&frequency) ||
		!phaseAngleTextBox->GetValue().ToDouble(&phase))
		return;

	phaseTimeTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(phase / 360.0 / frequency), phase / 360.0 / frequency));
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
	if (!periodTextBox || !slopeTextBox)
		return;

	double period(0.0), slope(0.0);
	if (!periodTextBox->GetValue().ToDouble(&period) ||
		!slopeTextBox->GetValue().ToDouble(&slope))
		return;

	double factor(1.0);
	if (lastSelection == SignalType::Triangle)
		factor = 2.0;

	initialValueTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(period * slope / factor), period * slope / factor));
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
	if (!periodTextBox || !initialValueTextBox)
		return;

	double period(0.0), amplitude(0.0);
	if (!periodTextBox->GetValue().ToDouble(&period) ||
		!initialValueTextBox->GetValue().ToDouble(&amplitude))
		return;

	double factor(1.0);
	if (lastSelection == SignalType::Triangle)
		factor = 2.0;

	slopeTextBox->ChangeValue(wxString::Format(_T("%0.*f"), PlotMath::GetPrecision(amplitude / period * factor), amplitude / period * factor));
}

}// namespace LibPlot2D
