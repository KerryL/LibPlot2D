/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  derivative.cpp
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes discrete-time derivatives of data.
// History:

// Local headers
#include "utilities/signals/derivative.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			DiscreteDerivative
// Function:		ComputeTimeHistory (static)
//
// Description:		Computes the discrete-time derivative time history for
//					the given signal.  Assumes y contains data and x is time.
//
// Input Arguments:
//		_data	= const Dataset2D& referring to the data of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the requested time history
//
//==========================================================================
Dataset2D DiscreteDerivative::ComputeTimeHistory(const Dataset2D &data)
{
	Dataset2D derivative(data);

	if (data.GetNumberOfPoints() < 2)
		return derivative;

	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); i++)
		derivative.GetYPointer()[i] = (data.GetYData(i) - data.GetYData(i - 1))
			/ (data.GetXData(i) - data.GetXData(i - 1));

	derivative.GetYPointer()[0] = derivative.GetYData(1);

	return derivative;
}