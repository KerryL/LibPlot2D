/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotCurve.cpp
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derived from Primitive for creating data curves on a plot.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/primitives/plotCurve.h"
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/renderer/plotRenderer.h"
#include "lp2d/renderer/primitives/axis.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//=============================================================================
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
//=============================================================================
PlotCurve::PlotCurve(RenderWindow &renderWindow, const Dataset2D& data)
	: Primitive(renderWindow), mData(data), mLine(renderWindow)
{
	mLine.SetBufferHint(GL_STATIC_DRAW);
	mBufferInfo.resize(2);// First one for lines, second one for the markers
}

//=============================================================================
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
//=============================================================================
PlotCurve::PlotCurve(const PlotCurve &plotCurve) : Primitive(plotCurve),
	mData(plotCurve.mData), mLine(mRenderWindow)
{
	*this = plotCurve;
}

//=============================================================================
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
//=============================================================================
void PlotCurve::InitializeMarkerVertexBuffer()
{
	mBufferInfo[1].GetOpenGLIndices();

	mBufferInfo[1].vertexCount = mData.GetNumberOfPoints() * 4;
	mBufferInfo[1].vertexBuffer.resize(mBufferInfo[1].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));
	assert(mRenderWindow.GetVertexDimension() == 2);

	mBufferInfo[1].vertexCountModified = false;
}

//=============================================================================
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
//=============================================================================
void PlotCurve::Update(const unsigned int& i)
{
	if (i == 0)
	{
		int width, height;
			mRenderWindow.GetSize(&width, &height);
			width -= mYAxis->GetOffsetFromWindowEdge() + mYAxis->GetOppositeAxis()->GetOffsetFromWindowEdge();
			height -= mXAxis->GetOffsetFromWindowEdge() + mXAxis->GetOppositeAxis()->GetOffsetFromWindowEdge();

			if (mXAxis->IsLogarithmic())
				mXScale = (log10(mXAxis->GetMaximum()) - log10(mXAxis->GetMinimum())) / width;
			else
				mXScale = (mXAxis->GetMaximum() - mXAxis->GetMinimum()) / width;

			if (mYAxis->IsLogarithmic())
				mYScale = (log10(mYAxis->GetMaximum()) - log10(mYAxis->GetMinimum())) / height;
			else
				mYScale = (mYAxis->GetMaximum() - mYAxis->GetMinimum()) / height;

		if (mLineSize > 0.0)
		{
			const double lineSizeScale(1.2);

			mLine.SetLineColor(mColor);
			mLine.SetBackgroundColorForAlphaFade();
			mLine.SetWidth(mLineSize * lineSizeScale);
			mLine.SetXScale(mXScale);
			mLine.SetYScale(mYScale);

			std::vector<double> xLogData;
			std::vector<double> yLogData;
			const std::vector<double>& xRef([this, &xLogData]()
			{
				if (this->mXAxis->IsLogarithmic())
				{
					xLogData = std::move(DoLogarithmicScale(this->mData.GetX()));
					return xLogData;
				}
				else
					return mData.GetX();
			}());

			const std::vector<double>& yRef([this, &yLogData]()
			{
				if (mYAxis->IsLogarithmic())
				{
					yLogData = std::move(DoLogarithmicScale(mData.GetY()));
					return yLogData;
				}
				else
					return mData.GetY();
			}());

			mLine.Build(xRef, yRef, mBufferInfo[i]);
		}
		else
			mLine.SetWidth(0.0);
	}
	else
	{
		if (mBufferInfo[i].vertexCountModified)
			InitializeMarkerVertexBuffer();

		BuildMarkers();

		glBindVertexArray(mBufferInfo[i].GetVertexArrayIndex());

		glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[i].GetVertexBufferIndex());
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * mBufferInfo[i].vertexCount * (mRenderWindow.GetVertexDimension() + 4),
			mBufferInfo[i].vertexBuffer.data(), GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
		glVertexAttribPointer(mRenderWindow.GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
		glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
			(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[i].vertexCount));

		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[i].indexBufferIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mBufferInfo[i].indexBuffer.size(),
			mBufferInfo[i].indexBuffer.data(), GL_DYNAMIC_DRAW);*/
		// TODO:  Why doesn't this work?  Similar code worked just fine before switching to std::vector

		glBindVertexArray(0);
	}

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
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
//=============================================================================
void PlotCurve::GenerateGeometry()
{
	if (mYAxis->GetOrientation() == Axis::Orientation::Left)
		dynamic_cast<PlotRenderer&>(mRenderWindow).LoadModelviewUniform(PlotRenderer::Modelview::Left);
	else
		dynamic_cast<PlotRenderer&>(mRenderWindow).LoadModelviewUniform(PlotRenderer::Modelview::Right);

	glEnable(GL_SCISSOR_TEST);

	if (mLineSize > 0.0)
	{
		glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

		if (mPretty)
			Line::DoPrettyDraw(mBufferInfo[0].indexBuffer.size());
		else
			Line::DoUglyDraw(mBufferInfo[0].vertexCount);
	}

	if (NeedsMarkersDrawn())
	{
		glBindVertexArray(mBufferInfo[1].GetVertexArrayIndex());
		glDrawArrays(GL_QUADS, 0, mBufferInfo[1].vertexCount);
	}

	glBindVertexArray(0);
	glDisable(GL_SCISSOR_TEST);

	assert(!RenderWindow::GLHasError());

	dynamic_cast<PlotRenderer&>(mRenderWindow).LoadModelviewUniform(PlotRenderer::Modelview::Fixed);
}

