/*=============================================================================
                                   LibPlot2D
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

/// Helper class for rendering a line.  Includes options to use OpenGL lines or
/// to use OpenGL triangles.  Using OpenGL triangles results in a prettier line
/// (better anti-aliasing) but may render more slowly.
class Line
{
public:
	/// Constructor.
	///
	/// \param renderWindow Window in which the line will be rendered.
	explicit Line(const RenderWindow& renderWindow);

	/// \name Option setting methods
	/// @{

	/// Sets a flag indicating whether or not we should use triangles or OpenGL
	/// lines to render this object.
	///
	/// \param pretty True to indicate that triangles should be used to render
	///               this object.
	inline void SetPretty(const bool &pretty) { mPretty = pretty; }

	/// Sets the line width.
	///
	/// \param width Line width in pixels.
	inline void SetWidth(const double &width)
	{ assert(width >= 0.0); mHalfWidth = 0.5 * width; }

	/// Sets the line color.
	///
	/// \param color Color of the line.
	inline void SetLineColor(const Color &color) { mLineColor = color; }

	/// Sets the background color.  This is only important when using pretty
	/// lines.  Typically, it is easier to achieve the desired results with
	/// SetBackgroundColorForAlphaFade(), but in some cases it is better to
	/// specify the background color explicitly.
	///
	/// \param color The color of the background.
	inline void SetBackgroundColor(const Color &color)
	{ mBackgroundColor = color; }

	/// Sets the background color to automatically fade from the line color to
	/// transparent.  This is typically preferred over manually calling
	/// SetBackgroundColor().
	inline void SetBackgroundColorForAlphaFade()
	{ mBackgroundColor = mLineColor; mBackgroundColor.SetAlpha(0.0); }

	/// Sets the scale factor for the x-component of the line.
	///
	/// \param scale Scale factor.
	inline void SetXScale(const double& scale)
	{ assert(scale > 0.0); mXScale = scale; }

	/// Sets the scale factor for the y-component of the line.
	///
	/// \param scale Scale factor.
	inline void SetYScale(const double& scale)
	{ assert(scale > 0.0); mYScale = scale; }

	/// Sets the OpenGL rendering type hint.
	///
	/// \param hint Hint to tell the drivers how often to expect this object to
	///             update.
	inline void SetBufferHint(const GLenum& hint) { mHint = hint; }

	/// @}

	/// Enumeration for specifying how the line should be updated.
	enum class UpdateMethod
	{
		Immediate,///< Send to OpenGL immediately.
		Manual///< Caller is responsible for sending to OpenGL.
	};

	/// \name Geometry creation methods
	/// @{

	/// Creates OpenGL buffers and fills them with the appropriate geometry
	/// data.  All options must be set prior to calling.
	///
	/// \param x1               X-ordinate of first end of the line.
	/// \param y1               Y-ordinate of first end of the line.
	/// \param x2               X-ordinate of second end of the line.
	/// \param y2               Y-ordinate of second end of the line.
	/// \param bufferInfo [out] Information regarding populated OpenGL buffers.
	/// \param update           Desired update method.
	void Build(const unsigned int &x1, const unsigned int &y1,
		const unsigned int &x2, const unsigned int &y2,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	/// Creates OpenGL buffers and fills them with the appropriate geometry
	/// data.  All options must be set prior to calling.
	///
	/// \param x1               X-ordinate of first end of the line.
	/// \param y1               Y-ordinate of first end of the line.
	/// \param x2               X-ordinate of second end of the line.
	/// \param y2               Y-ordinate of second end of the line.
	/// \param bufferInfo [out] Information regarding populated OpenGL buffers.
	/// \param update           Desired update method.
	void Build(const double &x1, const double &y1, const double &x2,
		const double &y2, Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	/// Creates OpenGL buffers and fills them with the appropriate geometry
	/// data.  All options must be set prior to calling.  Each point after the
	/// first results in a new line segment (i.e. this creates continuous
	/// lines).
	///
	/// \param points           List of x-y pairs designating the shape of the
	///                         line.
	/// \param bufferInfo [out] Information regarding populated OpenGL buffers.
	/// \param update           Desired update method.
	void Build(
		const std::vector<std::pair<unsigned int, unsigned int>> &points,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	/// Creates OpenGL buffers and fills them with the appropriate geometry
	/// data.  All options must be set prior to calling.  Each point after the
	/// first results in a new line segment (i.e. this creates continuous
	/// lines).
	///
	/// \param points           List of x-y pairs designating the shape of the
	///                         line.
	/// \param bufferInfo [out] Information regarding populated OpenGL buffers.
	/// \param update           Desired update method.
	void Build(const std::vector<std::pair<double, double>> &points,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	/// Creates OpenGL buffers and fills them with the appropriate geometry
	/// data.  All options must be set prior to calling.  Each point after the
	/// first results in a new line segment (i.e. this creates continuous
	/// lines).
	///
	/// \param x                List of x-ordinates of line points.
	/// \param y                List of x-ordinates of line points.
	/// \param bufferInfo [out] Information regarding populated OpenGL buffers.
	/// \param update           Desired update method.
	void Build(const std::vector<double>& x,
		const std::vector<double>& y, Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	/// Creates OpenGL buffers and fills them with the appropriate geometry
	/// data.  All options must be set prior to calling.  For every two points,
	/// one line segment is drawn (i.e. this creates discontinuous lines).
	///
	/// \param points           List of x-y pairs designating the locations of
	///                         the segments.
	/// \param bufferInfo [out] Information regarding populated OpenGL buffers.
	/// \param update           Desired update method.
	void BuildSegments(const std::vector<std::pair<double, double>> &points,
		Primitive::BufferInfo& bufferInfo,
		const UpdateMethod& update = UpdateMethod::Immediate) const;

	/// @}

	/// \name Methods for executing the OpenGL rendering.
	/// @{

	/// To be called after creation of Primitive::BufferInfo objects.  This
	/// method must only be used if geometry was created with a call to Build()
	/// after a call to SetPretty(false).
	///
	/// \param vertexCount Number of vertices in the corresponding BufferInfo
	///                    object.
	static void DoUglyDraw(const unsigned int& vertexCount);

	/// To be called after creation of Primitive::BufferInfo objects.  This
	/// method must only be used if geometry was created with a call to Build()
	/// or BuildSegments() after a call to SetPretty(true).
	///
	/// \param vertexCount Number of vertices in the corresponding BufferInfo
	///                    object.
	static void DoPrettyDraw(const unsigned int& indexCount);

	/// To be called after creation of Primitive::BufferInfo objects.  This
	/// method must only be used if geometry was created with a call to
	/// BuildSegments() after a call to SetPretty(false).
	///
	/// \param vertexCount Number of vertices in the corresponding BufferInfo
	///                    object.
	static void DoUglySegmentDraw(const unsigned int& vertexCount);

	/// @}

private:
	static const double mFadeDistance;
	double mHalfWidth;

	Color mLineColor = Color::ColorBlack;
	Color mBackgroundColor;
	bool mPretty = true;

	double mXScale = 1.0;
	double mYScale = 1.0;

	const RenderWindow& mRenderWindow;

	GLenum mHint;

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
