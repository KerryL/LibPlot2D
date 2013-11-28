/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  zoomBox.h
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Logic for drawing the zoom box as the user moves the mouse.
// History:

#ifndef _ZOOM_BOX_H_
#define _ZOOM_BOX_H_

// Local headers
#include "renderer/primitives/primitive.h"

class ZoomBox : public Primitive
{
public:
	// Constructor
	ZoomBox(RenderWindow &_renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	void SetAnchorCorner(const unsigned int &_xAnchor, const unsigned int &_yAnchor);
	void SetFloatingCorner(const unsigned int &_xFloat, const unsigned int &_yFloat);

	unsigned int GetXAnchor(void) { return xAnchor; };
	unsigned int GetYAnchor(void) { return yAnchor; };
	unsigned int GetXFloat(void) { return xFloat; };
	unsigned int GetYFloat(void) { return yFloat; };

private:
	unsigned int xAnchor, yAnchor, xFloat, yFloat;
};

#endif// _ZOOM_BOX_H_