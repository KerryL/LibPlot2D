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

/// Class for applying an arbitrary digital filter (transfer function) to data.
class Filter
{
public:
	/// Constructor.
	///
	/// \param sampleRate Frequency at which the digital filter is sampled.
	explicit Filter(const double &sampleRate);

	/// Constructor.
	///
	/// \param sampleRate   Frequency at which the digital filter is sampled.
	/// \param numerator    Coefficients corresponding to the numerator of a
	///                     continuous-time (s-domain) transfer function.
	///                     Coefficients must be list in order of highest power
	///                     to zero power of s.
	/// \param denominator  Coefficients corresponding to the denominator of a
	///                     continuous-time (s-domain) transfer function.
	///                     Coefficients must be list in order of highest power
	///                     to zero power of s.
	/// \param initialValue Initial value of filter input.
	Filter(const double &sampleRate, const std::vector<double> &numerator,
		const std::vector<double> &denominator,
		const double &initialValue = 0.0);

	/// Initializes the filter to the specified value.
	///
	/// \param initialValue Initial value of filter input.
	void Initialize(const double &initialValue);

	/// Applies the filter to the specified data.
	///
	/// \param u0 New input data to filter.
	///
	/// \returns The filtered value computed after considering \p u0.
	double Apply(const double &u0);

	/// Gets the last value passed to Apply().
	/// \returns The last input value to the filter.
	double GetRawValue() const { return mU[0]; }

	/// Gets the last value output from Apply().
	/// \returns the last output value of the filter.
	double GetFilteredValue() const { return mY[0]; }

	/// Extracts sorted numeric polynomial coefficients from the specified
	/// string.  Coefficients are ordered from highest power to zero power.
	///
	/// \param s String containing polynomial expression.
	///
	/// \returns A list of polynomial coefficients extracted from \p s.
	static std::vector<double> CoefficientsFromString(const std::string &s);

	/// Computes the steady-state gain for the specified continuous-time
	/// transfer function.
	///
	/// \param num Coefficients of the numerator of a continuous-time transfer
	///            function.  Coefficients must be listed in order of highest
	///            power to zero power of s.
	/// \param den Coefficients of the denominator of a continuous-time
	///            transfer function.  Coefficients must be listed in order of
	///            highest power to zero power of s.
	///
	/// \returns The steady-state gain for the specified transfer function.
	static double ComputeSteadyStateGain(const std::string &num,
		const std::string &den);

	/// Computes the steady-state gain for this filter.
	/// \returns the steady-state gain for this filter.
	double ComputeSteadyStateGain() const;

private:
	// Filter coefficients
	std::vector<double> mA;
	std::vector<double> mB;
	std::vector<double> mY;
	std::vector<double> mU;

	const double sampleRate;// [Hz]

	void ResizeArrays(const unsigned int &inSize, const unsigned int &outSize);
	void ShiftArray(std::vector<double>& s) const;

	void GenerateCoefficients(const std::vector<double> &numerator,
		const std::vector<double> &denominator);
	std::string AssembleZExpression(const std::vector<double>& coefficients,
		const unsigned int &highestPower) const;
	static std::vector<std::pair<int, double>> CollectLikeTerms(
		std::vector<std::pair<int, double>> terms);
	static std::vector<std::pair<int, double>> PadMissingTerms(
		std::vector<std::pair<int, double>> terms);
};

}// namespace LibPlot2D

#endif// FILTER_H_
