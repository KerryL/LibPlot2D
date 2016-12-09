/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  dataset2D.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Container for x and y-data series for plotting.
// History:

// Standard C++ headers
#include <fstream>
#include <utility>
#include <cassert>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//==========================================================================
// Class:			Dataset2D
// Function:		Dataset2D
//
// Description:		Constructor for the Dataset class.
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
Dataset2D::Dataset2D()
{
	// Initialize everything to zero size
	numberOfPoints = 0;
	xData = NULL;
	yData = NULL;
}

//==========================================================================
// Class:			Dataset2D
// Function:		Dataset2D
//
// Description:		Constructor for the Dataset class.
//
// Input Arguments:
//		target	= const Dataset2D& top copy into this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Dataset2D::Dataset2D(const Dataset2D& target)
{
	xData = NULL;
	yData = NULL;
	*this = target;
}

//==========================================================================
// Class:			Dataset2D
// Function:		Dataset2D
//
// Description:		Constructor for the Dataset class.
//
// Input Arguments:
//		numberOfPoints = const unsigned int &
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Dataset2D::Dataset2D(const unsigned int &numberOfPoints)
{
	xData = NULL;
	yData = NULL;
	Resize(numberOfPoints);
}

//==========================================================================
// Class:			Dataset2D
// Function:		~Dataset2D
//
// Description:		Destructor for the Dataset class.
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
Dataset2D::~Dataset2D()
{
	delete [] xData;
	xData = NULL;

	delete [] yData;
	yData = NULL;
}

//==========================================================================
// Class:			Dataset2D
// Function:		Reverse
//
// Description:		Reverses the order of the Y-data.  X-data remains unchanged.
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
void Dataset2D::Reverse()
{
	double *temp = new double[numberOfPoints];
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		temp[i] = yData[i];
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = temp[numberOfPoints - 1 - i];
}

//==========================================================================
// Class:			Dataset2D
// Function:		Resize
//
// Description:		Resizes the arrays.  Deletes all existing data before
//					resizing.
//
// Input Arguments:
//		numberOfPoints = const unsigned int &
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Dataset2D::Resize(const unsigned int &numberOfPoints)
{
	delete [] xData;
	delete [] yData;

	this->numberOfPoints = numberOfPoints;

	xData = new double[numberOfPoints];
	yData = new double[numberOfPoints];
}

//==========================================================================
// Class:			Dataset2D
// Function:		ExportDataToFile
//
// Description:		Exports the data for this object to a comma or tab-delimited
//					text file.
//
// Input Arguments:
//		pathAndFileName	= wxString pointing to the location where the file is
//						  to be saved
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Dataset2D::ExportDataToFile(wxString pathAndFileName) const
{
	wxString extension(pathAndFileName.substr(pathAndFileName.find_last_of('.') + 1));
	wxChar delimiter;
	if (extension.Cmp(_T("txt")) == 0)
		delimiter = '\t';// Tab delimited
	else if (extension.Cmp(_T("csv")) == 0)
		delimiter = ',';// Comma separated values
	else
		return;

	std::ofstream exportFile(pathAndFileName.mb_str(), std::ios::out);
	if (!exportFile.is_open() || !exportFile.good())
		return;

	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		exportFile << xData[i] << delimiter << yData[i] << std::endl;

	exportFile.close();
}

//==========================================================================
// Class:			Dataset2D
// Function:		GetYAt
//
// Description:		Retrieves the Y-value at the specified X-value.  Interpolates
//					if the X-value is not exactly on a point.  Returns true if
//					it interpolated.
//
// Input Arguments:
//		x	= const double& specifying the X-value
//
// Output Arguments:
//		y	= double& specifying the Y-value
//		exactValue	= bool* indicating whether or not the exact value is being returned
//
// Return Value:
//		true if specified x is within range of data, false otherwise
//
//==========================================================================
bool Dataset2D::GetYAt(const double &x, double &y, bool *exactValue) const
{
	// This assumes data is entered from small x to large x and that y is a function of x
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
	{
		if (xData[i] == x)
		{
			y = yData[i];

			if (exactValue)
				*exactValue = true;

			return true;
		}
		else if (xData[i] > x)
		{
			if (i > 0)
				y = yData[i - 1] + (yData[i] - yData[i - 1]) * (x - xData[i - 1]) / (xData[i] - xData[i - 1]);
			else
				y = yData[i];

			if (exactValue)
				*exactValue = false;

			return true;
		}
	}

	return false;
}

