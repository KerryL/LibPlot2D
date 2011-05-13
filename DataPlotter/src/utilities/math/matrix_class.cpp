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

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/math/matrix_class.h"
#include "utilities/math/vector_class.h"

//==========================================================================
// Class:			MATRIX
// Function:		MATRIX
//
// Description:		Constructor for the MATRIX class.  Allocates memory for
//					a matix of the specified size.
//
// Input Argurments:
//		_Rows		= const int& specifying the vertical size of the matrix
//		_Columns	= const int& specifying the horizontal size of the matrix
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MATRIX::MATRIX(const int &_Rows, const int &_Columns)
{
	// Initialize the number of rows and columns
	Rows = _Rows;
	Columns = _Columns;

	// Allocate memory for the elements
	Elements = new double[Rows * Columns];

	// Initialize the matrix
	Zero();
}

//==========================================================================
// Class:			MATRIX
// Function:		MATRIX
//
// Description:		Constructor for the MATRIX class.  Allocates memory for
//					a matix of the specified size and fills the matrix with
//					the specified elements.
//
// Input Argurments:
//		_Rows		= const int& specifying the vertical size of the matrix
//		_Columns	= const int& specifying the horizontal size of the matrix
//		Element1	= double specifying the first element of the matrix
//		...			= doubles specifying the rest of the elements
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MATRIX::MATRIX(const int &_Rows, const int &_Columns, double Element1, ...)
{
	// Initialize the number of rows and columns
	Rows = _Rows;
	Columns = _Columns;

	// Allocate memory for the elements
	Elements = new double[Rows * Columns];

	// Declare a va_list macro and initialize it with va_start
	va_list ArgumentList;
	va_start(ArgumentList, Element1);

	// Assign the first element
	Elements[0] = Element1;

	// Fill all of the elements with the arguments
	// FIXME:  There is no check to make sure the correct number of elements was
	// passed!  This could result in runtime crash or wrong calculations!
	int i;
	for (i = 1; i < Rows * Columns; i++)
		Elements[i] = va_arg(ArgumentList, double);

	// Terminate the variable argument list
	va_end(ArgumentList);
}

//==========================================================================
// Class:			MATRIX
// Function:		MATRIX
//
// Description:		Copy constructor for the MATRIX class.  Performs deep copy.
//
// Input Argurments:
//		Matrix	= MATRIX& to copy form
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MATRIX::MATRIX(const MATRIX &Matrix)
{
	// Initialize the elements pointer
	Elements = NULL;

	// Copy from the argument to this
	*this = Matrix;
}

//==========================================================================
// Class:			MATRIX
// Function:		~MATRIX
//
// Description:		Destructor for the MATRIX class.
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
MATRIX::~MATRIX()
{
	// Delete the elements
	delete [] Elements;
	Elements = NULL;
}

//==========================================================================
// Class:			MATRIX
// Function:		SetElement
//
// Description:		Changes the element at the specified location to the
//					specified value.
//
// Input Argurments:
//		Row		= const int& specifying the vertical position in the matrix
//		Column	= const int& specifying the horizontal position in the matrix
//		Value	= const double& specifying the new value of the element
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MATRIX::SetElement(const int &Row, const int &Column, const double &Value)
{
	// Check to make sure it is a valid index
	assert(Row <= Rows && Column <= Columns);

	// Set the element as requested
	Elements[(Row - 1) * Columns + Column - 1] = Value;

	return;
}

//==========================================================================
// Class:			MATRIX
// Function:		Set
//
// Description:		Sets the values of this matrix to the arguments passed
//					to this function.
//
// Input Argurments:
//		Element1	= double representing the first element
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
void MATRIX::Set(double Element1, ...)
{
	// Declare a va_list macro and initialize it with va_start
	va_list ArgumentList;
	va_start(ArgumentList, Element1);

	// Assign the first element
	Elements[0] = Element1;

	// Fill all of the elements with the arguments
	// FIXME:  There is no check to make sure the correct number of elements was
	// passed!  This could result in runtime crash or wrong calculations!
	int i;
	for (i = 1; i < Rows * Columns; i++)
		Elements[i] = va_arg(ArgumentList, double);

	// Terminate the variable argument list
	va_end(ArgumentList);

	return;
}

//==========================================================================
// Class:			MATRIX
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
double MATRIX::GetElement(const int &Row, const int &Column) const
{
	// Return the element at the specified location
	return Elements[(Row - 1) * Columns + Column - 1];
}

