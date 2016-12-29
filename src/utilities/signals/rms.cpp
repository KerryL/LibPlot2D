/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  rms.cpp
// Date:  5/3/2011
// Auth:  K. Loux
// Desc:  Computes root-mean-square of data.

// Standard C++ headers
#include <cmath>

// Local headers
#include "lp2d/utilities/signals/rms.h"
#include "lp2d/utilities/dataset2D.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			RootMeanSquare
// Function:		ComputeTimeHistory (static)
//
// Description:		Computes the root-mean-square time history for the given
//					signal.  Assumes y contains data and x is time.
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
Dataset2D RootMeanSquare::ComputeTimeHistory(const Dataset2D &data)
{
	Dataset2D rms(data);

	if (data.GetNumberOfPoints() == 0)
		return rms;

	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); ++i)
		rms.GetY()[i] = sqrt((rms.GetY()[i - 1] * rms.GetY()[i - 1] * i
			+ data.GetY()[i] * data.GetY()[i]) / (i + 1.0));

	return rms;
}

}// namespace LibPlot2D
