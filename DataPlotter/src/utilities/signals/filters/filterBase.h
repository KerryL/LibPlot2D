/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filterBase.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Base class (abstract) for digital filters.
// History:

#ifndef _FILTER_BASE_H_
#define _FILTER_BASE_H_

class FilterBase
{
public:
	// Constructor
	// NOTE:  Constructor MUST initialize the filter parameters a and b, and the input/output vectors y and u
	FilterBase(const double &_sampleRate);

	// Desctructor
	virtual ~FilterBase();

	// Resets all internal variables to initialize the filter to the specified value
	virtual void Initialize(const double &initialValue) = 0;

	// Main method for filtering incoming data
	virtual double Apply(const double &_u) = 0;

	// Returns latest raw data
	double GetRawValue(void) const { return u[0]; };

	// Returns latest filtered data
	double GetFilteredValue(void) const { return y[0]; };

protected:
	// Filter coefficients
	double *a;
	double *b;
	double *y;
	double *u;

	const double sampleRate;// [Hz]
};

#endif// _FILTER_BASE_H_