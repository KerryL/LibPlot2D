/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  color.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains class declaration for the Color class.  This class contains
//				 RGBA color definitions in the form of doubles for use with wxWidgets
//				 as well as OpenGL.
// History:

#ifndef _COLOR_H_
#define _COLOR_H_

// wxWidgets forward declarations
class wxString;
class wxColor;

class Color
{
public:
	// Constructors
	Color();
	Color(const double &_red, const double &_green, const double &_blue, double _alpha = 1.0);

	// Destructor
	~Color();

	// The color accessors
	inline double GetRed(void) const { return red; };
	inline double GetGreen(void) const { return green; };
	inline double GetBlue(void) const { return blue; };
	inline double GetAlpha(void)  const { return alpha; };

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
	void Set(const double &_red, const double &_green, const double &_blue, double _alpha = 1.0);
	void Set(const wxColor &color);
	void SetAlpha(const double &_alpha);

	// Conversion to a wxColor
	wxColor ToWxColor(void) const;

private:
	// The class data
	double red;
	double green;
	double blue;
	double alpha;

	// Checks to make sure all values are between 0 and 1 (forces this to be true)
	void ValidateColor(void);
};

#endif// _COLOR_H_