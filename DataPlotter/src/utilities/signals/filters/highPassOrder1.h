/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  highPassOrder1.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  First order high-pass digital filter.
// History:

#ifndef _HIGH_PASS_ORDER1_H_
#define _HIGH_PASS_ORDER1_H_

// Local headers
#include "utilities/signals/filters/filter.h"

class HighPassFirstOrderFilter : public Filter
{
public:
	// Constructor
	HighPassFirstOrderFilter(const double& cutoffFrequency,
		const double& sampleRate, const double& initialValue = 0.0);
};

#endif// _HIGH_PASS_ORDER1_H_