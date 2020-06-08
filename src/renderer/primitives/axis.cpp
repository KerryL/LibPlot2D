/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  axis.cpp
// Date:  5/2/2011
// Auth:  K. Loux
// Desc:  Derived from Primitive for creating axis objects on a plot.

// GLEW headers
#include <GL/glew.h>

// Standard C++ headers
#include <algorithm>

// Local headers
#include "lp2d/renderer/primitives/axis.h"
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			Axis
// Function:		Axis
//
// Description:		Constructor for the Axis class.
//
// Input Arguments:
//		mRenderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Axis::Axis(RenderWindow &mRenderWindow) : Primitive(mRenderWindow),
	mLabelText(mRenderWindow), mValueText(mRenderWindow),
	mAxisLines(mRenderWindow), mGridLines(mRenderWindow)
{
	mColor = Color::ColorBlack;
	mGridColor.Set(0.8, 0.8, 0.8, 1.0);
	SetDrawOrder(500);

	mBufferInfo.resize(4);// Main axis line, gridlines, values and label
}

//=============================================================================
// Class:			Axis
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
void Axis::Update(const unsigned int& i)
{
	if (i == 0)// Axis and ticks
	{
		mAxisLines.SetWidth(1.0);
		mAxisLines.SetLineColor(mColor);
		mAxisLines.SetBackgroundColorForAlphaFade();

		mGridLines.SetWidth(1.0);
		mGridLines.SetLineColor(mGridColor);
		mGridLines.SetBackgroundColorForAlphaFade();

		mAxisPoints.clear();
		mGridPoints.clear();
		DrawFullAxis();
		mAxisLines.BuildSegments(mAxisPoints, mBufferInfo[i]);
	}
	else if (i == 1)// Gridlines
	{
		mGridLines.BuildSegments(mGridPoints, mBufferInfo[i]);
	}
	else if (i == 2 && mValueText.IsOK())// Values
	{
		DrawTickLabels();
		mBufferInfo[i] = mValueText.BuildText();
	}
	else if (i == 3 && mLabelText.IsOK())// Label
	{
		DrawAxisLabel();
		mBufferInfo[i] = mLabelText.BuildText();
	}
}

//=============================================================================
// Class:			Axis
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
void Axis::GenerateGeometry()
{
	// Draw gridlines first
	if ((mMajorGrid || mMinorGrid) && mBufferInfo[1].indexBuffer.size() > 0)
	{
		glBindVertexArray(mBufferInfo[1].GetVertexArrayIndex());
		Line::DoPrettyDraw(mBufferInfo[1].indexBuffer.size());
	}

	// Axis and ticks next
	if (mBufferInfo[0].vertexCount > 0)
	{
		glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
		Line::DoPrettyDraw(mBufferInfo[0].indexBuffer.size());
	}

	if (mValueText.IsOK() && mBufferInfo[2].vertexCount > 0 && mTickStyle != TickStyle::NoTicks)
	{
		glBindVertexArray(mBufferInfo[2].GetVertexArrayIndex());
		mValueText.RenderBufferedGlyph(mBufferInfo[2].vertexCount);
	}

	if (!mLabel.IsEmpty() && mLabelText.IsOK() && mBufferInfo[3].vertexCount > 0 && mTickStyle != TickStyle::NoTicks)
	{
		glBindVertexArray(mBufferInfo[3].GetVertexArrayIndex());
		mLabelText.RenderBufferedGlyph(mBufferInfo[3].vertexCount);
	}

	glBindVertexArray(0);
}

//=============================================================================
// Class:			Axis
// Function:		DrawFullAxis
//
// Description:		Draws the axis, grid lines and tick marks.
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
void Axis::DrawFullAxis()
{
	unsigned int numberOfTicks, numberOfGridLines;
	int mainAxisLocation = ComputeMainAxisLocation();

	ComputeGridAndTickCounts(numberOfTicks, &numberOfGridLines);

	if (IsHorizontal())
	{
		if ((mMajorGrid || mMinorGrid) && mOppositeAxis)
			DrawHorizontalGrid(numberOfGridLines);

		if (mTickStyle != TickStyle::NoTicks)
			DrawHorizontalTicks(numberOfTicks, mainAxisLocation);
	}
	else
	{
		if ((mMajorGrid || mMinorGrid) && mOppositeAxis)
			DrawVerticalGrid(numberOfGridLines);

		if (mTickStyle != TickStyle::NoTicks)
			DrawVerticalTicks(numberOfTicks, mainAxisLocation);
	}

	DrawMainAxis(mainAxisLocation);
}

