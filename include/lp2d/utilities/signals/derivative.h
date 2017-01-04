/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  derivative.h
// Date:  5/3/2011
// Auth:  K. Loux
// Desc:  Computes discrete-time derivatives of data.

#ifndef DERIVATIVE_H_
#define DERIVATIVE_H_

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

/// Object for computing the discrete derivative of a Dataset2D.
class DiscreteDerivative
{
public:
	/// Creates a new Dataset2D corresponding to the derivative of the
	/// specified \p data.
	///
	/// \param data The source data.
	///
	/// \returns A new data set containing the derivative of the specified \p
	///          data.
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

}// namespace LibPlot2D

#endif// DERIVATIVE_H_
