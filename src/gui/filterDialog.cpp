/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  filterDialog.cpp
// Date:  4/20/2012
// Auth:  K. Loux
// Desc:  Dialog box for defining filter parameters.

// Local headers
#include "lp2d/gui/filterDialog.h"
#include "lp2d/utilities/math/plotMath.h"
#include "lp2d/utilities/math/expressionTree.h"
#include "lp2d/utilities/signals/filter.h"

// wxWidgets headers
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/statline.h>

namespace LibPlot2D
{

//=============================================================================
// Class:			FilterDialog
// Function:		Constant declarations
//
// Description:		Contant declarations for FilterDialog class.
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
const unsigned int FilterDialog::mDefaultPrecision(2);
const unsigned int FilterDialog::mCalculationPrecision(15);
const unsigned int FilterDialog::mMaxFilterOrder(10000);

//=============================================================================
// Class:			FilterDialog
// Function:		FilterDialog
//
// Description:		Constructor for FilterDialog class.
//
// Input Arguments:
//		parent		= wxWindow* that owns this object
//		parameters	= const FilterParameters*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
FilterDialog::FilterDialog(wxWindow *parent,
	const FilterParameters* parameters) : wxDialog(parent, wxID_ANY,
	_T("Specify Filter"), wxDefaultPosition)
{
	if (parameters)
		mParameters = *parameters;

	CreateControls();
	mInitialized = true;

	UpdateEnabledControls();
	UpdateTransferFunction();
}

//=============================================================================
// Class:			FilterDialog
// Function:		Event Table
//
// Description:		Specifies event handlers for dialog events.
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
BEGIN_EVENT_TABLE(FilterDialog, wxDialog)
	EVT_SPINCTRL(SpinID,			FilterDialog::OnSpinChange)
	EVT_SPIN(SpinID,				FilterDialog::OnSpin)
	EVT_RADIOBUTTON(RadioID,		FilterDialog::OnRadioChange)
	EVT_CHECKBOX(ButterworthID,		FilterDialog::OnButterworthChange)
	EVT_TEXT(TransferFunctionID,	FilterDialog::OnTransferFunctionChange)
	EVT_TEXT(InputTextID,			FilterDialog::OnInputTextChange)
END_EVENT_TABLE()

//=============================================================================
// Class:			FilterDialog
// Function:		CreateControls
//
// Description:		Populates the dialog with controls.
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
void FilterDialog::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxGROW, 5);

	mainSizer->Add(CreateRadioButtons());

	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateCheckBoxes(), 0, wxGROW);

	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateTextBoxes(), 0, wxGROW);

	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateTransferFunctionControls(), 1, wxGROW);

	mainSizer->AddSpacer(10);
	wxSizer *buttons = CreateButtonSizer(wxOK | wxCANCEL);
	if (buttons)
		mainSizer->Add(buttons, 0, wxALIGN_CENTER_HORIZONTAL);

	SetSizerAndFit(topSizer);

	Center();
}

//=============================================================================
// Class:			FilterDialog
// Function:		CreateTextBoxes
//
// Description:		Creates the text boxes and returns a sizer containing them.
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
//=============================================================================
wxSizer* FilterDialog::CreateTextBoxes()
{
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 5, 5);
	sizer->AddGrowableCol(1);

	mCutoffFrequencyBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		PlotMath::GetPrecision(mParameters.cutoffFrequency, mStringPrecision), mParameters.cutoffFrequency));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Cutoff Frequency [Hz]")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(mCutoffFrequencyBox, 0, wxEXPAND);

	mDampingRatioBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		PlotMath::GetPrecision(mParameters.dampingRatio, mStringPrecision), mParameters.dampingRatio));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Damping Ratio")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(mDampingRatioBox, 0, wxEXPAND);

	mWidthBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		PlotMath::GetPrecision(mParameters.width, mStringPrecision), mParameters.width));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Width [Hz]")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(mWidthBox, 0, wxEXPAND);

	mOrderSpin = new wxSpinCtrl(this, SpinID, wxString::Format("%i", mParameters.order));
	mOrderSpin->SetRange(1, mMaxFilterOrder);
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Order")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(mOrderSpin);

	return sizer;
}

