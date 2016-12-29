/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  filter.h
// Date:  5/16/2011
// Auth:  K. Loux
// Desc:  Class for digital filters.

#ifndef FILTER_H_
#define FILTER_H_

// Standard C++ headers
#include <vector>
#include <string>

namespace LibPlot2D
{

class Filter
{
public:
	// Constructor
	// NOTE:  Constructor MUST initialize the filter parameters a and b, and the input/output vectors y and u
	explicit Filter(const double &sampleRate);
	Filter(const double &sampleRate, const std::vector<double> &numerator,
		const std::vector<double> &denominator, const double &initialValue = 0.0);

	// Resets all internal variables to initialize the filter to the specified value
	void Initialize(const double &initialValue);

	// Main method for filtering incoming data
	double Apply(const double &u0);

	// Returns latest raw data
	double GetRawValue() const { return u[0]; }

	// Returns latest filtered data
	double GetFilteredValue() const { return y[0]; }

	static std::vector<double> CoefficientsFromString(const std::string &s);

	static double ComputeSteadyStateGain(const std::string &num, const std::string &den);
	double ComputeSteadyStateGain() const;

private:
	// Filter coefficients
	std::vector<double> a;
	std::vector<double> b;
	std::vector<double> y;
	std::vector<double> u;

	const double sampleRate;// [Hz]

	void ResizeArrays(const unsigned int &inSize, const unsigned int &outSize);
	void ShiftArray(std::vector<double>& s) const;

	void GenerateCoefficients(const std::vector<double> &numerator, const std::vector<double> &denominator);
	std::string AssembleZExpression(const std::vector<double>& coefficients, const unsigned int &highestPower) const;
	static std::vector<std::pair<int, double>> CollectLikeTerms(std::vector<std::pair<int, double>> terms);
	static std::vector<std::pair<int, double>> PadMissingTerms(std::vector<std::pair<int, double>> terms);
};

}// namespace LibPlot2D

#endif// FILTER_H_
