/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  matrix.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Contains class declaration for Matrix classes.

#ifndef MATRIX_H_
#define MATRIX_H_

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

// Local forward declarations
class Vector;

class Matrix
{
public:
	// Constructors
	Matrix();
	Matrix(const unsigned int &rows, const unsigned int &columns);
	Matrix(const unsigned int &rows, const unsigned int &columns, double element1, ...);
	Matrix(const Matrix &m);

	// Destructor
	~Matrix();

	// Sets the values of all of the elements
	void Set(double Element1, ...);

	// Make all elements zero
	void Zero();

	// Makes this matrix an identity matrix
	Matrix& MakeIdentity();
	static Matrix GetIdentity(const unsigned int &rows, const unsigned int &columns = 0);

	bool IsSquare() const { return rows == columns; }
	void Resize(const unsigned int &rows, const unsigned int &columns);
	Matrix& RemoveRow(const unsigned int &row);
	Matrix& RemoveColumn(const unsigned int &column);

	unsigned int GetMinimumDimension() const { return (rows < columns) ? rows : columns; }

	// Returns a matrix containing a sub-set of the contents of this matrix
	Matrix GetSubMatrix(const unsigned int &startRow, const unsigned int &startColumn,
		const unsigned int &subRows, const unsigned int &subColumns) const;

	// Retrieve properties of this matrix
	inline unsigned int GetNumberOfRows() const { return rows; }
	inline unsigned int GetNumberOfColumns() const { return columns; }
	double GetElement(const int &row, const int &column) const;

	// Set value of element at the specified location
	void SetElement(const unsigned int &row, const unsigned int &column, const double &value);

	// Print this object to a string
	wxString Print() const;

	// Operators
	Matrix& operator += (const Matrix &m);
	Matrix& operator -= (const Matrix &m);
	Matrix& operator *= (const Matrix &m);
	Matrix& operator *= (const double &n);
	Matrix& operator /= (const double &n);
	Matrix& operator = (const Matrix &m);
	double &operator () (const unsigned int &row, const unsigned int &column);
	const Matrix operator + (const Matrix &m) const;
	const Matrix operator - (const Matrix &m) const;
	const Matrix operator * (const Matrix &m) const;
	const Matrix operator * (const double &n) const;
	const Vector operator * (const Vector &v) const;
	const Matrix operator / (const double &target) const;
	const double &operator () (const unsigned int &row, const unsigned int &column) const;

	// Common matrix operations ------------------------------------
	bool GetSingularValueDecomposition(Matrix &U, Matrix &V, Matrix &W) const;

	Matrix GetTranspose() const;
	Matrix GetInverse() const;
	Matrix GetPsuedoInverse() const;
	Matrix GetDiagonalInverse() const;

	Matrix& Transpose();

	bool LeftDivide(const Matrix& b, Matrix &x) const;// x = A \ b
	Matrix GetRowReduced() const;
	unsigned int GetRank() const;

private:
	unsigned int rows;
	unsigned int columns;

	// The array of elements of this matrix
	double **elements;

	void FreeElements();
	void AllocateElements();

	// Helper functions for SVD algorithm
	double Pythag(const double& a, const double &b) const;
	void InitializeSVDMatrices(Matrix &U, Matrix &V, Matrix &W) const;
	double ReduceToBidiagonalForm(Matrix &U, Matrix &V, Matrix &W, double *rv1) const;
	void AccumulateRightHandTransforms(Matrix &U, Matrix &V, const double *rv1) const;
	void AccumulateLeftHandTransforms(Matrix &U, Matrix &V, Matrix &W) const;
	bool DiagonalizeBidiagonalForm(Matrix &U, Matrix &V, Matrix &W, double *rv1, const double &anorm) const;
	void RemoveZeroSingularValues(Matrix &U, Matrix &W) const;
	void SortSingularValues(Matrix &U, Matrix &V, Matrix &W) const;

	Matrix& SwapRows(const unsigned int &r1, const unsigned int &r2);

	// Helper function for row reduction
	void ZeroRowByScalingAndAdding(const unsigned int &pivotRow,
		const unsigned int &pivotColumn, const unsigned int &targetRow);
};

}// namespace LibPlot2D

#endif// MATRIX_H_