//=============================================================================
// Class:			FilterDialog
// Function:		CreateCheckBoxes
//
// Description:		Creates the check boxes and returns a sizer containing them.
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
//=============================================================================
wxSizer* FilterDialog::CreateCheckBoxes()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	mButterworthCheckBox = new wxCheckBox(this, ButterworthID, _T("Butterworth"));
	sizer->Add(mButterworthCheckBox, 0, wxALL, 2);

	mPhaselessCheckBox = new wxCheckBox(this, wxID_ANY, _T("Phaseless"));
	sizer->Add(mPhaselessCheckBox, 0, wxALL, 2);

	if (mParameters.butterworth)
		mButterworthCheckBox->SetValue(true);
	if (mParameters.phaseless)
		mPhaselessCheckBox->SetValue(true);

	return sizer;
}

//=============================================================================
// Class:			FilterDialog
// Function:		CreateRadioButtons
//
// Description:		Creates the type-selection radio buttons.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer* containing the radio buttons
//
//=============================================================================
wxSizer* FilterDialog::CreateRadioButtons()
{
	wxBoxSizer *typeSizer = new wxBoxSizer(wxVERTICAL);

	mLowPassRadio = new wxRadioButton(this, RadioID, _T("Low-Pass"));
	mHighPassRadio = new wxRadioButton(this, RadioID, _T("High-Pass"));
	mBandStopRadio = new wxRadioButton(this, RadioID, _T("Band-Stop"));
	mBandPassRadio = new wxRadioButton(this, RadioID, _T("Band-Pass"));
	mNotchRadio = new wxRadioButton(this, RadioID, _T("Notch"));
	mCustomRadio = new wxRadioButton(this, RadioID, _T("Custom"));

	typeSizer->Add(mLowPassRadio, 0, wxALL, 2);
	typeSizer->Add(mHighPassRadio, 0, wxALL, 2);
	typeSizer->Add(mBandStopRadio, 0, wxALL, 2);
	typeSizer->Add(mBandPassRadio, 0, wxALL, 2);
	typeSizer->Add(mNotchRadio, 0, wxALL, 2);
	typeSizer->Add(mCustomRadio, 0, wxALL, 2);

	if (mParameters.type == FilterParameters::Type::HighPass)
		mHighPassRadio->SetValue(true);
	else if (mParameters.type == FilterParameters::Type::LowPass)
		mLowPassRadio->SetValue(true);
	else if (mParameters.type == FilterParameters::Type::BandStop)
		mBandStopRadio->SetValue(true);
	else if (mParameters.type == FilterParameters::Type::BandPass)
		mBandPassRadio->SetValue(true);
	else if (mParameters.type == FilterParameters::Type::Notch)
		mNotchRadio->SetValue(true);
	else if (mParameters.type == FilterParameters::Type::Custom)
		mCustomRadio->SetValue(true);
	else
		assert(false);

	return typeSizer;
}

//=============================================================================
// Class:			FilterDialog
// Function:		CreateTransferFunctionControls
//
// Description:		Creates the text boxes for specifying the filter's transfer
//					function.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer* containing the buttons
//
//=============================================================================
wxSizer* FilterDialog::CreateTransferFunctionControls()
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Transfer Function")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

	mNumeratorBox = new wxTextCtrl(this, TransferFunctionID);
	mDenominatorBox = new wxTextCtrl(this, TransferFunctionID);

	mNumeratorBox->ChangeValue(mParameters.numerator);
	mDenominatorBox->ChangeValue(mParameters.denominator);

	wxBoxSizer *tfSizer = new wxBoxSizer(wxVERTICAL);
	tfSizer->Add(mNumeratorBox, 1, wxGROW | wxALL, 2);
	tfSizer->Add(new wxStaticLine(this), 0, wxGROW | wxALL, 2);
	tfSizer->Add(mDenominatorBox, 1, wxGROW | wxALL, 2);

	sizer->Add(tfSizer, 1, wxGROW);

	return sizer;
}

//=============================================================================
// Class:			FilterDialog
// Function:		OnSpinChange
//
// Description:		Processes spin control change events (order selection).
//
// Input Arguments:
//		event	= wxSpinEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::OnSpinChange(wxSpinEvent& WXUNUSED(event))
{
	HandleSpin();
}

//=============================================================================
// Class:			FilterDialog
// Function:		OnSpin
//
// Description:		Processes spin control change events (order selection).
//
// Input Arguments:
//		event	= wxSpinEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::OnSpin(wxSpinEvent& WXUNUSED(event))
{
	HandleSpin();
}

