/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  line.h
// Created:  4/2/2015
// Author:  K. Loux
// Description:  Object representing a line, drawn with triangles faded from line
//               color to background color in order to make the lines prettier, be
//               more consistent from platform to platofrm and to support sub-pixel
//               widths.
// History:

#ifndef LINE_H_
#define LINE_H_

// Standard C++ headers
#include <vector>
#include <utility>
#include <cassert>

// Local headers
#include "renderer/color.h"

class Line
{
public:
	Line();

	inline void SetWidth(const double &width) { assert(width > 0.0); halfWidth = 0.75 * width; }
	inline void SetLineColor(const Color &color) { lineColor = color; }
	inline void SetBackgroundColor(const Color &color) { backgroundColor = color; }
	inline void SetBackgroundColorForAlphaFade() { backgroundColor = lineColor; backgroundColor.SetAlpha(0.0); }

	void Draw(const unsigned int &x1, const unsigned int &y1, const unsigned int &x2,
		const unsigned int &y2) const;
	void Draw(const double &x1, const double &y1, const double &x2, const double &y2) const;
	void Draw(const std::vector<std::pair<unsigned int, unsigned int> > &points) const;
	void Draw(const std::vector<std::pair<double, double> > &points) const;

private:
	double halfWidth;// Due to the fading, setting the half width equal to the width seems to create a nice match for desired line width
	Color lineColor;
	Color backgroundColor;

	void ComputeOffsets(const double &x1, const double &y1, const double &x2,
		const double &y2, double &xOffset, double &yOffset) const;
};

#endif// LINE_H_