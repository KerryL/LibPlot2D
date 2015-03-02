/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  filterDialog.h
// Created:  4/20/2012
// Author:  K. Loux
// Description:  Dialog box for defining filter parameters.
// History:

#ifndef _FILTER_DIALOG_H_
#define _FILTER_DIALOG_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/math/complex.h"

// wxWidgets forward declarations
class wxSpinCtrl;
class wxSpinEvent;

struct FilterParameters
{
	enum Type
	{
		TypeLowPass,
		TypeHighPass,
		TypeBandPass,
		TypeBandStop,
		TypeNotch,
		TypeCustom
	} type;

	bool phaseless;
	bool butterworth;

	unsigned int order;

	double cutoffFrequency;// [Hz]
	double dampingRatio;
	double width;// [Hz]

	wxString numerator;
	wxString denominator;
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
	wxTextCtrl *widthBox;

	wxCheckBox *phaselessCheckBox;
	wxCheckBox *butterworthCheckBox;

	wxSpinCtrl *orderSpin;

	wxRadioButton *lowPassRadio;
	wxRadioButton *highPassRadio;
	wxRadioButton *bandStopRadio;
	wxRadioButton *bandPassRadio;
	wxRadioButton *notchRadio;
	wxRadioButton *customRadio;

	wxTextCtrl *numeratorBox;
	wxTextCtrl *denominatorBox;

	FilterParameters parameters;

	// Overload from wxDialog
	virtual void OnOKButton(wxCommandEvent &event);
	virtual bool TransferDataFromWindow(void);

	// Event handlers
	void OnSpinChange(wxSpinEvent &event);
	void OnSpinUp(wxSpinEvent &event);
	void OnSpinDown(wxSpinEvent &event);
	void OnRadioChange(wxCommandEvent &event);
	void OnButterworthChange(wxCommandEvent &event);
	void OnTransferFunctionChange(wxCommandEvent &event);
	void OnInputTextChange(wxCommandEvent &event);

	void HandleSpin(void);
	void UpdateTransferFunction(void);
	void UpdateEnabledControls(void);

	FilterParameters::Type GetType(void) const;

	bool automaticStringPrecision;
	unsigned int stringPrecision;
	unsigned int DetermineStringPrecision(void) const;

	void GetLowPassTF(wxString &numerator, wxString &denominator) const;
	void GetHighPassTF(wxString &numerator, wxString &denominator) const;
	void GetLowPassTF(wxString &numerator, wxString &denominator,
		const double &cutoff, const unsigned int &order) const;
	void GetHighPassTF(wxString &numerator, wxString &denominator,
		const double &cutoff, const unsigned int &order) const;
	void GetBandStopTF(wxString &numerator, wxString &denominator) const;
	void GetNotchTF(wxString &numerator, wxString &denominator) const;
	void GetBandPassTF(wxString &numerator, wxString &denominator) const;

	wxString GenerateButterworthDenominator(const unsigned int &order,
		const double &cutoff) const;
	wxString GenerateStandardDenominator(const unsigned int &order,
		const double &cutoff, const double &dampingRatio) const;
	wxString GenerateExpressionFromComplexRoots(const std::vector<Complex> &roots) const;

	enum EventIDs
	{
		RadioID = wxID_HIGHEST + 200,
		ButterworthID,
		SpinID,
		TransferFunctionID,
		InputTextID
	};

	void CreateControls(void);
	wxSizer* CreateTextBoxes(void);
	wxSizer* CreateCheckBoxes(void);
	wxSizer* CreateRadioButtons(void);
	wxSizer* CreateTransferFunctionControls(void);

	bool CutoffFrequencyIsValid(void);
	bool DampingRatioIsValid(void);
	bool WidthIsValid(void);
	bool ExpressionIsValid(const wxString& expression);

	static wxString GetOrderString(const unsigned int &order);
	static wxString GetPrimaryName(const wxString& name, const FilterParameters &parameters);
	static wxString AddDampingName(const wxString& name, const FilterParameters &parameters);
	static wxString AddWidthName(const wxString& name, const FilterParameters &parameters);

	static wxString GetHighPassName(const FilterParameters &parameters);
	static wxString GetLowPassName(const FilterParameters &parameters);
	static wxString GetBandStopName(const FilterParameters &parameters);
	static wxString GetBandPassName(const FilterParameters &parameters);
	static wxString GetNotchName(const FilterParameters &parameters);
	static wxString GetCustomName(const FilterParameters &parameters);

	void ComputeLogCutoffs(const double &center, const double &width,
		double &lowCutoff, double &highCutoff) const;

	bool initialized;

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _FILTER_DIALOG_H_