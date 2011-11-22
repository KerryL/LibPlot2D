/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  lowPassOrder2.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Second order low-pass digital filter.
// History:

#ifndef _LOW_PASS_ORDER2_H_
#define _LOW_PASS_ORDER2_H_

// Local headers
#include "utilities/signals/filters/filterBase.h"

class LowPassSecondOrderFilter : public FilterBase
{
public:
	// Constructors
	LowPassSecondOrderFilter(const double& cutoffFrequency, const double& dampingRatio,
		const double& sampleRate, const double& initialValue = 0.0);
	LowPassSecondOrderFilter(const LowPassSecondOrderFilter &f);

	// Mandatory overloads from FilterBase
	// Resets all internal variables to initialize the filter to the specified value
	virtual void Initialize(const double &initialValue);

	// Main method for filtering incoming data
	virtual double Apply(const double &_u);

	// Operators
	LowPassSecondOrderFilter& operator = (const LowPassSecondOrderFilter &f);
};

#endif// _LOW_PASS_ORDER2_H_