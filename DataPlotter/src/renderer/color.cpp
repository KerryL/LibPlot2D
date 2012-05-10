/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  color.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class definition for the Color class.  This class contains
//				 RGBA color definitions in the form of doubles for use with wxWidgets
//				 as well as OpenGL.
// History:

// wxWidgets headers
#include <wx/wx.h>
#include <wx/colour.h>

// Local headers
#include "renderer/color.h"

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Constructor for the Color class (default).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Color::Color()
{
	// Assign default values to the class members (solid black)
	red = 1.0;
	green = 1.0;
	blue = 1.0;
	alpha = 1.0;
}

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Constructor for the Color class.  Sets the class contents
//					as specified by the arguments.
//
// Input Arguments:
//		_red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		_green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		_blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		_alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Color::Color(const double &_red, const double &_green, const double &_blue, double _alpha)
{
	// Assign the arguments to the class members
	red = _red;
	green = _green;
	blue = _blue;
	alpha = _alpha;

	ValidateColor();
}

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Destructor for the Color class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Color::~Color()
{
}

//==========================================================================
// Class:			Color
// Function:		Constant Declarations
//
// Description:		Constants for the Color class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const Color Color::ColorRed(1.0, 0.0, 0.0);
const Color Color::ColorGreen(0.0, 1.0, 0.0);
const Color Color::ColorBlue(0.0, 0.0, 1.0);

const Color Color::ColorWhite(1.0, 1.0, 1.0);
const Color Color::ColorBlack(0.0, 0.0, 0.0);

const Color Color::ColorYellow(1.0, 1.0, 0.0);
const Color Color::ColorCyan(0.0, 1.0, 1.0);
const Color Color::ColorMagenta(1.0, 0.0, 1.0);

const Color Color::ColorOrange(1.0, 0.5, 0.0);
const Color Color::ColorPink(1.0, 0.0, 0.5);

const Color Color::ColorDrabGreen(0.5, 1.0, 0.0);
const Color Color::ColorPaleGreen(0.0, 1.0, 0.5);

const Color Color::ColorPurple(0.5, 0.0, 1.0);
const Color Color::ColorLightBlue(0.0, 0.5, 1.0);

const Color Color::ColorGray(0.5, 0.5, 0.5);

//==========================================================================
// Class:			Color
// Function:		Set
//
// Description:		Sets the RGBA values for this color.
//
// Input Arguments:
//		_red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		_green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		_blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		_alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::Set(const double &_red, const double &_green, const double &_blue, double _alpha)
{
	// Assign the arguments to the class members
	red = _red;
	green = _green;
	blue = _blue;
	alpha = _alpha;

	ValidateColor();
}

//==========================================================================
// Class:			Color
// Function:		Set
//
// Description:		Sets the RGBA values for this color.  Overload taking
//					wxColor argument.
//
// Input Arguments:
//		color	= wxColor& to match
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::Set(const wxColor &color)
{
	// Convert from the wxColor to Color
	red = (double)color.Red() / 255.0;
	green = (double)color.Green() / 255.0;
	blue = (double)color.Blue() / 255.0;
	alpha = (double)color.Alpha() / 255.0;

	ValidateColor();
}

//==========================================================================
// Class:			Color
// Function:		SetAlpha
//
// Description:		Sets the alpha value for this object.
//
// Input Arguments:
//		_alpha = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::SetAlpha(const double &_alpha)
{
	alpha = _alpha;

	ValidateColor();
}

//==========================================================================
// Class:			Color
// Function:		ToWxColor
//
// Description:		Returns a wxColor object that matches this object's color.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxColor that matches this color
//
//==========================================================================
wxColor Color::ToWxColor(void) const
{
	// Return object
	wxColor color;

	// Do the conversion to a wxColor object
	color.Set(char(red * 255), char(green * 255), char(blue * 255), char(alpha * 255));

	return color;
}

//==========================================================================
// Class:			Color
// Function:		ValidateColor
//
// Description:		Forces all components of the color to be between 0 and 1
//					if they are not already.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::ValidateColor(void)
{
	// Check red
	if (red < 0.0)
		red = 0.0;
	else if (red > 1.0)
		red = 1.0;

	// Check green
	if (green < 0.0)
		green = 0.0;
	else if (green > 1.0)
		green = 1.0;

	// Check blue
	if (blue < 0.0)
		blue = 0.0;
	else if (blue > 1.0)
		blue = 1.0;

	// Check alpha
	if (alpha < 0.0)
		alpha= 0.0;
	else if (alpha > 1.0)
		alpha = 1.0;
}