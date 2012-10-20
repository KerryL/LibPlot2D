/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filterDialog.cpp
// Created:  4/20/2012
// Author:  K. Loux
// Description:  Dialog box for defining filter parameters.
// History:

// Local headers
#include "application/filterDialog.h"
#include "utilities/math/complex.h"
#include "utilities/math/plotMath.h"
#include "utilities/math/expressionTree.h"

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/statline.h>

//==========================================================================
// Class:			FilterDialog
// Function:		FilterDialog
//
// Description:		Constructor for FilterDialog class.
//
// Input Arguments:
//		parent		= wxWindow* that owns this object
//		_parameters	= const FilterParameters*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
FilterDialog::FilterDialog(wxWindow *parent, const FilterParameters* _parameters)
									 : wxDialog(parent, wxID_ANY, _T("Specify Filter"), wxDefaultPosition)
{
	initialized = false;

	if (_parameters)
		parameters = *_parameters;
	else
	{
		parameters.cutoffFrequency = 5.0;
		parameters.dampingRatio = 1.0;
		parameters.order = 2;
		parameters.type = FilterParameters::TypeLowPass;
		parameters.phaseless = false;
		parameters.butterworth = false;
		parameters.width = parameters.cutoffFrequency;
		parameters.depth = -10.0;
		parameters.numerator.Clear();
		parameters.denominator.Clear();
	}

	CreateControls();

	initialized = true;

	UpdateEnabledControls();
	UpdateTransferFunction();
}

//==========================================================================
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
//==========================================================================
BEGIN_EVENT_TABLE(FilterDialog, wxDialog)
	EVT_BUTTON(wxID_OK,				FilterDialog::OnOKButton)
	EVT_SPINCTRL(SpinID,			FilterDialog::OnSpinChange)
	EVT_SPIN_UP(SpinID,				FilterDialog::OnSpinUp)
	EVT_SPIN_DOWN(SpinID,			FilterDialog::OnSpinDown)
	EVT_RADIOBUTTON(RadioID,		FilterDialog::OnRadioChange)
	EVT_CHECKBOX(ButterworthID,		FilterDialog::OnButterworthChange)
	EVT_TEXT(TransferFunctionID,	FilterDialog::OnTransferFunctionChange)
	EVT_TEXT(InputTextID,			FilterDialog::OnInputTextChange)
END_EVENT_TABLE()

//==========================================================================
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
//==========================================================================
void FilterDialog::CreateControls(void)
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
	mainSizer->Add(CreateDialogButtons(), 0, wxALIGN_CENTER_HORIZONTAL);

	SetSizerAndFit(topSizer);

	Center();
}

//==========================================================================
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
//==========================================================================
wxSizer* FilterDialog::CreateTextBoxes(void)
{
	wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 5, 5);
	sizer->AddGrowableCol(1);

	cutoffFrequencyBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		GetPrecision(parameters.cutoffFrequency), parameters.cutoffFrequency));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Cutoff Frequency [Hz]")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(cutoffFrequencyBox, 0, wxEXPAND);

	dampingRatioBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		GetPrecision(parameters.dampingRatio), parameters.dampingRatio));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Damping Ratio")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(dampingRatioBox, 0, wxEXPAND);

	widthBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		GetPrecision(parameters.width), parameters.width));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Width [Hz]")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(widthBox, 0, wxEXPAND);

	depthBox = new wxTextCtrl(this, InputTextID, wxString::Format("%0.*f",
		GetPrecision(parameters.depth), parameters.depth));
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Depth [dB]")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(depthBox, 0, wxEXPAND);

	orderSpin = new wxSpinCtrl(this, SpinID, wxString::Format("%i", parameters.order));
	orderSpin->SetRange(1, 10000);
	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Order")), wxALIGN_CENTER_VERTICAL);
	sizer->Add(orderSpin);

	return sizer;
}

