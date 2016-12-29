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
//		renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
Axis::Axis(RenderWindow &renderWindow) : Primitive(renderWindow), labelText(renderWindow),
	valueText(renderWindow), axisLines(renderWindow), gridLines(renderWindow)
{
	color.Set(0.0, 0.0, 0.0, 1.0);

	orientation = Orientation::Bottom;
	tickStyle = TickStyle::Through;

	tickSize = 7;

	minimum = 0.0;
	maximum = 1.0;
	majorResolution = 1.0;
	minorResolution = 1.0;

	offsetFromWindowEdge = 75;// [pixels]

	majorGrid = false;
	minorGrid = false;

	logarithmic = false;

	minAxis = nullptr;
	maxAxis = nullptr;
	oppositeAxis = nullptr;

	gridColor.Set(0.8, 0.8, 0.8, 1.0);
	SetDrawOrder(500);

	bufferInfo.resize(4);// Main axis line, gridlines, values and label
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
		axisLines.SetWidth(1.0);
		axisLines.SetLineColor(color);
		axisLines.SetBackgroundColorForAlphaFade();

		gridLines.SetWidth(1.0);
		gridLines.SetLineColor(gridColor);
		gridLines.SetBackgroundColorForAlphaFade();

		axisPoints.clear();
		gridPoints.clear();
		DrawFullAxis();
		axisLines.BuildSegments(axisPoints, bufferInfo[i]);
	}
	else if (i == 1)// Gridlines
	{
		gridLines.BuildSegments(gridPoints, bufferInfo[i]);
	}
	else if (i == 2 && valueText.IsOK())// Values
	{
		DrawTickLabels();
		bufferInfo[i] = valueText.BuildText();
	}
	else if (i == 3 && labelText.IsOK())// Label
	{
		DrawAxisLabel();
		bufferInfo[i] = labelText.BuildText();
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
	if ((majorGrid || minorGrid) && bufferInfo[1].indexBuffer.size() > 0)
	{
		glBindVertexArray(bufferInfo[1].GetVertexArrayIndex());
		Line::DoPrettyDraw(bufferInfo[1].indexBuffer.size());
	}

	// Axis and ticks next
	if (bufferInfo[0].vertexCount > 0)
	{
		glBindVertexArray(bufferInfo[0].GetVertexArrayIndex());
		Line::DoPrettyDraw(bufferInfo[0].indexBuffer.size());
	}

	if (valueText.IsOK() && bufferInfo[2].vertexCount > 0)
	{
		glBindVertexArray(bufferInfo[2].GetVertexArrayIndex());
		valueText.RenderBufferedGlyph(bufferInfo[2].vertexCount);
	}

	if (!label.IsEmpty() && labelText.IsOK() && bufferInfo[3].vertexCount > 0)
	{
		glBindVertexArray(bufferInfo[3].GetVertexArrayIndex());
		labelText.RenderBufferedGlyph(bufferInfo[3].vertexCount);
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
		if ((majorGrid || minorGrid) && oppositeAxis)
			DrawHorizontalGrid(numberOfGridLines);

		if (tickStyle != TickStyle::NoTicks)
			DrawHorizontalTicks(numberOfTicks, mainAxisLocation);
	}
	else
	{
		if ((majorGrid || minorGrid) && oppositeAxis)
			DrawVerticalGrid(numberOfGridLines);

		if (tickStyle != TickStyle::NoTicks)
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
	if (orientation == Orientation::Bottom || orientation == Orientation::Left)
		return offsetFromWindowEdge;
	else if (orientation == Orientation::Right)
		return renderWindow.GetSize().GetWidth() - offsetFromWindowEdge;

	//else// OrientationTop
	return renderWindow.GetSize().GetHeight() - offsetFromWindowEdge;
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
	if (logarithmic)
	{
		// Only allow strictly positive limits
		if (minimum <= 0.0)
			minimum = 0.1;
		if (maximum <= minimum)
			maximum = 10.0 * minimum;

		tickCount = ceil(log10(maximum)) - floor(log10(minimum)) - 1;
		if (gridCount)
			*gridCount = (tickCount + 1) * 8 + tickCount;
	}
	else
	{
		assert(majorResolution > 0.0 && PlotMath::IsValid(majorResolution));
		assert(minorResolution > 0.0 && PlotMath::IsValid(minorResolution));
		tickCount = (unsigned int)((maximum - minimum) / majorResolution + 0.5) - 1;
		if (gridCount)
		{
			if (minorGrid)
				*gridCount = (unsigned int)((maximum - minimum) / minorResolution + 0.5) - 1;
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
		axisPoints.push_back(std::make_pair(minAxis->GetOffsetFromWindowEdge(), mainAxisLocation));
		axisPoints.push_back(std::make_pair(renderWindow.GetSize().GetWidth()
			- maxAxis->GetOffsetFromWindowEdge(), mainAxisLocation));
	}
	else
	{
		axisPoints.push_back(std::make_pair(mainAxisLocation, minAxis->GetOffsetFromWindowEdge()));
		axisPoints.push_back(std::make_pair(mainAxisLocation, renderWindow.GetSize().GetHeight() -
			maxAxis->GetOffsetFromWindowEdge()));
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

	if (tickStyle == TickStyle::Inside)
		inside = 1.0;
	else if (tickStyle == TickStyle::Outside)
		outside = 1.0;
	else if (tickStyle == TickStyle::Through)
	{
		inside = 0.5;
		outside = 0.5;
	}

	if (orientation == Orientation::Top || orientation == Orientation::Right)
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
		if (minorGrid)
			location = ValueToPixel(GetNextGridValue(grid + 1));
		else
			location = ValueToPixel(GetNextTickValue(false, false, grid + 1));

		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetWidth() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		gridPoints.push_back(std::make_pair(location, static_cast<double>(offsetFromWindowEdge)));
		gridPoints.push_back(std::make_pair(location,
			static_cast<double>(renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge())));
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
		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetWidth() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		axisPoints.push_back(std::make_pair(location, static_cast<double>(mainAxisLocation - tickSize * outsideTick * sign)));
		axisPoints.push_back(std::make_pair(location, static_cast<double>(mainAxisLocation + tickSize * insideTick * sign)));
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
		if (minorGrid)
			location = ValueToPixel(GetNextGridValue(grid + 1));
		else
			location = ValueToPixel(GetNextTickValue(false, false, grid + 1));

		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetHeight() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		gridPoints.push_back(std::make_pair(static_cast<double>(offsetFromWindowEdge), location));
		gridPoints.push_back(std::make_pair(static_cast<double>(renderWindow.GetSize().GetWidth()
			- oppositeAxis->GetOffsetFromWindowEdge()), location));
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
		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetHeight() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		axisPoints.push_back(std::make_pair(static_cast<double>(mainAxisLocation - tickSize * outsideTick * sign), location));
		axisPoints.push_back(std::make_pair(static_cast<double>(mainAxisLocation + tickSize * insideTick * sign), location));
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
		value = pow(10.0, floor(log10(minimum)));
		int scale(1);
		while (value * scale <= minimum)
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
	if (label.IsEmpty())
		return;

	double fontOffsetFromWindowEdge = offsetFromWindowEdge / 3.0;
	if (!IsHorizontal())
		fontOffsetFromWindowEdge /= 2.0;

	// TODO:  Change plot dimension if there is a title? or if there is a title and a label for the top axis?
	Text::BoundingBox boundingBox = labelText.GetBoundingBox("H");// Some capital letter to assure uniform spacing
	double edgeOffset = GetAxisLabelTranslation(fontOffsetFromWindowEdge, boundingBox.yUp);

	boundingBox = labelText.GetBoundingBox(label.ToStdString());
	double textWidth = boundingBox.xRight - boundingBox.xLeft;
	double plotOffset = (double)minAxis->GetOffsetFromWindowEdge() - (double)maxAxis->GetOffsetFromWindowEdge();

	if (IsHorizontal())
		labelText.SetPosition(0.5 * (renderWindow.GetSize().GetWidth() - textWidth + plotOffset), edgeOffset);
	else
	{
		labelText.SetOrientation(M_PI * 0.5);
		labelText.SetPosition(0.5 * (renderWindow.GetSize().GetHeight() - textWidth + plotOffset), -edgeOffset);
	}

	labelText.SetText(label.ToStdString());
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
//		double indicating the translation to use for this label
//
//=============================================================================
double Axis::GetAxisLabelTranslation(const double &offset, const double &fontHeight) const
{
	switch (orientation)
	{
	case Orientation::Bottom:
		return offset;

	case Orientation::Left:
		return offset + fontHeight;

	case Orientation::Top:
		return renderWindow.GetSize().GetHeight() - offset - fontHeight;

	case Orientation::Right:
		return renderWindow.GetSize().GetWidth() - offset;

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
	unsigned int precision = GetPrecision();

	if (!wxString::Format("%0.*f", precision, minimum).ToDouble(&minimum)) { /*Warn the user?*/ }

	wxString valueLabel;
	double valueOffsetFromEdge = offsetFromWindowEdge * 0.8;
	unsigned int tick, numberOfTicks;
	ComputeGridAndTickCounts(numberOfTicks);
	for (tick = 0; tick < numberOfTicks + 2; ++tick)
	{
		double value(std::min(GetNextTickValue(tick == 0,
			tick == numberOfTicks + 1, tick), maximum));
		valueLabel.Printf("%0.*f", precision, value);

		// TODO:  Don't draw it if it's too close to the maximum (based on text size)
		ComputeTranslations(value, xTranslation, yTranslation,
			valueText.GetBoundingBox(valueLabel.ToStdString()), valueOffsetFromEdge);
		valueText.SetPosition(xTranslation, yTranslation);
		valueText.AppendText(valueLabel.ToStdString());
	}

	valueLabel.Printf("%0.*f", precision, maximum);
	if (!valueLabel.ToDouble(&maximum)) { /*Warn the user?*/ }
}

//=============================================================================
// Class:			Axis
// Function:		GetPrecision
//
// Description:		Determines appropriate precision to use for axis resolution.
//					Ideal precision is determined by the minimum number of digits
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
	// It does look nicer to use the raw return value of GetPrecision(), but it affects the function
	// of dragging the plot around.  Because we always force the limits to actually
	// match the printed values, it makes the dragging operation very coarse.  So we add two.
	return PlotMath::GetPrecision(minimum, majorResolution, logarithmic) + 2;
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
double Axis::GetNextTickValue(const bool &first, const bool &last, const unsigned int &tick) const
{
	if (logarithmic)
	{
		if (first)
			return minimum;
		else if (last)
			return maximum;
		else
			return pow(10.0, floor(log10(minimum)) + tick);
	}

	return minimum + (double)tick * majorResolution;
}

//=============================================================================
// Class:			Axis
// Function:		ComputeTranslations
//
// Description:		Computes the translations required for the specified bounding box.
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
void Axis::ComputeTranslations(const double &value, float &xTranslation, float &yTranslation,
	const Text::BoundingBox &boundingBox, const double &offset) const
{
	if (IsHorizontal())
	{
		if (orientation == Orientation::Bottom)
			yTranslation = offset - boundingBox.yUp;
		else
			yTranslation = renderWindow.GetSize().GetHeight() - offset;

		xTranslation = ValueToPixel(value) -
			(boundingBox.xRight - boundingBox.xLeft) / 2.0;
	}
	else
	{
		if (orientation == Orientation::Left)
			xTranslation = offset - boundingBox.xRight;
		else
			xTranslation = renderWindow.GetSize().GetWidth() - offset;

		yTranslation = ValueToPixel(value) -
			(boundingBox.yUp - boundingBox.yDown) / 2.0;
	}
}

//=============================================================================
// Class:			Axis
// Function:		IsHorizontal
//
// Description:		Checks to see if this object has horizontal orientation.
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
	if (orientation == Orientation::Bottom || orientation == Orientation::Top)
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
	if (PlotMath::IsNaN(minimum) || PlotMath::IsNaN(maximum))
		return false;

	// Make sure the pointers to the perpendicular axes have been provided
	if (!minAxis || !maxAxis)
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
		plotDimension = renderWindow.GetSize().GetWidth()
				- minAxis->GetOffsetFromWindowEdge()
				- maxAxis->GetOffsetFromWindowEdge();
	else
		plotDimension = renderWindow.GetSize().GetHeight()
				- minAxis->GetOffsetFromWindowEdge()
				- maxAxis->GetOffsetFromWindowEdge();

	// Do the scaling
	if (IsLogarithmic())
	{
		if (value <= 0.0 || minimum <= 0.0)
			return GetOffsetFromWindowEdge();
		else
			return minAxis->GetOffsetFromWindowEdge()
				+ (log10(value) - log10(minimum)) /
				(log10(maximum) - log10(minimum)) * plotDimension;
	}

	return minAxis->GetOffsetFromWindowEdge()
		+ (value - minimum) /
		(maximum - minimum) * plotDimension;
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
		fraction = double(pixel - (double)minAxis->GetOffsetFromWindowEdge()) / double(renderWindow.GetSize().GetWidth()
				- (double)minAxis->GetOffsetFromWindowEdge()
				- (double)maxAxis->GetOffsetFromWindowEdge());
	else
		fraction = double(pixel - (double)minAxis->GetOffsetFromWindowEdge()) / double(renderWindow.GetSize().GetHeight()
				- (double)minAxis->GetOffsetFromWindowEdge()
				- (double)maxAxis->GetOffsetFromWindowEdge());

	// Do the scaling
	if (IsLogarithmic())
		return pow(10.0, fraction * (log10(maximum) - log10(minimum)) + log10(minimum));

	return fraction * (maximum - minimum) + minimum;
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
	if (logarithmic)
		return pow(10.0, floor(log10(minimum)) + floor(tick / 9.0))
			* (tick - 9.0 * floor(tick / 9.0) + 1.0);

	return minimum + (double)tick * minorResolution;
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
	if (orientation == Orientation::Top || orientation == Orientation::Bottom)
	{
		return renderWindow.GetSize().GetWidth()
			- minAxis->GetOffsetFromWindowEdge()
			- maxAxis->GetOffsetFromWindowEdge();
	}
	else
	{
		return renderWindow.GetSize().GetHeight()
			- minAxis->GetOffsetFromWindowEdge()
			- maxAxis->GetOffsetFromWindowEdge();
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
	if (!labelText.SetFace(fontFileName) || !valueText.SetFace(fontFileName))
		return false;

	labelText.SetColor(color);
	valueText.SetColor(color);

	// For some reason, fonts tend to render more clearly at a larger size.  So
	// we up-scale to render the fonts then down-scale to achieve the desired
	// on-screen size.
	// TODO:  OGL4 Better to use a fixed large size and adjust scale accordingly?
	const double factor(3.0);
	labelText.SetSize(size * factor);
	valueText.SetSize(size * factor);

	labelText.SetScale(1.0 / factor);
	valueText.SetScale(1.0 / factor);

	return true;
}

}// namespace LibPlot2D
