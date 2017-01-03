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

class Axis : public Primitive
{
public:
	explicit Axis(RenderWindow &renderWindow);
	~Axis() = default;

	// Enumeration for the axis orientations
	enum class Orientation
	{
		Bottom,
		Top,
		Left,
		Right
	};

	// Enumeration for the tick styles
	enum class TickStyle
	{
		Through,
		Inside,
		Outside,
		NoTicks
	};

	// Set option methods
	void SetOrientation(const Orientation &orientation) { mOrientation = orientation; mModified = true; }
	void SetMinimum(const double &minimum) { mMinimum = minimum; mModified = true; }
	void SetMaximum(const double &maximum) { mMaximum = maximum; mModified = true; }
	void SetMajorResolution(const double &majorResolution) { mMajorResolution = majorResolution; mModified = true; }
	void SetMinorResolution(const double &minorResolution) { mMinorResolution = minorResolution; mModified = true; }
	void SetMajorGrid(const bool &majorGrid) { mMajorGrid = majorGrid; mModified = true; }
	void SetMinorGrid(const bool &minorGrid) { mMinorGrid = minorGrid; mModified = true; }
	void SetLabel(wxString label) { mLabel = label; mModified = true; }
	bool InitializeFonts(const std::string& fontFileName, const double& size);
	void SetGridColor(const Color &gridColor) { mGridColor = gridColor; mModified = true; }
	void SetTickStyle(const TickStyle &tickStyle) { mTickStyle = tickStyle; mModified = true; }
	void SetTickSize(const int &tickSize) { mTickSize = tickSize; mModified = true; }
	void SetOffsetFromWindowEdge(const unsigned int &offset) { mOffsetFromWindowEdge = offset; mModified = true; }

	void SetAxisAtMinEnd(const Axis *min) { mMinAxis = min; mModified = true; }
	void SetAxisAtMaxEnd(const Axis *max) { mMaxAxis = max; mModified = true; }
	void SetOppositeAxis(const Axis *opposite) { mOppositeAxis = opposite; mModified = true; }

	void SetLogarithmicScale(const bool &log) { mLogarithmic = log; mModified = true; }

	// Get option methods
	inline double GetMinimum() const { return mMinimum; }
	inline double GetMaximum() const { return mMaximum; }
	bool IsHorizontal() const;
	inline unsigned int GetOffsetFromWindowEdge() const { return mOffsetFromWindowEdge; }
	inline bool GetMajorGrid() const { return mMajorGrid; }
	inline bool GetMinorGrid() const { return mMinorGrid; }
	inline Color GetGridColor() const { return mGridColor; }

	inline const Axis* GetAxisAtMinEnd() const { return mMinAxis; }
	inline const Axis* GetAxisAtMaxEnd() const { return mMaxAxis; }
	inline const Axis* GetOppositeAxis() const { return mOppositeAxis; }
	inline Orientation GetOrientation() const { return mOrientation; }

	unsigned int GetAxisLength() const;

	inline wxString GetLabel() const { return mLabel; }

	inline bool IsLogarithmic() const { return mLogarithmic; }

	double ValueToPixel(const double &value) const;
	double PixelToValue(const int &pixel) const;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

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
