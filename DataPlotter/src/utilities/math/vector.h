/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vector.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for Vector class.  Vector class is for three element vector.
// History:
//	2/25/2008	- Changed RAD_TO_DEG and RAD_TO_DEG from constants to inline functions, K. Loux.
//	2/26/2008	- Made order of rotations an argument in the Rotate member function, K. Loux.
//	3/3/2008	- Changed function arguments to addresses where applicable to reduce overhead, K. Loux.
//	3/9/2008	- Removed dependency on MFC class CString by switching to wxString, K. Loux.
//	3/14/2008	- Added AnglesTo function, K. Loux.
//	3/23/2008	- Moved RAD_TO_DEG and DEG_TO_RAD and constants to Convert class and changed
//				  arguments for class functions from degrees to radians, K. Loux.
//	4/11/2009	- Changed all functions to take addresses of and use const, K. Loux.
//	4/17/2009	- Renamed ROTATION_AXIS enumeration to AXIS, K. Loux
//	11/22/2009	- Moved to vMath.lib, K. Loux.
//	11/1/2010	- Removed non-const Normalize(), K. Loux.

#ifndef _VECTOR_H_
#define _VECTOR_H_

// Standard C++ headers
#include <cmath>
#include <iostream>

// wxWidgets forward declarations
class wxString;

// Local forward declarations
class Matrix;

using namespace std;

class Vector
{
public:
	// Constructors
	Vector();
	Vector(const double &_x, const double &_y, const double &_z);
	Vector(const Vector &v);

	// Destructor
	~Vector();

	// Main class data
	double x, y, z;

	// Enumerations
	enum Axis
	{
		AxisX,
		AxisY,
		AxisZ
	};

	// Vector norm
	double Length(void) const { return sqrt(x * x + y * y + z * z); };

	// Calculates the distance between this and v
	double Distance(const Vector &v) const { return sqrt((x - v.x) * (x - v.x)
		+ (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z)); };

	// Rotation functions
	void Rotate(const Vector &cor, const Vector &rotations, Axis first = AxisX,
		Axis second = AxisY, Axis tThird = AxisZ);
	void Rotate(const Vector &cor, const double &Angle, const Axis &about);
	void Rotate(const double &angle, const Axis &about);
	void Rotate(const double &angle, const Vector &rotationAxis);

	// Returns a string with the name of the specified axis
	static wxString GetAxisName(Axis axis);

	// Return the angles between this and v
	Vector AnglesTo(const Vector &v) const;

	// Prints the object to a string
	wxString Print(void) const;

	// Set this object to the specified values
	void Set(const double &_x, const double &_y, const double &_z);

	// Force magnitude == 1 while preserving the direction
	Vector Normalize(void) const;

	static Matrix GenerateRotationMatrix(const Axis &axis, const double &angle);

	// Multiplication that results in a 3x3 matrix
	Matrix OuterProduct(const Vector &v) const;

	// Operators
	Vector operator + (const Vector &v) const;
	Vector operator - (const Vector &v) const;
	double operator * (const Vector &v) const { return x * v.x + y * v.y + z * v.z; };// Dot product
	Vector operator * (const double &n) const;// Scalar multiplication
	Vector operator / (const double &n) const;// Scalar division
	Vector Cross(const Vector &v) const;
	Vector& operator += (const Vector &v);
	Vector& operator -= (const Vector &v);
	Vector& operator *= (const double &n) { x *= n; y *= n; z *= n; return *this; };
	Vector& operator /= (const double &n) { x /= n; y /= n; z /= n; return *this; };
	bool operator == (const Vector &vector) const;
	bool operator != (const Vector &vector) const;
	friend ostream& operator << (ostream &writeOut, const Vector &v);
};

#endif// _VECTOR_H_