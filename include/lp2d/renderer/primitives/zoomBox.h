/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  zoomBox.h
// Date:  5/5/2011
// Auth:  K. Loux
// Desc:  Logic for drawing the zoom box as the user moves the mouse.

#ifndef ZOOM_BOX_H_
#define ZOOM_BOX_H_

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/line.h"

namespace LibPlot2D
{

/// Class representing a dynamically drawn rectangular box.  The box is used to
/// identify an area of the plot which will be "zoomed in on."
class ZoomBox : public Primitive
{
public:
	/// Constructor.
	///
	/// \param renderWindow The window that owns this primitive.
	explicit ZoomBox(RenderWindow &renderWindow);
	~ZoomBox() = default;

	/// Sets the location of the fixed corner of the box.
	///
	/// \param xAnchor X location of the anchor.
	/// \param yAnchor Y location of the anchor.
	void SetAnchorCorner(const unsigned int &xAnchor, const unsigned int &yAnchor);

	/// Sets the location of the floating corner of the box.
	///
	/// \param xFloat X location of the floating corner.
	/// \param yFloat Y location of the floating corner.
	void SetFloatingCorner(const unsigned int &xFloat, const unsigned int &yFloat);

	/// \name Private data accessors
	/// @{

	unsigned int GetXAnchor() { return mXAnchor; }
	unsigned int GetYAnchor() { return mYAnchor; }
	unsigned int GetXFloat() { return mXFloat; }
	unsigned int GetYFloat() { return mYFloat; }

	/// @}

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void Update(const unsigned int& i) override;
	void GenerateGeometry() override;

private:
	unsigned int mXAnchor = 0;
	unsigned int mYAnchor = 0;
	unsigned int mXFloat = 0;
	unsigned int mYFloat = 0;

	Line box;
};

}// namespace LibPlot2D

#endif// ZOOM_BOX_H_
