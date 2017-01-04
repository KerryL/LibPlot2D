/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  color.h
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Contains class declaration for the Color class.  This class contains
//        RGBA color definitions in the form of doubles for use with wxWidgets
//        as well as OpenGL.

#ifndef COLOR_H_
#define COLOR_H_

// wxWidgets forward declarations
class wxString;
class wxColor;

namespace LibPlot2D
{

/// Class representing colors.  Includes methods for converting between
/// wxWidgets and OpenGL.  Internally uses RGBA color model, but some
/// methods are provided for converting to and from alternative models.
class Color
{
public:
	Color() = default;

	/// Constructor.
	///
	/// \param red   Value of red (0.0 to 1.0).
	/// \param green Value of green (0.0 to 1.0).
	/// \param blue  Value of blue (0.0 to 1.0).
	/// \param alpha Value of alpha (0.0 to 1.0).
	Color(const double &red, const double &green, const double &blue,
		const double &alpha = 1.0);

	/// Constructor.
	///
	/// \param c Color to convert.
	explicit Color(const wxColor &c);

	/// \name Getters
	/// @{

	inline double GetRed() const { return mRed; }
	inline double GetGreen() const { return mGreen; }
	inline double GetBlue() const { return mBlue; }
	inline double GetAlpha()  const { return mAlpha; }

	double GetHue() const;///< 0 to 1 for 0 to 360 deg
	double GetSaturation() const;
	double GetLightness() const;
	double GetChroma() const;

	/// @}

	/// \name Constant colors
	/// @{

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

	/// @}

	/// Sets the color in RGBA-space.
	///
	/// \param red   Value of red (0.0 to 1.0).
	/// \param green Value of green (0.0 to 1.0).
	/// \param blue  Value of blue (0.0 to 1.0).
	/// \param alpha Value of alpha (0.0 to 1.0).
	void Set(const double &red, const double &green, const double &blue,
		const double &alpha = 1.0);

	/// Sets the color in HSL-space.
	///
	/// \param hue   Value of hue (0.0 to 1.0).
	/// \param sat   Value of saturation (0.0 to 1.0).
	/// \param lum   Value of luminance (0.0 to 1.0).
	/// \param alpha Value of alpha (0.0 to 1.0).
	void SetHSL(const double &hue, const double &sat, const double &lum,
		const double &alpha = 1.0);

	/// Sets this to be equivalent to the specified wxColor.
	///
	/// \param color Color to assign to this.
	void Set(const wxColor &color);

	/// Sets the alpha channel.
	///
	/// \param alpha Value to assign to the alpha channel.
	void SetAlpha(const double &alpha);

	/// Converts this to a wxColor.
	/// \returns A wxColor object equivalent to this.
	wxColor ToWxColor() const;

	/// Creates a color object given its RGBA values.
	///
	/// \param red   Value of red (0.0 to 1.0).
	/// \param green Value of green (0.0 to 1.0).
	/// \param blue  Value of blue (0.0 to 1.0).
	/// \param alpha Value of alpha (0.0 to 1.0).
	///
	/// \returns A Color representing the specified RGBA values.
	static Color GetColor(const double &red, const double &green,
		const double &blue, const double &alpha = 1.0);

	/// Creates a color object given its HSL values.
	///
	/// \param hue   Value of hue (0.0 to 1.0).
	/// \param sat   Value of saturation (0.0 to 1.0).
	/// \param lum   Value of luminance (0.0 to 1.0).
	/// \param alpha Value of alpha (0.0 to 1.0).
	///
	/// \returns A Color representing the specified HSL values.
	static Color GetColorHSL(const double &hue, const double &sat,
		const double &lum, const double &alpha = 1.0);

private:
	double mRed = 0.0;
	double mGreen = 0.0;
	double mBlue = 0.0;
	double mAlpha = 1.0;

	// Checks to make sure all values are between 0 and 1 (forces this to be true)
	void ValidateColor();
};

}// namespace LibPlot2D

#endif// COLOR_H_
