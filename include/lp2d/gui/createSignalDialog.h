/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  createSignalDialog.h
// Date:  8/19/2013
// Auth:  K. Loux
// Desc:  Dialog for creating a variety of signals (not dependent on other curves).

#ifndef CREATE_SIGNAL_DIALOG_H_
#define CREATE_SIGNAL_DIALOG_H_

// wxWidgets headers
#include <wx/dialog.h>

// Standard C++ headers
#include <memory>

// wxWidgets forward declarations
class wxTextCtrl;

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

class CreateSignalDialog : public wxDialog
{
public:
	CreateSignalDialog(wxWindow *parent, const double &startTime,
		const double &duration, const double &sampleRate);

	~CreateSignalDialog() = default;

	std::unique_ptr<Dataset2D>& GetSignal() { return mDataset; }

	wxString GetSignalName() const;

private:
	void CreateControls(const double &startTime, const double &duration,
		const double &sampleRate);

	wxTextCtrl *mSignalNameTextBox;
	wxTextCtrl *mStartTimeTextBox;
	wxTextCtrl *mDurationTextBox;
	wxTextCtrl *mSampleRateTextBox;

	wxComboBox *mSignalTypeComboBox;

	wxTextCtrl *mInitialValueTextBox;// or Amplitude
	wxTextCtrl *mFinalValueTextBox;// or Offset
	wxTextCtrl *mSlopeTextBox;// or Frequency Rate
	wxTextCtrl *mFrequencyTextBox;// or Event Time
	wxTextCtrl *mPeriodTextBox;
	wxTextCtrl *mPhaseAngleTextBox;
	wxTextCtrl *mPhaseTimeTextBox;

	wxStaticText *mInitialValueLabel;
	wxStaticText *mFinalValueLabel;
	wxStaticText *mSlopeLabel;
	wxStaticText *mSlopeUnits;
	wxStaticText *mFrequencyLabel;
	wxStaticText *mFrequencyUnits;

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

	std::unique_ptr<Dataset2D> mDataset;
	void CreateSignal(const double &startTime, const double &duration,
		const double &sampleRate);
	double GetValue(const double &time);

	enum class SignalType
	{
		Step,
		Ramp,
		Sinusoid,
		Square,
		Triangle,
		Sawtooth,
		Chirp,
		WhiteNoise,

		Count
	};

	static wxString GetSignalName(const SignalType &type);

	bool TransferDataFromWindow() override;

	void OnSignalTypeChangeEvent(wxCommandEvent &event);
	void OnAmplitudeChangeEvent(wxCommandEvent &event);
	void OnSlopeChangeEvent(wxCommandEvent &event);
	void OnFrequencyChangeEvent(wxCommandEvent &event);
	void OnPeriodChangeEvent(wxCommandEvent &event);
	void OnPhaseAngleChangeEvent(wxCommandEvent &event);
	void OnPhaseTimeChangeEvent(wxCommandEvent &event);

	void SetTextBoxLabelsAndEnables();
	void SetDefaultInputs();

	void UpdatePhaseAngle();
	void UpdatePhaseTime();

	void UpdateAmplitude();
	void UpdateSlope();

	SignalType mLastSelection;
	bool mKeepPhaseAngle;
	bool mKeepAmplitude;

	DECLARE_EVENT_TABLE();
};

}// namespace LibPlot2D

#endif// CREATE_SIGNAL_DIALOG_H_
