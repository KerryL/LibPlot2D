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

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/multiLine.h"
#include "utilities/math/plotMath.h"
#include "renderer/renderWindow.h"

//==========================================================================
// Class:			MultiLine
// Function:		MultiLine
//
// Description:		Constructor for MultiLine class.
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
MultiLine::MultiLine(const RenderWindow& renderWindow) : Line(renderWindow)
{
}

//==========================================================================
// Class:			MultiLine
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
void MultiLine::Build(const std::vector<std::pair<unsigned int, unsigned int> > &points)
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
// Class:			MultiLine
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
void MultiLine::Build(const std::vector<std::pair<double, double> > &points)
{
	if (points.size() < 2)
		return;

	if (pretty)
		DoPrettyDraw(points);
	else
		DoUglyDraw(points);
}

//==========================================================================
// Class:			MultiLine
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
void MultiLine::DoUglyDraw(const std::vector<std::pair<double, double> > &points)
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
// Class:			MultiLine
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
void MultiLine::DoPrettyDraw(const std::vector<std::pair<double, double> > &points)
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
// Class:			MultiLine
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
void MultiLine::DoUglyDraw(const unsigned int& vertexCount)
{
	glDrawArrays(GL_LINES, 0, vertexCount);
	glLineWidth(1.0f);// TODO:  Better way to do this? (prevent all lines after this from being drawn at this line's width)
}

//==========================================================================
// Class:			MultiLine
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
void MultiLine::DoPrettyDraw(const unsigned int& vertexCount)
{
	// TODO:  Instead of deriving from Line, maybe we just contain many lines?
	assert(vertexCount % 3 == 0);
	/*glDrawArrays(GL_TRIANGLES, 0, vertexCount / 3);
	glDrawArrays(GL_TRIANGLES, vertexCount / 3, vertexCount / 3);
	glDrawArrays(GL_TRIANGLES, 2 * vertexCount / 3, vertexCount / 3);*/
}
