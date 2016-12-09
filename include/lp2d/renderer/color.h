/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  color.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class declaration for the Color class.  This class contains
//				 RGBA color definitions in the form of doubles for use with wxWidgets
//				 as well as OpenGL.
// History:

#ifndef COLOR_H_
#define COLOR_H_

// wxWidgets forward declarations
class wxString;
class wxColor;

namespace LibPlot2D
{

class Color
{
public:
	Color();
	Color(const double &red, const double &green, const double &blue, const double &alpha = 1.0);
	Color(const wxColor &c);

	inline double GetRed() const { return red; }
	inline double GetGreen() const { return green; }
	inline double GetBlue() const { return blue; }
	inline double GetAlpha()  const { return alpha; }

	double GetHue() const;// 0 to 1 for 0 to 360 deg
	double GetSaturation() const;
	double GetLightness() const;
	double GetChroma() const;

	// Constant colors
	static const Color ColorRed;
	static const Color ColorGreen;
	static const Color ColorBlue;
	static const Color ColorWhite;
	static const Color ColorBlack;
	static const Color ColorYellow;
	static const Color ColorCyan;
	static const Color ColorMagenta;
	static const Color ColorOrange;
	static const Color ColorPink;
	static const Color ColorDrabGreen;
	static const Color ColorPaleGreen;
	static const Color ColorPurple;
	static const Color ColorLightBlue;
	static const Color ColorGray;

	// For setting the value
	void Set(const double &red, const double &green, const double &blue, const double &alpha = 1.0);
	void SetHSL(const double &hue, const double &sat, const double &lum, const double &alpha = 1.0);
	void Set(const wxColor &color);
	void SetAlpha(const double &alpha);

	wxColor ToWxColor() const;

	static Color GetColorHSL(const double &hue, const double &sat, const double &lum, const double &alpha = 1.0);
	static Color GetColor(const double &red, const double &green, const double &blue, const double &alpha = 1.0);

private:
	// The class data
	double red;
	double green;
	double blue;
	double alpha;

	// Checks to make sure all values are between 0 and 1 (forces this to be true)
	void ValidateColor();
};

}// namespace LibPlot2D

#endif// COLOR_H_