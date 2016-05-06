/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

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
#include "renderer/primitives/primitive.h"

// Local forward declarations
class RenderWindow;

class Line
{
public:
	Line(const RenderWindow& renderWindow);

	inline void SetPretty(const bool &pretty) { this->pretty = pretty; }
	inline void SetWidth(const double &width) { assert(width >= 0.0); halfWidth = 0.5 * width; }
	inline void SetLineColor(const Color &color) { lineColor = color; }
	inline void SetBackgroundColor(const Color &color) { backgroundColor = color; }
	inline void SetBackgroundColorForAlphaFade() { backgroundColor = lineColor; backgroundColor.SetAlpha(0.0); }

	inline void SetScale(const double& scale) { assert(scale > 0.0); this->scale = scale; }

	// Geometry is constructed in Build() call, so all options need to be set prior
	void Build(const unsigned int &x1, const unsigned int &y1, const unsigned int &x2,
		const unsigned int &y2);
	void Build(const double &x1, const double &y1, const double &x2, const double &y2);
	void Build(const std::vector<std::pair<unsigned int, unsigned int> > &points);
	void Build(const std::vector<std::pair<double, double> > &points);
	void Build(const double* const x, const double* const y, const unsigned int& count);

	Primitive::BufferInfo GetBufferInfo() const { return bufferInfo; }

	static void DoUglyDraw(const unsigned int& vertexCount);
	static void DoPrettyDraw(const unsigned int& vertexCount);

private:
	static const double fadeDistance;
	double halfWidth;
	Color lineColor;
	Color backgroundColor;
	bool pretty;

	double scale;

	const RenderWindow& renderWindow;
	Primitive::BufferInfo bufferInfo;

	void ComputeOffsets(const double &x1, const double &y1, const double &x2,
		const double &y2, double& dxLine, double& dyLine, double& dxEdge, double& dyEdge) const;
	void ComputeOffsets(const double &xPrior, const double &yPrior,
		const double &x, const double &y, const double &xNext, const double &yNext,
		double& dxLine, double& dyLine, double& dxEdge, double& dyEdge) const;

	struct Offsets
	{
		double dxLine;
		double dyLine;
		double dxEdge;
		double dyEdge;
	};

	void DoUglyDraw(const double &x1, const double &y1, const double &x2, const double &y2);
	void DoPrettyDraw(const double &x1, const double &y1, const double &x2, const double &y2);
	void DoUglyDraw(const std::vector<std::pair<double, double> > &points);
	void DoPrettyDraw(const std::vector<std::pair<double, double> > &points);

	void AllocateBuffer(const unsigned int& vertexCount);
};

#endif// LINE_H_