//=============================================================================
// Class:			Axis
// Function:		ComputeMainAxisLocation
//
// Description:		Calculates the location of the main axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		int, indicating the location of the main axis
//
//=============================================================================
int Axis::ComputeMainAxisLocation() const
{
	// Compute the mainAxisLocation (X for vertical axis, Y for horizontal axis)
	if (mOrientation == Orientation::Bottom || mOrientation == Orientation::Left)
		return mOffsetFromWindowEdge;
	else if (mOrientation == Orientation::Right)
		return mRenderWindow.GetSize().GetWidth() - mOffsetFromWindowEdge;

	//else// OrientationTop
	return mRenderWindow.GetSize().GetHeight() - mOffsetFromWindowEdge;
}

//=============================================================================
// Class:			Axis
// Function:		ComputeGridAndTickCounts
//
// Description:		Calculates the number of tick marks and (optionally) grid lines.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		tickCount	= unsigned int&
//		gridCount	= unsigned int*
//
// Return Value:
//		None
//
//=============================================================================
void Axis::ComputeGridAndTickCounts(unsigned int &tickCount, unsigned int *gridCount)
{
	if (mLogarithmic)
	{
		// Only allow strictly positive limits
		if (mMinimum <= 0.0)
			mMinimum = 0.1;
		if (mMaximum <= mMinimum)
			mMaximum = 10.0 * mMinimum;

		tickCount = ceil(log10(mMaximum)) - floor(log10(mMinimum)) - 1;
		if (gridCount)
			*gridCount = (tickCount + 1) * 8 + tickCount;
	}
	else
	{
		assert(mMajorResolution > 0.0 && PlotMath::IsValid(mMajorResolution));
		assert(mMinorResolution > 0.0 && PlotMath::IsValid(mMinorResolution));
		tickCount = static_cast<unsigned int>((mMaximum - mMinimum) / mMajorResolution + 0.5) - 1U;
		if (gridCount)
		{
			if (mMinorGrid)
				*gridCount = static_cast<unsigned int>((mMaximum - mMinimum) / mMinorResolution + 0.5) - 1U;
			else
				*gridCount = tickCount;
		}
	}
}

//=============================================================================
// Class:			Axis
// Function:		DrawMainAxis
//
// Description:		Draws the line representing the main axis.
//
// Input Arguments:
//		mainAxisLocation	= const int& indicating the offset for the axis
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Axis::DrawMainAxis(const int &mainAxisLocation)
{
	if (IsHorizontal())
	{
		mAxisPoints.push_back(std::make_pair(mMinAxis->GetOffsetFromWindowEdge(), mainAxisLocation));
		mAxisPoints.push_back(std::make_pair(mRenderWindow.GetSize().GetWidth()
			- mMaxAxis->GetOffsetFromWindowEdge(), mainAxisLocation));
	}
	else
	{
		mAxisPoints.push_back(std::make_pair(mainAxisLocation, mMinAxis->GetOffsetFromWindowEdge()));
		mAxisPoints.push_back(std::make_pair(mainAxisLocation, mRenderWindow.GetSize().GetHeight() -
			mMaxAxis->GetOffsetFromWindowEdge()));
	}
}

//=============================================================================
// Class:			Axis
// Function:		InitializeTickParameters
//
// Description:		Initializes the parameters used for drawing tick marks.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		inside	= int&
//		outside	= int&
//		sign	= int&
//
// Return Value:
//		None
//
//=============================================================================
void Axis::InitializeTickParameters(int &inside, int &outside, int &sign) const
{
	inside = 0;
	outside = 0;
	sign = 1;

	if (mTickStyle == TickStyle::Inside)
		inside = 1.0;
	else if (mTickStyle == TickStyle::Outside)
		outside = 1.0;
	else if (mTickStyle == TickStyle::Through)
	{
		inside = 0.5;
		outside = 0.5;
	}

	if (mOrientation == Orientation::Top || mOrientation == Orientation::Right)
		sign = -1.0;
}

