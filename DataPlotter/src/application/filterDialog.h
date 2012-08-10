/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

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
	enum Type
	{
		TypeLowPass,
		TypeHighPass,
		TypeBandPass,
		TypeBandStop,
		TypeNotch
	} type;

	bool phaseless;
	unsigned int order;
	double cutoffFrequency;// [Hz]
	double dampingRatio;
};

struct FilterOrderLimits
{
	struct MinOrder
	{
		static const unsigned int lowPass = 1;
		static const unsigned int highPass = 1;
	};

	struct MaxOrder
	{
		static const unsigned int lowPass = 2;
		static const unsigned int highPass = 1;
	};
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
	void OnSpinUp(wxSpinEvent &event);
	void OnSpinDown(wxSpinEvent &event);
	void OnRadioChange(wxCommandEvent &event);
	void OnCheckboxChange(wxCommandEvent &event);

	virtual bool TransferDataFromWindow(void);

	void HandleSpin(wxSpinEvent &event);
	void SetCorrectLimits(void);
	bool OrderIsValid(const unsigned int &order) const;

	FilterParameters::Type GetType(void) const;
	unsigned int GetMinOrder(const FilterParameters::Type &type) const;
	unsigned int GetMaxOrder(const FilterParameters::Type &type) const;

	enum EventIDs
	{
		RadioID,
		CheckboxID,
		SpinID
	};

	void CreateControls(void);
	void CreateTextBoxes(wxFlexGridSizer *sizer);
	wxBoxSizer* CreateRadioButtons(void);
	wxBoxSizer* CreateDialogButtons(void);

	bool CutoffFrequencyIsValid(void);
	bool DampingRatioIsValid(void);

	// For the event table
	DECLARE_EVENT_TABLE();
};

#endif// _FILTER_DIALOG_H_