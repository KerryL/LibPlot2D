/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotCurve.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derived from Primitive for creating plot curves objects.

#ifndef PLOT_CURVE_H_
#define PLOT_CURVE_H_

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/utilities/managedList.h"
#include "lp2d/renderer/line.h"

namespace LibPlot2D
{

// Local forward declarations
class Axis;
class Dataset2D;

class PlotCurve : public Primitive
{
public:
	PlotCurve(RenderWindow &renderWindow, const Dataset2D& data);
	PlotCurve(const PlotCurve &plotCurve);

	~PlotCurve() = default;

	inline void SetLineSize(const double &size) { mLineSize = size; mModified = true; }
	inline void SetMarkerSize(const double &size) { mMarkerSize = size; mModified = true; }
	inline void SetPretty(const bool &pretty) { mPretty = pretty; mLine.SetPretty(pretty); mModified = true; }

	// For setting up the plot
	inline void BindToXAxis(Axis* xAxis) { mXAxis = xAxis; mModified = true; }
	inline void BindToYAxis(Axis* yAxis) { mYAxis = yAxis; mModified = true; }

	inline Axis* GetYAxis() { return mYAxis; }

	// Overloaded operators
	PlotCurve& operator=(const PlotCurve &plotCurve);

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

private:
	// The axes with which this object is associated
	Axis *mXAxis = nullptr;
	Axis *mYAxis = nullptr;

	const Dataset2D& mData;

	Line mLine;

	bool mPretty = true;
	double mLineSize = 1.0;
	double mMarkerSize = -1.0;

	bool PointIsValid(const unsigned int &i) const;

	bool NeedsMarkersDrawn() const;
	void BuildMarkers();

	enum class RangeSize
	{
		Small,
		Large,
		Undetermined
	};

	bool RangeIsSmall() const;
	RangeSize XRangeIsSmall() const;
	RangeSize YRangeIsSmall() const;

	double mXScale;
	double mYScale;

	void InitializeMarkerVertexBuffer();
	static std::vector<double> DoLogarithmicScale(
		const std::vector<double>& values);
};

}// namespace LibPlot2D

#endif// PLOT_CURVE_H_
