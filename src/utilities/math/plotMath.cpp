/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotMath.cpp
// Date:  3/24/2008
// Auth:  K. Loux
// Desc:  Collection of methods related to mathematical operations.

// Local headers
#include "lp2d/utilities/math/plotMath.h"
#include "lp2d/utilities/dataset2D.h"

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <cstdlib>
#include <cassert>
#include <limits>
#include <cstdarg>
#include <sstream>

namespace LibPlot2D
{

//=============================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//
// Input Arguments:
//		n	= const double& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the number is less than NEARLY_ZERO
//
//=============================================================================
bool PlotMath::IsZero(const double &n, const double &eps)
{
	if (fabs(n) < eps)
		return true;
	else
		return false;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//					This function checks the magnitude of the Vector.
//
// Input Arguments:
//		v	= const Eigen::VectorXd& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the magnitude is less than NEARLY_ZERO
//
//=============================================================================
bool PlotMath::IsZero(const Eigen::VectorXd &v, const double &eps)
{
	if (v.norm() < eps)
		return true;

	return false;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		Clamp
//
// Description:		Ensures the specified value is between the limits.  In the
//					event that the value is out of the specified bounds, the
//					value that is returned is equal to the limit that the value
//					has exceeded.
//
// Input Arguments:
//		value		= const double& reference to the value which we want to
//					  clamp
//		lowerLimit	= const double& lower bound of allowable values
//		upperLimit	= const double& upper bound of allowable values
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the clamped value
//
//=============================================================================
double PlotMath::Clamp(const double &value, const double &lowerLimit,
	const double &upperLimit)
{
	// Make sure the arguments are valid
	assert(lowerLimit < upperLimit);

	if (value < lowerLimit)
		return lowerLimit;
	else if (value > upperLimit)
		return upperLimit;

	return value;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		RangeToPlusMinusPi
//
// Description:		Adds or subtracts 2 * pi to the specified angle until the
//					angle is between -pi and pi.
//
// Input Arguments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//=============================================================================
double PlotMath::RangeToPlusMinusPi(const double &angle)
{
	return fmod(angle + pi, 2.0 * pi) - pi;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		RangeToPlusMinus180
//
// Description:		Adds or subtracts 180 to the specified angle until the
//					angle is between -180 and 180.
//
// Input Arguments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//=============================================================================
double PlotMath::RangeToPlusMinus180(const double &angle)
{
	return fmod(angle + 180.0, 360.0) - 180.0;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		Unwrap
//
// Description:		Minimizes the jump between adjacent points by adding or
//					subtracting multiples of 2 * pi.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotMath::Unwrap(Dataset2D &data)
{
	double threshold(pi);
	unsigned int i;
	for (i = 1; i < data.GetNumberOfPoints(); ++i)
	{
		if (data.GetY()[i] - data.GetY()[i - 1] > threshold)
			data.GetY()[i] -= 2 * pi;
		if (data.GetY()[i] - data.GetY()[i - 1] < -threshold)
			data.GetY()[i] += 2 * pi;
	}
}

//=============================================================================
// Namespace:		PlotMath
// Function:		Sign
//
// Description:		Returns 1.0 for positive, -1.0 for negative and 0.0 for
//					zero.
//
// Input Arguments:
//		value		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double PlotMath::Sign(const double &value)
{
	if (value > 0.0)
		return 1.0;
	else if (value < 0.0)
		return -1.0;
	else
		return 0.0;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		ApplyBitMask
//
// Description:		Extracts a single bit from values of the specified dataset.
//
// Input Arguments:
//		data	= const Dataset2D&
//		bit		= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
Dataset2D PlotMath::ApplyBitMask(const Dataset2D &data,
	const unsigned int &bit)
{
	Dataset2D set(data);
	for (auto& y : set.GetY())
		y = ApplyBitMask(static_cast<unsigned int>(y), bit);
	return set;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		ApplyBitMask
//
// Description:		Extracts a single bit from the value.
//
// Input Arguments:
//		value	= const unsigned int&
//		bit		= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
unsigned int PlotMath::ApplyBitMask(const unsigned &value,
	const unsigned int &bit)
{
	return (value >> bit) & 1;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		XDataConsistentlySpaced
//
// Description:		Checks to see if the X-data has consistent deltas.
//
// Input Arguments:
//		data				= const Dataset2D&
//		tolerancePercent	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the x-data spacing is within the tolerance
//
//=============================================================================
bool PlotMath::XDataConsistentlySpaced(const Dataset2D &data,
	const double &tolerancePercent)
{
	assert(data.GetNumberOfPoints() > 1);

	unsigned int i;
	double minSpacing, maxSpacing;

	minSpacing = data.GetAverageDeltaX();
	maxSpacing = minSpacing;

	for (i = 2; i < data.GetNumberOfPoints(); ++i)
	{
		double spacing(data.GetX()[i] - data.GetX()[i - 1]);
		if (spacing < minSpacing)
			minSpacing = spacing;
		if (spacing > maxSpacing)
			maxSpacing = spacing;
	}

	// Handle decreasing data, too
	if (fabs(minSpacing) > fabs(maxSpacing))
	{
		double temp(minSpacing);
		minSpacing = maxSpacing;
		maxSpacing = temp;
	}

	return 1.0 - minSpacing / maxSpacing < tolerancePercent;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		GetAverageXSpacing
//
// Description:		Finds the average period of the data in the set.
//
// Input Arguments:
//		data	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double PlotMath::GetAverageXSpacing(const Dataset2D &data)
{
	return data.GetX().back() / (data.GetNumberOfPoints() - 1.0);
}

//=============================================================================
// Namespace:		PlotMath
// Function:		GetPrecision
//
// Description:		Determines the best number of digits after the decimal place
//					for a string representation of the specified value (for
//					use with printf-style %0.*f formatting.
//
// Input Arguments:
//		value				= const double&
//		significantDigits	= const unsigned int&
//		dropTrailingZeros	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the x-data spacing is within the tolerance
//
//=============================================================================
unsigned int PlotMath::GetPrecision(const double &value,
	const unsigned int &significantDigits, const bool &dropTrailingZeros)
{
	int precision(significantDigits - static_cast<unsigned int>(floor(log10(value)) - 1));
	if (precision < 0)
		precision = 0;
	if (!dropTrailingZeros)
		return precision;

	std::ostringstream ss;
	ss.precision(precision);
	ss << value;

	std::string number(ss.str());
	unsigned int i;
	for (i = number.size() - 1; i > 0; --i)
	{
		if (number[i] == '0')
			--precision;
		else
			break;
	}

	if (precision < 0)
		precision = 0;

	return precision;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		CountSignificantDigits
//
// Description:		Returns the number of significant digits in the string.
//
// Input Arguments:
//		valueString	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int PlotMath::CountSignificantDigits(const wxString &valueString)
{
	double value;
	if (!valueString.ToDouble(&value))
		return 0;

	wxString trimmedValueString = wxString::Format("%+0.15f", value);
	unsigned int first;
	for (first = 1; first < trimmedValueString.Len(); ++first)
	{
		if (trimmedValueString[first] != '0' &&
			trimmedValueString[first] != '.')
			break;
	}

	unsigned int last;
	for (last = trimmedValueString.Len() - 1; last > first; --last)
	{
		if (trimmedValueString[last] != '0' &&
			trimmedValueString[last] != '.')
			break;
	}

	unsigned int i;
	for (i = first + 1; i < last - 1; ++i)
	{
		if (trimmedValueString[i] == '.')
		{
			first++;
			break;
		}
	}

	return last - first + 1;
}

//=============================================================================
// Namespace:		PlotMath
// Function:		GetPrecision
//
// Description:		Returns the required precision (digits past zero) to
//					distinguish between adjacent graduations.
//
// Input Arguments:
//		minimum			= const double&
//		majorResolution	= const double&
//		isLogarithmic	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int PlotMath::GetPrecision(const double &minimum,
	const double &majorResolution, const bool &isLogarithmic)
{
	double baseValue;
	if (isLogarithmic)
		baseValue = minimum;
	else
		baseValue = majorResolution;

	if (log10(baseValue) >= 0.0)
		return 0;

	return -log10(baseValue) + 1;
}

}// namespace LibPlot2D
