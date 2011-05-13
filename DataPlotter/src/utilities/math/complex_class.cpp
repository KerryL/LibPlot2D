/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  complex_class.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class functionality for complex number class.
// History:

// Standard C++ headers
#include <iostream>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/math/complex_class.h"

//==========================================================================
// Class:			COMPLEX
// Function:		COMPLEX
//
// Description:		Constructor for the COMPLEX class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
COMPLEX::COMPLEX()
{
}

//==========================================================================
// Class:			COMPLEX
// Function:		COMPLEX
//
// Description:		Constructor for the COMPLEX class.
//
// Input Argurments:
//		_Real		= const double& Real component of the complex number
//		_Imaginary	= const double& imaginary component of the complex number
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
COMPLEX::COMPLEX(const double &_Real, const double &_Imaginary)
{
	// Assign the arguments to the class members
	Real = _Real;
	Imaginary = _Imaginary;
}

//==========================================================================
// Class:			COMPLEX
// Function:		~COMPLEX
//
// Description:		Destructor for the COMPLEX class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
COMPLEX::~COMPLEX()
{
}

//==========================================================================
// Class:			COMPLEX
// Function:		Constant Definitions
//
// Description:		Defines class level constants for the COMPLES class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const COMPLEX COMPLEX::I(0.0, 1.0);

//==========================================================================
// Class:			friend of COMPLEX
// Function:		operator <<
//
// Description:		Writes the value of Target to a stream.
//
// Input Argurments:
//		WriteOut	= ostream& to write out to
//		Complex		= const COMPLEX& value to be written to the stream
//
// Output Arguments:
//		None
//
// Return Value:
//		&ostream containing the formatted value
//
//==========================================================================
ostream &operator << (ostream &WriteOut, const COMPLEX &Complex)
{
	WriteOut << Complex.Print();

	return WriteOut;
}

