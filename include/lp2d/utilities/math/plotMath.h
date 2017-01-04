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

// Eigen headers
#include <Eigen/Eigen>

// Standard C++ headers
#include <limits>// For QNaN

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

// Local forward declarations
class Dataset2D;

/// Namespace containing commonly used mathematical and number processing
/// methods.
namespace PlotMath
{
	/// Constant for evauluating whether or not a number is "very small."
	const double NearlyZero = 1.0e-12;
	//const double QNAN = std::numeric_limits<double>::quiet_NaN();// Not currently used

	/// Determines if the specified value is close enough to zero to be
	/// considered zero.
	///
	/// \param n   Value to consider.
	/// \param eps Threshold value.
	///
	/// \returns True if the absolute value of \p n is less than \p eps.
	bool IsZero(const double &n, const double &eps = NearlyZero);

	/// Determines if the norm of the specified vector is close enough to zero
	/// to be considered zero.
	///
	/// \param v   Vector to consider.
	/// \param eps Threshold value.
	///
	/// \returns True if the norm value of \p v is less than \p eps.
	bool IsZero(const Eigen::VectorXd &v, const double &eps = NearlyZero);

	/// Checks to see if the specified value is not a number.
	///
	/// \param value Value to consider.
	///
	/// \returns True if \p value is not a number.
	template<typename T>
	bool IsNaN(const T &value);

	/// Checks to see if the specified value is infinite.
	///
	/// \param value Value to consider.
	///
	/// \returns True if \p value is infinite.
	template<typename T>
	bool IsInf(const T &value);

	/// Checks to see if the specified value is a number and is not infinite.
	///
	/// \param value Value to consider.
	///
	/// \returns True if \p value is a finite number.
	template<typename T>
	bool IsValid(const T &value);

	/// Forces the value to lie between the specified limits.
	///
	/// \param value      Value to clamp.
	/// \param lowerLimit Maximum allowed output value.
	/// \param upperLimit Minimum allowed outptu value.
	///
	/// \returns The \p upperLimit if \p value is greater than \p upperLimit,
	///          the \p lowerLimit if \p value is less than \p lowerLimit, or
	///          the unmodified \p value otherwise.
	double Clamp(const double &value, const double &lowerLimit,
		const double &upperLimit);

	/// Converts the argument to lie within the range <&plusmn><&pi>.
	///
	/// \param angle Angle in radians.
	///
	/// \returns An equivalent angle within the range <&plusmn><&pi>.
	double RangeToPlusMinusPi(const double &angle);

	/// Converts the argument to lie within the range <&plusmn>180 deg.
	///
	/// \param angle Angle in degrees.
	///
	/// \returns An equivalent angle within the range <&plusmn>180 deg.
	double RangeToPlusMinus180(const double &angle);

	/// Converts the \p data from an angle format where the values roll over to
	/// a format where the values are not limited.  This is the inverse of
	/// RangeToPlusMinusPi().  This is helpful when attempting to take the
	/// derivative, for example, to avoid large spikes in the data caused by
	/// the rollover.
	///
	/// \param data The data to unwrap.
	void Unwrap(Dataset2D &data);

	/// Checks to see if the \p data is consistently spaced (i.e. if the x-data
	/// is nearly monotonically increasing).
	///
	/// \param data             The data to assess.
	/// \param tolerancePercent The criteria to use for determining if the
	///                         spacing is "consistent."
	///
	/// \returns True if the deltas between adjacent x-values do not vary by
	///          more than \p tolerancePercent.
	bool XDataConsistentlySpaced(const Dataset2D &data,
		const double &tolerancePercent = 0.01);

	/// Computes the average delta between adjacent x-values.
	///
	/// \param data The source of the x-values.
	///
	/// \returns The average delta between adjacent x-values.
	double GetAverageXSpacing(const Dataset2D &data);

	/// Determines the sign of the argument.
	///
	/// \param value The value to consider.
	///
	/// \returns -1.0 if the \p value is negative, +1.0 if the \p value is
	///          positive, or 0.0 if the \p value is zero.
	double Sign(const double &value);

	/// Applies a bitmask to each value in the dataset.
	///
	/// \param data Data set to which the mask is applied
	/// \param bit  The bit to extract.  For example, \p bit = 0 will extract
	///             the LSB from the data value (it will not "and" the value
	///             with zero).
	///
	/// \returns A new Dataset2D containing the extracted bit.
	Dataset2D ApplyBitMask(const Dataset2D &data, const unsigned int &bit);

	/// Applies a bitmask to each value in the dataset.
	///
	/// \param value Vale set to which the mask is applied
	/// \param bit   The bit to extract.  For example, \p bit = 0 will extract
	///              the LSB from the value (it will not "and" the value with
	///              zero).
	///
	/// \returns The value of the specified bit in \p value (either 0 or 1).
	unsigned int ApplyBitMask(const unsigned &value, const unsigned int &bit);

	/// Returns the required precision to represent the specified \p value with
	/// the specified number of significant digits.
	///
	/// \param value             The value to assess.
	/// \param significantDigits The desired number of significant digits.
	/// \param dropTrailingZeros Indicates whether or not trailing zeros should
	///                          be included in the count.
	///
	/// \returns The number of digits to the right of the decimal point
	///          required to show the specified value with the specified number
	///          of significant digits.
	unsigned int GetPrecision(const double &value,
		const unsigned int &significantDigits = 2,
		const bool &dropTrailingZeros = true);

	/// Determines the number of significant digits in the specified number
	/// (represented by a string).
	///
	/// \param valueString The string to assess.
	///
	/// \returns The number of significant digits in the \p valueString.
	unsigned int CountSignificantDigits(const wxString &valueString);

	/// Returns the required precision to differentiate between adjacent
	/// graduations on a number line.
	///
	/// \param minimum         The minimum value to be considered.
	/// \param majorResolution The delta between the minimum value and the next
	///                        graduation.
	/// \param isLogarithmic   Flag indicating whether or not the graduations
	///                        are logarithmically spaced.
	///
	/// \returns The required precision to differentiate between adjacent
	///          graduations.
	unsigned int GetPrecision(const double &minimum,
		const double &majorResolution, const bool &isLogarithmic = false);
}

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