//==========================================================================
// Class:			MATRIX
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
MATRIX& MATRIX::MakeIdentity(void)
{
	// Determine the smaller dimension
	int MinDimension;
	if (Rows < Columns)
		MinDimension = Rows;
	else
		MinDimension = Columns;

	// Set everything to zero
	Zero();

	// Make the diagonal elements 1.0
	int i;
	for (i = 1; i <= MinDimension; i++)
		SetElement(i, i, 1.0);

	return *this;
}

//==========================================================================
// Class:			MATRIX
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
void MATRIX::Zero(void)
{
	// Go through all of the elements setting everything to zero
	int i;
	for (i = 0; i < Rows * Columns; i++)
		Elements[i] = 0.0;

	return;
}

//==========================================================================
// Class:			MATRIX
// Function:		GetSubMatrix
//
// Description:		Returns a sub-matrix made up of the specified portion of
//					this matrix.
//
// Input Argurments:
//		StartRow	= const int& specifying the starting row
//		StartColumn	= const int& specifying the starting column
//		SubRows		= const int& specifying the number of rows
//		SubColumns	= const int& specifying the number of columns
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX contining the specified sub-matrix
//
//==========================================================================
MATRIX MATRIX::GetSubMatrix(const int &StartRow, const int &StartColumn,
							const int &SubRows, const int &SubColumns) const
{
	// Our return matrix
	MATRIX SubMatrix(SubRows, SubColumns);

	// Make sure this matrix is big enough to create the desired sub-matrix
	if (Rows >= StartRow + SubRows - 1 && Columns >= StartColumn + SubColumns - 1)
	{
		// Assign the values for the sub-matrix
		int i, j;
		for (i = 0; i < SubRows; i++)
		{
			for (j = 0; j < SubColumns; j++)
				SubMatrix.SetElement(i + 1, j + 1,
					GetElement(StartRow + i, StartColumn + j));
		}
	}

	return SubMatrix;
}

//==========================================================================
// Class:			MATRIX
// Function:		Transpose
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
//		MATRIX&, reference to this
//
//==========================================================================
MATRIX& MATRIX::Transpose(void)
{
	// Define a temporary array of our elements
	double *TempElements = new double[Rows * Columns];

	// Assign all of our elements to this temporary array
	int i;
	for (i = 0; i < Rows * Columns; i++)
		TempElements[i] = Elements[i];

	// Go across each row and down each column swapping the values
	int j;
	for (i = 1; i <= Rows; i++)
	{
		for (j = 1; j <= Columns; j++)
			Elements[(j - 1) * Rows + i - 1] =
				TempElements[(i - 1) * Columns + j - 1];
	}

	// Swap the number of rows and columns
	double OldRows = Rows;
	Rows = Columns;
	Columns = OldRows;

	// Delete the temporary array
	delete [] TempElements;
	TempElements = NULL;

	return *this;
}

