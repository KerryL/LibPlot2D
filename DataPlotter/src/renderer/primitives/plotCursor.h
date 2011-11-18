/*===================================================================================
                                    DataPlotter
                         Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotCursor.h
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Represents an oscilloscope cursor on-screen.
// History:
//  5/12/2011 - Renamed to PlotCursor from Cursor due to conflict in X.h, K. Loux

#ifndef _CURSOR_H_
#define _CURSOR_H_

// Local headers
#include "renderer/primitives/primitive.h"

// Local forward declarations
class Axis;

class PlotCursor : public Primitive
{
public:
	// Constructor
	PlotCursor(RenderWindow &_renderWindow, const Axis &_axis);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	void SetValue(const double& _value);
	double GetValue(void) const { return value; };

	bool IsUnder(const unsigned int &pixel);

	// Assignment operator (to avoid Warning C4512 due to const reference member)
	PlotCursor& operator=(const PlotCursor &target);

private:
	// The axis we are associated with (perpendicular to)
	const Axis &axis;

	// Current value where this object meets the axis
	double value;
	unsigned int locationAlongAxis;

	void RescalePoint(unsigned int &point);
};

#endif// _CURSOR_H_