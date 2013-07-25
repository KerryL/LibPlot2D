/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  text.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive, this class is used to draw text.
// History:

#ifndef _TEXT_H_
#define _TEXT_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/primitives/primitive.h"

// FTGL forward declarations
class FTFont;

class TextRendering : public Primitive
{
public:
	// Constructor
	TextRendering(RenderWindow &_renderWindow);

	// Destructor
	~TextRendering();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Set option methods
	void SetAngle(double _angle) { angle = _angle; modified = true; };
	void SetFont(FTFont *_font) { font = _font; modified = true; };
	void SetText(wxString _text) { text = _text; modified = true; };
	void SetPosition(double _x, double _y) { x = _x; y = _y; modified = true; };
	void SetCentered(bool _centered) { centered = _centered; modified = true; };

	double GetTextHeight(void) const;
	double GetTextWidth(void) const;

	wxString GetText(void) const { return text; };

private:
	// The angle at which this text is inclined
	double angle;// 0 is horizontal, angle builds counter-clockwise about an axis out of the screen

	// The actual text content and font
	wxString text;
	FTFont *font;

	// Flag indicating whether the text is centered at (X, Y) or if, if false,
	// (X, Y) represents the lower left corner of the text bounding box
	bool centered;

	// Position of this object in the render window
	double x, y;
};

#endif// _TEXT_H_