// This method returns the vector x, as in the solution to the linear
// equation A * x = Vector.  Equivalent to MATLAB's left divide.
// This is robust to zeros at pivot locations (we swap rows as required)
// but an entire row or column of zeros (if the matrix doesn't have full
// rank) will still cause problems.
/*VECTOR MATRIX::UnderVector(VECTOR Vector)
{
	VECTOR Temp(NULL, NULL, NULL);
	double _Elements[9];
	int col, row;
	int PivotIndex;
	int tempRow;
	double Factor;
	double ElementToSwap;

	// If this isn't a 3x3 matrix we'll return a null vector
	if (Rows != 3 || Cols != 3)
		return Temp;

	// Copy the matrix elements (Elements) into local elements (_Elements)
	for (row = 0; row <= Rows * Cols - 1; row++)
		_Elements[row] = Elements[row];

	// Go row by row
	for (row = 1; row < Rows; row++)
	{
		PivotIndex = (row - 1) * (Cols + 1);
		// Check to see if the pivot location is zero (row = col here)
		if (_Elements[PivotIndex] == 0)
		{
			// Search for a non-zero element in the same column
			for (tempRow = row + 1; tempRow <= Rows; tempRow++)
			{
				// This goes down col to tempRow and checks the value
				if (_Elements[(tempRow -1) * Cols + row - 1] != 0)
					break;// Element in tempRow is non-zero!!
			}

			// Swap this row with TargetRow
			for (col = 1; col <= Cols; col++)
			{
				// ElementToSwap = the element at (tempRow, col)
				ElementToSwap = _Elements[(tempRow - 1) * Cols + col - 1];
				// The element at (tempRow, col) = the element at (row, col)
				_Elements[(tempRow - 1) * Cols + col - 1] = _Elements[(row - 1) * Cols + col - 1];
				// The element at (row, col) = ElementToSwap
				_Elements[(row - 1) * Cols + col - 1] = ElementToSwap;
			}

			// Swap the numerator vector as well
			if (row == 1 && tempRow == 2)
			{
				ElementToSwap = Target.X;
				Target.X = Target.Y;
				Target.Y = ElementToSwap;
			}
			else if (row == 1 && tempRow == 3)
			{
				ElementToSwap = Target.X;
				Target.X = Target.Z;
				Target.Z = ElementToSwap;
			}
			else if (row == 2)
			{
				ElementToSwap = Target.Y;
				Target.Y = Target.Z;
				Target.Z = ElementToSwap;
			}

		}

		// Do the elimination
		for (tempRow = row; tempRow < Rows; tempRow++)
		{
			// Factor is the number that multiplies the row we want to reduce (tempRow) so that it
			// adds to the pivot row (row) to make the element below the pivot zero
			// That is to say, the pivot row (row) doesn't change, but the rows beneath that (tempRow)
			// do change.
			Factor = -_Elements[(tempRow) * Cols + row - 1] / _Elements[PivotIndex];

			for (col = row; col <= Cols; col++)
				// the element at (tempRow, col) += Factor * the element at (row, col)
				_Elements[tempRow * Cols + col - 1] += Factor * _Elements[(row - 1) * Cols + col - 1];

			// Don't forget the numerator vector!
			if (row == 1)
			{
				if (tempRow == 1)
					Target.Y += Factor * Target.X;
				else if (tempRow == 2)
					Target.Z += Factor * Target.X;
			}
			else if (row == 2)
				Target.Z += Factor * Target.Y;
		}
	}

	// Solve the equations
	// The bottom row of the matrix, which now is a33 * Temp.Z = Target.Z
	Temp.Z = Target.Z / _Elements[8];
	// The middle row of the matrix, which now is a22 * Temp.Y + a23 * Temp.Z = Target.Y
	Temp.Y = (Target.Y - Temp.Z * _Elements[5]) / _Elements[4];
	// The top row of the matrix, now Temp.X is our only unknown
	Temp.X = (Target.X - Temp.Y * _Elements[1] - Temp.Z * _Elements[2]) / _Elements[0];

	return Temp;
}*/

