/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  line.cpp
// Created:  4/2/2015
// Author:  K. Loux
// Description:  Object representing a line, drawn with triangles faded from line
//               color to background color in order to make the lines prettier, be
//               more consistent from platform to platofrm and to support sub-pixel
//               widths.
// History:

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/line.h"
#include "utilities/math/plotMath.h"
#include "renderer/renderWindow.h"

//==========================================================================
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
//==========================================================================
// Under GTK, still having issues getting thin lines to appear smooth
// Don't want to sacrifice crisp thin lines we can achieve under MSW, so
// we add this #if
#ifdef __WXMSW__
const double Line::fadeDistance(0.05);
#else
const double Line::fadeDistance(0.6);// TODO:  Can this be reduced?
#endif

//==========================================================================
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
//==========================================================================
Line::Line(const RenderWindow& renderWindow) : renderWindow(renderWindow)
{
	pretty = true;
	SetWidth(1.0);
	lineColor = Color::ColorBlack;
	SetBackgroundColorForAlphaFade();

	xScale = 1.0;
	yScale = 1.0;

	bufferInfo.vertexBuffer = NULL;
	bufferInfo.vertexCountModified = false;
}

//==========================================================================
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
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::Build(const unsigned int &x1, const unsigned int &y1,
	const unsigned int &x2, const unsigned int &y2)
{
	Build(static_cast<double>(x1), static_cast<double>(y1),
		static_cast<double>(x2), static_cast<double>(y2));
}

//==========================================================================
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
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::Build(const double &x1, const double &y1,
	const double &x2, const double &y2)
{
	if (pretty)
		DoPrettyDraw(x1, y1, x2, y2);
	else
		DoUglyDraw(x1, y1, x2, y2);
}

//==========================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segments.
//
// Input Arguments:
//		points	= const std::vector<std::pair<unsigned int, unsigned int> >&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::Build(const std::vector<std::pair<unsigned int, unsigned int> > &points)
{
	std::vector<std::pair<double, double> > dPoints(points.size());
	unsigned int i;
	for (i = 0; i < points.size(); i++)
	{
		dPoints[i].first = static_cast<double>(points[i].first);
		dPoints[i].second = static_cast<double>(points[i].second);
	}
	Build(dPoints);
}

//==========================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segments.
//
// Input Arguments:
//		points	= cosnt std::vector<std::pair<double, double> >&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::Build(const std::vector<std::pair<double, double> > &points)
{
	if (points.size() < 2)
		return;

	if (pretty)
		DoPrettyDraw(points);
	else
		DoUglyDraw(points);
}

//==========================================================================
// Class:			Line
// Function:		Build
//
// Description:		Builds the specified line segments.
//
// Input Arguments:
//		x		= const double* const
//		y		= const double* const
//		count	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::Build(const double* const x, const double* const y, const unsigned int& count)
{
	std::vector<std::pair<double, double> > dPoints(count);
	unsigned int i;
	for (i = 0; i < dPoints.size(); i++)
	{
		dPoints[i].first = static_cast<double>(x[i]);
		dPoints[i].second = static_cast<double>(y[i]);
	}
	Build(dPoints);
}

//==========================================================================
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
//==========================================================================
void Line::ComputeOffsets(const double &x1, const double &y1, const double &x2,
	const double &y2, double& dxLine, double& dyLine, double& dxEdge, double& dyEdge) const
{
	double miter(atan2((y2 - y1) / yScale, (x2 - x1) / xScale) + M_PI * 0.5);

	dxLine = halfWidth * cos(miter) * xScale;
	dyLine = halfWidth * sin(miter) * yScale;

	dxEdge = dxLine * (halfWidth + fadeDistance) / halfWidth;
	dyEdge = dyLine * (halfWidth + fadeDistance) / halfWidth;
}

