/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  matrix_class.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class functionality for matrix class.
// History:

// Standard C++ headers
#include <cstdlib>
#include <cstdarg>
#include <cmath>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/math/matrix_class.h"
#include "utilities/math/vector_class.h"
#include "utilities/math/plot_math.h"

//==========================================================================
// Class:			Matrix
// Function:		Matrix
//
// Description:		Constructor for the Matrix class.  Allocates memory for
//					a matix of the specified size.
//
// Input Argurments:
//		_rows		= const unsigned int& specifying the vertical size of the matrix
//		_columns	= const unsigned int& specifying the horizontal size of the matrix
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Matrix::Matrix(const unsigned int &_rows, const unsigned int &_columns)
{
	// Initialize the number of rows and columns
	rows = _rows;
	columns = _columns;

	AllocateElements();

	// Initialize the matrix
	Zero();
}

//==========================================================================
// Class:			Matrix
// Function:		Matrix
//
// Description:		Constructor for the Matrix class.  Allocates memory for
//					a matix of the specified size and fills the matrix with
//					the specified elements.
//
// Input Argurments:
//		_rows		= const unsigned int& specifying the vertical size of the matrix
//		_columns	= const unsigned int& specifying the horizontal size of the matrix
//		element1	= double specifying the first element of the matrix
//		...			= doubles specifying the rest of the elements
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Matrix::Matrix(const unsigned int &_rows, const unsigned int &_columns, double element1, ...)
{
	// Initialize the number of rows and columns
	rows = _rows;
	columns = _columns;

	AllocateElements();

	// Declare a va_list macro and initialize it with va_start
	va_list argumentList;
	va_start(argumentList, element1);

	// Assign the first element
	elements[0][0] = element1;

	// Fill all of the elements with the arguments
	// FIXME:  There is no check to make sure the correct number of elements was
	// passed!  This could result in runtime crash or wrong calculations!
	unsigned int i;
	for (i = 1; i < rows * columns; i++)
		elements[i][i % columns] = va_arg(argumentList, double);

	// Terminate the variable argument list
	va_end(argumentList);
}

//==========================================================================
// Class:			Matrix
// Function:		Matrix
//
// Description:		Copy constructor for the Matrix class.  Performs deep copy.
//
// Input Argurments:
//		matrix	= Matrix& to copy form
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Matrix::Matrix(const Matrix &matrix)
{
	// Initialize the elements pointer
	elements = NULL;
	rows = 0;
	columns = 0;

	// Copy from the argument to this
	*this = matrix;
}

//==========================================================================
// Class:			Matrix
// Function:		~Matrix
//
// Description:		Destructor for the Matrix class.
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
Matrix::~Matrix()
{
	// Delete the elements
	FreeElements();
}

//==========================================================================
// Class:			Matrix
// Function:		SetElement
//
// Description:		Changes the element at the specified location to the
//					specified value.
//
// Input Argurments:
//		row		= const unsigned int& specifying the vertical position in the matrix
//		column	= const unsigned int& specifying the horizontal position in the matrix
//		value	= const double& specifying the new value of the element
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Matrix::SetElement(const unsigned int &row, const unsigned int &column, const double &value)
{
	// Check to make sure it is a valid index
	assert(row <= rows && column <= columns);

	// Set the element as requested
	elements[row][column] = value;

	return;
}

//==========================================================================
// Class:			Matrix
// Function:		Set
//
// Description:		Sets the values of this matrix to the arguments passed
//					to this function.
//
// Input Argurments:
//		element1	= double representing the first element
//		..			= doubles representing the other elements.  Elements are
//					  read by filling each column of a row before moving on
//					  to the next row.
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Matrix::Set(double element1, ...)
{
	// Declare a va_list macro and initialize it with va_start
	va_list argumentList;
	va_start(argumentList, element1);

	// Assign the first element
	elements[0][0] = element1;

	// Fill all of the elements with the arguments
	// FIXME:  There is no check to make sure the correct number of elements was
	// passed!  This could result in runtime crash or wrong calculations!
	unsigned int i;
	for (i = 1; i < rows * columns; i++)
		elements[i][i % rows] = va_arg(argumentList, double);

	// Terminate the variable argument list
	va_end(argumentList);

	return;
}