//==========================================================================
// Class:			MATRIX
// Function:		operator *
//
// Description:		Multiplication operator for the MATRIX class.
//
// Input Argurments:
//		Vector	= const VECTOR& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const VECTOR result of the matrix multiplication
//
//==========================================================================
const VECTOR MATRIX::operator * (const VECTOR &Vector) const
{
	VECTOR Temp(0, 0, 0);

	// Check to make sure we're a 3x3 matrix
	if (Rows == 3 && Columns == 3)
	{
		Temp.X = Vector.X * Elements[0] + Vector.Y * Elements[1] + Vector.Z * Elements[2];
		Temp.Y = Vector.X * Elements[3] + Vector.Y * Elements[4] + Vector.Z * Elements[5];
		Temp.Z = Vector.X * Elements[6] + Vector.Y * Elements[7] + Vector.Z * Elements[8];
	}

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator *=
//
// Description:		Multiplication assignment operator for the MATRIX class.
//
// Input Argurments:
//		Matrix	= const MATRIX& to multiply
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX&
//
//==========================================================================
MATRIX& MATRIX::operator *= (const MATRIX &Matrix)
{
	// Make sure the inner dimensions match
	assert(Columns == Matrix.Rows);

	// Store the information contained in this matrix
	MATRIX Temp(Rows, Matrix.Columns);
	Temp = *this;

	// Update this matrix, first by re-allcoating the memory so this matrix is the right size
	Columns = Matrix.Columns;
	delete [] Elements;
	Elements = new double[Rows * Columns];

	// Now do the multiplication, storing the results in this matrix
	int counter, i, j;
	double tempElement;

	for (i = 1; i <= Rows; i++)
	{
		for (j = 1; j <= Matrix.Columns; j++)
		{
			tempElement = 0.0;
			// The element at location (Row, Col) is the sum of the products of the
			// elements of row Row of this (now it's Temp) with column j of Target
			for (counter = 1; counter <= Columns; counter++)
				tempElement += Temp.Elements[(i - 1) * Columns + counter - 1] *
					Matrix.GetElement(counter, j);
			SetElement(i, j, tempElement);
		}
	}
	
	return *this;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator =
//
// Description:		Assignment operator for the MATRIX class.
//
// Input Argurments:
//		Matrix	= const MATRIX& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& result of the assignment
//
//==========================================================================
MATRIX& MATRIX::operator = (const MATRIX &Matrix)
{
	// Check for self assignment
	if (this == &Matrix)
		return *this;

	// Make sure the row and column dimensions are correct
	Rows = Matrix.Rows;
	Columns = Matrix.Columns;

	// Make sure we're the right size by simply making us the right size
	delete [] Elements;
	Elements = new double[Rows * Columns];

	// Now assign the elements one by one
	int i;
	for (i = 0; i < Rows * Columns; i++)
		Elements[i] = Matrix.Elements[i];

	return *this;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator +=
//
// Description:		Addition assignment operator for the MATRIX class.
//
// Input Argurments:
//		Matrix	= const MATRIX& to add
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& result of the addition
//
//==========================================================================
MATRIX& MATRIX::operator += (const MATRIX &Matrix)
{
	// Make sure dimensions match
	assert(Columns == Matrix.Columns && Rows == Matrix.Rows);

	// Add Matrix to this element by element
	int i;
	for (i = 0; i < Rows * Columns; i++)
		Elements[i] += Matrix.Elements[i];

	return *this;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator -=
//
// Description:		Subrtaction assignment operator for the MATRIX class.
//
// Input Argurments:
//		Matrix	= const MATRIX& to subtract
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& result of the subtraction
//
//==========================================================================
MATRIX& MATRIX::operator -= (const MATRIX &Matrix)
{
	// Make sure dimensions match
	assert(Columns == Matrix.Columns && Rows == Matrix.Rows);

	// Subtract Matrix from this element by element
	int i;
	for (i = 0; i < Rows * Columns; i++)
		Elements[i] -= Matrix.Elements[i];

	return *this;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator *=
//
// Description:		Element-wise multiplication assignment operator for the
//					MATRIX class.
//
// Input Argurments:
//		Double	= const double& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& result of the multiplication
//
//==========================================================================
MATRIX& MATRIX::operator *=(const double &Double)
{
	int i;
	for (i = 0; i < Rows * Columns; i++)
		Elements[i] *= Double;

	return *this;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator /=
//
// Description:		Element-wise division assignment operator for the
//					MATRIX class.
//
// Input Argurments:
//		Double	= const double& to divide by
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX& result of the division
//
//==========================================================================
MATRIX& MATRIX::operator /=(const double &Double)
{
	int i;

	for (i = 0; i < Rows * Columns; i++)
		Elements[i] /= Double;

	return *this;
}

//==========================================================================
// Class:			MATRIX
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
wxString MATRIX::Print(void) const
{
	wxString Temp, Intermediate;

	// Go row-by-row
	int i, j;
	for (i = 1; i <= Rows; i++)
	{
		// Go col-by-col
		for (j = 1; j <= Columns; j++)
		{
			// Add the current element to the temporary string
			Intermediate.Printf("%7.3f\t", Elements[(i - 1) * Columns + j - 1]);
			Temp.Append(Intermediate);
		}

		if (i < Rows)
			Temp.Append(_T("\n"));
	}

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		RowReduce
//
// Description:		Performs row-reduction on this object until the matrix
//					is upper-triangular.  FIXME:  Will this work for matricies
//					with less than full rank?  What if the matrix is not square?
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
MATRIX& MATRIX::RowReduce(void)
{
	int PivotIndex;
	int tempRow;
	double Factor;
	double ElementToSwap;

	// Go row by row
	int i, j;
	for (i = 1; i < Rows; i++)
	{
		PivotIndex = (i - 1) * (Columns + 1);
		// Check to see if the pivot location is zero (row = col here)
		if (Elements[PivotIndex] == 0)
		{
			// Search for a non-zero element in the same column
			for (tempRow = i + 1; tempRow <= Rows; tempRow++)
			{
				// This goes down col to tempRow and checks the value
				if (Elements[(tempRow -1) * Columns + i - 1] != 0)
					break;// Element in tempRow is non-zero!!
			}

			// Swap this row with TargetRow
			for (j = 1; j <= Columns; j++)
			{
				// ElementToSwap = the element at (tempRow, j)
				ElementToSwap = Elements[(tempRow - 1) * Columns + j - 1];
				// The element at (tempRow, j) = the element at (row, col)
				Elements[(tempRow - 1) * Columns + j - 1] = Elements[(i - 1) * Columns + j - 1];
				// The element at (i, j) = ElementToSwap
				Elements[(i - 1) * Columns + j - 1] = ElementToSwap;
			}
		}

		// Do the elimination
		for (tempRow = i; tempRow < Rows; tempRow++)
		{
			// Factor is the number that multiplies the row we want to reduce (tempRow) so that it
			// adds to the pivot row (i) to make the element below the pivot zero
			// That is to say, the pivot row (i) doesn't change, but the rows beneath that (tempRow)
			// do change.
			Factor = -Elements[(tempRow) * Columns + i - 1] / Elements[PivotIndex];

			for (j = i; j <= Columns; j++)
				// the element at (tempRow, col) += Factor * the element at (i, j)
				Elements[tempRow * Columns + j - 1] += Factor * Elements[(i - 1) * Columns + j - 1];
		}
	}

	return *this;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator +
//
// Description:		Addition operator for the MATRIX class.
//
// Input Argurments:
//		Matrix	= MATRIX to add
//
// Output Arguments:
//		None
//
// Return Value:
//		MATRIX contining the result of the addition
//
//==========================================================================
const MATRIX MATRIX::operator + (const MATRIX &Matrix) const
{
	// Create the return matrix
	MATRIX Temp(Matrix.Rows, Matrix.Columns);
	Temp = *this;

	// Do the addition
	Temp += Matrix;

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator -
//
// Description:		Subtraction operator for the MATRIX class.
//
// Input Argurments:
//		Matrix	= const MATRIX& to subtract
//
// Output Arguments:
//		None
//
// Return Value:
//		const MATRIX contining the result of the subtraction
//
//==========================================================================
const MATRIX MATRIX::operator - (const MATRIX &Matrix) const
{
	// Create the return matrix
	MATRIX Temp(Matrix.Rows, Matrix.Columns);
	Temp = *this;

	// Do the subtraction
	Temp -= Matrix;

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator *
//
// Description:		Multiplication operator for the MATRIX class.
//
// Input Argurments:
//		Target	= const MATRIX& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const MATRIX contining the result of the multiplication
//
//==========================================================================
const MATRIX MATRIX::operator * (const MATRIX &Matrix) const
{
	// Create the return matrix
	MATRIX Temp(Matrix.Rows, Matrix.Columns);
	Temp = *this;

	// Do the multiplication
	Temp *= Matrix;

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator *
//
// Description:		Element-wise multiplication operator for the MATRIX class.
//
// Input Argurments:
//		Double	= const double& to multiply by
//
// Output Arguments:
//		None
//
// Return Value:
//		const MATRIX contining the result of the multiplication
//
//==========================================================================
const MATRIX MATRIX::operator * (const double &Double) const
{
	// Create the return matrix
	MATRIX Temp(Rows, Columns);
	Temp = *this;

	// Do the multiplication
	Temp *= Double;

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator /
//
// Description:		Element-wise division operator for the MATRIX class.
//
// Input Argurments:
//		Double	= const double& to divide by
//
// Output Arguments:
//		None
//
// Return Value:
//		const MATRIX contining the result of the division
//
//==========================================================================
const MATRIX MATRIX::operator / (const double &Double) const
{
	// Create the return matrix
	MATRIX Temp(Rows, Columns);
	Temp = *this;

	// Do the division
	Temp /= Double;

	return Temp;
}

//==========================================================================
// Class:			MATRIX
// Function:		operator ()
//
// Description:		Overload of the () operator for this object.  Permits accessing
//					class data by using MATRIX(Row, Column).  Non-const version.
//
// Input Argurments:
//		Row		= const int& specifying the row of the desired element (0-based)
//		Column	= const int& specifying the column of the desired element (0-based)
//
// Output Arguments:
//		None
//
// Return Value:
//		double&, reference to the specified element
//
//==========================================================================
double &MATRIX::operator () (const int &Row, const int &Column)
{
	// Make sure the indecies are valid
	assert(Row >= 0 && Row < Rows && Column >= 0 && Column < Columns);

	// Return the specified element
	return Elements[(Row - 1) * Columns + Column - 1];
}

//==========================================================================
// Class:			MATRIX
// Function:		operator ()
//
// Description:		Overload of the () operator for this object.  Permits accessing
//					class data by using MATRIX(Row, Column).  Const version.
//
// Input Argurments:
//		Row		= const int& specifying the row of the desired element (0-based)
//		Column	= const int& specifying the column of the desired element (0-based)
//
// Output Arguments:
//		None
//
// Return Value:
//		const double&, reference to the specified element
//
//==========================================================================
const double &MATRIX::operator () (const int &Row, const int &Column) const
{
	// Make sure the indecies are valid
	assert(Row >= 0 && Row < Rows && Column >= 0 && Column < Columns);

	// Return the specified element
	return Elements[(Row - 1) * Columns + Column - 1];
}