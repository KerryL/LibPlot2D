/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  integral.h
// Date:  5/3/2011
// Auth:  K. Loux
// Desc:  Computes discrete-time integral of data.

#ifndef INTEGRAL_H_
#define INTEGRAL_H_

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

/// Object for computing the discrete integral of a Dataset2D.
class DiscreteIntegral
{
public:
	/// Creates a new Dataset2D corresponding to the integral of the specified
	/// \p data.
	///
	/// \param data The source data.
	///
	/// \returns A new data set containing the integral of the specified \p
	///          data.
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

}// namespace LibPlot2D

#endif// INTEGRAL_H_
