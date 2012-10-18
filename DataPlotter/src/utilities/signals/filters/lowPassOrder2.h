/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

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
#include "utilities/signals/filters/filter.h"

class LowPassSecondOrderFilter : public Filter
{
public:
	// Constructors
	LowPassSecondOrderFilter(const double& cutoffFrequency, const double& dampingRatio,
		const double& sampleRate, const double& initialValue = 0.0);
};

#endif// _LOW_PASS_ORDER2_H_