//=============================================================================
// Class:			Axis
// Function:		DrawHorizontalGrid
//
// Description:		Draws grid lines for horizontal axes.
//
// Input Arguments:
//		count	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Axis::DrawHorizontalGrid(const unsigned int &count)
{
	unsigned int grid;
	double location;
	for (grid = 0; grid < count; ++grid)
	{
		if (mMinorGrid)
			location = ValueToPixel(GetNextGridValue(grid + 1));
		else
			location = ValueToPixel(GetNextTickValue(false, false, grid + 1));

		if (location <= mMinAxis->GetOffsetFromWindowEdge() ||
			location >= mRenderWindow.GetSize().GetWidth() - mMaxAxis->GetOffsetFromWindowEdge())
			continue;

		mGridPoints.push_back(std::make_pair(location, static_cast<double>(mOffsetFromWindowEdge)));
		mGridPoints.push_back(std::make_pair(location,
			static_cast<double>(mRenderWindow.GetSize().GetHeight() - mOppositeAxis->GetOffsetFromWindowEdge())));
	}
}

//=============================================================================
// Class:			Axis
// Function:		DrawHorizontalTicks
//
// Description:		Draws tick marks for horizontal axes.
//
// Input Arguments:
//		count				= const unsigned int&
//		mainAxisLocation	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Axis::DrawHorizontalTicks(const unsigned int &count, const int &mainAxisLocation)
{
	int insideTick, outsideTick, sign;
	InitializeTickParameters(insideTick, outsideTick, sign);

	unsigned int tick;
	for (tick = 0; tick < count; ++tick)
	{
		double location(ValueToPixel(GetNextTickValue(false, false, tick + 1)));
		if (location <= mMinAxis->GetOffsetFromWindowEdge() ||
			location >= mRenderWindow.GetSize().GetWidth() - mMaxAxis->GetOffsetFromWindowEdge())
			continue;

		mAxisPoints.push_back(std::make_pair(location, static_cast<double>(mainAxisLocation - mTickSize * outsideTick * sign)));
		mAxisPoints.push_back(std::make_pair(location, static_cast<double>(mainAxisLocation + mTickSize * insideTick * sign)));
	}
}

//=============================================================================
// Class:			Axis
// Function:		DrawVerticalGrid
//
// Description:		Draws grid lines for vertical axes.
//
// Input Arguments:
//		count	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Axis::DrawVerticalGrid(const unsigned int &count)
{
	unsigned int grid;
	double location;
	for (grid = 0; grid < count; ++grid)
	{
		if (mMinorGrid)
			location = ValueToPixel(GetNextGridValue(grid + 1));
		else
			location = ValueToPixel(GetNextTickValue(false, false, grid + 1));

		if (location <= mMinAxis->GetOffsetFromWindowEdge() ||
			location >= mRenderWindow.GetSize().GetHeight() - mMaxAxis->GetOffsetFromWindowEdge())
			continue;

		mGridPoints.push_back(std::make_pair(static_cast<double>(mOffsetFromWindowEdge), location));
		mGridPoints.push_back(std::make_pair(static_cast<double>(mRenderWindow.GetSize().GetWidth()
			- mOppositeAxis->GetOffsetFromWindowEdge()), location));
	}
}

//=============================================================================
// Class:			Axis
// Function:		DrawVerticalTicks
//
// Description:		Draws tick marks for vertical axes.
//
// Input Arguments:
//		count				= const unsigned int&
//		mainAxisLocation	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Axis::DrawVerticalTicks(const unsigned int &count, const int &mainAxisLocation)
{
	int insideTick, outsideTick, sign;
	InitializeTickParameters(insideTick, outsideTick, sign);

	unsigned int tick;
	for (tick = 0; tick < count; ++tick)
	{
		double location(ValueToPixel(GetNextTickValue(false, false, tick + 1)));
		if (location <= mMinAxis->GetOffsetFromWindowEdge() ||
			location >= mRenderWindow.GetSize().GetHeight() - mMaxAxis->GetOffsetFromWindowEdge())
			continue;

		mAxisPoints.push_back(std::make_pair(static_cast<double>(mainAxisLocation - mTickSize * outsideTick * sign), location));
		mAxisPoints.push_back(std::make_pair(static_cast<double>(mainAxisLocation + mTickSize * insideTick * sign), location));
	}
}

//=============================================================================
// Class:			Axis
// Function:		GetNextLogValue
//
// Description:		Updates the value argument to indicate the next value for
//					drawing log grid lines.
//
// Input Arguments:
//		first	= const bool& indicating if this is the first grid line
//		value	= double& (input and output)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void Axis::GetNextLogValue(const bool &first, double &value) const
{
	if (first)
	{
		value = pow(10.0, floor(log10(mMinimum)));
		int scale(1);
		while (value * scale <= mMinimum)
			++scale;
		value *= scale;
	}
	else
	{
		value +=pow(10.0, floor(log10(value)));

		double orderOfMagnitude(floor(log10(value)));
		value /= pow(10.0, orderOfMagnitude);
		value = floor(value + 0.5);
		value *= pow(10.0, orderOfMagnitude);
	}
}

