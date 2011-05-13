/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  derivative.h
// Created:  5/3/2011
// Author:  K. Loux
// Description:  Computes discrete-time derivatives of data.
// History:

#ifndef _DERIVATIVE_H_
#define _DERIVATIVE_H_

// Local forward declarations
class Dataset2D;

class DiscreteDerivative
{
public:
	static Dataset2D ComputeTimeHistory(const Dataset2D &data);
};

#endif// _DERIVATIVE_H_