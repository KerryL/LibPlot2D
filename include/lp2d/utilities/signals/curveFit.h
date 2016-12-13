/*=============================================================================
                                    DataPlotter
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

class CurveFit
{
public:
	struct PolynomialFit
	{
		unsigned int order;
		std::vector<double> coefficients;

		double rSquared;
	};

	static PolynomialFit DoPolynomialFit(const Dataset2D &data, const unsigned int &order);
	static double EvaluateFit(const double &x, const PolynomialFit& fit);

private:
	static void ComputeRSquared(const Dataset2D &data, PolynomialFit& fit);
	//static void DoMatrixTest();
};

}// namespace LibPlot2D

#endif// CURVE_FIT_H_
