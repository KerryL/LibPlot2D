/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  line.cpp
// Date:  4/2/2015
// Auth:  K. Loux
// Desc:  Object representing a line, drawn with triangles faded from line
//        color to background color in order to make the lines prettier, be
//        more consistent from platform to platofrm and to support sub-pixel
//        widths.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/line.h"
#include "lp2d/utilities/math/plotMath.h"
#include "lp2d/renderer/renderWindow.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			Line
// Function:		Constant declarations
//
// Description:		Constant declarations for Line class.
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
const double Line::mFadeDistance(0.05);

//=============================================================================
// Class:			Line
// Function:		Line
//
// Description:		Constructor for Line class.
//
// Input Arguments:
//		renderWindow	= const RenderWindow&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Line::Line(const RenderWindow& renderWindow) : mRenderWindow(renderWindow),
	mHint(GL_DYNAMIC_DRAW)
{
	SetWidth(1.0);
	SetBackgroundColorForAlphaFade();
}

//=============================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segment.
//
// Input Arguments:
//		x1	= const unsigned int&
//		y1	= const unsigned int&
//		x2	= const unsigned int&
//		y2	= const unsigned int&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::Build(const unsigned int &x1, const unsigned int &y1,
	const unsigned int &x2, const unsigned int &y2,
	Primitive::BufferInfo& bufferInfo, const UpdateMethod& update) const
{
	Build(static_cast<double>(x1), static_cast<double>(y1),
		static_cast<double>(x2), static_cast<double>(y2), bufferInfo, update);
}

//=============================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segment.
//
// Input Arguments:
//		x1	= const double&
//		y1	= const double&
//		x2	= const double&
//		y2	= const double&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::Build(const double &x1, const double &y1,const double &x2,
	const double &y2, Primitive::BufferInfo& bufferInfo,
	const UpdateMethod& update) const
{
	if (mPretty)
	{
		std::vector<std::pair<double, double>> v;
		v.push_back(std::make_pair(x1, y1));
		v.push_back(std::make_pair(x2, y2));
		DoPrettyDraw(v, update, bufferInfo);
	}
	else
		DoUglyDraw(x1, y1, x2, y2, update, bufferInfo);
}

//=============================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segments.
//
// Input Arguments:
//		points	= const std::vector<std::pair<unsigned int, unsigned int>>&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::Build(
	const std::vector<std::pair<unsigned int, unsigned int>> &points,
	Primitive::BufferInfo& bufferInfo, const UpdateMethod& update) const
{
	std::vector<std::pair<double, double>> dPoints(points.size());
	unsigned int i;
	for (i = 0; i < points.size(); ++i)
	{
		dPoints[i].first = static_cast<double>(points[i].first);
		dPoints[i].second = static_cast<double>(points[i].second);
	}
	
	Build(dPoints, bufferInfo, update);
}

//=============================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segments.
//
// Input Arguments:
//		points	= cosnt std::vector<std::pair<double, double>>&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::Build(const std::vector<std::pair<double, double>> &points,
	Primitive::BufferInfo& bufferInfo, const UpdateMethod& update) const
{
	if (points.size() < 2)
		return;

	if (mPretty)
		DoPrettyDraw(points, update, bufferInfo);
	else
		DoUglyDraw(points, update, bufferInfo);
}

//=============================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segments.
//
// Input Arguments:
//		x		= const std::vector<double>&
//		y		= const std::vector<double>&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::Build(const std::vector<double>& x, const std::vector<double>& y,
	Primitive::BufferInfo& bufferInfo,const UpdateMethod& update) const
{
	assert(x.size() == y.size());

	std::vector<std::pair<double, double>> dPoints(x.size());
	unsigned int i;
	for (i = 0; i < dPoints.size(); ++i)
	{
		dPoints[i].first = x[i];
		dPoints[i].second = y[i];
	}
	Build(dPoints, bufferInfo, update);
}

//=============================================================================
// Class:			Line
// Function:		BuildSegments
//
// Description:		Builds the specified disconnected line segments.
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double>>
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::BuildSegments(const std::vector<std::pair<double, double>> &points,
	Primitive::BufferInfo& bufferInfo, const UpdateMethod& update) const
{
	if (points.size() == 0)
		return;
	assert(points.size() % 2 == 0);

	if (mPretty)
		DoPrettySegmentDraw(points, update, bufferInfo);
	else
		DoUglyDraw(points, update, bufferInfo);
}

