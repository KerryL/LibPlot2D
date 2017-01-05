/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  dataset2D.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Container for x and y-data series for plotting.

#ifndef DATASET_H_
#define DATASET_H_

// Standard C++ headers
#include <cassert>
#include <cstdlib>
#include <vector>
#include <memory>

// wxWidgets forward declarations
class wxString;

namespace LibPlot2D
{

/// Class for representing paired x-y data.
class Dataset2D
{
public:
	Dataset2D() = default;

	/// Constructor.
	///
	/// \param numberOfPoints Initial size of the buffers.
	explicit Dataset2D(const unsigned int &numberOfPoints);

	/// Exports the contents of the object to the specified file.
	///
	/// \param pathAndFileName File to write.
	void ExportDataToFile(wxString pathAndFileName) const;

	/// Resizes the internal buffers to hold the specified number of points.
	///
	/// \param numberOfPoints New size of internal buffers.
	void Resize(const unsigned int &numberOfPoints);

	/// Reverses the order of the data stored in this object.
	void Reverse();

	/// Computes the mean of the y-data.
	/// \returns The mean of the y-data.
	double ComputeYMean() const;

	/// Computes the average of the delta between adjacent x-data.
	/// \returns The average of the delta between adjacent x-data.
	double GetAverageDeltaX() const;

	/// Gets the number of points stored in this object.
	/// \returns The number of points stored in this object.
	unsigned int GetNumberOfPoints() const { return mXData.size(); }

	/// Gets the number of points within the specified range.
	///
	/// \param min Minimum end of the x-range.
	/// \param max Maximum end of the x-range.
	///
	/// \returns The number of points within the specified range.
	unsigned int GetNumberOfZoomedPoints(const double &min, const double &max) const;

	/// Creates a new data set equivalent to this one, but with the x-data
	/// multiplied by the specified factor.
	///
	/// \param target Value to multiply with each x-value.
	///
	/// \returns A new data set with multiplied x-data.
	Dataset2D& MultiplyXData(const double &target);

	/// Gets the y-value at the specified x-value.
	///
	/// \param x          	    X-value at which the y-value is desired.
	/// \param y [out]          Y-value corresponding to the specified \p x.
	/// \param exactValue [out] Set to indicate whether or not the returned
	///                         y-value was interpolated, or if it represents
	///                         an exact value that is present in the raw data.
	///
	/// \returns The new
	bool GetYAt(const double &x, double &y, bool *exactValue = nullptr) const;// TODO:  Get rid of this (only used in one place in MainFrame::UpdateCursorValues)

	/// Creates a new data set equivalent to this one, but with the x-data
	/// shifted by the specified amount.
	///
	/// \param shift Value to add to each x-value.
	///
	/// \returns A new data set with shifted x-data.
	Dataset2D& XShift(const double &shift);

	/// \name Private data accessors
	/// @{

	const std::vector<double>& GetX() const { return mXData; };
	const std::vector<double>& GetY() const { return mYData; };
	std::vector<double>& GetX() { return mXData; };
	std::vector<double>& GetY() { return mYData; };

	/// @}

	/// \name Overloaded operators
	/// @{

	Dataset2D& operator+=(const Dataset2D &target);
	Dataset2D& operator-=(const Dataset2D &target);
	Dataset2D& operator*=(const Dataset2D &target);
	Dataset2D& operator/=(const Dataset2D &target);

	const Dataset2D operator+(const Dataset2D &target) const;
	const Dataset2D operator-(const Dataset2D &target) const;
	const Dataset2D operator*(const Dataset2D &target) const;
	const Dataset2D operator/(const Dataset2D &target) const;

	Dataset2D& operator+=(const double &target);
	Dataset2D& operator-=(const double &target);
	Dataset2D& operator*=(const double &target);
	Dataset2D& operator/=(const double &target);

	const Dataset2D operator+(const double &target) const;
	const Dataset2D operator-(const double &target) const;
	const Dataset2D operator*(const double &target) const;
	const Dataset2D operator/(const double &target) const;
	const Dataset2D operator%(const double &target) const;

	/// @}

	/// \name Methods for applying math functions.
	/// Functions are applied to each element of the data set.
	/// @{

	Dataset2D& ToPower(const double &target);
	Dataset2D& ToPower(const Dataset2D &target);
	Dataset2D& ApplyPower(const double &target);
	Dataset2D& DoLog();
	Dataset2D& DoLog10();
	Dataset2D& DoExp();
	Dataset2D& DoAbs();
	Dataset2D& DoSin();
	Dataset2D& DoCos();
	Dataset2D& DoTan();
	Dataset2D& DoArcSin();
	Dataset2D& DoArcCos();
	Dataset2D& DoArcTan();

	const Dataset2D ToPower(const double &target) const;
	const Dataset2D ToPower(const Dataset2D &target) const;
	const Dataset2D ApplyPower(const double &target) const;
	const Dataset2D DoLog() const;
	const Dataset2D DoLog10() const;
	const Dataset2D DoExp() const;
	const Dataset2D DoAbs() const;
	const Dataset2D DoSin() const;
	const Dataset2D DoCos() const;
	const Dataset2D DoTan() const;
	const Dataset2D DoArcSin() const;
	const Dataset2D DoArcCos() const;
	const Dataset2D DoArcTan() const;

	/// @}

	/// \name Methods for performing arithmetic between two data sets.
	/// These methods handle data sets which have inconsistent x-data.
	/// @{

	static Dataset2D DoUnsyncrhonizedAdd(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedSubtract(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedMultiply(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedDivide(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedExponentiation(const Dataset2D &d1, const Dataset2D &d2);

	/// @}

private:
	std::vector<double> mXData, mYData;

	static void GetOverlappingOnSameTimebase(const Dataset2D &d1,
		const Dataset2D &d2, Dataset2D &d1Out, Dataset2D &d2Out);
};

}// namespace LibPlot2D

#endif// DATASET_H_
