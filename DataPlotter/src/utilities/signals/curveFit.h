/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  curve_fit.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Contains methods for fitting curves to datasets.
// History:

#ifndef _CURVE_FIT_H_
#define _CURVE_FIT_H_

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
};

#endif// _CURVE_FIT_H_