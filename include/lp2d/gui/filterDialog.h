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

/// Structure for storing information about filter parameters.
struct FilterParameters
{
	/// Enumeration for pre-defined filter types.
	enum class Type
	{
		LowPass,
		HighPass,
		BandPass,
		BandStop,
		Notch,
		Custom
	} type = FilterParameters::Type::LowPass;

	/// Flag indicating if the filter should be applied in both directions.
	bool phaseless = false;

	/// Flag indicating if the filter should be computed using Butterworth
	/// coefficients.
	bool butterworth = false;

	unsigned int order = 2;///< The order of the filter.

	double cutoffFrequency = 5.0;///< Filter cutoff frequency. <b>[Hz]</b>
	double dampingRatio = 1.0;///< Filter damping ratio.
	double width = 5.0;///< Filter width. <b>[Hz]</b>

	wxString numerator;///< Numerator of transfer function.
	wxString denominator;///< Denominator of transfer function.
};

/// Dialog allowing the user to specify filter parameters.
class FilterDialog : public wxDialog
{
public:
	/// Constructor.
	///
	/// \param parent     Pointer to window that owns this.
	/// \param parameters Default values for filter dialog.
	FilterDialog(wxWindow *parent, const FilterParameters* parameters = nullptr);
	~FilterDialog() = default;

	/// Gets the parameters associated with the current user selections.
	/// \returns The parameters associated with the current user selections.
	const FilterParameters& GetFilterParameters() const { return mParameters; }

	/// Gets the prefix for the filter name according to the specified
	/// parameters.
	///
	/// \param parameters Filter parameters for which the prefix is desired.
	///
	/// \returns The appropriate prefix.
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
	bool TransferDataFromWindow() override;

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

	bool mAutomaticStringPrecision = true;
	unsigned int mStringPrecision = mDefaultPrecision;
	bool mInitialized = false;

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

	// For the event table
	DECLARE_EVENT_TABLE();
};

}// namespace LibPlot2D

#endif// FILTER_DIALOG_H_
