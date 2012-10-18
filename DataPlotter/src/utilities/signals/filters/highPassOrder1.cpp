/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  highPassOrder1.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  First order high-pass digital filter.
// History:

// Local headers
#include "utilities/signals/filters/highPassOrder1.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			HighPassFirstOrderFilter
// Function:		HighPassFirstOrderFilter
//
// Description:		Constructor for the HighPassFirstOrderFilter class.
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
HighPassFirstOrderFilter::HighPassFirstOrderFilter(const double& cutoffFrequency,
	const double& sampleRate, const double& initialValue) : Filter(sampleRate)
{
	AllocateArrays(2, 2);

	double sampleTime = 1.0 / sampleRate;// [sec]
	double cutoffRadians = 2.0 * PlotMath::pi * cutoffFrequency;// [rad/sec]

	a[0] = 2.0;
	a[1] = -a[0];

	b[0] = cutoffRadians * sampleTime + 2.0;
	b[1] = cutoffRadians * sampleTime - 2.0;

	Initialize(initialValue);
}