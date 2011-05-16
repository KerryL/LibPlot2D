/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  filter_base_class.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Base class (abstract) for digital filters.
// History:

// Standard C++ headers
#include <cstdlib>

// Local headers
#include "utilities/signals/filters/filter_base_class.h"

//==========================================================================
// Class:			FilterBase
// Function:		FilterBase
//
// Description:		Constructor for the FilterBase class.
//
// Input Argurments:
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
// Function:		~FilterBase
//
// Description:		Destructor for the FilterBase class.
//
// Input Argurments:
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