//=============================================================================
// Class:			FilterDialog
// Function:		HandleSpin
//
// Description:		Processes spin control change events (order selection).
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
void FilterDialog::HandleSpin()
{
	UpdateEnabledControls();
	UpdateTransferFunction();
}

//=============================================================================
// Class:			FilterDialog
// Function:		OnRadioChange
//
// Description:		Processes radio button change events (type selection).
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
void FilterDialog::OnRadioChange(wxCommandEvent& WXUNUSED(event))
{
	if (!mInitialized)
		return;

	if (mBandPassRadio->GetValue() || mBandStopRadio->GetValue())
		mOrderSpin->SetRange(2, mMaxFilterOrder);
	else
		mOrderSpin->SetRange(1, mMaxFilterOrder);

	UpdateEnabledControls();
	UpdateTransferFunction();
}

//=============================================================================
// Class:			FilterDialog
// Function:		OnButterworthChange
//
// Description:		Processes checkbox change events (butterworth selection).
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
//=============================================================================
void FilterDialog::OnButterworthChange(wxCommandEvent& WXUNUSED(event))
{
	UpdateEnabledControls();
	UpdateTransferFunction();
}

//=============================================================================
// Class:			FilterDialog
// Function:		OnInputTextChange
//
// Description:		Processes input text box change events.
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
//=============================================================================
void FilterDialog::OnInputTextChange(wxCommandEvent& WXUNUSED(event))
{
	UpdateEnabledControls();
	UpdateTransferFunction();
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetType
//
// Description:		Returns the currently selected filter type.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		FilterParameters::Type
//
//=============================================================================
FilterParameters::Type FilterDialog::GetType() const
{
	if (!mInitialized)
		return FilterParameters::Type::LowPass;

	if (mHighPassRadio->GetValue())
		return FilterParameters::Type::HighPass;
	else if (mLowPassRadio->GetValue())
		return FilterParameters::Type::LowPass;
	else if (mBandStopRadio->GetValue())
		return FilterParameters::Type::BandStop;
	else if (mBandPassRadio->GetValue())
		return FilterParameters::Type::BandPass;
	else if (mNotchRadio->GetValue())
		return FilterParameters::Type::Notch;
	else if (mCustomRadio->GetValue())
		return FilterParameters::Type::Custom;

	assert(false);
	return FilterParameters::Type::LowPass;
}

//=============================================================================
// Class:			FilterDialog
// Function:		TransferDataFromWindow
//
// Description:		Validates dialog contents prior to allowing OK to be executed.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if dialog contents are valid
//
//=============================================================================
bool FilterDialog::TransferDataFromWindow()
{
	wxString originalNumerator = mNumeratorBox->GetValue();
	wxString originalDenominator = mDenominatorBox->GetValue();

	unsigned int activePrecision = mStringPrecision;

	if (!mCustomRadio->GetValue())
	{
		mStringPrecision = mCalculationPrecision;// Extended precision for computation
		mAutomaticStringPrecision = false;
		UpdateTransferFunction();
	}

	double steadyStateGain = Filter::ComputeSteadyStateGain(
		std::string(mNumeratorBox->GetValue().mb_str()), std::string(mDenominatorBox->GetValue().mb_str()));

	if (!PlotMath::IsZero(steadyStateGain - 1.0) && !PlotMath::IsZero(steadyStateGain))
	{
		if (wxMessageBox(wxString::Format(
			"The steady-state gain for the specified filter is %f (typically 1.0 or 0.0).  Continue anyway?",
			steadyStateGain), _T("Unusual Filter Gain"), wxICON_QUESTION + wxYES_NO, GetParent()) == wxNO)
		{
			mNumeratorBox->SetValue(originalNumerator);
			mDenominatorBox->SetValue(originalDenominator);
			mStringPrecision = activePrecision;

			return false;
		}
	}

	mParameters.order = mOrderSpin->GetValue();
	mParameters.phaseless = mPhaselessCheckBox->GetValue();
	mParameters.butterworth = mButterworthCheckBox->GetValue();
	mParameters.numerator = mNumeratorBox->GetValue();
	mParameters.denominator = mDenominatorBox->GetValue();
	mParameters.type = GetType();

	if (!CutoffFrequencyIsValid() ||
		!DampingRatioIsValid() ||
		!WidthIsValid() ||
		!ExpressionIsValid(mNumeratorBox->GetValue()) ||
		!ExpressionIsValid(mDenominatorBox->GetValue()))
	{
		mNumeratorBox->SetValue(originalNumerator);
		mDenominatorBox->SetValue(originalDenominator);
		mStringPrecision = activePrecision;

		return false;
	}

	return true;
}

//=============================================================================
// Class:			FilterDialog
// Function:		CutoffFrequencyIsValid
//
// Description:		Validates the cutoff frequency value.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if dialog contents are valid
//
//=============================================================================
bool FilterDialog::CutoffFrequencyIsValid()
{
	if (!mCutoffFrequencyBox->GetValue().ToDouble(&mParameters.cutoffFrequency))
	{
		wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Error Defining Filter"));
		return false;
	}
	else if (mParameters.cutoffFrequency <= 0.0)
	{
		wxMessageBox(_T("ERROR:  Cutoff frequency must be strictly positive!"), _T("Error Defining Filter"));
		return false;
	}

	return true;
}

//=============================================================================
// Class:			FilterDialog
// Function:		DampingRatioIsValid
//
// Description:		Validates the damping ratio value.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if dialog contents are valid
//
//=============================================================================
bool FilterDialog::DampingRatioIsValid()
{
	if (DampingRatioInputRequired())
	{
		if (!mDampingRatioBox->GetValue().ToDouble(&mParameters.dampingRatio))
		{
			wxMessageBox(_T("ERROR:  Damping ratio must be numeric!"), _T("Error Defining Filter"));
			return false;
		}
		else if (mParameters.dampingRatio <= 0.0)
		{
			wxMessageBox(_T("ERROR:  Damping ratio must be strictly positive!"), _T("Error Defining Filter"));
			return false;
		}
	}

	return true;
}

//=============================================================================
// Class:			FilterDialog
// Function:		WidthIsValid
//
// Description:		Validates the width value.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if dialog contents are valid
//
//=============================================================================
bool FilterDialog::WidthIsValid()
{
	if (mParameters.type == FilterParameters::Type::BandStop ||
		mParameters.type == FilterParameters::Type::BandPass ||
		mParameters.type == FilterParameters::Type::Notch)
	{
		if (!mWidthBox->GetValue().ToDouble(&mParameters.width))
		{
			wxMessageBox(_T("ERROR:  Width must be numeric!"), _T("Error Defining Filter"));
			return false;
		}
		else if (mParameters.width < 0.0)
		{
			wxMessageBox(_T("ERROR:  Width must be positive!"), _T("Error Defining Filter"));
			return false;
		}
	}

	return true;
}

//=============================================================================
// Class:			FilterDialog
// Function:		ExpressionIsValid
//
// Description:		Validates the expression.
//
// Input Arguments:
//		expression	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if dialog contents are valid
//
//=============================================================================
bool FilterDialog::ExpressionIsValid(const wxString &expression)
{
	if (mParameters.type != FilterParameters::Type::Custom)
		return true;

	ExpressionTree e;
	std::string temp, errorString;
	errorString = e.Solve(std::string(expression.mb_str()), temp);
	if (!errorString.empty())
	{
		wxMessageBox(_T("ERROR:  ") + errorString, _T("Error Defining Filter"));
		return false;
	}

	return true;
}

//=============================================================================
// Class:			FilterDialog
// Function:		UpdateTransferFunction
//
// Description:		Updates the transfer function to match user selections.
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
void FilterDialog::UpdateTransferFunction()
{
	if (!mInitialized || mCustomRadio->GetValue())
		return;

	mStringPrecision = DetermineStringPrecision();

	wxString num, den;
	if (mHighPassRadio->GetValue())
		GetHighPassTF(num, den);
	else if (mLowPassRadio->GetValue())
		GetLowPassTF(num, den);
	else if (mBandStopRadio->GetValue())
		GetBandStopTF(num, den);
	else if (mBandPassRadio->GetValue())
		GetBandPassTF(num, den);
	else if (mNotchRadio->GetValue())
		GetNotchTF(num, den);
	else
		assert(false);

	mNumeratorBox->ChangeValue(num);
	mDenominatorBox->ChangeValue(den);
}

//=============================================================================
// Class:			FilterDialog
// Function:		DetermineStringPrecision
//
// Description:		Determines the best number of digits to use to display the
//					transfer function.  Returns mStringPrecision if
//					automaticStringPrecision is false.
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
//=============================================================================
unsigned int FilterDialog::DetermineStringPrecision() const
{
	if (!mAutomaticStringPrecision)
		return mStringPrecision;

	unsigned int cutoffSigFig, dampingSigFig(0), widthSigFig(0);

	cutoffSigFig = PlotMath::CountSignificantDigits(mCutoffFrequencyBox->GetValue());

	if (mDampingRatioBox->IsEnabled())
		dampingSigFig = PlotMath::CountSignificantDigits(mDampingRatioBox->GetValue());

	if (mWidthBox->IsEnabled())
		widthSigFig = PlotMath::CountSignificantDigits(mWidthBox->GetValue());

	return std::max<unsigned int>(cutoffSigFig, std::max<unsigned int>(dampingSigFig, widthSigFig));
}

//=============================================================================
// Class:			FilterDialog
// Function:		GenerateButterworthDenominator
//
// Description:		Generates the characteristic equation for the poles of a
//					butterworth filter with the specified order and cutoff.
//
// Input Arguments:
//		order	= const unsigned int&
//		cutoff	= const double& [rad/sec]
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GenerateButterworthDenominator(const unsigned int &order,
	const double &cutoff) const
{
	std::vector<Complex> poles;
	unsigned int i;
	for (i = 0; i < order; ++i)
		poles.push_back(Complex(cutoff, 0.0)
			* Complex(exp(1.0), 0.0).ToPower(
			Complex(0.0, (2.0 * (i + 1.0) + order - 1.0) * M_PI / (2.0 * order))));

	return GenerateExpressionFromComplexRoots(poles);
}

//=============================================================================
// Class:			FilterDialog
// Function:		GenerateStandardDenominator
//
// Description:		Generates the characteristic equation for the poles of a
//					standard-form filter with the specified order and cutoff.
//
// Input Arguments:
//		order			= const unsigned int&
//		cutoff			= const double& [rad/sec]
//		dampingRatio	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GenerateStandardDenominator(const unsigned int &order,
	const double &cutoff, const double &dampingRatio) const
{
	wxString s;
	if (order > 1)
		s = wxString::Format("s^2+%0.*f*s+%0.*f",
			PlotMath::GetPrecision(2.0 * cutoff * dampingRatio, mStringPrecision),
			2.0 * cutoff * dampingRatio,
			PlotMath::GetPrecision(cutoff * cutoff, mStringPrecision), cutoff * cutoff);

	if (order > 2)
	{
		s.Prepend(_T("("));
		s.Append(_T(")"));
		if (order > 3)
			s.Append(wxString::Format("^%i", order / 2));
	}

	if (order % 2 == 1)
	{
		if (!s.IsEmpty())
			s.Append(_T("*("));
		s.Append(wxString::Format("s+%0.*f", PlotMath::GetPrecision(cutoff, mStringPrecision), cutoff));
		if (s[0] == '(')
			s.Append(_T(")"));
	}

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GenerateExpressionFromComplexRoots
//
// Description:		Generates a string expression for the characteristic
//					equation with the specified complex roots.
//
// Input Arguments:
//		roots	= const std::vector<Complex>&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GenerateExpressionFromComplexRoots(
	const std::vector<Complex> &roots) const
{
	std::vector<Complex> terms(roots.size() + 1, Complex(0.0, 0.0));
	terms[0].mReal = 1.0;
	unsigned int i, j;
	for (i = 0; i < roots.size(); ++i)// from MATLAB's poly.m
	{
		for (j = i + 1; j > 0; --j)
			terms[j] -= roots[i] * terms[j - 1];
	}

	wxString s, coefficient;
	for (i = 0; i < terms.size(); ++i)
	{
		// TODO:  Check to ensure imaginary part is zero?  I think this is guaranteed through the above math?
		if (!PlotMath::IsZero(terms[i].mReal))
		{
			if (PlotMath::IsZero(terms[i].mReal - 1.0))
				coefficient.Clear();
			else
			{
				coefficient.Printf("+%0.*f", PlotMath::GetPrecision(terms[i].mReal, mStringPrecision), terms[i].mReal);
				if (i != terms.size() - 1)
					coefficient.Append(_T("*"));
			}

			if (i == terms.size() - 1)
				s.Append(coefficient);
			else if (i == terms.size() - 2)
				s.Append(coefficient + _T("s"));
			else
				s.Append(wxString::Format("%ss^%li", coefficient.mb_str(), terms.size() - i - 1));
		}
	}

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetLowPassTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a low-pass filter as specified
//					by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetLowPassTF(wxString &numerator, wxString &denominator) const
{
	double cutoff;
	if (!mCutoffFrequencyBox->GetValue().ToDouble(&cutoff))
		return;
	GetLowPassTF(numerator, denominator, cutoff * 2.0 * M_PI, mOrderSpin->GetValue());
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetHighPassTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a high-pass filter as specified
//					by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetHighPassTF(wxString &numerator, wxString &denominator) const
{
	double cutoff;
	if (!mCutoffFrequencyBox->GetValue().ToDouble(&cutoff))
		return;
	GetHighPassTF(numerator, denominator, cutoff * 2.0 * M_PI, mOrderSpin->GetValue());
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetLowPassTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a low-pass filter as specified
//					by the user.
//
// Input Arguments:
//		cutoff	= const double& [rad/sec]
//		order	= const unisgned int&
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetLowPassTF(wxString &numerator, wxString &denominator,
	const double &cutoff, const unsigned int &order) const
{
	double damping;
	if (order > 1 && !mDampingRatioBox->GetValue().ToDouble(&damping))
		return;

	numerator = wxString::Format("%0.*f", PlotMath::GetPrecision(cutoff, mStringPrecision),
		pow(cutoff, static_cast<int>(order)));

	if (mButterworthCheckBox->GetValue())
		denominator = GenerateButterworthDenominator(order, cutoff);
	else
		denominator = GenerateStandardDenominator(order, cutoff, damping);
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetHighPassTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a high-pass filter as specified
//					by the user.
//
// Input Arguments:
//		cutoff		= const double& [rad/sec]
//		order		= const unisgned int&
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetHighPassTF(wxString &numerator, wxString &denominator,
	const double &cutoff, const unsigned int &order) const
{
	double damping;
	if (order > 1 && !mDampingRatioBox->GetValue().ToDouble(&damping))
		return;

	if (order > 1)
		numerator = wxString::Format("s^%i", order);
	else
		numerator = _T("s");
	if (mButterworthCheckBox->GetValue())
		denominator = GenerateButterworthDenominator(order, cutoff);
	else
		denominator = GenerateStandardDenominator(order, cutoff, damping);
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetBandStopTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a band-stop filter as specified
//					by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetBandStopTF(wxString &numerator, wxString &denominator) const
{
	double cutoff, width(0.0);
	if (!mCutoffFrequencyBox->GetValue().ToDouble(&cutoff) ||
		!mWidthBox->GetValue().ToDouble(&width))
		return;
	cutoff *= 2.0 * M_PI;
	width *= 2.0 * M_PI;

	double lowCutoff, highCutoff;
	ComputeLogCutoffs(cutoff, width, lowCutoff, highCutoff);

	unsigned int order = mOrderSpin->GetValue() / 2;
	wxString highNum, highDen, lowNum, lowDen;
	GetLowPassTF(lowNum, lowDen, lowCutoff, order);
	GetHighPassTF(highNum, highDen, highCutoff, mOrderSpin->GetValue() - order);
	numerator = _T("(") + highNum + _T(")*(") + lowDen + _T(")+(") + lowNum + _T(")*(") + highDen + _T(")");
	denominator = _T("(") + highDen + _T(")*(") + lowDen + _T(")");
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetNotchTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a band-stop filter as specified
//					by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetNotchTF(wxString &numerator, wxString &denominator) const
{
	double cutoff, width(0.0);
	if (!mCutoffFrequencyBox->GetValue().ToDouble(&cutoff) ||
		!mWidthBox->GetValue().ToDouble(&width))
		return;
	cutoff *= 2.0 * M_PI;
	width *= 2.0 * M_PI;

	// Note that the numerator cutoff can be varied to get a high-pass notch or low-pass notch
	numerator.Printf("s^2+%0.*f", PlotMath::GetPrecision(cutoff * cutoff, mStringPrecision), cutoff * cutoff);
	denominator = GenerateStandardDenominator(2, cutoff, width / cutoff * 0.5);
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetBandPassTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a band-pass filter as specified
//					by the user.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		numerator	= wxString&
//		denominator	= wxString&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::GetBandPassTF(wxString &numerator, wxString &denominator) const
{
	double cutoff, width(0.0);
	if (!mCutoffFrequencyBox->GetValue().ToDouble(&cutoff) ||
		!mWidthBox->GetValue().ToDouble(&width))
		return;
	cutoff *= 2.0 * M_PI;
	width *= 2.0 * M_PI;

	double lowCutoff, highCutoff;
	ComputeLogCutoffs(cutoff, width, lowCutoff, highCutoff);

	unsigned int order = mOrderSpin->GetValue() / 2;
	wxString highNum, highDen, lowNum, lowDen;
	GetLowPassTF(lowNum, lowDen, highCutoff, order);
	GetHighPassTF(highNum, highDen, lowCutoff, mOrderSpin->GetValue() - order);
	numerator = _T("(") + highNum + _T(")*(") + lowNum + _T(")");
	denominator = _T("(") + highDen + _T(")*(") + lowDen + _T(")");
}

//=============================================================================
// Class:			FilterDialog
// Function:		OnTransferFunctionChange
//
// Description:		Selects custom radio button if the transfer function boxes
//					change by user input.
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
//=============================================================================
void FilterDialog::OnTransferFunctionChange(wxCommandEvent& WXUNUSED(event))
{
	if (!mInitialized)
		return;

	mCustomRadio->SetValue(true);
	UpdateEnabledControls();
}

//=============================================================================
// Class:			FilterDialog
// Function:		UpdateEnabledControls
//
// Description:		Makes sure the proper controls are enabled/disabled.
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
void FilterDialog::UpdateEnabledControls()
{
	if (!mInitialized)
		return;

	mCutoffFrequencyBox->Enable(!mCustomRadio->GetValue());
	mButterworthCheckBox->Enable(mLowPassRadio->GetValue() || mHighPassRadio->GetValue() ||
		mBandStopRadio->GetValue() || mBandPassRadio->GetValue());
	mDampingRatioBox->Enable(DampingRatioInputRequired());

	mOrderSpin->Enable(mLowPassRadio->GetValue() || mHighPassRadio->GetValue() ||
		mBandStopRadio->GetValue() || mBandPassRadio->GetValue());
	mWidthBox->Enable(mBandStopRadio->GetValue() || mBandPassRadio->GetValue() ||
		mNotchRadio->GetValue());
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetFilterNamePrefix
//
// Description:		Generates string describing specified filter.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString describing filter
//
//=============================================================================
wxString FilterDialog::GetFilterNamePrefix(const FilterParameters &parameters)
{
	wxString name;
	if (parameters.type == FilterParameters::Type::HighPass)
		name = GetHighPassName(parameters);
	else if (parameters.type == FilterParameters::Type::LowPass)
		name = GetLowPassName(parameters);
	else if (parameters.type == FilterParameters::Type::BandStop)
		name = GetBandStopName(parameters);
	else if (parameters.type == FilterParameters::Type::BandPass)
		name = GetBandPassName(parameters);
	else if (parameters.type == FilterParameters::Type::Notch)
		name = GetNotchName(parameters);
	else if (parameters.type == FilterParameters::Type::Custom)
		name = GetCustomName(parameters);
	else
		assert(false);

	if (parameters.phaseless)
		name.Append(", Phaseless");

	return name;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetOrderString
//
// Description:		Returns a string representing the indicated order.
//
// Input Arguments:
//		order	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetOrderString(const unsigned int &order)
{
	wxString orderString(wxString::Format("%u", order));
	if (orderString.Last() == '1' && order != 11)
		orderString.Append(_T("st"));
	else if (orderString.Last() == '2' && order != 12)
		orderString.Append(_T("nd Order"));
	else if (orderString.Last() == '3' && order != 13)
		orderString.Append(_T("rd Order"));
	else
		orderString.Append(_T("th Order"));

	return orderString;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetHighPassName
//
// Description:		Returns a name for the specified filter parameters.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetHighPassName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("High-Pass"), parameters));
	s = AddDampingName(s, parameters);

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetLowPassName
//
// Description:		Returns a name for the specified filter parameters.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetLowPassName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Low-Pass"), parameters));
	s = AddDampingName(s, parameters);

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetBandStopName
//
// Description:		Returns a name for the specified filter parameters.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetBandStopName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Band-Stop"), parameters));
	s = AddWidthName(s, parameters);

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetNotchName
//
// Description:		Returns a name for the specified filter parameters.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetNotchName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Notch"), parameters));
	s = AddWidthName(s, parameters);

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetBandPassName
//
// Description:		Returns a name for the specified filter parameters.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetBandPassName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Band-Pass"), parameters));
	s = AddWidthName(s, parameters);

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetHighPassName
//
// Description:		Returns a name for the specified filter parameters.
//
// Input Arguments:
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetCustomName(const FilterParameters &parameters)
{
	return parameters.numerator + _T(" / ") + parameters.denominator;
}

//=============================================================================
// Class:			FilterDialog
// Function:		GetPrimaryName
//
// Description:		Returns the initial portion of the name for the filter.
//
// Input Arguments:
//		name		= const wxString&
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::GetPrimaryName(const wxString& name, const FilterParameters &parameters)
{
	wxString s(GetOrderString(parameters.order));
	s.Append(wxString::Format(" %s, %0.*f Hz", name.mb_str(),
		PlotMath::GetPrecision(parameters.cutoffFrequency), parameters.cutoffFrequency));

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		AddDampingName
//
// Description:		Adds damping to the name (if applicable).
//
// Input Arguments:
//		name		= const wxString&
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::AddDampingName(const wxString& name, const FilterParameters &parameters)
{
	wxString s(name);
	if (parameters.order > 1 + static_cast<unsigned int>(parameters.phaseless))
	{
		if (parameters.butterworth)
			s.Append(_T(", Butterworth"));
		else
			s.Append(wxString::Format(", zeta = %0.*f",
			PlotMath::GetPrecision(parameters.dampingRatio), parameters.dampingRatio));
	}

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		AddWidthName
//
// Description:		Adds width and depth fields to the name.
//
// Input Arguments:
//		name		= const wxString&
//		parameters	= const FilterParameters&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//=============================================================================
wxString FilterDialog::AddWidthName(const wxString& name, const FilterParameters &parameters)
{
	wxString s(name);
	s.Append(wxString::Format(" x %0.*f Hz",
		PlotMath::GetPrecision(parameters.width), parameters.width));

	return s;
}

//=============================================================================
// Class:			FilterDialog
// Function:		ComputeLogCutoffs
//
// Description:		Computes upper and lower cutoff frequencies for wide-band
//					filters given the center frequency and width.
//
// Input Arguments:
//		center	= const double&
//		width	= const double&
//
// Output Arguments:
//		lowCutoff	= double&
//		highCutoff	= double&
//
// Return Value:
//		None
//
//=============================================================================
void FilterDialog::ComputeLogCutoffs(const double &center, const double &width,
	double &lowCutoff, double &highCutoff) const
{
	// Let the parameters exactly set the center frequency and upper cutoff
	// Compute the lower cutoff using our log() method
	highCutoff = center + width * 0.5;
	lowCutoff = pow(10.0, 2.0 * log10(center) - log10(highCutoff));
}

//=============================================================================
// Class:			FilterDialog
// Function:		DampingRatioInputRequired
//
// Description:		According to selected options, determines if the damping
//					ratio input box needs to be enabled.
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
//=============================================================================
bool FilterDialog::DampingRatioInputRequired()
{
	if ((mButterworthCheckBox->IsEnabled() && mButterworthCheckBox->GetValue()) ||
		mCustomRadio->GetValue() ||
		mNotchRadio->GetValue())
		return false;

	if ((mLowPassRadio->GetValue() || mHighPassRadio->GetValue()) &&
		mOrderSpin->GetValue() > 1)
		return true;

	if ((mBandStopRadio->GetValue() || mBandPassRadio->GetValue()) &&
		mOrderSpin->GetValue() > 2)
		return true;

	return false;
}

//=============================================================================
// Class:			FilterDialog
// Function:		OrderIsValid
//
// Description:		Determines if the order value is valid, according to the
//					other options.
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
//=============================================================================
/*bool FilterDialog::OrderIsValid()
{
	if (mCustomRadio->GetValue() ||
		mNotchRadio->GetValue())
		return true;

	if (mOrderSpin->GetValue() < 1)
		return false;

	if ((mBandPassRadio->GetValue() || mBandStopRadio->GetValue()) &&
		mOrderSpin->GetValue() < 2)
		return false;

	return true;
}*/

}// namespace LibPlot2D
