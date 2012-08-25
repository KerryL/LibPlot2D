/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  axis.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating axis objects on a plot.
// History:
//	11/17/2010	- Fixed some bugs related to rendering of ticks and grid lines, K. Loux.
//	07/30/2012	- Added logarithmically-scalled plotting, K. Loux.

// Local headers
#include "renderer/primitives/axis.h"
#include "renderer/renderWindow.h"
#include "utilities/math/plotMath.h"

// FTGL headers
#include <FTGL/ftgl.h>

//==========================================================================
// Class:			Axis
// Function:		Axis
//
// Description:		Constructor for the Axis class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Axis::Axis(RenderWindow &_renderWindow) : Primitive(_renderWindow)
{
	color.Set(0.0, 0.0, 0.0, 1.0);

	orientation = OrientationBottom;
	tickStyle = TickStyleThrough;

	tickSize = 7;

	minimum = 0.0;
	maximum = 1.0;
	majorResolution = 1.0;
	minorResolution = 1.0;

	offsetFromWindowEdge = 75;// [pixels]

	grid = false;

	logarithmic = false;

	font = NULL;
	
	minAxis = NULL;
	maxAxis = NULL;
	oppositeAxis = NULL;

	gridColor.Set(0.8, 0.8, 0.8, 1.0);
}

//==========================================================================
// Class:			Axis
// Function:		~Axis
//
// Description:		Destructor for the Axis class.
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
Axis::~Axis()
{
}

//==========================================================================
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
//==========================================================================
void Axis::GenerateGeometry(void)
{
	DrawFullAxis();

	if (font)
	{
		if (!label.IsEmpty())
			DrawAxisLabel();

		DrawTickLabels();
	}
}

//==========================================================================
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
//==========================================================================
void Axis::DrawFullAxis(void)
{
	double tickSpacing, gridSpacing;// To avoid rounding errors, we convert this back to an int after we do some math
	int numberOfTicks, numberOfGridLines;
	int mainAxisLocation = ComputeMainAxisLocation();

	ComputeGridAndTickCounts(numberOfTicks, &numberOfGridLines);

	glLineWidth(1.0f);
	glBegin(GL_LINES);

	int axisLength = DrawMainAxis(mainAxisLocation);
	gridSpacing = (double)axisLength / double(numberOfGridLines + 1);
	tickSpacing = (double)axisLength / double(numberOfTicks + 1);

	if (IsHorizontal())
	{
		if (grid && oppositeAxis)
			DrawHorizontalGrid(numberOfGridLines, gridSpacing);

		if (tickStyle != TickStyleNone)
			DrawHorizontalTicks(numberOfTicks, mainAxisLocation, tickSpacing);
	}
	else
	{
		if (grid && oppositeAxis)
			DrawVerticalGrid(numberOfGridLines, gridSpacing);

		if (tickStyle != TickStyleNone)
			DrawVerticalTicks(numberOfTicks, mainAxisLocation, tickSpacing);
	}

	glEnd();
}

//==========================================================================
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
//==========================================================================
int Axis::ComputeMainAxisLocation(void) const
{
	// Compute the mainAxisLocation (X for vertical axis, Y for horizontal axis)
	if (orientation == OrientationBottom || orientation == OrientationLeft)
		return offsetFromWindowEdge;
	else if (orientation == OrientationRight)
		return renderWindow.GetSize().GetWidth() - offsetFromWindowEdge;

	//else// OrientationTop
	return renderWindow.GetSize().GetHeight() - offsetFromWindowEdge;
}

//==========================================================================
// Class:			Axis
// Function:		ComputeGridAndTickCounts
//
// Description:		Calculates the number of tick marks and (optionally) grid lines.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		tickCount	= int&
//		gridCount	= int*
//
// Return Value:
//		None
//
//==========================================================================
void Axis::ComputeGridAndTickCounts(int &tickCount, int *gridCount)
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
		tickCount = int((maximum - minimum) / minorResolution + 0.5) - 1;
		if (gridCount)
			*gridCount = int((maximum - minimum) / majorResolution + 0.5) - 1;
	}
}

