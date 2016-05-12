/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  multiLine.h
// Created:  5/11/2016
// Author:  K. Loux
// Description:  Object representing a series of unconnected line segments, drawn with
//               triangles faded from line color to background color in order to make
//               the lines prettier, be more consistent from platform to platofrm and
//               to support sub-pixel widths.
// History:

#ifndef MULTI_LINE_H_
#define MULTI_LINE_H_

// Local headers
#include "renderer/line.h"

class MultiLine : public Line
{
public:
	MultiLine(const RenderWindow& renderWindow);
	virtual ~MultiLine() {}

	// Geometry is constructed in Build() call, so all options need to be set prior
	void Build(const std::vector<std::pair<unsigned int, unsigned int> > &points);
	void Build(const std::vector<std::pair<double, double> > &points);

	static void DoUglyDraw(const unsigned int& vertexCount);
	static void DoPrettyDraw(const unsigned int& vertexCount);

private:
	void DoUglyDraw(const std::vector<std::pair<double, double> > &points);
	void DoPrettyDraw(const std::vector<std::pair<double, double> > &points);
};

#endif// LINE_H_
