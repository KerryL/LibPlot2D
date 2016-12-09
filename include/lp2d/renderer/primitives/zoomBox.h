/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  zoomBox.h
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Logic for drawing the zoom box as the user moves the mouse.
// History:

#ifndef ZOOM_BOX_H_
#define ZOOM_BOX_H_

// Local headers
#include "lp2d/renderer/primitives/primitive.h"
#include "lp2d/renderer/line.h"

namespace LibPlot2D
{

class ZoomBox : public Primitive
{
public:
	ZoomBox(RenderWindow &renderWindow);

	void SetAnchorCorner(const unsigned int &xAnchor, const unsigned int &yAnchor);
	void SetFloatingCorner(const unsigned int &xFloat, const unsigned int &yFloat);

	unsigned int GetXAnchor() { return xAnchor; }
	unsigned int GetYAnchor() { return yAnchor; }
	unsigned int GetXFloat() { return xFloat; }
	unsigned int GetYFloat() { return yFloat; }

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

private:
	unsigned int xAnchor, yAnchor, xFloat, yFloat;

	Line box;
};

}// namespace LibPlot2D

#endif// ZOOM_BOX_H_