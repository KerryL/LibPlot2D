/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  vector.cpp
// Date:  11/3/2007
// Auth:  K. Loux
// Desc:  Contains class functionality for vector class.

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "lp2d/utilities/math/vector.h"
#include "lp2d/utilities/math/matrix.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			Vector
// Function:		Vector
//
// Description:		Constructor for the Vector class.
//
// Input Arguments:
//		x	= const double& specifying first component of the vector
//		y	= const double& specifying second component of the vector
//		z	= const double& specifying third component of the vector
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Vector::Vector(const double &x, const double &y, const double &z) : x(x), y(y), z(z)
{
}

//=============================================================================
// Class:			Vector
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
// Input Arguments:
//		cor			= const Vector& specifying the point which this will be rotated
//					  about
//		rotations	= const Vector& specifying the angles which this will be rotated
//					  through [rad]
//		first		= Axis specifying the first axis of rotation
//		second		= Axis specifying the second axis of rotation
//		third		= Axis specifying the third axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Vector::Rotate (const Vector &cor, const Vector &rotations,
					 Axis first, Axis second, Axis third)
{
	// Rotate self around point CoR with rotations in Rotations vector
	Vector temp;

	// First do the translation
	temp = *this - cor;

	// Define the rotation matrices using the sine and cosines computed above.
	// There will be three (one for each rotation).
	Matrix firstRotation = GenerateRotationMatrix(first, rotations.x);
	Matrix secondRotation = GenerateRotationMatrix(second, rotations.y);
	Matrix thirdRotation = GenerateRotationMatrix(third, rotations.z);

	// Combine all three to create the complete rotation matrix
	Matrix rotationMatrix(3, 3);
	rotationMatrix = thirdRotation * secondRotation * firstRotation;

	// Now we can apply the rotations and translate the vector back
	*this = rotationMatrix * temp + cor;
}

//=============================================================================
// Class:			Vector
// Function:		Rotate
//
// Description:		Performs rotation of this object around point CoR.  This
//					version only performs rotation about one specified axis.
//
// Input Arguments:
//		cor		= const Vector& specifying the point which this will be rotated about
//		angle	= const double& specifying the angle which this will be rotated through
//		about	= const Axis& specifying the axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Vector::Rotate(const Vector &cor, const double &angle, const Axis &about)
{
	// Translate the vector to the origin
	Vector translatedVector = *this - cor;

	// Perform the rotation
	translatedVector.Rotate(angle, about);

	// Translate the vector back to its original position
	*this = translatedVector + cor;
}

//=============================================================================
// Class:			Vector
// Function:		Rotate
//
// Description:		Rotates this object about the specified axis by the
//					the specified angle.
//
// Input Arguments:
//		angle	= const double& specifying the distance to rotate this object [rad]
//		about	= const Axis& specifying the axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Vector::Rotate(const double &angle, const Axis &about)
{
	// Define the rotation matrix using the sine and cosines computed above.
	Matrix rotationMatrix = GenerateRotationMatrix(about, angle);

	// Now we can apply the rotations
	*this = rotationMatrix * *this;
}

//=============================================================================
// Class:			Vector
// Function:		Rotate
//
// Description:		Rotates this object about the specified axis by the
//					the specified angle.  This assumes the rotation axis
//					passes through the origin.
//
// Input Arguments:
//		Angle			= const double& specifying the distance to rotate this object [rad]
//		RotationAxis	= const Vector& specifying the axis of rotation
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Vector::Rotate(const double &angle, const Vector &rotationAxis)
{
	Matrix rotationMatrix = GenerateRotationMatrix(angle, rotationAxis);

	// Apply the rotation
	*this = rotationMatrix * *this;
}

//=============================================================================
// Class:			Vector
// Function:		GenerateRotationMatrix
//
// Description:		Creates a 3x3 rotation matrix for rotation about the
//					specified (arbitrary) axis.
//
// Input Arguments:
//		angle			= const double& specifying the distance to rotate this object [rad]
//		rotationAxis	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix, 3x3 rotation matrix
//
//=============================================================================
Matrix Vector::GenerateRotationMatrix(const double &angle, const Vector& rotationAxis)
{
	// To rotate this vector about an arbitrary axis, we use the following matrix
	Matrix rotationMatrix(3, 3);

	// For the sake of readability
	double u = rotationAxis.x;
	double v = rotationAxis.y;
	double w = rotationAxis.z;

	// Calculate the elements of the matrix
	double norm = rotationAxis.Length();
	double normSqrd = norm * norm;
	double term11 = (u * u + (v * v + w * w) * cos(angle)) / normSqrd;
	double term12 = (u * v * (1 - cos(angle)) - w * sin(angle) * norm) / normSqrd;
	double term13 = (u * w * (1 - cos(angle)) + v * sin(angle) * norm) / normSqrd;
	double term21 = (u * v * (1 - cos(angle)) + w * sin(angle) * norm) / normSqrd;
	double term22 = (v * v + (u * u + w * w) * cos(angle)) / normSqrd;
	double term23 = (v * w * (1 - cos(angle)) - u * sin(angle) * norm) / normSqrd;
	double term31 = (u * w * (1 - cos(angle)) - v * sin(angle) * norm) / normSqrd;
	double term32 = (v * w * (1 - cos(angle)) + u * sin(angle) * norm) / normSqrd;
	double term33 = (w * w + (u * u + v * v) * cos(angle)) / normSqrd;

	// Assign the elements of the vector
	rotationMatrix.Set(term11, term12, term13,
		term21, term22, term23,
		term31, term32, term33);

	return rotationMatrix;
}