//==========================================================================
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
//==========================================================================
void Line::ComputeOffsets(const double &xPrior, const double &yPrior,
	const double &x, const double &y, const double &xNext, const double &yNext,
	double& dxLine, double& dyLine, double& dxEdge, double& dyEdge) const
{
	const double anglePrior(atan2((y - yPrior) / yScale, (x - xPrior) / xScale));
	const double angleNext(atan2((yNext - y) / yScale, (xNext - x) / xScale));
	double miter(0.5 * (anglePrior + angleNext));
	
	if (fabs(angleNext - anglePrior) < M_PI)
		miter += M_PI * 0.5;
	else
		miter -= M_PI * 0.5;

	double miterLength(halfWidth), fade(fadeDistance);
	const double divisor(sin((M_PI - angleNext + anglePrior) * 0.5));// TODO:  This is wrong
	if (!PlotMath::IsZero(divisor))
	{
		miterLength /= fabs(divisor);
		fade /= fabs(divisor);
	}

	dxLine = miterLength * cos(miter) * xScale;
	dyLine = miterLength * sin(miter) * yScale;

	dxEdge = dxLine * (miterLength + fade) / miterLength;
	dyEdge = dyLine * (miterLength + fade) / miterLength;
}

//==========================================================================
// Class:			Line
// Function:		AllocateBuffer
//
// Description:		Allocated local vertex buffer;
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
//==========================================================================
void Line::AllocateBuffer(const unsigned int& vertexCount)
{
	// TODO:  Do we leak these gl objects?
	glGenVertexArrays(1, &bufferInfo.vertexArrayIndex);
	glGenBuffers(1, &bufferInfo.vertexBufferIndex);

	bufferInfo.vertexCount = vertexCount;
	bufferInfo.vertexBuffer = new float[bufferInfo.vertexCount * (renderWindow.GetVertexDimension() + 4)];
	assert(renderWindow.GetVertexDimension() == 2);
}

//==========================================================================
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
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::DoUglyDraw(const double &x1, const double &y1, const double &x2, const double &y2)
{
	AllocateBuffer(2);

	bufferInfo.vertexBuffer[0] = (float)x1;
	bufferInfo.vertexBuffer[1] = (float)y1;
	bufferInfo.vertexBuffer[2] = (float)x2;
	bufferInfo.vertexBuffer[3] = (float)y2;

	bufferInfo.vertexBuffer[4] = (float)lineColor.GetRed();
	bufferInfo.vertexBuffer[5] = (float)lineColor.GetGreen();
	bufferInfo.vertexBuffer[6] = (float)lineColor.GetBlue();
	bufferInfo.vertexBuffer[7] = (float)lineColor.GetAlpha();

	bufferInfo.vertexBuffer[8] = (float)lineColor.GetRed();
	bufferInfo.vertexBuffer[9] = (float)lineColor.GetGreen();
	bufferInfo.vertexBuffer[10] = (float)lineColor.GetBlue();
	bufferInfo.vertexBuffer[11] = (float)lineColor.GetAlpha();

	glBindVertexArray(bufferInfo.vertexArrayIndex);

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (renderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * renderWindow.GetVertexDimension() * bufferInfo.vertexCount));

	glLineWidth(2.0 * halfWidth);

	glBindVertexArray(0);

	delete[] bufferInfo.vertexBuffer;
	bufferInfo.vertexBuffer = NULL;
}

