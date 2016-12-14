/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  derivative.cpp
// Date:  5/3/2011
// Auth:  K. Loux
// Desc:  Computes discrete-time derivatives of data.

// Local headers
#include "lp2d/utilities/signals/derivative.h"
#include "lp2d/utilities/dataset2D.h"

namespace LibPlot2D
{

//=============================================================================
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
//=============================================================================
Dataset2D DiscreteDerivative::ComputeTimeHistory(const Dataset2D &data)
{
	Dataset2D derivative(data);

	if (data.GetNumberOfPoints() < 2)
		return derivative;

	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); ++i)
		derivative.GetYPointer()[i] = (data.GetYData(i) - data.GetYData(i - 1))
			/ (data.GetXData(i) - data.GetXData(i - 1));

	derivative.GetYPointer()[0] = derivative.GetYData(1);

	return derivative;
}

}// namespace LibPlot2D
