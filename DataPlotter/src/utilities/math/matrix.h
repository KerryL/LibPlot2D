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

class Matrix
{
public:
	// Constructors
	Matrix();
	Matrix(const unsigned int &_Rows, const unsigned int &_Columns);
	Matrix(const unsigned int &_Rows, const unsigned int &_Columns, double Element1, ...);
	Matrix(const Matrix &matrix);

	// Destructor
	~Matrix();

	// Sets the values of all of the elements
	void Set(double Element1, ...);

	// Make all elements zero
	void Zero(void);

	// Makes this matrix an identity matrix
	Matrix& MakeIdentity(void);
	static Matrix GetIdentity(const unsigned int &_rows, const unsigned int &_columns = 0);

	bool IsSquare(void) const { return rows == columns; };
	void Resize(const unsigned int &_rows, const unsigned int &_columns);
	Matrix& RemoveRow(const unsigned int &row);
	Matrix& RemoveColumn(const unsigned int &column);

	unsigned int GetMinimumDimension(void) const { return (rows < columns) ? rows : columns; };

	// Returns a matrix containing a sub-set of the contents of this matrix
	Matrix GetSubMatrix(const unsigned int &startRow, const unsigned int &startColumn,
		const unsigned int &subRows, const unsigned int &subColumns) const;

	// Retrieve properties of this matrix
	inline unsigned int GetNumberOfRows(void) const { return rows; };
	inline unsigned int GetNumberOfColumns(void) const { return columns; };
	double GetElement(const int &row, const int &column) const;

	// Set value of element at the specified location
	void SetElement(const unsigned int &row, const unsigned int &column, const double &value);

	// Print this object to a string
	wxString Print(void) const;

	// Operators
	Matrix& operator += (const Matrix &target);
	Matrix& operator -= (const Matrix &target);
	Matrix& operator *= (const Matrix &target);
	Matrix& operator *= (const double &target);
	Matrix& operator /= (const double &target);
	Matrix& operator = (const Matrix &target);
	double &operator () (const unsigned int &row, const unsigned int &column);
	const Matrix operator + (const Matrix &target) const;
	const Matrix operator - (const Matrix &target) const;
	const Matrix operator * (const Matrix &target) const;
	const Matrix operator * (const double &target) const;
	const VECTOR operator * (const VECTOR &target) const;
	const Matrix operator / (const double &target) const;
	const double &operator () (const unsigned int &row, const unsigned int &column) const;

	// Common matrix operations ------------------------------------
	bool GetSingularValueDecomposition(Matrix &U, Matrix &V, Matrix &W) const;

	Matrix GetTranspose(void) const;
	Matrix GetInverse(void) const;
	Matrix GetPsuedoInverse(void) const;
	Matrix GetDiagonalInverse(void) const;

	Matrix LeftDivide(const Matrix& b) const;// x = A \ b
	Matrix GetRowReduced(void) const;
	unsigned int GetRank(void) const;

private:
	// The size of this matrix
	unsigned int rows;
	unsigned int columns;

	// The array of elements of this matrix
	double **elements;

	void FreeElements(void);
	void AllocateElements(void);

	// Helper function for SVD algorithm
	double pythag(const double& a, const double &b) const;
};

#endif// _MATRIX_CLASS_H_