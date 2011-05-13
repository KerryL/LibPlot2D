/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  axis.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from PRIMITVE, this class is used to draw plot axis.
// History:

#ifndef _AXIS_H_
#define _AXIS_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class Axis : public Primitive
{
public:
	// Constructor
	Axis(RenderWindow &_renderWindow);

	// Destructor
	~Axis();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
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
	void SetOrientation(AxisOrientation _orientation) { orientation = _orientation; modified = true; };
	void SetMinimum(double _minimum) { minimum = _minimum; modified = true; };
	void SetMaximum(double _maximum) { maximum = _maximum; modified = true; };
	void SetMajorResolution(double _majorResolution) { majorResolution = _majorResolution; modified = true; };
	void SetMinorResolution(double _minorResolution) { minorResolution = _minorResolution; modified = true; };
	void SetGrid(bool _grid) { grid = _grid; modified = true; };
	void SetLabel(wxString _label) { label = _label; modified = true; };
	void SetFont(FTFont *_font) { font = _font; modified = true; };
	void SetGridColor(Color _gridColor) { gridColor = _gridColor; modified = true; };
	void SetTickStyle(TickStyle _tickStyle) { tickStyle = _tickStyle; modified = true; };
	void SetTickSize(int _tickSize) { tickSize = _tickSize; modified = true; };

	// Get option methods
	double GetMinimum(void) const { return minimum; };
	double GetMaximum(void) const { return maximum; };
	bool IsHorizontal(void) const;
	static unsigned int GetOffsetFromWindowEdge(void) { return offsetFromWindowEdge; };
	bool GetGrid(void) const { return grid; };

private:
	// This object's orientation
	AxisOrientation orientation;

	// This object's range
	double minimum;
	double maximum;

	// For the tick and grid spacing
	double majorResolution;
	double minorResolution;

	// The tick options
	TickStyle tickStyle;
	bool grid;
	int tickSize;

	// Color of the grid
	Color gridColor;

	static const unsigned int offsetFromWindowEdge;

	// The axis label and font
	wxString label;
	FTFont *font;
};

#endif// _AXIS_H_