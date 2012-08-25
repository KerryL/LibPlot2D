/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  matrix.cpp
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
#include "utilities/math/matrix.h"
#include "utilities/math/vector.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			Matrix
// Function:		Matrix
//
// Description:		Constructor for the Matrix class.  Does not allocate any
//					memory.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Matrix::Matrix()
{
	rows = 0;
	columns = 0;
	elements = NULL;
}

//==========================================================================
// Class:			Matrix
// Function:		Matrix
//
// Description:		Constructor for the Matrix class.  Allocates memory for
//					a matix of the specified size.
//
// Input Arguments:
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
	rows = _rows;
	columns = _columns;

	AllocateElements();
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
// Input Arguments:
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
	rows = _rows;
	columns = _columns;

	AllocateElements();

	va_list argumentList;
	va_start(argumentList, element1);

	elements[0][0] = element1;

	// Fill all of the elements with the arguments
	// NOTE:  There is no check to make sure the correct number of elements was
	// passed!  This could result in runtime crash or wrong calculations!
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
		{
			if (i != 0 || j != 0)// Already assigned element [0][0]
				elements[i][j] = va_arg(argumentList, double);
		}
	}

	va_end(argumentList);
}

//==========================================================================
// Class:			Matrix
// Function:		Matrix
//
// Description:		Copy constructor for the Matrix class.  Performs deep copy.
//
// Input Arguments:
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
	elements = NULL;
	rows = 0;
	columns = 0;

	*this = matrix;
}

//==========================================================================
// Class:			Matrix
// Function:		~Matrix
//
// Description:		Destructor for the Matrix class.
//
// Input Arguments:
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
	FreeElements();
}

//==========================================================================
// Class:			Matrix
// Function:		SetElement
//
// Description:		Changes the element at the specified location to the
//					specified value.
//
// Input Arguments:
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
	assert(row < rows && column < columns);

	elements[row][column] = value;
}

//==========================================================================
// Class:			Matrix
// Function:		Set
//
// Description:		Sets the values of this matrix to the arguments passed
//					to this function.
//
// Input Arguments:
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
	va_list argumentList;
	va_start(argumentList, element1);

	elements[0][0] = element1;

	// Fill all of the elements with the arguments
	// NOTE:  There is no check to make sure the correct number of elements was
	// passed!  This could result in runtime crash or wrong calculations!
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
		{
			if (i != 0 || j != 0)// Already assigned element [0][0]
				elements[i][j] = va_arg(argumentList, double);
		}
	}

	va_end(argumentList);;
}

//==========================================================================
// Class:			Matrix
// Function:		GetElement
//
// Description:		Returns the element at the specified location.
//
// Input Arguments:
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
	return elements[row][column];
}

//==========================================================================
// Class:			Matrix
// Function:		MakeIdentity
//
// Description:		Makes this matrix an identity matrix.
//
// Input Arguments:
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
	Zero();

	unsigned int i;
	for (i = 0; i < GetMinimumDimension(); i++)
		elements[i][i] = 1.0;

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		Zero
//
// Description:		Sets all elements of this matrix to zero.
//
// Input Arguments:
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
	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
			elements[i][j] = 0.0;
	}
}

//==========================================================================
// Class:			Matrix
// Function:		GetSubMatrix
//
// Description:		Returns a sub-matrix made up of the specified portion of
//					this matrix.
//
// Input Arguments:
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

	Matrix subMatrix(subRows, subColumns);

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
// Input Arguments:
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
// Input Arguments:
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
	// Normal equations solution (not very robust?)
	//return GetInverse() * b;

	// Use singular value decomposition
	Matrix U;
	Matrix V;
	Matrix W;

	if (!GetSingularValueDecomposition(U, V, W))
	{
		// TODO:  Generate an error?
		return *this;
	}

	return V * W.GetDiagonalInverse().GetTranspose() * U.GetTranspose() * b;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *
//
// Description:		Multiplication operator for the Matrix class.
//
// Input Arguments:
//		target	= const Vector& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const Vector result of the matrix multiplication
//
//==========================================================================
const Vector Matrix::operator * (const Vector &target) const
{
	assert(rows == 3 && columns == 3);

	Vector temp(0.0, 0.0, 0.0);

	// Check to make sure we're a 3x3 matrix
	temp.x = target.x * elements[0][0] + target.y * elements[0][1] + target.z * elements[0][2];
	temp.y = target.x * elements[1][0] + target.y * elements[1][1] + target.z * elements[1][2];
	temp.z = target.x * elements[2][0] + target.y * elements[2][1] + target.z * elements[2][2];

	return temp;
}

//==========================================================================
// Class:			Matrix
// Function:		operator *=
//
// Description:		Multiplication assignment operator for the Matrix class.
//
// Input Arguments:
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
	assert(columns == target.rows);

	Matrix result(rows, target.columns);

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

	*this = result;
	
	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		operator =
//
// Description:		Assignment operator for the Matrix class.
//
// Input Arguments:
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
	if (this == &target)
		return *this;

	Resize(target.rows, target.columns);

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
// Input Arguments:
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
	assert(columns == target.columns && rows == target.rows);

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
// Input Arguments:
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
	assert(columns == target.columns && rows == target.rows);

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
// Input Arguments:
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
// Input Arguments:
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
// Input Arguments:
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

	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
		{
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
// Description:		Performs row-reduction on copy of this until the matrix
//					is upper-triangular.
//
// Input Arguments:
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
	unsigned int curRow, pivotCol(0), pivotRow;
	Matrix reduced(*this);

	for (pivotRow = 0; pivotRow < GetMinimumDimension(); pivotRow++)
	{
		if (!PlotMath::IsZero(reduced.elements[pivotRow][pivotCol]))
		{
			for (curRow = pivotRow + 1; curRow < rows; curRow++)
			{
				if (!PlotMath::IsZero(reduced.elements[curRow][pivotCol]))
					reduced.ZeroRowByScalingAndAdding(pivotRow, pivotCol, curRow);
			}
		}
		else
		{
			for (curRow = pivotRow + 1; curRow < rows; curRow++)
			{
				if (!PlotMath::IsZero(reduced.elements[curRow][pivotCol]))
				{
					reduced.SwapRows(pivotRow, curRow);
					pivotCol--;
					break;
				}
			}

			pivotRow--;
		}

		pivotCol++;
		if (pivotCol >= columns)
			break;
	}

	return reduced;
}

//==========================================================================
// Class:			Matrix
// Function:		SwapRows
//
// Description:		Swaps the values in the specified rows.
//
// Input Arguments:
//		r1	= const unsigned int&
//		r2	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix&, reference to this
//
//==========================================================================
Matrix& Matrix::SwapRows(const unsigned int &r1, const unsigned int &r2)
{
	double swap;
	unsigned int i;
	for (i = 0; i < columns; i++)
	{
		swap = elements[r1][i];
		elements[r1][i] = elements[r2][i];
		elements[r2][i] = swap;
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		ZeroRowByScalingAndAdding
//
// Description:		Makes the element at (targtRow, pivotColumn) zero by scaling
//					the target row and adding it to the pivot row.
//
// Input Arguments:
//		pivotrow		= const unsigned int&
//		pivotColumn		= const unsigned int&
//		targetColumn	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Matrix::ZeroRowByScalingAndAdding(const unsigned int &pivotRow,
	const unsigned int &pivotColumn, const unsigned int &targetRow)
{
	double factor = elements[pivotRow][pivotColumn] / elements[targetRow][pivotColumn];

	unsigned int i;
	for (i = pivotColumn; i < columns; i++)
		elements[targetRow][i] = elements[targetRow][i] * factor - elements[pivotRow][i];
}

//==========================================================================
// Class:			Matrix
// Function:		operator+
//
// Description:		Addition operator for the Matrix class.
//
// Input Arguments:
//		target	= Matrix to add
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix contining the result of the addition
//
//==========================================================================
const Matrix Matrix::operator+(const Matrix &target) const
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
// Function:		operator-
//
// Description:		Subtraction operator for the Matrix class.
//
// Input Arguments:
//		target	= const Matrix& to subtract
//
// Output Arguments:
//		None
//
// Return Value:
//		const Matrix contining the result of the subtraction
//
//==========================================================================
const Matrix Matrix::operator-(const Matrix &target) const
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
// Function:		operator*
//
// Description:		Multiplication operator for the Matrix class.
//
// Input Arguments:
//		target	= const Matrix& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const Matrix contining the result of the multiplication
//
//==========================================================================
const Matrix Matrix::operator*(const Matrix &target) const
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
// Function:		operator*
//
// Description:		Element-wise multiplication operator for the Matrix class.
//
// Input Arguments:
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
// Input Arguments:
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
// Input Arguments:
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
// Input Arguments:
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
// Input Arguments:
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
	if (!IsSquare() || GetRank() != rows)
		return GetPsuedoInverse();

	// NOTE:  I'm not sure there is a point to having two inverse methods?
	return GetPsuedoInverse();
}

