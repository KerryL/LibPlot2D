/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  curveFit.cpp
// Date:  5/16/2011
// Auth:  K. Loux
// Desc:  Contains methods for fitting curves to datasets.

// Local headers
#include "lp2d/utilities/signals/curveFit.h"
#include "lp2d/utilities/dataset2D.h"

// Eigen headers
#include <Eigen/Dense>

// Standard C++ headers
#include <cmath>
#include <numeric>

namespace LibPlot2D
{

//=============================================================================
// Class:			CurveFit
// Function:		DoPolynomialFit
//
// Description:		Generates coefficients for a best fit (least squares) curve
//					of the specified order.  After calling this function, the
//					programmer is responsible for freeing memory associated with
//					the array of coefficients.
//
// Input Arguments:
//		data	= const Dataset2D& to fit
//		order	= const unsigned int& specifying the order of the polynomial
//
// Output Arguments:
//		None
//
// Return Value:
//		PolynomialFit containing the relevant curve fit data
//
//=============================================================================
CurveFit::PolynomialFit CurveFit::DoPolynomialFit(const Dataset2D &data,
	const unsigned int &order)
{
	PolynomialFit fit;
	fit.coefficients.resize(order + 1);
	fit.order = order;

	Eigen::MatrixXd A(data.GetNumberOfPoints(), order + 1);
	Eigen::VectorXd b(data.GetNumberOfPoints());

	// Here we scale by the maximum X value to give the A matrix a better condition number
	unsigned int i;
	double maxX(data.GetX()[0]);
	for (i = 1; i < data.GetNumberOfPoints(); ++i)
	{
		if (data.GetX()[i] > maxX)
			maxX = data.GetX()[i];
	}

	for (i = 0; i < data.GetNumberOfPoints(); ++i)
	{
		b(i,0) = data.GetY()[i];
		A(i,0) = 1.0;
		unsigned int j;
		for (j = 0; j < order; ++j)
			A(i,j+1) = data.GetX()[i] * A(i,j) / maxX;
	}

	Eigen::VectorXd coefficients(
		A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b));
	for (i = 0; i <= order; ++i)
		fit.coefficients[i] = coefficients(i,0) / pow(maxX, static_cast<int>(i));
	ComputeRSquared(data, fit);

	return fit;
}

//=============================================================================
// Class:			CurveFit
// Function:		ComputeRSquared
//
// Description:		Computes the coefficient of determination value for the
//					specified fit.
//
// Input Arguments:
//		data	= const Dataset2D& of original data
//		fit		= PolynomialFit& containing the information required to draw
//				  the best-fit curve
//
// Output Arguments:
//		fit		= PolynomialFit& (output argument is the rSquared member)
//
// Return Value:
//		PolynomialFit containing the relevant curve fit data
//
//=============================================================================
void CurveFit::ComputeRSquared(const Dataset2D &data, PolynomialFit& fit)
{
	// Determine the mean of the sampled data
	double yBar(std::accumulate(data.GetY().cbegin(), data.GetY().cend(), 0.0)
		/ static_cast<double>(data.GetNumberOfPoints()));

	// Determine ssTotal (total sum of squares) and ssResidual (residual sum of squares)
	double ssTotal(0.0), ssResidual(0.0);
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); ++i)
	{
		ssTotal += (data.GetY()[i] - yBar) * (data.GetY()[i] - yBar);
		double fitValue(EvaluateFit(data.GetX()[i], fit));
		ssResidual += (data.GetY()[i] - fitValue) * (data.GetY()[i] - fitValue);
	}

	// Assign the R^2 value
	fit.rSquared = 1.0 - ssResidual / ssTotal;
}

//=============================================================================
// Class:			CurveFit
// Function:		EvaluateFit
//
// Description:		Returns the calculated y-value at the specified x-value
//					for the specified fit.
//
// Input Arguments:
//		x	= const double& at which point to evaluate the fit
//		fit	= PolynomialFit& containing the information required to draw
//				  the best-fit curve
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the result of evalutign the fit equation at the
//		specified point
//
//=============================================================================
double CurveFit::EvaluateFit(const double &x, const PolynomialFit& fit)
{
	double value(0.0);
	unsigned int i;
	for (i = 0; i <= fit.order; ++i)
		value += fit.coefficients[i] * pow(x, static_cast<int>(i));

	return value;
}

}// namespace LibPlot2D
