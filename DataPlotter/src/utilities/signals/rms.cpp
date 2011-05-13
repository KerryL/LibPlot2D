/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  rms.cpp
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes root-mean-square of data.
// History:

// Standard C++ headers
#include <cmath>

// Local headers
#include "utilities/signals/rms.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			RootMeanSquare
// Function:		ComputeTimeHistory (static)
//
// Description:		Computes the root-mean-square time history for the given
//					signal.  Assumes y contains data and x is time.
//
// Input Argurments:
//		_data	= const Dataset2D& referring to the data of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the requested time history
//
//==========================================================================
Dataset2D RootMeanSquare::ComputeTimeHistory(const Dataset2D &data)
{
	Dataset2D rms(data);

	if (data.GetNumberOfPoints() == 0)
		return rms;

	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); i++)
		rms.GetYPointer()[i] = sqrt((rms.GetYPointer()[i - 1] * rms.GetYPointer()[i - 1] * i
			+ data.GetYData(i) * data.GetYData(i)) / (i + 1.0));

	return rms;
}