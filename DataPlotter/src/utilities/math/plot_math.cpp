/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_math.cpp
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.
// History:

// Standard C++ headers
#include <cstdlib>
#include <assert.h>

// Local headers
#include "utilities/math/plot_math.h"
#include "utilities/math/vector_class.h"
#include "utilities/math/matrix_class.h"

//==========================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//
// Input Argurments:
//		toCheck	= const double& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the number is less than NEARLY_ZERO
//
//==========================================================================
bool PlotMath::IsZero(const double &toCheck)
{
	if (fabs(toCheck) < NEARLY_ZERO)
		return true;
	else
		return false;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//					This function checks the magnitude of the VECTOR.
//
// Input Argurments:
//		toCheck	= const VECTOR& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the magnitude is less than NEARLY_ZERO
//
//==========================================================================
bool PlotMath::IsZero(const VECTOR &toCheck)
{
	// Check each component of the vector
	if (toCheck.Length() < NEARLY_ZERO)
		return true;

	return false;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Argurments:
//		toCheck	= const double& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool PlotMath::IsNaN(const double &toCheck)
{
	return toCheck != toCheck;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//					Vector version - returns false if any component is NaN.
//
// Input Argurments:
//		ToCheck	= VECTOR& to be checked for containing valid numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool PlotMath::IsNaN(const VECTOR &toCheck)
{
	return IsNaN(toCheck.X) || IsNaN(toCheck.Y) || IsNaN(toCheck.Z);
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Clamp
//
// Description:		Ensures the specified value is between the limits.  In the
//					event that the value is out of the specified bounds, the
//					value that is returned is equal to the limit that the value
//					has exceeded.
//
// Input Argurments:
//		value		= const double& reference to the value which we want to clamp
//		lowerLimit	= const double& lower bound of allowable values
//		upperLimit	= const double& upper bound of allowable values
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the clamped value
//
//==========================================================================
double PlotMath::Clamp(const double &value, const double &lowerLimit, const double &upperLimit)
{
	// Make sure the arguments are valid
	assert(lowerLimit < upperLimit);

	if (value < lowerLimit)
		return lowerLimit;
	else if (value > upperLimit)
		return upperLimit;

	return value;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		RangeToPlusMinusPi
//
// Description:		Adds or subtracts 2 * PI to the specified angle until the
//					angle is between -pi and pi.
//
// Input Argurments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//==========================================================================
double PlotMath::RangeToPlusMinusPi(const double &_angle)
{
	double angle = _angle;
	while (angle <= PI)
		angle += 2 * PI;
	while (angle > PI)
		angle -= 2 * PI;

	return angle;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Sign
//
// Description:		Returns 1 for positive, -1 for negative and 0 for zero.
//
// Input Argurments:
//		value		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotMath::Sign(const double &value)
{
	if (value > 0.0)
		return 1.0;
	else if (value < 0.0)
		return -1.0;
	else
		return 0.0;
}