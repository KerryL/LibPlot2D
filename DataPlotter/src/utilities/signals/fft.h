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
		/*WindowForce,
		WindowExponential*/
	};

	static Dataset2D ComputeFFT(const Dataset2D &data);
	static void ComputeTransferFunction(const Dataset2D &input, const Dataset2D &output,
		Dataset2D &amplitude, Dataset2D &phase);
	static Dataset2D ComputeCrossPowerSpectrum(const Dataset2D &set1, const Dataset2D &set2);
	static Dataset2D ComputePowerSpectrum(const Dataset2D &set);

private:
	static void ApplyWindow(Dataset2D &data, const FFTWindow &window);
	static void ApplyHannWindow(Dataset2D &data);
	static void ApplyHammingWindow(Dataset2D &data);
	static void ApplyFlatTopWindow(Dataset2D &data);
	/*static void ApplyForceWindow(Dataset2D &data);
	static void ApplyExponentialWindow(Dataset2D &data);*/

	static void DoBitReversal(const unsigned int &fftPoints, Dataset2D &set);
	static void DoFFT(const unsigned int &powerOfTwo, const unsigned int &fftPoints, Dataset2D &temp);

	static Dataset2D ConvertDoubleSidedToSingleSided(const Dataset2D &fullSpectrum);

	static void ConvertAmplitudeToDecibels(Dataset2D &fft);
	static void PopulateFrequencyData(Dataset2D &data, const double &sampleRate);

	static Dataset2D GetAmplitudeData(const Dataset2D &rawFFT, const double &sampleRate);
	static Dataset2D GetPhaseData(const Dataset2D &rawFFT, const double &sampleRate);

	static Dataset2D ComputeRawFFT(const Dataset2D &data, const FFTWindow &window);
	static void InitializeRawFFTDataset(Dataset2D &rawFFT, const Dataset2D &data,
		const unsigned int &numberOfPoints, const FFTWindow &window);

	static Dataset2D ComplexMultiply(const Dataset2D &a, const Dataset2D &b);
	static Dataset2D ComplexDivide(const Dataset2D &a, const Dataset2D &b);
};

#endif// _FFT_H_