//==========================================================================
// Class:			Line
// Function:		DoPrettyDraw
//
// Description:		Draws a line using OpenGL triangles.
//
// Input Arguments:
//		x1	= const double&
//		y1	= const double&
//		x2	= const double&
//		y2	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::DoPrettyDraw(const double &x1, const double &y1, const double &x2, const double &y2)
{
	double dxLine, dyLine, dxEdge, dyEdge;
	ComputeOffsets(x1, y1, x2, y2, dxLine, dyLine, dxEdge, dyEdge);

	/* Six triangles per segment - two on each side of the core line, plus two for the line itself
	   Triangles need to be drawn in counter clockwise direction
	
	We do this:

	2    4    6    8
	+----+----+----+
	|\   |\   |\   |
	| \  | \  | \  |
	|  \ |  \ |  \ |
	|   \|   \|   \|
	+----+----+----+
	1    3    5    7

	where the line (x1, y1) to (x2, y2) passes halfway between points (3 and 5) and (4 and 6)
	*/

	AllocateBuffer(8);

	bufferInfo.vertexBuffer[0] = (float)(x1 - dxEdge);
	bufferInfo.vertexBuffer[1] = (float)(y1 - dyEdge);

	bufferInfo.vertexBuffer[2] = (float)(x2 - dxEdge);
	bufferInfo.vertexBuffer[3] = (float)(y2 - dyEdge);

	bufferInfo.vertexBuffer[4] = (float)(x1 - dxLine);
	bufferInfo.vertexBuffer[5] = (float)(y1 - dyLine);

	bufferInfo.vertexBuffer[6] = (float)(x2 - dxLine);
	bufferInfo.vertexBuffer[7] = (float)(y2 - dyLine);

	bufferInfo.vertexBuffer[8] = (float)(x1 + dxLine);
	bufferInfo.vertexBuffer[9] = (float)(y1 + dyLine);

	bufferInfo.vertexBuffer[10] = (float)(x2 + dxLine);
	bufferInfo.vertexBuffer[11] = (float)(y2 + dyLine);

	bufferInfo.vertexBuffer[12] = (float)(x1 + dxEdge);
	bufferInfo.vertexBuffer[13] = (float)(y1 + dyEdge);

	bufferInfo.vertexBuffer[14] = (float)(x2 + dxEdge);
	bufferInfo.vertexBuffer[15] = (float)(y2 + dyEdge);

	bufferInfo.vertexBuffer[16] = (float)backgroundColor.GetRed();
	bufferInfo.vertexBuffer[17] = (float)backgroundColor.GetGreen();
	bufferInfo.vertexBuffer[18] = (float)backgroundColor.GetBlue();
	bufferInfo.vertexBuffer[19] = (float)backgroundColor.GetAlpha();

	bufferInfo.vertexBuffer[20] = (float)backgroundColor.GetRed();
	bufferInfo.vertexBuffer[21] = (float)backgroundColor.GetGreen();
	bufferInfo.vertexBuffer[22] = (float)backgroundColor.GetBlue();
	bufferInfo.vertexBuffer[23] = (float)backgroundColor.GetAlpha();

	bufferInfo.vertexBuffer[24] = (float)lineColor.GetRed();
	bufferInfo.vertexBuffer[25] = (float)lineColor.GetGreen();
	bufferInfo.vertexBuffer[26] = (float)lineColor.GetBlue();
	bufferInfo.vertexBuffer[27] = (float)lineColor.GetAlpha();

	bufferInfo.vertexBuffer[28] = (float)lineColor.GetRed();
	bufferInfo.vertexBuffer[29] = (float)lineColor.GetGreen();
	bufferInfo.vertexBuffer[30] = (float)lineColor.GetBlue();
	bufferInfo.vertexBuffer[31] = (float)lineColor.GetAlpha();

	bufferInfo.vertexBuffer[32] = (float)lineColor.GetRed();
	bufferInfo.vertexBuffer[33] = (float)lineColor.GetGreen();
	bufferInfo.vertexBuffer[34] = (float)lineColor.GetBlue();
	bufferInfo.vertexBuffer[35] = (float)lineColor.GetAlpha();

	bufferInfo.vertexBuffer[36] = (float)lineColor.GetRed();
	bufferInfo.vertexBuffer[37] = (float)lineColor.GetGreen();
	bufferInfo.vertexBuffer[38] = (float)lineColor.GetBlue();
	bufferInfo.vertexBuffer[39] = (float)lineColor.GetAlpha();

	bufferInfo.vertexBuffer[40] = (float)backgroundColor.GetRed();
	bufferInfo.vertexBuffer[41] = (float)backgroundColor.GetGreen();
	bufferInfo.vertexBuffer[42] = (float)backgroundColor.GetBlue();
	bufferInfo.vertexBuffer[43] = (float)backgroundColor.GetAlpha();

	bufferInfo.vertexBuffer[44] = (float)backgroundColor.GetRed();
	bufferInfo.vertexBuffer[45] = (float)backgroundColor.GetGreen();
	bufferInfo.vertexBuffer[46] = (float)backgroundColor.GetBlue();
	bufferInfo.vertexBuffer[47] = (float)backgroundColor.GetAlpha();

	glBindVertexArray(bufferInfo.vertexArrayIndex);

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (renderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * renderWindow.GetVertexDimension() * bufferInfo.vertexCount));

	glBindVertexArray(0);

	delete[] bufferInfo.vertexBuffer;
	bufferInfo.vertexBuffer = NULL;
}

