/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

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
#include "utilities/math/plotMath.h"

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
//		red	= const double& specifying the amount of red in this color (0.0 - 1.0)
//		green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		alpha	= const double& specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Color::Color(const double &red, const double &green, const double &blue, const double &alpha)
{
	Set(red, green, blue, alpha);
	ValidateColor();
}

//==========================================================================
// Class:			Color
// Function:		Color
//
// Description:		Constructor for the Color class.  Sets the class contents
//					as specified by the specified wxColor.
//
// Input Arguments:
//		c	= const wxColor&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Color::Color(const wxColor &c)
{
	Set(c.Red() * 255.0, c.Green() * 255.0, c.Blue() * 255.0, c.Alpha() * 255.0);
	ValidateColor();
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
// Function:		GetHue
//
// Description:		Returns the hue for the color.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Color::GetHue() const
{
	double chroma(GetChroma());
	if (chroma == 0.0)
		return 0.0;// Undefined

	double maxColor = std::max(red, std::max(green, blue));

	if (maxColor == red)
		return fmod((green - blue) / chroma / 6.0, 1.0);
	else if (maxColor == green)
		return fmod(((blue - red) / chroma + 2.0) / 6.0, 1.0);
	else
		return fmod(((red - green) / chroma + 4.0) / 6.0, 1.0);
}

//==========================================================================
// Class:			Color
// Function:		GetSaturation
//
// Description:		Returns the saturation for the color.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Color::GetSaturation() const
{
	const double lum(GetLightness());
	if (lum == 0.0 || lum == 1.0)
		return 0.0;

	return GetChroma() / (1.0 - fabs(2.0 * lum - 1.0));
}

//==========================================================================
// Class:			Color
// Function:		GetLightness
//
// Description:		Returns the lightness for the color (uses Rec. 601 NTSC).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Color::GetLightness() const
{
	return 0.5 * (std::max(red, std::max(green, blue)) + std::min(red, std::min(green, blue)));
}

//==========================================================================
// Class:			Color
// Function:		GetChroma
//
// Description:		Returns the chroma for the color.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double Color::GetChroma() const
{
	return std::max(red, std::max(green, blue)) - std::min(red, std::min(green, blue));
}

//==========================================================================
// Class:			Color
// Function:		SetHSL
//
// Description:		Sets the HSLA values for this color.
//
// Input Arguments:
//		hue		= const double& (0 to 1, indication 0 to 360 deg)
//		sat		= const double&
//		lum		= const double&
//		alpha	= const double& specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::SetHSL(const double &hue, const double &sat, const double &lum, const double &alpha)
{
	double chroma = (1.0 - fabs(2.0 * lum - 1.0)) * sat;
	double huePrime = hue * 6.0;
	double x = chroma * (1.0 - fabs(fmod(huePrime, 2.0) - 1.0));

	if (huePrime < 1.0)
	{
		red = chroma;
		green = x;
		blue = 0.0;
	}
	else if (huePrime < 2.0)
	{
		red = x;
		green = chroma;
		blue = 0.0;
	}
	else if (huePrime < 3.0)
	{
		red = 0.0;
		green = chroma;
		blue = x;
	}
	else if (huePrime < 4.0)
	{
		red = 0.0;
		green = x;
		blue = chroma;
	}
	else if (huePrime < 5.0)
	{
		red = x;
		green = 0.0;
		blue = chroma;
	}
	else
	{
		red = chroma;
		green = 0.0;
		blue = x;
	}

	double m = lum - 0.5 * chroma;
	red += m;
	green += m;
	blue += m;

	this->alpha = alpha;
}

//==========================================================================
// Class:			Color
// Function:		Set
//
// Description:		Sets the RGBA values for this color.
//
// Input Arguments:
//		red		= const double& specifying the amount of red in this color (0.0 - 1.0)
//		green	= const double& specifying the amount of green in this color (0.0 - 1.0)
//		blue	= const double& specifying the amount of blue in this color (0.0 - 1.0)
//		alpha	= double specifying the opacity of this color (0.0 - 1.0)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Color::Set(const double &red, const double &green, const double &blue, const double &alpha)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
	this->alpha = alpha;

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
wxColor Color::ToWxColor() const
{
	wxColor color;
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
void Color::ValidateColor()
{
	if (red < 0.0)
		red = 0.0;
	else if (red > 1.0)
		red = 1.0;

	if (green < 0.0)
		green = 0.0;
	else if (green > 1.0)
		green = 1.0;

	if (blue < 0.0)
		blue = 0.0;
	else if (blue > 1.0)
		blue = 1.0;

	if (alpha < 0.0)
		alpha= 0.0;
	else if (alpha > 1.0)
		alpha = 1.0;
}

//==========================================================================
// Class:			Color
// Function:		GetColorHSL
//
// Description:		Returns a color matching the specified RGBA value.
//
// Input Arguments:
//		hue		= const double&
//		sat	= const double&
//		lum	= const double&
//		alpha	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Color
//
//==========================================================================
Color Color::GetColorHSL(const double &hue, const double &sat, const double &lum, const double &alpha)
{
	Color c;
	c.SetHSL(hue, sat, lum, alpha);
	return c;
}

//==========================================================================
// Class:			Color
// Function:		GetColor
//
// Description:		Returns a color matching the specified RGBA value.
//
// Input Arguments:
//		red		= const double&
//		green	= const double&
//		blue	= const double&
//		alpha	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		Color
//
//==========================================================================
Color Color::GetColor(const double &red, const double &green, const double &blue, const double &alpha)
{
	Color c(red, green, blue, alpha);
	return c;
}