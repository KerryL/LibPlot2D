/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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

	void SetLocation(const int& location);
	double GetValue(void) const { return value; };

	bool IsUnder(const unsigned int &pixel);

	// Assignment operator (to avoid Warning C4512 due to const reference member)
	PlotCursor& operator=(const PlotCursor &target);

private:
	// The axis we are associated with (perpendicular to)
	const Axis &axis;

	// Current value where this object meets the axis
	double value;
	int locationAlongAxis;
};

#endif// _CURSOR_H_