//==========================================================================
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
//		int, indicating the length of the axis on screen
//
//==========================================================================
int Axis::DrawMainAxis(const int &mainAxisLocation) const
{
	int axisLength;
	if (IsHorizontal())
	{
		axisLength = renderWindow.GetSize().GetWidth() -
				minAxis->GetOffsetFromWindowEdge() - maxAxis->GetOffsetFromWindowEdge();
		glVertex2i(minAxis->GetOffsetFromWindowEdge(), mainAxisLocation);
		glVertex2i(renderWindow.GetSize().GetWidth()
				- maxAxis->GetOffsetFromWindowEdge(), mainAxisLocation);
	}
	else
	{
		axisLength = renderWindow.GetSize().GetHeight() -
				minAxis->GetOffsetFromWindowEdge() - maxAxis->GetOffsetFromWindowEdge();
		glVertex2i(mainAxisLocation, minAxis->GetOffsetFromWindowEdge());
		glVertex2i(mainAxisLocation, renderWindow.GetSize().GetHeight() -
				maxAxis->GetOffsetFromWindowEdge());
	}

	return axisLength;
}

//==========================================================================
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
//==========================================================================
void Axis::InitializeTickParameters(int &inside, int &outside, int &sign) const
{
	inside = 0;
	outside = 0;
	sign = 1;

	if (tickStyle == TickStyleInside)
		inside = 1.0;
	else if (tickStyle == TickStyleOutside)
		outside = 1.0;
	else if (tickStyle == TickStyleThrough)
	{
		inside = 0.5;
		outside = 0.5;
	}

	if (orientation == OrientationTop || orientation == OrientationRight)
		sign = -1.0;
}

