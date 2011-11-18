/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_object_class.cpp
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Intermediate class for creating plots from arrays of data.
// History:

// FTGL headers
#include <FTGL/ftgl.h>

// Local headers
#include "application/plot_object_class.h"
#include "application/main_frame_class.h"
#include "renderer/plot_renderer_class.h"
#include "renderer/color_class.h"
#include "renderer/primitives/plot_curve.h"
#include "renderer/primitives/axis.h"
#include "renderer/primitives/text_class.h"
#include "utilities/math/plot_math.h"
#include "utilities/dataset2D.h"

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
	// Create the actors
	axisTop = new Axis(renderer);
	axisBottom = new Axis(renderer);
	axisLeft = new Axis(renderer);
	axisRight = new Axis(renderer);
	titleObject = new TextRendering(renderer);

	// Find the location of the fonts directory
	wxString fontFile;
#ifdef __WXMSW__
	fontFile = wxGetOSDirectory() + _T("\\fonts\\arial.ttf");
#elif defined __WXGTK__
	// FIXME:  This probably isn't very portable...
	fontFile = _T("/usr/share/fonts/dejavu/DejaVuSans.ttf");
#else
	// Unknown platfrom - warn the user
#	warning "Unrecognized platform - unable to locate font files!"
	fontFile = wxEmptyString;
#endif

	// Create the fonts
	axisFont = new FTGLTextureFont(fontFile.c_str());
	titleFont = new FTGLTextureFont(fontFile.c_str());

	// Make sure the fonts loaded OK
	if (axisFont->Error() || titleFont->Error())
	{
		delete axisFont;
		axisFont = NULL;

		delete titleFont;
		titleFont = NULL;
	}
	else
	{
		axisFont->FaceSize(12);
		axisFont->CharMap(FT_ENCODING_UNICODE);

		titleFont->FaceSize(18);
		titleFont->CharMap(FT_ENCODING_UNICODE);
	}

	// Initialize auto-scaling to true
	ResetAutoScaling();

	// Set the background color (establish the default)
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
	// Delete the font objects
	delete axisFont;
	axisFont = NULL;
	delete titleFont;
	titleFont = NULL;
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
	// Format the plot
	FormatPlot();

	renderer.UpdateCursors();

	// Update the display of cursor values
	dynamic_cast<MainFrame*>(renderer.GetParent())->UpdateCursorValues(
		renderer.GetLeftCursorVisible(), renderer.GetRightCursorVisible(),
		renderer.GetLeftCursorValue(), renderer.GetRightCursorValue());

	return;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXGrid
