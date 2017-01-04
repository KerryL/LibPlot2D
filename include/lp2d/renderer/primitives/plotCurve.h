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

/// Object for rendering a Dataset2D.
class PlotCurve : public Primitive
{
public:
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	/// \param data         The data associated with this curve.
	PlotCurve(RenderWindow &renderWindow, const Dataset2D& data);
	explicit PlotCurve(const PlotCurve &plotCurve);

	~PlotCurve() = default;

	/// Sets the width of the curve.
	///
	/// \param size Width of the curve in pixels.
	inline void SetLineSize(const double &size) { mLineSize = size; mModified = true; }

	/// Sets the size of the data marker.
	///
	/// \param size Size of the data marker.
	inline void SetMarkerSize(const double &size) { mMarkerSize = size; mModified = true; }

	/// Sets a flag indicating whether or not to use the higher-quality (but
	/// slower) rendering algorithm.
	///
	/// \param pretty Set to true to use the rendering algorithm with better
	///               anti-aliasing.
	inline void SetPretty(const bool &pretty) { mPretty = pretty; mLine.SetPretty(pretty); mModified = true; }

	/// Binds the curve to the specified x-axis.
	///
	/// \param xAxis Axis to which this curve should be bound.
	inline void BindToXAxis(Axis* xAxis) { mXAxis = xAxis; mModified = true; }

	/// Binds the curve to the specified y-axis.
	///
	/// \param yAxis Axis to which this curve should be bound.
	inline void BindToYAxis(Axis* yAxis) { mYAxis = yAxis; mModified = true; }

	/// Gets the associated y-axis.
	/// \returns The associated y-axis.
	inline Axis* GetYAxis() { return mYAxis; }

	/// Assignment operator overload.
	///
	/// \param plotCurve Curve to assign to this.
	///
	/// \returns Reference to this.
	PlotCurve& operator=(const PlotCurve &plotCurve);

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void Update(const unsigned int& i) override;
	void GenerateGeometry() override;

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