//=============================================================================
// Class:			Axis
// Function:		DrawAxisLabel
//
// Description:		Draws the label text for the axis.
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
void Axis::DrawAxisLabel()
{
	if (mLabel.IsEmpty())
		return;

	double fontOffsetFromWindowEdge = mOffsetFromWindowEdge / 3.0;
	if (!IsHorizontal())
		fontOffsetFromWindowEdge /= 2.0;

	// TODO:  Change plot dimension if there is a title? or if there is a title and a label for the top axis?
	Text::BoundingBox boundingBox = mLabelText.GetBoundingBox("H");// Some capital letter to assure uniform spacing
	const double edgeOffset(GetAxisLabelTranslation(fontOffsetFromWindowEdge,
		boundingBox.yUp));

	boundingBox = mLabelText.GetBoundingBox(mLabel.ToStdString());
	const double textWidth = boundingBox.xRight - boundingBox.xLeft;
	const double plotOffset = static_cast<double>(mMinAxis->GetOffsetFromWindowEdge())
		- static_cast<double>(mMaxAxis->GetOffsetFromWindowEdge());

	if (IsHorizontal())
		mLabelText.SetPosition(0.5 * (mRenderWindow.GetSize().GetWidth()
			- textWidth + plotOffset), edgeOffset);
	else
	{
		mLabelText.SetOrientation(M_PI * 0.5);
		mLabelText.SetPosition(0.5 * (mRenderWindow.GetSize().GetHeight()
			- textWidth + plotOffset), -edgeOffset);
	}

	mLabelText.SetText(mLabel.ToStdString());
}

//=============================================================================
// Class:			Axis
// Function:		GetAxisLabelTranslation
//
// Description:		Determines the translation required for drawing the label.
//
// Input Arguments:
//		offset		= const double&
//		fontHeight	= const double& indicating the height of the font
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the translation to use for this mLabel
//
//=============================================================================
double Axis::GetAxisLabelTranslation(const double &offset,
	const double &fontHeight) const
{
	switch (mOrientation)
	{
	case Orientation::Bottom:
		return offset;

	case Orientation::Left:
		return offset + fontHeight;

	case Orientation::Top:
		return mRenderWindow.GetSize().GetHeight() - offset - fontHeight;

	case Orientation::Right:
		return mRenderWindow.GetSize().GetWidth() - offset;

	default:
		assert(false);
		return 0.0;
	}
}

//=============================================================================
// Class:			Axis
// Function:		DrawTickLabels
//
// Description:		Draws the numeric labels for each axis tick.  This also
//					determines the precision for each tick label.  The goal
//					is to provide just enough precision so that adjacent tick
//					marks are distinguishable, and then add just a hair more.
//					Here we also reset the axis min and max values to be exactly
//					the vales of the min and max labels after rounding (keeps
//					tick labels accurate).
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
void Axis::DrawTickLabels()
{
	float xTranslation, yTranslation;
	const unsigned int precision(GetPrecision());

	mMinimum = GetPrecisionLimitedMinimum();

	wxString valueLabel;
	double valueOffsetFromEdge = mOffsetFromWindowEdge * 0.8;
	unsigned int tick, numberOfTicks;
	ComputeGridAndTickCounts(numberOfTicks);
	for (tick = 0; tick < numberOfTicks + 2; ++tick)
	{
		double value(std::min(GetNextTickValue(tick == 0,
			tick == numberOfTicks + 1, tick), mMaximum));
		valueLabel.Printf("%0.*f", precision, value);

		// TODO:  Don't draw it if it's too close to the maximum (based on text size)
		ComputeTranslations(value, xTranslation, yTranslation,
			mValueText.GetBoundingBox(valueLabel.ToStdString()),
			valueOffsetFromEdge);
		mValueText.SetPosition(xTranslation, yTranslation);
		mValueText.AppendText(valueLabel.ToStdString());
	}

	mMaximum = GetPrecisionLimitedMaximum();
}

//=============================================================================
// Class:			Axis
// Function:		GetPrecisionLimitedMinimum
//
// Description:		Gets the value of the minimum axis limit given with the
//					precision of the displayed number.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double Axis::GetPrecisionLimitedMinimum() const
{
	const unsigned int precision(GetPrecision());
	double minimum;
	if (!wxString::Format("%0.*f", precision, mMinimum).ToDouble(&minimum)) { /*Warn the user?*/ }
	return minimum;
}

