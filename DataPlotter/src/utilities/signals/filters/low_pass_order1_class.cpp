/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  low_pass_order1_class.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  First order low-pass digital filter.
// History:

// Local headers
#include "utilities/signals/filters/low_pass_order1_class.h"
#include "utilities/math/plot_math.h"

//==========================================================================
// Class:			LowPassFirstOrderFilter
// Function:		LowPassFirstOrderFilter
//
// Description:		Constructor for the LowPassFirstOrderFilter class.
//
// Input Argurments:
//		cutoffFrequency	= const double& specifying the cutoff frequency [Hz]
//		_sampleRate		= const double& specifying the sampling rate [Hz]
//		initialValue	= const double& specifying initial conditions for this filter
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
LowPassFirstOrderFilter::LowPassFirstOrderFilter(const double& cutoffFrequency,
	const double& sampleRate, const double& initialValue) : FilterBase(sampleRate)
{
	// Allocate and determine the coefficients for the filter
	u = new double[2];
	a = new double[1];

	y = new double[2];
	b = new double[1];

	double sampleTime = 1.0 / sampleRate;// [sec]
	double cutoffRadians = 2.0 * PlotMath::PI * cutoffFrequency;// [rad/sec]

	a[0] = sampleTime * cutoffRadians;
	// a1 = a0, so we don't store it

	double b0 = a[0] + 2.0;
	b[0] = a[0] - 2.0;// Actually b1, but we're not storing b0

	// Scale the coefficients so b0 = 1;
	a[0] /= b0;
	b[0] /= b0;

	// Initialize the filter
	Initialize(initialValue);
}

//==========================================================================
// Class:			LowPassFirstOrderFilter
// Function:		Initialize
//
// Description:		Initialized (or re-initializes) the filter to the specified value.
//
// Input Argurments:
//		initialValue	= const double& specifying initial conditions for this filter
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void LowPassFirstOrderFilter::Initialize(const double &initialValue)
{
	y[0] = initialValue;
	y[1] = initialValue;

	u[0] = initialValue;
	u[1] = initialValue;

	return;
}

//==========================================================================
// Class:			LowPassFirstOrderFilter
// Function:		Apply
//
// Description:		Applies the filter.
//
// Input Argurments:
//		_u	= const double& specifying the raw data input to the filter
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the newly filtered data (same units as input data)
//
//==========================================================================
double LowPassFirstOrderFilter::Apply(const double &_u)
{
	// Shift the inputs one space
	u[1] = u[0];
	u[0] = _u;

	y[1] = y[0];
	y[0] = (u[0] + u[1]) * a[0] - y[1] * b[0];

	return y[0];
}