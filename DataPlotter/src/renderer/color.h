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
	Color(const double &_Red, const double &_Green, const double &_Blue, double _Alpha = 1.0);

	// Destructor
	~Color();

	// The color accessors
	inline double GetRed(void) const { return Red; };
	inline double GetGreen(void) const { return Green; };
	inline double GetBlue(void) const { return Blue; };
	inline double GetAlpha(void)  const { return Alpha; };

	// Constant colors
	static const Color ColorRed;
	static const Color ColorGreen;
	static const Color ColorBlue;
	static const Color ColorWhite;
	static const Color ColorBlack;
	static const Color ColorYellow;
	static const Color ColorCyan;
	static const Color ColorMagenta;
	static const Color ColorGray;

	// For setting the value
	void Set(const double &_Red, const double &_Green, const double &_Blue, double _Alpha = 1.0);
	void Set(const wxColor &Color);
	void SetAlpha(const double &_Alpha);

	// Conversion to a wxColor
	wxColor ToWxColor(void) const;

private:
	// The class data
	double Red;
	double Green;
	double Blue;
	double Alpha;

	// Checks to make sure all values are between 0 and 1 (forces this to be true)
	void ValidateColor(void);
};

#endif// _COLOR_H_