/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  filterDialog.h
// Date:  4/20/2012
// Auth:  K. Loux
// Desc:  Dialog box for defining filter parameters.

#ifndef FILTER_DIALOG_H_
#define FILTER_DIALOG_H_

// Local headers
#include "lp2d/utilities/math/complex.h"

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// wxWidgets forward declarations
class wxSpinCtrl;
class wxSpinEvent;

namespace LibPlot2D
{

struct FilterParameters
{
	enum class Type
	{
		LowPass,
		HighPass,
		BandPass,
		BandStop,
		Notch,
		Custom
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
	FilterDialog(wxWindow *parent, const FilterParameters* parameters = nullptr);

	~FilterDialog() = default;

	const FilterParameters& GetFilterParameters() const { return mParameters; }
	static wxString GetFilterNamePrefix(const FilterParameters &parameters);

private:
	static const unsigned int mDefaultPrecision;
	static const unsigned int mCalculationPrecision;

	static const unsigned int mMaxFilterOrder;

	wxTextCtrl *mCutoffFrequencyBox;
	wxTextCtrl *mDampingRatioBox;
	wxTextCtrl *mWidthBox;

	wxCheckBox *mPhaselessCheckBox;
	wxCheckBox *mButterworthCheckBox;

	wxSpinCtrl *mOrderSpin;

	wxRadioButton *mLowPassRadio;
	wxRadioButton *mHighPassRadio;
	wxRadioButton *mBandStopRadio;
	wxRadioButton *mBandPassRadio;
	wxRadioButton *mNotchRadio;
	wxRadioButton *mCustomRadio;

	wxTextCtrl *mNumeratorBox;
	wxTextCtrl *mDenominatorBox;

	FilterParameters mParameters;

	// Overload from wxDialog
	virtual bool TransferDataFromWindow();

	// Event handlers
	void OnSpinChange(wxSpinEvent &event);
	void OnSpin(wxSpinEvent &event);
	void OnRadioChange(wxCommandEvent &event);
	void OnButterworthChange(wxCommandEvent &event);
	void OnTransferFunctionChange(wxCommandEvent &event);
	void OnInputTextChange(wxCommandEvent &event);

	void HandleSpin();
	void UpdateTransferFunction();
	void UpdateEnabledControls();

	FilterParameters::Type GetType() const;

	bool automaticStringPrecision;
	unsigned int stringPrecision;
	unsigned int DetermineStringPrecision() const;

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

	void CreateControls();
	wxSizer* CreateTextBoxes();
	wxSizer* CreateCheckBoxes();
	wxSizer* CreateRadioButtons();
	wxSizer* CreateTransferFunctionControls();

	bool CutoffFrequencyIsValid();
	bool DampingRatioIsValid();
	bool WidthIsValid();
	bool ExpressionIsValid(const wxString& expression);

	bool DampingRatioInputRequired();

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

	bool mInitialized;

	// For the event table
	DECLARE_EVENT_TABLE();
};

}// namespace LibPlot2D

#endif// FILTER_DIALOG_H_
