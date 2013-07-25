/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  integral.cpp
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes discrete-time integral of data.
// History:

// Local headers
#include "utilities/signals/integral.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			DiscreteIntegral
// Function:		ComputeTimeHistory (static)
//
// Description:		Computes the discrete-time integral time history for the
//					given signal.  Assumes y contains data and x is time.
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
Dataset2D DiscreteIntegral::ComputeTimeHistory(const Dataset2D &data)
{
	Dataset2D integral(data);

	if (data.GetNumberOfPoints() < 2)
		return integral;

	integral.GetYPointer()[0] = 0.0;

	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); i++)
		integral.GetYPointer()[i] = integral.GetYData(i - 1) +
			(data.GetXData(i) - data.GetXData(i - 1)) * 0.5
			* (data.GetYData(i) + data.GetYData(i - 1));

	return integral;
}