//=============================================================================
// Class:			Axis
// Function:		GetPrecisionLimitedMaximum
//
// Description:		Gets the value of the maximum axis limit given with the
//					precision of the displayed number.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double Axis::GetPrecisionLimitedMaximum() const
{
	const unsigned int precision(GetPrecision());
	double maximum;
	if (!wxString::Format("%0.*f", precision, mMaximum).ToDouble(&maximum)) { /*Warn the user?*/ }
	return maximum;
}

//=============================================================================
// Class:			Axis
// Function:		GetPrecision
//
// Description:		Determines appropriate precision to use for axis resolution.
//					Ideal precision is determined by the mMinimum number of digits
//					to differentiate one tick mark from the next, plus two.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int Axis::GetPrecision() const
{
	// It does look nicer to use the raw return value of GetPrecision(), but it
	// affects the function of dragging the plot around.  Because we always
	// force the limits to actually match the printed values, it makes the
	// dragging operation very coarse.  So we add two (if the major resolution
	// is small).
	const auto autoResolution(PlotMath::GetPrecision(mMinimum, mMajorResolution, mLogarithmic));
	if (mMajorResolution < 10)
		return autoResolution + 2;
	else if (mMajorResolution < 100)
		return autoResolution + 1;
	return autoResolution;
}

//=============================================================================
// Class:			Axis
// Function:		GetNextTickValue
//
// Description:		Computes value to display at the next tick mark.
//
// Input Arguments:
//		first	= const bool&
//		last	= const bool&
//		tick	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double Axis::GetNextTickValue(const bool &first, const bool &last,
	const unsigned int &tick) const
{
	if (mLogarithmic)
	{
		if (first)
			return mMinimum;
		else if (last)
			return mMaximum;
		else
			return pow(10.0, floor(log10(mMinimum)) + tick);
	}

	return mMinimum + static_cast<double>(tick) * mMajorResolution;
}

//=============================================================================
// Class:			Axis
// Function:		ComputeTranslations
//
// Description:		Computes the translations required for the specified
//					bounding box.
//
// Input Arguments:
//		value			= const double&
//		boundingBox		= const Text::BoundingBox&
//		offset			= const double&
//
// Output Arguments:
//		xTranslation	= float&
//		yTranslation	= float&
//
// Return Value:
//		double
//
//=============================================================================
void Axis::ComputeTranslations(const double &value, float &xTranslation,
	float &yTranslation, const Text::BoundingBox &boundingBox,
	const double &offset) const
{
	if (IsHorizontal())
	{
		if (mOrientation == Orientation::Bottom)
			yTranslation = offset - boundingBox.yUp;
		else
			yTranslation = mRenderWindow.GetSize().GetHeight() - offset;

		xTranslation = ValueToPixel(value) -
			(boundingBox.xRight - boundingBox.xLeft) / 2.0;
	}
	else
	{
		if (mOrientation == Orientation::Left)
			xTranslation = offset - boundingBox.xRight;
		else
			xTranslation = mRenderWindow.GetSize().GetWidth() - offset;

		yTranslation = ValueToPixel(value) -
			(boundingBox.yUp - boundingBox.yDown) / 2.0;
	}
}

//=============================================================================
// Class:			Axis
// Function:		IsHorizontal
//
// Description:		Checks to see if this object has horizontal mOrientation.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for horizontal, false for vertical
//
//=============================================================================
bool Axis::IsHorizontal() const
{
	if (mOrientation == Orientation::Bottom ||
		mOrientation == Orientation::Top)
		return true;

	return false;
}

//=============================================================================
// Class:			Axis
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
bool Axis::HasValidParameters()
{
	// Don't draw if any of the limits are not numbers
	if (PlotMath::IsNaN(mMinimum) || PlotMath::IsNaN(mMaximum))
		return false;

	// Make sure the pointers to the perpendicular axes have been provided
	if (!mMinAxis || !mMaxAxis)
		return false;

	return true;
}

