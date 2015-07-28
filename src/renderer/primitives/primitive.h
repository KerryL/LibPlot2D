/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  primitive.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Base class for creating 3D objects.
// History:

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

// Local headers
#include "renderer/color.h"

// Forward declarations
class RenderWindow;

class Primitive
{
public:
	Primitive(RenderWindow &renderWindow);
	Primitive(const Primitive &primitive);

	virtual ~Primitive();

	// Performs the drawing operations
	void Draw();

	// Called when something is modified to re-create this object with
	// all of the latest information
	virtual void GenerateGeometry() = 0;

	// Checks to see if this object's parameters are valid and allow drawing
	virtual bool HasValidParameters() = 0;

	// Private data accessors
	void SetVisibility(const bool &isVisible);
	void SetColor(const Color &color);
	Color GetColor() { return color; }
	void SetDrawOrder(const unsigned int &drawOrder) { this->drawOrder = drawOrder; }
	void SetModified() { modified = true; }// Forces a re-draw

	bool GetIsVisible() const { return isVisible; }
	unsigned int GetDrawOrder() const { return drawOrder; }

	// Overloaded operators
	Primitive& operator=(const Primitive &primitive);

protected:
	bool isVisible;

	Color color;

	bool modified;

	RenderWindow &renderWindow;

	void EnableAlphaBlending();
	void DisableAlphaBlending();

private:
	// The openGL list index
	unsigned int listIndex;
	unsigned int drawOrder;
};

#endif// PRIMITIVE_H_