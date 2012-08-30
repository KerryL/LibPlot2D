/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataset2D.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Container for x and y-data series for plotting.
// History:

#ifndef _DATASET_H_
#define _DATASET_H_

// Standard C++ headers
#include <assert.h>

// wxWidgets forward declarations
class wxString;

class Dataset2D
{
public:
	// Constructors
	Dataset2D();
	Dataset2D(const Dataset2D& target);
	Dataset2D(const unsigned int &_numberOfPoints);

	// Destructor
	~Dataset2D();

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString pathAndFileName) const;

	void Resize(const unsigned int &_numberOfPoints);
	void Reverse(void);

	unsigned int GetNumberOfPoints(void) const { return numberOfPoints; };
	unsigned int GetNumberOfZoomedPoints(const double &min, const double &max) const;
	double *GetXPointer(void) { return xData; };
	double *GetYPointer(void) { return yData; };
	const double *GetXPointer(void) const { return xData; };
	const double *GetYPointer(void) const { return yData; };
	double GetXData(const unsigned int &i) const { assert(i < numberOfPoints); return xData[i]; };
	double GetYData(const unsigned int &i) const { assert(i < numberOfPoints); return yData[i]; };

	Dataset2D& MultiplyXData(const double &target);
	bool GetYAt(double &x) const;

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
	Dataset2D& DoLog(void);
	Dataset2D& DoLog10(void);
	Dataset2D& DoExp(void);
	Dataset2D& DoAbs(void);

	const Dataset2D ToPower(const double &target) const;
	const Dataset2D ToPower(const Dataset2D &target) const;
	const Dataset2D ApplyPower(const double &target) const;
	const Dataset2D DoLog(void) const;
	const Dataset2D DoLog10(void) const;
	const Dataset2D DoExp(void) const;
	const Dataset2D DoAbs(void) const;

private:
	// The number of points contained within this object
	unsigned int numberOfPoints;

	// The data
	double *xData, *yData;
};

#endif// _DATASET_H_