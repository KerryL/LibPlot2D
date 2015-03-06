/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotObject.cpp
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Intermediate class for creating plots from arrays of data.
// History:

// FTGL headers
#include <FTGL/ftgl.h>

// Local headers
#include "application/plotObject.h"
#include "application/mainFrame.h"
#include "renderer/plotRenderer.h"
#include "renderer/color.h"
#include "renderer/primitives/plotCurve.h"
#include "renderer/primitives/text.h"
#include "utilities/math/plotMath.h"
#include "utilities/dataset2D.h"
#include "utilities/fontFinder.h"

//==========================================================================
// Class:			PlotObject
// Function:		PlotObject
//
// Description:		Constructor for PlotObject class.
//
// Input Arguments:
//		_renderer	= PlotRenderer& reference to the object that handles the
//					  drawing operations
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotObject::PlotObject(PlotRenderer &_renderer) : renderer(_renderer)
{
	CreateAxisObjects();
	InitializeFonts();

	ResetAutoScaling();
	renderer.SetBackgroundColor(Color::ColorWhite);
}

//==========================================================================
// Class:			PlotObject
// Function:		~PlotObject
//
// Description:		Destructor for PlotObject class.
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
PlotObject::~PlotObject()
{
	delete axisFont;
	axisFont = NULL;
	delete titleFont;
	titleFont = NULL;
}

//==========================================================================
// Class:			PlotObject
// Function:		CreateAxisObjects
//
// Description:		Initializes the axis objects.
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
void PlotObject::CreateAxisObjects(void)
{
	axisTop = new Axis(renderer);
	axisBottom = new Axis(renderer);
	axisLeft = new Axis(renderer);
	axisRight = new Axis(renderer);
	titleObject = new TextRendering(renderer);

	// Tell each axis how they relate to other axes
	axisTop->SetAxisAtMaxEnd(axisRight);
	axisTop->SetAxisAtMinEnd(axisLeft);
	axisTop->SetOppositeAxis(axisBottom);

	axisBottom->SetAxisAtMaxEnd(axisRight);
	axisBottom->SetAxisAtMinEnd(axisLeft);
	axisBottom->SetOppositeAxis(axisTop);

	axisLeft->SetAxisAtMaxEnd(axisTop);
	axisLeft->SetAxisAtMinEnd(axisBottom);
	axisLeft->SetOppositeAxis(axisRight);

	axisRight->SetAxisAtMaxEnd(axisTop);
	axisRight->SetAxisAtMinEnd(axisBottom);
	axisRight->SetOppositeAxis(axisLeft);
}

//==========================================================================
// Class:			PlotObject
// Function:		InitializeFonts
//
// Description:		Sets up plot fonts.
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
void PlotObject::InitializeFonts(void)
{
	// Find the name of the font that we want to use
	wxString fontFile;
	wxArrayString preferredFonts;

	preferredFonts.Add(_T("DejaVu Sans"));// GTK preference
	preferredFonts.Add(_T("Arial"));// MSW preference

	bool foundFont = FontFinder::GetPreferredFontFileName(wxFONTENCODING_SYSTEM,
		preferredFonts, false, fontFile);

	if (!foundFont)
	{
		if (!fontFile.IsEmpty())
		{
			wxString fontName;
			if (FontFinder::GetFontName(fontFile, fontName))
				wxMessageBox(_T("Could not find preferred plot font; using ") + fontName + _T(" from ") + fontFile);
			else
				wxMessageBox(_T("Could not find preferred plot font; using ") + fontFile);

		}
		else
			wxMessageBox(_T("Could not find any *.ttf files - cannot generate plot fonts"));
	}

	CreateFontObjects(fontFile);
}

