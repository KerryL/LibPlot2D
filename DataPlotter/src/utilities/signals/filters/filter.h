/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filter.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Class for digital filters.
// History:

#ifndef _FILTER_H_
#define _FILTER_H_

class Filter
{
public:
	// Constructor
	// NOTE:  Constructor MUST initialize the filter parameters a and b, and the input/output vectors y and u
	Filter(const double &_sampleRate);
	Filter(const double &_sampleRate, const double *numerator,
		const unsigned int &numeratorSize, const double *denominator,
		const unsigned int &denominatorSize, const double &initialValue);
	Filter(const Filter &f);

	// Desctructor
	virtual ~Filter();

	// Resets all internal variables to initialize the filter to the specified value
	void Initialize(const double &initialValue);

	// Main method for filtering incoming data
	double Apply(const double &_u);

	// Returns latest raw data
	double GetRawValue(void) const { return u[0]; };

	// Returns latest filtered data
	double GetFilteredValue(void) const { return y[0]; };

	// Assignment operator (avoids MSVC Warning C4512)
	Filter& operator=(const Filter &f);

protected:
	// Filter coefficients
	double *a;
	double *b;
	double *y;
	double *u;

	unsigned int outSize;
	unsigned int inSize;

	const double sampleRate;// [Hz]

	void AllocateArrays(const unsigned int &_inSize, const unsigned int &_outSize);
	void ShiftArray(double *s, const unsigned int &size);
	void DeleteArrays(void);
};

#endif// _FILTER_H_