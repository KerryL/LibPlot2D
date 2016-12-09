/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  dataset2D.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Container for x and y-data series for plotting.
// History:

#ifndef DATASET_H_
#define DATASET_H_

// Standard C++ headers
#include <cassert>
#include <cstdlib>

// wxWidgets forward declarations
class wxString;

class Dataset2D
{
public:
	Dataset2D();
	Dataset2D(const Dataset2D& target);
	Dataset2D(const unsigned int &numberOfPoints);

	~Dataset2D();

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString pathAndFileName) const;

	void Resize(const unsigned int &numberOfPoints);
	void Reverse();

	double ComputeYMean() const;
	double GetAverageDeltaX() const;

	unsigned int GetNumberOfPoints() const { return numberOfPoints; }
	unsigned int GetNumberOfZoomedPoints(const double &min, const double &max) const;
	double *GetXPointer() { return xData; }
	double *GetYPointer() { return yData; }
	const double *GetXPointer() const { return xData; }
	const double *GetYPointer() const { return yData; }
	double GetXData(const unsigned int &i) const { assert(i < numberOfPoints); return xData[i]; }
	double GetYData(const unsigned int &i) const { assert(i < numberOfPoints); return yData[i]; }

	Dataset2D& MultiplyXData(const double &target);
	bool GetYAt(const double &x, double &y, bool *exactValue = NULL) const;// TODO:  Get rid of this (only used in one place in MainFrame::UpdateCursorValues)

	Dataset2D& XShift(const double &shift);

	// Overloaded operators
	Dataset2D& operator=(const Dataset2D &target);

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

	static Dataset2D DoUnsyncrhonizedAdd(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedSubtract(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedMultiply(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedDivide(const Dataset2D &d1, const Dataset2D &d2);
	static Dataset2D DoUnsyncrhonizedExponentiation(const Dataset2D &d1, const Dataset2D &d2);

private:
	unsigned int numberOfPoints;
	double *xData, *yData;

	static void GetOverlappingOnSameTimebase(const Dataset2D &d1,
		const Dataset2D &d2, Dataset2D &d1Out, Dataset2D &d2Out);
};

#endif// DATASET_H_