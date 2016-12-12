/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  vector.h
// Date:  11/3/2007
// Auth:  K. Loux
// Desc:  Contains class declaration for Vector class.  Vector class is for
//        three element vector.

#ifndef VECTOR_H_
#define VECTOR_H_

// Standard C++ headers
#include <cmath>
#include <iostream>

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

// Local forward declarations
class Matrix;

class Vector
{
public:
	// Constructors
	Vector();
	Vector(const double &x, const double &y, const double &z);
	Vector(const Vector &v);

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
	double Length() const { return sqrt(x * x + y * y + z * z); };

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
	wxString Print() const;

	// Set this object to the specified values
	void Set(const double &x, const double &y, const double &z);

	// Force magnitude == 1 while preserving the direction
	Vector Normalize() const;

	static Matrix GenerateRotationMatrix(const Axis &axis, const double &angle);
	static Matrix GenerateRotationMatrix(const double &angle, const Vector& rotationAxis);

	// Multiplication that results in a 3x3 matrix
	Matrix OuterProduct(const Vector &v) const;

	// Operators
	Vector operator+(const Vector &v) const;
	Vector operator-(const Vector &v) const;
	double operator*(const Vector &v) const { return x * v.x + y * v.y + z * v.z; };// Dot product
	Vector operator*(const double &n) const;// Scalar multiplication
	Vector operator/(const double &n) const;// Scalar division
	Vector Cross(const Vector &v) const;
	Vector& operator+=(const Vector &v);
	Vector& operator-=(const Vector &v);
	Vector& operator*=(const double &n) { x *= n; y *= n; z *= n; return *this; };
	Vector& operator/=(const double &n) { x /= n; y /= n; z /= n; return *this; };
	bool operator==(const Vector &vector) const;
	bool operator!=(const Vector &vector) const;
	friend std::ostream& operator<<(std::ostream &writeOut, const Vector &v);
};

}// namespace LibPlot2D

#endif// VECTOR_H_