//==========================================================================
// Class:			Matrix
// Function:		GetPsuedoInverse
//
// Description:		Returns the pseudo-inverse of this matrix.
//
// Input Arguments:
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
	Matrix U;
	Matrix W;
	Matrix V;

	if (!GetSingularValueDecomposition(U, V, W))
	{
		// TODO:  Generate an error?
		return *this;
	}

	return V * W.GetDiagonalInverse() * U.GetTranspose();
}

//==========================================================================
// Class:			Matrix
// Function:		GetDiagonalInverse
//
// Description:		Returns the inverse of a diagonal matrix.  Calling this
//					on a non-diagonal matrix will return a result, but it
//					will be meaningless.  There is no check to ensure this
//					is only called on diagonal matrices.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix, inverse of this (as long as this is a diagonal matrix)
//
//==========================================================================
Matrix Matrix::GetDiagonalInverse(void) const
{
	Matrix inverse(*this);

	// Invert the components of W along the diagonal
	unsigned int i;
	for (i = 0; i < inverse.GetMinimumDimension(); i++)
	{
		if (PlotMath::IsZero(elements[i][i]))
			inverse.elements[i][i] = 0.0;
		else
			inverse.elements[i][i] = 1.0 / elements[i][i];
	}

	return inverse;
}

//==========================================================================
// Class:			Matrix
// Function:		pythag
//
// Description:		Helper method for SVD calculation (used in psuedo-inverse).
//
// Input Arguments:
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
// Input Arguments:
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
	// TODO:  Is it better to use SVD for this?  Rank = # of non-zero singular values
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
// Input Arguments:
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
// Input Arguments:
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
}

//==========================================================================
// Class:			Matrix
// Function:		Resize
//
// Description:		Resizes the dynamic memory for this object to accommodate
//					the specified size.
//
// Input Arguments:
//		_rows		= const unsigned int& specifying new vertical dimension
//		_columns	= const unsigned int& specifying new horizontal dimension
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Matrix::Resize(const unsigned int &_rows, const unsigned int &_columns)
{
	FreeElements();

	rows = _rows;
	columns = _columns;

	AllocateElements();
}

