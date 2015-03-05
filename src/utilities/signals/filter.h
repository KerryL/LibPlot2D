/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  filter.h
// Created:  5/16/2011
// Author:  K. Loux
// Description:  Class for digital filters.
// History:

#ifndef _FILTER_H_
#define _FILTER_H_

// Standard C++ headers
#include <vector>
#include <string>

class Filter
{
public:
	// Constructor
	// NOTE:  Constructor MUST initialize the filter parameters a and b, and the input/output vectors y and u
	Filter(const double &_sampleRate);
	Filter(const double &_sampleRate, const std::vector<double> &numerator,
		const std::vector<double> &denominator, const double &initialValue = 0.0);
	Filter(const Filter &f);

	// Desctructor
	virtual ~Filter();

	// Resets all internal variables to initialize the filter to the specified value
	void Initialize(const double &initialValue);

	// Main method for filtering incoming data
	double Apply(const double &_u);

	// Returns latest raw data
	double GetRawValue(void) const { return u[0]; };

	// Returns latest filtered data
	double GetFilteredValue(void) const { return y[0]; };

	// Assignment operator (avoids MSVC Warning C4512)
	Filter& operator=(const Filter &f);

	static std::vector<double> CoefficientsFromString(const std::string &s);

	static double ComputeSteadyStateGain(const std::string &num, const std::string &den);
	double ComputeSteadyStateGain() const;

private:
	// Filter coefficients
	double *a;
	double *b;
	double *y;
	double *u;

	unsigned int outSize;
	unsigned int inSize;

	const double sampleRate;// [Hz]

	void AllocateArrays(const unsigned int &_inSize, const unsigned int &_outSize);
	void ShiftArray(double *s, const unsigned int &size);
	void DeleteArrays(void);

	void GenerateCoefficients(const std::vector<double> &numerator, const std::vector<double> &denominator);
	std::string AssembleZExpression(const std::vector<double>& coefficients, const unsigned int &highestPower) const;
	static std::vector<std::pair<int, double> > CollectLikeTerms(std::vector<std::pair<int, double> > terms);
	static std::vector<std::pair<int, double> > PadMissingTerms(std::vector<std::pair<int, double> > terms);
};

#endif// _FILTER_H_