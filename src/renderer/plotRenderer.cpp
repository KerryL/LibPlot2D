/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotRenderer.cpp
// Created:  5/4/2011
// Author:  K. Loux
// Description:  Derived from RenderWindow, this class is used to display plots on
//				 the screen.
// History:

// Standard C++ headers
#include <cassert>
#include <algorithm>

// GLEW headers
#include <GL/glew.h>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "renderer/plotRenderer.h"
#include "application/plotObject.h"
#include "application/mainFrame.h"
#include "renderer/primitives/zoomBox.h"
#include "renderer/primitives/plotCursor.h"
#include "renderer/primitives/axis.h"
#include "renderer/primitives/legend.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			PlotRenderer
// Function:		Constant declarations
//
// Description:		Constant declarations for the PlotRenderer class.
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
const unsigned int PlotRenderer::maxXTicks(7);
const unsigned int PlotRenderer::maxYTicks(10);

//==========================================================================
// Class:			PlotRenderer
// Function:		defaultVertexShader
//
// Description:		Default vertex shader.
//
// Input Arguments:
//		0	= position
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const std::string PlotRenderer::defaultVertexShader(
	"#version 330\n"
	"\n"
	"uniform mat4 modelviewMatrix;\n"
	"uniform mat4 projectionMatrix;\n"
	"\n"
	"layout(location = 0) in vec2 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"\n"
	"out vec4 vertexColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vertexColor = color;\n"
	"    gl_Position = projectionMatrix * modelviewMatrix * vec4(position, 0.0, 1.0);\n"
	"}\n"
);

