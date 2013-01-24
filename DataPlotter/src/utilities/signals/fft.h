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
#include <set>
#include <vector>

// Local forward declarations
class Dataset2D;

class FastFourierTransform
{
public:
	/// Enumeration of available FFT window functions
	enum FFTWindow
	{
		WindowUniform,///< For use with transients whose duration is less than the length of the window
		WindowHann,///< For use in general-purpose applications
		WindowHamming,///< For use with closely-spaced sine waves
		WindowFlatTop,///< For use in accurate single-tone amplitude measurements
		//WindowForce,
		WindowExponential,///< For use with response measurements (i.e. response to an impact test)
		WindowCount///< Number of windows available
	};

	/// Computes FFT of the specified dataset with default options.
	/*! By default, the sample is chopped so the size is equal to the next-
		lowest power of two.  No averaging is used.  A Hann window is applied,
		and the data is mean-subtracted.

		\param[in] data	for which FFT should be computed

		\return The processed amplitude vs. frequency FFT information
	*/
	static Dataset2D ComputeFFT(const Dataset2D &data);

	/// Computes FFT of the specified dataset with specified options.
	/*! Averaging is determined based on the length of the signal, the
		specified window size and overlap.

		\param[in] data			for which FFT should be computed
		\param[in] window		to be applied
		\param[in] windowSize	length of the window
		\param[in] overlap		between adjacent windows in percent (0.0 to 1.0)
		\param[in] subtractMean	indicating whether or not the data should be mean-subtracted to remove DC content

		\return The processed amplitude vs. frequency FFT information

		\sa GetNumberOfAverages
	*/
	static Dataset2D ComputeFFT(Dataset2D data, const FFTWindow &window,
		unsigned int windowSize, const double &overlap, const bool &subtractMean);

	/// Computes the Frequency Response Function for the specified signals.
	/*! Determines the frequency-dependent relationship between the specified signals.
		Overlap is computed based on the specified number of averages and the length of
		the signal.

		\param[in] input			signal time history
		\param[in] output			signal time history
		\param[in] numberOfAverages	to use (note that increased averages may result
									in smoother curves, but also results in reduced
									frequency resolution)
		\param[in] window			to be applied
		\param[in] moduloPhase		indicating whether or not the phase data should
									be limited to +/-180 deg
		\param[out] amplitude		containing the FRF amplitude data <b>[decibels]</b>
		\param[out] phase			containing the FRF phase data <b>[degrees]</b>
		\param[out] coherence		containing the FRF coherence data <b>[unitless]</b>

		\sa ComputeOverlap
	*/
	static void ComputeFRF(const Dataset2D &input, const Dataset2D &output, unsigned int numberOfAverages,
		const FFTWindow &window, const bool &moduloPhase, Dataset2D &amplitude, Dataset2D *phase = NULL, Dataset2D *coherence = NULL);

	/// Computes the coherence function for the specified signals.
	/*!
		\param[in] input	signal time history
		\param[in] output	signal time history

		\return The processed coherence vs. frequency information <b>[unitless]</b>
	*/
	static Dataset2D ComputeCoherence(const Dataset2D& input, const Dataset2D& output);

	/// Returns the number of averages to be used given the specified window parameters.
	/*!
		\param[in] windowSize	number of points in each window
		\param[in] overlap		in percent (0.0 to 1.0) between adjacent windows
		\param[in] dataSize		number of points in signal time history

		\return The number of averages
	*/
	static unsigned int GetNumberOfAverages(const unsigned int windowSize,
		const double &overlap, const unsigned int &dataSize);

	/// Returns the percent overlap between adjacent windows, given the specified window parameters.
	/*!
		\param[in] windowSize		number of points in each window
		\param[in] numberOfAverages	to use
		\param[in] dataSize			number of points in signal time history

		\return The percent overlap between adjacent windows
	*/
	static double ComputeOverlap(unsigned int &windowSize,
		unsigned int &numberOfAverages, const unsigned int &dataSize);

	/// Returns a string containing the name of the specified window.
	/*!
		\param[in] window

		\return The name of the window
	*/
	static std::string GetWindowName(const FFTWindow &window);

	/// Returns the largest power of two that is smaller than the specified sample size.
	/*!
		\param[in] sampleSize	the number of points in the sample

		\return The largest allowable power of two for the specified sample size
	*/
	static unsigned int GetMaxPowerOfTwo(const unsigned int &sampleSize);

private:
	/// Applies the specified window to the dataset.
	/*!
		\param[in,out] data	to be windowed
		\param[in] window	to be applied
	*/
	static void ApplyWindow(Dataset2D &data, const FFTWindow &window);

	/// Applies a Hann window to the dataset.
	/*!
		\param[in,out] data	to be windowed
	*/
	static void ApplyHannWindow(Dataset2D &data);

	/// Applies a Hamming window to the dataset.
	/*!
		\param[in,out] data	to be windowed
	*/
	static void ApplyHammingWindow(Dataset2D &data);

	/// Applies a Flat Top window to the dataset.
	/*!
		\param[in,out] data	to be windowed
	*/
	static void ApplyFlatTopWindow(Dataset2D &data);

	/// Applies a Force window to the dataset.
	/*!
		\param[in,out] data	to be windowed
	*/
	//static void ApplyForceWindow(Dataset2D &data);

	/// Applies an Exponential window to the dataset.
	/*!
		\param[in,out] data	to be windowed
	*/
	static void ApplyExponentialWindow(Dataset2D &data);

	static void DoBitReversal(Dataset2D &set);
	static void DoFFT(Dataset2D &temp);

	static void ZeroDataset(Dataset2D &data);
	static Dataset2D GenerateConstantDataset(const double &xValue, const double &yValue, const unsigned int &size);

