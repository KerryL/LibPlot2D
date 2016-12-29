/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  line.h
// Date:  4/2/2015
// Auth:  K. Loux
// Desc:  Object representing a line, drawn with triangles faded from line
//        color to background color in order to make the lines prettier, be
//        more consistent from platform to platofrm and to support sub-pixel
//        widths.

#ifndef LINE_H_
#define LINE_H_

// Standard C++ headers
#include <vector>
#include <utility>
#include <cassert>

// Local headers
#include "lp2d/renderer/color.h"
#include "lp2d/renderer/primitives/primitive.h"

namespace LibPlot2D
{

// Local forward declarations
class RenderWindow;

class Line
{
public:
	explicit Line(const RenderWindow& renderWindow);
	virtual ~Line() = default;

	inline void SetPretty(const bool &pretty) { this->pretty = pretty; }
	inline void SetWidth(const double &width) { assert(width >= 0.0); halfWidth = 0.5 * width; }
	inline void SetLineColor(const Color &color) { lineColor = color; }
	inline void SetBackgroundColor(const Color &color) { backgroundColor = color; }
	inline void SetBackgroundColorForAlphaFade() { backgroundColor = lineColor; backgroundColor.SetAlpha(0.0); }

	inline void SetXScale(const double& scale) { assert(scale > 0.0); xScale = scale; }
	inline void SetYScale(const double& scale) { assert(scale > 0.0); yScale = scale; }

	inline void SetBufferHint(const GLenum& hint) { this->hint = hint; }

	enum class UpdateMethod
	{
		Immediate,// Send to OpenGL immediately
		Manual// Caller is responsible for sending to OpenGL
	};

	// Geometry is constructed in Build() call, so all options need to be set prior
	void Build(const unsigned int &x1, const unsigned int &y1,
		const unsigned int &x2, const unsigned int &y2,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;
	void Build(const double &x1, const double &y1, const double &x2,
		const double &y2, Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;
	void Build(
		const std::vector<std::pair<unsigned int, unsigned int>> &points,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;
	void Build(const std::vector<std::pair<double, double>> &points,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;
	void Build(const std::vector<double>& x,
		const std::vector<double>& y, Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;
	void BuildSegments(const std::vector<std::pair<double, double>> &points,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	static void DoUglyDraw(const unsigned int& vertexCount);
	static void DoPrettyDraw(const unsigned int& indexCount);

	static void DoUglySegmentDraw(const unsigned int& vertexCount);

private:
	static const double fadeDistance;
	double halfWidth;

	Color lineColor;
	Color backgroundColor;
	bool pretty;

	double xScale;
	double yScale;

	GLenum hint;

	const RenderWindow& renderWindow;

	void ComputeOffsets(const double &x1, const double &y1, const double &x2,
		const double &y2, double& dxLine, double& dyLine, double& dxEdge,
		double& dyEdge) const;
	void ComputeOffsets(const double &xPrior, const double &yPrior,
		const double &x, const double &y, const double &xNext,
		const double &yNext, double& dxLine, double& dyLine, double& dxEdge,
		double& dyEdge) const;

	struct Offsets
	{
		double dxLine;
		double dyLine;
		double dxEdge;
		double dyEdge;
	};

	void DoUglyDraw(const double &x1, const double &y1, const double &x2,
		const double &y2, const UpdateMethod& update,
		Primitive::BufferInfo& bufferInfo) const;
	void DoUglyDraw(const std::vector<std::pair<double, double>> &points,
		const UpdateMethod& update, Primitive::BufferInfo& bufferInfo) const;
	void DoPrettyDraw(const std::vector<std::pair<double, double>> &points,
		const UpdateMethod& update, Primitive::BufferInfo& bufferInfo) const;

	void DoPrettySegmentDraw(
		const std::vector<std::pair<double, double>> &points,
		const UpdateMethod& update, Primitive::BufferInfo& bufferInfo) const;

	enum class LineStyle
	{
		Continuous,
		Segments
	};

	void AssignVertexData(const std::vector<std::pair<double, double>>& points,
		const LineStyle& style, Primitive::BufferInfo& bufferInfo) const;

	void AllocateBuffer(const unsigned int& vertexCount,
		const unsigned int& triangleCount, Primitive::BufferInfo& bufferInfo) const;
};

}// namespace LibPlot2D

#endif// LINE_H_
