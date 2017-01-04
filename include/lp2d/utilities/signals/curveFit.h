/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  curveFit.h
// Date:  5/16/2011
// Auth:  K. Loux
// Desc:  Contains methods for fitting curves to datasets.

#ifndef CURVE_FIT_H_
#define CURVE_FIT_H_

// Standard C++ headers
#include <vector>

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

/// Class for fitting a polynomial curve to Dataset2D objects.
class CurveFit
{
public:
	/// Structure for storing information about polynomial fits.
	struct PolynomialFit
	{
		unsigned int order;///< The order of the fit.
		std::vector<double> coefficients;///< List of fit coefficients.

		double rSquared;///< Coefficient of determination for the fit.
	};

	/// Performs the polynomial fit.
	///
	/// \param data  The source data.
	/// \param order The order to use for the fit.
	///
	/// \returns Information about the fit.
	static PolynomialFit DoPolynomialFit(const Dataset2D &data, const unsigned int &order);

	/// Evaluates the fit expression for the specified x-value.  In other
	/// words, this method calculates the y-value corresponding to the
	/// specified x-value, given the specified fit information.
	///
	/// \param x   X-value at which the fit should be evaluated.
	/// \param fit Information about how to evaluate the fit.
	///
	/// \returns The y-value corresponding to the specified x-value.
	static double EvaluateFit(const double &x, const PolynomialFit& fit);

private:
	static void ComputeRSquared(const Dataset2D &data, PolynomialFit& fit);
};

}// namespace LibPlot2D

#endif// CURVE_FIT_H_
