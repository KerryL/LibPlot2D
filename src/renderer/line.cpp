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
void Line::Draw(const unsigned int &x1, const unsigned int &y1, const unsigned int &x2,
		const unsigned int &y2) const
{
	Draw(static_cast<double>(x1), static_cast<double>(y1), static_cast<double>(x2), static_cast<double>(y2));
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
void Line::Draw(const double &x1, const double &y1, const double &x2, const double &y2) const
{
	double xOffset, yOffset;
	ComputeOffsets(x1, y1, x2, y2, xOffset, yOffset);

	/* Four triangles per segment - two on each side of the core line
	   Triangles need to be drawn in counter clockwise direction
	
	We do this:

	2    4    6
	+----+----+
	|\   |\   |
	| \  | \  |
	|  \ |  \ |
	|   \|   \|
	+----+----+
	1    3    5

	where point 3 is (x1, y1) and point 4 is (x2, y2)
	*/

	glBegin(GL_TRIANGLE_STRIP);

	glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
	glVertex2f(x1 - xOffset, y1 - yOffset);
	glVertex2f(x2 - xOffset, y2 - yOffset);

	glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);

	glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
	glVertex2f(x1 + xOffset, y1 + yOffset);
	glVertex2f(x2 + xOffset, y2 + yOffset);

	glEnd();
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

	std::vector<std::pair<double, double> > offsets(points.size() - 1);
	double xOffset, yOffset;

	/* Draw the line in two passes, first from center to on side, then from
	   center to the other side

	 For each side, we do this:

	2    4
	+----+
	|\   |
	| \  |
	|  \ |
	|   \|
	+----+
	1    3

	where point 1 is [i - 1] and point 3 is [i].  Then we do it again, where
	point 2 is [i - 1] and 4 is [i].
	*/
	glBegin(GL_TRIANGLE_STRIP);

	unsigned int i;
	for (i = 1; i < points.size(); i++)
	{
		ComputeOffsets(points[i - 1].first, points[i - 1].second, points[i].first, points[i].second, xOffset, yOffset);
		offsets[i - 1].first = xOffset;
		offsets[i - 1].second = yOffset;

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i - 1].first, points[i - 1].second);

		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i - 1].first - offsets[i - 1].first, points[i - 1].second + offsets[i - 1].second);

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i].first, points[i].second);
		
		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i].first - offsets[i - 1].first, points[i].second + offsets[i - 1].second);
	}

	glEnd();
	glBegin(GL_TRIANGLE_STRIP);

	for (i = 1; i < points.size(); i++)
	{
		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i - 1].first + offsets[i - 1].first, points[i - 1].second - offsets[i - 1].second);

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i - 1].first, points[i - 1].second);

		glColor4f(backgroundColor.GetRed(), backgroundColor.GetGreen(), backgroundColor.GetBlue(), backgroundColor.GetAlpha());
		glVertex2f(points[i].first + offsets[i - 1].first, points[i].second - offsets[i - 1].second);

		glColor4f(lineColor.GetRed(), lineColor.GetGreen(), lineColor.GetBlue(), lineColor.GetAlpha());
		glVertex2f(points[i].first, points[i].second);
	}

	glEnd();
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
//		xOffset	= const double&
//		yOffset	= const double&
//
// Return Value:
//		None
//
//==========================================================================
void Line::ComputeOffsets(const double &x1, const double &y1, const double &x2,
	const double &y2, double &xOffset, double &yOffset) const
{
	// TODO:  Could improve line endings - instead of drawing them |- to core line,
	//        we could instead "miter" the corners to nicely meet adjacent segments
	if (PlotMath::IsZero(y2 - y1))
	{
		xOffset = 0.0;
		yOffset = halfWidth * PlotMath::Sign(x2 - x1);
	}
	else if (PlotMath::IsZero(x2 - x1))
	{
		xOffset = halfWidth * PlotMath::Sign(y1 - y2);
		yOffset = 0.0;
	}
	else
	{
		double slope = (y2 - y1) / (x2 - x1);

		yOffset = sqrt(halfWidth * halfWidth / (1.0 + slope * slope)) * PlotMath::Sign(x2 - x1);
		xOffset = fabs(slope * yOffset) * PlotMath::Sign(y1 - y2);
	}
}