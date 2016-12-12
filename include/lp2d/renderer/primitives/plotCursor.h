/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  plotCursor.h
// Date:  5/5/2011
// Auth:  K. Loux
// Desc:  Represents an oscilloscope cursor on-screen.

#ifndef PLOT_CURSOR_H_
#define PLOT_CURSOR_H_

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/line.h"

namespace LibPlot2D
{

// Local forward declarations
class Axis;

class PlotCursor : public Primitive
{
public:
	PlotCursor(RenderWindow &renderWindow, const Axis &axis);

	void SetLocation(const int& location);
	double GetValue() const { return value; }

	bool IsUnder(const unsigned int &pixel);

	// Assignment operator (to avoid Warning C4512 due to const reference member)
	PlotCursor& operator=(const PlotCursor &target);

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

private:
	// The axis we are associated with (perpendicular to)
	const Axis &axis;

	Line line;

	// Current value where this object meets the axis
	double value;
	int locationAlongAxis;
};

}// namespace LibPlot2D

#endif// PLOT_CURSOR_H_
