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

// wxWidgets headers
#include <wx/spinctrl.h>
#include <wx/radiobut.h>

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
	// Initialize some members to NULL to avoid errors during initialization
	highPassRadio = NULL;
	lowPassRadio = NULL;

	// Set up default parameters if none specified
	if (_parameters)
		parameters = *_parameters;
	else
	{
		parameters.cutoffFrequency = 5.0;
		parameters.dampingRatio = 1.0;
		parameters.order = 2;
		parameters.type = FilterParameters::TypeLowPass;
		parameters.phaseless = false;
	}

	CreateControls();
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
	EVT_BUTTON(wxID_OK,	FilterDialog::OnOKButton)
	EVT_SPINCTRL(SpinID, FilterDialog::OnSpinChange)
	EVT_SPIN_UP(SpinID, FilterDialog::OnSpinUp)
	EVT_SPIN_DOWN(SpinID, FilterDialog::OnSpinDown)
	EVT_RADIOBUTTON(RadioID, FilterDialog::OnRadioChange)
	EVT_CHECKBOX(CheckboxID, FilterDialog::OnCheckboxChange)
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
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 5);

	wxFlexGridSizer *inputSizer = new wxFlexGridSizer(2, 5, 5);
	mainSizer->Add(inputSizer, 0, wxEXPAND);
	inputSizer->AddGrowableCol(1);

	CreateTextBoxes(inputSizer);
	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateRadioButtons());

	UpdateSpin();
	UpdateDamping();
	mainSizer->AddSpacer(10);
	mainSizer->Add(CreateDialogButtons(), 0, wxALIGN_CENTER_HORIZONTAL);

	// Set the sizer to this dialog
	SetSizerAndFit(topSizer);

	Center();
}

