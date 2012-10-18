/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  lowPassOrder2.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Second order low-pass digital filter.
// History:

// Local headers
#include "utilities/signals/filters/lowPassOrder2.h"
#include "utilities/math/plotMath.h"

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
	: Filter(sampleRate)
{
	AllocateArrays(3, 3);

	double sampleTime = 1.0 / sampleRate;// [sec]
	double cutoffRadians = 2.0 * PlotMath::pi * cutoffFrequency;// [rad/sec]

	a[0] = sampleTime * sampleTime * cutoffRadians * cutoffRadians;
	a[1] = 2.0 * a[0];
	a[2] = a[0];

	b[0] = 4.0 + 4.0 * sampleTime * dampingRatio * cutoffRadians + a[0];
	b[1] = 2.0 * a[0] - 8.0;
	b[2] = 4.0 - 4.0 * sampleTime * dampingRatio * cutoffRadians + a[0];

	Initialize(initialValue);
}