//==========================================================================
// Class:			Line
// Function:		DoUglyDraw
//
// Description:		Draws a line strip using OpenGL lines.
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double> >&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::DoUglyDraw(const std::vector<std::pair<double, double> > &points)
{
	AllocateBuffer(points.size());

	const unsigned int dimension(renderWindow.GetVertexDimension());
	const unsigned int start(points.size() * dimension);
	unsigned int i;
	for (i = 0; i < points.size(); i++)
	{
		bufferInfo.vertexBuffer[i * dimension] = (float)points[i].first;
		bufferInfo.vertexBuffer[i * dimension + 1] = (float)points[i].second;

		bufferInfo.vertexBuffer[start + i * 4] = (float)lineColor.GetRed();
		bufferInfo.vertexBuffer[start + i * 4 + 1] = (float)lineColor.GetGreen();
		bufferInfo.vertexBuffer[start + i * 4 + 2] = (float)lineColor.GetBlue();
		bufferInfo.vertexBuffer[start + i * 4 + 3] = (float)lineColor.GetAlpha();
	}

	glBindVertexArray(bufferInfo.vertexArrayIndex);

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (renderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * renderWindow.GetVertexDimension() * bufferInfo.vertexCount));

	glLineWidth(2.0 * halfWidth);

	glBindVertexArray(0);

	delete[] bufferInfo.vertexBuffer;
	bufferInfo.vertexBuffer = NULL;
}