//==========================================================================
// Class:			Matrix
// Function:		GetElement
//
// Description:		Returns the element at the specified location.
//
// Input Argurments:
//		Row		= const int& specifying the row
//		Column = const int& specifying the column
//
// Output Arguments:
//		None
//
// Return Value:
//		double containing the value of the element at the specified location
//
//==========================================================================
double Matrix::GetElement(const int &row, const int &column) const
{
	// Return the element at the specified location
	return elements[row][column];
}

//==========================================================================
// Class:			Matrix
// Function:		MakeIdentity
//
// Description:		Makes this matrix an identity matrix.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& reference to this
//
//==========================================================================
Matrix& Matrix::MakeIdentity(void)
{
	// Determine the smaller dimension
	int minDimension;
	if (rows < columns)
		minDimension = rows;
	else
		minDimension = columns;

	// Set everything to zero
	Zero();

	// Make the diagonal elements 1.0
	int i;
	for (i = 0; i < minDimension; i++)
		SetElement(i, i, 1.0);

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		Zero
//
// Description:		Sets all elements of this matrix to zero.
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
void Matrix::Zero(void)
{
	// Go through all of the elements setting everything to zero
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] = 0.0;
	}

	return;
}

//==========================================================================
// Class:			Matrix
// Function:		GetSubMatrix
//
// Description:		Returns a sub-matrix made up of the specified portion of
//					this matrix.
//
// Input Argurments:
//		StartRow	= const unsigned int& specifying the starting row
//		StartColumn	= const unsigned int& specifying the starting column
//		SubRows		= const unsigned int& specifying the number of rows
//		SubColumns	= const unsigned int& specifying the number of columns
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix contining the specified sub-matrix
//
//==========================================================================
Matrix Matrix::GetSubMatrix(const unsigned int &startRow, const unsigned int &startColumn,
							const unsigned int &subRows, const unsigned int &subColumns) const
{
	assert(startRow + subRows < rows && startColumn + subColumns < columns);

	// Our return matrix
	Matrix subMatrix(subRows, subColumns);

	// Assign the values for the sub-matrix
	unsigned int i, j;
	for (i = 0; i < subRows; i++)
	{
		for (j = 0; j < subColumns; j++)
			subMatrix.elements[i][j] = elements[i + startRow][j + startColumn];
	}

	return subMatrix;
}

//==========================================================================
// Class:			Matrix
// Function:		GetTranspose
//
// Description:		Transposes this matrix.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix, trasposed version of this
//
//==========================================================================
Matrix Matrix::GetTranspose(void) const
{
	Matrix transpose(columns, rows);

	// Go across each row and down each column swapping the values
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			transpose.elements[j][i] = elements[i][j];
	}

	return transpose;
}