//==========================================================================
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
//==========================================================================
wxSizer* FilterDialog::CreateCheckBoxes(void)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	butterworthCheckBox = new wxCheckBox(this, ButterworthID, _T("Butterworth"));
	sizer->Add(butterworthCheckBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

	phaselessCheckBox = new wxCheckBox(this, wxID_ANY, _T("Phaseless"));
	sizer->Add(phaselessCheckBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

	if (parameters.butterworth)
		butterworthCheckBox->SetValue(true);
	if (parameters.phaseless)
		phaselessCheckBox->SetValue(true);

	return sizer;
}

//==========================================================================
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
//==========================================================================
wxSizer* FilterDialog::CreateRadioButtons(void)
{
	wxBoxSizer *typeSizer = new wxBoxSizer(wxVERTICAL);

	lowPassRadio = new wxRadioButton(this, RadioID, _T("Low-Pass"));
	highPassRadio = new wxRadioButton(this, RadioID, _T("High-Pass"));
	bandStopRadio = new wxRadioButton(this, RadioID, _T("Band-Stop"));
	bandPassRadio = new wxRadioButton(this, RadioID, _T("Band-Pass"));
	notchRadio = new wxRadioButton(this, RadioID, _T("Notch"));
	customRadio = new wxRadioButton(this, RadioID, _T("Custom"));

	typeSizer->Add(lowPassRadio, 0, wxALL, 2);
	typeSizer->Add(highPassRadio, 0, wxALL, 2);
	typeSizer->Add(bandStopRadio, 0, wxALL, 2);
	typeSizer->Add(bandPassRadio, 0, wxALL, 2);
	typeSizer->Add(notchRadio, 0, wxALL, 2);
	typeSizer->Add(customRadio, 0, wxALL, 2);

	if (parameters.type == FilterParameters::TypeHighPass)
		highPassRadio->SetValue(true);
	else if (parameters.type == FilterParameters::TypeLowPass)
		lowPassRadio->SetValue(true);
	else if (parameters.type == FilterParameters::TypeBandStop)
		bandStopRadio->SetValue(true);
	else if (parameters.type == FilterParameters::TypeBandPass)
		bandPassRadio->SetValue(true);
	else if (parameters.type == FilterParameters::TypeNotch)
		notchRadio->SetValue(true);
	else if (parameters.type == FilterParameters::TypeCustom)
		customRadio->SetValue(true);
	else
		assert(false);

	return typeSizer;
}

//==========================================================================
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
//==========================================================================
wxSizer* FilterDialog::CreateTransferFunctionControls(void)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	sizer->Add(new wxStaticText(this, wxID_ANY, _T("Transfer Function")), 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

	numeratorBox = new wxTextCtrl(this, TransferFunctionID);
	denominatorBox = new wxTextCtrl(this, TransferFunctionID);

	numeratorBox->ChangeValue(parameters.numerator);
	denominatorBox->ChangeValue(parameters.denominator);

	wxBoxSizer *tfSizer = new wxBoxSizer(wxVERTICAL);
	tfSizer->Add(numeratorBox, 1, wxGROW | wxALL, 2);
	tfSizer->Add(new wxStaticLine(this), 0, wxGROW | wxALL, 2);
	tfSizer->Add(denominatorBox, 1, wxGROW | wxALL, 2);

	sizer->Add(tfSizer, 1, wxGROW);

	return sizer;
}

//==========================================================================
// Class:			FilterDialog
// Function:		CreateDialogButtons
//
// Description:		Creates the OK and Cancel buttons at the bottom of the dialog.
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
//==========================================================================
wxSizer* FilterDialog::CreateDialogButtons(void)
{
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"));
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"));

	okButton->SetDefault();

	buttonSizer->Add(okButton, 1, wxALL, 5);
	buttonSizer->Add(cancelButton, 1, wxALL, 5);

	return buttonSizer;
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnOKButton
//
// Description:		Validates data and passes command to default handler.
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
//==========================================================================
void FilterDialog::OnOKButton(wxCommandEvent &event)
{
	parameters.order = orderSpin->GetValue();
	parameters.phaseless = phaselessCheckBox->GetValue();
	parameters.butterworth = butterworthCheckBox->GetValue();
	parameters.numerator = numeratorBox->GetValue();
	parameters.denominator = denominatorBox->GetValue();

	if (lowPassRadio->GetValue())
		parameters.type = FilterParameters::TypeLowPass;
	else if (highPassRadio->GetValue())
		parameters.type = FilterParameters::TypeHighPass;
	else if (bandStopRadio->GetValue())
		parameters.type = FilterParameters::TypeBandStop;
	else if (bandPassRadio->GetValue())
		parameters.type = FilterParameters::TypeBandPass;
	else if (notchRadio->GetValue())
		parameters.type = FilterParameters::TypeNotch;
	else if (customRadio->GetValue())
		parameters.type = FilterParameters::TypeCustom;
	else
		assert(false);

	if (!CutoffFrequencyIsValid() ||
		!DampingRatioIsValid() ||
		!WidthIsValid() ||
		!DepthIsValid() ||
		!ExpressionIsValid(numeratorBox->GetValue()) ||
		!ExpressionIsValid(denominatorBox->GetValue()))
		return;

	event.Skip();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnSpinChange
//
// Description:		Processes spin control change events (order selection).
//
// Input Arguments:
//		event	= wxSpinEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FilterDialog::OnSpinChange(wxSpinEvent& WXUNUSED(event))
{
	/*if (!initialized)
		return;

	if (!OrderIsValid(event.GetInt()))
		event.Veto();

	UpdateEnabledControls();
	UpdateTransferFunction();*/
	HandleSpin();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnSpinUp
//
// Description:		Processes spin control change events (order selection).
//
// Input Arguments:
//		event	= wxSpinEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FilterDialog::OnSpinUp(wxSpinEvent& WXUNUSED(event))
{
	HandleSpin();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnSpinDown
//
// Description:		Processes spin control change events (order selection).
//
// Input Arguments:
//		event	= wxSpinEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FilterDialog::OnSpinDown(wxSpinEvent& WXUNUSED(event))
{
	HandleSpin();
}

//==========================================================================
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
//==========================================================================
void FilterDialog::HandleSpin(void)
{
	/*if (!initialized)
		return;

	if (!OrderIsValid(event.GetInt()))
	{
		unsigned int order(orderSpin->GetValue());
		if ((unsigned)event.GetInt() > order)// increasing value
		{
			order = event.GetInt();
			while (!OrderIsValid(order))
				order++;
		}
		else if (event.GetInt() > 1)// decreasing value AND greater than 1
		{
			order = event.GetInt();
			while (!OrderIsValid(order))
				order--;
		}
		event.Veto();
		orderSpin->SetValue(order);
	}*/

	UpdateEnabledControls();
	UpdateTransferFunction();
}

//==========================================================================
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
//==========================================================================
void FilterDialog::OnRadioChange(wxCommandEvent& WXUNUSED(event))
{
	if (!initialized)
		return;

	UpdateEnabledControls();
	UpdateTransferFunction();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnPhaselessChange
//
// Description:		Processes checkbox change events (phaseless selection).
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
//==========================================================================
/*void FilterDialog::OnPhaselessChange(wxCommandEvent& WXUNUSED(event))
{
	if (!initialized)
		return;

	unsigned int originalOrder = orderSpin->GetValue();
	if (phaselessCheckBox->GetValue())
		orderSpin->SetValue(originalOrder * 2);
	else
		orderSpin->SetValue(originalOrder / 2);

	UpdateEnabledControls();
	UpdateTransferFunction();
}*/

//==========================================================================
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
//==========================================================================
void FilterDialog::OnButterworthChange(wxCommandEvent& WXUNUSED(event))
{
	UpdateEnabledControls();
	UpdateTransferFunction();
}

//==========================================================================
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
//==========================================================================
void FilterDialog::OnInputTextChange(wxCommandEvent& WXUNUSED(event))
{
	UpdateTransferFunction();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OrderIsValid
//
// Description:		Verifies that the specified order is acceptable given the
//					other filter parameters
//
// Input Arguments:
//		order	= const unsigned int& specifying the desired order
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if order is OK
//
//==========================================================================
/*bool FilterDialog::OrderIsValid(const unsigned int &order) const
{
	if (!initialized)
		return false;

	if (phaselessCheckBox->GetValue() && order % 2 != 0)
		return false;

	return true;
}*/

//==========================================================================
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
//==========================================================================
FilterParameters::Type FilterDialog::GetType(void) const
{
	if (!initialized)
		return FilterParameters::TypeLowPass;

	if (highPassRadio->GetValue())
		return FilterParameters::TypeHighPass;
	else if (lowPassRadio->GetValue())
		return FilterParameters::TypeLowPass;
	else if (bandStopRadio->GetValue())
		return FilterParameters::TypeBandStop;
	else if (bandPassRadio->GetValue())
		return FilterParameters::TypeBandPass;
	else if (notchRadio->GetValue())
		return FilterParameters::TypeNotch;
	else if (customRadio->GetValue())
		return FilterParameters::TypeCustom;

	assert(false);
	return FilterParameters::TypeLowPass;
}

//==========================================================================
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
//==========================================================================
bool FilterDialog::TransferDataFromWindow(void)
{
	return true;//OrderIsValid(orderSpin->GetValue());
}

//==========================================================================
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
//==========================================================================
bool FilterDialog::CutoffFrequencyIsValid(void)
{
	if (!cutoffFrequencyBox->GetValue().ToDouble(&parameters.cutoffFrequency))
	{
		wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Error Defining Filter"));
		return false;
	}
	else if (parameters.cutoffFrequency <= 0.0)
	{
		wxMessageBox(_T("ERROR:  Cutoff frequency must be positive!"), _T("Error Defining Filter"));
		return false;
	}

	return true;
}

//==========================================================================
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
//==========================================================================
bool FilterDialog::DampingRatioIsValid(void)
{
	if (!parameters.butterworth && parameters.type != FilterParameters::TypeCustom)
	{
		if (!dampingRatioBox->GetValue().ToDouble(&parameters.dampingRatio))
		{
			wxMessageBox(_T("ERROR:  Damping ratio must be numeric!"), _T("Error Defining Filter"));
			return false;
		}
		else if (parameters.dampingRatio <= 0.0)
		{
			wxMessageBox(_T("ERROR:  Damping ratio must be strictly positive!"), _T("Error Defining Filter"));
			return false;
		}
	}

	return true;
}

//==========================================================================
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
//==========================================================================
bool FilterDialog::WidthIsValid(void)
{
	if (parameters.type == FilterParameters::TypeBandStop ||
		parameters.type == FilterParameters::TypeBandPass)
	{
		if (!widthBox->GetValue().ToDouble(&parameters.width))
		{
			wxMessageBox(_T("ERROR:  Width must be numeric!"), _T("Error Defining Filter"));
			return false;
		}
		else if (parameters.width < 0.0)
		{
			wxMessageBox(_T("ERROR:  Width must be positive!"), _T("Error Defining Filter"));
			return false;
		}
	}

	return true;
}

//==========================================================================
// Class:			FilterDialog
// Function:		DepthIsValid
//
// Description:		Validates the depth value.
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
//==========================================================================
bool FilterDialog::DepthIsValid(void)
{
	if (parameters.type == FilterParameters::TypeBandStop ||
		parameters.type == FilterParameters::TypeBandPass)
	{
		if (!depthBox->GetValue().ToDouble(&parameters.depth))
		{
			wxMessageBox(_T("ERROR:  Depth must be numeric!"), _T("Error Defining Filter"));
			return false;
		}
		/*else if (parameters.depth <= 0.0)
		{
			::wxMessageBox(_T("ERROR:  Depth must be strictly positive!"), _T("Error Defining Filter"));
			return false;
		}*/
	}

	return true;
}

//==========================================================================
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
//==========================================================================
bool FilterDialog::ExpressionIsValid(const wxString &expression)
{
	if (parameters.type != FilterParameters::TypeCustom)
		return true;

	ExpressionTree e;
	std::string temp, errorString;
	errorString = e.Solve(expression.c_str(), temp);
	if (!errorString.empty())
	{
		wxMessageBox(_T("ERROR:  ") + errorString, _T("Error Defining Filter"));
		return false;
	}

	return true;
}

//==========================================================================
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
//==========================================================================
void FilterDialog::UpdateTransferFunction(void)
{
	if (!initialized || customRadio->GetValue())
		return;

	wxString num, den;
	if (highPassRadio->GetValue())
		GetHighPassTF(num, den);
	else if (lowPassRadio->GetValue())
		GetLowPassTF(num, den);
	else if (bandStopRadio->GetValue())
		GetBandStopTF(num, den);
	else if (bandPassRadio->GetValue())
		GetBandPassTF(num, den);
	else if (notchRadio->GetValue())
		GetNotchTF(num, den);
	else
		assert(false);

	numeratorBox->ChangeValue(num);
	denominatorBox->ChangeValue(den);
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GenerateButterworthDenominator(const unsigned int &order,
	const double &cutoff) const
{
	std::vector<Complex> poles, terms(order + 1, Complex(0.0, 0.0));
	unsigned int i;
	for (i = 0; i < order; i++)
		poles.push_back(Complex(cutoff, 0.0)
			* Complex(exp(1.0), 0.0).ToPower(
			Complex(0.0, (2.0 * (i + 1.0) + order - 1.0) * M_PI / (2.0 * order))));

	terms[0].real = 1.0;
	unsigned int j;
	for (i = 0; i < order; i++)// from MATLAB's poly.m
	{
		for (j = i + 1; j > 0; j--)
			terms[j] -= poles[i] * Complex(terms[j - 1]);
	}

	wxString s, coefficient;
	for (i = 0; i < terms.size(); i++)
	{
		// FIXME:  Check to ensure imaginary part is zero?
		if (!PlotMath::IsZero(terms[i].real))
		{
			if (PlotMath::IsZero(terms[i].real - 1.0))
				coefficient.Clear();
			else
			{
				coefficient.Printf("+%0.*f", GetPrecision(terms[i].real), terms[i].real);
				if (i != terms.size() - 1)
					coefficient.Append(_T("*"));
			}

			if (i == terms.size() - 1)
				s.Append(coefficient);
			else if (i == terms.size() - 2)
				s.Append(coefficient + _T("s"));
			else
				s.Append(wxString::Format("%ss^%li", coefficient.c_str(), terms.size() - i - 1));
		}
	}

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GenerateStandardDenominator(const unsigned int &order,
	const double &cutoff, const double &dampingRatio) const
{
	wxString s;
	if (order > 1)
		s = wxString::Format("s^2+%0.*f*s+%0.*f",
			GetPrecision(2.0 * cutoff * dampingRatio),
			2.0 * cutoff * dampingRatio, GetPrecision(cutoff * cutoff), cutoff * cutoff);

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
		s.Append(wxString::Format("s+%0.*f", GetPrecision(cutoff), cutoff));
		if (s[0] == '(')
			s.Append(_T(")"));
	}

	return s;
}

//==========================================================================
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
//==========================================================================
void FilterDialog::GetLowPassTF(wxString &numerator, wxString &denominator) const
{
	double cutoff, damping;
	if (!cutoffFrequencyBox->GetValue().ToDouble(&cutoff))
		return;
	else if (orderSpin->GetValue() > 1 && !dampingRatioBox->GetValue().ToDouble(&damping))
		return;

	cutoff *= 2.0 * M_PI;// [rad/sec]
	if (orderSpin->GetValue() > 1)
		numerator = wxString::Format("%0.*f^%i", GetPrecision(cutoff), cutoff, orderSpin->GetValue());
	else
		numerator = wxString::Format("%0.*f", GetPrecision(cutoff), cutoff);
	if (butterworthCheckBox->GetValue())
		denominator = GenerateButterworthDenominator(orderSpin->GetValue(), cutoff);
	else
		denominator = GenerateStandardDenominator(orderSpin->GetValue(), cutoff, damping);
}

//==========================================================================
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
//==========================================================================
void FilterDialog::GetHighPassTF(wxString &numerator, wxString &denominator) const
{
	double cutoff, damping;
	if (!cutoffFrequencyBox->GetValue().ToDouble(&cutoff))
		return;
	else if (orderSpin->GetValue() > 1 && !dampingRatioBox->GetValue().ToDouble(&damping))
		return;

	if (orderSpin->GetValue() > 1)
		numerator = wxString::Format("s^%i", orderSpin->GetValue());
	else
		numerator = _T("s");
	if (butterworthCheckBox->GetValue())
		denominator = GenerateButterworthDenominator(orderSpin->GetValue(), cutoff);
	else
		denominator = GenerateStandardDenominator(orderSpin->GetValue(), cutoff, damping);
}

//==========================================================================
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
//==========================================================================
void FilterDialog::GetBandStopTF(wxString &numerator, wxString &denominator) const
{
	/*wxString highNum, highDen, lowNum, lowDen;
	GetLowPassTF(lowNum, lowDen, );
	GetHighPassTF(highNum, highDen);
	numerator = _T("(") + highNum + _T(")*(") + lowNum + _T(")");
	denominator = _T("(") + highDen + _T(")*(") + lowDen + _T(")");*/
	// FIXME:  Implement
	numerator.Clear();
	denominator.Clear();
}

//==========================================================================
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
//==========================================================================
void FilterDialog::GetBandPassTF(wxString &numerator, wxString &denominator) const
{
	numerator.Clear();
	denominator.Clear();
	// FIXME:  Implement
}

//==========================================================================
// Class:			FilterDialog
// Function:		GetNotchTF
//
// Description:		Populates the arguments with string descriptors for the
//					numerator and denominator of a notch filter as specified
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
//==========================================================================
void FilterDialog::GetNotchTF(wxString &numerator, wxString &denominator) const
{
	numerator.Clear();
	denominator.Clear();
	// FIXME:  Implement
}

//==========================================================================
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
//==========================================================================
void FilterDialog::OnTransferFunctionChange(wxCommandEvent& WXUNUSED(event))
{
	if (!initialized)
		return;

	customRadio->SetValue(true);
	UpdateEnabledControls();
}

//==========================================================================
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
//==========================================================================
void FilterDialog::UpdateEnabledControls(void)
{
	if (!initialized)
		return;

	butterworthCheckBox->Enable(!customRadio->GetValue());

	cutoffFrequencyBox->Enable(!customRadio->GetValue());
	dampingRatioBox->Enable((orderSpin->GetValue() > 2 ||
		(orderSpin->GetValue() > 1 && !phaselessCheckBox->GetValue())) &&
		!customRadio->GetValue() && !butterworthCheckBox->GetValue());

	orderSpin->Enable(!customRadio->GetValue() && !bandStopRadio->GetValue() && !bandPassRadio->GetValue());
	widthBox->Enable(bandStopRadio->GetValue() || bandPassRadio->GetValue());
	depthBox->Enable(bandStopRadio->GetValue() || bandPassRadio->GetValue());
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetFilterNamePrefix(const FilterParameters &parameters)
{
	wxString name;
	if (parameters.type == FilterParameters::TypeHighPass)
		name = GetHighPassName(parameters);
	else if (parameters.type == FilterParameters::TypeLowPass)
		name = GetLowPassName(parameters);
	else if (parameters.type == FilterParameters::TypeBandStop)
		name = GetBandStopName(parameters);
	else if (parameters.type == FilterParameters::TypeBandPass)
		name = GetBandPassName(parameters);
	else if (parameters.type == FilterParameters::TypeNotch)
		name = GetNotchName(parameters);
	else if (parameters.type == FilterParameters::TypeCustom)
		name = GetCustomName(parameters);
	else
		assert(false);

	if (parameters.phaseless)
		name.Append(", Phaseless");

	return name;
}

//==========================================================================
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
//==========================================================================
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

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetHighPassName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("High-Pass"), parameters));
	s = AddDampingName(s, parameters);

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetLowPassName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Low-Pass"), parameters));
	s = AddDampingName(s, parameters);

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetBandStopName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Band-Stop"), parameters));
	s = AddWidthDepthName(s, parameters);

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetBandPassName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Band-Pass"), parameters));
	s = AddWidthDepthName(s, parameters);

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetNotchName(const FilterParameters &parameters)
{
	wxString s(GetPrimaryName(_T("Notch"), parameters));
	s = AddWidthDepthName(s, parameters);

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetCustomName(const FilterParameters &parameters)
{
	return parameters.numerator + _T(" / ") + parameters.denominator;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::GetPrimaryName(const wxString& name, const FilterParameters &parameters)
{
	wxString s(GetOrderString(parameters.order));
	s.Append(wxString::Format(" %s, %0.*f Hz", name.c_str(),
		GetPrecision(parameters.cutoffFrequency), parameters.cutoffFrequency));

	return s;
}

//==========================================================================
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
//==========================================================================
wxString FilterDialog::AddDampingName(const wxString& name, const FilterParameters &parameters)
{
	wxString s(name);
	if (parameters.order > 1 + (unsigned int)parameters.phaseless)
	{
		if (parameters.butterworth)
			s.Append(_T(", Butterworth"));
		else
			s.Append(wxString::Format(", zeta = %0.*f",
			GetPrecision(parameters.dampingRatio), parameters.dampingRatio));
	}

	return s;
}

//==========================================================================
// Class:			FilterDialog
// Function:		AddWidthDepthName
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
//==========================================================================
wxString FilterDialog::AddWidthDepthName(const wxString& name, const FilterParameters &parameters)
{
	wxString s(name);
	s.Append(wxString::Format(" x %0.*f Hz, %0.*f dB",
		GetPrecision(parameters.width), parameters.width,
		GetPrecision(parameters.depth), parameters.depth));

	return s;
}

//==========================================================================
// Class:			FilterDialog
// Function:		GetPrecision
//
// Description:		Determines the appropriate number of significant digits
//					to use when representing the specified value.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int, number of places after the decimal
//
//==========================================================================
unsigned int FilterDialog::GetPrecision(const double &value)
{
	return 1;// FIXME:  Implement
}