/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  primitive.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Base class for creating 3D objects.
// History:

#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

// Local headers
#include "renderer/color_class.h"

// Forward declarations
class RenderWindow;

class Primitive
{
public:
	// Constructor
	Primitive(RenderWindow &_renderWindow);
	Primitive(const Primitive &primitive);

	// Destructor
	virtual ~Primitive();

	// Performs the drawing operations
	void Draw(void);

	// Called when something is modified to re-create this object with
	// all of the latest information
	virtual void GenerateGeometry(void) = 0;

	// Checks to see if this object's parameters are valid and allow drawing
	virtual bool HasValidParameters(void) = 0;

	// Private data accessors
	void SetVisibility(const bool &_isVisible);
	void SetColor(const Color &_color);
	Color GetColor(void) { return color; };

	bool GetIsVisible(void) const { return isVisible; };

	// Overloaded operators
	Primitive& operator = (const Primitive &primitive);

protected:
	// Visibility flag
	bool isVisible;

	// The color variable
	Color color;

	// The "this has been modified" flag
	bool modified;

	// The render window that contains this object
	RenderWindow &renderWindow;

private:
	// The openGL list index
	unsigned int listIndex;
};

#endif// _PRIMITIVE_H_