/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  plotObject.cpp
// Date:  5/4/2011
// Auth:  K. Loux
// Desc:  Intermediate class for creating plots from arrays of data.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/gui/plotObject.h"
#include "lp2d/gui/guiInterface.h"
#include "lp2d/renderer/plotRenderer.h"
#include "lp2d/renderer/color.h"
#include "lp2d/renderer/primitives/plotCurve.h"
#include "lp2d/renderer/primitives/textRendering.h"
#include "lp2d/renderer/primitives/legend.h"
#include "lp2d/utilities/math/plotMath.h"
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/utilities/fontFinder.h"

namespace LibPlot2D
{

//=============================================================================
// Class:			PlotObject
// Function:		Constant declarations
//
// Description:		Constant declarations for PlotObject class.
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
const unsigned int PlotObject::mHorizontalOffsetWithLabel(75);
const unsigned int PlotObject::mHorizontalOffsetWithoutLabel(50);
const unsigned int PlotObject::mVerticalOffsetWithLabel(100);
const unsigned int PlotObject::mVerticalOffsetWithoutLabel(75);

//=============================================================================
// Class:			PlotObject
// Function:		PlotObject
//
// Description:		Constructor for PlotObject class.
//
// Input Arguments:
//		renderer		= PlotRenderer& reference to the object that handles the
//						  drawing operations
//		guiInterface	= GuiInterface&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
PlotObject::PlotObject(PlotRenderer &renderer, GuiInterface& guiInterface)
	: mRenderer(renderer), mGuiInterface(guiInterface)
{
	CreateAxisObjects();
	InitializeFonts();
	ResetAutoScaling();
}

//=============================================================================
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
//=============================================================================
void PlotObject::CreateAxisObjects()
{
	mAxisTop = new Axis(mRenderer);
	mAxisBottom = new Axis(mRenderer);
	mAxisLeft = new Axis(mRenderer);
	mAxisRight = new Axis(mRenderer);
	mTitleObject = new TextRendering(mRenderer);

	// Tell each axis how they relate to other axes
	mAxisTop->SetAxisAtMaxEnd(mAxisRight);
	mAxisTop->SetAxisAtMinEnd(mAxisLeft);
	mAxisTop->SetOppositeAxis(mAxisBottom);

	mAxisBottom->SetAxisAtMaxEnd(mAxisRight);
	mAxisBottom->SetAxisAtMinEnd(mAxisLeft);
	mAxisBottom->SetOppositeAxis(mAxisTop);

	mAxisLeft->SetAxisAtMaxEnd(mAxisTop);
	mAxisLeft->SetAxisAtMinEnd(mAxisBottom);
	mAxisLeft->SetOppositeAxis(mAxisRight);

	mAxisRight->SetAxisAtMaxEnd(mAxisTop);
	mAxisRight->SetAxisAtMinEnd(mAxisBottom);
	mAxisRight->SetOppositeAxis(mAxisLeft);

	FormatAxesBasics();
}

//=============================================================================
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
//=============================================================================
void PlotObject::InitializeFonts()
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
		return;
	}

	mFontFileName = fontFile.ToStdString();
	mAxisBottom->InitializeFonts(mFontFileName, 12);
	//mAxisTop->InitializeFonts(mFontFileName, 12);// No tick labels for top axis
	mAxisLeft->InitializeFonts(mFontFileName, 12);
	mAxisRight->InitializeFonts(mFontFileName, 12);

	mTitleObject->InitializeFonts(mFontFileName, 18);
}

//=============================================================================
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
//=============================================================================
void PlotObject::Update()
{
	FormatPlot();
	ComputeTransformationMatrices();

	mRenderer.UpdateCursors();
	mGuiInterface.UpdateCursorValues(
		mRenderer.GetLeftCursorVisible(), mRenderer.GetRightCursorVisible(),
		mRenderer.GetLeftCursorValue(), mRenderer.GetRightCursorValue());
}

