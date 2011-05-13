/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

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
};

#endif// _FFT_H_