/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  rms.h
// Date:  5/3/2011
// Auth:  K. Loux
// Desc:  Computes root-mean-square of data.

#ifndef RMS_H_
#define RMS_H_

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

class RootMeanSquare
{
public:
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

}// namespace LibPlot2D

#endif// RMS_H_
