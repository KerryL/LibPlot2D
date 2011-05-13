/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  curve_fit.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes best-fit trends for data sets.
// History:

#ifndef _CURVE_FIT_H_
#define _CURVE_FIT_H_

// Local forward declarations
class Dataset2D;

class CurveFit
{
public:
	struct CoefficientSet
	{
		unsigned int order;
		double *coefficients;
	};

	static CoefficientSet Compute(const Dataset2D &data, const unsigned int &order);
};

#endif// _CURVE_FIT_H_