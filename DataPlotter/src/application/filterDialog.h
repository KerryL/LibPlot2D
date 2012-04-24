/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filterDialog.h
// Created:  4/20/2012
// Author:  K. Loux
// Description:  Dialog box for defining filter parameters.
// History:

#ifndef _FILTER_DIALOG_H_
#define _FILTER_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward
class wxSpinCtrl;
class wxSpinEvent;

struct FilterParameters
{
	// FIXME:  Could be made more general (gain, specify TF numerator and denominator etc.)
	enum Type
	{
		TypeLowPass,
		TypeHighPass
		// FIXME:  Band pass, band stop, notch, etc.
	} type;

	bool phaseless;
	unsigned int order;
	double cutoffFrequency;// [Hz]
	double dampingRatio;
};

class FilterDialog : public wxDialog
{
public:
	// Constructor
	FilterDialog(wxWindow *parent, const FilterParameters* _parameters = NULL);

	const FilterParameters& GetFilterParameters(void) const { return parameters; };
	static wxString GetFilterNamePrefix(const FilterParameters &parameters);

private:
	wxTextCtrl *cutoffFrequencyBox;
	wxTextCtrl *dampingRatioBox;

	wxCheckBox *phaselessCheckBox;
	wxSpinCtrl *orderSpin;

	wxRadioButton *lowPassRadio;
	wxRadioButton *highPassRadio;

	FilterParameters parameters;

	// Overload from wxDialog
	virtual void OnOKButton(wxCommandEvent &event);

	// Event handlers
	void OnSpinChange(wxSpinEvent &event);
	void OnRadioChange(wxCommandEvent &event);
	void OnCheckboxChange(wxCommandEvent &event);

	void SetCorrectLimits(void);

	enum EventIDs
	{
		RadioID,
		CheckboxID,
		SpinID
	};

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _FILTER_DIALOG_H_