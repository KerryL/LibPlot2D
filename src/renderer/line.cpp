/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

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

// wxWidgets headers
#include <wx/glcanvas.h>

// Local headers
#include "renderer/line.h"
#include "utilities/math/plotMath.h"

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
const double Line::fadeDistance(0.6);
#endif

//==========================================================================
// Class:			Line
// Function:		Line
//
// Description:		Constructor for Line class.
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
Line::Line()
{
	pretty = true;
	SetWidth(1.0);
	lineColor = Color::ColorBlack;
	SetBackgroundColorForAlphaFade();
}

//==========================================================================
// Class:			Line
// Function:		Draw
//
// Description:		Draws the specified line segment.
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
void Line::Draw(const unsigned int &x1, const unsigned int &y1,
	const unsigned int &x2, const unsigned int &y2) const
{
	Draw(static_cast<double>(x1), static_cast<double>(y1),
		static_cast<double>(x2), static_cast<double>(y2));
}

//==========================================================================
// Class:			Line
// Function:		Draw
//
// Description:		Draws the specified line segment.
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
void Line::Draw(const double &x1, const double &y1,
	const double &x2, const double &y2) const
{
	if (pretty)
		DoPrettyDraw(x1, y1, x2, y2);
	else
		DoUglyDraw(x1, y1, x2, y2);
}

//==========================================================================
// Class:			Line
// Function:		Draw
//
// Description:		Draws the specified line segments.
//
// Input Arguments:
//		points	= cosnt std::vector<std::pair<unsigned int, unsigned int> >&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Line::Draw(const std::vector<std::pair<unsigned int, unsigned int> > &points) const
{
	std::vector<std::pair<double, double> > dPoints(points.size());
	unsigned int i;
	for (i = 0; i < points.size(); i++)
	{
		dPoints[i].first = static_cast<double>(points[i].first);
		dPoints[i].second = static_cast<double>(points[i].second);
	}
	Draw(dPoints);
}

//==========================================================================
// Class:			Line
// Function:		Draw
//
// Description:		Draws the specified line segments.
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
void Line::Draw(const std::vector<std::pair<double, double> > &points) const
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
	// TODO:  Could improve line endings - instead of drawing them |- to core line,
	//        we could instead "miter" the corners to nicely meet adjacent segments
	if (PlotMath::IsZero(y2 - y1))
	{
		dxLine = 0.0;
		dyLine = halfWidth * PlotMath::Sign(x2 - x1);

		dxEdge = 0.0;
		dyEdge = (halfWidth + fadeDistance) * PlotMath::Sign(x2 - x1);
	}
	else if (PlotMath::IsZero(x2 - x1))
	{
		dxLine = halfWidth * PlotMath::Sign(y1 - y2);
		dyLine = 0.0;

		dxEdge = (halfWidth + fadeDistance) * PlotMath::Sign(y1 - y2);
		dyEdge = 0.0;
	}
	else
	{
		double slope = (y2 - y1) / (x2 - x1);

		dyLine = sqrt(halfWidth * halfWidth / (1.0 + slope * slope)) * PlotMath::Sign(x2 - x1);
		dxLine = fabs(slope * dyLine) * PlotMath::Sign(y1 - y2);

		dxEdge = dxLine * (halfWidth + fadeDistance) / halfWidth;
		dyEdge = dyLine * (halfWidth + fadeDistance) / halfWidth;
	}
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
void Line::DoUglyDraw(const double &x1, const double &y1, const double &x2, const double &y2) const
{
	glLineWidth(2.0 * halfWidth);
	glBegin(GL_LINES);

	glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);

	glEnd();
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
void Line::DoPrettyDraw(const double &x1, const double &y1, const double &x2, const double &y2) const
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

	glBegin(GL_TRIANGLE_STRIP);

	glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
	glVertex2f(x1 - dxEdge, y1 - dyEdge);
	glVertex2f(x2 - dxEdge, y2 - dyEdge);

	glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
	glVertex2f(x1 - dxLine, y1 - dyLine);
	glVertex2f(x2 - dxLine, y2 - dyLine);
	glVertex2f(x1 + dxLine, y1 + dyLine);
	glVertex2f(x2 + dxLine, y2 + dyLine);

	glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
	glVertex2f(x1 + dxEdge, y1 + dyEdge);
	glVertex2f(x2 + dxEdge, y2 + dyEdge);

	glEnd();
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
void Line::DoUglyDraw(const std::vector<std::pair<double, double> > &points) const
{
	glLineWidth(2.0 * halfWidth);
	glBegin(GL_LINE_STRIP);

	glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());

	unsigned int i;
	for (i = 0; i < points.size(); i++)
		glVertex2f(points[i].first, points[i].second);

	glEnd();
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
void Line::DoPrettyDraw(const std::vector<std::pair<double, double> > &points) const
{
	struct Offsets
	{
		double dxLine;
		double dyLine;
		double dxEdge;
		double dyEdge;
	};

	std::vector<Offsets> offsets(points.size() - 1);
	double dxLine, dyLine, dxEdge, dyEdge;

	/* Draw the line in three passes, first from center to on side, then from
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
	glBegin(GL_TRIANGLE_STRIP);

	unsigned int i;
	// Left side blend
	for (i = 1; i < points.size(); i++)
	{
		ComputeOffsets(points[i - 1].first, points[i - 1].second, points[i].first, points[i].second,
			dxLine, dyLine, dxEdge, dyEdge);
		offsets[i - 1].dxLine = dxLine;
		offsets[i - 1].dyLine = dyLine;
		offsets[i - 1].dxEdge = dxEdge;
		offsets[i - 1].dyEdge = dyEdge;

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i - 1].first - offsets[i - 1].dxLine, points[i - 1].second - offsets[i - 1].dyLine);

		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i - 1].first - offsets[i - 1].dxEdge, points[i - 1].second - offsets[i - 1].dyEdge);

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i].first - offsets[i - 1].dxLine, points[i].second - offsets[i - 1].dyLine);
		
		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i].first - offsets[i - 1].dxEdge, points[i].second - offsets[i - 1].dyEdge);
	}

	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());

	// Core line
	for (i = 1; i < points.size(); i++)
	{
		glVertex2f(points[i - 1].first - offsets[i - 1].dxLine, points[i - 1].second - offsets[i - 1].dyLine);
		glVertex2f(points[i - 1].first + offsets[i - 1].dxLine, points[i - 1].second + offsets[i - 1].dyLine);
		glVertex2f(points[i].first - offsets[i - 1].dxLine, points[i].second - offsets[i - 1].dyLine);
		glVertex2f(points[i].first + offsets[i - 1].dxLine, points[i].second + offsets[i - 1].dyLine);
	}

	glEnd();
	glBegin(GL_TRIANGLE_STRIP);

	// Right side blend
	for (i = 1; i < points.size(); i++)
	{
		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i - 1].first + offsets[i - 1].dxEdge, points[i - 1].second + offsets[i - 1].dyEdge);

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i - 1].first + offsets[i - 1].dxLine, points[i - 1].second + offsets[i - 1].dyLine);

		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i].first + offsets[i - 1].dxEdge, points[i].second + offsets[i - 1].dyEdge);
		
		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i].first + offsets[i - 1].dxLine, points[i].second + offsets[i - 1].dyLine);
	}

	glEnd();
}