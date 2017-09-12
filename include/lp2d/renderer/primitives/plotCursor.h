/*=============================================================================
                                   LibPlot2D
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

/// Class representing an oscilloscope-style cursor line.
class PlotCursor : public Primitive
{
public:
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	/// \param axis         The axis from which cursor location values are
	///                     taken (i.e. the axis to which the cursor is
	///                     perpendicular).
	PlotCursor(RenderWindow &renderWindow, const Axis &axis);
	~PlotCursor() = default;

	/// Sets the location of the cursor along the axis.
	///
	/// \param location The location of the cursor (in pixels).
	void SetLocation(const int& location);

	/// Gets the value at which the cursor crosses the axis.
	/// \returns The value at which the cursor crosses the axis.
	double GetValue() const { return mValue; }

	/// Checks to see if the specified \p pixel is under the cursor.
	///
	/// \param pixel Value to consider.  The direction associated with this
	///              argument is that which is perpendicular to the rendered
	///              cursor.
	///
	/// \returns True if the pixel is beneath the cursor.
	bool IsUnder(const unsigned int &pixel);

	PlotCursor& operator=(const PlotCursor &target) = delete;// To avoid C4512

	/// Updates the value of where the cursor instersects the axis.
	void Recalculate();

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void Update(const unsigned int& i) override;
	void GenerateGeometry() override;

private:
	// The axis we are associated with (perpendicular to)
	const Axis &mAxis;

	Line mLine;

	// Current value where this object meets the axis
	double mValue = -1.0;
	int mLocationAlongAxis;
};

}// namespace LibPlot2D

#endif// PLOT_CURSOR_H_
