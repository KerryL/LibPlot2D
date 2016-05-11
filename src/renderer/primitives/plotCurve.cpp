/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotCurve.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating data curves on a plot.
// History:
//	11/9/2010	- Modified to accomodate 3D plots, K. Loux.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "renderer/primitives/plotCurve.h"
#include "renderer/renderWindow.h"
#include "renderer/plotRenderer.h"
#include "renderer/primitives/axis.h"
#include "utilities/dataset2D.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			PlotCurve
// Function:		PlotCurve
//
// Description:		Constructor for the PlotCurve class.
//
// Input Arguments:
//		renderWindow	= RenderWindow* pointing to the object that owns this
//		data			= const Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotCurve::PlotCurve(RenderWindow &renderWindow, const Dataset2D& data)
	: Primitive(renderWindow), data(data), line(renderWindow)
{
	xAxis = NULL;
	yAxis = NULL;

	lineSize = 1;
	markerSize = -1;
	pretty = true;

	bufferInfo.push_back(BufferInfo());// Add a second empty info block for the markers
}

//==========================================================================
// Class:			PlotCurve
// Function:		PlotCurve
//
// Description:		Copy constructor for the PlotCurve class.
//
// Input Arguments:
//		plotCurve	= const PlotCurve& to copy to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotCurve::PlotCurve(const PlotCurve &plotCurve) : Primitive(plotCurve),
	data(plotCurve.data), line(renderWindow)
{
	*this = plotCurve;
}

//==========================================================================
// Class:			PlotCurve
// Function:		~PlotCurve
//
// Description:		Destructor for the PlotCurve class.
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
PlotCurve::~PlotCurve()
{
}

//==========================================================================
// Class:			PlotCurve
// Function:		InitializeMarkerVertexBuffer
//
// Description:		Initializes the vertex buffer for storing the marker information.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::InitializeMarkerVertexBuffer()
{
	delete[] bufferInfo[1].vertexBuffer;

	bufferInfo[1].GetOpenGLIndices();

	bufferInfo[1].vertexCount = data.GetNumberOfPoints() * 4;
	bufferInfo[1].vertexBuffer = new float[bufferInfo[1].vertexCount * (renderWindow.GetVertexDimension() + 4)];
	assert(renderWindow.GetVertexDimension() == 2);

	bufferInfo[1].vertexCountModified = false;
}