//==========================================================================
// Class:			PlotObject
// Function:		CreateFontObjects
//
// Description:		Creates the font objects.
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
void PlotObject::CreateFontObjects(const wxString &fontFile)
{
	axisFont = new FTGLTextureFont(fontFile.c_str());
	titleFont = new FTGLTextureFont(fontFile.c_str());

	if (axisFont->Error())
	{
		delete axisFont;
		axisFont = NULL;

		wxMessageBox(_T("Error loading axis font"));
	}
	else
	{
		axisFont->FaceSize(12);
		axisFont->CharMap(FT_ENCODING_UNICODE);
	}

	if (titleFont->Error())
	{
		delete titleFont;
		titleFont = NULL;

		wxMessageBox(_T("Error loading title font"));
	}
	else
	{
		titleFont->FaceSize(18);
		titleFont->CharMap(FT_ENCODING_UNICODE);
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		Update
//
// Description:		Updates the data in the plot and re-sets the fonts, sizes
//					and positions.
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
void PlotObject::Update(void)
{
	FormatPlot();

	renderer.UpdateCursors();
	dynamic_cast<MainFrame*>(renderer.GetMainFrame())->UpdateCursorValues(
		renderer.GetLeftCursorVisible(), renderer.GetRightCursorVisible(),
		renderer.GetLeftCursorValue(), renderer.GetRightCursorValue());
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXMajorGrid
//
// Description:		Sets the status of the x-axis major grid.
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
void PlotObject::SetXMajorGrid(const bool &gridOn)
{
	axisBottom->SetMajorGrid(gridOn);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXMinorGrid
//
// Description:		Sets the status of the x-axis minor grid.
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
void PlotObject::SetXMinorGrid(const bool &gridOn)
{
	axisBottom->SetMinorGrid(gridOn);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYMajorGrid
//
// Description:		Sets the status of the left y-axis major grid.
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
void PlotObject::SetLeftYMajorGrid(const bool &gridOn)
{
	axisLeft->SetMajorGrid(gridOn);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYMinorGrid
//
// Description:		Sets the status of the left y-axis minor grid.
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
void PlotObject::SetLeftYMinorGrid(const bool &gridOn)
{
	axisLeft->SetMinorGrid(gridOn);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYMajorGrid
//
// Description:		Sets the status of the right y-axis major grid.
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
void PlotObject::SetRightYMajorGrid(const bool &gridOn)
{
	axisRight->SetMajorGrid(gridOn);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYMinorGrid
//
// Description:		Sets the status of the right y-axis minor grid.
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
void PlotObject::SetRightYMinorGrid(const bool &gridOn)
{
	axisRight->SetMinorGrid(gridOn);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXMajorResolution
//
// Description:		Sets the major resolution for the bottom axis.
//
// Input Arguments:
//		resolution	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetXMajorResolution(const double &resolution)
{
	xMajorResolution = resolution;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYMajorResolution
//
// Description:		Sets the major resolution for the left axis.
//
// Input Arguments:
//		resolution	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetLeftYMajorResolution(const double &resolution)
{
	yLeftMajorResolution = resolution;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYMajorResolution
//
// Description:		Sets the major resolution for the right axis.
//
// Input Arguments:
//		resolution	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetRightYMajorResolution(const double &resolution)
{
	yRightMajorResolution = resolution;
}

//==========================================================================
// Class:			PlotObject
// Function:		RemoveExistingPlots
//
// Description:		Deletes the existing plots.
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
void PlotObject::RemoveExistingPlots(void)
{
	while (plotList.size() > 0)
		RemovePlot(0);
}

//==========================================================================
// Class:			PlotObject
// Function:		RemovePlot
//
// Description:		Deletes the specified plot.
//
// Input Arguments:
//		index	= const unsigned int& specifying the curve to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::RemovePlot(const unsigned int &index)
{
	renderer.RemoveActor(plotList[index]);

	plotList.erase(plotList.begin() + index);
	dataList.erase(dataList.begin() + index);
}

//==========================================================================
// Class:			PlotObject
// Function:		AddCurve
//
// Description:		Adds a curve to the plot.
//
// Input Arguments:
//		data	= const Dataset2D& to plot
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::AddCurve(const Dataset2D &data)
{
	PlotCurve *newPlot = new PlotCurve(renderer);
	plotList.push_back(newPlot);
	dataList.push_back(&data);

	newPlot->BindToXAxis(axisBottom);
	newPlot->BindToYAxis(axisLeft);
	newPlot->SetData(&data);
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatPlot
//
// Description:		Formats the plot actor.
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
void PlotObject::FormatPlot(void)
{
	FormatAxesBasics();
	FormatTitle();
	if (dataList.size() == 0)
		return;

	SetOriginalAxisLimits();
	CheckForZeroRange();
	CheckAutoScaling();
	MatchYAxes();
	FormatCurves();

	const unsigned int maxXTicks(7);
	const unsigned int maxYTicks(10);

	double xMajor(xMajorResolution);
	double yLeftMajor(yLeftMajorResolution);
	double yRightMajor(yRightMajorResolution);

	// Set up the axes resolution (and at the same time tweak the max and min)
	AutoScaleAxis(xMin, xMax, xMajor, maxXTicks, axisBottom->IsLogarithmic(), !autoScaleX);
	AutoScaleAxis(yLeftMin, yLeftMax, yLeftMajor, maxYTicks, axisLeft->IsLogarithmic(), !autoScaleLeftY);
	AutoScaleAxis(yRightMin, yRightMax, yRightMajor, maxYTicks, axisRight->IsLogarithmic(), !autoScaleRightY);

	double xMinor = ComputeMinorResolution(xMin, xMax, xMajor, axisBottom->GetAxisLength());
	double yLeftMinor = ComputeMinorResolution(yLeftMin, yLeftMax, yLeftMajor, axisLeft->GetAxisLength());
	double yRightMinor = ComputeMinorResolution(yRightMin, yRightMax, yRightMajor, axisRight->GetAxisLength());

	ValidateRangeLimits(xMin, xMax, autoScaleX, xMajor, xMinor);
	ValidateRangeLimits(yLeftMin, yLeftMax, autoScaleLeftY, yLeftMajor, yLeftMinor);
	ValidateRangeLimits(yRightMin, yRightMax, autoScaleRightY, yRightMajor, yRightMinor);

	ValidateLogarithmicLimits(*axisBottom, xMin);
	ValidateLogarithmicLimits(*axisLeft, yLeftMin);
	ValidateLogarithmicLimits(*axisRight, yRightMin);

	ResetOriginalLimits();
	ApplyRangeLimits(xMinor, xMajor, yLeftMinor, yLeftMajor, yRightMinor, yRightMajor);
	UpdateLimitValues();
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatAxesBasics
//
// Description:		Formats each axis with orientation, color, etc.
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
void PlotObject::FormatAxesBasics(void)
{
	Axis::TickStyle tickStyle = Axis::TickStyleInside;

	FormatBottomBasics(tickStyle);
	FormatTopBasics(tickStyle);
	FormatLeftBasics(tickStyle);
	FormatRightBasics(tickStyle);

	SetAxesColor(Color::ColorBlack);
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatBottomBasics
//
// Description:		Formats the orientation, tick style and font for the bottom axis.
//
// Input Arguments:
//		tickStyle	= const Axis::TickStyle&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::FormatBottomBasics(const Axis::TickStyle &tickStyle)
{
	axisBottom->SetOrientation(Axis::OrientationBottom);
	axisBottom->SetFont(axisFont);
	axisBottom->SetTickStyle(tickStyle);

	if (axisBottom->GetLabel().IsEmpty())
		axisBottom->SetOffsetFromWindowEdge(50);
	else
		axisBottom->SetOffsetFromWindowEdge(75);
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatTopBasics
//
// Description:		Formats the orientation, tick style and font for the top axis.
//
// Input Arguments:
//		tickStyle	= const Axis::TickStyle&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::FormatTopBasics(const Axis::TickStyle &tickStyle)
{
	axisTop->SetOrientation(Axis::OrientationTop);
	axisTop->SetTickStyle(tickStyle);

	if (axisTop->GetLabel().IsEmpty())
		axisTop->SetOffsetFromWindowEdge(50);
	else
		axisTop->SetOffsetFromWindowEdge(75);

	if (!titleObject->GetText().IsEmpty())
		axisTop->SetOffsetFromWindowEdge(axisTop->GetOffsetFromWindowEdge() + titleObject->GetTextHeight());
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatLeftBasics
//
// Description:		Formats the orientation, tick style and font for the left axis.
//
// Input Arguments:
//		tickStyle	= const Axis::TickStyle&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::FormatLeftBasics(const Axis::TickStyle &tickStyle)
{
	axisLeft->SetOrientation(Axis::OrientationLeft);
	axisLeft->SetFont(axisFont);
	axisLeft->SetTickStyle(tickStyle);

	if (axisLeft->GetLabel().IsEmpty())
		axisLeft->SetOffsetFromWindowEdge(75);
	else
		axisLeft->SetOffsetFromWindowEdge(100);
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatRightBasics
//
// Description:		Formats the orientation, tick style and font for the right axis.
//
// Input Arguments:
//		tickStyle	= const Axis::TickStyle&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::FormatRightBasics(const Axis::TickStyle &tickStyle)
{
	axisRight->SetOrientation(Axis::OrientationRight);
	axisRight->SetFont(axisFont);
	axisRight->SetTickStyle(tickStyle);

	if (axisRight->GetLabel().IsEmpty())
		axisRight->SetOffsetFromWindowEdge(75);
	else
		axisRight->SetOffsetFromWindowEdge(100);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetAxesColor
//
// Description:		Sets all of the axes to the specified color.
//
// Input Arguments:
//		color	= const Color&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetAxesColor(const Color &color)
{
	axisBottom->SetColor(color);
	axisTop->SetColor(color);
	axisLeft->SetColor(color);
	axisRight->SetColor(color);
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatTitle
//
// Description:		Formats the title object.
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
void PlotObject::FormatTitle(void)
{
	titleObject->SetFont(titleFont);
	titleObject->SetCentered(true);
	titleObject->SetPosition(renderer.GetSize().GetWidth() / 2.0,
		renderer.GetSize().GetHeight() - axisTop->GetOffsetFromWindowEdge() / 2.0);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetOriginalAxisLimits
//
// Description:		Finds the range of each axis and sets the "original" values
//					(limits for if the axese were autoscaled).
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
void PlotObject::SetOriginalAxisLimits(void)
{
	leftUsed = false;
	rightUsed = false;
	unsigned int i;
	Axis *yAxis;
	for (i = 0; i < (unsigned int)dataList.size(); i++)
	{
		if (!plotList[i]->GetIsVisible())
			continue;
		if (!leftUsed && !rightUsed)
		{
			xMinOriginal = GetFirstValidValue(dataList[i]->GetXPointer(),
				dataList[i]->GetNumberOfPoints());
			xMaxOriginal = xMinOriginal;
		}

		yAxis = plotList[i]->GetYAxis();
		if (yAxis == axisLeft && !leftUsed)
		{
			leftUsed = true;
			yLeftMinOriginal = GetFirstValidValue(dataList[i]->GetYPointer(),
				dataList[i]->GetNumberOfPoints());
			yLeftMaxOriginal = yLeftMinOriginal;
		}
		else if (yAxis == axisRight && !rightUsed)
		{
			rightUsed = true;
			yRightMinOriginal = GetFirstValidValue(dataList[i]->GetYPointer(),
				dataList[i]->GetNumberOfPoints());
			yRightMaxOriginal = yRightMinOriginal;
		}
		GetAxisExtremes(*dataList[i], yAxis);
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		GetFirstValidValue
//
// Description:		Retrieves the first valid value from the specified array.
//
// Input Arguments:
//		data	= const double*
//		size	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotObject::GetFirstValidValue(const double* data, const unsigned int &size) const
{
	assert(data);

	unsigned int i;
	for (i = 0; i < size; i++)
	{
		if (PlotMath::IsValid(data[i]))
			return data[i];
	}

	return data[0];
}

//==========================================================================
// Class:			PlotObject
// Function:		MatchYAxes
//
// Description:		If one of the y-axes does not have any associated curves,
//					forces the limits to match the opposite y-axis.
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
void PlotObject::MatchYAxes(void)
{
	// If one axis is unused, make it match the other
	if (leftUsed && !rightUsed)
	{
		axisRight->SetLogarithmicScale(axisLeft->IsLogarithmic());
		yRightMinOriginal = yLeftMinOriginal;
		yRightMaxOriginal = yLeftMaxOriginal;
		yRightMin = yLeftMin;
		yRightMax = yLeftMax;
		yRightMajorResolution = yLeftMajorResolution;
	}
	else if (!leftUsed && rightUsed)
	{
		axisLeft->SetLogarithmicScale(axisRight->IsLogarithmic());
		yLeftMinOriginal = yRightMinOriginal;
		yLeftMaxOriginal = yRightMaxOriginal;
		yLeftMin = yRightMin;
		yLeftMax = yRightMax;
		yLeftMajorResolution = yRightMajorResolution;
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		GetAxisExtremes
//
// Description:		Parese the specified dataset and sets the associated mins
//					and maxes (original) equal to the extremum for the dataset.
//
// Input Arguments:
//		data	= const Dataset2D&
//		yAxis	= Axis* indicating the associated y-axis for the data
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::GetAxisExtremes(const Dataset2D &data, Axis *yAxis)
{
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); i++)
	{
		if (PlotMath::IsValid<double>(data.GetXData(i)))
		{
			if (data.GetXData(i) > xMaxOriginal)
				xMaxOriginal = data.GetXData(i);
			else if (data.GetXData(i) < xMinOriginal)
				xMinOriginal = data.GetXData(i);
		}

		if (yAxis == axisLeft && PlotMath::IsValid<double>(data.GetYData(i)))
		{
			if (data.GetYData(i) > yLeftMaxOriginal)
				yLeftMaxOriginal = data.GetYData(i);
			else if (data.GetYData(i) < yLeftMinOriginal)
				yLeftMinOriginal = data.GetYData(i);
		}
		else if (yAxis == axisRight && PlotMath::IsValid<double>(data.GetYData(i)))
		{
			if (data.GetYData(i) > yRightMaxOriginal)
				yRightMaxOriginal = data.GetYData(i);
			else if (data.GetYData(i) < yRightMinOriginal)
				yRightMinOriginal = data.GetYData(i);
		}
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		ApplyRangeLimits
//
// Description:		Applies the limits and resolutions for each axis.
//
// Input Arguments:
//		xMinor		= const double&
//		xMajor		= const double&
//		yLeftMinor	= const double&
//		yLeftMajor	= const double&
//		yRightMinor	= const double&
//		yRightMajor	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::ApplyRangeLimits(const double &xMinor, const double &xMajor,
	const double &yLeftMinor, const double &yLeftMajor,
	const double &yRightMinor, const double &yRightMajor)
{
	axisBottom->SetMinimum(xMin);
	axisBottom->SetMaximum(xMax);
	axisBottom->SetMinorResolution(xMinor);
	axisBottom->SetMajorResolution(xMajor);

	axisTop->SetLogarithmicScale(axisBottom->IsLogarithmic());// Make it match the bottom
	axisTop->SetMinimum(xMin);
	axisTop->SetMaximum(xMax);
	axisTop->SetMinorResolution(xMinor);
	axisTop->SetMajorResolution(xMajor);

	axisLeft->SetMinimum(yLeftMin);
	axisLeft->SetMaximum(yLeftMax);
	axisLeft->SetMinorResolution(yLeftMinor);
	axisLeft->SetMajorResolution(yLeftMajor);

	axisRight->SetMinimum(yRightMin);
	axisRight->SetMaximum(yRightMax);
	axisRight->SetMinorResolution(yRightMinor);
	axisRight->SetMajorResolution(yRightMajor);
}

//==========================================================================
// Class:			PlotObject
// Function:		ValidateRangeLimits
//
// Description:		Ensures the limit values are valid numbers.  If they are
//					not valid, axis limits are set to default values of +/- 1.
//
// Input Arguments:
//		min			= double&
//		max			= double&
//		autoScale	= const bool&
//		major		= double&
//		minor		= double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::ValidateRangeLimits(double &min, double &max,
	const bool &autoScale, double &major, double &minor) const
{
	if (PlotMath::IsNaN(min) || PlotMath::IsNaN(max))
	{
		min = -1.0;
		max = 1.0;
		AutoScaleAxis(min, max, major, 7, false, !autoScale);// TODO:  Magic number here!
		minor = major;
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		ApplyRangeLimits
//
// Description:		Resets the axes' limits to their original values, if the
//					axis is set to be auto-scaled.
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
void PlotObject::ResetOriginalLimits(void)
{
	if (autoScaleX)
	{
		xMinOriginal = xMin;
		xMaxOriginal = xMax;
	}

	if (autoScaleLeftY)
	{
		yLeftMinOriginal = yLeftMin;
		yLeftMaxOriginal = yLeftMax;
	}

	if (autoScaleRightY)
	{
		yRightMinOriginal = yRightMin;
		yRightMaxOriginal = yRightMax;
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		ValidateLogarithmicLimits
//
// Description:		Checks that the minimum value for the axis is strictly
//					positive, if the scaling is logarithmic.
//
// Input Arguments:
//		axis	= Axis&
//		min		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::ValidateLogarithmicLimits(Axis &axis, const double &min)
{
	if (axis.IsLogarithmic() && min <= 0.0)
	{
		::wxMessageBox(_T("Logarithmic scaling may only be used with strictly positive data."), _T("Logarithmic Scaling Error"));
		axis.SetLogarithmicScale(false);
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		HandleZeroRangeAxis
//
// Description:		Adjusts the specified min and max so they are not equal.
//
// Input Arguments:
//		min	= double& (also output)
//		max = double& (also output)
//
// Output Arguments:
//		min	= double& (also input)
//		max = double& (also input)
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::HandleZeroRangeAxis(double &min, double &max) const
{
	if (PlotMath::IsZero(min))
	{
		min = -1.0;
		max = 1.0;
	}
	else
	{
		min -= min * 0.1;
		max += max * 0.1;
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		CheckForZeroRange
//
// Description:		If the min and max for any axis is equal, it adjusts the
//					scaling so they are not equal.
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
void PlotObject::CheckForZeroRange(void)
{
	if (PlotMath::IsZero(xMaxOriginal - xMinOriginal))
		HandleZeroRangeAxis(xMinOriginal, xMaxOriginal);

	if (PlotMath::IsZero(yLeftMaxOriginal - yLeftMinOriginal))
		HandleZeroRangeAxis(yLeftMinOriginal, yLeftMaxOriginal);

	if (PlotMath::IsZero(yRightMaxOriginal - yRightMinOriginal))
		HandleZeroRangeAxis(yRightMinOriginal, yRightMaxOriginal);
}

//==========================================================================
// Class:			PlotObject
// Function:		CheckAutoScaling
//
// Description:		Checks if each axis is auto-scaled, and if so, adjusts
//					the axis limits to the original values.
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
void PlotObject::CheckAutoScaling()
{
	if (autoScaleX)
	{
		xMin = xMinOriginal;
		xMax = xMaxOriginal;
		xMajorResolution = 0.0;
	}

	if (autoScaleLeftY)
	{
		yLeftMin = yLeftMinOriginal;
		yLeftMax = yLeftMaxOriginal;
		yLeftMajorResolution = 0.0;
	}

	if (autoScaleRightY)
	{
		yRightMin = yRightMinOriginal;
		yRightMax = yRightMaxOriginal;
		yRightMajorResolution = 0.0;
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		UpdateLimitValues
//
// Description:		Updates the axes' limits so they are exactly the same as
//					what is displayed on screen (after rounding, etc.).
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
void PlotObject::UpdateLimitValues(void)
{
	axisBottom->Draw();
	xMin = axisBottom->GetMinimum();
	xMax = axisBottom->GetMaximum();

	axisLeft->Draw();
	yLeftMin = axisLeft->GetMinimum();
	yLeftMax = axisLeft->GetMaximum();

	axisRight->Draw();
	yRightMin = axisRight->GetMinimum();
	yRightMax = axisRight->GetMaximum();
}

//==========================================================================
// Class:			PlotObject
// Function:		AutoScaleAxis
//
// Description:		Calculates the number of tick marks to use to span the range
//					and make the plot look nice.  Also responsible for ensuring
//					accuracy of tick, grid line and label locations.
//
// Input Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		majorRes	= double&
//		maxTicks	= const int& specifying the maximum number of ticks to use
//		logarithmic	= const bool& specifying whether or not to use a logarithmic scale
//		forceLimits	= const bool& specifying whether or not to preserve the specified limits
//
// Output Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		majorRes	= double&
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::AutoScaleAxis(double &min, double &max, double &majorRes,
	const int &maxTicks, const bool &logarithmic, const bool &forceLimits) const
{
	if (logarithmic)
		AutoScaleLogAxis(min, max, majorRes, forceLimits);
	else
		AutoScaleLinearAxis(min, max, majorRes, maxTicks, forceLimits);
}

//==========================================================================
// Class:			PlotObject
// Function:		AutoScaleLogAxis
//
// Description:		Determines best axis limits for log axes.
//
// Input Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		majorRes	= double&
//		forceLimits	= const bool& specifying whether or not to preserve the specified limits
//
// Output Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		majorRes	= double&
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::AutoScaleLogAxis(double &min, double &max, double &majorRes,
	const bool &forceLimits) const
{
	if (!forceLimits)
	{
		// Determine the nearest power of 10 for each limit
		min = pow(10.0, floor(log10(min)));
		max = pow(10.0, ceil(log10(max)));
	}
	else
	{
		if (min <= 0.0)
			min = pow(10.0, floor(log10(min)));
		if (max <= 0.0)
			max = pow(10.0, ceil(log10(max)));
	}

	majorRes = 10.0;
}

//==========================================================================
// Class:			PlotObject
// Function:		AutoScaleLinearAxis
//
// Description:		Determines best axis limits for linear axes.
//
// Input Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		majorRes	= double&
//		maxTicks	= const int& specifying the maximum number of ticks to use
//		forceLimits	= const bool& specifying whether or not to preserve the specified limits
//
// Output Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		majorRes	= double&
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::AutoScaleLinearAxis(double &min, double &max, double &majorRes,
	const int &maxTicks, const bool &forceLimits) const
{
	double range = max - min;
	int orderOfMagnitude = (int)log10(range);
	double tickSpacing = range / maxTicks;

	// Acceptable resolution steps are:
	//	Ones,
	//	Twos (even numbers), and
	//	Fives (multiples of five),
	// each within the order of magnitude (i.e. [37, 38, 39], [8.5, 9.0, 9.5], and [20, 40, 60] are all acceptable)

	// Scale the tick spacing so it is between 0.1 and 10.0
	double scaledSpacing = tickSpacing / pow(10.0, orderOfMagnitude - 1);

	if (!forceLimits)
	{
		if (scaledSpacing > 5.0)
			scaledSpacing = 10.0;
		else if (scaledSpacing > 2.0)
			scaledSpacing = 5.0;
		else if (scaledSpacing > 1.0)
			scaledSpacing = 2.0;
		else if (scaledSpacing > 0.5)
			scaledSpacing = 1.0;
		else if (scaledSpacing > 0.2)
			scaledSpacing = 0.5;
		else if (scaledSpacing > 0.1)
			scaledSpacing = 0.2;
		else
			scaledSpacing = 0.1;
	}

	tickSpacing = scaledSpacing * pow(10.0, orderOfMagnitude - 1);
	if (majorRes == 0.0)
		majorRes = tickSpacing;
	RoundMinMax(min, max, majorRes, forceLimits);
}

//==========================================================================
// Class:			PlotObject
// Function:		RoundMinMax
//
// Description:		Rounds the min and max to the appropriate precision.
//
// Input Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//		tickSpacing	= const double& specifying spacing between tick marks
//		forceLimits	= const bool& specifying whether or not to preserve the specified limits
//
// Output Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::RoundMinMax(double &min, double &max, const double &tickSpacing, const bool &forceLimits) const
{
	if (!forceLimits)
	{
		if (fmod(min, tickSpacing) != 0)
		{
			if (min < 0)
			{
				min -= fmod(min, tickSpacing);
				min -= tickSpacing;
			}
			else
				min -= fmod(min, tickSpacing);
		}
		if (fmod(max, tickSpacing) != 0)
		{
			if (max > 0)
			{
				max -= fmod(max, tickSpacing);
				max += tickSpacing;
			}
			else
				max -= fmod(max, tickSpacing);
		}
	}

	if (PlotMath::IsZero(min))
		min = 0.0;
	if (PlotMath::IsZero(max))
		max = 0.0;
}

//==========================================================================
// Class:			PlotObject
// Function:		ComputeMinorResolution
//
// Description:		Returns the optimal minor resolution for the axis.
//
// Input Arguments:
//		min				= const double&
//		max				= const double&
//		majorResolution	= const double&
//		axisLength		= const double& on screen length (pixels)
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotObject::ComputeMinorResolution(const double &min, const double &max,
	const double &majorResolution, const double &axisLength) const
{
	const double idealDensity(1.0 / 30.0);// gridlines per pixel
	unsigned int lines = idealDensity * axisLength;
	double minorResolution = (max - min) / lines;
	double majorMinorRatio = floor(majorResolution / minorResolution + 0.5);
	if (majorMinorRatio == 0.0)
		return majorResolution;
	return majorResolution / majorMinorRatio;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXMin
//
// Description:		Sets the lower X limit.
//
// Input Arguments:
//		_xMin	= const double& describing desired minimum X limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetXMin(const double &_xMin)
{
	// If the both limits are at the original value, enable auto-scaling again
	if (xMax == xMaxOriginal && _xMin == xMinOriginal && xMajorResolution == 0.0)
		autoScaleX = true;
	else
		autoScaleX = false;
	xMin = _xMin;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXMax
//
// Description:		Sets the upper X limit.
//
// Input Arguments:
//		_xMax	= const double& describing desired maximum X limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetXMax(const double &_xMax)
{
	// If both limits are at the original value, enable auto-scaling again
	if (xMin == xMinOriginal && _xMax == xMaxOriginal && xMajorResolution == 0.0)
		autoScaleX = true;
	else
		autoScaleX = false;
	xMax = _xMax;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYMin
//
// Description:		Sets the lower Y limit.
//
// Input Arguments:
//		_yMin	= const double& describing desired minimum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetLeftYMin(const double &_yMin)
{
	// If both limits are at the original value, enable auto-scaling again
	if (yLeftMax == yLeftMaxOriginal && _yMin == yLeftMinOriginal && yLeftMajorResolution == 0.0)
		autoScaleLeftY = true;
	else
		autoScaleLeftY = false;
	yLeftMin = _yMin;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYMax
//
// Description:		Sets the upper Y limit.
//
// Input Arguments:
//		_yMax	= const double& describing desired maximum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetLeftYMax(const double &_yMax)
{
	// If both limits are at the original value, enable auto-scaling again
	if (yLeftMin == yLeftMinOriginal && _yMax == yLeftMaxOriginal && yLeftMajorResolution == 0.0)
		autoScaleLeftY = true;
	else
		autoScaleLeftY = false;
	yLeftMax = _yMax;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYMin
//
// Description:		Sets the lower Y limit.
//
// Input Arguments:
//		_yMin	= const double& describing desired minimum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetRightYMin(const double &_yMin)
{
	// If the both limits are at the original value, enable auto-scaling again
	if (yRightMax == yRightMaxOriginal && _yMin == yRightMinOriginal && yRightMajorResolution == 0.0)
		autoScaleRightY = true;
	else
		autoScaleRightY = false;
	yRightMin = _yMin;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYMax
//
// Description:		Sets the upper Y limit.
//
// Input Arguments:
//		_yMax	= const double& describing desired maximum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetRightYMax(const double &_yMax)
{
	// If the both limits are at the original value, enable auto-scaling again
	if (yRightMin == yRightMinOriginal && _yMax == yRightMaxOriginal && yRightMajorResolution == 0.0)
		autoScaleRightY = true;
	else
		autoScaleRightY = false;
	yRightMax = _yMax;
}

//==========================================================================
// Class:			PlotObject
// Function:		ResetAutoScaling
//
// Description:		Resets auto-scaling for all axes.
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
void PlotObject::ResetAutoScaling(void)
{
	autoScaleX = true;
	autoScaleLeftY = true;
	autoScaleRightY = true;

	xMajorResolution = 0.0;
	yLeftMajorResolution = 0.0;
	yRightMajorResolution = 0.0;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetCurveProperties
//
// Description:		Sets properties for the specified curve.
//
// Input Arguments:
//		index		= const unsigned int&
//		color		= const Color&
//		visible		= const bool&
//		rightAxis	= const bool&
//		lineSize	= const unsigned int&
//		markerSize	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetCurveProperties(const unsigned int &index, const Color &color,
	const bool &visible, const bool &rightAxis, const unsigned int &lineSize, const int &markerSize)
{
	plotList[index]->SetColor(color);
	plotList[index]->SetVisibility(visible);
	plotList[index]->SetLineSize(lineSize);
	plotList[index]->SetMarkerSize(markerSize);

	if (rightAxis)
		plotList[index]->BindToYAxis(axisRight);
	else
		plotList[index]->BindToYAxis(axisLeft);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetMajorGrid
//
// Description:		Resets major grid for all axes.
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
void PlotObject::SetMajorGrid(const bool &gridOn)
{
	axisBottom->SetMajorGrid(gridOn);
	axisLeft->SetMajorGrid(gridOn);

	// These axis default to off, but can be specifically turned on via a right-click
	axisTop->SetMajorGrid(false);
	axisRight->SetMajorGrid(false);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetMinorGrid
//
// Description:		Resets minor grid for all axes.
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
void PlotObject::SetMinorGrid(const bool &gridOn)
{
	axisBottom->SetMinorGrid(gridOn);
	axisLeft->SetMinorGrid(gridOn);

	// These axis default to off, but can be specifically turned on via a right-click
	axisTop->SetMinorGrid(false);
	axisRight->SetMinorGrid(false);
}

//==========================================================================
// Class:			PlotObject
// Function:		GetMajorGrid
//
// Description:		Returns status of major gridlines.
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
bool PlotObject::GetMajorGrid(void)
{
	if (axisBottom == NULL)
		return false;

	return axisBottom->GetMajorGrid();
}

//==========================================================================
// Class:			PlotObject
// Function:		GetMinorGrid
//
// Description:		Returns status of minor gridlines.
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
bool PlotObject::GetMinorGrid(void)
{
	if (axisBottom == NULL)
		return false;

	return axisBottom->GetMinorGrid();
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXLabel
//
// Description:		Sets the x-axis text.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetXLabel(wxString text)
{
	axisBottom->SetLabel(text);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYLabel
//
// Description:		Sets the left y-axis text.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetLeftYLabel(wxString text)
{
	axisLeft->SetLabel(text);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYLabel
//
// Description:		Sets the title text.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetRightYLabel(wxString text)
{
	axisRight->SetLabel(text);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetTitle
//
// Description:		Sets the title text.
//
// Input Arguments:
//		text	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetTitle(wxString text)
{
	titleObject->SetText(text);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetGridColor
//
// Description:		Sets gridline color.
//
// Input Arguments:
//		color	= const color&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetGridColor(const Color &color)
{
	axisBottom->SetGridColor(color);
	axisTop->SetGridColor(color);
	axisLeft->SetGridColor(color);
	axisRight->SetGridColor(color);
}

//==========================================================================
// Class:			PlotObject
// Function:		GetGridColor
//
// Description:		Resets auto-scaling for all axes.
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
Color PlotObject::GetGridColor(void) const
{
	return axisBottom->GetGridColor();
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXLogarithmic
//
// Description:		Sets the X axis to be scaled logarithmicly (or not).
//
// Input Arguments:
//		log	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetXLogarithmic(const bool &log)
{
	if (!axisBottom)
		return;

	axisBottom->SetLogarithmicScale(log);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftLogarithmic
//
// Description:		Sets the left Y axis to be scaled logarithmicly (or not).
//
// Input Arguments:
//		log	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetLeftLogarithmic(const bool &log)
{
	if (!axisLeft)
		return;

	axisLeft->SetLogarithmicScale(log);
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightLogarithmic
//
// Description:		Sets the right Y axis to be scaled logarithmicly (or not).
//
// Input Arguments:
//		log	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetRightLogarithmic(const bool &log)
{
	if (!axisRight)
		return;

	axisRight->SetLogarithmicScale(log);
}

//==========================================================================
// Class:			PlotObject
// Function:		FormatCurves
//
// Description:		Formats the curve objects.
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
void PlotObject::FormatCurves(void)
{
	unsigned int i;
	for (i = 0; i < (unsigned int)plotList.size(); i++)
	{
		plotList[i]->SetModified();
	}
}

//==========================================================================
// Class:			PlotObject
// Function:		GetXLabel
//
// Description:		Returns the x label text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString PlotObject::GetXLabel(void) const
{
	return axisBottom->GetLabel();
}

//==========================================================================
// Class:			PlotObject
// Function:		GetXLabel
//
// Description:		Returns the left y label text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString PlotObject::GetLeftYLabel(void) const
{
	return axisLeft->GetLabel();
}

//==========================================================================
// Class:			PlotObject
// Function:		GetXLabel
//
// Description:		Returns the right y label text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString PlotObject::GetRightYLabel(void) const
{
	return axisRight->GetLabel();
}

//==========================================================================
// Class:			PlotObject
// Function:		GetTitle
//
// Description:		Returns the title text.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString PlotObject::GetTitle(void) const
{
	return titleObject->GetText();
}