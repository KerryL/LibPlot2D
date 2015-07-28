/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  curveFit.cpp
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Contains methods for fitting curves to datasets.
// History:

// Standard C++ headers
#include <cmath>

// Local headers
#include "utilities/signals/curveFit.h"
#include "utilities/dataset2D.h"
#include "utilities/math/matrix.h"

//==========================================================================
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
//==========================================================================
/*#include <Windows.h>// Headers for testing matrix operations under MSVC++
#include <iostream>
#include <wx/wx.h>*/
CurveFit::PolynomialFit CurveFit::DoPolynomialFit(const Dataset2D &data, const unsigned int &order)
{
	PolynomialFit fit;
	fit.coefficients = new double[order + 1];
	fit.order = order;

	Matrix A(data.GetNumberOfPoints(), order + 1);
	Matrix b(data.GetNumberOfPoints(), 1);

	// Here we scale by the maximum X value to give the A matrix a better condition number
	unsigned int i, j;
	double maxX(data.GetXData(0));
	for (i = 1; i < data.GetNumberOfPoints(); i++)
	{
		if (data.GetXData(i) > maxX)
			maxX = data.GetXData(i);
	}

	for (i = 0; i < data.GetNumberOfPoints(); i++)
	{
		b(i,0) = data.GetYData(i);
		A(i,0) = 1.0;
		for (j = 0; j < order; j++)
			A(i,j+1) = data.GetXData(i) * A(i,j) / maxX;
	}

	Matrix coefficients;
	if (!A.LeftDivide(b, coefficients))// we're solving the matrix equation Ax = b
	{
		// TODO:  generate error message?
	}
	for (i = 0; i <= order; i++)
		fit.coefficients[i] = coefficients(i,0) / pow(maxX, (int)i);
	ComputeRSquared(data, fit);
	//DoMatrixText();

	return fit;
}

//==========================================================================
// Class:			CurveFit
// Function:		DoMatrixTest
//
// Description:		Tests SVD algorithm based on example in Wikipedia article.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*void CurveFit::DoMatrixTest(void)
{
	Matrix M(4,5);
	M(0,0) = 1.0;
	M(0,4) = 2.0;
	M(1,2) = 3.0;
	M(3,1) = 4.0;
	OutputDebugString(_T("M:\n") + M.Print() + _T("\n"));

	// MATLAB test file
	Matrix A(5,4, 1.0, 3.0, 1.0, 2.0, 2.5, 1.0, 1.0, 1.0, 9.3, 9.1, 23.0, 0.5, 6.4, 6.5, 6.6, 6.7, 20.0, -20.0, 1.0, 2.0);
	OutputDebugString(_T("A:\n") + A.Print() + _T("\n"));

	Matrix U, V, W;
	M.GetSingularValueDecomposition(U, V, W);
	//A.GetSingularValueDecomposition(U, V, W);
	OutputDebugString(_T("U:\n") + U.Print() + _T("\n"));
	OutputDebugString(_T("V':\n") + V.GetTranspose().Print() + _T("\n"));
	OutputDebugString(_T("W:\n") + W.Print() + _T("\n"));

	Matrix Itest(U.GetTranspose() * U);
	OutputDebugString(_T("U' * U:\n") + Itest.Print() + _T("\n"));
	Itest = V * V.GetTranspose();
	OutputDebugString(_T("V * V':\n") + Itest.Print() + _T("\n"));
	Matrix originalAgain(U * W * V.GetTranspose());
	OutputDebugString(_T("U * W * V':\n") + originalAgain.Print() + _T("\n"));
}//*/

//==========================================================================
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
}

//==========================================================================
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
//==========================================================================
double CurveFit::EvaluateFit(const double &x, const PolynomialFit& fit)
{
	double value(0.0);
	unsigned int i;
	for (i = 0; i <= fit.order; i++)
		value += fit.coefficients[i] * pow(x, (int)i);

	return value;
}