//=============================================================================
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
//=============================================================================
bool PlotCurve::PointIsValid(const unsigned int &i) const
{
	assert(i < mData.GetNumberOfPoints());

	return PlotMath::IsValid<double>(mData.GetX()[i]) &&
		PlotMath::IsValid<double>(mData.GetY()[i]);
}

//=============================================================================
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
//=============================================================================
bool PlotCurve::HasValidParameters()
{
	if (mXAxis != nullptr && mYAxis != nullptr && mData.GetNumberOfPoints() > 1)
	{
		if (mXAxis->IsHorizontal() && !mYAxis->IsHorizontal())
			return true;
	}

	return false;
}

//=============================================================================
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
//=============================================================================
PlotCurve& PlotCurve::operator=(const PlotCurve &plotCurve)
{
	if (this == &plotCurve)
		return *this;

	this->Primitive::operator=(plotCurve);

	return *this;
}

//=============================================================================
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
//=============================================================================
void PlotCurve::BuildMarkers()
{
	float halfMarkerYSize = 2 * mMarkerSize * mYScale;
	float halfMarkerXSize = 2 * mMarkerSize * mXScale;
	const unsigned int dimension(mRenderWindow.GetVertexDimension());
	const unsigned int colorStart(mData.GetNumberOfPoints() * dimension * 4);

	// Use function pointers to save a few checks in the loop
	PlotRenderer::ScalingFunction xScaleFunction(
		dynamic_cast<PlotRenderer&>(mRenderWindow).GetXScaleFunction());
	PlotRenderer::ScalingFunction yScaleFunction;

	if (mYAxis->GetOrientation() == Axis::Orientation::Left)
		yScaleFunction = dynamic_cast<PlotRenderer&>(mRenderWindow).GetLeftYScaleFunction();
	else
		yScaleFunction = dynamic_cast<PlotRenderer&>(mRenderWindow).GetRightYScaleFunction();

	unsigned int i;
	for (i = 0; i < mData.GetNumberOfPoints(); ++i)
	{
		float x(static_cast<float>(xScaleFunction(mData.GetX()[i])));
		float y(static_cast<float>(yScaleFunction(mData.GetY()[i])));

		mBufferInfo[1].vertexBuffer[i * 4 * dimension] = x + halfMarkerXSize;
		mBufferInfo[1].vertexBuffer[i * 4 * dimension + 1] = y + halfMarkerYSize;

		mBufferInfo[1].vertexBuffer[i * 4 * dimension + dimension] = x + halfMarkerXSize;
		mBufferInfo[1].vertexBuffer[i * 4 * dimension + dimension + 1] = y - halfMarkerYSize;

		mBufferInfo[1].vertexBuffer[i * 4 * dimension + 2 * dimension] = x - halfMarkerXSize;
		mBufferInfo[1].vertexBuffer[i * 4 * dimension + 2 * dimension + 1] = y - halfMarkerYSize;

		mBufferInfo[1].vertexBuffer[i * 4 * dimension + 3 * dimension] = x - halfMarkerXSize;
		mBufferInfo[1].vertexBuffer[i * 4 * dimension + 3 * dimension + 1] = y + halfMarkerYSize;

		mBufferInfo[1].vertexBuffer[colorStart + i * 16] = static_cast<float>(mColor.GetRed());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 1] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 2] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 3] = static_cast<float>(mColor.GetAlpha());

		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 4] = static_cast<float>(mColor.GetRed());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 5] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 6] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 7] = static_cast<float>(mColor.GetAlpha());

		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 8] = static_cast<float>(mColor.GetRed());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 9] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 10] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 11] = static_cast<float>(mColor.GetAlpha());

		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 12] = static_cast<float>(mColor.GetRed());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 13] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 14] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[1].vertexBuffer[colorStart + i * 16 + 15] = static_cast<float>(mColor.GetAlpha());
	}
}

