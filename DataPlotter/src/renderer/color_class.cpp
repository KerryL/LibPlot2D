/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  color_class.cpp
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
#include "renderer/color_class.h"

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Constructor for the Color class (default).
//
// Input Argurments:
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
	Red = 1.0;
	Green = 1.0;
	Blue = 1.0;
	Alpha = 1.0;
}

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Constructor for the Color class.  Sets the class contents
//					as specified by the arguments.
//
// Input Argurments:
//		_Red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		_Green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		_Blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		_Alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Color::Color(const double &_Red, const double &_Green, const double &_Blue, double _Alpha)
{
	// Assign the arguments to the class members
	Red = _Red;
	Green = _Green;
	Blue =_Blue;
	Alpha = _Alpha;

	ValidateColor();
}

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Destructor for the Color class.
//
// Input Argurments:
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
// Input Argurments:
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
const Color Color::ColorGray(0.5, 0.5, 0.5);

//==========================================================================
// Class:			Color
// Function:		Set
//
// Description:		Sets the RGBA values for this color.
//
// Input Argurments:
//		_Red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		_Green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		_Blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		_Alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::Set(const double &_Red, const double &_Green, const double &_Blue, double _Alpha)
{
	// Assign the arguments to the class members
	Red = _Red;
	Green = _Green;
	Blue =_Blue;
	Alpha = _Alpha;

	ValidateColor();

	return;
}

//==========================================================================
// Class:			Color
// Function:		Set
//
// Description:		Sets the RGBA values for this color.  Overload taking
//					wxColor argument.
//
// Input Argurments:
//		Color	= wxColor& to match
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::Set(const wxColor &Color)
{
	// Convert from the wxColor to Color
	Red = (double)Color.Red() / 255.0;
	Green = (double)Color.Green() / 255.0;
	Blue = (double)Color.Blue() / 255.0;
	Alpha = (double)Color.Alpha() / 255.0;

	ValidateColor();

	return;
}

//==========================================================================
// Class:			Color
// Function:		SetAlpha
//
// Description:		Sets the alpha value for this object.
//
// Input Argurments:
//		_Alpha = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::SetAlpha(const double &_Alpha)
{
	Alpha = _Alpha;

	ValidateColor();

	return;
}

//==========================================================================
// Class:			Color
// Function:		ToWxColor
//
// Description:		Returns a wxColor object that matches this object's color.
//
// Input Argurments:
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
	wxColor Color;

	// Do the conversion to a wxColor object
	Color.Set(char(Red * 255), char(Green * 255), char(Blue * 255), char(Alpha * 255));

	return Color;
}

//==========================================================================
// Class:			Color
// Function:		ValidateColor
//
// Description:		Forces all componenets of the color to be between 0 and 1
//					if they are not already.
//
// Input Argurments:
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
	if (Red < 0.0)
		Red = 0.0;
	else if (Red > 1.0)
		Red = 1.0;

	// Check green
	if (Green < 0.0)
		Green = 0.0;
	else if (Green > 1.0)
		Green = 1.0;

	// Check blue
	if (Blue < 0.0)
		Blue = 0.0;
	else if (Blue > 1.0)
		Blue = 1.0;

	// Check alpha
	if (Alpha < 0.0)
		Alpha= 0.0;
	else if (Alpha > 1.0)
		Alpha = 1.0;

	return;
}