//==========================================================================
// Class:			Matrix
// Function:		LeftDivide
//
// Description:		Performs division from the left.  For example, to solve
//					Ax=b for x, left divide x = A \ b, where this matrix is A.
//					Same as A^-1 * b.
//
// Input Argurments:
//		b	= const Matrix& vector to divide this into
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix result of the division (x in the above example)
//
//==========================================================================
Matrix Matrix::LeftDivide(const Matrix &b) const
{
	return GetInverse() * b;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *
//
// Description:		Multiplication operator for the Matrix class.
//
// Input Argurments:
//		target	= const VECTOR& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const VECTOR result of the matrix multiplication
//
//==========================================================================
const VECTOR Matrix::operator * (const VECTOR &target) const
{
	assert(rows == 3 && columns == 3);

	VECTOR temp(0.0, 0.0, 0.0);

	// Check to make sure we're a 3x3 matrix
	temp.X = target.X * elements[0][0] + target.Y * elements[0][1] + target.Z * elements[0][2];
	temp.Y = target.X * elements[1][0] + target.Y * elements[1][1] + target.Z * elements[1][2];
	temp.Z = target.X * elements[2][0] + target.Y * elements[2][1] + target.Z * elements[2][2];

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *=
//
// Description:		Multiplication assignment operator for the Matrix class.
//
// Input Argurments:
//		target	= const Matrix& to multiply
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX&
//
//==========================================================================
Matrix& Matrix::operator *= (const Matrix &target)
{
	// Make sure the inner dimensions match
	assert(columns == target.rows);

	// Store the information contained in this matrix
	Matrix result(rows, target.columns);

	// Now do the multiplication, storing the results in this matrix
	unsigned int counter, i, j;
	for (i = 0; i < result.rows; i++)
	{
		for (j = 0; j < result.columns; j++)
		{
			result.elements[i][j] = 0.0;
			for (counter = 0; counter < columns; counter++)
				result.elements[i][j] += elements[i][counter] * target.elements[counter][j];
		}
	}

	// Assign the result to this
	*this = result;
	
	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		operator =
//
// Description:		Assignment operator for the Matrix class.
//
// Input Argurments:
//		target	= const Matrix& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix& result of the assignment
//
//==========================================================================
Matrix& Matrix::operator = (const Matrix &target)
{
	// Check for self assignment
	if (this == &target)
		return *this;

	FreeElements();

	// Make sure the row and column dimensions are correct
	rows = target.rows;
	columns = target.columns;

	AllocateElements();

	// Now assign the elements one by one
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] = target.elements[i][j];
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		operator +=
//
// Description:		Addition assignment operator for the Matrix class.
//
// Input Argurments:
//		target	= const Matrix& to add
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix& result of the addition
//
//==========================================================================
Matrix& Matrix::operator += (const Matrix &target)
{
	// Make sure dimensions match
	assert(columns == target.columns && rows == target.rows);

	// Add target to this element by element
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] += target.elements[i][j];
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		operator -=
//
// Description:		Subrtaction assignment operator for the Matrix class.
//
// Input Argurments:
//		target	= const Matrix& to subtract
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& result of the subtraction
//
//==========================================================================
Matrix& Matrix::operator -= (const Matrix &target)
{
	// Make sure dimensions match
	assert(columns == target.columns && rows == target.rows);

	// Add target to this element by element
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] -= target.elements[i][j];
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *=
//
// Description:		Element-wise multiplication assignment operator for the
//					Matrix class.
//
// Input Argurments:
//		target	= const double& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix& result of the multiplication
//
//==========================================================================
Matrix& Matrix::operator *=(const double &target)
{
	// Add target to this element by element
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] *= target;
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		operator /=
//
// Description:		Element-wise division assignment operator for the
//					Matrix class.
//
// Input Argurments:
//		Double	= const double& to divide by
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix& result of the division
//
//==========================================================================
Matrix& Matrix::operator /=(const double &target)
{
	// Add target to this element by element
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] /= target;
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		Print
//
// Description:		Prints the contents of this object to a wxString.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the contents of this matrix
//
//==========================================================================
wxString Matrix::Print(void) const
{
	wxString temp, intermediate;

	// Go row-by-row
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		// Go col-by-col
		for (j = 0; j < columns; j++)
		{
			// Add the current element to the temporary string
			intermediate.Printf("%7.3f\t", elements[i][j]);
			temp.Append(intermediate);
		}

		if (i < rows - 1)
			temp.Append(_T("\n"));
	}

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		GetRowReduced
//
// Description:		Performs row-reduction on this object until the matrix
//					is upper-triangular.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix& reference to this
//
//==========================================================================
Matrix Matrix::GetRowReduced(void) const
{
	unsigned int curCol, curRow, pivotCol(0), pivotRow;
	unsigned int minDimension;
	double factor;
	Matrix reduced(*this);

	if (rows < columns)
		minDimension = rows;
	else
		minDimension = columns;

	for (pivotRow = 0; pivotRow < minDimension; pivotRow++)
	{
		// Make sure the pivot is non-zero
		// If it is zero, move the row to the bottom and start over
		// (or if it is all zeros below, then advance to the next column)
		if (!PlotMath::IsZero(reduced.elements[pivotRow][pivotCol]))
		{
			for (curRow = pivotRow + 1; curRow < rows; curRow++)
			{
				// Scale the pivot row and add it to this row such that the
				// element of this row in the pivot column becomes zero
				if (!PlotMath::IsZero(reduced.elements[curRow][pivotCol]))
				{
					factor = reduced.elements[pivotRow][pivotCol] /
						reduced.elements[curRow][pivotCol];

					for (curCol = pivotCol; curCol < columns; curCol++)
						reduced.elements[curRow][curCol] =
							reduced.elements[curRow][curCol] * factor -
							reduced.elements[pivotRow][curCol];
				}
			}
		}
		else
		{
			// Find a non-zero row to swap with
			for (curRow = pivotRow + 1; curRow < rows; curRow++)
			{
				if (!PlotMath::IsZero(reduced.elements[curRow][pivotCol]))
				{
					double temp;
					for (curCol = pivotCol; curCol < columns; curCol++)
					{
						temp = reduced.elements[pivotRow][curCol];
						reduced.elements[pivotRow][curCol] =
							reduced.elements[curRow][curCol];
						reduced.elements[curRow][curCol] = temp;
					}

					// Decrement the pivot column because we need it to
					// be the pivot column again
					pivotCol--;

					// We did the swap, so we can stop searching
					break;
				}
			}

			// If we didn't find anything, we just move on, but we
			// decrement the pivot row because we need it to be the
			// pivot row again
			pivotRow--;
		}

		// Increment the pivot column
		pivotCol++;

		// If we get all the way to the end of the matrix and don't find
		// anything, then we're done!
		if (pivotCol >= columns)
			break;
	}

	return reduced;
}

//==========================================================================
// Class:			Matrix
// Function:		operator +
//
// Description:		Addition operator for the Matrix class.
//
// Input Argurments:
//		target	= Matrix to add
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix contining the result of the addition
//
//==========================================================================
const Matrix Matrix::operator + (const Matrix &target) const
{
	// Create the return matrix
	Matrix temp(target.rows, target.columns);
	temp = *this;

	// Do the addition
	temp += target;

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator -
//
// Description:		Subtraction operator for the Matrix class.
//
// Input Argurments:
//		target	= const Matrix& to subtract
//
// Output Arguments:
//		None
//
// Return Value:
//		const Matrix contining the result of the subtraction
//
//==========================================================================
const Matrix Matrix::operator - (const Matrix &target) const
{
	// Create the return matrix
	Matrix temp(target.rows, target.columns);
	temp = *this;

	// Do the subtraction
	temp -= target;

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *
//
// Description:		Multiplication operator for the Matrix class.
//
// Input Argurments:
//		target	= const Matrix& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const Matrix contining the result of the multiplication
//
//==========================================================================
const Matrix Matrix::operator * (const Matrix &target) const
{
	// Create the return matrix
	Matrix temp(target.rows, target.columns);
	temp = *this;

	// Do the multiplication
	temp *= target;

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *
//
// Description:		Element-wise multiplication operator for the Matrix class.
//
// Input Argurments:
//		target	= const double& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const Matrix contining the result of the multiplication
//
//==========================================================================
const Matrix Matrix::operator * (const double &target) const
{
	// Create the return matrix
	Matrix temp(rows, columns);
	temp = *this;

	// Do the multiplication
	temp *= target;

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator /
//
// Description:		Element-wise division operator for the Matrix class.
//
// Input Argurments:
//		target	= const double& to divide by
//
// Output Arguments:
//		None
//
// Return Value:
//		const Matrix contining the result of the division
//
//==========================================================================
const Matrix Matrix::operator / (const double &target) const
{
	// Create the return matrix
	Matrix temp(rows, columns);
	temp = *this;

	// Do the division
	temp /= target;

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator ()
//
// Description:		Overload of the () operator for this object.  Permits accessing
//					class data by using Matrix(row, column).  Non-const version.
//
// Input Argurments:
//		row		= const unsigned int& specifying the row of the desired element (0-based)
//		column	= const unsigned int& specifying the column of the desired element (0-based)
//
// Output Arguments:
//		None
//
// Return Value:
//		double&, reference to the specified element
//
//==========================================================================
double &Matrix::operator () (const unsigned int &row, const unsigned int &column)
{
	// Make sure the indecies are valid
	assert(row < rows && column < columns);

	// Return the specified element
	return elements[row][column];
}

//==========================================================================
// Class:			Matrix
// Function:		operator ()
//
// Description:		Overload of the () operator for this object.  Permits accessing
//					class data by using Matrix(Row, Column).  Const version.
//
// Input Argurments:
//		row		= const unsigned int& specifying the row of the desired element (0-based)
//		column	= const unsigned int& specifying the column of the desired element (0-based)
//
// Output Arguments:
//		None
//
// Return Value:
//		const double&, reference to the specified element
//
//==========================================================================
const double &Matrix::operator () (const unsigned int &row, const unsigned int &column) const
{
	// Make sure the indecies are valid
	assert(row < rows && column < columns);

	// Return the specified element
	return elements[row][column];
}

//==========================================================================
// Class:			Matrix
// Function:		GetInverse
//
// Description:		Returns the inverse of this matrix.  If this matrix is badly
//					scaled or is rectangular, the psuedo-inverse is returned.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix, inverse of this
//
//==========================================================================
Matrix Matrix::GetInverse(void) const
{
	if (rows != columns || GetRank() != rows)
		return GetPsuedoInverse();

	// FIXME:  Calculate the real inverse!
	return GetPsuedoInverse();
}

//==========================================================================
// Class:			Matrix
// Function:		GetPsuedoInverse
//
// Description:		Returns the pseudo-inverse of this matrix.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix, inverse of this
//
//==========================================================================
Matrix Matrix::GetPsuedoInverse(void) const
{
	// Use singular value decomposition to compute the inverse
	// SVD algorithm interpreted from Numerical Recipies in C
	Matrix U(rows, columns);
	Matrix W(columns, columns);
	Matrix V(columns, columns);

	// Copy target to U
	int i, j;
	for (i = 0; i < (int)U.rows; i++)
	{
		for (j = 0; j < (int)U.columns; j++)
			U.elements[i][j] = elements[i][j];
	}

	// Reduce to bidiagonal form
	int its, jj, k, l, nm;
	double *rv1 = new double[U.columns];// FIXME:  clean up memory here
	double anorm, c, f, g, h, s, scale, x, y, z;
	anorm = 0.0;
	g = 0.0;
	scale = 0.0;
	for (i = 0; i < (int)U.columns; i++)
	{
		l = i + 2;
		rv1[i] = scale * g;
		g = 0.0;
		scale = 0.0;
		s = 0.0;
		if (i < (int)U.rows)
		{
			for (k = i; k < (int)U.rows; k++)
				scale += fabs(U.elements[k][i]);

			if (scale != 0.0)
			{
				for (k = i; k < (int)U.rows; k++)
				{
					U.elements[k][i] /= scale;
					s += U.elements[k][i] * U.elements[k][i];
				}

				f = U.elements[i][i];
				if (f >= 0.0)
					g = -sqrt(s);
				else
					g = sqrt(s);

				h = f * g - s;
				U.elements[i][i] = f - g;

				for (j = l - 1; j < (int)U.columns; j++)
				{
					s = 0.0;
					for (k = i; k < (int)U.rows; k++)
						s += U.elements[k][i] * U.elements[k][j];
					f = s / h;
					for (k = i; k < (int)U.rows; k++)
						U.elements[k][j] += f * U.elements[k][i];
				}
				for (k = i; k < (int)U.rows; k++)
					U.elements[k][i] *= scale;
			}
		}

		W.elements[i][i] = scale * g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;

		if (i < (int)U.rows && i != (int)U.columns - 1)
		{
			for (k = l - 1; k < (int)U.columns; k++)
				scale += fabs(U.elements[i][k]);

			if (scale != 0.0)
			{
				for (k = l - 1; k < (int)U.columns; k++)
				{
					U.elements[i][k] /= scale;
					s += U.elements[i][k] * U.elements[i][k];
				}

				f = U.elements[i][l - 1];
				if (f >= 0.0)
					g = -sqrt(s);
				else
					g =sqrt(s);

				h = f * g - s;
				U.elements[i][l - 1] = f - g;
                    
				for (k = l - 1; k < (int)U.columns; k++)
					rv1[k] = U.elements[i][k] / h;

				for (j = l - 1; j < (int)U.rows; j++)
				{
					s = 0.0;
					for (k = l - 1; k < (int)U.columns; k++)
						s += U.elements[j][k] * U.elements[i][k];
					for (k = l - 1; k < (int)U.columns; k++)
						U.elements[j][k] += s * rv1[k];
				}

				for (k = l - 1; k < (int)U.columns; k++)
					U.elements[i][k] *= scale;
			}
		}

		if (anorm < fabs((W.elements[i][i]) + fabs(rv1[i])))
			anorm = fabs(W.elements[i][i]) + fabs(rv1[i]);
	}

	// Accumulation of right-hand transforms
	l = 0;// This value isn't used, but it avoids a java compiler warning
	for (i = U.columns - 1; i >= 0; i--)
	{
		if (i < (int)U.columns - 1)
		{
			if (g != 0.0)
			{
				for (j = l; j < (int)U.columns; j++)
					V.elements[j][i] =
							(U.elements[i][j] / U.elements[i][l])
							/ g;

				for (j = l; j < (int)U.columns; j++)
				{
					s = 0.0;
					for (k = l; k < (int)U.columns; k++)
						s += U.elements[i][k] * V.elements[k][j];

					for (k = l; k < (int)U.columns; k++)
						V.elements[k][j] += s * V.elements[k][i];
				}
			}

			for (j = l; j < (int)U.columns; j++)
			{
				V.elements[i][j] = 0.0;
				V.elements[j][i] = 0.0;
			}
		}
		V.elements[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}

	// Accumulation of left-hand transforms
	int minDimension;
	if (U.rows < U.columns)
		minDimension = U.rows;
	else
		minDimension = U.columns;
	for (i = minDimension - 1; i >= 0; i--)
	{
		l = i + 1;
		g = W.elements[i][i];
		for (j = l; j < (int)U.columns; j++)
			U.elements[i][j] = 0.0;

		if (g != 0.0)
		{
			g = 1.0 / g;
			for (j = l; j < (int)U.columns; j++)
			{
				s = 0.0;
				for (k = l; k < (int)U.rows; k++)
					s += U.elements[k][i] * U.elements[k][j];

				f = (s / U.elements[i][i]) * g;

				for (k = i; k < (int)U.rows; k++)
					U.elements[k][j] += f * U.elements[k][i];
			}

			for (j = i; j < (int)U.rows; j++)
				U.elements[j][i] *= g;
		}
		else
		{
			for (j = i; j < (int)U.rows; j++)
				U.elements[j][i] = 0.0;
		}
		U.elements[i][i]++;
	}

	// Diagonalization of the bidiagonal form
	bool finished;
	double eps = 1e-6;
	for (k = U.columns - 1; k >= 0; k--)
	{
		for (its = 0; its < 30; its++)
		{
			finished = false;
			nm = 0;// This value isn't used, but it avoids compiler warnings
			for (l = k; l >= 0; l--)
			{
				nm = l - 1;
				if (l == 0 || fabs(rv1[l]) <= eps * anorm)
				{
					finished = true;
					break;
				}

				if (fabs(W.elements[nm][nm]) <= eps * anorm)
					break;
			}

			if (!finished)
			{
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++)
				{
					f = s * rv1[i];
					rv1[i] = c * rv1[i];

					if (fabs(f) <= eps * anorm)
						break;

					g = W.elements[i][i];
					h = pythag(f, g);
					W.elements[i][i] = h;
					h = 1.0 / h;
					c = g * h;
					s = -f * h;
					for (j = 0; j < (int)U.rows; j++)
					{
						y = U.elements[j][nm];
						z = U.elements[j][i];
						U.elements[j][nm] = y * c + z * s;
						U.elements[j][i] = z * c - y * s;
					}
				}
			}

			z = W.elements[k][k];
			if (l == k)
			{
				if (z < 0.0)
				{
					W.elements[k][k] = -z;
					for (j = 0; j < (int)U.columns; j++)
						V.elements[j][k] = -V.elements[j][k];
				}
				break;
			}

			// Print an error if we've hit the iteration limit
			if (its == 29)
			{
				// FIXME:  Print error message?
			}

			x = W.elements[l][l];
			nm = k - 1;
			y = W.elements[nm][nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y-z) * (y+z) + (g-h) * (g+h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			if (f >= 0.0)
				f = ((x-z) * (x+z) + h * ((y / (f + fabs(g))) - h)) / x;
			else
				f = ((x-z) * (x+z) + h * ((y / (f - fabs(g))) - h)) / x;

			c = 1.0;
			s = 1.0;
			for (j = l; j <= nm; j++)
			{
				i = j + 1;
				g = rv1[i];
				y = W.elements[i][i];
				h = s * g;
				g = c * g;
				z = pythag(f,h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;

				for (jj = 0; jj < (int)U.columns; jj++)
				{
					x = V.elements[jj][j];
					z = V.elements[jj][i];
					V.elements[jj][j] = x * c + z * s;
					V.elements[jj][i] = z * c - x * s;
				}

				z = pythag(f, h);
				W.elements[j][j] = z;

				if (z != 0.0)
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}

				f = c * g + s * y;
				x = c * y - s * g;

				for (jj = 0; jj < (int)U.rows; jj++)
				{
					y = U.elements[jj][j];
					z = U.elements[jj][i];
					U.elements[jj][j] = y * c + z * s;
					U.elements[jj][i] = z * c - y * s;
				}
			}

			rv1[l] = 0.0;
			rv1[k] = f;
			W.elements[k][k] = x;
		}
	}

	// Remove zero-value singular values and the corresponding columns and
	// rows from the U and V matrices
	// Without this, the results are close, but this makes them much better
	for (i = 0; i < (int)W.columns; i++)
	{
		// No need to use Math.abs - we've already ensured positive values
		if (PlotMath::IsZero(W.elements[i][i]))
		{
			W.elements[i][i] = 1.0;
			U.elements[i][i] = 0.0;
			V.elements[i][i] = 0.0;
		}
	}

	// Some testing aids
	/*Matrix IfromU = U.GetTranspose() * U;
	Matrix IfromV = V.GetTranspose() * V;
	Matrix IfromV2 = V * V.GetTranspose();
	Matrix originalAgain = U * W * V.GetTranspose();*/

	// Invert the components of W along the diagonal
	for (i = 0; i < (int)W.columns; i++)
		W.elements[i][i] = 1.0 / W.elements[i][i];

	return V * W * U.GetTranspose();
}

//==========================================================================
// Class:			Matrix
// Function:		pythag
//
// Description:		Helper method for SVD calculation (used in psuedo-inverse).
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Matrix::pythag(const double& a, const double &b) const
{
	double absa = fabs(a);
	double absb = fabs(b);

	if (absa > absb)
		return absa * sqrt(1.0 + absb * absb / (absa * absa));
	else if (absb == 0.0)
		return 0.0;

	return absb * sqrt(1.0 + absa * absa / (absb * absb));
}

//==========================================================================
// Class:			Matrix
// Function:		GetRank
//
// Description:		Returns the inverse of this matrix.  If this matrix is badly
//					scaled or is rectangular, the psuedo-inverse is returned.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int indicating the rank of this matrix
//
//==========================================================================
unsigned int Matrix::GetRank(void) const
{
	Matrix reduced = GetRowReduced();

	unsigned int rank(0), curRow, curCol;
	for (curRow = 0; curRow < rows; curRow++)
	{
		for (curCol = 0; curCol < columns; curCol++)
		{
			if (!PlotMath::IsZero(elements[curRow][curCol]))
			{
				// Row contained a non-zero element - increment the rank
				// and stop looking at the other elements in this row
				rank++;
				break;
			}
		}
	}
	
	return rank;
}

//==========================================================================
// Class:			Matrix
// Function:		FreeElements
//
// Description:		Frees memory associated with this object.
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
void Matrix::FreeElements(void)
{
	unsigned int i;
	for (i = 0; i < rows; i++)
		delete [] elements[i];
	delete [] elements;
	elements = NULL;
}

//==========================================================================
// Class:			Matrix
// Function:		AllocateElements
//
// Description:		Allocates memory for the elements according to the number
//					of rows and columns that make up this object.
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
void Matrix::AllocateElements(void)
{
	elements = new double*[rows];
	unsigned int i;
	for (i = 0; i < rows; i++)
		elements[i] = new double[columns];

	return;
}