//=============================================================================
// Class:			Vector
// Function:		GenerateRotationMatrix
//
// Description:		Creates a 3x3 rotation matrix for rotation about the
//					specified axis.
//
// Input Arguments:
//		axis	= const Axis& specifying the axis of rotation
//		angle	= const double& specifying the distance to rotate this object [rad]
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix, 3x3 rotation matrix
//
//=============================================================================
Matrix Vector::GenerateRotationMatrix(const Axis &axis, const double &angle)
{
	double s = sin(angle);
	double c = cos(angle);

	// Define the rotation matrix using the sine and cosines computed above.
	Matrix rotationMatrix(3, 3);

	// Create the first rotation matrix
	if (axis == AxisX)
		rotationMatrix.Set(	1.0,	0.0,	0.0,
							0.0,	c,		-s,
							0.0,	s,		c);
	else if (axis == AxisY)
		rotationMatrix.Set(	c,		0.0,	s,
							0.0,	1.0,	0.0,
							-s,		0.0,	c);
	else// if (axis == AxisZ)
		rotationMatrix.Set(	c,		-s,		0.0,
							s,		c,		0.0,
							0.0,	0.0,	1.0);

	return rotationMatrix;
}

//=============================================================================
// Class:			Vector
// Function:		GetAxisName
//
// Description:		Returns the name of the specified axis.
//
// Input Arguments:
//		Axis	= const Axis& specifying the axis name we want
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the axis name
//
//=============================================================================
wxString Vector::GetAxisName(Axis axis)
{
	switch (axis)
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

//=============================================================================
// Class:			Vector
// Function:		AnglesTo
//
// Description:		Returns the angles between this and Target projected
//					onto each the X-Y, Y-Z, and X-Z planes.  The returned
//					vector stores the angles in .X (for Y-Z plane), .Y (for
//					X-Z plane), and .Z (for X-Y plane).  Angles are
//					returned in radians with the convention that the angles
//					go from this to Target.
//
// Input Arguments:
//		v	= const Vector& to reference for angle computations
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector containing the angles between this and v [rad]
//
//=============================================================================
Vector Vector::AnglesTo(const Vector &v) const
{
	Vector temp;
	double thisAngle;
	double vAngle;

	// When projected onto one of the planes, we ignore the component
	// of the vectors in the normal-to-the-plane direction.
	// First we find the angle (relative to one of the axis) of this,
	// then we find the angle of Target (relative to the same axis).
	// The difference is the angle between the vectors.

	// Y-Z Plane
	thisAngle = atan2(y, z);
	vAngle = atan2(v.y, v.z);
	temp.x = vAngle - thisAngle;

	// X-Z Plane
	thisAngle = atan2(x, z);
	vAngle = atan2(v.x, v.z);
	temp.y = vAngle - thisAngle;

	// X-Y Plane
	thisAngle = atan2(x, y);
	vAngle = atan2(v.x, v.y);
	temp.z = vAngle - thisAngle;

	return temp;
}

//=============================================================================
// Class:			friend of Vector
// Function:		operator <<
//
// Description:		Prints the contents of the specified object to an
//					ostream object.
//
// Input Arguments:
//		v	= const Vector& to be printed
//
// Output Arguments:
//		WriteOut	= &std::ostream to which the vector will be printed
//
// Return Value:
//		&std::ostream containing the printed vector
//
//=============================================================================
std::ostream &operator << (std::ostream &writeOut, const Vector &v)
{
	// Add the string to the stream
	writeOut << v.Print();

	return writeOut;
}

//=============================================================================
// Class:			Vector
// Function:		Print
//
// Description:		Prints the contents of this object to a string.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted vector data
//
//=============================================================================
wxString Vector::Print() const
{
	// Format and fill the string
	wxString temp;
	temp.Printf("[%0.3lf, %0.3lf, %0.3lf]", x, y, z);

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		Set
//
// Description:		Sets the contents of this vector as specified.
//
// Input Arguments:
//		xIn	= const double& specifying first component of the vector
//		yIn	= const double& specifying second component of the vector
//		zIn	= const double& specifying third component of the vector
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Vector::Set(const double &xIn, const double &yIn, const double &zIn)
{
	// Assign the arguments to the class members
	x = xIn;
	y = yIn;
	z = zIn;
}

//=============================================================================
// Class:			Vector
// Function:		Normalize
//
// Description:		Turns this vector into a unit vector (magnitude of 1).
//					Const version (returns a normalized vector, but doesn't
//					modify this object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector representing the normalized object
//
//=============================================================================
Vector Vector::Normalize() const
{
	// Get the length of the vector
	double magnitude = Length();

	// Avoid divide by zero
	if (magnitude == 0.0)
		return *this;

	// Normalize the components to this length
	Vector temp;
	temp.x = x / magnitude;
	temp.y = y / magnitude;
	temp.z = z / magnitude;

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		OuterProduct
//
// Description:		Performs the multiplication that results in a 3x3 matrix.
//
// Input Arguments:
//		v	= const Vector& to be multiplied
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix(3, 3)
//
//=============================================================================
Matrix Vector::OuterProduct(const Vector &v) const
{
	Matrix temp(3, 3,
		x * v.x, x * v.y, x * v.z,
		y * v.x, y * v.y, y * v.z,
		z * v.x, z * v.y, z * v.z);

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		operator +
//
// Description:		Addition operator for Vector class.
//
// Input Arguments:
//		v	= const Vector& to be added to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector result of the addition
//
//=============================================================================
Vector Vector::operator + (const Vector &v) const
{
	// Make a copy of this
	Vector temp = *this;

	// Do the addition
	temp += v;

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		operator -
//
// Description:		Subtraction operator for Vector class.
//
// Input Arguments:
//		v	= const Vector& to be subtracted from this
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector result of the subtraction
//
//=============================================================================
Vector Vector::operator - (const Vector &v) const
{
	// Make a copy of this
	Vector temp = *this;

	// Do the subtraction
	temp -= v;

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		operator *
//
// Description:		Scalar multiplication operator for Vector class.
//
// Input Arguments:
//		n	= const double& to multiply each element of this
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector result of the multiplication
//
//=============================================================================
Vector Vector::operator * (const double &n) const
{
	// Make a copy of this
	Vector temp = *this;

	// Do the multiplication
	temp *= n;

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		operator /
//
// Description:		Scalar division operator for Vector class.
//
// Input Arguments:
//		n	= const double& to divide each element of this
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector result of the division
//
//=============================================================================
Vector Vector::operator / (const double &n) const
{
	// Make a copy of this
	Vector temp = *this;

	// Do the division
	temp /= n;

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		Cross
//
// Description:		Vector cross product.
//
// Input Arguments:
//		v	= const Vector& to multiply this object
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector result of the cross product
//
//=============================================================================
Vector Vector::Cross(const Vector &v) const
{
	Vector temp;

	// Perform the cross product operation
	temp.x = y * v.z - z * v.y;
	temp.y = z * v.x - x * v.z;
	temp.z = x * v.y - y * v.x;

	return temp;
}

//=============================================================================
// Class:			Vector
// Function:		operator +=
//
// Description:		Addition assignment operator for Vector class.
//
// Input Arguments:
//		v	= const Vector& to be added to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector& result of the addition
//
//=============================================================================
Vector& Vector::operator += (const Vector &v)
{
	// Perform the addition
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

//=============================================================================
// Class:			Vector
// Function:		operator -=
//
// Description:		Subtraction assignment operator for Vector class.
//
// Input Arguments:
//		v	= const Vector& to be subtracted from this
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector& result of the subtraction
//
//=============================================================================
Vector& Vector::operator -= (const Vector &v)
{
	// Perform the subtraction
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

//=============================================================================
// Class:			Vector
// Function:		operator ==
//
// Description:		Equal comparison operator for Vector class.
//
// Input Arguments:
//		v	= const Vector& to be compared to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for is equal to, false for is not equal to
//
//=============================================================================
bool Vector::operator == (const Vector &v) const
{
	// Make the comparison
	if (x == v.x && y == v.y && z == v.z)
		return true;
	else
		return false;
}

//=============================================================================
// Class:			Vector
// Function:		operator !=
//
// Description:		Unequal comparison operator for Vector class.
//
// Input Arguments:
//		v	= const Vector& to be compared to this
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, false for is equal to, true for is not equal to
//
//=============================================================================
bool Vector::operator != (const Vector &v) const
{
	return !(*this == v);
}

}// namespace LibPlot2D
