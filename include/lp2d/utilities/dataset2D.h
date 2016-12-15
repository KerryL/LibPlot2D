/*=============================================================================
                                    DataPlotter
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

class Dataset2D
{
public:
	Dataset2D() = default;
	explicit Dataset2D(const unsigned int &numberOfPoints);

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString pathAndFileName) const;

	void Resize(const unsigned int &numberOfPoints);
	void Reverse();

	double ComputeYMean() const;
	double GetAverageDeltaX() const;

	unsigned int GetNumberOfPoints() const { return xData.size(); }
	unsigned int GetNumberOfZoomedPoints(const double &min, const double &max) const;

	const std::vector<double>& GetX() const { return xData; };
	const std::vector<double>& GetY() const { return yData; };
	std::vector<double>& GetX() { return xData; };
	std::vector<double>& GetY() { return yData; };

	Dataset2D& MultiplyXData(const double &target);
	bool GetYAt(const double &x, double &y, bool *exactValue = nullptr) const;// TODO:  Get rid of this (only used in one place in MainFrame::UpdateCursorValues)

	Dataset2D& XShift(const double &shift);

	// Overloaded operators
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
	std::vector<double> xData, yData;

	static void GetOverlappingOnSameTimebase(const Dataset2D &d1,
		const Dataset2D &d2, Dataset2D &d1Out, Dataset2D &d2Out);
};

}// namespace LibPlot2D

#endif// DATASET_H_