//==========================================================================
// Class:			PlotCurve
// Function:		Update
//
// Description:		Updates the GL buffers associated with this object.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotCurve::Update(const unsigned int& i)
{
	if (i == 0)
	{
		int width, height;
			renderWindow.GetSize(&width, &height);
			width -= yAxis->GetOffsetFromWindowEdge() + yAxis->GetOppositeAxis()->GetOffsetFromWindowEdge();
			height -= xAxis->GetOffsetFromWindowEdge() + xAxis->GetOppositeAxis()->GetOffsetFromWindowEdge();

			xScale = (xAxis->GetMaximum() - xAxis->GetMinimum()) / width;
			yScale = (yAxis->GetMaximum() - yAxis->GetMinimum()) / height;

		if (lineSize > 0)
		{
			const double lineSizeScale(1.2);

			line.SetLineColor(color);
			line.SetBackgroundColorForAlphaFade();
			line.SetWidth(lineSize * lineSizeScale);
			line.SetXScale(xScale);
			line.SetYScale(yScale);

			line.Build(data.GetXPointer(), data.GetYPointer(), data.GetNumberOfPoints());
		}
		else
			line.SetWidth(0.0);

		// TODO:  Need to delete openGL objects?
		if (bufferInfo.size() == 0)
			bufferInfo.push_back(line.GetBufferInfo());
		else
			bufferInfo[i] = line.GetBufferInfo();
	}
	else
	{
		if (bufferInfo[i].vertexCountModified)
			InitializeMarkerVertexBuffer();

		BuildMarkers();

		glBindVertexArray(bufferInfo[i].vertexArrayIndex);

		glBindBuffer(GL_ARRAY_BUFFER, bufferInfo[i].vertexBufferIndex);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * bufferInfo[i].vertexCount * (renderWindow.GetVertexDimension() + 4),
			bufferInfo[i].vertexBuffer, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(renderWindow.GetPositionLocation());
		glVertexAttribPointer(renderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(renderWindow.GetColorLocation());
		glVertexAttribPointer(renderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
			(void*)(sizeof(GLfloat) * renderWindow.GetVertexDimension() * bufferInfo[i].vertexCount));

		glBindVertexArray(0);
	}
}

//==========================================================================
// Class:			PlotCurve
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
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
void PlotCurve::GenerateGeometry()
{
	if (bufferInfo.size() == 0)
		return;

	if (yAxis->GetOrientation() == Axis::OrientationLeft)
		dynamic_cast<PlotRenderer&>(renderWindow).LoadModelviewUniform(PlotRenderer::ModelviewLeft);
	else
		dynamic_cast<PlotRenderer&>(renderWindow).LoadModelviewUniform(PlotRenderer::ModelviewRight);

	glEnable(GL_SCISSOR_TEST);
	glBindVertexArray(bufferInfo[0].vertexArrayIndex);

	if (pretty)
		Line::DoPrettyDraw(bufferInfo[0].vertexCount);
	else
		Line::DoUglyDraw(bufferInfo[0].vertexCount);

	if (NeedsMarkersDrawn())
	{
		glBindVertexArray(bufferInfo[1].vertexArrayIndex);
		glDrawArrays(GL_QUADS, 0, bufferInfo[1].vertexCount);
	}

	glBindVertexArray(0);
	glDisable(GL_SCISSOR_TEST);

	dynamic_cast<PlotRenderer&>(renderWindow).LoadModelviewUniform(PlotRenderer::ModelviewFixed);
}

//==========================================================================
// Class:			PlotCurve
// Function:		PointIsValid
//
// Description:		Checks to see if the specified point is a real, finite number.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for valid, false otherwise
//
//==========================================================================
bool PlotCurve::PointIsValid(const unsigned int &i) const
{
	assert(i < data.GetNumberOfPoints());

	return PlotMath::IsValid<double>(data.GetXData(i)) &&
		PlotMath::IsValid<double>(data.GetYData(i));
}

//==========================================================================
// Class:			PlotCurve
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool PlotCurve::HasValidParameters()
{
	if (xAxis != NULL && yAxis != NULL && data.GetNumberOfPoints() > 1)
	{
		if (xAxis->IsHorizontal() && !yAxis->IsHorizontal())
			return true;
	}

	return false;
}

//==========================================================================
// Class:			PlotCurve
// Function:		operator=
//
// Description:		Assignment operator for PlotCurve class.
//
// Input Arguments:
//		plotCurve	= const PlotCurve& to assign to this object
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCurve&, reference to this object
//
//==========================================================================
PlotCurve& PlotCurve::operator=(const PlotCurve &plotCurve)
{
	if (this == &plotCurve)
		return *this;

	this->Primitive::operator=(plotCurve);

	return *this;
}

//==========================================================================
// Class:			PlotCurve
// Function:		BuildMarkers
//
// Description:		Plots markers at all un-interpolated points.
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
void PlotCurve::BuildMarkers()
{
	float halfMarkerXSize = 2 * markerSize * xScale;
	float halfMarkerYSize = 2 * markerSize * yScale;
	const unsigned int dimension(renderWindow.GetVertexDimension());
	const unsigned int colorStart(data.GetNumberOfPoints() * dimension * 4);

	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
	{
		bufferInfo[1].vertexBuffer[i * 4 * dimension] = (float)data.GetXData(i) + halfMarkerXSize;
		bufferInfo[1].vertexBuffer[i * 4 * dimension + 1] = (float)data.GetYData(i) + halfMarkerYSize;

		bufferInfo[1].vertexBuffer[i * 4 * dimension + dimension] = (float)data.GetXData(i) + halfMarkerXSize;
		bufferInfo[1].vertexBuffer[i * 4 * dimension + dimension + 1] = (float)data.GetYData(i) - halfMarkerYSize;

		bufferInfo[1].vertexBuffer[i * 4 * dimension + 2 * dimension] = (float)data.GetXData(i) - halfMarkerXSize;
		bufferInfo[1].vertexBuffer[i * 4 * dimension + 2 * dimension + 1] = (float)data.GetYData(i) - halfMarkerYSize;

		bufferInfo[1].vertexBuffer[i * 4 * dimension + 3 * dimension] = (float)data.GetXData(i) - halfMarkerXSize;
		bufferInfo[1].vertexBuffer[i * 4 * dimension + 3 * dimension + 1] = (float)data.GetYData(i) + halfMarkerYSize;

		bufferInfo[1].vertexBuffer[colorStart + i * 16] = (float)color.GetRed();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 1] = (float)color.GetGreen();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 2] = (float)color.GetBlue();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 3] = (float)color.GetAlpha();

		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 4] = (float)color.GetRed();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 5] = (float)color.GetGreen();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 6] = (float)color.GetBlue();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 7] = (float)color.GetAlpha();

		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 8] = (float)color.GetRed();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 9] = (float)color.GetGreen();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 10] = (float)color.GetBlue();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 11] = (float)color.GetAlpha();

		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 12] = (float)color.GetRed();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 13] = (float)color.GetGreen();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 14] = (float)color.GetBlue();
		bufferInfo[1].vertexBuffer[colorStart + i * 16 + 15] = (float)color.GetAlpha();
	}
}

