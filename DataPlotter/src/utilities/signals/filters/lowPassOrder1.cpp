/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  lowPassOrder1.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  First order low-pass digital filter.
// History:

// Local headers
#include "utilities/signals/filters/lowPassOrder1.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			LowPassFirstOrderFilter
// Function:		LowPassFirstOrderFilter
//
// Description:		Constructor for the LowPassFirstOrderFilter class.
//
// Input Arguments:
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
	const double& sampleRate, const double& initialValue) : Filter(sampleRate)
{
	AllocateArrays(2, 2);

	double sampleTime = 1.0 / sampleRate;// [sec]
	double cutoffRadians = 2.0 * PlotMath::pi * cutoffFrequency;// [rad/sec]

	a[0] = sampleTime * cutoffRadians;
	a[1] = a[0];

	b[0] = a[0] + 2.0;
	b[1] = a[0] - 2.0;

	Initialize(initialValue);
}