//==========================================================================
// Class:			Dataset2D
// Function:		XShift
//
// Description:		Shifts the data's time series by the specified amount.
//
// Input Arguments:
//		shift	= const double& to add to this object's X-data
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this object
//
//==========================================================================
Dataset2D& Dataset2D::XShift(const double &shift)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		xData[i] += shift;

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator=
//
// Description:		Overloaded assignment operator.
//
// Input Arguments:
//		target	= const Dataset2D& to assign to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this object
//
//==========================================================================
Dataset2D& Dataset2D::operator=(const Dataset2D &target)
{
	// Check for self assignment
	if (this == &target)
		return *this;

	Resize(target.numberOfPoints);

	// Do the copy
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
	{
		xData[i] = target.xData[i];
		yData[i] = target.yData[i];
	}

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator+=
//
// Description:		Overloaded operator (+=).
//
// Input Arguments:
//		target	= const Dataset2D& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this object
//
//==========================================================================
Dataset2D& Dataset2D::operator+=(const Dataset2D &target)
{
	assert(numberOfPoints == target.numberOfPoints);

	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] += target.yData[i];

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator-=
//
// Description:		Overloaded operator (-=).
//
// Input Arguments:
//		target	= const Dataset2D& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this object
//
//==========================================================================
Dataset2D& Dataset2D::operator-=(const Dataset2D &target)
{
	assert(numberOfPoints == target.numberOfPoints);

	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] -= target.yData[i];

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator*=
//
// Description:		Overloaded operator (*=).
//
// Input Arguments:
//		target	= const Dataset2D& to multiply by this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this object
//
//==========================================================================
Dataset2D& Dataset2D::operator*=(const Dataset2D &target)
{
	assert(numberOfPoints == target.numberOfPoints);

	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] *= target.yData[i];

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator/=
//
// Description:		Overloaded operator (/=).
//
// Input Arguments:
//		target	= const Dataset2D& divide into this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this object
//
//==========================================================================
Dataset2D& Dataset2D::operator/=(const Dataset2D &target)
{
	assert(numberOfPoints == target.numberOfPoints);

	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] /= target.yData[i];

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator+
//
// Description:		Overloaded operator (+).
//
// Input Arguments:
//		target	= const Dataset2D& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired sum
//
//==========================================================================
const Dataset2D Dataset2D::operator+(const Dataset2D &target) const
{
	Dataset2D result = *this;
	result += target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator-
//
// Description:		Overloaded operator (-).
//
// Input Arguments:
//		target	= const Dataset2D& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired difference
//
//==========================================================================
const Dataset2D Dataset2D::operator-(const Dataset2D &target) const
{
	Dataset2D result = *this;
	result -= target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator*
//
// Description:		Overloaded operator (*).
//
// Input Arguments:
//		target	= const Dataset2D& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired product
//
//==========================================================================
const Dataset2D Dataset2D::operator*(const Dataset2D &target) const
{
	Dataset2D result = *this;
	result *= target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator/
//
// Description:		Overloaded operator (/).
//
// Input Arguments:
//		target	= const Dataset2D& to divide into this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired ratio
//
//==========================================================================
const Dataset2D Dataset2D::operator/(const Dataset2D &target) const
{
	Dataset2D result = *this;
	result /= target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator+=
//
// Description:		Overloaded operator (+=).
//
// Input Arguments:
//		target	= const double& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::operator+=(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] += target;

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator-=
//
// Description:		Overloaded operator (-=).
//
// Input Arguments:
//		target	= const double& to subract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::operator-=(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] -= target;

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator*=
//
// Description:		Overloaded operator (*=).
//
// Input Arguments:
//		target	= const double& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::operator*=(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] *= target;

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator/=
//
// Description:		Overloaded operator (/=).
//
// Input Arguments:
//		target	= const double& to divide into this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::operator/=(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] /= target;

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator+
//
// Description:		Overloaded operator (+).
//
// Input Arguments:
//		target	= const double& to add to this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired sum
//
//==========================================================================
const Dataset2D Dataset2D::operator+(const double &target) const
{
	Dataset2D result(*this);
	result += target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator-
//
// Description:		Overloaded operator (-).
//
// Input Arguments:
//		target	= const double& to subtract from this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired difference
//
//==========================================================================
const Dataset2D Dataset2D::operator-(const double &target) const
{
	Dataset2D result(*this);
	result -= target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator*
//
// Description:		Overloaded operator (*).
//
// Input Arguments:
//		target	= const double& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired product
//
//==========================================================================
const Dataset2D Dataset2D::operator*(const double &target) const
{
	Dataset2D result(*this);
	result *= target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator/
//
// Description:		Overloaded operator (/).
//
// Input Arguments:
//		target	= const double& to divide into this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired ratio
//
//==========================================================================
const Dataset2D Dataset2D::operator/(const double &target) const
{
	Dataset2D result(*this);
	result /= target;

	return result;
}

//==========================================================================
// Class:			Dataset2D
// Function:		operator%
//
// Description:		Overloaded operator (%).
//
// Input Arguments:
//		target	= const double& to divide into this
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D& containing desired ratio
//
//==========================================================================
const Dataset2D Dataset2D::operator%(const double &target) const
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = fmod(yData[i], target);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		MultiplyXData
//
// Description:		Multiplies specified value with X vector.
//
// Input Arguments:
//		target	= const double& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::MultiplyXData(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		xData[i] *= target;

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		ToPower
//
// Description:		Raises each element to the specified power.
//
// Input Arguments:
//		target	= const double& indicating the power
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::ToPower(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = pow(yData[i], target);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		ToPower
//
// Description:		Element-wise application of ToPower method.
//
// Input Arguments:
//		target	= const Dataset2D& indicating the power
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
Dataset2D& Dataset2D::ToPower(const Dataset2D &target)
{
	assert(numberOfPoints == target.numberOfPoints);

	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = pow(yData[i], target.GetYData(i));
	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		ToPower
//
// Description:		Element-wise application of ToPower method.
//
// Input Arguments:
//		target	= const Dataset2D& indicating the power
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::ToPower(const Dataset2D &target) const
{
	Dataset2D result(*this);
	return result.ToPower(target);
}

//==========================================================================
// Class:			Dataset2D
// Function:		MultiplyXData
//
// Description:		Multiplies specified value with X vector.
//
// Input Arguments:
//		target	= const double& to multiply with this
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D& reference to this
//
//==========================================================================
const Dataset2D Dataset2D::ToPower(const double &target) const
{
	Dataset2D result(*this);
	return result.ToPower(target);;
}

//==========================================================================
// Class:			Dataset2D
// Function:		GetNumberOfZoomedPoints
//
// Description:		Returns the number of data points within the zoomed area.
//					Assumes that the x-data is increasing only.
//
// Input Arguments:
//		min	= const double&
//		max	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int Dataset2D::GetNumberOfZoomedPoints(const double &min, const double &max) const
{
	unsigned int start(0), end(0);
	while (start < numberOfPoints && xData[start] < min)
		start++;
	end = start;
	while (end < numberOfPoints && xData[end] < max)
		end++;

	return end - start;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoLog
//
// Description:		Applies the log function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoLog()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = log(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoLog10
//
// Description:		Applies the log10 function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoLog10()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = log10(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoExp
//
// Description:		Applies the exp function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoExp()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = exp(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoAbs
//
// Description:		Applies the abs function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoAbs()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = abs(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoSin
//
// Description:		Applies the sin function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoSin()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = sin(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoCos
//
// Description:		Applies the cos function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoCos()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = cos(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoTan
//
// Description:		Applies the tan function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoTan()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = tan(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoArcSin
//
// Description:		Applies the asin function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoArcSin()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = asin(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoArcCos
//
// Description:		Applies the acos function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoArcCos()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = acos(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoArcTan
//
// Description:		Applies the atan function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::DoArcTan()
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = atan(yData[i]);

	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoLog
//
// Description:		Applies the log function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoLog() const
{
	Dataset2D result(*this);
	return result.DoLog();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoLog10
//
// Description:		Applies the log10 function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoLog10() const
{
	Dataset2D result(*this);
	return result.DoLog10();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoExp
//
// Description:		Applies the exp function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoExp() const
{
	Dataset2D result(*this);
	return result.DoExp();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoAbs
//
// Description:		Applies the abs function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoAbs() const
{
	Dataset2D result(*this);
	return result.DoAbs();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoSin
//
// Description:		Applies the sin function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoSin() const
{
	Dataset2D result(*this);
	return result.DoSin();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoCos
//
// Description:		Applies the cos function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoCos() const
{
	Dataset2D result(*this);
	return result.DoCos();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoTan
//
// Description:		Applies the tan function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoTan() const
{
	Dataset2D result(*this);
	return result.DoTan();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoArcSin
//
// Description:		Applies the asin function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoArcSin() const
{
	Dataset2D result(*this);
	return result.DoArcSin();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoArcCos
//
// Description:		Applies the acos function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoArcCos() const
{
	Dataset2D result(*this);
	return result.DoArcCos();
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoArcTan
//
// Description:		Applies the atan function to each Y-value in the dataset.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D
//
//==========================================================================
const Dataset2D Dataset2D::DoArcTan() const
{
	Dataset2D result(*this);
	return result.DoArcTan();
}

//==========================================================================
// Class:			Dataset2D
// Function:		ApplyPower
//
// Description:		Raises the specified value to the power equal to the y-value
//					of the dataset.
//
// Input Arguments:
//		target	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D&
//
//==========================================================================
Dataset2D& Dataset2D::ApplyPower(const double &target)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = pow(target, yData[i]);
	return *this;
}

//==========================================================================
// Class:			Dataset2D
// Function:		ApplyPower
//
// Description:		Raises the specified value to the power equal to the y-value
//					of the dataset.
//
// Input Arguments:
//		target	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		const Dataset2D&
//
//==========================================================================
const Dataset2D Dataset2D::ApplyPower(const double &target) const
{
	Dataset2D result(*this);
	return result.ApplyPower(target);
}

//==========================================================================
// Class:			Dataset2D
// Function:		ComputeYMean
//
// Description:		Computes the average of the Y-data.
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
double Dataset2D::ComputeYMean() const
{
	double sum(0.0);
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		sum += yData[i];

	return sum / (double)numberOfPoints;
}

//==========================================================================
// Class:			Dataset2D
// Function:		GetAverageDeltaX
//
// Description:		Computes the average spacing of the X-values.
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
double Dataset2D::GetAverageDeltaX() const
{
	double sum(0.0);
	unsigned int i;
	for (i = 1; i < numberOfPoints; i++)
		sum += xData[i] - xData[i - 1];

	return sum / ((double)numberOfPoints - 1.0);
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoUnsyncrhonizedAdd
//
// Description:		Returns a datset representing the sum of the arguments,
//					but only over the range where the arguments inersect.
//
// Input Arguments:
//		d1	= const Dataset2D&
//		d2	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D Dataset2D::DoUnsyncrhonizedAdd(const Dataset2D &d1, const Dataset2D &d2)
{
	Dataset2D common1, common2;
	GetOverlappingOnSameTimebase(d1, d2, common1, common2);
	return common1 + common2;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoUnsyncrhonizedSubtract
//
// Description:		Returns a datset representing the difference of the arguments,
//					but only over the range where the arguments inersect.
//
// Input Arguments:
//		d1	= const Dataset2D&
//		d2	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D Dataset2D::DoUnsyncrhonizedSubtract(const Dataset2D &d1, const Dataset2D &d2)
{
	Dataset2D common1, common2;
	GetOverlappingOnSameTimebase(d1, d2, common1, common2);
	return common1 - common2;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoUnsyncrhonizedMultiply
//
// Description:		Returns a datset representing the product of the arguments,
//					but only over the range where the arguments inersect.
//
// Input Arguments:
//		d1	= const Dataset2D&
//		d2	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D Dataset2D::DoUnsyncrhonizedMultiply(const Dataset2D &d1, const Dataset2D &d2)
{
	Dataset2D common1, common2;
	GetOverlappingOnSameTimebase(d1, d2, common1, common2);
	return common1 * common2;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoUnsyncrhonizedDivide
//
// Description:		Returns a datset representing the quotient of the arguments,
//					but only over the range where the arguments inersect.
//
// Input Arguments:
//		d1	= const Dataset2D&
//		d2	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D Dataset2D::DoUnsyncrhonizedDivide(const Dataset2D &d1, const Dataset2D &d2)
{
	Dataset2D common1, common2;
	GetOverlappingOnSameTimebase(d1, d2, common1, common2);
	return common1 / common2;
}

//==========================================================================
// Class:			Dataset2D
// Function:		DoUnsyncrhonizedExponentiation
//
// Description:		Returns a datset representing the power of the arguments,
//					but only over the range where the arguments inersect.
//
// Input Arguments:
//		d1	= const Dataset2D&
//		d2	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D Dataset2D::DoUnsyncrhonizedExponentiation(const Dataset2D &d1, const Dataset2D &d2)
{
	Dataset2D common1, common2;
	GetOverlappingOnSameTimebase(d1, d2, common1, common2);
	return common1.ToPower(common2);
}

//==========================================================================
// Class:			Dataset2D
// Function:		GetOverlappingOnSameTimebase
//
// Description:		Modifies the output datasets so they contain the information
//					in the original datasets, but only for the overlapping portion.
//					The first dataset is used as the master clock, so the second (d2)
//					is resampled as necessary to ensure that both output datasets
//					have a common timebase.
//
// Input Arguments:
//		d1	= const Dataset2D&
//		d2	= const Dataset2D&
//
// Output Arguments:
//		d1Out	= Dataset2D&
//		d2Out	= Dataset2D&
//
// Return Value:
//		Dataset2D
//
//==========================================================================
void Dataset2D::GetOverlappingOnSameTimebase(const Dataset2D &d1,
	const Dataset2D &d2, Dataset2D &d1Out, Dataset2D &d2Out)
{
	unsigned int start(0);
	if (d1.GetXData(start) < d2.GetXData(0))
	{
		while (d1.GetXData(start) < d2.GetXData(0))
			start++;
	}

	unsigned int end1(d1.GetNumberOfPoints() - 1);
	const unsigned int end2(d2.GetNumberOfPoints() - 1);
	if (d1.GetXData(end1) > d2.GetXData(end2))
	{
		while (d2.GetXData(end2) < d1.GetXData(end1))
			end1--;
	}

	d1Out.Resize(end1 - start);
	d2Out.Resize(end1 - start);

	double x;
	unsigned int i;
	for (i = 0; i < end1 - start; i++)
	{
		x = d1.GetXData(start + i);
		d1Out.GetXPointer()[i] = x;
		d2Out.GetXPointer()[i] = x;
		d1.GetYAt(x, d1Out.GetYPointer()[i]);
		d2.GetYAt(x, d2Out.GetYPointer()[i]);
	}
}

}// namespace LibPlot2D