//=============================================================================
// Class:			Line
// Function:		ComputeOffsets
//
// Description:		Computes the offsets for the outside vertices based on the
//					line width and orientation.
//
// Input Arguments:
//		x1	= const double&
//		y1	= const double&
//		x2	= const double&
//		y2	= const double&
//
// Output Arguments:
//		dxLine	= const double&
//		dyLine	= const double&
//		dxEdge	= const double&
//		dyEdge	= const double&
//
// Return Value:
//		None
//
//=============================================================================
void Line::ComputeOffsets(const double &x1, const double &y1, const double &x2,
	const double &y2, double& dxLine, double& dyLine, double& dxEdge, double& dyEdge) const
{
	double miter(atan2((y2 - y1) / mYScale, (x2 - x1) / mXScale) + M_PI * 0.5);

	dxLine = mHalfWidth * cos(miter) * mXScale;
	dyLine = mHalfWidth * sin(miter) * mYScale;

	dxEdge = dxLine * (mHalfWidth + mFadeDistance) / mHalfWidth;
	dyEdge = dyLine * (mHalfWidth + mFadeDistance) / mHalfWidth;
}

//=============================================================================
// Class:			Line
// Function:		ComputeOffsets
//
// Description:		Computes the offsets for the outside vertices based on the
//					line width and orientation.
//
// Input Arguments:
//		xPrior	= const double&
//		yPrior	= const double&
//		x		= const double&
//		y		= const double&
//		xNext	= const double&
//		yNext	= const double&
//
// Output Arguments:
//		dxLine	= const double&
//		dyLine	= const double&
//		dxEdge	= const double&
//		dyEdge	= const double&
//
// Return Value:
//		None
//
//=============================================================================
void Line::ComputeOffsets(const double &xPrior, const double &yPrior,
	const double &x, const double &y, const double &xNext, const double &yNext,
	double& dxLine, double& dyLine, double& dxEdge, double& dyEdge) const
{
	const double anglePrior(atan2((y - yPrior) / mYScale,
		(x - xPrior) / mXScale));
	const double angleNext(atan2((yNext - y) / mYScale,
		(xNext - x) / mXScale));
	double miter(0.5 * (anglePrior + angleNext));
	
	if (fabs(angleNext - anglePrior) < M_PI)
		miter += M_PI * 0.5;
	else
		miter -= M_PI * 0.5;

	double miterLength(mHalfWidth), fade(mFadeDistance);
	const double divisor(sin((M_PI - angleNext + anglePrior) * 0.5));
	if (!PlotMath::IsZero(divisor))
	{
		miterLength /= fabs(divisor);
		fade /= fabs(divisor);
	}

	dxLine = miterLength * cos(miter) * mXScale;
	dyLine = miterLength * sin(miter) * mYScale;

	dxEdge = dxLine * (miterLength + fade) / miterLength;
	dyEdge = dyLine * (miterLength + fade) / miterLength;
}

//=============================================================================
// Class:			Line
// Function:		AllocateBuffer
//
// Description:		Allocated local vertex buffer;
//
// Input Arguments:
//		vertexCount		= const unsigned int&
//		triangleCount	= const unsigned int&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::AllocateBuffer(const unsigned int& vertexCount,
	const unsigned int& triangleCount, Primitive::BufferInfo& bufferInfo) const
{
	bufferInfo.GetOpenGLIndices(triangleCount > 0);

	bufferInfo.vertexCount = vertexCount;
	bufferInfo.vertexBuffer.resize(bufferInfo.vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));
	assert(mRenderWindow.GetVertexDimension() == 2);

	if (triangleCount > 0)
		bufferInfo.indexBuffer.resize(triangleCount * 3);

	//bufferInfo.vertexCountModified = false;// TODO:  OGL4 Would be good to use this, but need a way to trigger update requests
}

