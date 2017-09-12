/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  axis.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derived from Primitive, this class is used to draw plot axis.

#ifndef AXIS_H_
#define AXIS_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/line.h"
#include "lp2d/renderer/text.h"

namespace LibPlot2D
{

/// Class representing a plot axis scale.
class Axis : public Primitive
{
public:
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	explicit Axis(RenderWindow &renderWindow);
	~Axis() = default;

	/// Axis orientation enumeration.
	enum class Orientation
	{
		Bottom,
		Top,
		Left,
		Right
	};

	/// Style options for rendering axis tick marks.
	enum class TickStyle
	{
		Through,
		Inside,
		Outside,
		NoTicks
	};

	/// \name Set option methods
	/// @{

	void SetOrientation(const Orientation &orientation) { mOrientation = orientation; mModified = true; }
	void SetMinimum(const double &minimum) { mMinimum = minimum; mModified = true; }
	void SetMaximum(const double &maximum) { mMaximum = maximum; mModified = true; }
	void SetMajorResolution(const double &majorResolution) { mMajorResolution = majorResolution; mModified = true; }
	void SetMinorResolution(const double &minorResolution) { mMinorResolution = minorResolution; mModified = true; }
	void SetMajorGrid(const bool &majorGrid) { mMajorGrid = majorGrid; mModified = true; }
	void SetMinorGrid(const bool &minorGrid) { mMinorGrid = minorGrid; mModified = true; }
	void SetLabel(wxString label) { mLabel = label; mModified = true; }
	void SetGridColor(const Color &gridColor) { mGridColor = gridColor; mModified = true; }
	void SetTickStyle(const TickStyle &tickStyle) { mTickStyle = tickStyle; mModified = true; }
	void SetTickSize(const int &tickSize) { mTickSize = tickSize; mModified = true; }
	void SetOffsetFromWindowEdge(const unsigned int &offset) { mOffsetFromWindowEdge = offset; mModified = true; }
	void SetLogarithmicScale(const bool &log) { mLogarithmic = log; mModified = true; }

	/// @}

	/// \name Configuration methods
	/// @{

	/// Must be called exactly once immediately following axis creation.  Sets
	/// the relationship between this axis and the adjacent axis that
	/// intersections this one at this axis' minimum value.
	///
	/// \param min Adjacent axis at the minimum end of this.
	void SetAxisAtMinEnd(const Axis *min) { assert(!mMinAxis); mMinAxis = min; mModified = true; }

	/// Must be called exactly once immediately following axis creation.  Sets
	/// the relationship between this axis and the adjacent axis that
	/// intersections this one at this axis' maximum value.
	///
	/// \param max Adjacent axis at the minimum end of this.
	void SetAxisAtMaxEnd(const Axis *max) { assert(!mMaxAxis); mMaxAxis = max; mModified = true; }

	/// Must be called exactly once immediately following axis creation.  Sets
	/// the relationship between this axis and the parallel axis on the
	/// opposite side of the plot area.
	///
	/// \param opposite Axis at the opposite side of the plot area.
	void SetOppositeAxis(const Axis *opposite) { assert(!mOppositeAxis); mOppositeAxis = opposite; mModified = true; }

	/// Initializes the font objects used to render axis value and label text.
	///
	/// \param fontFileName Path and file name to TrueType font file.
	/// \param size         Size of the text in pixels.
	///
	/// \returns True if initialization was successful.
	bool InitializeFonts(const std::string& fontFileName, const double& size);

	/// @}

	/// \name Get option methods
	/// @{

	inline double GetMinimum() const { return mMinimum; }
	inline double GetMaximum() const { return mMaximum; }
	bool IsHorizontal() const;
	inline unsigned int GetOffsetFromWindowEdge() const { return mOffsetFromWindowEdge; }
	inline bool GetMajorGrid() const { return mMajorGrid; }
	inline bool GetMinorGrid() const { return mMinorGrid; }
	inline Color GetGridColor() const { return mGridColor; }

	inline wxString GetLabel() const { return mLabel; }
	inline bool IsLogarithmic() const { return mLogarithmic; }

	/// Gets the minimum axis value limited by the precision displayed on the plot.
	/// \returns The minimum axis value to display precision.
	double GetPrecisionLimitedMinimum() const;

	/// Gets the maximum axis value limited by the precision displayed on the plot.
	/// \returns The maximum axis value to display precision.
	double GetPrecisionLimitedMaximum() const;

	/// @}

	/// \name Configuration polling methods
	/// @{

	inline const Axis* GetAxisAtMinEnd() const { return mMinAxis; }
	inline const Axis* GetAxisAtMaxEnd() const { return mMaxAxis; }
	inline const Axis* GetOppositeAxis() const { return mOppositeAxis; }
	inline Orientation GetOrientation() const { return mOrientation; }

	/// @}

	/// Gets the length of the axis.
	/// \returns The length of the axis in pixels.
	unsigned int GetAxisLength() const;

	/// Converts from axis units to pixels.
	///
	/// \param value Value in axis units.
	///
	/// \returns Value in pixels.
	double ValueToPixel(const double &value) const;

	/// Converts from pixels to axis units.
	///
	/// \param pixel Value in pixels.
	///
	/// \returns Value in axis units.
	double PixelToValue(const int &pixel) const;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void Update(const unsigned int& i) override;
	void GenerateGeometry() override;

private:
	// This object's orientation
	Orientation mOrientation = Orientation::Bottom;

	// This object's range
	double mMinimum = 0.0;
	double mMaximum = 1.0;

	// For the tick and grid spacing
	double mMajorResolution = 1.0;
	double mMinorResolution = 1.0;

	bool mLogarithmic = false;

	// The tick options
	TickStyle mTickStyle = TickStyle::Through;
	bool mMajorGrid = false;
	bool mMinorGrid = false;
	int mTickSize = 7;

	Color mGridColor;

	// Distance for edge of plot render window to the axis
	unsigned int mOffsetFromWindowEdge = 75;// [pixels]
	
	// Pointers to the axes at either end of this axis
	const Axis *mMinAxis = nullptr;
	const Axis *mMaxAxis = nullptr;
	const Axis *mOppositeAxis = nullptr;

	// The axis label and font
	wxString mLabel;
	Text mLabelText;
	Text mValueText;

	Line mAxisLines;
	Line mGridLines;
	std::vector<std::pair<double, double>> mAxisPoints;
	std::vector<std::pair<double, double>> mGridPoints;

	void DrawFullAxis();
	int ComputeMainAxisLocation() const;
	void ComputeGridAndTickCounts(unsigned int &tickCount, unsigned int *gridCount = nullptr);
	void DrawMainAxis(const int &mainAxisLocation);
	void DrawHorizontalGrid(const unsigned int &count);
	void DrawHorizontalTicks(const unsigned int &count, const int &mainAxisLocation);
	void DrawVerticalGrid(const unsigned int &count);
	void DrawVerticalTicks(const unsigned int &count, const int &mainAxisLocation);
	void InitializeTickParameters(int &inside, int &outside, int &sign) const;
	void GetNextLogValue(const bool &first, double &value) const;
	double GetNextTickValue(const bool &first, const bool &last, const unsigned int &tick) const;
	double GetNextGridValue(const unsigned int &tick) const;

	void DrawAxisLabel();
	void DrawTickLabels();

	double GetAxisLabelTranslation(const double &offset, const double &fontHeight) const;
	unsigned int GetPrecision() const;
	void ComputeTranslations(const double &value, float &xTranslation, float &yTranslation,
		const Text::BoundingBox &boundingBox, const double &offset) const;
};

}// namespace LibPlot2D

#endif// AXIS_H_
