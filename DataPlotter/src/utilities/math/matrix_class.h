/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  matrix_class.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class declaration for MATRIX classes.
// History:

#ifndef _MATRIX_CLASS_H_
#define _MATRIX_CLASS_H_

// wxWidgets forward declarations
class wxString;

// vMath forward declarations
class VECTOR;

class MATRIX
{
public:
	// Constructors
	MATRIX(const int &_Rows, const int &_Columns);
	MATRIX(const int &_Rows, const int &_Columns, double Element1, ...);
	MATRIX(const MATRIX &Matrix);

	// Destructor
	~MATRIX();

	// Sets the values of all of the elements
	void Set(double Element1, ...);

	// Make all elements zero
	void Zero(void);

	// Makes this matrix an identity matrix
	MATRIX& MakeIdentity(void);

	// Transposition function
	MATRIX& Transpose(void);

	// Returns a matrix containing a sub-set of the contents of this matrix
	MATRIX GetSubMatrix(const int &StartRow, const int &StartColumn,
		const int &SubRows, const int &SubColumns) const;

	// Retrieve properties of this matrix
	inline int GetNumberOfRows(void) const { return Rows; };
	inline int GetNumberOfColumns(void) const { return Columns; };
	double GetElement(const int &Row, const int &Column) const;

	// Returns a pointer to the start of the array
	inline double *GetFirstElementPointer(void) const { return Elements; };

	// Set value of element at the specified location
	void SetElement(const int &Row, const int &Column, const double &Value);

	// Print this object to a string
	wxString Print(void) const;

	// Operators
	MATRIX& operator += (const MATRIX &Matrix);
	MATRIX& operator -= (const MATRIX &Matrix);
	MATRIX& operator *= (const MATRIX &Matrix);
	MATRIX& operator *= (const double &Double);
	MATRIX& operator /= (const double &Double);
	MATRIX& operator = (const MATRIX &Matrix);
	double &operator () (const int &Row, const int &Column);
	const MATRIX operator + (const MATRIX &Matrix) const;
	const MATRIX operator - (const MATRIX &Matrix) const;
	const MATRIX operator * (const MATRIX &Matrix) const;
	const MATRIX operator * (const double &Double) const;
	const VECTOR operator * (const VECTOR &Vector) const;
	const MATRIX operator / (const double &Double) const;
	const double &operator () (const int &Row, const int &Column) const;

	//VECTOR UnderVector (VECTOR Vector);// Left divide in MATLAB -> x = A\b where x and b are VECTORs
	MATRIX& RowReduce(void);

private:
	// The size of this matrix
	int Rows;
	int Columns;

	// The array of elements of this matrix
	double *Elements;
};

#endif// _MATRIX_CLASS_H_