/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  axis.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive, this class is used to draw plot axis.
// History:
//	07/30/2012	- Added logarithmically-scalled plotting, K. Loux.

#ifndef _AXIS_H_
#define _AXIS_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;
class FTBBox;

class Axis : public Primitive
{
public:
	// Constructor
	Axis(RenderWindow &_renderWindow);

	// Destructor
	~Axis();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Enumeration for the axis orientations
	enum AxisOrientation
	{
		OrientationBottom,
		OrientationTop,
		OrientationLeft,
		OrientationRight
	};

	// Enumeration for the tick styles
	enum TickStyle
	{
		TickStyleThrough,
		TickStyleInside,
		TickStyleOutside,
		TickStyleNone
	};

	// Set option methods
	void SetOrientation(const AxisOrientation &orientation) { this->orientation = orientation; modified = true; };
	void SetMinimum(const double &minimum) { this->minimum = minimum; modified = true; };
	void SetMaximum(const double &maximum) { this->maximum = maximum; modified = true; };
	void SetMajorResolution(const double &majorResolution) { this->majorResolution = majorResolution; modified = true; };
	void SetMinorResolution(const double &minorResolution) { this->minorResolution = minorResolution; modified = true; };
	void SetMajorGrid(const bool &majorGrid) { this->majorGrid = majorGrid; modified = true; };
	void SetMinorGrid(const bool &minorGrid) { this->minorGrid = minorGrid; modified = true; };
	void SetLabel(wxString label) { this->label = label; modified = true; };
	void SetFont(FTFont *font) { this->font = font; modified = true; };
	void SetGridColor(const Color &gridColor) { this->gridColor = gridColor; modified = true; };
	void SetTickStyle(const TickStyle &tickStyle) { this->tickStyle = tickStyle; modified = true; };
	void SetTickSize(const int &tickSize) { this->tickSize = tickSize; modified = true; };
	void SetOffsetFromWindowEdge(const unsigned int &offset) { offsetFromWindowEdge = offset; modified = true; };

	void SetAxisAtMinEnd(const Axis *min) { minAxis = min; modified = true; };
	void SetAxisAtMaxEnd(const Axis *max) { maxAxis = max; modified = true; };
	void SetOppositeAxis(const Axis *opposite) { oppositeAxis = opposite; modified = true; };

	void SetLogarithmicScale(const bool &log) { logarithmic = log; modified = true; };

	// Get option methods
	inline double GetMinimum(void) const { return minimum; };
	inline double GetMaximum(void) const { return maximum; };
	bool IsHorizontal(void) const;
	inline unsigned int GetOffsetFromWindowEdge(void) const { return offsetFromWindowEdge; };
	inline bool GetMajorGrid(void) const { return majorGrid; };
	inline bool GetMinorGrid(void) const { return minorGrid; };
	inline Color GetGridColor(void) const { return gridColor; };

	inline const Axis* GetAxisAtMinEnd(void) const { return minAxis; };
	inline const Axis* GetAxisAtMaxEnd(void) const { return maxAxis; };
	inline const Axis* GetOppositeAxis(void) const { return oppositeAxis; };

	inline wxString GetLabel(void) const { return label; };

	inline bool IsLogarithmic(void) const { return logarithmic; };

	int ValueToPixel(const double &value) const;
	double PixelToValue(const int &pixel) const;

private:
	// This object's orientation
	AxisOrientation orientation;

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

	// Color of the grid
	Color gridColor;

	// Distance for edge of plot render window to the axis
	unsigned int offsetFromWindowEdge;// [pixles]

	// Pointers to the axes at either end of this axis
	const Axis *minAxis;
	const Axis *maxAxis;
	const Axis *oppositeAxis;

	// The axis label and font
	wxString label;
	FTFont *font;

	void DrawFullAxis(void);
	int ComputeMainAxisLocation(void) const;
	void ComputeGridAndTickCounts(unsigned int &tickCount, unsigned int *gridCount = NULL);
	void DrawMainAxis(const int &mainAxisLocation) const;
	void DrawHorizontalGrid(const unsigned int &count) const;
	void DrawHorizontalTicks(const unsigned int &count, const int &mainAxisLocation) const;
	void DrawVerticalGrid(const unsigned int &count) const;
	void DrawVerticalTicks(const unsigned int &count, const int &mainAxisLocation) const;
	void InitializeTickParameters(int &inside, int &outside, int &sign) const;
	void GetNextLogValue(const bool &first, double &value) const;
	double GetNextTickValue(const bool &first, const bool &last, const unsigned int &tick) const;
	double GetNextGridValue(const unsigned int &tick) const;

	void DrawAxisLabel(void) const;
	void DrawTickLabels(void);

	double GetAxisLabelTranslation(const double &offset, const double &fontHeight) const;
	unsigned int GetPrecision(void) const;
	void ComputeTranslations(const double &value, int &xTranslation, int &yTranslation,
		const FTBBox &boundingBox, const double &offset) const;
};

#endif// _AXIS_H_