/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  axis.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating axis objects on a plot.
// History:
//	11/17/2010	- Fixed some bugs related to rendering of ticks and grid lines, K. Loux.
//	07/30/2012	- Added logarithmically-scalled plotting, K. Loux.

// Standard C++ headers
#include <algorithm>

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
//		renderWindow	= RenderWindow& reference to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Axis::Axis(RenderWindow &renderWindow) : Primitive(renderWindow), line(renderWindow)
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

	majorGrid = false;
	minorGrid = false;

	logarithmic = false;

	font = NULL;

	minAxis = NULL;
	maxAxis = NULL;
	oppositeAxis = NULL;

	gridColor.Set(0.8, 0.8, 0.8, 1.0);
	SetDrawOrder(2500);

	bufferInfo.push_back(BufferInfo());// Ticks
	bufferInfo.push_back(BufferInfo());// Values
	bufferInfo.push_back(BufferInfo());// Gridlines
	bufferInfo.push_back(BufferInfo());// Label
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
void Axis::Update(const unsigned int& i)
{
	// TODO:  Can we do it this way?  Maybe need dynamic vectors of Lines?
	if (i == 0)// Axis
	{
		// TODO
	}
	else if (i == 1)// Ticks
	{
		// TODO
	}
	else if (i == 2)// Values
	{
		// TODO
	}
	else if (i == 3)// Gridlines
	{
		// TODO
	}
	else if (i == 4)// Label
	{
		// TODO
	}
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
void Axis::GenerateGeometry()
{
	/*DrawFullAxis();

	glColor4d(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());

	if (font)
	{
		if (!label.IsEmpty())
			DrawAxisLabel();

		DrawTickLabels();
	}*/
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
void Axis::DrawFullAxis()
{
	unsigned int numberOfTicks, numberOfGridLines;
	int mainAxisLocation = ComputeMainAxisLocation();

	ComputeGridAndTickCounts(numberOfTicks, &numberOfGridLines);

	line.SetWidth(1.0);
	line.SetLineColor(color);
	line.SetBackgroundColorForAlphaFade();

	if (IsHorizontal())
	{
		if ((majorGrid || minorGrid) && oppositeAxis)
			DrawHorizontalGrid(numberOfGridLines);

		if (tickStyle != TickStyleNone)
			DrawHorizontalTicks(numberOfTicks, mainAxisLocation);
	}
	else
	{
		if ((majorGrid || minorGrid) && oppositeAxis)
			DrawVerticalGrid(numberOfGridLines);

		if (tickStyle != TickStyleNone)
			DrawVerticalTicks(numberOfTicks, mainAxisLocation);
	}

	DrawMainAxis(mainAxisLocation);
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
int Axis::ComputeMainAxisLocation() const
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
//		tickCount	= unsigned int&
//		gridCount	= unsigned int*
//
// Return Value:
//		None
//
//==========================================================================
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
//		None
//
//==========================================================================
void Axis::DrawMainAxis(const int &mainAxisLocation) const
{
	// TODO:  Update for OGL4
	/*if (IsHorizontal())
		line.Draw(minAxis->GetOffsetFromWindowEdge(), mainAxisLocation,
			renderWindow.GetSize().GetWidth()
			- maxAxis->GetOffsetFromWindowEdge(), mainAxisLocation);
	else
		line.Draw(mainAxisLocation, minAxis->GetOffsetFromWindowEdge(),
			mainAxisLocation, renderWindow.GetSize().GetHeight() -
			maxAxis->GetOffsetFromWindowEdge());*/
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
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawHorizontalGrid(const unsigned int &count) const
{
	// TODO:  Update for OGL4
	/*Line gridLine;
	gridLine.SetLineColor(gridColor);
	gridLine.SetBackgroundColorForAlphaFade();

	// The first and last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
	unsigned int grid;
	double location;
	for (grid = 1; grid <= count + 1; grid++)
	{
		if (minorGrid)
			location = ValueToPixel(GetNextGridValue(grid));
		else
			location = ValueToPixel(GetNextTickValue(false, false, grid));

		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetWidth() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		gridLine.Draw(location, static_cast<double>(offsetFromWindowEdge), location,
			static_cast<double>(renderWindow.GetSize().GetHeight() - oppositeAxis->GetOffsetFromWindowEdge()));
	}*/
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
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawHorizontalTicks(const unsigned int &count, const int &mainAxisLocation) const
{
	// TODO:  Update for OGL4
	/*int insideTick, outsideTick, sign;
	InitializeTickParameters(insideTick, outsideTick, sign);

	// The first and last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
	unsigned int tick;
	double location;
	for (tick = 1; tick <= count + 1; tick++)
	{
		location = ValueToPixel(GetNextTickValue(false, false, tick));
		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetWidth() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		line.Draw(location, static_cast<double>(mainAxisLocation - tickSize * outsideTick * sign),
			location, static_cast<double>(mainAxisLocation + tickSize * insideTick * sign));
	}*/
}

//==========================================================================
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
//==========================================================================
void Axis::DrawVerticalGrid(const unsigned int &count) const
{
	// TODO:  Update for OGL4
	/*Line gridLine;
	gridLine.SetLineColor(gridColor);
	gridLine.SetBackgroundColorForAlphaFade();

	// The first and last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
	unsigned int grid;
	double location;
	for (grid = 1; grid <= count + 1; grid++)
	{
		if (minorGrid)
			location = ValueToPixel(GetNextGridValue(grid));
		else
			location = ValueToPixel(GetNextTickValue(false, false, grid));

		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetHeight() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		gridLine.Draw(static_cast<double>(offsetFromWindowEdge), location,
			static_cast<double>(renderWindow.GetSize().GetWidth() - oppositeAxis->GetOffsetFromWindowEdge()), location);
	}*/
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
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Axis::DrawVerticalTicks(const unsigned int &count, const int &mainAxisLocation) const
{
	// TODO:  Update for OGL4
	/*
	int insideTick, outsideTick, sign;
	InitializeTickParameters(insideTick, outsideTick, sign);

	// The first inside tick never needs to be drawn
	// The last inside ticks do not need to be drawn, thus we start this loop with tick = 1.
	unsigned int tick;
	double location;
	for (tick = 1; tick <= count + 1; tick++)
	{
		location = ValueToPixel(GetNextTickValue(false, false, tick));
		if (location <= minAxis->GetOffsetFromWindowEdge() ||
			location >= renderWindow.GetSize().GetHeight() - maxAxis->GetOffsetFromWindowEdge())
			continue;

		line.Draw(static_cast<double>(mainAxisLocation - tickSize * outsideTick * sign), location,
			static_cast<double>(mainAxisLocation + tickSize * insideTick * sign), location);
	}*/
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
void Axis::DrawAxisLabel() const
{
	// TODO:  Update for OGL4
	/*
	double fontOffsetFromWindowEdge = offsetFromWindowEdge / 3.0;
	if (!IsHorizontal())
		fontOffsetFromWindowEdge /= 2.0;

	// TODO:  Change plot dimension if there is a title? or if there is a title and a label for the top axis?
	FTBBox boundingBox = font->BBox("H");// Some capital letter to assure uniform spacing
	double yTranslation = GetAxisLabelTranslation(fontOffsetFromWindowEdge, boundingBox.Upper().Y());

	boundingBox = font->BBox(label.mb_str());
	double textWidth = boundingBox.Upper().X() - boundingBox.Lower().X();
	double plotOffset = (double)minAxis->GetOffsetFromWindowEdge() - (double)maxAxis->GetOffsetFromWindowEdge();

	glPushMatrix();
		glLoadIdentity();

		if (IsHorizontal())
			glTranslated(0.5 * (renderWindow.GetSize().GetWidth() - textWidth + plotOffset), yTranslation, 0.0);
		else
		{
			glRotated(90.0, 0.0, 0.0, 1.0);
			glTranslated(0.5 * (renderWindow.GetSize().GetHeight() - textWidth + plotOffset), -yTranslation, 0.0);
		}

		font->Render(label.mb_str());
	glPopMatrix();*/
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
//==========================================================================
void Axis::DrawTickLabels()
{
	FTBBox boundingBox;
	int xTranslation, yTranslation;
	unsigned int precision = GetPrecision();

	if (!wxString::Format("%0.*f", precision, minimum).ToDouble(&minimum)) { /*Warn the user?*/ }

	double value;
	wxString valueLabel;
	double valueOffsetFromEdge = offsetFromWindowEdge * 0.8;
	unsigned int tick, numberOfTicks;
	ComputeGridAndTickCounts(numberOfTicks);
	for (tick = 0; tick < numberOfTicks + 2; tick++)
	{
		value = std::min(GetNextTickValue(tick == 0, tick == numberOfTicks + 1, tick), maximum);
		valueLabel.Printf("%0.*f", precision, value);

		// TODO:  Update for OGL4
		// TODO:  Don't draw it if it's too close to the maximum (based on text size)
		/*glPushMatrix();
			glLoadIdentity();
			ComputeTranslations(value, xTranslation, yTranslation, font->BBox(valueLabel.mb_str()), valueOffsetFromEdge);
			glTranslated(xTranslation, yTranslation, 0.0);
			font->Render(valueLabel.mb_str());
		glPopMatrix();*/
	}

	valueLabel.Printf("%0.*f", precision, maximum);
	if (!valueLabel.ToDouble(&maximum)) { /*Warn the user?*/ }
}

//==========================================================================
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
//==========================================================================
unsigned int Axis::GetPrecision() const
{
	// It does look nicer to use the raw return value of GetPrecision(), but it affects the function
	// of dragging the plot around.  Because we always force the limits to actually
	// match the printed values, it makes the dragging operation very coarse.  So we add two.
	return PlotMath::GetPrecision(minimum, majorResolution, logarithmic) + 2;
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
//		value			= const double&
//		boundingBox		= const FTBBox&
//		offset			= const double&
//
// Output Arguments:
//		xTranslation	= int&
//		yTranslation	= int&
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
bool Axis::IsHorizontal() const
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
//		double specifying the location of the object in screen coordinates
//
//==========================================================================
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

//==========================================================================
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
//==========================================================================
double Axis::GetNextGridValue(const unsigned int &tick) const
{
	if (logarithmic)
		return pow(10.0, floor(log10(minimum)) + floor(tick / 9.0))
			* (tick - 9.0 * floor(tick / 9.0) + 1.0);

	return minimum + (double)tick * minorResolution;
}

//==========================================================================
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
//==========================================================================
unsigned int Axis::GetAxisLength() const
{
	if (orientation == OrientationTop || orientation == OrientationBottom)
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