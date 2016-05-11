/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotCursor.h
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Represents an oscilloscope cursor on-screen.
// History:
//  5/12/2011 - Renamed to PlotCursor from Cursor due to conflict in X.h, K. Loux

#ifndef CURSOR_H_
#define CURSOR_H_

// Local headers
#include "renderer/primitives/primitive.h"
#include "renderer/line.h"

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

#endif// CURSOR_H_