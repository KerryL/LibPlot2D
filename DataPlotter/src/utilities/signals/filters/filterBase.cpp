/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filterBase.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Base class (abstract) for digital filters.
// History:

// Standard C++ headers
#include <cstdlib>

// Local headers
#include "utilities/signals/filters/filterBase.h"

//==========================================================================
// Class:			FilterBase
// Function:		FilterBase
//
// Description:		Constructor for the FilterBase class.
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
FilterBase::FilterBase(const double &_sampleRate) : sampleRate(_sampleRate)
{
	// Initialize the pointers to NULL
	a = NULL;
	b = NULL;
	u = NULL;
	y = NULL;
}

//==========================================================================
// Class:			FilterBase
// Function:		FilterBase
//
// Description:		Copy constructor for the FilterBase class.
//
// Input Arguments:
//		f	= const FilterBase&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
FilterBase::FilterBase(const FilterBase &f) : sampleRate(f.sampleRate)
{
	// Assign argument to this object
	*this = f;
}

//==========================================================================
// Class:			FilterBase
// Function:		~FilterBase
//
// Description:		Destructor for the FilterBase class.
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
FilterBase::~FilterBase()
{
	// Clean up the filter parameters
	delete [] a;
	a = NULL;

	delete [] b;
	b = NULL;

	delete [] u;
	u = NULL;

	delete [] y;
	y = NULL;
}

//==========================================================================
// Class:			FilterBase
// Function:		operator=
//
// Description:		Assignment operator.
//
// Input Arguments:
//		f	=	const FilterBase&
//
// Output Arguments:
//		None
//
// Return Value:
//		FilterBase&, reference to this
//
//==========================================================================
FilterBase& FilterBase::operator = (const FilterBase &f)
{
	// Check for self assignment (not really any need to do this, but it
	// avoids the compiler warning for unreference formal parameters)
	if (this == &f)
		return *this;

	// Doesn't actually do any assigning - this is handeled by derived classes

	return *this;
}