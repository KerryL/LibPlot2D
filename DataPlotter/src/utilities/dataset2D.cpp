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
	// Determine what type of delimiter to use
	wxString extension(pathAndFileName.substr(pathAndFileName.find_last_of('.') + 1));
	wxChar delimiter;
	if (extension.Cmp(_T("txt")) == 0)
		// Tab delimited
		delimiter = '\t';
	else if (extension.Cmp(_T("csv")) == 0)
		// Comma separated values
		delimiter = ',';
	else
		return;

	// Perform the save - open the file
	std::ofstream exportFile(pathAndFileName.c_str(), std::ios::out);

	// Warn the user if the file could not be opened failed
	if (!exportFile.is_open() || !exportFile.good())
		return;

	// Write the information to file
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
		exportFile << xData[i] << delimiter << yData[i] << std::endl;

	// Close the file
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
//
// Return Value:
//		True if interpolation was used, false otherwise
//
//==========================================================================
bool Dataset2D::GetYAt(double &x) const
{
	// This assumes data is entered from small x to large x and that y is a function of x
	unsigned int i;
	for (i = 0; i < numberOfPoints; i++)
	{
		if (xData[i] == x)
		{
			x = yData[i];
			return true;
		}
		else if (xData[i] > x)
		{
			if (i > 0)
				x = yData[i - 1] + (yData[i] - yData[i - 1]) * (x - xData[i - 1]) / (xData[i] - xData[i - 1]);
			else
				x = yData[i];

			break;
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
	Dataset2D result = *this;
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
	Dataset2D result = *this;
	result.ToPower(target);

	return result;
}