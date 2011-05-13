/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vector_class.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class functionality for vector class.
// History:

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/math/vector_class.h"
#include "utilities/math/matrix_class.h"

//==========================================================================
// Class:			VECTOR
// Function:		VECTOR
//
// Description:		Constructor for the VECTOR class.
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
VECTOR::VECTOR()
{
}

//==========================================================================
// Class:			VECTOR
// Function:		VECTOR
//
// Description:		Constructor for the VECTOR class.
//
// Input Argurments:
//		_X	= const double& specifying first component of the vector
//		_Y	= const double& specifying second component of the vector
//		_Z	= const double& specifying third component of the vector
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
VECTOR::VECTOR(const double &_X, const double &_Y, const double &_Z)
{
	// Assign the arguments to the class members
	X = _X;
	Y = _Y;
	Z = _Z;
}

//==========================================================================
// Class:			VECTOR
// Function:		~VECTOR
//
// Description:		Destructor for the VECTOR class.
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
VECTOR::~VECTOR()
{
}

//==========================================================================
// Class:			VECTOR
// Function:		Rotate
//
// Description:		Performs Euler rotation of this object around point CoR.
//					Rotations vector contains first (.X), second (.Y), and
//					third (.Z).  Rotations are performed in that order and
//					around the global coordinate system each time. 
//					Rotations vector must be in radians.  The final three 
//					arguments define the rotation angle.  Care must be taken
//					in setting these arguments, as results will not be
//					accurate unless they are correct.
//
// Input Argurments:
//		CoR			= const VECTOR& specifying the point which this will be rotated
//					  about
//		Rotations	= const VECTOR& specifying the angles which this will be rotated
//					  through [rad]
//		First		= AXIS specifying the first axis of rotation
//		Second		= AXIS specifying the second axis of rotation
//		Third		= AXIS specifying the third axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void VECTOR::Rotate (const VECTOR &CoR, const VECTOR &Rotations,
					 AXIS First, AXIS Second, AXIS Third)
{
	// Rotate self around point CoR with rotations in Rotations vector
	VECTOR temp;
	double S1 = sin(Rotations.X);
	double C1 = cos(Rotations.X);
	double S2 = sin(Rotations.Y);
	double C2 = cos(Rotations.Y);
	double S3 = sin(Rotations.Z);
	double C3 = cos(Rotations.Z);

	// First do the translation
	temp = *this - CoR;

	// Define the rotation matricies using the sine and cosines computed above.  There will be three
	// (one for each rotation).
	MATRIX FirstRotation(3, 3);
	MATRIX SecondRotation(3, 3);
	MATRIX ThirdRotation(3, 3);

	// Create the first rotation matrix
	if (First == AxisX)
		FirstRotation.Set(	1.0,	0.0,	0.0,
							0.0,	C1,		-S1,
							0.0,	S1,		C1);
	else if (First == AxisY)
		FirstRotation.Set(	C1,		0.0,	S1,
							0.0,	1.0,	0.0,
							-S1,	0.0,	C1);
	else// if (First == AxisZ)
		FirstRotation.Set(	C1,		-S1,	0.0,
							S1,		C1,		0.0,
							0.0,	0.0,	1.0);

	// Create the second rotation matrix
	if (Second == AxisX)
		SecondRotation.Set(	1.0,	0.0,	0.0,
							0.0,	C2,		-S2,
							0.0,	S2,		C2);
	else if (Second == AxisY)
		SecondRotation.Set(	C2,		0.0,	S2,
							0.0,	1.0,	0.0,
							-S2,	0.0,	C2);
	else// if (Second == AxisZ)
		SecondRotation.Set(	C2,		-S2,	0.0,
							S2,		C2,		0.0,
							0.0,	0.0,	1.0);

	// Create the third rotation matrix
	if (Third == AxisX)
		ThirdRotation.Set(	1.0,	0.0,	0.0,
							0.0,	C3,		-S3,
							0.0,	S3,		C3);
	else if (Third == AxisY)
		ThirdRotation.Set(	C3,		0.0,	S3,
							0.0,	1.0,	0.0,
							-S3,	0.0,	C3);
	else// if (Third == AxisZ)
		ThirdRotation.Set(	C3,		-S3,	0.0,
							S3,		C3,		0.0,
							0.0,	0.0,	1.0);

	// Combine all three to create the complete rotation matrix
	MATRIX RotationMatrix(3, 3);
	RotationMatrix = ThirdRotation * SecondRotation * FirstRotation;

	// Now we can apply the rotations and translate the vector back
	*this = RotationMatrix * temp + CoR;

	return;
}

