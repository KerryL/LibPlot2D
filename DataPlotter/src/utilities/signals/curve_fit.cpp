/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  curve_fit.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Contains methods for fitting curves to datasets.
// History:

// Standard C++ headers
#include <cmath>

// Local headers
#include "utilities/signals/curve_fit.h"
#include "utilities/dataset2D.h"
#include "utilities/math/matrix_class.h"

//==========================================================================
// Class:			CurveFit
// Function:		DoPolynomialFit
//
// Description:		Generates coefficients for a best fit (least squares) curve
//					of the specified order.  After calling this function, the
//					programmer is responsible for freeing memory associated with
//					the array of coefficients.
//
// Input Argurments:
//		data	= const Dataset2D& to fit
//		order	= const unsigned int& specifying the order of the polynomial
//
// Output Arguments:
//		None
//
// Return Value:
//		PolynomialFit containing the relevant curve fit data
//
//==========================================================================
CurveFit::PolynomialFit CurveFit::DoPolynomialFit(const Dataset2D &data, const unsigned int &order)
{
	PolynomialFit fit;

	// Do the fitting
	fit.coefficients = new double[order + 1];
	fit.order = order;

	// Create the matrix and vector we need (we're solving the matrix equation Ax = b)
	Matrix A(data.GetNumberOfPoints(), order + 1);
	Matrix b(data.GetNumberOfPoints(), 1);
	unsigned int i, j;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
	{
		b(i,0) = data.GetYData(i);
		A(i,0) = 1.0;
		for (j = 0; j < order; j++)
			A(i,j+1) = data.GetXData(i) * A(i,j);
	}

	Matrix coefficients(A.LeftDivide(b));

	// Assign the coefficients to our PolynomialFit data
	for (i = 0; i <= order; i++)
		fit.coefficients[i] = coefficients(i,0);

	ComputeRSquared(data, fit);

	return fit;
}

//==========================================================================
// Class:			CurveFit
// Function:		ComputeRSquared
//
// Description:		Computes the coefficient of determination value for the
//					specified fit.
//
// Input Argurments:
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
//==========================================================================
void CurveFit::ComputeRSquared(const Dataset2D &data, PolynomialFit& fit)
{
	// Determine the mean of the sampled data
	double yBar(0.0);
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		yBar += data.GetYData(i);
	yBar /= (double)data.GetNumberOfPoints();

	// Determine ssTotal (total sum of squares) and ssResidual (residual sum of squares)
	double ssTotal(0.0), ssResidual(0.0);
	double fitValue;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
	{
		ssTotal += (data.GetYData(i) - yBar) * (data.GetYData(i) - yBar);
		fitValue = EvaluateFit(data.GetXData(i), fit);
		ssResidual += (data.GetYData(i) - fitValue) * (data.GetYData(i) - fitValue);
	}

	// Assign the R^2 value
	fit.rSquared = 1.0 - ssResidual / ssTotal;

	return;
}

//==========================================================================
// Class:			CurveFit
// Function:		EvaluateFit
//
// Description:		Returns the calculated y-value at the specified x-value
//					for the specified fit.
//
// Input Argurments:
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
//==========================================================================
double CurveFit::EvaluateFit(const double &x, const PolynomialFit& fit)
{
	double value(0.0);
	unsigned int i;
	for (i = 0; i <= fit.order; i++)
		value += fit.coefficients[i] * pow(x, (int)i);

	return value;
}