//==========================================================================
// Class:			Line
// Function:		DoPrettyDraw
//
// Description:		Draws a line strip using OpenGL triangles.
//
// Input Arguments:
//		points	= const std::vector<std::pair<double, double> >&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::DoPrettyDraw(const std::vector<std::pair<double, double> > &points)
{
	struct Offsets
	{
		double dxLine;
		double dyLine;
		double dxEdge;
		double dyEdge;
	};

	std::vector<Offsets> offsets(points.size());

	/* Draw the line in three passes, first from center to one side, then from
	   center to the other side

	 For each side and down the center of the line, we do this:

	2    4
	+----+
	|\   |
	| \  | -> Direction of strip
	|  \ |
	|   \|
	+----+
	1    3
	*/

	AllocateBuffer(points.size() * 6);// TODO:  Can we pack the vertices differently to recude this to * 4?  Or use a custom shader?

	const unsigned int dimension(renderWindow.GetVertexDimension());
	const unsigned int coordinatesPerStripe(points.size() * dimension * 2);
	const unsigned int colorValuesPerStripe(coordinatesPerStripe * 2);
	const unsigned int colorStartLeft(coordinatesPerStripe * 3);
	unsigned int i;
	for (i = 0; i < points.size(); i++)
	{
		if (i == 0)
			ComputeOffsets(points[i].first, points[i].second, points[i + 1].first,
				points[i + 1].second, offsets[i].dxLine, offsets[i].dyLine,
				offsets[i].dxEdge, offsets[i].dyEdge);
		else if (i == points.size() - 1)
			ComputeOffsets(points[i - 1].first, points[i - 1].second, points[i].first,
				points[i].second, offsets[i].dxLine, offsets[i].dyLine,
				offsets[i].dxEdge, offsets[i].dyEdge);
		else
			ComputeOffsets(points[i - 1].first, points[i - 1].second,
				points[i].first, points[i].second, points[i + 1].first,
				points[i + 1].second, offsets[i].dxLine, offsets[i].dyLine,
				offsets[i].dxEdge, offsets[i].dyEdge);

		bufferInfo.vertexBuffer[i * dimension * 2] = (float)(points[i].first - offsets[i].dxLine);
		bufferInfo.vertexBuffer[i * dimension * 2 + 1] = (float)(points[i].second - offsets[i].dyLine);

		bufferInfo.vertexBuffer[i * dimension * 2 + dimension] = (float)(points[i].first - offsets[i].dxEdge);
		bufferInfo.vertexBuffer[i * dimension * 2 + dimension + 1] = (float)(points[i].second - offsets[i].dyEdge);

		bufferInfo.vertexBuffer[colorStartLeft + i * 8] = (float)lineColor.GetRed();
		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 1] = (float)lineColor.GetGreen();
		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 2] = (float)lineColor.GetBlue();
		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 3] = (float)lineColor.GetAlpha();

		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 4] = (float)backgroundColor.GetRed();
		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 5] = (float)backgroundColor.GetGreen();
		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 6] = (float)backgroundColor.GetBlue();
		bufferInfo.vertexBuffer[colorStartLeft + i * 8 + 7] = (float)backgroundColor.GetAlpha();
	}

	const unsigned int colorStartCenter(colorStartLeft + colorValuesPerStripe);
	for (i = 0; i < points.size(); i++)
	{
		bufferInfo.vertexBuffer[coordinatesPerStripe + i * dimension * 2] = (float)(points[i].first - offsets[i].dxLine);
		bufferInfo.vertexBuffer[coordinatesPerStripe + i * dimension * 2 + 1] = (float)(points[i].second - offsets[i].dyLine);

		bufferInfo.vertexBuffer[coordinatesPerStripe + i * dimension * 2 + dimension] = (float)(points[i].first + offsets[i].dxLine);
		bufferInfo.vertexBuffer[coordinatesPerStripe + i * dimension * 2 + dimension + 1] = (float)(points[i].second + offsets[i].dyLine);

		bufferInfo.vertexBuffer[colorStartCenter + i * 8] = (float)lineColor.GetRed();
		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 1] = (float)lineColor.GetGreen();
		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 2] = (float)lineColor.GetBlue();
		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 3] = (float)lineColor.GetAlpha();

		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 4] = (float)lineColor.GetRed();
		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 5] = (float)lineColor.GetGreen();
		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 6] = (float)lineColor.GetBlue();
		bufferInfo.vertexBuffer[colorStartCenter + i * 8 + 7] = (float)lineColor.GetAlpha();
	}

	const unsigned int vertexStartRight(2 * coordinatesPerStripe);
	const unsigned int colorStartRight(colorStartCenter + colorValuesPerStripe);
	for (i = 0; i < points.size(); i++)
	{
		bufferInfo.vertexBuffer[vertexStartRight + i * dimension * 2] = (float)(points[i].first + offsets[i].dxEdge);
		bufferInfo.vertexBuffer[vertexStartRight + i * dimension * 2 + 1] = (float)(points[i].second + offsets[i].dyEdge);

		bufferInfo.vertexBuffer[vertexStartRight + i * dimension * 2 + dimension] = (float)(points[i].first + offsets[i].dxLine);
		bufferInfo.vertexBuffer[vertexStartRight + i * dimension * 2 + dimension + 1] = (float)(points[i].second + offsets[i].dyLine);

		bufferInfo.vertexBuffer[colorStartRight + i * 8] = (float)backgroundColor.GetRed();
		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 1] = (float)backgroundColor.GetGreen();
		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 2] = (float)backgroundColor.GetBlue();
		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 3] = (float)backgroundColor.GetAlpha();

		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 4] = (float)lineColor.GetRed();
		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 5] = (float)lineColor.GetGreen();
		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 6] = (float)lineColor.GetBlue();
		bufferInfo.vertexBuffer[colorStartRight + i * 8 + 7] = (float)lineColor.GetAlpha();
	}

	glBindVertexArray(bufferInfo.vertexArrayIndex);

	glBindBuffer(GL_ARRAY_BUFFER, bufferInfo.vertexBufferIndex);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * bufferInfo.vertexCount * (renderWindow.GetVertexDimension() + 4),
		bufferInfo.vertexBuffer, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(renderWindow.GetPositionLocation());
	glVertexAttribPointer(renderWindow.GetPositionLocation(),
		renderWindow.GetVertexDimension(), GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(renderWindow.GetColorLocation());
	glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * colorStartLeft));

	glBindVertexArray(0);

	delete[] bufferInfo.vertexBuffer;
	bufferInfo.vertexBuffer = NULL;
}

//==========================================================================
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
//==========================================================================
void Line::DoUglyDraw(const unsigned int& vertexCount)
{
	glDrawArrays(GL_LINE_STRIP, 0, vertexCount);
	glLineWidth(1.0f);// TODO:  Better way to do this?
}

//==========================================================================
// Class:			Line
// Function:		DoPrettyDraw
//
// Description:		Draws a line strip using OpenGL triangles.
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
//==========================================================================
void Line::DoPrettyDraw(const unsigned int& vertexCount)
{
	if (vertexCount == 8)
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
	else
	{
		assert(vertexCount % 3 == 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount / 3);
		glDrawArrays(GL_TRIANGLE_STRIP, vertexCount / 3, vertexCount / 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 2 * vertexCount / 3, vertexCount / 3);
	}
}
