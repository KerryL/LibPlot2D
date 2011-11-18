/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  low_pass_order2_class.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Second order low-pass digital filter.
// History:

// Local headers
#include "utilities/signals/filters/low_pass_order2_class.h"
#include "utilities/math/plot_math.h"

//==========================================================================
// Class:			LowPassSecondOrderFilter
// Function:		LowPassSecondOrderFilter
//
// Description:		Constructor for the LowPassSecondOrderFilter class.
//
// Input Arguments:
//		cutoffFrequency	= const double& specifying the cutoff frequency [Hz]
//		dampingRatio	= const double& specifying the damping ratio [-]
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
LowPassSecondOrderFilter::LowPassSecondOrderFilter(const double& cutoffFrequency,
	const double& dampingRatio, const double& sampleRate, const double& initialValue)
	: FilterBase(sampleRate)
{
	// Allocate and determine the coefficients for the filter
	u = new double[3];
	a = new double[1];

	y = new double[3];
	b = new double[2];

	double sampleTime = 1.0 / sampleRate;// [sec]
	double cutoffRadians = 2.0 * PlotMath::PI * cutoffFrequency;// [rad/sec]

	a[0] = sampleTime * sampleTime * cutoffRadians * cutoffRadians;
	// a2 = a0 and a1 = 2 * a0, so we don't store those

	double b0 = 4.0 + 4.0 * sampleTime * dampingRatio * cutoffRadians + a[0];
	b[0] = 2.0 * a[0] - 8.0;// Actually b1, but we're not storing b0
	b[1] = 4.0 - 4.0 * sampleTime * dampingRatio * cutoffRadians + a[0];// Just like above, this is actually b2

	// Scale the coefficients so b0 = 1;
	a[0] /= b0;
	b[0] /= b0;
	b[1] /= b0;

	// Initialize the filter
	Initialize(initialValue);
}

//==========================================================================
// Class:			LowPassSecondOrderFilter
// Function:		Initialize
//
// Description:		Initialized (or re-initializes) the filter to the specified value.
//
// Input Arguments:
//		initialValue	= const double& specifying initial conditions for this filter
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void LowPassSecondOrderFilter::Initialize(const double &initialValue)
{
	y[0] = initialValue;
	y[1] = initialValue;
	y[2] = initialValue;

	u[0] = initialValue;
	u[1] = initialValue;
	u[2] = initialValue;

	return;
}

//==========================================================================
// Class:			LowPassSecondOrderFilter
// Function:		Apply
//
// Description:		Applies the filter.
//
// Input Arguments:
//		_u	= const double& specifying the raw data input to the filter
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the newly filtered data (same units as input data)
//
//==========================================================================
double LowPassSecondOrderFilter::Apply(const double &_u)
{
	// Shift the inputs one space
	u[2] = u[1];
	u[1] = u[0];
	u[0] = _u;

	y[2] = y[1];
	y[1] = y[0];
	y[0] = (u[0] + 2.0 * u[1] + u[2]) * a[0] - y[1] * b[0] - y[2] * b[1];

	return y[0];
}