//==========================================================================
// Class:			Matrix
// Function:		GetSingularValueDecomposition
//
// Description:		Computes singular value decomposition of this matrix.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		U	= Matrix&
//		V	= Matrix&
//		W	= Matrix& containing the singular values along its diagonal
//
// Return Value:
//		true if success, false if iteration limit was reached
//
//==========================================================================
bool Matrix::GetSingularValueDecomposition(Matrix &U, Matrix &V, Matrix &W) const
{
	// SVD algorithm interpreted from Numerical Recipies in C
	U.Resize(rows, columns);
	W.Resize(columns, columns);
	//U.Resize(rows, rows);// Minimal solution has these dimensions, but
	//W.Resize(rows, columns);// apparently this algorithm does not compute the minimal solution
	W.Zero();
	V.Resize(columns, columns);

	// Copy target to U
	int i, j;
	for (i = 0; i < (int)U.rows; i++)
	{
		for (j = 0; j < (int)V.rows; j++)
			U.elements[i][j] = elements[i][j];
	}

	// Reduce to bidiagonal form
	int its, jj, k, l(0), nm(0);
	double *rv1 = new double[V.rows];
	double anorm, c, f, g, h, s, scale, x, y, z;
	anorm = 0.0;
	g = 0.0;
	scale = 0.0;
	for (i = 0; i < (int)V.rows; i++)
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

				for (j = l - 1; j < (int)V.rows; j++)
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

		if (i < (int)U.rows && i != (int)V.rows - 1)
		{
			for (k = l - 1; k < (int)V.rows; k++)
				scale += fabs(U.elements[i][k]);

			if (scale != 0.0)
			{
				for (k = l - 1; k < (int)V.rows; k++)
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
                    
				for (k = l - 1; k < (int)V.rows; k++)
					rv1[k] = U.elements[i][k] / h;

				for (j = l - 1; j < (int)U.rows; j++)
				{
					s = 0.0;
					for (k = l - 1; k < (int)V.rows; k++)
						s += U.elements[j][k] * U.elements[i][k];
					for (k = l - 1; k < (int)V.rows; k++)
						U.elements[j][k] += s * rv1[k];
				}

				for (k = l - 1; k < (int)V.rows; k++)
					U.elements[i][k] *= scale;
			}
		}

		if (anorm < fabs((W.elements[i][i]) + fabs(rv1[i])))
			anorm = fabs(W.elements[i][i]) + fabs(rv1[i]);
	}

	// Accumulation of right-hand transforms
	for (i = V.rows - 1; i >= 0; i--)
	{
		if (i < (int)V.rows - 1)
		{
			if (g != 0.0)
			{
				for (j = l; j < (int)V.rows; j++)
					V.elements[j][i] =
							(U.elements[i][j] / U.elements[i][l])
							/ g;

				for (j = l; j < (int)V.rows; j++)
				{
					s = 0.0;
					for (k = l; k < (int)V.rows; k++)
						s += U.elements[i][k] * V.elements[k][j];

					for (k = l; k < (int)V.rows; k++)
						V.elements[k][j] += s * V.elements[k][i];
				}
			}

			for (j = l; j < (int)V.rows; j++)
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
	for (i = GetMinimumDimension() - 1; i >= 0; i--)
	{
		l = i + 1;
		g = W.elements[i][i];
		for (j = l; j < (int)V.rows; j++)
			U.elements[i][j] = 0.0;

		if (g != 0.0)
		{
			g = 1.0 / g;
			for (j = l; j < (int)V.rows; j++)
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
	int its_limit = 30;
	for (k = V.rows - 1; k >= 0; k--)
	{
		for (its = 0; its < its_limit; its++)
		{
			finished = false;
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
					for (j = 0; j < (int)V.rows; j++)
						V.elements[j][k] = -V.elements[j][k];
				}
				break;
			}

			// Print an error if we've hit the iteration limit
			if (its == its_limit - 1)
				return false;

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

				for (jj = 0; jj < (int)V.rows; jj++)
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

	delete [] rv1;

	// Remove zero-value singular values and the corresponding columns and
	// rows from the U and V matrices
	// Without this, the results are close, but this makes them much better
	for (i = 0; i < (int)GetMinimumDimension(); i++)
	{
		// No need to use Math.abs - we've already ensured positive values
		if (PlotMath::IsZero(W.elements[i][i]))
		{
			W.elements[i][i] = 0.0;
			U.elements[i][i] = 0.0;
		}
	}

	// Sort singular values (and corresponding columns of U and V) by decreasing magnitude
	its = 1;
	double sw;
	double *su = new double[U.rows];
	double *sv = new double[V.rows];

	do
	{
		its *= 3;
		its++;
	} while (its <= (int)V.rows);

	do
	{
		its /= 3;
		for (i = its; i < (int)V.rows; i++)
		{
			sw = W.elements[i][i];
			for (k = 0; k < (int)U.rows; k++)
				su[k] = U.elements[k][i];

			for (k = 0; k < (int)V.rows; k++)
				sv[k] = V.elements[k][i];

			j = i;
			while (W.elements[j - its][j - its] < sw)
			{
				W.elements[j][j] = W.elements[j - its][j - its];
				for (k = 0; k < (int)U.rows; k++)
					U.elements[k][j] = U.elements[k][j - its];

				for (k = 0; k < (int)V.rows; k++)
					V.elements[k][j] = V.elements[k][j - its];

				j -= its;
				if (j < its)
					break;
			}

			W.elements[j][j] = sw;

			for (k = 0; k < (int)U.rows; k++)
				U.elements[k][j] = su[k];

			for (k = 0; k < (int)V.rows; k++)
				V.elements[k][j] = sv[k];
		}
	} while (its > 1);

	for (k = 0; k < (int)V.rows; k++)
	{
		s = 0.0;
		for (i = 0; i < (int)U.rows; i++)
		{
			if (U.elements[i][k] < 0.0)
				s++;
		}

		for (j = 0; j < (int)V.rows; j++)
		{
			if (V.elements[j][k] < 0.0)
				s++;
		}

		if (s > (U.rows + V.rows) / 2)
		{
			for (i = 0; i < (int)U.rows; i++)
				U.elements[i][k] = -U.elements[i][k];

			for (j = 0; j < (int)V.rows; j++)
				V.elements[j][k] = -V.elements[j][k];
		}
	}

	delete [] su;
	delete [] sv;

	return true;
}

//==========================================================================
// Class:			Matrix
// Function:		RemoveRow
//
// Description:		Removes the specified row from the matrix
//
// Input Arguments:
//		None
//
// Output Arguments:
//		row	= const unsigned int& specifying the row to remove
//
// Return Value:
//		Matrix&, reference to this
//
//==========================================================================
Matrix& Matrix::RemoveRow(const unsigned int &row)
{
	assert(row < rows);

	Matrix original(*this);
	Resize(rows - 1, columns);

	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
		{
			if (i < row)
				elements[i][j] = original.elements[i][j];
			else
				elements[i][j] = original.elements[i + 1][j];
		}
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		RemoveColumn
//
// Description:		Removes the specified column from the matrix.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		column	= const unsigned int& specifying the column to remove
//
// Return Value:
//		Matrix&, reference to this
//
//==========================================================================
Matrix& Matrix::RemoveColumn(const unsigned int &column)
{
	assert(column < columns);

	Matrix original(*this);
	Resize(rows, columns - 1);

	unsigned int i, j;
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < columns; j++)
		{
			if (j < column)
				elements[i][j] = original.elements[i][j];
			else
				elements[i][j] = original.elements[i][j + 1];
		}
	}

	return *this;
}

//==========================================================================
// Class:			Matrix
// Function:		GetIdentity
//
// Description:		Returns an identity matrix of the specified dimension.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		_rows	= const unsigned int& specifying the number of rows
//		_column	= const unsigned int& specifying the number of columns; zero
//				  is interpreted as square (set equal to _rows)
//
// Return Value:
//		Matrix containing 1s along diagonal and zeros elsewhere
//
//==========================================================================
Matrix Matrix::GetIdentity(const unsigned int &_rows, const unsigned int &_columns)
{
	Matrix identity;

	if (_columns == 0)
		identity.Resize(_rows, _rows);
	else
		identity.Resize(_rows, _columns);

	return identity.MakeIdentity();
}