//=============================================================================
// Class:			Line
// Function:		DoUglyDraw
//
// Description:		Draws a line using OpenGL lines.
//
// Input Arguments:
//		x1	= const double&
//		y1	= const double&
//		x2	= const double&
//		y2	= const double&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoUglyDraw(const double &x1, const double &y1,
	const double &x2, const double &y2, const UpdateMethod& update,
	Primitive::BufferInfo& bufferInfo) const
{
	AllocateBuffer(2, 0, bufferInfo);

	bufferInfo.vertexBuffer[0] = (float)x1;
	bufferInfo.vertexBuffer[1] = (float)y1;
	bufferInfo.vertexBuffer[2] = (float)x2;
	bufferInfo.vertexBuffer[3] = (float)y2;

	bufferInfo.vertexBuffer[4] = (float)mLineColor.GetRed();
	bufferInfo.vertexBuffer[5] = (float)mLineColor.GetGreen();
	bufferInfo.vertexBuffer[6] = (float)mLineColor.GetBlue();
	bufferInfo.vertexBuffer[7] = (float)mLineColor.GetAlpha();

	bufferInfo.vertexBuffer[8] = (float)mLineColor.GetRed();
	bufferInfo.vertexBuffer[9] = (float)mLineColor.GetGreen();
	bufferInfo.vertexBuffer[10] = (float)mLineColor.GetBlue();
	bufferInfo.vertexBuffer[11] = (float)mLineColor.GetAlpha();

	if (update != UpdateMethod::Immediate)
		return;

	glBindVertexArray(bufferInfo.GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer.data(), mHint);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * bufferInfo.vertexCount));

	glLineWidth(2.0 * mHalfWidth);

	glBindVertexArray(0);

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Line
// Function:		DoUglyDraw
//
// Description:		Draws a line strip using OpenGL lines.
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double>>&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoUglyDraw(const std::vector<std::pair<double, double>> &points,
	const UpdateMethod& update, Primitive::BufferInfo& bufferInfo) const
{
	AllocateBuffer(points.size(), 0, bufferInfo);

	const unsigned int dimension(mRenderWindow.GetVertexDimension());
	const unsigned int start(points.size() * dimension);
	unsigned int i;
	for (i = 0; i < points.size(); ++i)
	{
		bufferInfo.vertexBuffer[i * dimension] = (float)points[i].first;
		bufferInfo.vertexBuffer[i * dimension + 1] = (float)points[i].second;

		bufferInfo.vertexBuffer[start + i * 4] = (float)mLineColor.GetRed();
		bufferInfo.vertexBuffer[start + i * 4 + 1] = (float)mLineColor.GetGreen();
		bufferInfo.vertexBuffer[start + i * 4 + 2] = (float)mLineColor.GetBlue();
		bufferInfo.vertexBuffer[start + i * 4 + 3] = (float)mLineColor.GetAlpha();
	}

	if (update != UpdateMethod::Immediate)
		return;

	glBindVertexArray(bufferInfo.GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer.data(), mHint);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * bufferInfo.vertexCount));

	glLineWidth(2.0 * mHalfWidth);

	glBindVertexArray(0);

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Line
// Function:		DoPrettyDraw
//
// Description:		Draws a line strip using OpenGL triangles.
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double>>&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoPrettyDraw(const std::vector<std::pair<double, double>> &points,
	const UpdateMethod& update, Primitive::BufferInfo& bufferInfo) const
{
	std::vector<Offsets> offsets(points.size());

	/* Draw the line as follows:

	3+----+7
	 |\   |
	 | \  |
	 |  \ |
	 |   \|
	2+----+6
	 |\   |
	 | \  | __> Direction of line
	 |  \ |
	 |   \|
	1+----+5
	 |\   |
	 | \  |
	 |  \ |
	 |   \|
	0+----+4
	*/

	AllocateBuffer(points.size() * 4, 6 * (points.size() - 1), bufferInfo);
	AssignVertexData(points, LineStyle::Continuous, bufferInfo);

	unsigned int i;
	for (i = 0; i < points.size() - 1; ++i)
	{
		bufferInfo.indexBuffer[i * 18] = i * 4;
		bufferInfo.indexBuffer[i * 18 + 1] = i * 4 + 1;
		bufferInfo.indexBuffer[i * 18 + 2] = (i + 1) * 4;

		bufferInfo.indexBuffer[i * 18 + 3] = i * 4 + 1;
		bufferInfo.indexBuffer[i * 18 + 4] = i * 4 + 2;
		bufferInfo.indexBuffer[i * 18 + 5] = (i + 1) * 4 + 1;

		bufferInfo.indexBuffer[i * 18 + 6] = i * 4 + 2;
		bufferInfo.indexBuffer[i * 18 + 7] = i * 4 + 3;
		bufferInfo.indexBuffer[i * 18 + 8] = (i + 1) * 4 + 2;

		bufferInfo.indexBuffer[i * 18 + 9] = i * 4 + 1;
		bufferInfo.indexBuffer[i * 18 + 10] = (i + 1) * 4 + 1;
		bufferInfo.indexBuffer[i * 18 + 11] = (i + 1) * 4;

		bufferInfo.indexBuffer[i * 18 + 12] = i * 4 + 2;
		bufferInfo.indexBuffer[i * 18 + 13] = (i + 1) * 4 + 2;
		bufferInfo.indexBuffer[i * 18 + 14] = (i + 1) * 4 + 1;

		bufferInfo.indexBuffer[i * 18 + 15] = i * 4 + 3;
		bufferInfo.indexBuffer[i * 18 + 16] = (i + 1) * 4 + 3;
		bufferInfo.indexBuffer[i * 18 + 17] = (i + 1) * 4 + 2;
	}

	if (update != UpdateMethod::Immediate)
		return;

	glBindVertexArray(bufferInfo.GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer.data(), mHint);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(),
		mRenderWindow.GetVertexDimension(), GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * 4 * points.size()));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferInfo.GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * bufferInfo.indexBuffer.size(),
		bufferInfo.indexBuffer.data(), mHint);

	glBindVertexArray(0);

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Line
// Function:		DoPrettySegmentDraw
//
// Description:		Draws disconnected lines using OpenGL triangles.
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double>>&
//		update	= const UpdateMethod&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoPrettySegmentDraw(const std::vector<std::pair<double, double>> &points,
	const UpdateMethod& update, Primitive::BufferInfo& bufferInfo) const
{
	assert(!RenderWindow::GLHasError());
	/* Draw the segments as follows:

	3+----+7
	 |\   |
	 | \  |
	 |  \ |
	 |   \|
	2+----+6
	 |\   |
	 | \  | __> Direction of segment
	 |  \ |
	 |   \|
	1+----+5
	 |\   |
	 | \  |
	 |  \ |
	 |   \|
	0+----+4
	*/

	assert(points.size() % 2 == 0);
	AllocateBuffer(points.size() * 4, 6 * (points.size() / 2), bufferInfo);
	AssignVertexData(points, LineStyle::Segments, bufferInfo);

	unsigned int i;
	for (i = 0; i < points.size() / 2; ++i)
	{
		bufferInfo.indexBuffer[i * 18] = i * 8;
		bufferInfo.indexBuffer[i * 18 + 1] = i * 8 + 1;
		bufferInfo.indexBuffer[i * 18 + 2] = i * 8 + 4;

		bufferInfo.indexBuffer[i * 18 + 3] = i * 8 + 1;
		bufferInfo.indexBuffer[i * 18 + 4] = i * 8 + 2;
		bufferInfo.indexBuffer[i * 18 + 5] = i * 8 + 5;

		bufferInfo.indexBuffer[i * 18 + 6] = i * 8 + 2;
		bufferInfo.indexBuffer[i * 18 + 7] = i * 8 + 3;
		bufferInfo.indexBuffer[i * 18 + 8] = i * 8 + 6;

		bufferInfo.indexBuffer[i * 18 + 9] = i * 8 + 1;
		bufferInfo.indexBuffer[i * 18 + 10] = i * 8 + 5;
		bufferInfo.indexBuffer[i * 18 + 11] = i * 8 + 4;

		bufferInfo.indexBuffer[i * 18 + 12] = i * 8 + 2;
		bufferInfo.indexBuffer[i * 18 + 13] = i * 8 + 6;
		bufferInfo.indexBuffer[i * 18 + 14] = i * 8 + 5;

		bufferInfo.indexBuffer[i * 18 + 15] = i * 8 + 3;
		bufferInfo.indexBuffer[i * 18 + 16] = i * 8 + 7;
		bufferInfo.indexBuffer[i * 18 + 17] = i * 8 + 6;
	}

	if (update != UpdateMethod::Immediate)
		return;

	glBindVertexArray(bufferInfo.GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer.data(), mHint);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(),
		mRenderWindow.GetVertexDimension(), GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * 4 * points.size()));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferInfo.GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * bufferInfo.indexBuffer.size(),
		bufferInfo.indexBuffer.data(), mHint);

	glBindVertexArray(0);

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Line
// Function:		AssignVertexData
//
// Description:		Assigns vertex data to the vertex buffer (pretty lines).
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double>>&
//		style	= const LineStyle&
//
// Output Arguments:
//		bufferInfo	= Primitive::BufferInfo&
//
// Return Value:
//		None
//
//=============================================================================
void Line::AssignVertexData(const std::vector<std::pair<double, double>>& points,
	const LineStyle& style, Primitive::BufferInfo& bufferInfo) const
{
	std::vector<Offsets> offsets(points.size());
	const unsigned int dimension(mRenderWindow.GetVertexDimension());
	const unsigned int colorStart(dimension * 4 * points.size());
	unsigned int i;
	for (i = 0; i < points.size(); ++i)
	{
		if (i == 0 || (style == LineStyle::Segments && i % 2 == 0))
			ComputeOffsets(points[i].first, points[i].second, points[i + 1].first,
				points[i + 1].second, offsets[i].dxLine, offsets[i].dyLine,
				offsets[i].dxEdge, offsets[i].dyEdge);
		else if (style == LineStyle::Segments)
			offsets[i] = offsets[i - 1];
		else if (i == points.size() - 1)
			ComputeOffsets(points[i - 1].first, points[i - 1].second, points[i].first,
				points[i].second, offsets[i].dxLine, offsets[i].dyLine,
				offsets[i].dxEdge, offsets[i].dyEdge);
		else
			ComputeOffsets(points[i - 1].first, points[i - 1].second,
				points[i].first, points[i].second, points[i + 1].first,
				points[i + 1].second, offsets[i].dxLine, offsets[i].dyLine,
				offsets[i].dxEdge, offsets[i].dyEdge);

		bufferInfo.vertexBuffer[i * dimension * 4] = (float)(points[i].first + offsets[i].dxEdge);
		bufferInfo.vertexBuffer[i * dimension * 4 + 1] = (float)(points[i].second + offsets[i].dyEdge);

		bufferInfo.vertexBuffer[i * dimension * 4 + dimension] = (float)(points[i].first + offsets[i].dxLine);
		bufferInfo.vertexBuffer[i * dimension * 4 + dimension + 1] = (float)(points[i].second + offsets[i].dyLine);

		bufferInfo.vertexBuffer[i * dimension * 4 + 2 * dimension] = (float)(points[i].first - offsets[i].dxLine);
		bufferInfo.vertexBuffer[i * dimension * 4 + 2 * dimension + 1] = (float)(points[i].second - offsets[i].dyLine);

		bufferInfo.vertexBuffer[i * dimension * 4 + 3 * dimension] = (float)(points[i].first - offsets[i].dxEdge);
		bufferInfo.vertexBuffer[i * dimension * 4 + 3 * dimension + 1] = (float)(points[i].second - offsets[i].dyEdge);

		bufferInfo.vertexBuffer[colorStart + i * 16] = (float)mBackgroundColor.GetRed();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 1] = (float)mBackgroundColor.GetGreen();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 2] = (float)mBackgroundColor.GetBlue();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 3] = (float)mBackgroundColor.GetAlpha();

		bufferInfo.vertexBuffer[colorStart + i * 16 + 4] = (float)mLineColor.GetRed();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 5] = (float)mLineColor.GetGreen();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 6] = (float)mLineColor.GetBlue();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 7] = (float)mLineColor.GetAlpha();

		bufferInfo.vertexBuffer[colorStart + i * 16 + 8] = (float)mLineColor.GetRed();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 9] = (float)mLineColor.GetGreen();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 10] = (float)mLineColor.GetBlue();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 11] = (float)mLineColor.GetAlpha();

		bufferInfo.vertexBuffer[colorStart + i * 16 + 12] = (float)mBackgroundColor.GetRed();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 13] = (float)mBackgroundColor.GetGreen();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 14] = (float)mBackgroundColor.GetBlue();
		bufferInfo.vertexBuffer[colorStart + i * 16 + 15] = (float)mBackgroundColor.GetAlpha();
	}
}

