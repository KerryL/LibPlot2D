/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filter.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Base class (abstract) for digital filters.
// History:

// Standard C++ headers
#include <cstdlib>

// Local headers
#include "utilities/signals/filters/filter.h"

//==========================================================================
// Class:			Filter
// Function:		Filter
//
// Description:		Constructor for the Filter class.
//
// Input Arguments:
//		_sampleRate		= const double& specifying the sampling rate in Hz
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Filter::Filter(const double &_sampleRate) : sampleRate(_sampleRate)
{
	a = NULL;
	b = NULL;
	u = NULL;
	y = NULL;
}

//==========================================================================
// Class:			Filter
// Function:		Filter
//
// Description:		Constructor for the Filter class for arbitrary filters.
//					Passed arguments are assumed to be for a continuous time
//					filter (s-domain), and will be translated into filter
//					coefficients according to the specified sample rate.
//
// Input Arguments:
//		_sampleRate		= const double& specifying the sampling rate in Hz
//		numerator		= const double* array containing the numerator
//						  coefficients from highest power to zero power (constant)
//		numeratorSize	= const unsigned int& number of numerator coefficients
//		denominaotr		= const double* array containing the denominator
//						  coefficients from highest power to zero power (constant)
//		denominatorSize	= const unsigned int& number of denominator coefficients
//		initialValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Filter::Filter(const double &_sampleRate, const double *numerator,
		const unsigned int &numeratorSize, const double *denominator,
		const unsigned int &denominatorSize, const double &initialValue) : sampleRate(_sampleRate)
{
	// FIXME:  Needs to be implemented

	Initialize(initialValue);
}

//==========================================================================
// Class:			Filter
// Function:		Filter
//
// Description:		Copy constructor for the Filter class.
//
// Input Arguments:
//		f	= const Filter&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Filter::Filter(const Filter &f) : sampleRate(f.sampleRate)
{
	*this = f;
}

//==========================================================================
// Class:			Filter
// Function:		~Filter
//
// Description:		Destructor for the Filter class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Filter::~Filter()
{
	DeleteArrays();
}

//==========================================================================
// Class:			Filter
// Function:		operator=
//
// Description:		Assignment operator.
//
// Input Arguments:
//		f	=	const Filter&
//
// Output Arguments:
//		None
//
// Return Value:
//		Filter&, reference to this
//
//==========================================================================
Filter& Filter::operator=(const Filter &f)
{
	if (this == &f)
		return *this;

	DeleteArrays();

	a = new double[f.inSize];
	b = new double[f.outSize];
	u = new double[f.inSize];
	y = new double[f.outSize];

	unsigned int i;
	for (i = 0; i < inSize; i++)
	{
		a[i] = f.a[i];
		u[i] = f.u[i];
	}

	for (i = 0; i < outSize; i++)
	{
		b[i] = f.b[i];
		y[i] = f.y[i];
	}

	return *this;
}

//==========================================================================
// Class:			Filter
// Function:		DeleteArrays
//
// Description:		Deletes dynamically allocated memory for this object.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Filter::DeleteArrays(void)
{
	delete [] a;
	delete [] b;
	delete [] y;
	delete [] u;
}

//==========================================================================
// Class:			Filter
// Function:		Initialize
//
// Description:		Initializes the filter to the specified value.
//
// Input Arguments:
//		initialValue	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Filter::Initialize(const double &initialValue)
{
	unsigned int i;
	for (i = 0; i < inSize; i++)
		u[i] = initialValue;

	for (i = 0; i < outSize; i++)
		y[i] = initialValue;
}

//==========================================================================
// Class:			Filter
// Function:		Apply
//
// Description:		Applies the filter to the new input value.
//
// Input Arguments:
//		_u	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the filtered value
//
//==========================================================================
double Filter::Apply(const double &_u)
{
	ShiftArray(u, inSize);
	u[0] = _u;

	ShiftArray(y, outSize);

	y[0] = 0.0;
	unsigned int i;
	for (i = 0; i < inSize; i++)
		y[0] += a[i] * u[i];
	for (i = 1; i < outSize; i++)
		y[0] -= b[i] * y[i];
	y[0] /= b[0];

	return y[0];
}

//==========================================================================
// Class:			Filter
// Function:		ShiftArray
//
// Description:		Shifts the array values by one index (value with highest
//					index is lost).
//
// Input Arguments:
//		s		= double*
//		size	= const unsigned int& indicating the size of the array
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Filter::ShiftArray(double *s, const unsigned int &size)
{
	unsigned int i;
	for (i = 1; i < size; i++)
		s[i] = s[i - 1];
}

//==========================================================================
// Class:			Filter
// Function:		AllocateArrays
//
// Description:		Allocates the coefficient and input/output storage arrays.
//
// Input Arguments:
//		_inSize		= const unsigned int&
//		_outSize	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Filter::AllocateArrays(const unsigned int &_inSize, const unsigned int &_outSize)
{
	inSize = _inSize;
	outSize = _outSize;

	a = new double[inSize];
	b = new double[outSize];
	u = new double[inSize];
	y = new double[outSize];
}