//==========================================================================
// Class:			COMPLEX
// Function:		Print
//
// Description:		Prints this object to a string.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted value of this object
//
//==========================================================================
wxString COMPLEX::Print(void) const
{
	wxString Temp;

	if (Imaginary >= 0)
		Temp.Printf("%0.3f + %0.3f i", Real, Imaginary);
	else
		Temp.Printf("%0.3f - %0.3f i", Real, -Imaginary);

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator +
//
// Description:		Addition operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the addition
//
//==========================================================================
const COMPLEX COMPLEX::operator + (const COMPLEX &Complex) const
{
	// Make a copy of this object
	COMPLEX Temp = *this;

	// Do the addition
	Temp += Complex;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator -
//
// Description:		Subraction operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the subtraction
//
//==========================================================================
const COMPLEX COMPLEX::operator - (const COMPLEX &Complex) const
{
	// Make a copy of this object
	COMPLEX Temp = *this;

	// Do the subtraction
	Temp -= Complex;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator *
//
// Description:		Multiplication operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the multiplication
//
//==========================================================================
const COMPLEX COMPLEX::operator * (const COMPLEX &Complex) const
{
	// Make a copy of this object
	COMPLEX Temp = *this;

	// Do the multiplication
	Temp *= Complex;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator /
//
// Description:		Division operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the division
//
//==========================================================================
const COMPLEX COMPLEX::operator / (const COMPLEX &Complex) const
{
	// Make a copy of this object
	COMPLEX Temp = *this;

	// Do the division
	Temp /= Complex;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		ToPower
//
// Description:		Raises this object to the specified power.
//
// Input Argurments:
//		Power	= const double& specifiying the power to which this will be raised
//
// Output Arguments:
//		None
//
// Return Value:
//		COMPLEX& result of the power
//
//==========================================================================
COMPLEX& COMPLEX::ToPower(const double &Power)
{
	// Convert this from Cartesian to polar form
	double r = sqrt(Real * Real + Imaginary * Imaginary);
	double theta = atan2(Imaginary, Real);

	// Use De Moivre's theorem to raise this to a power
	r = pow(r, Power);
	theta *= Power;

	// Convert back to Cartesian form
	Real = r * cos(theta);
	Imaginary = r * sin(theta);

	return *this;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator +=
//
// Description:		Addition assignment operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		COMPLEX& result of the addition
//
//==========================================================================
COMPLEX& COMPLEX::operator += (const COMPLEX &Complex)
{
	// Add the real components
	Real += Complex.Real;

	// Add the imaginary components
	Imaginary += Complex.Imaginary;

	return *this;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator -=
//
// Description:		Subraction assignment operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		COMPLEX& result of the subtraction
//
//==========================================================================
COMPLEX& COMPLEX::operator -= (const COMPLEX &Complex)
{
	// Subtract the real components
	Real -= Complex.Real;

	// Subtract the imaginary components
	Imaginary -= Complex.Imaginary;

	return *this;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator *=
//
// Description:		Multiplication assignment operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		COMPLEX& result of the multiplication
//
//==========================================================================
COMPLEX& COMPLEX::operator *= (const COMPLEX &Complex)
{
	double Temp = Real;

	// Similar to a dot product, the real component of the result
	// is the sum of the products of the like components
	Real = Real * Complex.Real - Imaginary * Complex.Imaginary;

	// Similar to a cross product, the imaginary component of the
	// result is the sum of the products of the opposite components
	Imaginary = Temp * Complex.Imaginary + Imaginary * Complex.Real;

	return *this;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator /=
//
// Description:		Division assignment operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		COMPLEX& result of the division
//
//==========================================================================
COMPLEX& COMPLEX::operator /= (const COMPLEX &Complex)
{
	double Temp = Real;

	// Compute the real portion of the result
	Real = (Real * Complex.Real + Imaginary * Complex.Imaginary) /
		(Complex.Real * Complex.Real + Complex.Imaginary * Complex.Imaginary);

	// Compute the imaginary portion of the result
	Imaginary = (Imaginary * Complex.Real - Temp * Complex.Imaginary) /
		(Complex.Real * Complex.Real + Complex.Imaginary * Complex.Imaginary);

	return *this;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator ==
//
// Description:		Equal to comparison operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to compare to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for equal to, false for not equal to
//
//==========================================================================
bool COMPLEX::operator == (const COMPLEX &Complex) const
{
	// Check to see if both the real and imaginary components are equal
	if (Real == Complex.Real && Imaginary == Complex.Imaginary)
		return true;
	else
		return false;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator !=
//
// Description:		Not equal to comparison operator for the COMPLEX class.
//
// Input Argurments:
//		Complex	= const COMPLEX& to compare to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, false for equal to, true for not equal to
//
//==========================================================================
bool COMPLEX::operator != (const COMPLEX &Complex) const
{
	return !(*this == Complex);
}

//==========================================================================
// Class:			COMPLEX
// Function:		GetConjugate
//
// Description:		Returns the complex conjugate of this object.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX with the same real part as this and an imaginary component
//		with opposite magnitude as this
//
//==========================================================================
const COMPLEX COMPLEX::GetConjugate(void) const
{
	COMPLEX Temp;

	// Direct assignment of the real component
	Temp.Real = Real;

	// The imaginary part is the opposite of this
	Temp.Imaginary = -Imaginary;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator +
//
// Description:		Addition operator for the COMPLEX class.
//
// Input Argurments:
//		Double	= const double& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the addition
//
//==========================================================================
const COMPLEX COMPLEX::operator + (const double &Double) const
{
	COMPLEX Temp;

	// Add the real component
	Temp.Real = Real + Double;

	// Direct assignment of the imaginary componet
	Temp.Imaginary = Imaginary;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator -
//
// Description:		Subtraction operator for the COMPLEX class.
//
// Input Argurments:
//		Double	= const double& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the subtraction
//
//==========================================================================
const COMPLEX COMPLEX::operator - (const double &Double) const
{
	COMPLEX Temp;

	// Subtract the real component
	Temp.Real = Real - Double;

	// Direct assignment of the imaginary component
	Temp.Imaginary = Imaginary;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator *
//
// Description:		Multiplication operator for the COMPLEX class.
//
// Input Argurments:
//		Double	= const double& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the multiplication
//
//==========================================================================
const COMPLEX COMPLEX::operator * (const double &Double) const
{
	COMPLEX Temp;

	// Perform the multiplication on both components
	Temp.Real = Real * Double;
	Temp.Imaginary = Imaginary * Double;

	return Temp;
}

//==========================================================================
// Class:			COMPLEX
// Function:		operator /
//
// Description:		Division operator for the COMPLEX class.
//
// Input Argurments:
//		Double	= const double& to divide by this
//
// Output Arguments:
//		None
//
// Return Value:
//		const COMPLEX result of the division
//
//==========================================================================
const COMPLEX COMPLEX::operator / (const double &Double) const
{
	COMPLEX Temp;

	// Perform the division on both components
	Temp.Real = Real / Double;
	Temp.Imaginary = Imaginary / Double;

	return Temp;
}