//==========================================================================
// Class:			FilterDialog
// Function:		CreateTextBoxes
//
// Description:		Creates the text boxes and adds them to the sizer.
//
// Input Arguments:
//		sizer	= wxFlexGridSizer* to which the boxes are added
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FilterDialog::CreateTextBoxes(wxFlexGridSizer *sizer)
{
	// Create the text boxes and their labels
	wxString valueString;
	wxStaticText *cutoffLabel = new wxStaticText(this, wxID_ANY, _T("Cutoff Frequency"));
	valueString.Printf("%f", parameters.cutoffFrequency);
	cutoffFrequencyBox = new wxTextCtrl(this, wxID_ANY, valueString);
	sizer->Add(cutoffLabel, wxALIGN_CENTER_VERTICAL);
	sizer->Add(cutoffFrequencyBox, 0, wxEXPAND);

	wxStaticText *dampingLabel = new wxStaticText(this, wxID_ANY, _T("Damping Ratio"));
	valueString.Printf("%f", parameters.dampingRatio);
	dampingRatioBox = new wxTextCtrl(this, wxID_ANY, valueString);
	sizer->Add(dampingLabel, wxALIGN_CENTER_VERTICAL);
	sizer->Add(dampingRatioBox, 0, wxEXPAND);

	phaselessCheckBox = new wxCheckBox(this, CheckboxID, _T("Phaseless"));
	orderSpin = new wxSpinCtrl(this, SpinID, _T("Order"));
	orderSpin->SetRange(1, 2);
	orderSpin->SetValue(parameters.order);
	sizer->Add(phaselessCheckBox, wxALIGN_CENTER_VERTICAL);
	sizer->Add(orderSpin);
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
//		wxBoxSizer* containing the radio buttons
//
//==========================================================================
wxBoxSizer* FilterDialog::CreateRadioButtons(void)
{
	wxBoxSizer *typeSizer = new wxBoxSizer(wxVERTICAL);

	lowPassRadio = new wxRadioButton(this, RadioID, _T("Low-Pass"));
	highPassRadio = new wxRadioButton(this, RadioID, _T("High-Pass"));

	typeSizer->Add(lowPassRadio, 0, wxALL, 2);
	typeSizer->Add(highPassRadio, 0, wxALL, 2);

	if (parameters.type == FilterParameters::TypeHighPass)
		highPassRadio->SetValue(true);
	else
		lowPassRadio->SetValue(true);

	return typeSizer;
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
//		wxBoxSizer* containing the buttons
//
//==========================================================================
wxBoxSizer* FilterDialog::CreateDialogButtons(void)
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

	// Validate the values
	if (!CutoffFrequencyIsValid())
		return;
	if (!DampingRatioIsValid())
		return;

	if (lowPassRadio->GetValue())
		parameters.type = FilterParameters::TypeLowPass;
	else if (highPassRadio->GetValue())
		parameters.type = FilterParameters::TypeHighPass;
	else
		assert(false);

	// Skip this event if the data is valid
	event.Skip();
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
	name.Printf("%0.1f Hz ", parameters.cutoffFrequency);

	if (parameters.order == 1)
		name += _T("1st Order ");
	else if (parameters.order == 2)
		name += _T("2nd Order ");
	else if (parameters.order == 4)
		name += _T("4th Order ");
	else
		assert(false);

	if (parameters.phaseless)
		name += _T(" Phaseless");

	if (parameters.type == FilterParameters::TypeLowPass)
	{
		name += _T(" Low-Pass Filter");
		if (parameters.order == 2)
			name << " (zeta = " << parameters.dampingRatio << ")";
	}
	else if (parameters.type == FilterParameters::TypeHighPass)
		name += _T(" High-Pass Filter");
	else
		assert(false);

	return name;
}

//==========================================================================
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
//==========================================================================
void FilterDialog::OnSpinChange(wxSpinEvent &event)
{
	if (!OrderIsValid(event.GetInt()))
	{
		int factor(1), order(orderSpin->GetValue());
		if (phaselessCheckBox->GetValue())
			factor *= 2;

		if (abs(event.GetInt() - (int)GetMinOrder(GetType()) * factor) < abs(event.GetInt() - (int)GetMaxOrder(GetType()) * factor))
			order = GetMinOrder(GetType()) * factor;
		else
			order = GetMaxOrder(GetType()) * factor;

		orderSpin->SetValue(order);
	}

	UpdateSpin();
	UpdateDamping();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnSpinUp
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
//==========================================================================
void FilterDialog::OnSpinUp(wxSpinEvent &event)
{
	HandleSpin(event);
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnSpinDown
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
//==========================================================================
void FilterDialog::OnSpinDown(wxSpinEvent &event)
{
	HandleSpin(event);
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnSpinDown
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
//==========================================================================
void FilterDialog::HandleSpin(wxSpinEvent &event)
{
	if (!OrderIsValid(event.GetInt()))
	{
		unsigned int factor(1), order(orderSpin->GetValue());
		if (phaselessCheckBox->GetValue())
			factor *= 2;

		if ((unsigned)event.GetInt() < GetMinOrder(GetType()) * factor)
			order = GetMinOrder(GetType()) * factor;
		else if ((unsigned)event.GetInt() > GetMaxOrder(GetType()) * factor)
			order = GetMaxOrder(GetType()) * factor;
		else if ((unsigned)event.GetInt() > order)// increasing value
		{
			order = event.GetInt();
			while (!OrderIsValid(order) && order < GetMaxOrder(GetType()) * factor)
				order++;
		}
		else// if (event.GetInt() < order)// decreasing value
		{
			order = event.GetInt();
			while (!OrderIsValid(order) && order > GetMinOrder(GetType()) * factor)
				order--;
		}

		orderSpin->SetValue(order);
	}

	UpdateSpin();
	UpdateDamping();
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
	UpdateSpin();
	UpdateDamping();
}

//==========================================================================
// Class:			FilterDialog
// Function:		OnCheckboxChange
//
// Description:		Processes checkbox change events (phaseless selection).
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
void FilterDialog::OnCheckboxChange(wxCommandEvent& WXUNUSED(event))
{
	unsigned int originalOrder = orderSpin->GetValue();
	UpdateSpin();

	if (phaselessCheckBox->GetValue())
		orderSpin->SetValue(originalOrder * 2);
	else
		orderSpin->SetValue(originalOrder / 2);

	UpdateDamping();
}

//==========================================================================
// Class:			FilterDialog
// Function:		UpdateSpin
//
// Description:		Ensures proper control limits are set.
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
void FilterDialog::UpdateSpin(void)
{
	if (!highPassRadio || !lowPassRadio)
		return;

	if (highPassRadio->GetValue())
		orderSpin->Enable(false);
	else
		orderSpin->Enable();

	unsigned int factor(1);
	if (phaselessCheckBox->GetValue())
		factor = 2;

	orderSpin->SetRange(GetMinOrder(GetType()) * factor, GetMaxOrder(GetType()) * factor);
}

//==========================================================================
// Class:			FilterDialog
// Function:		UpdateDamping
//
// Description:		Ensures dampnig control is enabled/disabled properly.
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
void FilterDialog::UpdateDamping(void)
{
	if (!highPassRadio || !lowPassRadio)
		return;

	unsigned int factor(1);
	if (phaselessCheckBox->GetValue())
		factor = 2;

	if (orderSpin->GetValue() / factor == 1)
		dampingRatioBox->Enable(false);
	else
		dampingRatioBox->Enable(true);
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
bool FilterDialog::OrderIsValid(const unsigned int &order) const
{
	if (!highPassRadio || !lowPassRadio)
		return false;

	unsigned int factor(1);
	if (phaselessCheckBox->GetValue())
	{
		if (order % 2 != 0)
			return false;

		factor *= 2;
	}

	return order >= GetMinOrder(GetType()) * factor && order <= GetMaxOrder(GetType()) * factor;
}

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
	if (!highPassRadio || !lowPassRadio)
		return FilterParameters::TypeLowPass;

	if (highPassRadio->GetValue())
		return FilterParameters::TypeHighPass;
	else if (lowPassRadio->GetValue())
		return FilterParameters::TypeLowPass;

	assert(false);
	return FilterParameters::TypeLowPass;
}

//==========================================================================
// Class:			FilterDialog
// Function:		GetMinOrder
//
// Description:		Returns the minimum allowable order for the specified type
//					(for a non-phaseless filter).
//
// Input Arguments:
//		type	= const FilterParameters::Type & specifying the filter type
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int, minimum filter order for non-phaseless filter
//
//==========================================================================
unsigned int FilterDialog::GetMinOrder(const FilterParameters::Type &type) const
{
	switch (type)
	{
	case FilterParameters::TypeHighPass:
		return FilterOrderLimits::MinOrder::highPass;

	case FilterParameters::TypeLowPass:
		return FilterOrderLimits::MinOrder::lowPass;

	default:
		assert(false);
	}

	return 0;
}

//==========================================================================
// Class:			FilterDialog
// Function:		GetMaxOrder
//
// Description:		Returns the maximum allowable order for the specified type
//					(for a non-phaseless filter).
//
// Input Arguments:
//		type	= const FilterParameters::Type & specifying the filter type
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int, maximum filter order for non-phaseless filter
//
//==========================================================================
unsigned int FilterDialog::GetMaxOrder(const FilterParameters::Type &type) const
{
	switch (type)
	{
	case FilterParameters::TypeHighPass:
		return FilterOrderLimits::MaxOrder::highPass;

	case FilterParameters::TypeLowPass:
		return FilterOrderLimits::MaxOrder::lowPass;

	default:
		assert(false);
	}

	return 0;
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
	return OrderIsValid(orderSpin->GetValue());
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
		::wxMessageBox(_T("ERROR:  Cutoff frequency must be numeric!"), _T("Error Defining Filter"));
		return false;
	}
	else if (parameters.cutoffFrequency <= 0.0)
	{
		::wxMessageBox(_T("ERROR:  Cutoff frequency must be positive!"), _T("Error Defining Filter"));
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
	if ((parameters.order > 1 && !phaselessCheckBox->GetValue()) ||
		(parameters.order > 2 && phaselessCheckBox->GetValue()))
	{
		if (!dampingRatioBox->GetValue().ToDouble(&parameters.dampingRatio))
		{
			::wxMessageBox(_T("ERROR:  Damping ratio must be numeric!"), _T("Error Defining Filter"));
			return false;
		}
		else if (parameters.dampingRatio <= 0.0)
		{
			::wxMessageBox(_T("ERROR:  Damping ratio must be positive!"), _T("Error Defining Filter"));
			return false;
		}
	}

	return true;
}