/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

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
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeFFT (static)
//
// Description:		Computes the fast fourier transform for the given signal.
//					Assumes y contains data and x is time.
//
// Input Arguments:
//		_data	= const Dataset2D& referring to the data of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the FFT results
//
//==========================================================================
Dataset2D FastFourierTransform::ComputeFFT(const Dataset2D &data)
{
	double sampleRate = 1.0 / (data.GetXData(1) - data.GetXData(0));// [Hz]

	Dataset2D rawFFT = ComputeRawFFT(data, WindowHann);
	Dataset2D fft = GetAmplitudeData(rawFFT, sampleRate);
	fft = ConvertDoubleSidedToSingleSided(fft);
	//ConvertAmplitudeToDecibels(fft);// Appearance can be achieved with log scaled y-axis, so don't force it on them

	return fft;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		InitializeRawFFTDataset (static)
//
// Description:		Initializes the raw FFT dataset for future processing.
//					Chops the data to a power of 2 size and applies a window.
//
// Input Arguments:
//		data			= const Dataset2D&
//		numberOfPoints	= const unsigned int&
//		window			= const FFTWindow&
//
// Output Arguments:
//		rawFFT			= Dataset2D&
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::InitializeRawFFTDataset(Dataset2D &rawFFT,
	const Dataset2D &data, const unsigned int &numberOfPoints, const FFTWindow &window)
{
	rawFFT.Resize(numberOfPoints);

	unsigned int i;
	for (i = 0; i < rawFFT.GetNumberOfPoints(); i++)
	{
		rawFFT.GetXPointer()[i] = data.GetYData(i);
		rawFFT.GetYPointer()[i] = 0.0;
	}

	ApplyWindow(rawFFT, window);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeRawFFT (static)
//
// Description:		Computes the raw (complex) FFT data for the specified
//					time-domain data.
//
// Input Arguments:
//		data	= const Dataset2D&
//		window	= const FFTWindow& indicating the type of window to use
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComputeRawFFT(const Dataset2D &data, const FFTWindow &window)
{
	// FIXME:  Remove trends?
	Dataset2D rawFFT;

	// Determine nearest power of 2
	// FIXME:  Allow selection based on desired freq. resoltuion, etc.
	int powerOfTwo = int(log((float)data.GetNumberOfPoints()) / log(2.0f));
	unsigned int fftPoints = (unsigned int)pow(2.0, powerOfTwo);

	InitializeRawFFTDataset(rawFFT, data, fftPoints, window);
	if (data.GetNumberOfPoints() < 2)
		return rawFFT;

	DoBitReversal(fftPoints, rawFFT);
	DoFFT(powerOfTwo, fftPoints, rawFFT);

	return rawFFT;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeTransferFunction (static)
//
// Description:		Computes transfer function between input and output data.
//
// Input Arguments:
//		input		= const Dataset2D&
//		output		= const Dataset2D&
//
// Output Arguments:
//		amplitude	= Dataset2D&
//		phase		= Dataset2D&
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ComputeTransferFunction(const Dataset2D &input, const Dataset2D &output,
	Dataset2D &amplitude, Dataset2D &phase)
{
	Dataset2D fftIn, fftOut;
	fftIn = ComputeRawFFT(input, WindowUniform);
	fftOut = ComputeRawFFT(output, WindowUniform);

	Dataset2D crossPower = ComputeCrossPowerSpectrum(input, output);
	Dataset2D power = ComputePowerSpectrum(input);
	Dataset2D rawTF = ComplexDivide(crossPower, power);

	double sampleRate = 1.0 / (input.GetXData(1) - input.GetXData(0));// [Hz]

	amplitude = ConvertDoubleSidedToSingleSided(GetAmplitudeData(rawTF, sampleRate));
	phase = ConvertDoubleSidedToSingleSided(GetPhaseData(rawTF, sampleRate));

	//ConvertAmplitudeToDecibels(amplitude);// FIXME:  Not sure about this one...

	// FIXME:  Allow averaging several computations as described in NI application note
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeCrossPowerSpectrum (static)
//
// Description:		Computes the cross power spectrum of the specified raw
//					FFT data sets.
//
// Input Arguments:
//		set1	= const Dataset2D&
//		set2	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComputeCrossPowerSpectrum(const Dataset2D &set1, const Dataset2D &set2)
{
	assert(set1.GetNumberOfPoints() == set2.GetNumberOfPoints());

	// Complex conjugate is multiplication of imaginary part by -1
	Dataset2D complexConjugate = set1 * -1.0;
	Dataset2D crossPowerSpectrum = ComplexMultiply(set2, complexConjugate)
		/ (set1.GetNumberOfPoints() * set2.GetNumberOfPoints());

	return crossPowerSpectrum;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputePowerSpectrum (static)
//
// Description:		Computes the power spectrum of the specified raw FFT data.
//
// Input Arguments:
//		set		= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComputePowerSpectrum(const Dataset2D &set)
{
	return ComputeCrossPowerSpectrum(set, set);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		DoBitReversal (static)
//
// Description:		Performs bit reversal on processing dataset (step one).
//
// Input Arguments:
//		fftPoints	= const unsigned int&
//		set			= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::DoBitReversal(const unsigned int &fftPoints, Dataset2D &set)
{
	unsigned int i, j, k;
	double tempX, tempY;

	j = 0;
	for (i = 0; i < fftPoints - 1; i++)
	{
		if (i < j)
		{
			tempX = set.GetXData(i);
			tempY = set.GetYData(i);
			set.GetXPointer()[i] = set.GetXData(j);
			set.GetYPointer()[i] = set.GetYData(j);
			set.GetXPointer()[j] = tempX;
			set.GetYPointer()[j] = tempY;
		}

		k = fftPoints >> 1;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}

		j += k;
	}
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		DoFFT (static)
//
// Description:		Performs the calculation to get raw FFT data.
//
// Input Arguments:
//		powerOfTwo	= const unsigned int&
//		fftPoitns	= const unsigned int&
//		temp		= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::DoFFT(const unsigned int &powerOfTwo, const unsigned int &fftPoints, Dataset2D &temp)
{
	unsigned int i, j, i1, l, l1, l2;
	double c1, c2, t1, t2, u1, u2, z;

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
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ConvertDoubleSidedToSingleSided (static)
//
// Description:		Converts double-sided data to single-sided.  Discards
//					negative frequency portion of data and scales amplitudes
//					as necessary.
//
// Input Arguments:
//		fullSpectrum	= const Dataset2D& containing double-sided data
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing single-sided data
//
//==========================================================================
Dataset2D FastFourierTransform::ConvertDoubleSidedToSingleSided(const Dataset2D &fullSpectrum)
{
	Dataset2D halfSpectrum(fullSpectrum.GetNumberOfPoints() / 2);

	halfSpectrum.GetXPointer()[0] = fullSpectrum.GetXData(0);
	halfSpectrum.GetYPointer()[0] = fullSpectrum.GetYData(0);// No factor of 2 for DC point

	unsigned int i;
	for (i = 1; i < halfSpectrum.GetNumberOfPoints(); i++)
	{
		halfSpectrum.GetXPointer()[i] = fullSpectrum.GetXData(i);
		halfSpectrum.GetYPointer()[i] = fullSpectrum.GetYData(i) * 2.0;
	}

	return halfSpectrum;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ConvertAmplitudeToDecibels (static)
//
// Description:		Converts from amplitude (linear) to decibels (logarithmic).
//					This MUST be done AFTER converting to single-sided spectrum.
//
// Input Arguments:
//		fft		= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ConvertAmplitudeToDecibels(Dataset2D &fft)
{
	unsigned int i;
	double referenceAmplitude(0.0);

	for (i = 0; i < fft.GetNumberOfPoints(); i++)
	{
		if (fft.GetYData(i) > referenceAmplitude)
			referenceAmplitude = fft.GetYData(i);
	}

	for (i = 0; i < fft.GetNumberOfPoints(); i++)
		fft.GetYPointer()[i] = 20.0 * log10(fft.GetYData(i) / referenceAmplitude);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		PopulateFrequencyData (static)
//
// Description:		Populates the X-data of the specified dataset with frequency
//					data appropriate for the sample rate.
//
// Input Arguments:
//		sampleRate	= const double& [Hz]
//
// Output Arguments:
//		data		= Dataset2D&
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::PopulateFrequencyData(Dataset2D &data, const double &sampleRate)
{
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetXPointer()[i] = (double)i * sampleRate / (double)data.GetNumberOfPoints();
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		GetAmplitudeData (static)
//
// Description:		Calculates the amplitude data from raw FFT data (containing
//					real and imaginary data components).
//
// Input Arguments:
//		rawFFT		= const Dataset2D&
//		sampleRate	= const double& [Hz]
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::GetAmplitudeData(const Dataset2D &rawFFT, const double &sampleRate)
{
	Dataset2D data(rawFFT);
	PopulateFrequencyData(data, sampleRate);

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetYPointer()[i] = sqrt(rawFFT.GetXData(i) * rawFFT.GetXData(i)
		+ rawFFT.GetYData(i) * rawFFT.GetYData(i)) / (double)rawFFT.GetNumberOfPoints();

	return data;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		GetPhaseData (static)
//
// Description:		Calculates the phase data from raw FFT data (containing
//					real and imaginary data components).
//
// Input Arguments:
//		rawFFT		= const Dataset2D&
//		sampleRate	= const double& [Hz]
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::GetPhaseData(const Dataset2D &rawFFT, const double &sampleRate)
{
	Dataset2D data(rawFFT);
	PopulateFrequencyData(data, sampleRate);

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetYPointer()[i] = atan2(rawFFT.GetYData(i), rawFFT.GetXData(i)) * 180.0 / PlotMath::Pi;

	return data;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComplexMultiply (static)
//
// Description:		Performs element-wise complex multiplication of the
//					specified datasets.
//
// Input Arguments:
//		a	= const Dataset2D&
//		b	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComplexMultiply(const Dataset2D &a, const Dataset2D &b)
{
	assert(a.GetNumberOfPoints() == b.GetNumberOfPoints());

	Dataset2D result(a.GetNumberOfPoints());

	unsigned int i;
	for (i = 0; i < a.GetNumberOfPoints(); i++)
	{
		result.GetXPointer()[i] = a.GetXData(i) * b.GetXData(i) - a.GetYData(i) * b.GetYData(i);
		result.GetYPointer()[i] = a.GetYData(i) * b.GetXData(i) + a.GetXData(i) * b.GetYData(i);
	}

	return result;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComplexDivide (static)
//
// Description:		Performs element-wise complex division of the specified
//					datasets.
//
// Input Arguments:
//		a	= const Dataset2D&, numerator
//		b	= const Dataset2D&, denominator
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComplexDivide(const Dataset2D &a, const Dataset2D &b)
{
	assert(a.GetNumberOfPoints() == b.GetNumberOfPoints());

	Dataset2D result(a.GetNumberOfPoints());

	unsigned int i;
	double denominator;
	for (i = 0; i < a.GetNumberOfPoints(); i++)
	{
		denominator = b.GetXData(i) * b.GetXData(i) + b.GetYData(i) * b.GetYData(i);
		result.GetXPointer()[i] = (a.GetXData(i) * b.GetXData(i) + a.GetYData(i) * b.GetYData(i)) / denominator;
		result.GetYPointer()[i] = (a.GetYData(i) * b.GetXData(i) - a.GetXData(i) * b.GetYData(i)) / denominator;
	}

	return result;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyWindow (static)
//
// Description:		Applies the specified window type to the data.  The data
//					must have size equal to the number of points in the FFT.
//
// Input Arguments:
//		data	= Dataset2D&
//		window	= const FFTWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ApplyWindow(Dataset2D &data, const FFTWindow &window)
{
	if (window == WindowUniform)
		return;// No processing necessary
	else if (window == WindowHann)
		ApplyHannWindow(data);
	else if (window == WindowHamming)
		ApplyHammingWindow(data);
	else if (window == WindowFlatTop)
		ApplyFlatTopWindow(data);
	/*else if (window == WindowForce)
		ApplyForceWindow(data);
	else if (window == WindowExponential)
		ApplyExponentialWindow(data);*/
	else
		assert(false);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyHannWindow (static)
//
// Description:		Applies a Hann window to the data.  Note that there is a
//					missing factor of 0.5 - this cancels when divided by the
//					coherent gain of 0.5.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ApplyHannWindow(Dataset2D &data)
{
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetXPointer()[i] *= 1.0
		- cos(2.0 * PlotMath::Pi * (double)i / double(data.GetNumberOfPoints() - 1));
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyHammingWindow (static)
//
// Description:		Applies a Hamming window to the data.  Scales by coherent
//					gain of 0.54.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ApplyHammingWindow(Dataset2D &data)
{
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetXPointer()[i] *= (0.54 - 0.46
		* cos(2.0 * PlotMath::Pi * (double)i / double(data.GetNumberOfPoints() - 1))) / 0.54;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyFlatTopWindow (static)
//
// Description:		Applies a flat top window to the data.  Scales by coherent
//					gain of 0.22.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ApplyFlatTopWindow(Dataset2D &data)
{
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetXPointer()[i] *= (1.0
		- 1.93 * cos(2.0 * PlotMath::Pi * (double)i / double(data.GetNumberOfPoints() - 1))
		+ 1.29 * cos(4.0 * PlotMath::Pi * (double)i / double(data.GetNumberOfPoints() - 1))
		- 0.388 * cos(6.0 * PlotMath::Pi * (double)i / double(data.GetNumberOfPoints() - 1))
		+ 0.032 * cos(8.0 * PlotMath::Pi * (double)i / double(data.GetNumberOfPoints() - 1))) / 0.22;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyForceWindow (static)
//
// Description:		Applies a force window to the data.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*void FastFourierTransform::ApplyForceWindow(Dataset2D &data)
{
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyExponentialWindow (static)
//
// Description:		Applies an exponential window to the data.
//
// Input Arguments:
//		data	= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ApplyExponentialWindow(Dataset2D &data)
{
}*/