//=============================================================================
// Class:			PlotCurve
// Function:		DoLogarithmicScale
//
// Description:		Handles scaling for arrays of logarithmic data.
//
// Input Arguments:
//		value	= const std::vector<double>&
//
// Output Arguments:
//		None
//
// Return Value:
//		std::vector<double>
//
//=============================================================================
std::vector<double> PlotCurve::DoLogarithmicScale(
	const std::vector<double>& values)
{
	std::vector<double> scaledValues(values);
	for (auto& v : scaledValues)
		v = PlotRenderer::DoLogarithmicScale(v);

	return scaledValues;
}

//=============================================================================
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
//=============================================================================
bool PlotCurve::RangeIsSmall() const
{
	if (mData.GetNumberOfPoints() < 2)
		return false;

	switch (XRangeIsSmall())
	{
	case RangeSize::Small:
		return true;

	case RangeSize::Large:
		return false;

	default:
	case RangeSize::Undetermined:
		break;
	}

	return YRangeIsSmall() == RangeSize::Small;
}

//=============================================================================
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
//=============================================================================
PlotCurve::RangeSize PlotCurve::XRangeIsSmall() const
{
	double period(fabs(mData.GetX()[1] - mData.GetX()[0]));
	if (period == 0.0)
		return RangeSize::Undetermined;

	unsigned int points = (unsigned int)floor((mXAxis->GetMaximum() - mXAxis->GetMinimum()) / period);
	if (points == 0)
		return RangeSize::Small;

	unsigned int spacing = (mRenderWindow.GetSize().GetWidth()
		- mXAxis->GetAxisAtMaxEnd()->GetOffsetFromWindowEdge()
		- mXAxis->GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) / points;

	if (spacing > 7)
		return RangeSize::Small;

	return RangeSize::Large;
}

//=============================================================================
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
//=============================================================================
PlotCurve::RangeSize PlotCurve::YRangeIsSmall() const
{
	double period(mData.GetY()[1] - mData.GetY()[0]);
	if (period == 0.0)
		return RangeSize::Undetermined;

	unsigned int points = (unsigned int)floor((mYAxis->GetMaximum() - mYAxis->GetMinimum()) / period);
	if (points == 0)
		return RangeSize::Small;

	unsigned int spacing = (mRenderWindow.GetSize().GetHeight()
		- mYAxis->GetAxisAtMaxEnd()->GetOffsetFromWindowEdge()
		- mYAxis->GetAxisAtMinEnd()->GetOffsetFromWindowEdge()) / points;

	if (spacing > 7)
		return RangeSize::Small;

	return RangeSize::Large;
}

//=============================================================================
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
//=============================================================================
bool PlotCurve::NeedsMarkersDrawn() const
{
	return mMarkerSize > 0 || (mMarkerSize < 0 && RangeIsSmall());
}

}// namespace LibPlot2D
