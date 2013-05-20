/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataset2D.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Container for x and y-data series for plotting.
// History:

// Standard C++ headers
#include <fstream>
#include <utility>
#include <assert.h>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "utilities/dataset2D.h"
#include "utilities/math/plotMath.h"

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
//		_numberOfPoints = const unsigned int &
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Dataset2D::Dataset2D(const unsigned int &_numberOfPoints)
{
	xData = NULL;
	yData = NULL;
	Resize(_numberOfPoints);
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
void Dataset2D::Reverse(void)
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
//		_numberOfPoints = const unsigned int &
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Dataset2D::Resize(const unsigned int &_numberOfPoints)
{
	delete [] xData;
	delete [] yData;

	numberOfPoints = _numberOfPoints;

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

	std::ofstream exportFile(pathAndFileName.c_str(), std::ios::out);
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
//		value	= double& specifying the X-value
//
// Output Arguments:
//		value	= double& specifying the Y-value
//		exactValue	= bool* indicating whether or not the exact value is being returned
//
// Return Value:
//		true if specified x is within range of data, false otherwise
//
//==========================================================================
bool Dataset2D::GetYAt(double &x, bool *exactValue) const
{
	// This assumes data is entered from small x to large x and that y is a function of x
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
	{
		if (xData[i] == x)
		{
			x = yData[i];

			if (exactValue)
				*exactValue = true;

			return true;
		}
		else if (xData[i] > x)
		{
			if (i > 0)
				x = yData[i - 1] + (yData[i] - yData[i - 1]) * (x - xData[i - 1]) / (xData[i] - xData[i - 1]);
			else
				x = yData[i];

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
		yData[i] = PlotMath::Modulo(yData[i], target);

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
Dataset2D& Dataset2D::DoLog(void)
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
Dataset2D& Dataset2D::DoLog10(void)
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
Dataset2D& Dataset2D::DoExp(void)
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
Dataset2D& Dataset2D::DoAbs(void)
{
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		yData[i] = abs(yData[i]);

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
//		const Dataset2D&
//
//==========================================================================
const Dataset2D Dataset2D::DoLog(void) const
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
//		const Dataset2D&
//
//==========================================================================
const Dataset2D Dataset2D::DoLog10(void) const
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
//		const Dataset2D&
//
//==========================================================================
const Dataset2D Dataset2D::DoExp(void) const
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
//		const Dataset2D&
//
//==========================================================================
const Dataset2D Dataset2D::DoAbs(void) const
{
	Dataset2D result(*this);
	return result.DoAbs();
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
double Dataset2D::ComputeYMean(void) const
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
double Dataset2D::GetAverageDeltaX(void) const
{
	double sum(0.0);
	unsigned int i;
	for (i = 1; i < numberOfPoints; i++)
		sum += xData[i] - xData[i - 1];

	return sum / ((double)numberOfPoints - 1.0);
}