//
// Description:		Sets the status of the x-axis grid.
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
void PlotObject::SetXGrid(const bool &gridOn)
{
	axisBottom->SetGrid(gridOn);

	return;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetLeftYGrid
//
// Description:		Sets the status of the left y-axis grid.
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
void PlotObject::SetLeftYGrid(const bool &gridOn)
{
	axisLeft->SetGrid(gridOn);

	return;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetRightYGrid
//
// Description:		Sets the status of the right y-axis grid.
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
void PlotObject::SetRightYGrid(const bool &gridOn)
{
	axisRight->SetGrid(gridOn);

	return;
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
	// Remove all existing plots from the list
	while (plotList.GetCount() > 0)
		RemovePlot(0);

	return;
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
	// Remove the object from the renderer object
	renderer.RemoveActor(plotList[index]);

	// Remove it from the local list
	plotList.Remove(index);
	dataList.Remove(index);

	return;
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
	plotList.Add(newPlot);
	dataList.Add(&data);

	newPlot->BindToXAxis(axisBottom);
	newPlot->BindToYAxis(axisLeft);
	newPlot->SetData(&data);

	return;
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
	if (dataList.GetCount() == 0)
		return;

	// Determine "Original" values by parsing data associated with each axis
	bool leftFound = false;
	bool rightFound = false;
	unsigned int i, j;
	Axis *yAxis;
	for (i = 0; i < (unsigned int)dataList.GetCount(); i++)
	{
		// If this plot is not visible, ignore it
		if (!plotList[i]->GetIsVisible())
			continue;

		// Initialize the X limits the first time through only
		if (!leftFound && !rightFound)
		{
			xMinOriginal = dataList[i]->GetXData(0);
			xMaxOriginal = xMinOriginal;
		}

		// Determine which y-axis this dataset is associated with
		yAxis = plotList[i]->GetYAxis();
		if (yAxis == axisLeft && !leftFound)
		{
			leftFound = true;
			yLeftMinOriginal = dataList[i]->GetYData(0);
			yLeftMaxOriginal = yLeftMinOriginal;
		}
		else if (yAxis == axisRight && !rightFound)
		{
			rightFound = true;
			yRightMinOriginal = dataList[i]->GetYData(0);
			yRightMaxOriginal = yRightMinOriginal;
		}

		for (j = 0; j < dataList[i]->GetNumberOfPoints(); j++)
		{
			if (dataList[i]->GetXData(j) > xMaxOriginal)
				xMaxOriginal = dataList[i]->GetXData(j);
			else if (dataList[i]->GetXData(j) < xMinOriginal)
				xMinOriginal = dataList[i]->GetXData(j);

			if (yAxis == axisLeft)
			{
				if (dataList[i]->GetYData(j) > yLeftMaxOriginal)
					yLeftMaxOriginal = dataList[i]->GetYData(j);
				else if (dataList[i]->GetYData(j) < yLeftMinOriginal)
					yLeftMinOriginal = dataList[i]->GetYData(j);
			}
			else
			{
				if (dataList[i]->GetYData(j) > yRightMaxOriginal)
					yRightMaxOriginal = dataList[i]->GetYData(j);
				else if (dataList[i]->GetYData(j) < yRightMinOriginal)
					yRightMinOriginal = dataList[i]->GetYData(j);
			}
		}
	}

	// If one axis is unused, make it match the other
	if (leftFound && !rightFound)
	{
		yRightMinOriginal = yLeftMinOriginal;
		yRightMaxOriginal = yLeftMaxOriginal;
	}
	else if (!leftFound && rightFound)
	{
		yLeftMinOriginal = yRightMinOriginal;
		yLeftMaxOriginal = yRightMaxOriginal;
	}

	// Apply limits

	// Tell the curves they will need to be re-drawn
	for (i = 0; i < (unsigned int)plotList.GetCount(); i++)
		plotList[i]->SetModified();

	// If the axes mins and maxes are equal, stretch the range to make the plot display
	// We use IsZero to check for equality to avoid problems with very small numbers
	if (PlotMath::IsZero(xMaxOriginal - xMinOriginal))
	{
		if (PlotMath::IsZero(xMinOriginal))
		{
			xMinOriginal = -1.0;
			xMaxOriginal = 1.0;
		}
		else
		{
			xMinOriginal -= xMinOriginal * 0.1;
			xMaxOriginal += xMaxOriginal * 0.1;
		}
	}

	if (PlotMath::IsZero(yLeftMaxOriginal - yLeftMinOriginal))
	{
		if (PlotMath::IsZero(yLeftMinOriginal))
		{
			yLeftMinOriginal = -1.0;
			yLeftMaxOriginal = 1.0;
		}
		else
		{
			yLeftMinOriginal -= yLeftMinOriginal * 0.1;
			yLeftMaxOriginal += yLeftMaxOriginal * 0.1;
		}
	}

	if (PlotMath::IsZero(yRightMaxOriginal - yRightMinOriginal))
	{
		if (PlotMath::IsZero(yRightMinOriginal))
		{
			yRightMinOriginal = -1.0;
			yRightMaxOriginal = 1.0;
		}
		else
		{
			yRightMinOriginal -= yRightMinOriginal * 0.1;
			yRightMaxOriginal += yRightMaxOriginal * 0.1;
		}
	}

	// If we want to auto-scale the plot, set the range limits to the original values
	if (autoScaleX)
	{
		xMin = xMinOriginal;
		xMax = xMaxOriginal;
	}

	if (autoScaleLeftY)
	{
		yLeftMin = yLeftMinOriginal;
		yLeftMax = yLeftMaxOriginal;
	}

	if (autoScaleRightY)
	{
		yRightMin = yRightMinOriginal;
		yRightMax = yRightMaxOriginal;
	}

	// Set up the axes resolution (and at the same time tweak the max and min)
	// FIXME:  Make maximum number of ticks dependant on plot size and width of
	// number (i.e. 1 2 3 fits better than 0.001 0.002 0.003)
	double xMajorResolution = AutoScaleAxis(xMin, xMax, 7, !autoScaleX);
	double xMinorResolution = xMajorResolution;
	double yLeftMajorResolution = AutoScaleAxis(yLeftMin, yLeftMax, 10, !autoScaleLeftY);
	double yLeftMinorResolution = yLeftMajorResolution;
	double yRightMajorResolution = AutoScaleAxis(yRightMin, yRightMax, 10, !autoScaleRightY);
	double yRightMinorResolution = yRightMajorResolution;

	// Make sure the auto-scaled values are numbers
	// If they're not numbers, set them to +/- 1 and recalculate the tick spacing
	// (with inputs of +/- 1, they will always give valid results)
	if (PlotMath::IsNaN(xMin) || PlotMath::IsNaN(xMax))
	{
		xMin = -1.0;
		xMax = 1.0;
		xMajorResolution = AutoScaleAxis(xMin, xMax, 7, !autoScaleX);
		xMinorResolution = xMajorResolution;
	}

	if (PlotMath::IsNaN(yLeftMin) || PlotMath::IsNaN(yLeftMax))
	{
		yLeftMin = -1.0;
		yLeftMax = 1.0;
		yLeftMajorResolution = AutoScaleAxis(yLeftMin, yLeftMax, 7, !autoScaleLeftY);
		yLeftMinorResolution = yLeftMajorResolution;
	}

	if (PlotMath::IsNaN(yRightMin) || PlotMath::IsNaN(yRightMax))
	{
		yRightMin = -1.0;
		yRightMax = 1.0;
		yRightMajorResolution = AutoScaleAxis(yRightMin, yRightMax, 7, !autoScaleRightY);
		yRightMinorResolution = yRightMajorResolution;
	}

	// If we're auto-scaling, update the "original values" because chances are they
	// have been tweaked to make the numbers prettier
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

	// Apply the desired properties to each axis
	Axis::TickStyle tickStyle = Axis::TickStyleInside;

	axisBottom->SetOrientation(Axis::OrientationBottom);
	axisBottom->SetMinimum(xMin);
	axisBottom->SetMaximum(xMax);
	axisBottom->SetMinorResolution(xMinorResolution);
	axisBottom->SetMajorResolution(xMajorResolution);
	axisBottom->SetFont(axisFont);
	axisBottom->SetTickStyle(tickStyle);

	axisTop->SetOrientation(Axis::OrientationTop);
	axisTop->SetMinimum(xMin);
	axisTop->SetMaximum(xMax);
	axisTop->SetMinorResolution(xMinorResolution);
	axisTop->SetMajorResolution(xMajorResolution);
	axisTop->SetTickStyle(tickStyle);

	axisLeft->SetOrientation(Axis::OrientationLeft);
	axisLeft->SetMinimum(yLeftMin);
	axisLeft->SetMaximum(yLeftMax);
	axisLeft->SetMinorResolution(yLeftMinorResolution);
	axisLeft->SetMajorResolution(yLeftMajorResolution);
	axisLeft->SetFont(axisFont);
	//axisLeft->SetLabel(wxEmptyString);// FIXME
	axisLeft->SetTickStyle(tickStyle);

	axisRight->SetOrientation(Axis::OrientationRight);
	axisRight->SetMinimum(yRightMin);
	axisRight->SetMaximum(yRightMax);
	axisRight->SetMinorResolution(yRightMinorResolution);
	axisRight->SetMajorResolution(yRightMajorResolution);
	axisRight->SetFont(axisFont);
	//axisRight->SetLabel(wxEmptyString);// FIXME
	axisRight->SetTickStyle(tickStyle);

	// Set the title properties
	/*titleObject->SetFont(titleFont);
	titleObject->SetCentered(true);
	titleObject->SetText(wxEmptyString);
	titleObject->SetPosition(renderer.GetSize().GetWidth() / 2.0,
		renderer.GetSize().GetHeight() - 75.0 / 2.0);// 75.0 is from OffsetFromWindowEdge in axis.cpp*/

	// Set the axis colors
	Color color = Color::ColorBlack;
	axisBottom->SetColor(color);
	axisTop->SetColor(color);
	axisLeft->SetColor(color);
	axisRight->SetColor(color);

	// Update the axis limits so they are exactly the same as what is displayed on screen
	axisBottom->GenerateGeometry();
	xMin = axisBottom->GetMinimum();
	xMax = axisBottom->GetMaximum();

	axisLeft->GenerateGeometry();
	yLeftMin = axisLeft->GetMinimum();
	yLeftMax = axisLeft->GetMaximum();

	axisRight->GenerateGeometry();
	yRightMin = axisRight->GetMinimum();
	yRightMax = axisRight->GetMaximum();

	return;
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
//		maxTicks	= int specifying the maximum number of ticks to use
//		forceLimits	= const bool& specifying whether or not to preserve the specified limits
//
// Output Arguments:
//		min			= double& specifying the minimum value for the axis (required for input and output)
//		max			= double& specifying the maximum value for the axis (required for input and output)
//
// Return Value:
//		double, spacing between each tick mark for the axis (MajorResolution)
//
//==========================================================================
double PlotObject::AutoScaleAxis(double &min, double &max, int maxTicks, const bool &forceLimits)
{
	// Get the order of magnitude of the axes to decide how to scale them
	double range = max - min;
	int orderOfMagnitude = (int)log10(range);
	double tickSpacing = range / maxTicks;

	// Acceptable resolution steps are:
	//	Ones,
	//	Twos (even numbers), and
	//	Fives (multiples of five),
	// each within the order of magnitude (i.e. [37, 38, 39], [8.5, 9.0, 9.5], and [20, 40, 60] are all acceptable)

	// Determine which method will result in the least whitespace before and after the actual range,
	// and will get us closest to the maximum number of ticks.

	// Scale the tick spacing so it is between 0.1 and 10.0
	double scaledSpacing = tickSpacing / pow(10.0, orderOfMagnitude - 1);

	// Choose the maximum spacing value that fits our criteria
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

	// Re-scale back to the correct order of magnitude
	tickSpacing = scaledSpacing * pow(10.0, orderOfMagnitude - 1);

	// Round the min and max down and up, respectively, so the plot fits within the range [Min Max]
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

	// If numerical processing leads to ugly numbers, clean them up a bit
	if (PlotMath::IsZero(min))
		min = 0.0;
	if (PlotMath::IsZero(max))
		max = 0.0;

	return tickSpacing;
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
	// Make sure the limit is within the bounds of the original
	/*if (_xMin <= xMinOriginal)
	{
		xMin = xMinOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (xMax == xMaxOriginal)
			autoScaleX = true;

		return;
	}
	
	// Make the assignment and disable auto-scaling
	xMin = _xMin;
	autoScaleX = false;*/

	// If the both limits are at the original value, enable auto-scaling again
	if (xMax == xMaxOriginal && _xMin == xMinOriginal)
		autoScaleX = true;
	else
	{
		// Make the assignment and disable auto-scaling
		xMin = _xMin;
		autoScaleX = false;
	}

	return;
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
	// Make sure the limit is within the bounds of the original
	/*if (_xMax >= xMaxOriginal)
	{
		xMax = xMaxOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (xMin == xMinOriginal)
			autoScaleX = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	xMax = _xMax;
	autoScaleX = false;*/

	// If the both limits are at the original value, enable auto-scaling again
	if (xMin == xMinOriginal && _xMax == xMaxOriginal)
		autoScaleX = true;
	else
	{
		// Make the assignment and disable auto-scaling
		xMax = _xMax;
		autoScaleX = false;
	}

	return;
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
	// Make sure the limit is within the bounds of the original
	/*if (_yMin <= yLeftMinOriginal)
	{
		yLeftMin = yLeftMinOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (yLeftMax == yLeftMaxOriginal)
			autoScaleLeftY = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	yLeftMin = _yMin;
	autoScaleLeftY = false;*/

	// If the both limits are at the original value, enable auto-scaling again
	if (yLeftMax == yLeftMaxOriginal && _yMin == yLeftMinOriginal)
		autoScaleLeftY = true;
	else
	{
		// Make the assignment and disable auto-scaling
		yLeftMin = _yMin;
		autoScaleLeftY = false;
	}

	return;
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
	// Make sure the limit is within the bounds of the original
	/*if (_yMax >= yLeftMaxOriginal)
	{
		yLeftMax = yLeftMaxOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (yLeftMin == yLeftMinOriginal)
			autoScaleLeftY = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	yLeftMax = _yMax;
	autoScaleLeftY = false;*/

	// If the both limits are at the original value, enable auto-scaling again
	if (yLeftMin == yLeftMinOriginal && _yMax == yLeftMaxOriginal)
		autoScaleLeftY = true;
	else
	{
		// Make the assignment and disable auto-scaling
		yLeftMax = _yMax;
		autoScaleLeftY = false;
	}

	return;
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
	// Make sure the limit is within the bounds of the original
	/*if (_yMin <= yRightMinOriginal)
	{
		yRightMin = yRightMinOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (yRightMax == yRightMaxOriginal)
			autoScaleRightY = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	yRightMin = _yMin;
	autoScaleRightY = false;*/

	// If the both limits are at the original value, enable auto-scaling again
	if (yRightMax == yRightMaxOriginal && _yMin == yRightMinOriginal)
		autoScaleRightY = true;
	else
	{
		// Make the assignment and disable auto-scaling
		yRightMin = _yMin;
		autoScaleRightY = false;
	}

	return;
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
	// Make sure the limit is within the bounds of the original
	/*if (_yMax >= yRightMaxOriginal)
	{
		yRightMax = yRightMaxOriginal;

		// If the opposite limit is also at the original value, enable auto-scaling again
		if (yRightMin == yRightMinOriginal)
			autoScaleRightY = true;

		return;
	}

	// Make the assignment and disable auto-scaling
	yRightMax = _yMax;
	autoScaleRightY = false;*/

	// If the both limits are at the original value, enable auto-scaling again
	if (yRightMin == yRightMinOriginal && _yMax == yRightMaxOriginal)
		autoScaleRightY = true;
	else
	{
		// Make the assignment and disable auto-scaling
		yRightMax = _yMax;
		autoScaleRightY = false;
	}

	return;
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
	// Enable auto-scaling for all axes
	autoScaleX = true;
	autoScaleLeftY = true;
	autoScaleRightY = true;

	return;
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
//		size		= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotObject::SetCurveProperties(const unsigned int &index, const Color &color,
	const bool &visible, const bool &rightAxis, const unsigned int &size)
{
	plotList[index]->SetColor(color);
	plotList[index]->SetVisibility(visible);
	plotList[index]->SetSize(size);

	if (rightAxis)
		plotList[index]->BindToYAxis(axisRight);
	else
		plotList[index]->BindToYAxis(axisLeft);

	return;
}

//==========================================================================
// Class:			PlotObject
// Function:		SetGrid
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
void PlotObject::SetGrid(const bool &gridOn)
{
	axisBottom->SetGrid(gridOn);
	axisLeft->SetGrid(gridOn);

	// These axis default to off, but can be specifically turned on via a right-click
	axisTop->SetGrid(false);
	axisRight->SetGrid(false);

	return;
}

//==========================================================================
// Class:			PlotObject
// Function:		GetGrid
//
// Description:		Returns status of gridlines.
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
bool PlotObject::GetGrid(void)
{
	if (axisBottom == NULL)
		return false;

	return axisBottom->GetGrid();
}

//==========================================================================
// Class:			PlotObject
// Function:		SetXLabel
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
void PlotObject::SetXLabel(wxString text)
{
	axisBottom->SetLabel(text);

	return;
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