	/// Computes the cross-power spectrum of two FFTs.
	/*! To be applied to raw (complex) FFT data.

		\param[in] fftIn	complex FFT of input signal
		\param[in] fftOut	complex FFT of output signal

		\return Complex cross power spectrum
	*/
	static Dataset2D ComputeCrossPowerSpectrum(const Dataset2D &fftIn, const Dataset2D &fftOut);

	/// Computes the (auto) power spectrum of two FFTs.
	/*! To be applied to raw (complex) FFT data.

		\param[in] fftIn	complex FFT of input signal
		\param[in] fftOut	complex FFT of output signal

		\return Complex power spectrum
	*/
	static Dataset2D ComputePowerSpectrum(const Dataset2D &fft);

	/// Converts double-sided (symmetric) FFT data to be single-sided.
	/*! Discards the negative frequency data.  To be applied to frequency-
		formatted datasets (not raw complex FFT data).

		\param[in] fullSpectrum	FFT data containing the full negative and
									positive frequency data
		\param[in] preserveDCValue	indicating whether or not the DC point
									should be preserved.  Added to allow FRF
									computation to discard the 0 Hz point and
									allow plotting against a logarithmic axis.

		\return Single-sided FFT data
	*/
	static Dataset2D ConvertDoubleSidedToSingleSided(const Dataset2D &fullSpectrum, const bool &preserveDCValue = true);

	/// Extracts a single window from the specified dataset.
	/*!
		\param[in] data			signal time history
		\param[in] sample		indicating which portion of the data to chop from
								(i.e. if 3 averages are to be used, sample will
								vary from 0 to 2)
		\param[in] windowSize	number of points in each window
		\param[in] overlap		percentage of shared points between adjacent windows
	*/
	static Dataset2D ChopSample(const Dataset2D &data, const unsigned int &sample,
		const unsigned int &windowSize, const double &overlap);

	/// Adds the specified data to the average.
	/*! Also initializes \a average if it has not been initialized previously.
		To be applied to frequency-formatted datasets (not raw complex FFT data).

		\param[in,out] average	signal time history
		\param[in] data			frequency-formatted data to add to the average
		\param[in] count		total number of averages
	*/
	static void AddToAverage(Dataset2D &average, const Dataset2D &data, const unsigned int &count);

	/// Computes the specified amplitude data to decibels.
	/*!
		\param[in,out] fft		processed amplitude vs. frequency FFT data
	*/
	static void ConvertAmplitudeToDecibels(Dataset2D &fft);

	/// Populates the frequency data (x-data) for the specified dataset.
	/*!
		\param[in,out] data		properly sized but otherwise uninitialized dataset
		\param[in] sampleRate	of signal time history <b>[Hz]</b>
	*/
	static void PopulateFrequencyData(Dataset2D &data, const double &sampleRate);

	/// Extracts amplitude data from complex FFT data.
	/*!
		\param[in] rawFFT		complex FFT data
		\param[in] sampleRate	of signal time history <b>[Hz]</b>

		\return Amplitude vs. frequency for specified complex FFT data
	*/
	static Dataset2D GetAmplitudeData(const Dataset2D &rawFFT, const double &sampleRate);

	/// Extracts phase data from complex FFT data.
	/*!
		\param[in] rawFFT		complex FFT data
		\param[in] sampleRate	of signal time history <b>[Hz]</b>

		\return Phase vs. frequency for specified complex FFT data
	*/
	static Dataset2D GetPhaseData(const Dataset2D &rawFFT, const double &sampleRate, const bool &moduloPhase);

	static Dataset2D ComputeRawFFT(const Dataset2D &data, const FFTWindow &window);
	static void InitializeRawFFTDataset(Dataset2D &rawFFT, const Dataset2D &data, const FFTWindow &window);

	/// Element-wise addition of two complex datasets.
	/*! This treats the x-component as the real part and the y-component as the
		imaginary part of each element.  This differs from the Dataset2D
		arithmetic methods, which only operate on the y-components.

		\param[in] a
		\param[in] b

		\return Sum of the input arguments
	*/
	static Dataset2D ComplexAdd(const Dataset2D &a, const Dataset2D &b);

	/// Element-wise multiplication of two complex datasets.
	/*! This treats the x-component as the real part and the y-component as the
		imaginary part of each element.  This differs from the Dataset2D
		arithmetic methods, which only operate on the y-components.

		\param[in] a
		\param[in] b

		\return Product of the input arguments
	*/
	static Dataset2D ComplexMultiply(const Dataset2D &a, const Dataset2D &b);

	/// Element-wise division of two complex datasets.
	/*! This treats the x-component as the real part and the y-component as the
		imaginary part of each element.  This differs from the Dataset2D
		arithmetic methods, which only operate on the y-components.

		\param[in] a
		\param[in] b

		\return Ratio of the input arguments
	*/
	static Dataset2D ComplexDivide(const Dataset2D &a, const Dataset2D &b);

	/// Determines the magnitude of each complex element.
	/*!
		\param[in] a

		\return Magnitude of each complex element
	*/
	static Dataset2D ComplexMagnitude(const Dataset2D &a);

	/// Element-wise exponentiation of a complex dataset.
	/*! This treats the x-component as the real part and the y-component as the
		imaginary part of each element.  This differs from the Dataset2D
		arithmetic methods, which only operate on the y-components.

		\param[in] a

		\return Input dataset raised to the specified power
	*/
	static Dataset2D ComplexPower(const Dataset2D &a, const double &power);

	static unsigned int ComputeRequiredOverlapPoints(const unsigned int &dataSize,
		const unsigned int &windowSize, const unsigned int &averages);
};

#endif// _FFT_H_