//=============================================================================
// Class:			PlotObject
// Function:		SetEqualScaling
//
// Description:		Forces the x- and y-axes to have equal scaling.
//
// Input Arguments:
//		equalScaling	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetEqualScaling(const bool& equalScaling)
{
	mEqualScaling = equalScaling;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetXMajorGrid(const bool &gridOn)
{
	mAxisBottom->SetMajorGrid(gridOn);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetXMinorGrid(const bool &gridOn)
{
	mAxisBottom->SetMinorGrid(gridOn);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetLeftYMajorGrid(const bool &gridOn)
{
	mAxisLeft->SetMajorGrid(gridOn);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetLeftYMinorGrid(const bool &gridOn)
{
	mAxisLeft->SetMinorGrid(gridOn);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetRightYMajorGrid(const bool &gridOn)
{
	mAxisRight->SetMajorGrid(gridOn);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetRightYMinorGrid(const bool &gridOn)
{
	mAxisRight->SetMinorGrid(gridOn);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetXMajorResolution(const double &resolution)
{
	mXMajorResolution = resolution;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetLeftYMajorResolution(const double &resolution)
{
	mYLeftMajorResolution = resolution;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetRightYMajorResolution(const double &resolution)
{
	mYRightMajorResolution = resolution;
}

//=============================================================================
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
//=============================================================================
void PlotObject::RemoveExistingPlots()
{
	while (mPlotList.size() > 0)
		RemovePlot(0);
}

//=============================================================================
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
//=============================================================================
void PlotObject::RemovePlot(const unsigned int &index)
{
	mRenderer.RemoveActor(mPlotList[index]);

	mPlotList.erase(mPlotList.begin() + index);
	mDataList.erase(mDataList.begin() + index);
}

//=============================================================================
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
//=============================================================================
void PlotObject::AddCurve(const Dataset2D &data)
{
	PlotCurve *newPlot = new PlotCurve(mRenderer, data);
	mPlotList.push_back(newPlot);
	mDataList.push_back(&data);

	newPlot->BindToXAxis(mAxisBottom);
	newPlot->BindToYAxis(mAxisLeft);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatPlot()
{
	UpdateAxesOffsets();
	FormatTitle();
	if (mDataList.size() == 0)
		return;

	SetOriginalAxisLimits();
	CheckForZeroRange();
	CheckAutoScaling();
	MatchYAxes();
	FormatCurves();

	bool forceLeftYLimits(!mAutoScaleLeftY), forceRightYLimits(!mAutoScaleRightY);
	if (mLeftUsed && !mRightUsed)
		forceRightYLimits = forceLeftYLimits;
	else if (mRightUsed && !mLeftUsed)
		forceLeftYLimits = forceRightYLimits;

	double xMajor(mXMajorResolution);
	double yLeftMajor(mYLeftMajorResolution);
	double yRightMajor(mYRightMajorResolution);

	// Set up the axes resolution (and at the same time tweak the max and min)
	AutoScaleAxis(mXMin, mXMax, xMajor, PlotRenderer::mMaxXTicks, mAxisBottom->IsLogarithmic(), !mAutoScaleX);
	if (mEqualScaling)
	{
		ForceEqualScaling(mAxisBottom, mAxisLeft, 0.5 * (mYLeftMinOriginal + mYLeftMaxOriginal), mYLeftMin, mYLeftMax);
		mYLeftMajorResolution = yLeftMajor = xMajor;
	}
	else
		AutoScaleAxis(mYLeftMin, mYLeftMax, yLeftMajor, PlotRenderer::mMaxYTicks, mAxisLeft->IsLogarithmic(), forceLeftYLimits);
	AutoScaleAxis(mYRightMin, mYRightMax, yRightMajor, PlotRenderer::mMaxYTicks, mAxisRight->IsLogarithmic(), forceRightYLimits);

	double xMinor = ComputeMinorResolution(mXMin, mXMax, xMajor, mAxisBottom->GetAxisLength());
	double yLeftMinor = ComputeMinorResolution(mYLeftMin, mYLeftMax, yLeftMajor, mAxisLeft->GetAxisLength());
	double yRightMinor = ComputeMinorResolution(mYRightMin, mYRightMax, yRightMajor, mAxisRight->GetAxisLength());

	ValidateRangeLimits(mXMin, mXMax, mAutoScaleX, xMajor, xMinor);
	ValidateRangeLimits(mYLeftMin, mYLeftMax, !forceLeftYLimits, yLeftMajor, yLeftMinor);
	ValidateRangeLimits(mYRightMin, mYRightMax, !forceRightYLimits, yRightMajor, yRightMinor);

	ValidateLogarithmicLimits(*mAxisBottom, mXMin);
	ValidateLogarithmicLimits(*mAxisLeft, mYLeftMin);
	ValidateLogarithmicLimits(*mAxisRight, mYRightMin);

	ResetOriginalLimits();
	ApplyRangeLimits(xMinor, xMajor, yLeftMinor, yLeftMajor, yRightMinor, yRightMajor);
	UpdateLimitValues();

	if (mNeedScissorUpdate)
	{
		UpdateScissorArea();
		mNeedScissorUpdate = false;
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatAxesBasics()
{
	Axis::TickStyle tickStyle = Axis::TickStyle::Inside;

	FormatBottomBasics(tickStyle);
	FormatTopBasics(Axis::TickStyle::NoTicks);
	FormatLeftBasics(tickStyle);
	FormatRightBasics(tickStyle);

	SetAxesColor(Color::ColorBlack);
}

//=============================================================================
// Class:			PlotObject
// Function:		GetHorizontalAxisOffset
//
// Description:		Returns the number of pixels that a horizontal axis is
//					to be offset from the edge of the plot area.
//
// Input Arguments:
//		withLabel	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int PlotObject::GetHorizontalAxisOffset(const bool &withLabel) const
{
	if (withLabel)
		return mHorizontalOffsetWithLabel;

	return mHorizontalOffsetWithoutLabel;
}

//=============================================================================
// Class:			PlotObject
// Function:		GetVerticalAxisOffset
//
// Description:		Returns the number of pixels that a vertical axis is
//					to be offset from the edge of the plot area.
//
// Input Arguments:
//		withLabel	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//=============================================================================
unsigned int PlotObject::GetVerticalAxisOffset(const bool &withLabel) const
{
	if (withLabel)
		return mVerticalOffsetWithLabel;

	return mVerticalOffsetWithoutLabel;
}

//=============================================================================
// Class:			PlotObject
// Function:		UpdateAxesOffsets
//
// Description:		Updates the offsets for each axis from the edge of the viewable area.
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
void PlotObject::UpdateAxesOffsets()
{
	if (mAxisBottom->GetLabel().IsEmpty())
		mAxisBottom->SetOffsetFromWindowEdge(mHorizontalOffsetWithoutLabel);
	else
		mAxisBottom->SetOffsetFromWindowEdge(mHorizontalOffsetWithLabel);

	if (mAxisTop->GetLabel().IsEmpty())
		mAxisTop->SetOffsetFromWindowEdge(mHorizontalOffsetWithoutLabel);
	else
		mAxisTop->SetOffsetFromWindowEdge(mHorizontalOffsetWithLabel);

	if (!mTitleObject->GetText().IsEmpty())
		mAxisTop->SetOffsetFromWindowEdge(mAxisTop->GetOffsetFromWindowEdge()
			+ mTitleObject->GetTextHeight());

	if (mAxisLeft->GetLabel().IsEmpty())
		mAxisLeft->SetOffsetFromWindowEdge(mVerticalOffsetWithoutLabel);
	else
		mAxisLeft->SetOffsetFromWindowEdge(mVerticalOffsetWithLabel);

	if (mAxisRight->GetLabel().IsEmpty())
		mAxisRight->SetOffsetFromWindowEdge(mVerticalOffsetWithoutLabel);
	else
		mAxisRight->SetOffsetFromWindowEdge(mVerticalOffsetWithLabel);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatBottomBasics(const Axis::TickStyle &tickStyle)
{
	mAxisBottom->SetOrientation(Axis::Orientation::Bottom);
	mAxisBottom->SetTickStyle(tickStyle);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatTopBasics(const Axis::TickStyle &tickStyle)
{
	mAxisTop->SetOrientation(Axis::Orientation::Top);
	mAxisTop->SetTickStyle(tickStyle);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatLeftBasics(const Axis::TickStyle &tickStyle)
{
	mAxisLeft->SetOrientation(Axis::Orientation::Left);
	mAxisLeft->SetTickStyle(tickStyle);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatRightBasics(const Axis::TickStyle &tickStyle)
{
	mAxisRight->SetOrientation(Axis::Orientation::Right);
	mAxisRight->SetTickStyle(tickStyle);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetAxesColor(const Color &color)
{
	mAxisBottom->SetColor(color);
	mAxisTop->SetColor(color);
	mAxisLeft->SetColor(color);
	mAxisRight->SetColor(color);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatTitle()
{
	mTitleObject->SetCentered(true);
	mTitleObject->SetPosition(mRenderer.GetSize().GetWidth() / 2.0,
		mRenderer.GetSize().GetHeight() - mAxisTop->GetOffsetFromWindowEdge() / 2.0);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetOriginalAxisLimits()
{
	mLeftUsed = false;
	mRightUsed = false;
	unsigned int i;
	Axis *yAxis;
	for (i = 0; i < static_cast<unsigned int>(mDataList.size()); ++i)
	{
		if (!mPlotList[i]->GetIsVisible())
			continue;
		if (!mLeftUsed && !mRightUsed)
		{
			mXMinOriginal = GetFirstValidValue(mDataList[i]->GetX());
			mXMaxOriginal = mXMinOriginal;
		}

		yAxis = mPlotList[i]->GetYAxis();
		if (yAxis == mAxisLeft && !mLeftUsed)
		{
			mLeftUsed = true;
			mYLeftMinOriginal = GetFirstValidValue(mDataList[i]->GetY());
			mYLeftMaxOriginal = mYLeftMinOriginal;
		}
		else if (yAxis == mAxisRight && !mRightUsed)
		{
			mRightUsed = true;
			mYRightMinOriginal = GetFirstValidValue(mDataList[i]->GetY());
			mYRightMaxOriginal = mYRightMinOriginal;
		}
		GetAxisExtremes(*mDataList[i], yAxis);
	}
}

//=============================================================================
// Class:			PlotObject
// Function:		GetFirstValidValue
//
// Description:		Retrieves the first valid value from the specified array.
//
// Input Arguments:
//		data	= const std::vector<double>&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//=============================================================================
double PlotObject::GetFirstValidValue(const std::vector<double>& data) const
{
	for (const auto& value : data)
	{
		if (PlotMath::IsValid(value))
			return value;
	}

	return 0.0;
}

//=============================================================================
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
//=============================================================================
void PlotObject::MatchYAxes()
{
	// If one axis is unused, make it match the other
	if (mLeftUsed && !mRightUsed)
	{
		mAxisRight->SetLogarithmicScale(mAxisLeft->IsLogarithmic());
		mYRightMinOriginal = mYLeftMinOriginal;
		mYRightMaxOriginal = mYLeftMaxOriginal;
		mYRightMin = mYLeftMin;
		mYRightMax = mYLeftMax;
		mYRightMajorResolution = mYLeftMajorResolution;
	}
	else if (!mLeftUsed && mRightUsed)
	{
		mAxisLeft->SetLogarithmicScale(mAxisRight->IsLogarithmic());
		mYLeftMinOriginal = mYRightMinOriginal;
		mYLeftMaxOriginal = mYRightMaxOriginal;
		mYLeftMin = mYRightMin;
		mYLeftMax = mYRightMax;
		mYLeftMajorResolution = mYRightMajorResolution;
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::GetAxisExtremes(const Dataset2D &data, Axis *yAxis)
{
	unsigned int i;
	for (i = 0; i < data.GetNumberOfPoints(); ++i)
	{
		if (PlotMath::IsValid<double>(data.GetX()[i]))
		{
			if (data.GetX()[i] > mXMaxOriginal)
				mXMaxOriginal = data.GetX()[i];
			else if (data.GetX()[i] < mXMinOriginal)
				mXMinOriginal = data.GetX()[i];
		}

		if (yAxis == mAxisLeft && PlotMath::IsValid<double>(data.GetY()[i]))
		{
			if (data.GetY()[i] > mYLeftMaxOriginal)
				mYLeftMaxOriginal = data.GetY()[i];
			else if (data.GetY()[i] < mYLeftMinOriginal)
				mYLeftMinOriginal = data.GetY()[i];
		}
		else if (yAxis == mAxisRight && PlotMath::IsValid<double>(data.GetY()[i]))
		{
			if (data.GetY()[i] > mYRightMaxOriginal)
				mYRightMaxOriginal = data.GetY()[i];
			else if (data.GetY()[i] < mYRightMinOriginal)
				mYRightMinOriginal = data.GetY()[i];
		}
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::ApplyRangeLimits(const double &xMinor, const double &xMajor,
	const double &yLeftMinor, const double &yLeftMajor,
	const double &yRightMinor, const double &yRightMajor)
{
	mAxisBottom->SetMinimum(mXMin);
	mAxisBottom->SetMaximum(mXMax);
	mAxisBottom->SetMinorResolution(xMinor);
	mAxisBottom->SetMajorResolution(xMajor);

	mAxisTop->SetLogarithmicScale(mAxisBottom->IsLogarithmic());// Make it match the bottom
	mAxisTop->SetMinimum(mXMin);
	mAxisTop->SetMaximum(mXMax);
	mAxisTop->SetMinorResolution(xMinor);
	mAxisTop->SetMajorResolution(xMajor);

	mAxisLeft->SetMinimum(mYLeftMin);
	mAxisLeft->SetMaximum(mYLeftMax);
	mAxisLeft->SetMinorResolution(yLeftMinor);
	mAxisLeft->SetMajorResolution(yLeftMajor);

	mAxisRight->SetMinimum(mYRightMin);
	mAxisRight->SetMaximum(mYRightMax);
	mAxisRight->SetMinorResolution(yRightMinor);
	mAxisRight->SetMajorResolution(yRightMajor);
}

//=============================================================================
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
//=============================================================================
void PlotObject::ValidateRangeLimits(double &min, double &max,
	const bool &autoScale, double &major, double &minor) const
{
	if (PlotMath::IsNaN(min) || PlotMath::IsNaN(max))
	{
		min = -1.0;
		max = 1.0;
		AutoScaleAxis(min, max, major, PlotRenderer::mMaxXTicks, false, !autoScale);
		minor = major;
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::ResetOriginalLimits()
{
	if (mAutoScaleX)
	{
		mXMinOriginal = mXMin;
		mXMaxOriginal = mXMax;
	}

	if (mAutoScaleLeftY)
	{
		mYLeftMinOriginal = mYLeftMin;
		mYLeftMaxOriginal = mYLeftMax;
	}

	if (mAutoScaleRightY)
	{
		mYRightMinOriginal = mYRightMin;
		mYRightMaxOriginal = mYRightMax;
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::ValidateLogarithmicLimits(Axis &axis, const double &min)
{
	if (axis.IsLogarithmic() && min <= 0.0)
	{
		::wxMessageBox(_T("Logarithmic scaling may only be used with strictly positive data."), _T("Logarithmic Scaling Error"));
		axis.SetLogarithmicScale(false);
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::HandleZeroRangeAxis(double &min, double &max) const
{
	if (PlotMath::IsZero(min))
	{
		min = -1.0;
		max = 1.0;
	}
	else
	{
		min -= fabs(min) * 0.1;
		max += fabs(max) * 0.1;
	}
}

//=============================================================================
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
//=============================================================================
void PlotObject::CheckForZeroRange()
{
	if (PlotMath::IsZero(mXMaxOriginal - mXMinOriginal))
		HandleZeroRangeAxis(mXMinOriginal, mXMaxOriginal);

	if (PlotMath::IsZero(mYLeftMaxOriginal - mYLeftMinOriginal))
		HandleZeroRangeAxis(mYLeftMinOriginal, mYLeftMaxOriginal);

	if (PlotMath::IsZero(mYRightMaxOriginal - mYRightMinOriginal))
		HandleZeroRangeAxis(mYRightMinOriginal, mYRightMaxOriginal);
}

//=============================================================================
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
//=============================================================================
void PlotObject::CheckAutoScaling()
{
	if (mAutoScaleX)
	{
		mXMin = mXMinOriginal;
		mXMax = mXMaxOriginal;
		mXMajorResolution = 0.0;
	}

	if (mAutoScaleLeftY)
	{
		mYLeftMin = mYLeftMinOriginal;
		mYLeftMax = mYLeftMaxOriginal;
		mYLeftMajorResolution = 0.0;
	}

	if (mAutoScaleRightY)
	{
		mYRightMin = mYRightMinOriginal;
		mYRightMax = mYRightMaxOriginal;
		mYRightMajorResolution = 0.0;
	}
}

double PlotObject::GetAxisUnitsPerPixel(const Axis* axis)
{
	return (axis->GetMaximum() - axis->GetMinimum()) / axis->GetAxisLength();
}

void PlotObject::ForceEqualScaling(const Axis* refAxis, const Axis* targetAxis,
	const double& centerRange, double& minLimit, double& maxLimit)
{
	const auto scale(GetAxisUnitsPerPixel(refAxis));
	const double axisLengthInUnits(targetAxis->GetAxisLength() * scale);// [unit]
	minLimit = centerRange - 0.5 * axisLengthInUnits;
	maxLimit = centerRange + 0.5 * axisLengthInUnits;
}

//=============================================================================
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
//=============================================================================
void PlotObject::UpdateLimitValues()
{
	mXMin = mAxisBottom->GetPrecisionLimitedMinimum();
	mXMax = mAxisBottom->GetPrecisionLimitedMaximum();

	mYLeftMin = mAxisLeft->GetPrecisionLimitedMinimum();
	mYLeftMax = mAxisLeft->GetPrecisionLimitedMaximum();

	mYRightMin = mAxisRight->GetPrecisionLimitedMinimum();
	mYRightMax = mAxisRight->GetPrecisionLimitedMaximum();
}

//=============================================================================
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
//=============================================================================
void PlotObject::AutoScaleAxis(double &min, double &max, double &majorRes,
	const int &maxTicks, const bool &logarithmic, const bool &forceLimits) const
{
	if (logarithmic)
		AutoScaleLogAxis(min, max, majorRes, forceLimits);
	else
		AutoScaleLinearAxis(min, max, majorRes, maxTicks, forceLimits);
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
void PlotObject::AutoScaleLinearAxis(double &min, double &max, double &majorRes,
	const int &maxTicks, const bool &forceLimits) const
{
	if (majorRes == 0.0)
		majorRes = PlotRenderer::ComputeTickSpacing(min, max, maxTicks);
	RoundMinMax(min, max, majorRes, forceLimits);
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
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

//=============================================================================
// Class:			PlotObject
// Function:		SetXMin
//
// Description:		Sets the lower X limit.
//
// Input Arguments:
//		xMin	= const double& describing desired minimum X limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetXMin(const double &xMin)
{
	// If the both limits are at the original value, enable auto-scaling again
	if (mXMax == mXMaxOriginal && xMin == mXMinOriginal && mXMajorResolution == 0.0)
		mAutoScaleX = true;
	else
		mAutoScaleX = false;
	mXMin = xMin;
}

//=============================================================================
// Class:			PlotObject
// Function:		SetXMax
//
// Description:		Sets the upper X limit.
//
// Input Arguments:
//		xMax	= const double& describing desired maximum X limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetXMax(const double &xMax)
{
	// If both limits are at the original value, enable auto-scaling again
	if (mXMin == mXMinOriginal && xMax == mXMaxOriginal && mXMajorResolution == 0.0)
		mAutoScaleX = true;
	else
		mAutoScaleX = false;
	mXMax = xMax;
}

//=============================================================================
// Class:			PlotObject
// Function:		SetLeftYMin
//
// Description:		Sets the lower Y limit.
//
// Input Arguments:
//		yMin	= const double& describing desired minimum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetLeftYMin(const double &yMin)
{
	// If both limits are at the original value, enable auto-scaling again
	if (mYLeftMax == mYLeftMaxOriginal && yMin == mYLeftMinOriginal && mYLeftMajorResolution == 0.0)
		mAutoScaleLeftY = true;
	else
		mAutoScaleLeftY = false;
	mYLeftMin = yMin;
}

//=============================================================================
// Class:			PlotObject
// Function:		SetLeftYMax
//
// Description:		Sets the upper Y limit.
//
// Input Arguments:
//		yMax	= const double& describing desired maximum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetLeftYMax(const double &yMax)
{
	// If both limits are at the original value, enable auto-scaling again
	if (mYLeftMin == mYLeftMinOriginal && yMax == mYLeftMaxOriginal && mYLeftMajorResolution == 0.0)
		mAutoScaleLeftY = true;
	else
		mAutoScaleLeftY = false;
	mYLeftMax = yMax;
}

//=============================================================================
// Class:			PlotObject
// Function:		SetRightYMin
//
// Description:		Sets the lower Y limit.
//
// Input Arguments:
//		yMin	= const double& describing desired minimum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetRightYMin(const double &yMin)
{
	// If the both limits are at the original value, enable auto-scaling again
	if (mYRightMax == mYRightMaxOriginal && yMin == mYRightMinOriginal && mYRightMajorResolution == 0.0)
		mAutoScaleRightY = true;
	else
		mAutoScaleRightY = false;
	mYRightMin = yMin;
}

//=============================================================================
// Class:			PlotObject
// Function:		SetRightYMax
//
// Description:		Sets the upper Y limit.
//
// Input Arguments:
//		yMax	= const double& describing desired maximum Y limit
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetRightYMax(const double &yMax)
{
	// If the both limits are at the original value, enable auto-scaling again
	if (mYRightMin == mYRightMinOriginal && yMax == mYRightMaxOriginal && mYRightMajorResolution == 0.0)
		mAutoScaleRightY = true;
	else
		mAutoScaleRightY = false;
	mYRightMax = yMax;
}

//=============================================================================
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
//=============================================================================
void PlotObject::ResetAutoScaling()
{
	mAutoScaleX = true;
	mAutoScaleLeftY = true;
	mAutoScaleRightY = true;

	mXMajorResolution = 0.0;
	mYLeftMajorResolution = 0.0;
	mYRightMajorResolution = 0.0;
}

//=============================================================================
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
//		lineSize	= const double&
//		markerSize	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void PlotObject::SetCurveProperties(const unsigned int &index, const Color &color,
	const bool &visible, const bool &rightAxis, const double &lineSize, const int &markerSize)
{
	mPlotList[index]->SetColor(color);
	mPlotList[index]->SetVisibility(visible);
	mPlotList[index]->SetLineSize(lineSize);
	mPlotList[index]->SetMarkerSize(markerSize);

	if (rightAxis)
		mPlotList[index]->BindToYAxis(mAxisRight);
	else
		mPlotList[index]->BindToYAxis(mAxisLeft);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetMajorGrid(const bool &gridOn)
{
	mAxisBottom->SetMajorGrid(gridOn);
	mAxisLeft->SetMajorGrid(gridOn);

	// These axis default to off, but can be specifically turned on via a right-click
	mAxisTop->SetMajorGrid(false);
	mAxisRight->SetMajorGrid(false);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetMinorGrid(const bool &gridOn)
{
	mAxisBottom->SetMinorGrid(gridOn);
	mAxisLeft->SetMinorGrid(gridOn);

	// These axis default to off, but can be specifically turned on via a right-click
	mAxisTop->SetMinorGrid(false);
	mAxisRight->SetMinorGrid(false);
}

//=============================================================================
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
//=============================================================================
bool PlotObject::GetMajorGrid()
{
	if (mAxisBottom == nullptr)
		return false;

	return mAxisBottom->GetMajorGrid();
}

//=============================================================================
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
//=============================================================================
bool PlotObject::GetMinorGrid()
{
	if (mAxisBottom == nullptr)
		return false;

	return mAxisBottom->GetMinorGrid();
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetXLabel(wxString text)
{
	mAxisBottom->SetLabel(text);
	mNeedScissorUpdate = true;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetLeftYLabel(wxString text)
{
	mAxisLeft->SetLabel(text);
	mNeedScissorUpdate = true;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetRightYLabel(wxString text)
{
	mAxisRight->SetLabel(text);
	mNeedScissorUpdate = true;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetTitle(wxString text)
{
	mTitleObject->SetText(text);
	mNeedScissorUpdate = true;
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetGridColor(const Color &color)
{
	mAxisBottom->SetGridColor(color);
	mAxisTop->SetGridColor(color);
	mAxisLeft->SetGridColor(color);
	mAxisRight->SetGridColor(color);
}

//=============================================================================
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
//=============================================================================
Color PlotObject::GetGridColor() const
{
	return mAxisBottom->GetGridColor();
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetXLogarithmic(const bool &log)
{
	if (!mAxisBottom)
		return;

	mAxisBottom->SetLogarithmicScale(log);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetLeftLogarithmic(const bool &log)
{
	if (!mAxisLeft)
		return;

	mAxisLeft->SetLogarithmicScale(log);
}

//=============================================================================
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
//=============================================================================
void PlotObject::SetRightLogarithmic(const bool &log)
{
	if (!mAxisRight)
		return;

	mAxisRight->SetLogarithmicScale(log);
}

//=============================================================================
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
//=============================================================================
void PlotObject::FormatCurves()
{
	for (auto& plot : mPlotList)
	{
		plot->SetModified();
		plot->SetPretty(mPretty);
	}
}

//=============================================================================
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
//=============================================================================
wxString PlotObject::GetXLabel() const
{
	return mAxisBottom->GetLabel();
}

//=============================================================================
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
//=============================================================================
wxString PlotObject::GetLeftYLabel() const
{
	return mAxisLeft->GetLabel();
}

//=============================================================================
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
//=============================================================================
wxString PlotObject::GetRightYLabel() const
{
	return mAxisRight->GetLabel();
}

//=============================================================================
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
//=============================================================================
wxString PlotObject::GetTitle() const
{
	return mTitleObject->GetText();
}

//=============================================================================
// Class:			PlotObject
// Function:		GetTotalPointCount
//
// Description:		Returns the total number of points contained in the curves.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned long long
//
//=============================================================================
unsigned long long PlotObject::GetTotalPointCount() const
{
	unsigned long long count(0);
	for (const auto& data : mDataList)
		count += data->GetNumberOfPoints();

	return count;
}

//=============================================================================
// Class:			PlotObject
// Function:		ComputeTransformationMatrices
//
// Description:		Calculates the transformation matrices for plot curves
//					associated with the right and left y-axes.
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
void PlotObject::ComputeTransformationMatrices()
{
	Eigen::Matrix4d left(Eigen::Matrix4d::Identity());
	Eigen::Matrix4d right(Eigen::Matrix4d::Identity());

	RenderWindow::Translate(left, Eigen::Vector3d(
		mAxisLeft->GetOffsetFromWindowEdge(),
		mAxisBottom->GetOffsetFromWindowEdge(), 0.0));
	RenderWindow::Translate(right, Eigen::Vector3d(
		mAxisLeft->GetOffsetFromWindowEdge(),
		mAxisBottom->GetOffsetFromWindowEdge(), 0.0));

	int width, height;
	mRenderer.GetSize(&width, &height);

	const double plotAreaWidth{ static_cast<double>(
		width - mAxisLeft->GetOffsetFromWindowEdge()
		- mAxisRight->GetOffsetFromWindowEdge()) };
	const double plotAreaHeight{ static_cast<double>(
		height - mAxisBottom->GetOffsetFromWindowEdge()
		- mAxisTop->GetOffsetFromWindowEdge()) };

	const double xScale{ plotAreaWidth /
		(mRenderer.DoXScale(mAxisBottom->GetMaximum())
		- mRenderer.DoXScale(mAxisBottom->GetMinimum())) };
	const double leftYScale{ plotAreaHeight /
		(mRenderer.DoLeftYScale(mAxisLeft->GetMaximum())
		- mRenderer.DoLeftYScale(mAxisLeft->GetMinimum())) };
	const double rightYScale{ plotAreaHeight /
		(mRenderer.DoRightYScale(mAxisRight->GetMaximum())
		- mRenderer.DoRightYScale(mAxisRight->GetMinimum())) };

	RenderWindow::Scale(left, Eigen::Vector3d(xScale, leftYScale, 1.0));
	RenderWindow::Scale(right, Eigen::Vector3d(xScale, rightYScale, 1.0));

	RenderWindow::Translate(left, Eigen::Vector3d(
		-mRenderer.DoXScale(mAxisBottom->GetMinimum()),
		-mRenderer.DoLeftYScale(mAxisLeft->GetMinimum()), 0.0));
	RenderWindow::Translate(right, Eigen::Vector3d(
		-mRenderer.DoXScale(mAxisBottom->GetMinimum()),
		-mRenderer.DoRightYScale(mAxisRight->GetMinimum()), 0.0));

	mRenderer.SetLeftModelview(left);
	mRenderer.SetRightModelview(right);
}

//=============================================================================
// Class:			PlotObject
// Function:		UpdateScissorArea
//
// Description:		Updates the scissor buffer according to the size of the
//					plot area.
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
void PlotObject::UpdateScissorArea() const
{
	assert(!RenderWindow::GLHasError());

	int width, height;
	mRenderer.GetSize(&width, &height);
	glEnable(GL_SCISSOR_TEST);
	glScissor(mAxisLeft->GetOffsetFromWindowEdge(),
		mAxisBottom->GetOffsetFromWindowEdge(),
		width - mAxisRight->GetOffsetFromWindowEdge() - mAxisLeft->GetOffsetFromWindowEdge(),
		height - mAxisTop->GetOffsetFromWindowEdge() - mAxisBottom->GetOffsetFromWindowEdge());

	glDisable(GL_SCISSOR_TEST);

	assert(!RenderWindow::GLHasError());
}

//=============================================================================
// Class:			PlotObject
// Function:		CurveMarkersVisible
//
// Description:		Checks to see if the specified curve has visible markers.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//=============================================================================
bool PlotObject::CurveMarkersVisible(const unsigned int& i) const
{
	return mPlotList[i]->NeedsMarkersDrawn();
}

}// namespace LibPlot2D
