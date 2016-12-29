/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotMath.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Collection of methods related to mathematical operations.

#ifndef PLOT_MATH_H_
#define PLOT_MATH_H_

// Standard C++ headers
#include <limits>// For QNaN

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

// Local forward declarations
class Vector;
class Dataset2D;

namespace PlotMath
{
	// Constant declarations
	const double NearlyZero = 1.0e-12;
	//const double QNAN = std::numeric_limits<double>::quiet_NaN();// Not currently used
	const double pi = 3.141592653589793238462643;

	// Prototypes =====================================================
	// For determining if a number is close enough to zero to regard as zero
	bool IsZero(const double &n, const double &eps = NearlyZero);
	bool IsZero(const Vector &v, const double &eps = NearlyZero);

	template<typename T>
	bool IsNaN(const T &value);

	template<typename T>
	bool IsInf(const T &value);

	template<typename T>
	bool IsValid(const T &value);

	// Ensure the value is between two definined limits
	double Clamp(const double &value, const double &lowerLimit, const double &upperLimit);

	double RangeToPlusMinusPi(const double &angle);
	double RangeToPlusMinus180(const double &angle);

	void Unwrap(Dataset2D &data);

	bool XDataConsistentlySpaced(const Dataset2D &data, const double &tolerancePercent = 0.01);
	double GetAverageXSpacing(const Dataset2D &data);

	// Returns the sign of the argument
	double Sign(const double &value);

	Dataset2D ApplyBitMask(const Dataset2D &data, const unsigned int &bit);
	unsigned int ApplyBitMask(const unsigned &value, const unsigned int &bit);

	unsigned int GetPrecision(const double &value,
		const unsigned int &significantDigits = 2, const bool &dropTrailingZeros = true);

	unsigned int CountSignificantDigits(const wxString &valueString);

	void sprintf(char *dest, const unsigned int &size, const char *format, ...);

	unsigned int GetPrecision(const double &minimum, const double &majorResolution, const bool &isLogarithmic = false);
}

// Template methods must be defined here:
//=============================================================================
// Namespace:		PlotMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Arguments:
//		value	= const T& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//=============================================================================
template<typename T>
bool PlotMath::IsNaN(const T &value)
{
	return value != value;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		IsInf
//
// Description:		Determines if the specified number is infinite.
//
// Input Arguments:
//		value	= const T&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is ininite
//
//=============================================================================
template<typename T>
bool PlotMath::IsInf(const T &value)
{
	return std::numeric_limits<T>::has_infinity &&
		value == std::numeric_limits<T>::infinity();
}

//=============================================================================
// Namespace:		PlotMath
// Function:		IsValid
//
// Description:		Determines if the specified value is a valid number.
//
// Input Arguments:
//		value	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is valid
//
//=============================================================================
template<typename T>
bool PlotMath::IsValid(const T &value)
{
	return !IsNaN<T>(value) && !IsInf<T>(value);
}

}// namespace LibPlot2D

#endif// PLOT_MATH_H_
