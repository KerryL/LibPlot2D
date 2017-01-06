/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  color.cpp
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Contains class definition for the Color class.  This class contains
//        RGBA color definitions in the form of doubles for use with wxWidgets
//        as well as OpenGL.

// Standard C++ headers
#include <algorithm>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/colour.h>

// Local headers
#include "lp2d/renderer/color.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			Color
// Function:		Color
//
// Description:		Constructor for the Color class.  Sets the class contents
//					as specified by the arguments.  Arguments must lie between
//					0.0 and 1.0.
//
// Input Arguments:
//		red		= const double& specifying the amount of red in this color
//		green	= const double& specifying the amount of green in this color
//		blue	= const double& specifying the amount of blue in this color
//		alpha	= const double& specifying the opacity of this color
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Color::Color(const double &red, const double &green, const double &blue,
	const double &alpha)
{
	Set(red, green, blue, alpha);
	ValidateColor();
}

//=============================================================================
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
//=============================================================================
Color::Color(const wxColor &c)
{
	Set(c.Red() / 255.0, c.Green() / 255.0, c.Blue() / 255.0,
		c.Alpha() / 255.0);
	ValidateColor();
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
double Color::GetHue() const
{
	double chroma(GetChroma());
	if (chroma == 0.0)
		return 0.0;// Undefined

	double maxColor = std::max(mRed, std::max(mGreen, mBlue));

	if (maxColor == mRed)
		return fmod((mGreen - mBlue) / chroma / 6.0, 1.0);
	else if (maxColor == mGreen)
		return fmod(((mBlue - mRed) / chroma + 2.0) / 6.0, 1.0);
	else
		return fmod(((mRed - mGreen) / chroma + 4.0) / 6.0, 1.0);
}

//=============================================================================
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
//=============================================================================
double Color::GetSaturation() const
{
	const double lum(GetLightness());
	if (lum == 0.0 || lum == 1.0)
		return 0.0;

	return GetChroma() / (1.0 - fabs(2.0 * lum - 1.0));
}

//=============================================================================
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
//=============================================================================
double Color::GetLightness() const
{
	return 0.5 * (std::max(mRed, std::max(mGreen, mBlue))
		+ std::min(mRed, std::min(mGreen, mBlue)));
}

//=============================================================================
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
//=============================================================================
double Color::GetChroma() const
{
	return std::max(mRed, std::max(mGreen, mBlue))
		- std::min(mRed, std::min(mGreen, mBlue));
}

//=============================================================================
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
//=============================================================================
void Color::SetHSL(const double &hue, const double &sat, const double &lum,
	const double &alpha)
{
	double chroma = (1.0 - fabs(2.0 * lum - 1.0)) * sat;
	double huePrime = hue * 6.0;
	double x = chroma * (1.0 - fabs(fmod(huePrime, 2.0) - 1.0));

	if (huePrime < 1.0)
	{
		mRed = chroma;
		mGreen = x;
		mBlue = 0.0;
	}
	else if (huePrime < 2.0)
	{
		mRed = x;
		mGreen = chroma;
		mBlue = 0.0;
	}
	else if (huePrime < 3.0)
	{
		mRed = 0.0;
		mGreen = chroma;
		mBlue = x;
	}
	else if (huePrime < 4.0)
	{
		mRed = 0.0;
		mGreen = x;
		mBlue = chroma;
	}
	else if (huePrime < 5.0)
	{
		mRed = x;
		mGreen = 0.0;
		mBlue = chroma;
	}
	else
	{
		mRed = chroma;
		mGreen = 0.0;
		mBlue = x;
	}

	double m = lum - 0.5 * chroma;
	mRed += m;
	mGreen += m;
	mBlue += m;

	mAlpha = alpha;
}

//=============================================================================
// Class:			Color
// Function:		Set
//
// Description:		Sets the RGBA values for this color.
//
// Input Arguments:
//		red		= const double& specifying the amount of red in this color
//		green	= const double& specifying the amount of green in this color
//		blue	= const double& specifying the amount of blue in this color
//		alpha	= const double& specifying the opacity of this color
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Color::Set(const double &red, const double &green, const double &blue,
	const double &alpha)
{
	mRed = red;
	mGreen = green;
	mBlue = blue;
	mAlpha = alpha;

	ValidateColor();
}

//=============================================================================
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
//=============================================================================
void Color::Set(const wxColor &color)
{
	// Convert from the wxColor to Color
	mRed = color.Red() / 255.0;
	mGreen = color.Green() / 255.0;
	mBlue = color.Blue() / 255.0;
	mAlpha = color.Alpha() / 255.0;

	ValidateColor();
}

//=============================================================================
// Class:			Color
// Function:		SetAlpha
//
// Description:		Sets the alpha value for this object.
//
// Input Arguments:
//		alpha = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Color::SetAlpha(const double &alpha)
{
	mAlpha = alpha;
	ValidateColor();
}

//=============================================================================
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
//=============================================================================
wxColor Color::ToWxColor() const
{
	wxColor color;
	color.Set(static_cast<char>(mRed * 255),
		static_cast<char>(mGreen * 255),
		static_cast<char>(mBlue * 255),
		static_cast<char>(mAlpha * 255));

	return color;
}

//=============================================================================
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
//=============================================================================
void Color::ValidateColor()
{
	if (mRed < 0.0)
		mRed = 0.0;
	else if (mRed > 1.0)
		mRed = 1.0;

	if (mGreen < 0.0)
		mGreen = 0.0;
	else if (mGreen > 1.0)
		mGreen = 1.0;

	if (mBlue < 0.0)
		mBlue = 0.0;
	else if (mBlue > 1.0)
		mBlue = 1.0;

	if (mAlpha < 0.0)
		mAlpha= 0.0;
	else if (mAlpha > 1.0)
		mAlpha = 1.0;
}

//=============================================================================
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
//=============================================================================
Color Color::GetColorHSL(const double &hue, const double &sat,
	const double &lum, const double &alpha)
{
	Color c;
	c.SetHSL(hue, sat, lum, alpha);
	return c;
}

//=============================================================================
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
//=============================================================================
Color Color::GetColor(const double &red, const double &green,
	const double &blue, const double &alpha)
{
	Color c(red, green, blue, alpha);
	return c;
}

}// namespace LibPlot2D
