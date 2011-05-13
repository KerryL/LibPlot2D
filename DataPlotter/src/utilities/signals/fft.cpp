/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fft.cpp
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Performs fast fourier transform on data.
// History:

// Standard C++ headers
#include <cmath>

// Local headers
#include "utilities/signals/fft.h"
#include "utilities/dataset2D.h"

//==========================================================================
// Class:			FastFourierTransform
// Function:		Compute (static)
//
// Description:		Computes the fast fourier transform for the given signal.
//					Assumes y contains data and x is time.
//
// Input Argurments:
//		_data	= const Dataset2D& referring to the data of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the requested time history
//
//==========================================================================
Dataset2D FastFourierTransform::Compute(const Dataset2D &data)
{
	// FIXME:  Remove trends?
	// FIXME:  Windowing?
	Dataset2D temp;
	Dataset2D fft;
	// FFT must contain the real and imaginary arrays of data

	// Determine nearest power of 2
	// FIXME:  Allow selection based on desired freq. resoltuion, etc.
	int powerOfTwo = int(log((float)data.GetNumberOfPoints()) / log(2.0f));
	unsigned int fftPoints = (unsigned int)pow(2.0, powerOfTwo);
	double sampleRate = 1.0 / (data.GetXData(1) - data.GetXData(0));// [Hz]

	temp.Resize(fftPoints);
	fft.Resize(fftPoints / 2);

	unsigned int a;
	for (a = 0; a < temp.GetNumberOfPoints(); a++)
	{
		temp.GetXPointer()[a] = data.GetYData(a);
		temp.GetYPointer()[a] = 0.0;
	}

	if (data.GetNumberOfPoints() < 2)
		return temp;

	unsigned int i, i1, j, k, i2, l, l1, l2;
	double c1, c2, tempX, tempY, t1, t2, u1, u2, z;

	// Do the bit reversal
	i2 = fftPoints >> 1;
	j = 0;
	for (i = 0; i < fftPoints - 1; i++)
	{
		if (i < j)
		{
			tempX = temp.GetXData(i);
			tempY = temp.GetYData(i);
			temp.GetXPointer()[i] = temp.GetXData(j);
			temp.GetYPointer()[i] = temp.GetYData(j);
			temp.GetXPointer()[j] = tempX;
			temp.GetYPointer()[j] = tempY;
		}

		k = i2;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}

		j += k;
	}

	// Compute the FFT
	c1 = -1.0; 
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l < (unsigned int)powerOfTwo; l++)
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0; 
		u2 = 0.0;
		for (j = 0; j < l1; j++)
		{
			for (i = j; i < fftPoints; i += l2)
			{
				i1 = i + l1;
				t1 = u1 * temp.GetXData(i1) - u2 * temp.GetYData(i1);
				t2 = u1 * temp.GetYData(i1) + u2 * temp.GetXData(i1);
				temp.GetXPointer()[i1] = temp.GetXData(i) - t1; 
				temp.GetYPointer()[i1] = temp.GetYData(i) - t2;
				temp.GetXPointer()[i] += t1;
				temp.GetYPointer()[i] += t2;
			}
			z =  u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	double magnitude, frequency;
	for (i = 0; i < fft.GetNumberOfPoints(); i++)
	{
		// Break out into magnitude (Y) and frequency (X)
		magnitude = 2.0 / fftPoints * temp.GetYData(i);
		frequency = 1.0 / (data.GetXData(1) - data.GetXData(0)) / fftPoints;
		fft.GetYPointer()[i] = 2.0 / fftPoints * sqrt(
			temp.GetXData(i) * temp.GetXData(i) + temp.GetYData(i) * temp.GetYData(i));
		fft.GetXPointer()[i] = i * sampleRate / fftPoints;
	}

	return fft;
}