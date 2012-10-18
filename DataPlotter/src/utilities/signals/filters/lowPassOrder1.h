/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  lowPassOrder1.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  First order low-pass digital filter.
// History:

#ifndef _LOW_PASS_ORDER1_H_
#define _LOW_PASS_ORDER1_H_

// Local headers
#include "utilities/signals/filters/filter.h"

class LowPassFirstOrderFilter : public Filter
{
public:
	// Constructor
	LowPassFirstOrderFilter(const double& cutoffFrequency,
		const double& sampleRate, const double& initialValue = 0.0);
};

#endif// _LOW_PASS_ORDER1_H_