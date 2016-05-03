/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  curveFit.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Contains methods for fitting curves to datasets.
// History:

#ifndef CURVE_FIT_H_
#define CURVE_FIT_H_

// Local forward declarations
class Dataset2D;

class CurveFit
{
public:
	struct PolynomialFit
	{
		unsigned int order;
		double *coefficients;

		double rSquared;
	};

	static PolynomialFit DoPolynomialFit(const Dataset2D &data, const unsigned int &order);
	static double EvaluateFit(const double &x, const PolynomialFit& fit);

private:
	static void ComputeRSquared(const Dataset2D &data, PolynomialFit& fit);
	//static void DoMatrixTest();
};

#endif// CURVE_FIT_H_