//==========================================================================
// Class:			VECTOR
// Function:		Rotate
//
// Description:		Performs rotation of this object around point CoR.  This
//					version only performs rotation about one specified axis.
//
// Input Argurments:
//		CoR		= const VECTOR& specifying the point which this will be rotated about
//		Angle	= const double& specifying the angle which this will be rotated through
//		About	= const AXIS& specifying the axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void VECTOR::Rotate(const VECTOR &CoR, const double &Angle, const AXIS &About)
{
	// Translate the vector to the origin
	VECTOR TranslatedVector = *this - CoR;

	// Perform the rotation
	TranslatedVector.Rotate(Angle, About);

	// Translate the vector back to its original position
	*this = TranslatedVector + CoR;

	return;
}

//==========================================================================
// Class:			VECTOR
// Function:		Rotate
//
// Description:		Rotates this object about the specified axis by the
//					the specified angle.
//
// Input Argurments:
//		Angle	= const double& specifying the distance to rotate this object [rad]
//		About	= const AXIS& specifying the axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void VECTOR::Rotate(const double &Angle, const AXIS &About)
{
	double S = sin(Angle);
	double C = cos(Angle);

	// Define the rotation matrix using the sine and cosines computed above.
	MATRIX RotationMatrix(3, 3);

	// Create the first rotation matrix
	if (About == AxisX)
		RotationMatrix.Set(	1.0,	0.0,	0.0,
							0.0,	C,		-S,
							0.0,	S,		C);
	else if (About == AxisY)
		RotationMatrix.Set(	C,		0.0,	S,
							0.0,	1.0,	0.0,
							-S,		0.0,	C);
	else// if (About == AxisZ)
		RotationMatrix.Set(	C,		-S,		0.0,
							S,		C,		0.0,
							0.0,	0.0,	1.0);

	// Now we can apply the rotations
	*this = RotationMatrix * *this;

	return;
}

//==========================================================================
// Class:			VECTOR
// Function:		Rotate
//
// Description:		Rotates this object about the specified axis by the
//					the specified angle.  This assumes the rotation axis
//					passes through the origin.
//
// Input Argurments:
//		Angle			= const double& specifying the distance to rotate this object [rad]
//		RotationAxis	= const VECTOR& specifying the axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void VECTOR::Rotate(const double &Angle, const VECTOR &RotationAxis)
{
	// To rotate this vector about an arbitrary axis, we use the following matrix
	MATRIX RotationMatrix(3, 3);

	// For the sake of readability
	double U = RotationAxis.X;
	double V = RotationAxis.Y;
	double W = RotationAxis.Z;

	// Calculate the elements of the matrix
	double Norm = RotationAxis.Length();
	double NormSqrd = Norm * Norm;
	double Term11 = (U * U + (V * V + W * W) * cos(Angle)) / NormSqrd;
	double Term12 = (U * V * (1 - cos(Angle)) - W * sin(Angle) * Norm) / NormSqrd;
	double Term13 = (U * W * (1 - cos(Angle)) + V * sin(Angle) * Norm) / NormSqrd;
	double Term21 = (U * V * (1 - cos(Angle)) + W * sin(Angle) * Norm) / NormSqrd;
	double Term22 = (V * V + (U * U + W * W) * cos(Angle)) / NormSqrd;
	double Term23 = (V * W * (1 - cos(Angle)) - U * sin(Angle) * Norm) / NormSqrd;
	double Term31 = (U * W * (1 - cos(Angle)) - V * sin(Angle) * Norm) / NormSqrd;
	double Term32 = (V * W * (1 - cos(Angle)) + U * sin(Angle) * Norm) / NormSqrd;
	double Term33 = (W * W + (U * U + V * V) * cos(Angle)) / NormSqrd;

	// Assign the elements of the vector
	RotationMatrix.Set(Term11, Term12, Term13,
		Term21, Term22, Term23,
		Term31, Term32, Term33);

	// Apply the rotation
	*this = RotationMatrix * *this;

	return;
}

