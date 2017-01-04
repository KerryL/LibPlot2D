/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  complex.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Contains class declaration for complex number class.

#ifndef COMPLEX_H_
#define COMPLEX_H_

// X11 (and possibly others) define Complex - we don't want that.
#ifdef Complex
#undef Complex
#endif// Complex

// Standard C++ headers
#include <ostream>

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

/// Class for representing complex numbers.
class Complex
{
public:
	Complex() = default;

	/// Constructor.
	///
	/// \param real      The real part of the number.
	/// \param imaginary The imaginary part of the number.
	Complex(const double &real, const double &imaginary);

	/// Prints the value to a string.
	/// \returns A string representing this object.
	wxString Print() const;

	/// Gets the complex conjugate of this object.
	/// \returns The complex conjugate of this object.
	const Complex GetConjugate() const;

	/// Gets the length of the polar representation of this object.
	/// \returns The length of the polar representation of this object.
	double GetPolarLength() const;

	/// Gets the angle of the polar representation of this object.
	/// \returns The angle (in radians) corresponding to the polar
	///          representation of this object.
	double GetPolarAngle() const;

	/// \name Operators
	/// @{

	// Operators
	const Complex operator+(const Complex &complex) const;
	const Complex operator-(const Complex &complex) const;
	const Complex operator*(const Complex &complex) const;
	const Complex operator/(const Complex &complex) const;
	Complex& operator+=(const Complex &complex);
	Complex& operator-=(const Complex &complex);
	Complex& operator*=(const Complex &complex);
	Complex& operator/=(const Complex &complex);
	bool operator==(const Complex &complex) const;
	bool operator!=(const Complex &complex) const;
	const Complex operator+(const double &value) const;
	const Complex operator-(const double &value) const;
	const Complex operator*(const double &value) const;
	const Complex operator/(const double &value) const;

	// Raises this object to the specified power
	Complex& ToPower(const double &power);
	const Complex ToPower(const double &power) const;
	Complex& ToPower(const Complex &power);
	const Complex ToPower(const Complex &power) const;

	/// @}

	/// Method for outputing Complex objects to streams.
	///
	/// \param writeOut Stream to which writing should occur.
	/// \param complex  Value to write.
	///
	/// \returns A reference to \p writeOut.
	friend std::ostream &operator<<(std::ostream &writeOut, const Complex &complex);

	/// \name Class data
	/// @{

	double mReal;///< The real part of the number.
	double mImaginary;///< The imaginary part of the number.

	/// @}

	static const Complex mI;///< Square root of negative one.
};

}// namespace LibPlot2D

#endif// COMPLEX_H_
