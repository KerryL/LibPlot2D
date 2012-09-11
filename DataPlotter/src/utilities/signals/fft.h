/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fft.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Performs fast fourier transform on data.
// History:

#ifndef _FFT_H_
#define _FFT_H_

// Standard C++ headers
#include <string>

// Local forward declarations
class Dataset2D;

class FastFourierTransform
{
public:
	enum FFTWindow
	{
		WindowUniform,
		WindowHann,
		WindowHamming,
		WindowFlatTop,
		//WindowForce,
		WindowExponential,
		WindowCount
	};

	static Dataset2D ComputeFFT(const Dataset2D &data);
	static Dataset2D ComputeFFT(Dataset2D data, const FFTWindow &window,
		unsigned int windowSize, const double &overlap, const bool &subtractMean);
	static void ComputeFRF(const Dataset2D &input, const Dataset2D &output, unsigned int numberOfAverages,
		const FFTWindow &window, const bool &moduloPhase, Dataset2D &amplitude, Dataset2D *phase = NULL, Dataset2D *coherence = NULL);
	static Dataset2D ComputeCoherence(const Dataset2D& input, const Dataset2D& output);

	static unsigned int GetNumberOfAverages(const unsigned int windowSize,
		const double &overlap, const unsigned int &dataSize);
	static double ComputeOverlap(unsigned int &windowSize,
		unsigned int &numberOfAverages, const unsigned int &dataSize);

	static std::string GetWindowName(const FFTWindow &window);
	static unsigned int GetMaxPowerOfTwo(const unsigned int &sampleSize);

private:
	static void ApplyWindow(Dataset2D &data, const FFTWindow &window);
	static void ApplyHannWindow(Dataset2D &data);
	static void ApplyHammingWindow(Dataset2D &data);
	static void ApplyFlatTopWindow(Dataset2D &data);
	//static void ApplyForceWindow(Dataset2D &data);
	static void ApplyExponentialWindow(Dataset2D &data);

	static void DoBitReversal(Dataset2D &set);
	static void DoFFT(Dataset2D &temp);

	static void ZeroDataset(Dataset2D &data);
	static Dataset2D GenerateConstantDataset(const double &xValue, const double &yValue, const unsigned int &size);

	static Dataset2D ComputeCrossPowerSpectrum(const Dataset2D &fftIn, const Dataset2D &fftOut);
	static Dataset2D ComputePowerSpectrum(const Dataset2D &fft);

	static Dataset2D ConvertDoubleSidedToSingleSided(const Dataset2D &fullSpectrum, const bool &preserveDCValue = true);

	static Dataset2D ChopSample(const Dataset2D &data, const unsigned int &sample,
		const unsigned int &windowSize, const double &overlap);
	static void AddToAverage(Dataset2D &average, const Dataset2D &data, const unsigned int &count);

	static void ConvertAmplitudeToDecibels(Dataset2D &fft);
	static void PopulateFrequencyData(Dataset2D &data, const double &sampleRate);

	static Dataset2D GetAmplitudeData(const Dataset2D &rawFFT, const double &sampleRate);
	static Dataset2D GetPhaseData(const Dataset2D &rawFFT, const double &sampleRate, const bool &moduloPhase);

	static Dataset2D ComputeRawFFT(const Dataset2D &data, const FFTWindow &window);
	static void InitializeRawFFTDataset(Dataset2D &rawFFT, const Dataset2D &data, const FFTWindow &window);

	static Dataset2D ComplexAdd(const Dataset2D &a, const Dataset2D &b);
	static Dataset2D ComplexMultiply(const Dataset2D &a, const Dataset2D &b);
	static Dataset2D ComplexDivide(const Dataset2D &a, const Dataset2D &b);

	static Dataset2D ComplexMagnitude(const Dataset2D &a);
	static Dataset2D ComplexPower(const Dataset2D &a, const double &power);

	static unsigned int ComputeRequiredOverlapPoints(const unsigned int &dataSize,
		const unsigned int &windowSize, const unsigned int &averages);
};

#endif// _FFT_H_