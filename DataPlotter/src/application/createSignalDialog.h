/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  createSignalDialog.h
// Created:  8/19/2013
// Author:  K. Loux
// Description:  Dialog for creating a variety of signals (not dependent on other curves).
// History:

#ifndef _CREATE_SIGNAL_DIALOG_H_
#define _CREATE_SIGNAL_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

// wxWidgets forward declarations
class wxTextCtrl;

// Local forward declarations
class Dataset2D;

class CreateSignalDialog : public wxDialog
{
public:
	CreateSignalDialog(wxWindow *parent, const double &startTime, const double &duration,
		const double &sampleRate);

	Dataset2D *GetSignal(void) const { return dataset; };
	wxString GetSignalName(void) const;

private:
	void CreateControls(const double &startTime, const double &duration,
		const double &sampleRate);

	wxTextCtrl *signalNameTextBox;
	wxTextCtrl *startTimeTextBox;
	wxTextCtrl *durationTextBox;
	wxTextCtrl *sampleRateTextBox;

	wxComboBox *signalTypeComboBox;

	wxTextCtrl *initialValueTextBox;// or Amplitude
	wxTextCtrl *finalValueTextBox;// or Offset
	wxTextCtrl *slopeTextBox;// or Frequency Rate
	wxTextCtrl *frequencyTextBox;// or Event Time
	wxTextCtrl *periodTextBox;
	wxTextCtrl *phaseAngleTextBox;
	wxTextCtrl *phaseTimeTextBox;

	wxStaticText *initialValueLabel;
	wxStaticText *finalValueLabel;
	wxStaticText *slopeLabel;
	wxStaticText *slopeUnits;
	wxStaticText *frequencyLabel;
	wxStaticText *frequencyUnits;
	/*wxStaticText *periodLabel;
	wxStaticText *periodUnits;
	wxStaticText *phaseAngleLabel;
	wxStaticText *phaseAngleUnits;
	wxStaticText *phaseTimeLabel;
	wxStaticText *phaseTimeUnits;*/

	enum EventIDs
	{
		idInitialValue = wxID_HIGHEST + 400,
		idFinalValue,
		idSlope,
		idFrequency,
		idPeriod,
		idPhaseAngle,
		idPhaseTime
	};

	Dataset2D *dataset;
	void CreateSignal(const double &startTime, const double &duration,
		const double &sampleRate);
	double GetValue(const double &time);

	enum SignalType
	{
		SignalStep,
		SignalRamp,
		SignalSinusoid,
		SignalSquare,
		SignalTriangle,
		SignalSawtooth,
		SignalChirp,
		SignalWhiteNoise,

		SignalCount
	};

	static wxString GetSignalName(const SignalType &type);

	virtual bool TransferDataFromWindow(void);

	void OnSignalTypeChangeEvent(wxCommandEvent &event);
	void OnAmplitudeChangeEvent(wxCommandEvent &event);
	void OnSlopeChangeEvent(wxCommandEvent &event);
	void OnFrequencyChangeEvent(wxCommandEvent &event);
	void OnPeriodChangeEvent(wxCommandEvent &event);
	void OnPhaseAngleChangeEvent(wxCommandEvent &event);
	void OnPhaseTimeChangeEvent(wxCommandEvent &event);

	void SetTextBoxLabelsAndEnables(void);
	void SetDefaultInputs(void);
	SignalType lastSelection;

	void UpdatePhaseAngle(void);
	void UpdatePhaseTime(void);
	bool keepPhaseAngle;

	void UpdateAmplitude(void);
	void UpdateSlope(void);
	bool keepAmplitude;

	DECLARE_EVENT_TABLE();
};

#endif// _CREATE_SIGNAL_DIALOG_H_
