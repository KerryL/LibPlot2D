/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotMath.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.
// History:

#ifndef _PLOT_MATH_H_
#define _PLOT_MATH_H_

// Standard C++ headers
#include <limits>// For QNaN

// Local forward declarations
class Vector;

namespace PlotMath
{

	// Constant declarations
	const double NEARLY_ZERO = 1.0e-12;
	const double QNAN = std::numeric_limits<double>::quiet_NaN();
	const double Pi = 3.141592653589793238462643;

	// Prototypes =====================================================
	// For determining if a number is close enough to zero to regard as zero
	bool IsZero(const double &toCheck);
	bool IsZero(const Vector &toCheck);

	// Checks to see if a value is not a number
	bool IsNaN(const double &toCheck);
	bool IsNaN(const Vector &toCheck);

	// Ensure the value is between two definined limits
	double Clamp(const double &value, const double &lowerLimit, const double &upperLimit);

	// Converts the angle to be between -PI and PI
	double RangeToPlusMinusPi(const double &_angle);

	// Returns the sign of the argument
	double Sign(const double &value);
}

#endif// _PLOT_MATH_H_