//==========================================================================
// Class:			PlotCurve
// Function:		RangeIsSmall
//
// Description:		Determines if the range is small enough to warrant
//					drawing the point markers.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool PlotCurve::RangeIsSmall() const
{
	if (data.GetNumberOfPoints() < 2)
		return false;

	switch (XRangeIsSmall())
	{
	case RangeSizeSmall:
		return true;

	case RangeSizeLarge:
		return false;

	default:
	case RangeSizeUndetermined:
		break;
	}

	return YRangeIsSmall() == RangeSizeSmall;
}

//==========================================================================
// Class:			PlotCurve
// Function:		XRangeIsSmall
//
// Description:		Determines if the x-range is small enough to warrant
//					drawing the point markers.  A "small enough range" is
//					one where there are less than some number of pixels in the
//					x-direction between points (on average).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCurve::RangeSize
//
//==========================================================================
PlotCurve::RangeSize PlotCurve::XRangeIsSmall() const
{
	double period = data.GetXData(1) - data.GetXData(0);
	if (period == 0.0)
		return RangeSizeUndetermined;

	unsigned int points = (unsigned int)floor((xAxis->GetMaximum() - xAxis->GetMinimum()) / period);
	if (points == 0)
		return RangeSizeSmall;

	unsigned int spacing = (renderWindow.GetSize().GetWidth()
		- xAxis->GetAxisAtMaxEnd()->GetOffsetFromWindowEdge()
		- xAxis->GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) / points;

	if (spacing > 7)
		return RangeSizeSmall;

	return RangeSizeLarge;
}

//==========================================================================
// Class:			PlotCurve
// Function:		YRangeIsSmall
//
// Description:		Determines if the y-range is small enough to warrant
//					drawing the point markers.  A "small enough range" is
//					one where there are less than some number of pixels in the
//					y-direction between points (on average).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		PlotCurve::RangeSize
//
//==========================================================================
PlotCurve::RangeSize PlotCurve::YRangeIsSmall() const
{
	double period = data.GetYData(1) - data.GetYData(0);
	if (period == 0.0)
		return RangeSizeUndetermined;

	unsigned int points = (unsigned int)floor((yAxis->GetMaximum() - yAxis->GetMinimum()) / period);
	if (points == 0)
		return RangeSizeSmall;

	unsigned int spacing = (renderWindow.GetSize().GetHeight()
		- yAxis->GetAxisAtMaxEnd()->GetOffsetFromWindowEdge()
		- yAxis->GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) / points;

	if (spacing > 7)
		return RangeSizeSmall;

	return RangeSizeLarge;
}

//==========================================================================
// Class:			PlotCurve
// Function:		NeedsMarkersDrawn
//
// Description:		Determines if we should draw plot markers.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool PlotCurve::NeedsMarkersDrawn() const
{
	return markerSize > 0 || (markerSize < 0 && RangeIsSmall());
}