//==========================================================================
// Class:			Axis
// Function:		DrawHorizontalGrid
//
// Description:		Draws grid lines for horizontal axes.
//
// Input Arguments:
//		count	= const unsigned int&
//		spacing	= double
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawHorizontalGrid(const unsigned int &count, double spacing) const
{
	glColor4d(gridColor.GetRed(), gridColor.GetGreen(), gridColor.GetBlue(), gridColor.GetAlpha());

	unsigned int grid;
	for (grid = 0; grid < count; grid++)
	{
		if (logarithmic)
		{
			GetNextLogValue(grid == 0, spacing);
			if (spacing >= maximum)
				break;

			glVertex2i(ValueToPixel(spacing), offsetFromWindowEdge);
			glVertex2i(ValueToPixel(spacing),
				renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge());
		}
		else
		{
			glVertex2i(minAxis->GetOffsetFromWindowEdge() + (grid + 1) * spacing, offsetFromWindowEdge);
			glVertex2i(minAxis->GetOffsetFromWindowEdge() + (grid + 1) * spacing,
				renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge());
		}
	}

	glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

//==========================================================================
// Class:			Axis
// Function:		DrawHorizontalTicks
//
// Description:		Draws tick marks for horizontal axes.
//
// Input Arguments:
//		count				= const unsigned int&
//		mainAxisLocation	= const int&
//		spacing				= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawHorizontalTicks(const unsigned int &count, const int &mainAxisLocation, const double &spacing) const
{
	int insideTick, outsideTick, sign;
	InitializeTickParameters(insideTick, outsideTick, sign);

	// The first and last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
	unsigned int tick;
	for (tick = 1; tick <= count; tick++)
	{
		if (logarithmic)
		{
			unsigned int location = ValueToPixel(pow(10.0, floor(log10(minimum)) + tick));
			glVertex2i(location, mainAxisLocation - tickSize * outsideTick * sign);
			glVertex2i(location, mainAxisLocation + tickSize * insideTick * sign);
		}
		else
		{
			glVertex2i(minAxis->GetOffsetFromWindowEdge() + tick * spacing,
				mainAxisLocation - tickSize * outsideTick * sign);
			glVertex2i(minAxis->GetOffsetFromWindowEdge() + tick * spacing,
				mainAxisLocation + tickSize * insideTick * sign);
		}
	}
}

//==========================================================================
// Class:			Axis
// Function:		DrawVerticalGrid
//
// Description:		Draws grid lines for vertical axes.
//
// Input Arguments:
//		count	= const unsigned int&
//		spacing	= double
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawVerticalGrid(const unsigned int &count, double spacing) const
{
	glColor4d(gridColor.GetRed(), gridColor.GetGreen(), gridColor.GetBlue(), gridColor.GetAlpha());

	unsigned int grid;
	for (grid = 0; grid < count; grid++)
	{
		if (logarithmic)
		{
			GetNextLogValue(grid == 0, spacing);
			if (spacing >= maximum)
				break;

			glVertex2i(offsetFromWindowEdge, ValueToPixel(spacing));
			glVertex2i(renderWindow.GetSize().GetWidth() - oppositeAxis->GetOffsetFromWindowEdge(),
				ValueToPixel(spacing));
		}
		else
		{
			glVertex2i(offsetFromWindowEdge, minAxis->GetOffsetFromWindowEdge() + (grid + 1) * spacing);
			glVertex2i(renderWindow.GetSize().GetWidth() - oppositeAxis->GetOffsetFromWindowEdge(),
				minAxis->GetOffsetFromWindowEdge() + (grid + 1) * spacing);
		}
	}

	glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
}

//==========================================================================
// Class:			Axis
// Function:		DrawVerticalTicks
//
// Description:		Draws tick marks for vertical axes.
//
// Input Arguments:
//		count				= const unsigned int&
//		mainAxisLocation	= const int&
//		spacing				= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawVerticalTicks(const unsigned int &count, const int &mainAxisLocation, const double &spacing) const
{
	int insideTick, outsideTick, sign;
	InitializeTickParameters(insideTick, outsideTick, sign);

	// The first and last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
	unsigned int tick;
	for (tick = 1; tick <= count; tick++)
	{
		if (logarithmic)
		{
			unsigned int location = ValueToPixel(pow(10.0, floor(log10(minimum)) + tick));
			glVertex2i(mainAxisLocation - tickSize * outsideTick * sign, location);
			glVertex2i(mainAxisLocation + tickSize * insideTick * sign, location);
		}
		else
		{
			glVertex2i(mainAxisLocation - tickSize * outsideTick * sign,
				minAxis->GetOffsetFromWindowEdge() + tick * spacing);
			glVertex2i(mainAxisLocation + tickSize * insideTick * sign,
				minAxis->GetOffsetFromWindowEdge() + tick * spacing);
		}
	}
}

//==========================================================================
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
//==========================================================================
void Axis::GetNextLogValue(const bool &first, double &value) const
{
	if (first)
	{
		value = pow(10.0, floor(log10(minimum)));
		int scale(1);
		while (value * scale <= minimum)
			scale++;
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

//==========================================================================
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
//==========================================================================
void Axis::DrawAxisLabel(void) const
{
	// TODO:  Should add something here to center text over the plot area, not the render window
	double fontOffsetFromWindowEdge = offsetFromWindowEdge / 3.0;
	if (!IsHorizontal())
		fontOffsetFromWindowEdge /= 2.0;

	// TODO:  Change plot dimension if there is a title? or if there is a title and a label for the top axis?
	FTBBox boundingBox = font->BBox("H");// Some capital letter to assure uniform spacing
	double yTranslation = GetAxisLabelTranslation(fontOffsetFromWindowEdge, boundingBox.Upper().Y());

	glPushMatrix();
		glLoadIdentity();

		boundingBox = font->BBox(label.c_str());

		if (IsHorizontal())
			glTranslated((renderWindow.GetSize().GetWidth() + boundingBox.Lower().X()
				- boundingBox.Upper().X()) / 2.0, yTranslation, 0.0);
		else
		{
			glRotated(90.0, 0.0, 0.0, 1.0);
			glTranslated((boundingBox.Lower().X() - boundingBox.Upper().X()
				+ renderWindow.GetSize().GetHeight()) / 2.0, -yTranslation, 0.0);
		}

		font->Render(label.c_str());
	glPopMatrix();
}

//==========================================================================
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
//==========================================================================
double Axis::GetAxisLabelTranslation(const double &offset, const double &fontHeight) const
{
	switch (orientation)
	{
	case OrientationBottom:
		return offset;

	case OrientationLeft:
		return offset + fontHeight;

	case OrientationTop:
		return renderWindow.GetSize().GetHeight() - offset - fontHeight;

	case OrientationRight:
		return renderWindow.GetSize().GetWidth() - offset;

	default:
		assert(false);
		return 0.0;
	}
}

//==========================================================================
// Class:			Axis
// Function:		DrawTickLabels
//
// Description:		Draws the numeric labels for each axis tick.  This also
//					determines the precision for each tick label.  The goal
//					is to provide just enough precision so that adjacent tick
//					marks are distinguishable, and then add just a hair more.
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
void Axis::DrawTickLabels(void)
{
	FTBBox boundingBox;
	int xTranslation, yTranslation;
	unsigned int precision = GetPrecision();

	// Set the maximum and minimum to be exactly the values shown after rounding
	if (!wxString::Format("%0.*f", precision, minimum).ToDouble(&minimum)) { /*Warn the user?*/ }

	double value;
	wxString valueLabel;
	double valueOffsetFromEdge = offsetFromWindowEdge * 0.8;
	int tick, numberOfTicks;
	ComputeGridAndTickCounts(numberOfTicks);
	for (tick = 0; tick < numberOfTicks + 2; tick++)
	{
		value = std::min(GetNextTickValue(tick == 0, tick == numberOfTicks + 1, tick), maximum);
		valueLabel.Printf("%0.*f", precision, value);

		glPushMatrix();
			glLoadIdentity();
			ComputeTranslations(value, xTranslation, yTranslation, font->BBox(valueLabel), valueOffsetFromEdge);
			glTranslated(xTranslation, yTranslation, 0.0);
			font->Render(valueLabel);
		glPopMatrix();
	}

	if (!valueLabel.ToDouble(&maximum)) { /*Warn the user?*/ }
}

//==========================================================================
// Class:			Axis
// Function:		GetPrecision
//
// Description:		Determines appropriate precision to use for axis resolution.
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
//==========================================================================
unsigned int Axis::GetPrecision(void) const
{
	unsigned int precision;
	double baseValue;
	if (logarithmic)
		baseValue = minimum;
	else
		baseValue = majorResolution;

	if (log10(baseValue) >= 0.0)
		precision = 0;
	else
		precision = -log10(baseValue) + 1;

	precision += 2;

	return precision;
}

//==========================================================================
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
//==========================================================================
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

//==========================================================================
// Class:			Axis
// Function:		ComputeTranslations
//
// Description:		Computes the translations required for the specified bounding box.
//
// Input Arguments:
//		value	= 
//		last	= const bool&
//		tick	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
void Axis::ComputeTranslations(const double &value, int &xTranslation, int &yTranslation,
	const FTBBox &boundingBox, const double &offset) const
{
	if (IsHorizontal())
	{
		if (orientation == OrientationBottom)
			yTranslation = offset - boundingBox.Upper().Y();
		else
			yTranslation = renderWindow.GetSize().GetHeight() - offset;

		xTranslation = ValueToPixel(value) -
			(boundingBox.Upper().X() - boundingBox.Lower().X()) / 2.0;
	}
	else
	{
		if (orientation == OrientationLeft)
			xTranslation = offset - boundingBox.Upper().X();
		else
			xTranslation = renderWindow.GetSize().GetWidth() - offset;

		yTranslation = ValueToPixel(value) -
			(boundingBox.Upper().Y() - boundingBox.Lower().Y()) / 2.0;
	}
}

//==========================================================================
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
//==========================================================================
bool Axis::IsHorizontal(void) const
{
	if (orientation == OrientationBottom || orientation == OrientationTop)
		return true;

	return false;
}

//==========================================================================
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
//==========================================================================
bool Axis::HasValidParameters(void)
{
	// Don't draw if any of the limits are not numbers
	if (PlotMath::IsNaN(minimum) || PlotMath::IsNaN(maximum))
		return false;
	
	// Make sure the pointers to the perpendicular axes have been provided
	if (!minAxis || !maxAxis)
		return false;

	return true;
}

//==========================================================================
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
//		int specifying the location of the object in screen coordinates
//
//==========================================================================
int Axis::ValueToPixel(const double &value) const
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

//==========================================================================
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
//==========================================================================
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