//==========================================================================
// Class:			PlotRenderer
// Function:		PlotRenderer
//
// Description:		Constructor for PlotRenderer class.
//
// Input Arguments:
//		wxParent	= wxWindow&
//		plotOwner	= PlotOwner& reference to this applications's main window
//		id			= wxWindowID
//		attr		= const wxGLAttributes& NOTE: Under GTK, must contain WX_GL_DOUBLEBUFFER at minimum
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PlotRenderer::PlotRenderer(wxWindow &wxParent, PlotOwner &plotOwner, wxWindowID id, const wxGLAttributes& attr)
	: RenderWindow(wxParent, id, attr, wxDefaultPosition, wxDefaultSize),
	plotOwner(plotOwner)
{
	CreateActors();

	SetView3D(false);

	draggingLeftCursor = false;
	draggingRightCursor = false;
	draggingLegend = false;

	ignoreNextMouseMove = false;

	curveQuality = QualityAlwaysHigh;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		~PlotRenderer
//
// Description:		Destructor for PlotRenderer class.
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
PlotRenderer::~PlotRenderer()
{
	delete plot;
	plot = NULL;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		Event Tables
//
// Description:		Event table for the PlotRenderer class.
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
BEGIN_EVENT_TABLE(PlotRenderer, RenderWindow)
	EVT_SIZE(PlotRenderer::OnSize)

	// Interaction events
	EVT_MOUSEWHEEL(		PlotRenderer::OnMouseWheelEvent)
	EVT_MOTION(			PlotRenderer::OnMouseMoveEvent)

	EVT_LEAVE_WINDOW(	PlotRenderer::OnMouseLeaveWindowEvent)

	// Click events
	EVT_LEFT_DCLICK(	PlotRenderer::OnDoubleClickEvent)
	EVT_RIGHT_UP(		PlotRenderer::OnRightButtonUpEvent)
	EVT_LEFT_UP(		PlotRenderer::OnLeftButtonUpEvent)
	EVT_LEFT_DOWN(		PlotRenderer::OnLeftButtonDownEvent)
	EVT_MIDDLE_UP(		PlotRenderer::OnMiddleButtonUpEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			PlotRenderer
// Function:		UpdateDisplay
//
// Description:		Updates the displayed plots to match the current data.
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
void PlotRenderer::UpdateDisplay()
{
	plot->Update();
	Refresh();
	Update();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		CreateActors
//
// Description:		Creates the actors for this plot.
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
void PlotRenderer::CreateActors()
{
	plot = new PlotObject(*this);
	SetBackgroundColor(Color::ColorWhite);

	// Also create the zoom box and cursors, even though they aren't drawn yet
	zoomBox = new ZoomBox(*this);
	leftCursor = new PlotCursor(*this, *plot->GetBottomAxis());
	rightCursor = new PlotCursor(*this, *plot->GetBottomAxis());

	if (!plot->GetAxisFont().empty())
	{
		plot->Update();// Need to make sure sizes update before we reference them to position the legend
		const unsigned int offset(5);
		legend = new Legend(*this);
		legend->SetFont(plot->GetAxisFont(), 12);
		legend->SetLegendReference(Legend::TopRight);
		legend->SetWindowReference(Legend::TopRight);
		legend->SetPosition(plot->GetRightYAxis()->GetOffsetFromWindowEdge() + offset,
			plot->GetTopAxis()->GetOffsetFromWindowEdge() + offset);
		legend->SetVisibility(false);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnSize
//
// Description:		Handles EVT_SIZE events for this class.  Required to make
//					the plot size update with the window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		event	= wxSizeEvent&
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnSize(wxSizeEvent &event)
{
	ignoreNextMouseMove = true;

	if (leftCursor->GetIsVisible())
		leftCursor->SetVisibility(true);
	if (rightCursor->GetIsVisible())
		rightCursor->SetVisibility(true);

	/*unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)// TODO:  Necessary?
		primitiveList[i]->SetModified();*/

	if (legend)
		legend->SetModified();

	plot->UpdatePlotAreaSize();
	UpdateDisplay();

	// Skip this event so the base class OnSize event fires, too
	event.Skip();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseWheelEvent
//
// Description:		Event handler for the mouse wheel event.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMouseWheelEvent(wxMouseEvent &event)
{
	if (view3D)
	{
		event.Skip();
		return;
	}

	// ZOOM in or out
	double zoomScaleX = 0.05;// [% of current scale]
	double zoomScaleY = 0.05;// [% of current scale]

	// If the CTRL key is down (and not SHIFT), only scale the X-axis
	if (event.ControlDown() && !event.ShiftDown())
		zoomScaleY = 0.0;

	// If the SHIFT key is down (and not CTRL), only scale the Y-axis
	else if (event.ShiftDown() && !event.ControlDown())
		zoomScaleX = 0.0;

	// TODO:  Focus the zooming around the cursor
	// Adjust the axis limits to achieve zooming
	double xDelta = (plot->GetXMax() - plot->GetXMin()) * zoomScaleX * event.GetWheelRotation() / 120.0;
	double yLeftDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * zoomScaleY * event.GetWheelRotation() / 120.0;
	double yRightDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * zoomScaleY * event.GetWheelRotation() / 120.0;

	plot->SetXMin(plot->GetXMin() + xDelta);
	plot->SetXMax(plot->GetXMax() - xDelta);
	plot->SetLeftYMin(plot->GetLeftYMin() + yLeftDelta);
	plot->SetLeftYMax(plot->GetLeftYMax() - yLeftDelta);
	plot->SetRightYMin(plot->GetRightYMin() + yRightDelta);
	plot->SetRightYMax(plot->GetRightYMax() - yRightDelta);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseMoveEvent
//
// Description:		Event handler for the mouse move event.  Only used to
//					capture drag events for rotating, panning, or dollying
//					the scene.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMouseMoveEvent(wxMouseEvent &event)
{
	if (view3D)
	{
		event.Skip();
		return;
	}

	if (!event.Dragging() || ignoreNextMouseMove)// ignoreNextMouseMove prevents panning on maximize by double clicking title bar or after creating a context menu
	{
		plot->SetPrettyCurves((curveQuality & QualityHighStatic) != 0);
		ignoreNextMouseMove = false;
		StoreMousePosition(event);
		return;
	}

	if (draggingLegend && legend)
		legend->SetDeltaPosition(event.GetX() - lastMousePosition[0], lastMousePosition[1] - event.GetY());
	else if (draggingLeftCursor)
		leftCursor->SetLocation(event.GetX());
	else if (draggingRightCursor)
		rightCursor->SetLocation(event.GetX());
	// ZOOM:  Left or Right mouse button + CTRL or SHIFT
	else if ((event.ControlDown() || event.ShiftDown()) && (event.RightIsDown() || event.LeftIsDown()))
		ProcessZoom(event);
	// ZOOM WITH BOX: Right mouse button
	else if (event.RightIsDown())
		ProcessZoomWithBox(event);
	// PAN:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
		ProcessPan(event);
	else// Not recognized
	{
		StoreMousePosition(event);
		return;
	}

	plot->SetPrettyCurves((curveQuality & QualityHighDrag) != 0);
	StoreMousePosition(event);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnRightButtonUpEvent
//
// Description:		Handles end of zoom-by-box events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnRightButtonUpEvent(wxMouseEvent &event)
{
	plot->SetPrettyCurves((curveQuality & QualityHighStatic) != 0);

	if (!zoomBox->GetIsVisible())
	{
		ProcessRightClick(event);
		return;
	}

	ProcessZoomBoxEnd();
	SaveCurrentZoom();
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMiddleButtonUpEvent
//
// Description:		Undoes the zoom level (if available).
//
// Input Arguments:
//		event	= wxMouseEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMiddleButtonUpEvent(wxMouseEvent& WXUNUSED(event))
{
	UndoZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ZoomChanged
//
// Description:		Determines if the zoom level is different from the previous
//					zoom level.
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
bool PlotRenderer::ZoomChanged() const
{
	if (zoom.size() == 0)
		return true;

	Zoom lastZoom = zoom.top();

	if (lastZoom.xMin == plot->GetXMin() &&
		lastZoom.xMax == plot->GetXMax() &&
		lastZoom.xMajor == plot->GetXMajorResolution() &&
		lastZoom.leftYMin == plot->GetLeftYMin() &&
		lastZoom.leftYMax == plot->GetLeftYMax() &&
		lastZoom.leftYMajor == plot->GetLeftYMajorResolution() &&
		lastZoom.rightYMin == plot->GetRightYMin() &&
		lastZoom.rightYMax == plot->GetRightYMax() &&
		lastZoom.rightYMajor == plot->GetRightYMajorResolution())
		return false;

	return true;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SaveCurrentZoom
//
// Description:		Saves the current zoom level.
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
void PlotRenderer::SaveCurrentZoom()
{
	if (!ZoomChanged())
		return;

	Zoom currentZoom;

	currentZoom.xMin = plot->GetXMin();
	currentZoom.xMax = plot->GetXMax();
	currentZoom.xMajor = plot->GetXMajorResolution();
	currentZoom.leftYMin = plot->GetLeftYMin();
	currentZoom.leftYMax = plot->GetLeftYMax();
	currentZoom.leftYMajor = plot->GetLeftYMajorResolution();
	currentZoom.rightYMin = plot->GetRightYMin();
	currentZoom.rightYMax = plot->GetRightYMax();
	currentZoom.rightYMajor = plot->GetRightYMajorResolution();

	zoom.push(currentZoom);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		UndoZoom
//
// Description:		Reverts to the previous zoom level.
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
void PlotRenderer::UndoZoom()
{
	if (zoom.size() < 2)
		return;

	zoom.pop();// Pop the current zoom in order to read the previous zoom
	Zoom lastZoom = zoom.top();

	plot->SetXMin(lastZoom.xMin);
	plot->SetXMax(lastZoom.xMax);
	plot->SetXMajorResolution(lastZoom.xMajor);
	plot->SetLeftYMin(lastZoom.leftYMin);
	plot->SetLeftYMax(lastZoom.leftYMax);
	plot->SetLeftYMajorResolution(lastZoom.leftYMajor);
	plot->SetRightYMin(lastZoom.rightYMin);
	plot->SetRightYMax(lastZoom.rightYMax);
	plot->SetRightYMajorResolution(lastZoom.rightYMajor);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ClearZoomStack
//
// Description:		Empties the zoom stack.
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
void PlotRenderer::ClearZoomStack()
{
	while (zoom.size() > 0)
		zoom.pop();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetMajorGridOn
//
// Description:		Returns status of the major grid lines.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for visible, false for hidden
//
//==========================================================================
bool PlotRenderer::GetMajorGridOn()
{
	return plot->GetMajorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetMinorGridOn
//
// Description:		Returns status of the minor grid lines.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for visible, false for hidden
//
//==========================================================================
bool PlotRenderer::GetMinorGridOn()
{
	return plot->GetMinorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetMajorGridOn
//
// Description:		Turns on plot major grid.
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
void PlotRenderer::SetMajorGridOn()
{
	plot->SetMajorGrid(true);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetMinorGridOn
//
// Description:		Turns on plot minor grid.
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
void PlotRenderer::SetMinorGridOn()
{
	plot->SetMinorGrid(true);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetMajorGridOff
//
// Description:		Turns off plot major grid.
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
void PlotRenderer::SetMajorGridOff()
{
	plot->SetMajorGrid(false);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetMinorGridOff
//
// Description:		Turns off plot minor grid.
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
void PlotRenderer::SetMinorGridOff()
{
	plot->SetMinorGrid(false);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetCurveQuality
//
// Description:		Updates the internal curve quality flag.
//
// Input Arguments:
//		curveQuality	= const CurveQuality&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetCurveQuality(const CurveQuality& curveQuality)
{
	this->curveQuality = curveQuality;
	plot->SetPrettyCurves((curveQuality & QualityHighStatic) != 0);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		LegendIsVisible
//
// Description:		Returns the status of the legend visibility.
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
bool PlotRenderer::LegendIsVisible()
{
	if (!legend)
		return false;
	return legend->GetIsVisible();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLegendOn
//
// Description:		Sets the legend to visible.
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
void PlotRenderer::SetLegendOn()
{
	if (!legend)
		return;
	legend->SetVisibility(true);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLegendOff
//
// Description:		Sets the legend to invisible.
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
void PlotRenderer::SetLegendOff()
{
	if (!legend)
		return;
	legend->SetVisibility(false);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		UpdateLegend
//
// Description:		Updates the contents of the legend.
//
// Input Arguments:
//		entries	= const std::vector<Legend::LegendEntryInfo>
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::UpdateLegend(const std::vector<Legend::LegendEntryInfo> &entries)
{
	if (legend)
		legend->SetContents(entries);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomMajorGrid
//
// Description:		Returns the status of the bottom major grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of bottom grid
//
//==========================================================================
bool PlotRenderer::GetBottomMajorGrid() const
{
	return plot->GetBottomAxis()->GetMajorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomMinorGrid
//
// Description:		Returns the status of the bottom minor grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of bottom grid
//
//==========================================================================
bool PlotRenderer::GetBottomMinorGrid() const
{
	return plot->GetBottomAxis()->GetMinorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftMajorGrid
//
// Description:		Returns the status of the left major grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of left grid
//
//==========================================================================
bool PlotRenderer::GetLeftMajorGrid() const
{
	return plot->GetLeftYAxis()->GetMajorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftMinorGrid
//
// Description:		Returns the status of the left minor grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of left grid
//
//==========================================================================
bool PlotRenderer::GetLeftMinorGrid() const
{
	return plot->GetLeftYAxis()->GetMinorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightMajorGrid
//
// Description:		Returns the status of the right major grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of right grid
//
//==========================================================================
bool PlotRenderer::GetRightMajorGrid() const
{
	return plot->GetRightYAxis()->GetMajorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightMinorGrid
//
// Description:		Returns the status of the right minor grid.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of right grid
//
//==========================================================================
bool PlotRenderer::GetRightMinorGrid() const
{
	return plot->GetRightYAxis()->GetMinorGrid();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetBottomMajorGrid
//
// Description:		Sets the status of the bottom axis' major grid.
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
void PlotRenderer::SetBottomMajorGrid(const bool &grid)
{
	plot->SetXMajorGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetBottomMinorGrid
//
// Description:		Sets the status of the bottom axis' minor grid.
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
void PlotRenderer::SetBottomMinorGrid(const bool &grid)
{
	plot->SetXMinorGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftMajorGrid
//
// Description:		Sets the status of the left axis' major grid.
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
void PlotRenderer::SetLeftMajorGrid(const bool &grid)
{
	plot->SetLeftYMajorGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftMinorGrid
//
// Description:		Sets the status of the left axis' minor grid.
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
void PlotRenderer::SetLeftMinorGrid(const bool &grid)
{
	plot->SetLeftYMinorGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightMajorGrid
//
// Description:		Sets the status of the right axis' major grid.
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
void PlotRenderer::SetRightMajorGrid(const bool &grid)
{
	plot->SetRightYMajorGrid(grid);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightMinorGrid
//
// Description:		Sets the status of the right axis' minor grid.
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
void PlotRenderer::SetRightMinorGrid(const bool &grid)
{
	plot->SetRightYMinorGrid(grid);
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetBottomMajorResolution
//
// Description:		Sets the bottom axis' major resolution.
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
void PlotRenderer::SetBottomMajorResolution(const double &resolution)
{
	plot->SetXMajorResolution(resolution);
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftMajorResolution
//
// Description:		Sets the left axis' major resolution.
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
void PlotRenderer::SetLeftMajorResolution(const double &resolution)
{
	plot->SetLeftYMajorResolution(resolution);
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightMajorResolution
//
// Description:		Sets the right axis' major resolution.
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
void PlotRenderer::SetRightMajorResolution(const double &resolution)
{
	plot->SetRightYMajorResolution(resolution);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomMajorResolution
//
// Description:		Returns the bottom axis' major resolution.
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
//==========================================================================
double PlotRenderer::GetBottomMajorResolution() const
{
	return plot->GetXMajorResolution();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomMajorResolution
//
// Description:		Returns the left axis' major resolution.
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
//==========================================================================
double PlotRenderer::GetLeftMajorResolution() const
{
	return plot->GetLeftYMajorResolution();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetBottomMajorResolution
//
// Description:		Returns the right axis' major resolution.
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
//==========================================================================
double PlotRenderer::GetRightMajorResolution() const
{
	return plot->GetRightYMajorResolution();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetCurveProperties
//
// Description:		Sets properties for the specified curve object.
//
// Input Arguments:
//		index		= const unsigned int& specifying the curve
//		color		= const Color& of the curve
//		visible		= const bool& indiciating whether or not the curve is to
//					  be drawn
//		rightAxis	= const bool& indicating whether the curve should be tied
//					  to the left or right axis
//		lineSize	= const double&
//		markerSize	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetCurveProperties(const unsigned int &index, const Color &color,
									  const bool &visible, const bool &rightAxis,
									  const double &lineSize, const int &markerSize)
{
	plot->SetCurveProperties(index, color, visible, rightAxis, lineSize, markerSize);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetXLimits
//
// Description:		Sets the axis limits for the X axis.
//
// Input Arguments:
//		min	= const double&
//		max = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetXLimits(const double &min, const double &max)
{
	if (max > min)
	{
		plot->SetXMax(max);
		plot->SetXMin(min);
	}
	else
	{
		plot->SetXMax(min);
		plot->SetXMin(max);
	}

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftYLimits
//
// Description:		Sets the axis limits for the left Y axis.
//
// Input Arguments:
//		min	= const double&
//		max = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetLeftYLimits(const double &min, const double &max)
{
	if (max > min)
	{
		plot->SetLeftYMax(max);
		plot->SetLeftYMin(min);
	}
	else
	{
		plot->SetLeftYMax(min);
		plot->SetLeftYMin(max);
	}

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightYLimits
//
// Description:		Sets the axis limits for the right Y axis.
//
// Input Arguments:
//		min	= const double&
//		max = const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::SetRightYLimits(const double &min, const double &max)
{
	if (max > min)
	{
		plot->SetRightYMax(max);
		plot->SetRightYMin(min);
	}
	else
	{
		plot->SetRightYMax(min);
		plot->SetRightYMin(max);
	}

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AddCurve
//
// Description:		Sets properties for the specified curve object.
//
// Input Arguments:
//		data	= const Dataset2D& to be plotted
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::AddCurve(const Dataset2D &data)
{
	plot->AddCurve(data);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		RemoveAllCurves
//
// Description:		Removes all curves from the plot.
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
void PlotRenderer::RemoveAllCurves()
{
	plot->RemoveExistingPlots();

	if (plot->GetCurveCount() == 0)
		ClearZoomStack();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		RemoveCurve
//
// Description:		Removes all curves from the plot.
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
void PlotRenderer::RemoveCurve(const unsigned int& index)
{
	plot->RemovePlot(index);

	if (plot->GetCurveCount() == 0)
		ClearZoomStack();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScale
//
// Description:		Enables auto-scaling of the axes.
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
void PlotRenderer::AutoScale()
{
	plot->ResetAutoScaling();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScaleBottom
//
// Description:		Enables auto-scaling of the bottom axis.
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
void PlotRenderer::AutoScaleBottom()
{
	plot->SetAutoScaleBottom();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScaleLeft
//
// Description:		Enables auto-scaling of the left axis.
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
void PlotRenderer::AutoScaleLeft()
{
	plot->SetAutoScaleLeft();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		AutoScaleRight
//
// Description:		Enables auto-scaling of the right axis.
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
void PlotRenderer::AutoScaleRight()
{
	plot->SetAutoScaleRight();
	UpdateDisplay();
	SaveCurrentZoom();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetXLabel
//
// Description:		Sets the text for the x-axis label.
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
void PlotRenderer::SetXLabel(wxString text)
{
	/*const int oldOffset(plot->GetVerticalAxisOffset(!plot->GetXLabel().IsEmpty()));
	const int newOffset(plot->GetVerticalAxisOffset(!text.IsEmpty()));

	if (oldOffset != newOffset)
	{
		if (legend->GetWindowReference() == Legend::BottomLeft ||
			legend->GetWindowReference() == Legend::BottomCenter ||
			legend->GetWindowReference() == Legend::BottomRight)
			legend->SetDeltaPosition(0, newOffset - oldOffset);
		else if (legend->GetWindowReference() == Legend::MiddleLeft ||
			legend->GetWindowReference() == Legend::Center ||
			legend->GetWindowReference() == Legend::MiddleRight)
			legend->SetDeltaPosition(0, (newOffset - oldOffset) / 2);
	}*/ // Adjust legend position as plot area size changes
	// Not implemented due to possiblity that legend would be moved off-screen
	// Also, when moved to bottom corners of the plot area, depending on window size, best anchor is reported as left/right middle instead of bottom
	// Better solution would anchor legend to corner of plot area if placed within plot area, or corner of window if placed outside of plot area

	plot->SetXLabel(text);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftYLabel
//
// Description:		Sets the text for the y-axis label.
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
void PlotRenderer::SetLeftYLabel(wxString text)
{
	const int oldOffset(plot->GetHorizontalAxisOffset(!plot->GetLeftYLabel().IsEmpty()));
	const int newOffset(plot->GetHorizontalAxisOffset(!text.IsEmpty()));

	if (oldOffset != newOffset)
	{
		if (legend->GetWindowReference() == Legend::BottomCenter ||
			legend->GetWindowReference() == Legend::Center ||
			legend->GetWindowReference() == Legend::TopCenter)
			legend->SetDeltaPosition((newOffset - oldOffset) / 2, 0);
		else if (legend->GetWindowReference() == Legend::BottomLeft ||
			legend->GetWindowReference() == Legend::MiddleLeft ||
			legend->GetWindowReference() == Legend::TopLeft)
			legend->SetDeltaPosition(newOffset - oldOffset, 0);
	}

	plot->SetLeftYLabel(text);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightYLabel
//
// Description:		Sets the text for the y-axis label.
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
void PlotRenderer::SetRightYLabel(wxString text)
{
	const int oldOffset(plot->GetHorizontalAxisOffset(!plot->GetRightYLabel().IsEmpty()));
	const int newOffset(plot->GetHorizontalAxisOffset(!text.IsEmpty()));

	if (oldOffset != newOffset)
	{
		if (legend->GetWindowReference() == Legend::BottomCenter ||
			legend->GetWindowReference() == Legend::Center ||
			legend->GetWindowReference() == Legend::TopCenter)
			legend->SetDeltaPosition((newOffset - oldOffset) / 2, 0);
		else if (legend->GetWindowReference() == Legend::BottomRight ||
			legend->GetWindowReference() == Legend::MiddleRight ||
			legend->GetWindowReference() == Legend::TopRight)
			legend->SetDeltaPosition(oldOffset - newOffset, 0);
	}

	plot->SetRightYLabel(text);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetTitle
//
// Description:		Sets the plot title text.
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
void PlotRenderer::SetTitle(wxString text)
{
	/*const int oldOffset(plot->GetVerticalAxisOffset(!plot->GetTitle().IsEmpty()));
	const int newOffset(plot->GetVerticalAxisOffset(!text.IsEmpty()));

	if (oldOffset != newOffset)
	{
		if (legend->GetWindowReference() == Legend::TopLeft ||
			legend->GetWindowReference() == Legend::TopCenter ||
			legend->GetWindowReference() == Legend::TopRight)
			legend->SetDeltaPosition(0, newOffset - oldOffset);
		else if (legend->GetWindowReference() == Legend::MiddleLeft ||
			legend->GetWindowReference() == Legend::Center ||
			legend->GetWindowReference() == Legend::MiddleRight)
			legend->SetDeltaPosition(0, (newOffset - oldOffset) / 2);
	}*/ // Adjust legend position as plot area size changes
	// Not implemented due to possiblity that legend would be moved off-screen
	// Also, when moved to bottom corners of the plot area, depending on window size, best anchor is reported as left/right middle instead of bottom
	// Better solution would anchor legend to corner of plot area if placed within plot area, or corner of window if placed outside of plot area

	plot->SetTitle(text);
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseLeaveWindowEvent
//
// Description:		Cleans up some zoom box and cursor items.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnMouseLeaveWindowEvent(wxMouseEvent& WXUNUSED(event))
{
	// Hide the zoom box (but only if it's not already hidden!)
	if (zoomBox->GetIsVisible())
		zoomBox->SetVisibility(false);

	draggingLegend = false;
	draggingLeftCursor = false;
	draggingRightCursor = false;

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnDoubleClickEvent
//
// Description:		Handles double click events.  Allows user to change axis
//					limits or create a cursor.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnDoubleClickEvent(wxMouseEvent &event)
{
	unsigned int x = event.GetX();
	unsigned int y = event.GetY();

	// If the click is within the plot area, move a cursor there and make it visible
	if (x > plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		x < GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		ProcessPlotAreaDoubleClick(x);
	else
		ProcessOffPlotDoubleClick(x, y);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnMouseLeftDownEvent
//
// Description:		Checks to see if the user is dragging a cursor.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnLeftButtonDownEvent(wxMouseEvent &event)
{
	// Check to see if we're on a cursor or the legend
	if (legend && legend->IsUnder(event.GetX(), GetSize().GetHeight() - event.GetY()))
		draggingLegend = true;
	else if (leftCursor->IsUnder(event.GetX()))
		draggingLeftCursor = true;
	else if (rightCursor->IsUnder(event.GetX()))
		draggingRightCursor = true;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		OnLeftButtonUpEvent
//
// Description:		Makes sure we stop dragging when we stop clicking.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::OnLeftButtonUpEvent(wxMouseEvent& WXUNUSED(event))
{
	plot->SetPrettyCurves((curveQuality & QualityHighStatic) != 0);

	if (draggingLegend)
	{
		// TODO:  If legend is off screen, reset to default position and turn visibility off

		UpdateLegendAnchor();
	}

	draggingLegend = false;
	draggingLeftCursor = false;
	draggingRightCursor = false;

	SaveCurrentZoom();
	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		UpdateLegendAnchor
//
// Description:		Updates the anchor depending on legend position.
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
void PlotRenderer::UpdateLegendAnchor()
{
	if (!legend)
		return;

	std::vector<std::pair<double, Legend::PositionReference> > distances;
	double x, y;

	legend->GetPosition(Legend::BottomLeft, Legend::BottomLeft, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::BottomLeft));

	legend->GetPosition(Legend::BottomCenter, Legend::BottomCenter, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::BottomCenter));

	legend->GetPosition(Legend::BottomRight, Legend::BottomRight, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::BottomRight));

	legend->GetPosition(Legend::MiddleLeft, Legend::MiddleLeft, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::MiddleLeft));

	legend->GetPosition(Legend::Center, Legend::Center, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::Center));

	legend->GetPosition(Legend::MiddleRight, Legend::MiddleRight, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::MiddleRight));

	legend->GetPosition(Legend::TopLeft, Legend::TopLeft, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::TopLeft));

	legend->GetPosition(Legend::TopCenter, Legend::TopCenter, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::TopCenter));

	legend->GetPosition(Legend::TopRight, Legend::TopRight, x, y);
	distances.push_back(std::make_pair(x * x + y * y, Legend::TopRight));

	Legend::PositionReference bestRef = std::min_element(distances.begin(), distances.end())->second;
	legend->GetPosition(bestRef, bestRef, x, y);
	legend->SetWindowReference(bestRef);
	legend->SetLegendReference(bestRef);
	legend->SetPosition(x, y);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftCursorVisible
//
// Description:		Returns status of left cursor visibility flag.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of left cursor visibility flag
//
//==========================================================================
bool PlotRenderer::GetLeftCursorVisible() const
{
	return leftCursor->GetIsVisible();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightCursorVisible
//
// Description:		Returns status of right cursor visibility flag.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool indicating status of right cursor visibility flag
//
//==========================================================================
bool PlotRenderer::GetRightCursorVisible() const
{
	return rightCursor->GetIsVisible();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftCursorValue
//
// Description:		Returns x-value of left cursor.
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
//==========================================================================
double PlotRenderer::GetLeftCursorValue() const
{
	return leftCursor->GetValue();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightCursorValue
//
// Description:		Makes sure we stop dragging when we stop clicking.
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
//==========================================================================
double PlotRenderer::GetRightCursorValue() const
{
	return rightCursor->GetValue();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		UpdateCursors
//
// Description:		Updates the cursor calculations.
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
void PlotRenderer::UpdateCursors()
{
	// Tell the cursors they need to recalculate
	leftCursor->SetModified();
	rightCursor->SetModified();

	// Calculations are performed on Draw
	leftCursor->Draw();
	rightCursor->Draw();

	Refresh();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetXMin() const
{
	return plot->GetBottomAxis()->GetMinimum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetXMax() const
{
	return plot->GetBottomAxis()->GetMaximum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetLeftYMin() const
{
	return plot->GetLeftYAxis()->GetMinimum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetLeftYMax() const
{
	return plot->GetLeftYAxis()->GetMaximum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetRightYMin() const
{
	return plot->GetRightYAxis()->GetMinimum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXMin
//
// Description:		Returns the minimum value of the X-axis.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		double indicating the minimum value of the X-axis
//
//==========================================================================
double PlotRenderer::GetRightYMax() const
{
	return plot->GetRightYAxis()->GetMaximum();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetGridColor
//
// Description:		Returns the color of the gridlines for this plot.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Color
//
//==========================================================================
Color PlotRenderer::GetGridColor() const
{
	return plot->GetGridColor();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetGridColor
//
// Description:		Sets the color of the gridlines for this plot.
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
void PlotRenderer::SetGridColor(const Color &color)
{
	plot->SetGridColor(color);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXLogarithmic
//
// Description:		Returns a boolean indicating whether or not the X axis
//					is scaled logarithmicly.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the X axis has a logarithmic scale
//
//==========================================================================
bool PlotRenderer::GetXLogarithmic() const
{
	if (plot->GetBottomAxis())
		return plot->GetBottomAxis()->IsLogarithmic();

	return false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftLogarithmic
//
// Description:		Returns a boolean indicating whether or not the left Y axis
//					is scaled logarithmicly.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the left Y axis has a logarithmic scale
//
//==========================================================================
bool PlotRenderer::GetLeftLogarithmic() const
{
	if (plot->GetLeftYAxis())
		return plot->GetLeftYAxis()->IsLogarithmic();

	return false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightLogarithmic
//
// Description:		Returns a boolean indicating whether or not the right Y axis
//					is scaled logarithmicly.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the right Y axis has a logarithmic scale
//
//==========================================================================
bool PlotRenderer::GetRightLogarithmic() const
{
	if (plot->GetRightYAxis())
		return plot->GetRightYAxis()->IsLogarithmic();

	return false;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetXLogarithmic
//
// Description:		Sets the X axis to be logarithmic or standard scaling.
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
void PlotRenderer::SetXLogarithmic(const bool &log)
{
	plot->SetXLogarithmic(log);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetLeftLogarithmic
//
// Description:		Sets the left Y axis to be logarithmic or standard scaling.
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
void PlotRenderer::SetLeftLogarithmic(const bool &log)
{
	plot->SetLeftLogarithmic(log);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		SetRightLogarithmic
//
// Description:		Sets the right Y axis to be logarithmic or standard scaling.
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
void PlotRenderer::SetRightLogarithmic(const bool &log)
{
	plot->SetRightLogarithmic(log);

	UpdateDisplay();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXAxisZoomed
//
// Description:		Determines if the x-axis is zoomed (not autoscaled) and
//					returns the result.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the user zoomed in (or panned) along the x-axis
//
//==========================================================================
bool PlotRenderer::GetXAxisZoomed() const
{
	if (!plot->GetBottomAxis())
		return false;

	return !plot->GetXAxisAutoScaled();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessZoom
//
// Description:		Handles mouse-drag zoom events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessZoom(wxMouseEvent &event)
{
	// CTRL for Left Y-zoom
	// SHIFT for Right Y-zoom

	// ZOOM in or out
	double zoomXScale = 0.005 * (event.GetX() - lastMousePosition[0]);// [% of current scale]
	double zoomYScale = 0.005 * (event.GetY() - lastMousePosition[1]);// [% of current scale]

	// TODO:  Focus the zooming around the cursor
	// Adjust the axis limits
	double xDelta = (plot->GetXMax() - plot->GetXMin()) * zoomXScale;
	double yLeftDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * zoomYScale * (int)event.ControlDown();
	double yRightDelta = (plot->GetRightYMax() - plot->GetRightYMin()) * zoomYScale * (int)event.ShiftDown();

	// Left mouse button fixes left and bottom corner, right button fixes right and top corner
	if (event.LeftIsDown())
	{
		plot->SetXMax(plot->GetXMax() - xDelta);
		plot->SetLeftYMax(plot->GetLeftYMax() + yLeftDelta);
		plot->SetRightYMax(plot->GetRightYMax() + yRightDelta);
	}
	else
	{
		plot->SetXMin(plot->GetXMin() - xDelta);
		plot->SetLeftYMin(plot->GetLeftYMin() + yLeftDelta);
		plot->SetRightYMin(plot->GetRightYMin() + yRightDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessZoomWithBox
//
// Description:		Handles mouse-drag zoom box events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessZoomWithBox(wxMouseEvent &event)
{
	unsigned int x;
	unsigned int y;

	if (!zoomBox->GetIsVisible())
	{
		x = lastMousePosition[0];
		y = lastMousePosition[1];
		ForcePointWithinPlotArea(x, y);

		zoomBox->SetVisibility(true);
		zoomBox->SetAnchorCorner(x, GetSize().GetHeight() - y);
	}

	x = event.GetX();
	y = event.GetY();
	ForcePointWithinPlotArea(x, y);

	// Tell the zoom box where to draw the floaing corner
	zoomBox->SetFloatingCorner(x, GetSize().GetHeight() - y);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessPan
//
// Description:		Handles mouse-drag pan events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessPan(wxMouseEvent &event)
{
	PanBottomXAxis(event);
	PanLeftYAxis(event);
	PanRightYAxis(event);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		PanBottomXAxis
//
// Description:		Pans the bottom x-axis in resposne to mouse moves.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::PanBottomXAxis(wxMouseEvent &event)
{
	int width = GetSize().GetWidth() - plot->GetLeftYAxis()->GetOffsetFromWindowEdge() - plot->GetRightYAxis()->GetOffsetFromWindowEdge();

	if (plot->GetBottomAxis()->IsLogarithmic())
	{
		int pixelDelta = event.GetX() - lastMousePosition[0];
		plot->SetXMin(plot->GetBottomAxis()->PixelToValue(
			(int)plot->GetLeftYAxis()->GetOffsetFromWindowEdge() - pixelDelta));
		plot->SetXMax(plot->GetBottomAxis()->PixelToValue(
			GetSize().GetWidth() - (int)plot->GetRightYAxis()->GetOffsetFromWindowEdge() - pixelDelta));
	}
	else
	{
		double xDelta = (plot->GetXMax() - plot->GetXMin()) * (event.GetX() - lastMousePosition[0]) / width;
		plot->SetXMin(plot->GetXMin() - xDelta);
		plot->SetXMax(plot->GetXMax() - xDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		PanLeftYAxis
//
// Description:		Pans the left y-axis in resposne to mouse moves.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::PanLeftYAxis(wxMouseEvent &event)
{
	int height = GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() - plot->GetTopAxis()->GetOffsetFromWindowEdge();

	if (plot->GetLeftYAxis()->IsLogarithmic())
	{
		int pixelDelta = event.GetY() - lastMousePosition[1];
		plot->SetLeftYMin(plot->GetLeftYAxis()->PixelToValue(
			(int)plot->GetBottomAxis()->GetOffsetFromWindowEdge() + pixelDelta));
		plot->SetLeftYMax(plot->GetLeftYAxis()->PixelToValue(
			GetSize().GetHeight() - (int)plot->GetTopAxis()->GetOffsetFromWindowEdge() + pixelDelta));
	}
	else
	{
		double yLeftDelta = (plot->GetLeftYMax() - plot->GetLeftYMin()) * (event.GetY() - lastMousePosition[1]) / height;
		plot->SetLeftYMin(plot->GetLeftYMin() + yLeftDelta);
		plot->SetLeftYMax(plot->GetLeftYMax() + yLeftDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		PanRightYAxis
//
// Description:		Pans the right y-axis in resposne to mouse moves.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::PanRightYAxis(wxMouseEvent &event)
{
	int height = GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() - plot->GetTopAxis()->GetOffsetFromWindowEdge();

	if (plot->GetRightYAxis()->IsLogarithmic())
	{
		int pixelDelta = event.GetY() - lastMousePosition[1];
		plot->SetRightYMin(plot->GetRightYAxis()->PixelToValue(
			(int)plot->GetBottomAxis()->GetOffsetFromWindowEdge() + pixelDelta));
		plot->SetRightYMax(plot->GetRightYAxis()->PixelToValue(
			GetSize().GetHeight() - (int)plot->GetTopAxis()->GetOffsetFromWindowEdge() + pixelDelta));
	}
	else
	{
		double yRightDelta = (plot->GetRightYMax() - plot->GetRightYMin()) * (event.GetY() - lastMousePosition[1]) / height;
		plot->SetRightYMin(plot->GetRightYMin() + yRightDelta);
		plot->SetRightYMax(plot->GetRightYMax() + yRightDelta);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessPlotAreaDoubleClick
//
// Description:		Processes double click events occurring within the plot area.
//
// Input Arguments:
//		x	= const unsigned int&, location of click along x-axis
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessPlotAreaDoubleClick(const unsigned int &x)
{
	double value(plot->GetBottomAxis()->PixelToValue(x));

	if (!leftCursor->GetIsVisible())
	{
		leftCursor->SetVisibility(true);
		leftCursor->SetLocation(x);
	}
	else if (!rightCursor->GetIsVisible())
	{
		rightCursor->SetVisibility(true);
		rightCursor->SetLocation(x);
	}
	else
	{
		// Both cursors are visible - move the closer one to the click spot
		// NOTE:  Another option is to always alternate which one was moved?
		if (fabs(leftCursor->GetValue() - value) < fabs(rightCursor->GetValue() - value))
			leftCursor->SetLocation(x);
		else
			rightCursor->SetLocation(x);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessOffPlotDoubleClick
//
// Description:		Process double click events that occured outside the plot area.
//
// Input Arguments:
//		x	= const unsigned int&, location of click along x-axis
//		y	= const unsigned int&, location of click along y-axis
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessOffPlotDoubleClick(const unsigned int &x, const unsigned int &y)
{
	// Determine the context
	PlotOwner::PlotContext context;
	if (x < plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = PlotOwner::PlotContextLeftYAxis;
	else if (x > GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = PlotOwner::PlotContextRightYAxis;
	else if (y > GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() &&
		x > plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		x < GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge())
		context = PlotOwner::PlotContextXAxis;
	else
		context = PlotOwner::PlotContextPlotArea;

	plotOwner.DisplayAxisRangeDialog(context);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessRightClick
//
// Description:		Processes right click events.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ProcessRightClick(wxMouseEvent &event)
{
	// Determine the context
	PlotOwner::PlotContext context;
	unsigned int x = event.GetX();
	unsigned int y = event.GetY();
	if (x < plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = PlotOwner::PlotContextLeftYAxis;
	else if (x > GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge() &&
		y > plot->GetTopAxis()->GetOffsetFromWindowEdge() &&
		y < GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		context = PlotOwner::PlotContextRightYAxis;
	else if (y > GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge() &&
		x > plot->GetLeftYAxis()->GetOffsetFromWindowEdge() &&
		x < GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge())
		context = PlotOwner::PlotContextXAxis;
	else
		context = PlotOwner::PlotContextPlotArea;

	// Display the context menu (further events handled by Parent)
	plotOwner.CreatePlotContextMenu(GetPosition() + event.GetPosition(), context);
	ignoreNextMouseMove = true;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ProcessZoomBoxEnd
//
// Description:		Completes zoom and cleans up box after user releases zoom box.
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
void PlotRenderer::ProcessZoomBoxEnd()
{
	zoomBox->SetVisibility(false);
	
	if (plot->GetCurveCount() == 0)
		return;

	// Make sure the box isn't too small
	const int limit = 5;// [pixels]
	if (abs(int(zoomBox->GetXAnchor() - zoomBox->GetXFloat())) > limit &&
		abs(int(zoomBox->GetYAnchor() - zoomBox->GetYFloat())) > limit)
	{
		// Determine the new zoom range
		// Remember: OpenGL uses Bottom Left as origin, normal windows use Top Left as origin
		double xMin = plot->GetBottomAxis()->PixelToValue(
			std::min<unsigned int>(zoomBox->GetXFloat(), zoomBox->GetXAnchor()));
		double xMax = plot->GetBottomAxis()->PixelToValue(
			std::max<unsigned int>(zoomBox->GetXFloat(), zoomBox->GetXAnchor()));
		double yLeftMin = plot->GetLeftYAxis()->PixelToValue(
			std::min<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));
		double yLeftMax = plot->GetLeftYAxis()->PixelToValue(
			std::max<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));
		double yRightMin = plot->GetRightYAxis()->PixelToValue(
			std::min<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));
		double yRightMax = plot->GetRightYAxis()->PixelToValue(
			std::max<unsigned int>(zoomBox->GetYFloat(), zoomBox->GetYAnchor()));

		ComputePrettyLimits(xMin, xMax, maxXTicks);
		ComputePrettyLimits(yLeftMin, yLeftMax, maxYTicks);
		ComputePrettyLimits(yRightMin, yRightMax, maxYTicks);

		SetXLimits(xMin, xMax);
		SetLeftYLimits(yLeftMin, yLeftMax);
		SetRightYLimits(yRightMin, yRightMax);
	}
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ComputePrettyLimits
//
// Description:		Computes new limits given the specified range to ensure
//					pretty tick spacing.
//
// Input Arguments:
//		min			= double&
//		max			= double&
//		maxTicks	= const unsigned int&
//
// Output Arguments:
//		min	= double&
//		max	= double&
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ComputePrettyLimits(double &min, double &max, const unsigned int& maxTicks) const
{
	// Make the limits prettier by choosing a range that is exactly divisible
	// into the ideal spacing
	double spacing = ComputeTickSpacing(min, max, maxTicks);
	assert(spacing > 0.0 && PlotMath::IsValid(spacing));
	double range = floor((max - min) / spacing + 0.5) * spacing;

	// Split the difference to force the range to the desired value (keep the center point
	// the same before/after this adjustment)
	double shift = 0.5 * (range - max + min);
	max += shift;
	min -= shift;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ForcePointWithinPlotArea
//
// Description:		If the specified point is not within the plot area, it
//					modifies the coordinates such that it is.
//
// Input Arguments:
//		x	= unsigned int&
//		y	= unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::ForcePointWithinPlotArea(unsigned int &x, unsigned int &y)
{
	if (x < plot->GetLeftYAxis()->GetOffsetFromWindowEdge())
		x = plot->GetLeftYAxis()->GetOffsetFromWindowEdge();
	else if (x > GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge())
		x = GetSize().GetWidth() - plot->GetRightYAxis()->GetOffsetFromWindowEdge();

	if (y < plot->GetTopAxis()->GetOffsetFromWindowEdge())
		y = plot->GetTopAxis()->GetOffsetFromWindowEdge();
	else if (y > GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge())
		y = GetSize().GetHeight() - plot->GetBottomAxis()->GetOffsetFromWindowEdge();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetXLabel
//
// Description:		Return the value of the label text.
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
wxString PlotRenderer::GetXLabel() const
{
	return plot->GetXLabel();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetLeftYLabel
//
// Description:		Return the value of the label text.
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
wxString PlotRenderer::GetLeftYLabel() const
{
	return plot->GetLeftYLabel();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetRightYLabel
//
// Description:		Return the value of the label text.
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
wxString PlotRenderer::GetRightYLabel() const
{
	return plot->GetRightYLabel();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetTitle
//
// Description:		Return the value of the label text.
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
wxString PlotRenderer::GetTitle() const
{
	return plot->GetTitle();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		ComputeTickSpacing
//
// Description:		Computes the ideal tick spacing based on the specified range.
//
// Input Arguments:
//		min		= const
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
double PlotRenderer::ComputeTickSpacing(const double &min, const double &max,
	const int &maxTicks)
{
	assert(max > min);
	double range = max - min;
	int orderOfMagnitude = (int)log10(range);

	// To prevent changing the number of ticks as we drag, we should round the range here
	// The issues is that a range of about 10 could be 9.99999... or 10.000...1
	// and this changes the calculated order of magnitude
	// TODO:  Would it be better to compute the precision and round the min/max values
	// like we do in Axis?  When we do get undesired artifacts, moving the mouse off the
	// edge of the plot will re-draw without these artifacts, due to the logic in Axis
	const double roundingScale(pow(10.0, 2 - orderOfMagnitude));// So we don't loose too much precision
	range = floor(range * roundingScale + 0.5) / roundingScale;

	orderOfMagnitude = (int)log10(range);
	double tickSpacing = range / maxTicks;

	// Acceptable resolution steps are:
	//	Ones,
	//	Twos (even numbers), and
	//	Fives (multiples of five),
	// each within the order of magnitude (i.e. [37, 38, 39], [8.5, 9.0, 9.5], and [20, 40, 60] are all acceptable)

	// Scale the tick spacing so it is between 0.1 and 10.0
	double scaledSpacing = tickSpacing / pow(10.0, orderOfMagnitude - 1);

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

	return scaledSpacing * pow(10.0, orderOfMagnitude - 1);
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetImage
//
// Description:		Returns a copy of the current image.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxImage
//
//==========================================================================
wxImage PlotRenderer::GetImage() const
{
	plot->SetPrettyCurves((curveQuality & QualityHighWrite) != 0);
	if (((curveQuality & QualityHighStatic) != 0) != ((curveQuality & QualityHighWrite) != 0))
		{/*UpdateDisplay();*/}// TODO:  Can't call from const method, so HighQualityWrite flag currently doesn't work

	wxImage newImage(RenderWindow::GetImage());
	plot->SetPrettyCurves((curveQuality & QualityHighStatic) != 0);
	return newImage;
}

//==========================================================================
// Class:			PlotRenderer
// Function:		GetTotalPointCount
//
// Description:		Returns total number of points stored in plot curves.
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
//==========================================================================
unsigned long long PlotRenderer::GetTotalPointCount() const
{
	return plot->GetTotalPointCount();
}

//==========================================================================
// Class:			PlotRenderer
// Function:		LoadModelviewUniform
//
// Description:		Loads the specified modelview matrix to the openGL uniform.
//
// Input Arguments:
//		mv	= const Modelview&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PlotRenderer::LoadModelviewUniform(const Modelview& mv)
{
	float glModelviewMatrix[16];

	switch(mv)
	{
	case ModelviewLeft:
		ConvertMatrixToGL(leftModelview, glModelviewMatrix);
		break;

	case ModelviewRight:
		ConvertMatrixToGL(rightModelview, glModelviewMatrix);
		break;

	default:
	case ModelviewFixed:
		ConvertMatrixToGL(modelviewMatrix, glModelviewMatrix);
	}

	glUniformMatrix4fv(modelviewLocation, 1, GL_FALSE, glModelviewMatrix);
}
