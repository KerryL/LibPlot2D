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
	static Dataset2D Compute(const Dataset2D &data);

private:
	static void DoBitReversal(const unsigned int &fftPoints, Dataset2D &set);
	static void ConvertToMagnitudeFrequency(const unsigned int &fftPoints, const double &sampleRate,
		const Dataset2D &temp, Dataset2D &results);
	static void DoFFT(const unsigned int &powerOfTwo, const unsigned int &fftPoints, Dataset2D &temp);
};

#endif// _FFT_H_