//=============================================================================
// Class:			Line
// Function:		DoUglyDraw
//
// Description:		Draws a line strip using OpenGL lines.
//
// Input Arguments:
//		vertexCount	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoUglyDraw(const unsigned int& vertexCount)
{
	assert(vertexCount > 0);
	glDrawArrays(GL_LINE_STRIP, 0, vertexCount);// TODO:  Memory leak here
	glLineWidth(1.0f);// TODO:  OGL4 Better way to do this? (prevent all lines after this from being drawn at this line's width)  Maybe include in vertex attrib array?
	// Is it better to not even have ugly lines now?

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Line
// Function:		DoUglyDraw
//
// Description:		Draws a line strip using OpenGL lines.
//
// Input Arguments:
//		vertexCount	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoUglySegmentDraw(const unsigned int& vertexCount)
{
	assert(vertexCount > 0);
	glDrawArrays(GL_LINES, 0, vertexCount);
	glLineWidth(1.0f);// TODO:  OGL4 Better way to do this? (prevent all lines after this from being drawn at this line's width)  Maybe include in vertex attrib array?
	// Is it better to not even have ugly lines now?

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			Line
// Function:		DoPrettyDraw
//
// Description:		Draws a line strip using OpenGL triangles.
//
// Input Arguments:
//		indexCount	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Line::DoPrettyDraw(const unsigned int& indexCount)
{
	assert(indexCount > 0);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	assert(!RenderWindow::GLHasError());
}

}// namespace LibPlot2D