//=============================================================================
// Class:			Axis
// Function:		ValueToPixel
//
// Description:		Computes the pixel location along the axis that corresponds
//					to the specified plot-units value.
//
// Input Arguments:
//		value	= const double& containing the location of the point in plot
//				  coordinates
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying the location of the object in screen coordinates
//
//=============================================================================
double Axis::ValueToPixel(const double &value) const
{
	// Get the plot size
	int plotDimension;
	if (IsHorizontal())
		plotDimension = mRenderWindow.GetSize().GetWidth()
				- mMinAxis->GetOffsetFromWindowEdge()
				- mMaxAxis->GetOffsetFromWindowEdge();
	else
		plotDimension = mRenderWindow.GetSize().GetHeight()
				- mMinAxis->GetOffsetFromWindowEdge()
				- mMaxAxis->GetOffsetFromWindowEdge();

	// Do the scaling
	if (IsLogarithmic())
	{
		if (value <= 0.0 || mMinimum <= 0.0)
			return GetOffsetFromWindowEdge();
		else
			return mMinAxis->GetOffsetFromWindowEdge()
				+ (log10(value) - log10(mMinimum)) /
				(log10(mMaximum) - log10(mMinimum)) * plotDimension;
	}

	return mMinAxis->GetOffsetFromWindowEdge()
		+ (value - mMinimum) /
		(mMaximum - mMinimum) * plotDimension;
}

//=============================================================================
// Class:			Axis
// Function:		PixelToValue
//
// Description:		Computes the value (in plot-units) corresponding to the
//					specified on-screen pixel location.
//
// Input Arguments:
//		pixel	= const int& specifying desired screen-coordinate
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifing the corresponding value
//
//=============================================================================
double Axis::PixelToValue(const int &pixel) const
{
	// Get the plot size
	double fraction;
	if (IsHorizontal())
		fraction = (static_cast<double>(pixel)
			- mMinAxis->GetOffsetFromWindowEdge())
			/ (static_cast<double>(mRenderWindow.GetSize().GetWidth())
			- mMinAxis->GetOffsetFromWindowEdge()
			- mMaxAxis->GetOffsetFromWindowEdge());
	else
		fraction = (static_cast<double>(pixel)
			- mMinAxis->GetOffsetFromWindowEdge())
			/ (static_cast<double>(mRenderWindow.GetSize().GetHeight())
			- mMinAxis->GetOffsetFromWindowEdge()
			- mMaxAxis->GetOffsetFromWindowEdge());

	// Do the scaling
	if (IsLogarithmic())
		return pow(10.0, fraction * (log10(mMaximum) - log10(mMinimum))
			+ log10(mMinimum));

	return fraction * (mMaximum - mMinimum) + mMinimum;
}

//=============================================================================
// Class:			Axis
// Function:		GetNextGridValue
//
// Description:		Computes value to display at the next grid line.
//
// Input Arguments:
//		tick	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double Axis::GetNextGridValue(const unsigned int &tick) const
{
	if (mLogarithmic)
		return pow(10.0, floor(log10(mMinimum)) + floor(tick / 9.0))
			* (tick - 9.0 * floor(tick / 9.0) + 1.0);

	return mMinimum + tick * mMinorResolution;
}

//=============================================================================
// Class:			Axis
// Function:		GetAxisLength
//
// Description:		Returns the length of the axis in pixels.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int Axis::GetAxisLength() const
{
	if (mOrientation == Orientation::Top || mOrientation == Orientation::Bottom)
	{
		return mRenderWindow.GetSize().GetWidth()
			- mMinAxis->GetOffsetFromWindowEdge()
			- mMaxAxis->GetOffsetFromWindowEdge();
	}
	else
	{
		return mRenderWindow.GetSize().GetHeight()
			- mMinAxis->GetOffsetFromWindowEdge()
			- mMaxAxis->GetOffsetFromWindowEdge();
	}
}

//=============================================================================
// Class:			Axis
// Function:		InitializeFonts
//
// Description:		Initializes the font objects.
//
// Input Arguments:
//		fontFileName	= const std::string&
//		size			= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool Axis::InitializeFonts(const std::string& fontFileName, const double& size)
{
	if (!mLabelText.SetFace(fontFileName) || !mValueText.SetFace(fontFileName))
		return false;

	mLabelText.SetColor(mColor);
	mValueText.SetColor(mColor);

	// For some reason, fonts tend to render more clearly at a larger size.  So
	// we up-scale to render the fonts then down-scale to achieve the desired
	// on-screen size.
	// KRL - Tests 6/8/2020; appears this may no longer be true.
	// TODO:  OGL4 Better to use a fixed large size and adjust scale accordingly?
	const double factor(3.0);
	mLabelText.SetSize(size);// * factor);
	mValueText.SetSize(size);// * factor);

	mLabelText.SetScale(1.0);// / factor);
	mValueText.SetScale(1.0);// / factor);

	return true;
}

}// namespace LibPlot2D