//==========================================================================
// Class:			VECTOR
// Function:		GetAxisName
//
// Description:		Returns the name of the specified axis.
//
// Input Argurments:
//		Axis	= const AXIS& specifying the axis name we want
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the axis name
//
//==========================================================================
wxString VECTOR::GetAxisName(AXIS Axis)
{
	switch (Axis)
	{
	case AxisX:
		return _T("X");
		break;

	case AxisY:
		return _T("Y");
		break;

	case AxisZ:
		return _T("Z");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			VECTOR
// Function:		AnglesTo
//
// Description:		Returns the angles between this and Target projected
//					onto each the X-Y, Y-Z, and X-Z planes.  The returned
//					vector stores the angles in .X (for Y-Z plane), .Y (for
//					X-Z plane), and .Z (for X-Y plane).  Angles are
//					returned in radians with the convention that the angles
//					go from this to Target.
//
// Input Argurments:
//		Target	= const VECTOR& to reference for angle computations
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR containing the angles between this and Target [rad]
//
//==========================================================================
VECTOR VECTOR::AnglesTo(const VECTOR &Target) const
{
	VECTOR Temp;
	double ThisAngle;
	double TargetAngle;

	// When projected onto one of the planes, we ignore the component
	// of the vectors in the normal-to-the-plane direction.
	// First we find the angle (relative to one of the axis) of this,
	// then we find the angle of Target (relative to the same axis).
	// The difference is the angle between the vectors.

	// Y-Z Plane
	ThisAngle = atan2(Y, Z);
	TargetAngle = atan2(Target.Y, Target.Z);
	Temp.X = TargetAngle - ThisAngle;

	// X-Z Plane
	ThisAngle = atan2(X, Z);
	TargetAngle = atan2(Target.X, Target.Z);
	Temp.Y = TargetAngle - ThisAngle;

	// X-Y Plane
	ThisAngle = atan2(X, Y);
	TargetAngle = atan2(Target.X, Target.Y);
	Temp.Z = TargetAngle - ThisAngle;

	return Temp;
}

//==========================================================================
// Class:			friend of VECTOR
// Function:		operator <<
//
// Description:		Prints the contents of the specified object to an
//					ostream object.
//
// Input Argurments:
//		Target	= const VECTOR& to be printed
//
// Output Arguments:
//		WriteOut	= &ostream to which the vector will be printed
//
// Return Value:
//		&ostream containing the printed vector
//
//==========================================================================
ostream &operator << (ostream &WriteOut, const VECTOR &Target)
{
	// Add the string to the stream
	WriteOut << Target.Print();

	return WriteOut;
}

//==========================================================================
// Class:			VECTOR
// Function:		Print
//
// Description:		Prints the contents of this object to a string.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted vector data
//
//==========================================================================
wxString VECTOR::Print(void) const
{
	// Format and fill the string
	wxString Temp;
	Temp.Printf("[%0.3lf, %0.3lf, %0.3lf]", X, Y, Z);

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		Set
//
// Description:		Sets the contents of this vector as specified.
//
// Input Argurments:
//		_X	= const double& specifying first component of the vector
//		_Y	= const double& specifying second component of the vector
//		_Z	= const double& specifying third component of the vector
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void VECTOR::Set(const double &_X, const double &_Y, const double &_Z)
{
	// Assign the arguments to the class members
	X = _X;
	Y = _Y;
	Z = _Z;

	return;
}

//==========================================================================
// Class:			VECTOR
// Function:		Normalize
//
// Description:		Turns this vector into a unit vector (magnitude of 1).
//					Const version (returns a normalized vector, but doesn't
//					modify this object).
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR representing the normalized object
//
//==========================================================================
VECTOR VECTOR::Normalize(void) const
{
	// Get the length of the vector
	double Magnitude = Length();

	// Avoid divide by zero
	if (Magnitude == 0.0)
		return *this;

	// Normalize the components to this length
	VECTOR Temp;
	Temp.X = X / Magnitude;
	Temp.Y = Y / Magnitude;
	Temp.Z = Z / Magnitude;

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		OuterProduct
//
// Description:		Performs the multiplication that results in a 3x3 matrix.
//
// Input Argurments:
//		Vector	= const VECTOR& to be multiplied
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX(3, 3)
//
//==========================================================================
MATRIX VECTOR::OuterProduct(const VECTOR &Vector) const
{
	MATRIX Temp(3, 3,
		X * Vector.X, X * Vector.Y, X * Vector.Z,
		Y * Vector.X, Y * Vector.Y, Y * Vector.Z,
		Z * Vector.X, Z * Vector.Y, Z * Vector.Z);

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator +
//
// Description:		Addition operator for VECTOR class.
//
// Input Argurments:
//		Vector	= const VECTOR& to be added to this
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR result of the addition
//
//==========================================================================
VECTOR VECTOR::operator + (const VECTOR &Vector) const
{
	// Make a copy of this
	VECTOR Temp = *this;

	// Do the addition
	Temp += Vector;

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator -
//
// Description:		Subtraction operator for VECTOR class.
//
// Input Argurments:
//		Vector	= const VECTOR& to be subtracted from this
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR result of the subtraction
//
//==========================================================================
VECTOR VECTOR::operator - (const VECTOR &Vector) const
{
	// Make a copy of this
	VECTOR Temp = *this;

	// Do the subtraction
	Temp -= Vector;

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator *
//
// Description:		Scalar multiplication operator for VECTOR class.
//
// Input Argurments:
//		Double	= const double& to multiply each element of this
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR result of the multiplication
//
//==========================================================================
VECTOR VECTOR::operator * (const double &Double) const
{
	// Make a copy of this
	VECTOR Temp = *this;

	// Do the multiplication
	Temp *= Double;

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator /
//
// Description:		Scalar division operator for VECTOR class.
//
// Input Argurments:
//		Double	= const double& to divide each element of this
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR result of the division
//
//==========================================================================
VECTOR VECTOR::operator / (const double &Double) const
{
	// Make a copy of this
	VECTOR Temp = *this;

	// Do the division
	Temp /= Double;

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		Cross
//
// Description:		Vector cross product.
//
// Input Argurments:
//		Target	= const VECTOR& to multiply this object
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR result of the cross product
//
//==========================================================================
VECTOR VECTOR::Cross(const VECTOR &Vector) const
{
	VECTOR Temp;

	// Perform the cross product operation
	Temp.X = Y * Vector.Z - Z * Vector.Y;
	Temp.Y = Z * Vector.X - X * Vector.Z;
	Temp.Z = X * Vector.Y - Y * Vector.X;

	return Temp;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator +=
//
// Description:		Addition assignment operator for VECTOR class.
//
// Input Argurments:
//		Vector	= const VECTOR& to be added to this
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR& result of the addition
//
//==========================================================================
VECTOR& VECTOR::operator += (const VECTOR &Vector)
{
	// Perform the addition
	X += Vector.X;
	Y += Vector.Y;
	Z += Vector.Z;

	return *this;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator -=
//
// Description:		Subtraction assignment operator for VECTOR class.
//
// Input Argurments:
//		Vector	= const VECTOR& to be subtracted from this
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR& result of the subtraction
//
//==========================================================================
VECTOR& VECTOR::operator -= (const VECTOR &Vector)
{
	// Perform the subtraction
	X -= Vector.X;
	Y -= Vector.Y;
	Z -= Vector.Z;

	return *this;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator ==
//
// Description:		Equal comparison operator for VECTOR class.
//
// Input Argurments:
//		Vector	= const VECTOR& to be compared to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for is equal to, false for is not equal to
//
//==========================================================================
bool VECTOR::operator == (const VECTOR &Vector) const
{
	// Make the comparison
	if (X == Vector.X && Y == Vector.Y && Z == Vector.Z)
		return true;
	else
		return false;
}

//==========================================================================
// Class:			VECTOR
// Function:		operator !=
//
// Description:		Unequal comparison operator for VECTOR class.
//
// Input Argurments:
//		Vector	= const VECTOR& to be compared to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, false for is equal to, true for is not equal to
//
//==========================================================================
bool VECTOR::operator != (const VECTOR &Vector) const
{
	return !(*this == Vector);
}