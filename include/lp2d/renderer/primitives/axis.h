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
	void SetOrientation(const Orientation &orientation) { this->orientation = orientation; mModified = true; }
	void SetMinimum(const double &minimum) { this->minimum = minimum; mModified = true; }
	void SetMaximum(const double &maximum) { this->maximum = maximum; mModified = true; }
	void SetMajorResolution(const double &majorResolution) { this->majorResolution = majorResolution; mModified = true; }
	void SetMinorResolution(const double &minorResolution) { this->minorResolution = minorResolution; mModified = true; }
	void SetMajorGrid(const bool &majorGrid) { this->majorGrid = majorGrid; mModified = true; }
	void SetMinorGrid(const bool &minorGrid) { this->minorGrid = minorGrid; mModified = true; }
	void SetLabel(wxString label) { this->label = label; mModified = true; }
	bool InitializeFonts(const std::string& fontFileName, const double& size);
	void SetGridColor(const Color &gridColor) { this->gridColor = gridColor; mModified = true; }
	void SetTickStyle(const TickStyle &tickStyle) { this->tickStyle = tickStyle; mModified = true; }
	void SetTickSize(const int &tickSize) { this->tickSize = tickSize; mModified = true; }
	void SetOffsetFromWindowEdge(const unsigned int &offset) { offsetFromWindowEdge = offset; mModified = true; }

	void SetAxisAtMinEnd(const Axis *min) { minAxis = min; mModified = true; }
	void SetAxisAtMaxEnd(const Axis *max) { maxAxis = max; mModified = true; }
	void SetOppositeAxis(const Axis *opposite) { oppositeAxis = opposite; mModified = true; }

	void SetLogarithmicScale(const bool &log) { logarithmic = log; mModified = true; }

	// Get option methods
	inline double GetMinimum() const { return minimum; }
	inline double GetMaximum() const { return maximum; }
	bool IsHorizontal() const;
	inline unsigned int GetOffsetFromWindowEdge() const { return offsetFromWindowEdge; }
	inline bool GetMajorGrid() const { return majorGrid; }
	inline bool GetMinorGrid() const { return minorGrid; }
	inline Color GetGridColor() const { return gridColor; }

	inline const Axis* GetAxisAtMinEnd() const { return minAxis; }
	inline const Axis* GetAxisAtMaxEnd() const { return maxAxis; }
	inline const Axis* GetOppositeAxis() const { return oppositeAxis; }
	inline Orientation GetOrientation() const { return orientation; }

	unsigned int GetAxisLength() const;

	inline wxString GetLabel() const { return label; }

	inline bool IsLogarithmic() const { return logarithmic; }

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
	Orientation orientation;

	// This object's range
	double minimum;
	double maximum;

	// For the tick and grid spacing
	double majorResolution;
	double minorResolution;

	bool logarithmic;

	// The tick options
	TickStyle tickStyle;
	bool majorGrid;
	bool minorGrid;
	int tickSize;

	Color gridColor;

	// Distance for edge of plot render window to the axis
	unsigned int offsetFromWindowEdge;// [pixels]
	
	// Pointers to the axes at either end of this axis
	const Axis *minAxis;
	const Axis *maxAxis;
	const Axis *oppositeAxis;

	// The axis label and font
	wxString label;
	Text labelText;
	Text valueText;

	Line axisLines;
	Line gridLines;
	std::vector<std::pair<double, double>> axisPoints;
	std::vector<std::pair<double, double>> gridPoints;

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
