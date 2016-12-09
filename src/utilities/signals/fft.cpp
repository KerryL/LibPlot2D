/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  fft.cpp
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Performs fast fourier transform on data.
// History:

// Standard C++ headers
#include <cmath>
#include <algorithm>

// Local headers
#include "lp2d/utilities/signals/fft.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/utilities/math/plotMath.h"
#include "lp2d/utilities/signals/derivative.h"

namespace LibPlot2D
{

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeFFT (static)
//
// Description:		Computes the fast fourier transform for the given signal.
//					Assumes y contains data and x is time.  Quick version -
//					all default options are used.
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
	return ComputeFFT(data, WindowHann, 0, 0.0, true);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeFFT (static)
//
// Description:		Computes the fast fourier transform for the given signal.
//					Assumes y contains data and x is time.  This overload
//					contains all specifiable parameters.
//
// Input Arguments:
//		data			= Dataset2D referring to the data of interest
//		window			= const FFTWindow&
//		windowSize		= unsigned int, number of points in each sample;
//						  zero uses max sample size
//		overlap			= const double&, percentage overlap (0.0 - 1.0) between
//						  adjacent samples
//		subtractMean	= const bool& indicating that the data should be averaged,
//						  then have the average subtracted from the data
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing the FFT results
//
//==========================================================================
Dataset2D FastFourierTransform::ComputeFFT(Dataset2D data, const FFTWindow &window,
		unsigned int windowSize, const double &overlap, const bool &subtractMean)
{
	double sampleRate = 1.0 / data.GetAverageDeltaX();// [Hz]

	if (subtractMean)
		data -= data.ComputeYMean();

	if (windowSize == 0)
		windowSize = (unsigned int)pow(2, (double)GetMaxPowerOfTwo(data.GetNumberOfPoints()));

	Dataset2D rawFFT, fft;
	unsigned int i, count = GetNumberOfAverages(windowSize, overlap, data.GetNumberOfPoints());
	for (i = 0; i < count; i++)
	{
		rawFFT = ComputeRawFFT(ChopSample(data, i, windowSize, overlap), window);
		AddToAverage(fft, GetAmplitudeData(rawFFT, sampleRate), count);
	}
	fft = ConvertDoubleSidedToSingleSided(fft);
	//ConvertAmplitudeToDecibels(fft);// Appearance can be achieved with log scaled y-axis, so don't force it on them

	return fft;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		GetMaxPowerOfTwo (static)
//
// Description:		Returns the max allowable window size given the number of
//					data points.
//
// Input Arguments:
//		sampleSize	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int FastFourierTransform::GetMaxPowerOfTwo(const unsigned int &sampleSize)
{
	return (unsigned int)(log((double)sampleSize) / log(2.0));
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ChopSample (static)
//
// Description:		Chops the specified sample from the data.
//
// Input Arguments:
//		data		= const Dataset2D&
//		sample		= const unsigned int&
//		windowSize	= const unsigned int&
//		overlap		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ChopSample(const Dataset2D &data, const unsigned int &sample,
	const unsigned int &windowSize, const double &overlap)
{
	assert(overlap >= 0.0 && overlap <= 1.0 && windowSize > 0);

	unsigned int overlapSize = (unsigned int)(overlap * (double)windowSize);
	if (overlapSize >= windowSize)
		overlapSize = windowSize - 1;
	unsigned int start = sample * (windowSize - overlapSize);

	assert(start + windowSize <= data.GetNumberOfPoints());

	Dataset2D chopped(windowSize);
	unsigned int i;
	for (i = 0; i < windowSize; i++)
	{
		chopped.GetXPointer()[i] = data.GetXData(start + i);
		chopped.GetYPointer()[i] = data.GetYData(start + i);
	}

	return chopped;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		AddToAverage (static)
//
// Description:		Adds the data to the average.  Initializes the average
//					if it is empty.
//
// Input Arguments:
//		average	= Dataset2D& (input and output)
//		data	= const Dataset2D&
//		count	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::AddToAverage(Dataset2D &average, const Dataset2D &data, const unsigned int &count)
{
	unsigned int i;
	if (average.GetNumberOfPoints() == 0)
	{
		average.Resize(data.GetNumberOfPoints());
		for (i = 0; i < average.GetNumberOfPoints(); i++)
		{
			average.GetXPointer()[i] = data.GetXData(i);
			average.GetYPointer()[i] = 0.0;
		}
	}

	for (i = 0; i < average.GetNumberOfPoints(); i++)
		average.GetYPointer()[i] += data.GetYData(i) / double(count);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		InitializeRawFFTDataset (static)
//
// Description:		Initializes the raw FFT dataset for future processing.
//					Applies the specified window.
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
	const Dataset2D &data, const FFTWindow &window)
{
	rawFFT.Resize(data.GetNumberOfPoints());

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
//					time-domain data.  It is expected that the data has size
//					equal to the number of points to use for the FFT (it is
//					one FFT sample).
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
	Dataset2D rawFFT;

	InitializeRawFFTDataset(rawFFT, data, window);
	if (data.GetNumberOfPoints() < 2)
		return rawFFT;

	DoBitReversal(rawFFT);
	DoFFT(rawFFT);

	return rawFFT;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeFRF (static)
//
// Description:		Computes frequency response function between input and output data.
//
// Input Arguments:
//		input				= const Dataset2D&
//		output				= const Dataset2D&
//		numberOfAverages	= unsigned int
//		window				= const FFTWindow&
//		moduloRange			= const bool&
//
// Output Arguments:
//		amplitude			= Dataset2D& [dB]
//		phase				= Dataset2D* [deg]
//		coherence			= Dataset2D*
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::ComputeFRF(const Dataset2D &input, const Dataset2D &output,
	unsigned int numberOfAverages, const FFTWindow &window, const bool &moduloPhase,
	Dataset2D &amplitude, Dataset2D *phase, Dataset2D *coherence)
{
	assert(input.GetNumberOfPoints() == output.GetNumberOfPoints());

	unsigned int i, windowSize;
	double overlap = ComputeOverlap(windowSize, numberOfAverages, input.GetNumberOfPoints());

	Dataset2D fftIn, fftOut, crossPower(windowSize), power(windowSize), size(GenerateConstantDataset(numberOfAverages, 0.0, windowSize));
	ZeroDataset(crossPower);
	ZeroDataset(power);
	for (i = 0; i < numberOfAverages; i++)
	{
		fftIn = ComputeRawFFT(ChopSample(input, i, windowSize, overlap), window);
		fftOut = ComputeRawFFT(ChopSample(output, i, windowSize, overlap), window);

		crossPower = ComplexAdd(crossPower, ComputeCrossPowerSpectrum(fftIn, fftOut));
		power = ComplexAdd(power, ComputePowerSpectrum(fftIn));
	}

	crossPower = ComplexDivide(crossPower, size);
	power = ComplexDivide(power, size);
	Dataset2D rawFRF = ComplexDivide(crossPower, power);

	double sampleRate = 1.0 / input.GetAverageDeltaX();// [Hz]
	amplitude = ConvertDoubleSidedToSingleSided(GetAmplitudeData(rawFRF, sampleRate), false);
	if (phase)
		*phase = ConvertDoubleSidedToSingleSided(GetPhaseData(rawFRF, sampleRate, moduloPhase), false);
	if (coherence)
		*coherence = ConvertDoubleSidedToSingleSided(ComputeCoherence(input, output), false);

	ConvertAmplitudeToDecibels(amplitude);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeCrossPowerSpectrum (static)
//
// Description:		Computes the cross power spectrum of the specified raw
//					FFT data sets.
//
// Input Arguments:
//		fftIn	= const Dataset2D&
//		fftOut	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComputeCrossPowerSpectrum(const Dataset2D &fftIn, const Dataset2D &fftOut)
{
	assert(fftIn.GetNumberOfPoints() == fftOut.GetNumberOfPoints());

	Dataset2D size(fftIn.GetNumberOfPoints());
	unsigned int i;
	for (i = 0; i < size.GetNumberOfPoints(); i++)
	{
		size.GetXPointer()[i] = size.GetNumberOfPoints() * size.GetNumberOfPoints();
		size.GetYPointer()[i] = 0.0;
	}

	return  ComplexDivide(ComplexMultiply(fftOut, fftIn * -1.0), size);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputePowerSpectrum (static)
//
// Description:		Computes the power spectrum of the specified raw FFT data.
//
// Input Arguments:
//		fft		= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComputePowerSpectrum(const Dataset2D &fft)
{
	return ComputeCrossPowerSpectrum(fft, fft);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeCoherence (static)
//
// Description:		Computes the coherence function for the specified input/output.
//
// Input Arguments:
//		input	= const Dataset2D&
//		output	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComputeCoherence(const Dataset2D& input, const Dataset2D& output)
{
	assert(input.GetNumberOfPoints() == output.GetNumberOfPoints());

	unsigned int windowSize = (unsigned int)pow(2, (double)GetMaxPowerOfTwo(input.GetNumberOfPoints()));

	Dataset2D fftIn, fftOut;
	fftIn = ComputeRawFFT(ChopSample(input, 0, windowSize, 0.0), WindowUniform);
	fftOut = ComputeRawFFT(ChopSample(output, 0, windowSize, 0.0), WindowUniform);

	Dataset2D crossPower = ComputeCrossPowerSpectrum(fftIn, fftOut);
	Dataset2D inputPower = ComputePowerSpectrum(fftIn);
	Dataset2D outputPower = ComputePowerSpectrum(fftOut);

	Dataset2D rawCoherence = ComplexDivide(ComplexPower(ComplexMagnitude(crossPower), 2.0),
		ComplexMultiply(inputPower, outputPower));
	rawCoherence = ConvertDoubleSidedToSingleSided(rawCoherence);

	double sampleRate = 1.0 / input.GetAverageDeltaX();// [Hz]

	return GetAmplitudeData(rawCoherence, sampleRate);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ZeroDataset (static)
//
// Description:		Zeros out the data in the dataset.
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
void FastFourierTransform::ZeroDataset(Dataset2D &data)
{
	data = GenerateConstantDataset(0.0, 0.0, data.GetNumberOfPoints());
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		GenerateConstantDataset (static)
//
// Description:		Generates a dataset with constant x- and y-values.
//
// Input Arguments:
//		xValue	= const double&
//		yValue	= const double&
//		size	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Dataset2D FastFourierTransform::GenerateConstantDataset(const double &xValue, const double &yValue, const unsigned int &size)
{
	Dataset2D data(size);

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
	{
		data.GetXPointer()[i] = xValue;
		data.GetYPointer()[i] = yValue;
	}

	return data;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		DoBitReversal (static)
//
// Description:		Performs bit reversal on processing dataset.  It is assumed
//					that the set has already been padded/chopped.
//
// Input Arguments:
//		set			= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::DoBitReversal(Dataset2D &set)
{
	assert(double(set.GetNumberOfPoints()) / 2.0 == double(set.GetNumberOfPoints() / 2));

	unsigned int i, j, k;
	double tempX, tempY;

	j = 0;
	for (i = 0; i < set.GetNumberOfPoints() - 1; i++)
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

		k = set.GetNumberOfPoints() >> 1;
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
//		temp		= Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void FastFourierTransform::DoFFT(Dataset2D &temp)
{
	unsigned int i, j, i1, l, l1, l2;
	double c1, c2, t1, t2, u1, u2, z;
	unsigned int powerOfTwo = GetMaxPowerOfTwo(temp.GetNumberOfPoints());

	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l < powerOfTwo; l++)
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j < l1; j++)
		{
			for (i = j; i < temp.GetNumberOfPoints(); i += l2)
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
//		preserveDCValue	= const bool& indicating whether or not to keep the 0 Hz point (optional)
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D containing single-sided data
//
//==========================================================================
Dataset2D FastFourierTransform::ConvertDoubleSidedToSingleSided(const Dataset2D &fullSpectrum, const bool &preserveDCValue)
{
	Dataset2D halfSpectrum;
	unsigned int i;

	if (preserveDCValue)
	{
		halfSpectrum.Resize(fullSpectrum.GetNumberOfPoints() / 2 + 1);

		halfSpectrum.GetXPointer()[0] = fullSpectrum.GetXData(0);
		halfSpectrum.GetYPointer()[0] = fullSpectrum.GetYData(0);// No factor of 2 for DC point

		for (i = 1; i < halfSpectrum.GetNumberOfPoints(); i++)
		{
			halfSpectrum.GetXPointer()[i] = fullSpectrum.GetXData(i);
			halfSpectrum.GetYPointer()[i] = fullSpectrum.GetYData(i) * 2.0;
		}
	}
	else
	{
		halfSpectrum.Resize(fullSpectrum.GetNumberOfPoints() / 2);

		for (i = 0; i < halfSpectrum.GetNumberOfPoints(); i++)
		{
			halfSpectrum.GetXPointer()[i] = fullSpectrum.GetXData(i + 1);
			halfSpectrum.GetYPointer()[i] = fullSpectrum.GetYData(i + 1);
		}
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
//		moduloPhase	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::GetPhaseData(const Dataset2D &rawFFT, const double &sampleRate,
	const bool &moduloPhase)
{
	Dataset2D data(rawFFT);
	PopulateFrequencyData(data, sampleRate);

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetYPointer()[i] = atan2(rawFFT.GetYData(i), rawFFT.GetXData(i));

	if (!moduloPhase)
		PlotMath::Unwrap(data);

	data *= 180.0 / PlotMath::pi;

	return data;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComplexAdd (static)
//
// Description:		Performs element-wise complex addition of the
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
Dataset2D FastFourierTransform::ComplexAdd(const Dataset2D &a, const Dataset2D &b)
{
	assert(a.GetNumberOfPoints() == b.GetNumberOfPoints());

	Dataset2D result(a.GetNumberOfPoints());

	unsigned int i;
	for (i = 0; i < a.GetNumberOfPoints(); i++)
	{
		result.GetXPointer()[i] = a.GetXData(i) + b.GetXData(i);
		result.GetYPointer()[i] = a.GetYData(i) + b.GetYData(i);
	}

	return result;
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
// Function:		ComplexMagnitude (static)
//
// Description:		Computes the magnitude of each complex element.
//
// Input Arguments:
//		a	= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComplexMagnitude(const Dataset2D &a)
{
	Dataset2D result(a.GetNumberOfPoints());

	unsigned int i;
	for (i = 0; i < result.GetNumberOfPoints(); i++)
	{
		result.GetXPointer()[i] = sqrt(a.GetXData(i) * a.GetXData(i)
			+ a.GetYData(i) * a.GetYData(i));
		result.GetYPointer()[i] = 0.0;
	}

	return result;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComplexPower (static)
//
// Description:		Raises each complex element to the specified power.
//
// Input Arguments:
//		a		= const Dataset2D&
//		power	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Dataset2D
//
//==========================================================================
Dataset2D FastFourierTransform::ComplexPower(const Dataset2D &a, const double &power)
{
	Dataset2D result(a.GetNumberOfPoints());

	double magnitude, angle;

	unsigned int i;
	for (i = 0; i < a.GetNumberOfPoints(); i++)
	{
		magnitude = sqrt(a.GetXData(i) * a.GetXData(i) + a.GetYData(i) * a.GetYData(i));
		angle = atan2(a.GetYData(i), a.GetXData(i));

		magnitude = pow(magnitude, power);
		angle *= power;

		result.GetXPointer()[i] = magnitude * cos(angle);
		result.GetYPointer()[i] = magnitude * sin(angle);
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
		ApplyForceWindow(data);*/
	else if (window == WindowExponential)
		ApplyExponentialWindow(data);
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
		- cos(2.0 * PlotMath::pi * (double)i / double(data.GetNumberOfPoints() - 1));
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
		* cos(2.0 * PlotMath::pi * (double)i / double(data.GetNumberOfPoints() - 1))) / 0.54;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyFlatTopWindow (static)
//
// Description:		Applies a flat top window to the data.  Scales by coherent
//					gain of 0.22.  NOTE:  Scaling removed, as apparently it was
//					incorrect.  Can not find any references explaining this.
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
		data.GetXPointer()[i] *= 1.0
		- 1.93 * cos(2.0 * PlotMath::pi * (double)i / double(data.GetNumberOfPoints() - 1))
		+ 1.29 * cos(4.0 * PlotMath::pi * (double)i / double(data.GetNumberOfPoints() - 1))
		- 0.388 * cos(6.0 * PlotMath::pi * (double)i / double(data.GetNumberOfPoints() - 1))
		+ 0.032 * cos(8.0 * PlotMath::pi * (double)i / double(data.GetNumberOfPoints() - 1));
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
}*/

//==========================================================================
// Class:			FastFourierTransform
// Function:		ApplyExponentialWindow (static)
//
// Description:		Applies an exponential window to the data.  Denominator of
//					exponent is chosen based on sample length to reduce
//					amplitude to 2% of original value at the end of the window.
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
	unsigned int i;
	double tau(-((double)data.GetNumberOfPoints() - 1.0) / log(0.02));
	for (i = 0; i < data.GetNumberOfPoints(); i++)
		data.GetXPointer()[i] *= exp(-(double)i / tau);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		GetWindowName (static)
//
// Description:		Returns a string describing the specified window.
//
// Input Arguments:
//		window	= const FFTWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::string
//
//==========================================================================
std::string FastFourierTransform::GetWindowName(const FFTWindow &window)
{
	if (window == WindowUniform)
		return "Uniform";
	else if (window == WindowHann)
		return "Hann";
	else if (window == WindowHamming)
		return "Hamming";
	else if (window == WindowFlatTop)
		return "Flat Top";
	/*else if (window == WindowForce)
		return "Force";*/
	else if (window == WindowExponential)
		return "Exponential";

	assert(false);
	return "";
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		GetNumberOfAverages (static)
//
// Description:		Determines the number of samples to be averaged, given the
//					specified parameters.
//
// Input Arguments:
//		windowSize	= const unsigned int&
//		overlap		= const double&
//		dataSize	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int FastFourierTransform::GetNumberOfAverages(const unsigned int windowSize,
		const double &overlap, const unsigned int &dataSize)
{
	unsigned int overlapSize = (unsigned int)(overlap * (double)windowSize);
	if (overlapSize >= windowSize)
		overlapSize = windowSize - 1;
	return (dataSize - overlapSize) / (windowSize - overlapSize);
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeOverlap (static)
//
// Description:		Computes the required overlap (and updates other parameters).
//					Keeps the overlap <= 50%.
//
// Input Arguments:
//		numberOfAverages	= unsigned int&
//		dataSize			= const unsigned int&
//
// Output Arguments:
//		windowSize			= unsigned int&
//
// Return Value:
//		double
//
//==========================================================================
double FastFourierTransform::ComputeOverlap(unsigned int &windowSize,
		unsigned int &numberOfAverages, const unsigned int &dataSize)
{
	unsigned int maxWindowSize = (unsigned int)pow(2.0, (double)GetMaxPowerOfTwo(dataSize));
	if (numberOfAverages == 1)
	{
		windowSize = maxWindowSize;
		return 0.0;
	}

	if (numberOfAverages >= maxWindowSize)
		numberOfAverages = maxWindowSize / 2;

	windowSize = (unsigned int)pow(2.0,
		ceil(log((double)dataSize / (double)numberOfAverages) / log(2.0)));

	unsigned int overlapPoints = ComputeRequiredOverlapPoints(dataSize, windowSize, numberOfAverages);
	if ((double)overlapPoints / (double)windowSize > 0.5)
	{
		windowSize /= 2;
		return 0.0;
	}

	return (double)overlapPoints / (double)windowSize;
}

//==========================================================================
// Class:			FastFourierTransform
// Function:		ComputeRequiredOverlapPoints (static)
//
// Description:		Computes the required overlap points for the specified
//					parameters.
//
// Input Arguments:
//		dataSize	= const unsigned int&
//		windowSize	= const unsigned int&
//		averages	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int FastFourierTransform::ComputeRequiredOverlapPoints(const unsigned int &dataSize,
		const unsigned int &windowSize, const unsigned int &averages)
{
	if (averages == 1)
		return 0;

	double overlap = double(dataSize - double(windowSize * averages)) / (1.0 - (double)averages);
	if (overlap < 0.0)
		return 0;

	return (unsigned int)ceil(overlap);
}

}// namespace LibPlot2D
