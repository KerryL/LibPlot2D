/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  render_window_class.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:

// Standard C++ headers
#include <vector>
#include <algorithm>

// wxWidgets headers
#include <wx/dcclient.h>
#include <wx/image.h>

// Local headers
#include "renderer/render_window_class.h"
#include "utilities/math/matrix_class.h"
#include "utilities/math/vector_class.h"
#include "utilities/math/plot_math.h"

//==========================================================================
// Class:			RenderWindow
// Function:		RenderWindow
//
// Description:		Constructor for RenderWindow class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Argurments:
//		parent		= wxWindow& reference to the owner of this object
//		id			= wxWindowID to identify this window
//		position	= const wxPoint& specifying this object's position
//		size		= const wxSize& specifying this object's size
//		style		= long specifying this object's style flags
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
RenderWindow::RenderWindow(wxWindow &parent, wxWindowID id,
    const wxPoint& position, const wxSize& size, long style) : wxGLCanvas(
	&parent, id, position, size, style | wxFULL_REPAINT_ON_RESIZE)
{
	// Initialize the private data
	wireFrame = false;
	view3D = true;
	viewOrthogonal = false;

	// Initialize the frustum parameters
	AutoSetFrustum();

	// Initialize the transformation matricies
	modelToView = new Matrix(3, 3);
	modelToView->MakeIdentity();

	viewToModel = new Matrix(3, 3);
	viewToModel->MakeIdentity();

	// Initialize the camera position
	cameraPosition.Set(0.0, 0.0, 0.0);

	// Initialize the focal point parameters
	focalPoint.Set(0.0, 0.0, 0.0);
	isInteracting = false;

	// Initialize the openGL settings first time we try to render
	modified = true;
}

//==========================================================================
// Class:			RenderWindow
// Function:		~RenderWindow
//
// Description:		Destructor for RenderWindow class.  Deletes objects in
//					the scene and other dynamic variables.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
RenderWindow::~RenderWindow()
{
	// Clear out the list of primitives
	primitiveList.Clear();

	// Delete the transformation matricies
	delete modelToView;
	modelToView = NULL;

	delete viewToModel;
	viewToModel = NULL;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Event Table
//
// Description:		Event Table for the RENDER_WINDOW class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(RenderWindow, wxGLCanvas)
	// Window events
	EVT_SIZE(				RenderWindow::OnSize)
	EVT_PAINT(				RenderWindow::OnPaint)
	EVT_ERASE_BACKGROUND(	RenderWindow::OnEraseBackground)
	EVT_ENTER_WINDOW(		RenderWindow::OnEnterWindow)

	// Interaction events
	EVT_MOUSEWHEEL(			RenderWindow::OnMouseWheelEvent)
	EVT_MOTION(				RenderWindow::OnMouseMoveEvent)
	EVT_LEFT_UP(			RenderWindow::OnMouseUpEvent)
	EVT_MIDDLE_UP(			RenderWindow::OnMouseUpEvent)
	EVT_RIGHT_UP(			RenderWindow::OnMouseUpEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			RenderWindow
// Function:		Render
//
// Description:		Updates the scene with all of this object's options and
//					re-draws the image.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::Render()
{
	if (!GetContext() || !IsShownOnScreen())
		return;

	SetCurrent();

	// If modified, re-initialize
	if (modified)
		Initialize();

	// Background color
	glClearColor((float)backgroundColor.GetRed(), (float)backgroundColor.GetGreen(),
		(float)backgroundColor.GetBlue(), (float)backgroundColor.GetAlpha());

	// Clear color and depth buffers
	if (view3D)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);

	// Set the matrix mode to modelview
	glMatrixMode(GL_MODELVIEW);

	// Sort the primitives by Color.GetAlpha to ensure that transparent objects are rendered last
	SortPrimitivesByAlpha();

	// Draw all of the primitives
	int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
		primitiveList[i]->Draw();

	// Flush and swap the buffers to update the image
    glFlush();
    SwapBuffers();
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnPaint
//
// Description:		Event handler for the paint event.  Obtains the device
//					context and re-renders the scene.
//
// Input Argurments:
//		event	= wxPaintEvent& (UNUSED)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// Get the device context from this object
	wxPaintDC dc(this);

	// Render the scene
    Render();

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnSize
//
// Description:		Event handler for the window re-size event.
//
// Input Argurments:
//		event	= wxSizeEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnSize(wxSizeEvent& event)
{
    // This is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int w, h;
    GetClientSize(&w, &h);

	if (GetContext() && IsShownOnScreen())
	{
		SetCurrent();
		glViewport(0, 0, (GLint) w, (GLint) h);
	}

	// This takes care of any change in aspect ratio
	AutoSetFrustum();

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnEraseBackground
//
// Description:		Event handler for the erase background event.  This is
//					simply here to override default behaviour which would
//					cause the screen to flicker.
//
// Input Argurments:
//		event	= wxEraseEvent& (UNUSED)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing to avoid flashing
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnEnterWindow
//
// Description:		Event handler for the enter window event.
//
// Input Argurments:
//		event	= wxEraseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnEnterWindow(wxMouseEvent &event)
{
	// Bring focus to the render window
	//SetFocus();
	event.Skip();

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		RemoveActor
//
// Description:		Removes the specified actor from the display list, if it
//					is in the list.
//
// Input Argurments:
//		toRemove	= PRIMITVE* pointing to the object to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool RenderWindow::RemoveActor(Primitive *toRemove)
{
	// Make sure the arguement is valid
	if (!toRemove)
		return false;

	// Check every entry in the primtiives list to see if it matches the argument
	int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
	{
		if (toRemove == primitiveList[i])
		{
			// Found a match!  Remove it and return true
			primitiveList.Remove(i);
			return true;
		}
	}

	// If we haven't returned yet, return false
	return false;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Initialize
//
// Description:		Sets up the renderer's parameters.  Called on startup
//					and any time an option changes (wireframe vs. polygon, etc.)
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::Initialize()
{
	if (!GetContext() || !IsShownOnScreen())
		return;

	SetCurrent();

	// Set the openGL parameters depending on whether or not we're in 3D mode
	if (view3D)
	{
		// Turn Z-buffering on
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		// Z-buffer settings?  FIXME  figure out what these do
		glClearDepth(1.0);
		glDepthFunc(GL_LEQUAL);

		// Turn lighting on
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		// Smooth shading for nice-looking object
		glShadeModel(GL_SMOOTH);

		// Disable alpha blending (this is enabled as-needed when rendering objects)
		glDisable(GL_BLEND);

		// Enable anti-aliasing for polygons
		glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

		// FIXME:  Note in opengl.org samples, 9.2 Polygon Antialising
		// destination alpha is required for this algorithm to work?
	}
	else// 2D
	{
		// Disable Z-buffering, but allow testing
		//glEnable(GL_DEPTH_TEST);// FIXME:  Can't uncomment this line or the app fails to paint on any target machine (don't know why)
		glDepthMask(GL_FALSE);

		// Turn lighting off
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Displacement trick for exact pixelization
		glTranslated(0.375, 0.375, 0.0);

		// Enable the parameters required for anti-aliasing of lines
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}

	// Turn colors on
	glEnable(GL_COLOR_MATERIAL);

	// Check to see if we're drawing with polygons or wireframe
	if (wireFrame)
		// Wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		// Polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Set the viewing projection matrix
	Matrix projectionMatrix(4, 4);
	if (view3D)
	{
		double halfHeight = tan(verticalFOV) * nearClip;
		if (viewOrthogonal)
		{
			// Set up the elements for the orthogonal projection matrix (parallel projection)
			projectionMatrix.SetElement(0, 0, 1.0 / (aspectRatio * halfHeight));
			projectionMatrix.SetElement(1, 1, 1.0 / halfHeight);
			projectionMatrix.SetElement(2, 2, 2.0 / (nearClip - farClip));
			projectionMatrix.SetElement(2, 3, (nearClip + farClip) / (nearClip - farClip));
			//ProjectionMatrix.SetElement(3, 2, -1.0);// Removing this line does not give you a true orthographic projection, but it is necessary for dollying
			projectionMatrix.SetElement(3, 3, 1.0);
		}
		else
		{
			// Set up the elements for the perspective projection matrix
			projectionMatrix.SetElement(0, 0, nearClip / (aspectRatio * halfHeight));
			projectionMatrix.SetElement(1, 1, nearClip / halfHeight);
			projectionMatrix.SetElement(2, 2, (nearClip + farClip) / (nearClip - farClip));
			projectionMatrix.SetElement(2, 3, 2.0 * farClip * nearClip / (nearClip - farClip));
			projectionMatrix.SetElement(3, 2, -1.0);
		}
	}
	else
	{
		// Set up an orthogonal 2D projection matrix (this puts (0,0) at the lower left-hand corner of the window)
		projectionMatrix.SetElement(0, 0, 2.0 / GetSize().GetWidth());
		projectionMatrix.SetElement(1, 1, 2.0 / GetSize().GetHeight());
		projectionMatrix.SetElement(2, 2, -2.0);
		projectionMatrix.SetElement(0, 3, -1.0);
		projectionMatrix.SetElement(1, 3, -1.0);
		projectionMatrix.SetElement(2, 3, -1.0);
		projectionMatrix.SetElement(3, 3, 1.0);
	}

	// Assign the matrix
	glMatrixMode(GL_PROJECTION);

	// Convert from double** to double* where rows are appended to create the single vector representing the matrix
	double glMatrix[16];
	ConvertMatrixToGL(projectionMatrix, glMatrix);
	glLoadMatrixd(glMatrix);

	// Reset the modified flag
	modified = false;
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnMouseWheelEvent
//
// Description:		Event handler for the mouse wheel event.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnMouseWheelEvent(wxMouseEvent &event)
{
	// Perform a DOLLY interaction
	PerformInteraction(interactionDollyWheel, event);

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnMouseMoveEvent
//
// Description:		Event handler for the mouse move event.  Only used to
//					capture drag events for rotating, panning, or dollying
//					the scene.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnMouseMoveEvent(wxMouseEvent &event)
{
	// Don't perform and actions if this isn't a dragging event
	if (!event.Dragging())
	{
		StoreMousePosition(event);
		return;
	}

	// Determine the type of interaction to use
	InteractionType interaction;

	// Check to see if this should be using 2D or 3D interactions
	if (view3D)
	{
		// PAN:  Left mouse button + Shift OR Right mouse button
		if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
			interaction = interactionPan;

		// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
		else if ((event.LeftIsDown() && (event.CmdDown() || event.AltDown()))
			|| event.MiddleIsDown())
			interaction = interactionDollyDrag;

		// ROTATE:  Left mouse button (includes with any buttons not caught above)
		else if (event.LeftIsDown())
			interaction = interactionRotate;

		else// Not recognized
		{
			StoreMousePosition(event);
			return;
		}
	}
	else// 2D Interaction
	{
		// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
		if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
			interaction = interactionDollyDrag;

		// PAN:  Left mouse button (includes with any buttons not caught above)
		else if (event.LeftIsDown())
			interaction = interactionPan;

		else// Not recognized
		{
			StoreMousePosition(event);
			return;
		}
	}

	// Perform the interaction
	PerformInteraction(interaction, event);

	// Store the last mouse position
	StoreMousePosition(event);

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		PerformInteraction
//
// Description:		Performs the specified interaction.
//
// Input Argurments:
//		interaction	= InteractionType specifying which type of motion to create
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::PerformInteraction(InteractionType interaction, wxMouseEvent &event)
{
	// Make this the current open GL window
	SetCurrent();

	// Update the transformation matricies
	UpdateTransformationMatricies();

	// Set the matrix mode to the modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// If we haven't started interacting yet, find the focal point for this interaction
	if (!isInteracting)
	{
		// Get the new focal point
		// FIXME:  Get focal point
		//FocalPoint.Set(0.0, 0.0, 0.0);

		// Don't re-compute the focal point until the next interaction
		isInteracting = true;
	}

	// Modifying the modelview matrix moves the scene relative to
	// the eyepoint in the scene's coordinate system!!!

	// Depending on the type of interaction, perform different actions
	switch (interaction)
	{
	case interactionDollyWheel:
		DoWheelDolly(event);
		break;

	case interactionDollyDrag:
		DoDragDolly(event);
		break;

	case interactionPan:
		DoPan(event);
		break;

	case interactionRotate:
		DoRotate(event);
		break;

	default:
		break;
	}

	// Update the view
	Render();

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		StoreMousePosition
//
// Description:		Stores the current mouse position to a class member.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::StoreMousePosition(wxMouseEvent &event)
{
	// Store the current position in the last position variables
	lastMousePosition[0] = event.GetX();
	lastMousePosition[1] = event.GetY();

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnMouseUpEvent
//
// Description:		Event handler for a button becoming "unclicked."
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnMouseUpEvent(wxMouseEvent& WXUNUSED(event))
{
	// Reset the flag that indicates an interaction is in progress
	isInteracting = false;

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoRotate
//
// Description:		Performs the rotate event.  Read through comments below
//					for more information.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::DoRotate(wxMouseEvent &event)
{
	// Don't continue if we are not using 3D interactions
	if (!view3D)
		return;

	// Convert up and normal vectors from openGL coordinates to model coordinates
	VECTOR upDirection(0.0, 1.0, 0.0), normal(0.0, 0.0, 1.0), leftDirection;
	upDirection = TransformToModel(upDirection);
	normal = TransformToModel(normal);
	leftDirection = normal.Cross(upDirection);

	// Get a vector that represents the mouse position relative to the center of the screen
	VECTOR mouseVector = upDirection * double(GetSize().GetHeight() / 2 - event.GetY())
		+ leftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
	VECTOR lastMouseVector = upDirection * double(GetSize().GetHeight() / 2 - lastMousePosition[1])
		+ leftDirection * double(GetSize().GetWidth() / 2 - lastMousePosition[0]);

	// Get a vector that represents the mouse motion (projected onto a plane with the camera
	// position as a normal)
	VECTOR mouseMotion = mouseVector - lastMouseVector;

	// Find the axis of rotation
	VECTOR axisOfRotation = normal.Cross(mouseMotion);

	// Preliminary calculations
	long xDistance = GetSize().GetWidth() / 2 - event.GetX();
	long yDistance = GetSize().GetHeight() / 2 - event.GetY();
	long lastXDistance = GetSize().GetWidth() / 2 - lastMousePosition[0];
	long lastYDistance = GetSize().GetHeight() / 2 - lastMousePosition[1];

	// The angle is determined by how much the mouse moved.  1000 pixels of movement will result in
	// a full 360 degrees rotation of the car.
	// FIXME:  Add adjustable rotation sensitivity (actually, all of the interactions can be adjustable)
	double angle = sqrt(fabs(double((xDistance - lastXDistance) * (xDistance - lastXDistance))
		+ double((yDistance - lastYDistance) * (yDistance - lastYDistance)))) / 800.0 * 360.0;// [deg]

	// Translate to the focal point before performing the rotation to make the focal point
	// the center of rotation
	glTranslated(focalPoint.X, focalPoint.Y, focalPoint.Z);

	// Perform the rotation
	glRotated(angle, axisOfRotation.X, axisOfRotation.Y, axisOfRotation.Z);

	// Translate back from the focal point
	glTranslated(-focalPoint.X, -focalPoint.Y, -focalPoint.Z);

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoWheelDolly
//
// Description:		Performs a dolly event triggered by a mouse wheel roll.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::DoWheelDolly(wxMouseEvent &event)
{
	// Handle 3D dollying differently than 2D dollying
	if (view3D)
	{
		// Always dolly a constant distance
		double dollyDistance = 0.05;

		// FIXME:  Adjust the dolly distance so it is slower closer to the focal point and slower farther away

		// Get the normal direction (along which we will translate)
		VECTOR normal(0.0, 0.0, 1.0);
		normal = TransformToModel(normal);

		// Apply the dolly distance and flip the distance depending on whether we're wheeling in or out
		normal *= dollyDistance * event.GetWheelRotation();

		// Apply the translation
		glTranslated(normal.X, normal.Y, normal.Z);
	}
	else
	{
		// FIXME:  Nothing here!
	}

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoDragDolly
//
// Description:		Performs a dolly event triggered by mouse movement.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::DoDragDolly(wxMouseEvent &event)
{
	// Handle 3D dollying differntly from 2D dollying
	if (view3D)
	{
		// Always dolly a constant distance
		double dollyDistance = 0.1;

		// Convert up and normal vectors from openGL coordinates to model coordinates
		VECTOR upDirection(0.0, 1.0, 0.0), normal(0.0, 0.0, 1.0), leftDirection;
		upDirection = TransformToModel(upDirection);
		normal = TransformToModel(normal);
		leftDirection = normal.Cross(upDirection);

		// Get a vector that represents the mouse position relative to the center of the screen
		VECTOR mouseVector = upDirection * double(GetSize().GetHeight() / 2 - event.GetY())
			+ leftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
		VECTOR lastMouseVector = upDirection * double(GetSize().GetHeight() / 2 - lastMousePosition[1])
			+ leftDirection * double(GetSize().GetWidth() / 2 - lastMousePosition[0]);

		// Get a vector that represents the mouse motion (projected onto a plane with the camera
		// position as a normal)
		VECTOR mouseMotion = mouseVector - lastMouseVector;

		// Transform mouse motion to open GL coordinates
		mouseMotion = TransformToView(mouseMotion);

		// Apply the dolly distance and flip the distance depending on whether we're wheeling in or out
		normal *= dollyDistance * mouseMotion.Y;

		// Apply the translation
		glTranslated(normal.X, normal.Y, normal.Z);
	}
	else
	{
		// FIXME:  Nothing here!
	}

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoPan
//
// Description:		Performs a pan event.
//
// Input Argurments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::DoPan(wxMouseEvent &event)
{
	// Handle 3D panning differently from 2D panning
	if (view3D)
	{
		// Convert up and normal vectors from openGL coordinates to model coordinates
		VECTOR upDirection(0.0, 1.0, 0.0), normal(0.0, 0.0, 1.0), leftDirection;
		upDirection = TransformToModel(upDirection);
		normal = TransformToModel(normal);
		leftDirection = normal.Cross(upDirection);

		// Get a vector that represents the mouse position relative to the center of the screen
		VECTOR mouseVector = upDirection * double(GetSize().GetHeight() / 2 - event.GetY())
			+ leftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
		VECTOR lastMouseVector = upDirection * double(GetSize().GetHeight() / 2 - lastMousePosition[1])
			+ leftDirection * double(GetSize().GetWidth() / 2 - lastMousePosition[0]);

		// Get a vector that represents the mouse motion (projected onto a plane with the camera
		// position as a normal)
		VECTOR mouseMotion = mouseVector - lastMouseVector;

		// Determine and apply the motion factor
		double motionFactor = 0.15;
		mouseMotion *= motionFactor;

		// Apply the translation
		glTranslated(mouseMotion.X, mouseMotion.Y, mouseMotion.Z);

		// Update the focal point
		focalPoint -= mouseMotion;
	}
	else
	{
		// FIXME:  Nothing here!
	}

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		SetCameraView
//
// Description:		Sets the camera view as specified.
//
// Input Argurments:
//		position	= const VECTOR& specifying the camera position
//		lookAt		= const VECTOR& specifying the object at which the camera
//					  is to be pointed
//		upDirection	= const VECTOR& used to specify the final camera orientation DOF
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::SetCameraView(const VECTOR &position, const VECTOR &lookAt, const VECTOR &upDirection)
{
	// Make this the current open GL window, but only if the parent window is visible
	SetCurrent();

	// Set the matrix mode to the modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// Load an identity matrix
	glLoadIdentity();

	// Compute the MODELVIEW matrix
	// (Use calculations from gluLookAt documentation)
	VECTOR F = (lookAt - position).Normalize();
	VECTOR up = upDirection.Normalize();
	VECTOR S = F.Cross(up);
	if (!PlotMath::IsZero(S))
	{
		VECTOR U = S.Cross(F);
		Matrix modelViewMatrix(4, 4, S.X, S.Y, S.Z, 0.0,
									 U.X, U.Y, U.Z, 0.0,
									 -F.X, -F.Y, -F.Z, 0.0,
									 0.0, 0.0, 0.0, 1.0);

		// Assign it to the open GL modelview matrix
		double glMatrix[16];
		ConvertMatrixToGL(modelViewMatrix, glMatrix);
		glLoadMatrixd(glMatrix);
	}

	// Apply the translation
	glTranslated(-position.X, -position.Y, -position.Z);

	// Set the focal point
	focalPoint = lookAt;

	UpdateTransformationMatricies();

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		TransformToView
//
// Description:		Returns a vector equivalent to the specified vector
//					(assumed to be in model coordinates) in view coordinates.
//
// Input Argurments:
//		ModelVector	= const VECTOR& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
VECTOR RenderWindow::TransformToView(const VECTOR &modelVector) const
{
	return (*modelToView) * modelVector;
}

//==========================================================================
// Class:			RenderWindow
// Function:		TransformToModel
//
// Description:		Returns a vector equivalent to the specified vector
//					(assumed to be in view coordinates) in model coordinates.
//
// Input Argurments:
//		viewVector	= const VECTOR& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
VECTOR RenderWindow::TransformToModel(const VECTOR &viewVector) const
{
	return (*viewToModel) * viewVector;
}

//==========================================================================
// Class:			RenderWindow
// Function:		UpdateTransformationMatricies
//
// Description:		Updates the matricies for transforming from model coordinates
//					to view coordinates and vice-versa.  Also updates the camera
//					position variable.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::UpdateTransformationMatricies(void)
{
	Matrix modelViewMatrix(4, 4);
	double glMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, glMatrix);
	ConvertGLToMatrix(modelViewMatrix, glMatrix);

	// Extract the orientation matricies
	(*modelToView) = modelViewMatrix.GetSubMatrix(0, 0, 3, 3);
	(*viewToModel) = (*modelToView);
	viewToModel->GetTranspose();

	// Get the last column of the modelview matrix, which contains the translation information
	cameraPosition.X = modelViewMatrix.GetElement(0, 3);
	cameraPosition.Y = modelViewMatrix.GetElement(1, 3);
	cameraPosition.Z = modelViewMatrix.GetElement(2, 3);

	// Transfrom the camera position into model coordinates
	cameraPosition = TransformToModel(cameraPosition);

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		AutoSetFrustum
//
// Description:		Updates the view frustum to correctly match the viewport size.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::AutoSetFrustum(void)
{
	// This method is really for 3D renderers - for 2D, we just re-initialize to handle change in aspect ratio/size
	if (!view3D)
	{
		modified = true;
		return;
	}

	// Get this window's size
	wxSize WindowSize = GetSize();

	// Set the aspect ratio to match this window's size
	aspectRatio = (double)WindowSize.GetWidth() / (double)WindowSize.GetHeight();

	// Set the vertical FOV
	verticalFOV = 20.0 * PlotMath::PI / 180.0;

	// Set the clipping plane distances to something reasonable
	// FIXME:  Should this be smarter (distance between camera and focal point?)
	nearClip = 5.0;
	farClip = 500.0;

	// Tell it that we're modified
	modified = true;

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		GetGLError
//
// Description:		Returns a string describing any openGL errors.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString contiainin the error description
//
//==========================================================================
wxString RenderWindow::GetGLError(void) const
{
	wxString ErrorString;
	int Error = glGetError();

	switch (Error)
	{
	case GL_NO_ERROR:
		ErrorString.assign(_T("No errors"));
		break;

	case GL_INVALID_ENUM:
		ErrorString.assign(_T("Invalid enumeration"));
		break;

	case GL_INVALID_VALUE:
		ErrorString.assign(_T("Invalid value"));
		break;

	case GL_INVALID_OPERATION:
		ErrorString.assign(_T("Invalid operation"));
		break;

	case GL_STACK_OVERFLOW:
		ErrorString.assign(_T("Stack overflow"));
		break;

	case GL_STACK_UNDERFLOW:
		ErrorString.assign(_T("Stack underflow"));
		break;

	case GL_OUT_OF_MEMORY:
		ErrorString.assign(_T("Out of memory"));
		break;

	default:
		ErrorString.assign(_T("Unrecognized error"));
		break;
	}

	return ErrorString;
}

//==========================================================================
// Class:			RenderWindow
// Function:		WriteImageToFile
//
// Description:		Writes the contents of the render window to file.  Various
//					different file types are supported, specified by the file
//					extension.
//
// Input Argurments:
//		pathAndFileName	= wxString specifying the location to save the image to
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating success (true) or failure (false)
//
//==========================================================================
bool RenderWindow::WriteImageToFile(wxString pathAndFileName) const
{
	unsigned int height = GetSize().GetHeight();
	unsigned int width = GetSize().GetWidth();

	// Create a buffer in which we will store the raw image data
	GLubyte *imageBuffer = (GLubyte*)malloc(width * height * sizeof(GLubyte) * 3);

	// Tell OpenGL to tight-pack the data - don't pad the bytes at the end of a row
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// Read the image into the buffer
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageBuffer);

	// Move the image data from a raw buffer into a wxImage
	// We set the static data flag to true, so we're still responsible for deleting the buffer
	// Not sure why we need to do it this way, but it crashes every time if we don't
	wxImage newImage(width, height, imageBuffer, true);

	// If written as-is, the image will be upside down, so we flip it
	newImage = newImage.Mirror(false);

	// After the mirror, we can free the original buffer
	free(imageBuffer);

	// Set up the image handlers for all of the image types we anticipate
	wxInitAllImageHandlers();

	// Save the image to file
	return newImage.SaveFile(pathAndFileName);
}

//==========================================================================
// Class:			RenderWindow
// Function:		IsThisRendererSelected
//
// Description:		Writes the contents of the render window to file.  Various
//					different file types are supported, specified by the file
//					extension.
//
// Input Argurments:
//		pickedObject	= const PRIMITIVE* pointing to the selected primitive
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the selected primitive is
//		part of this object's scene
//
//==========================================================================
bool RenderWindow::IsThisRendererSelected(const Primitive *pickedObject) const
{
	// Iterate through the list of primitives in the scene
	// If one of them has the same address as our argurment, return true
	int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
	{
		if (primitiveList[i] == pickedObject)
			return true;
	}

	return false;
}

//==========================================================================
// Class:			RenderWindow
// Function:		SortPrimitivesByAlpha
//
// Description:		Sorts the PrimitiveList by Color.Alpha to ensure that
//					opaque objects are rendered prior to transparent objects.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::SortPrimitivesByAlpha(void)
{
	// Check to see if re-ordering is necessary
	int i;
	std::vector< ListItem > primitiveOrder;
	for (i = 0; i < primitiveList.GetCount(); i++)
		primitiveOrder.insert(primitiveOrder.end(), ListItem(primitiveList[i]->GetColor().GetAlpha(), i));

	// Do the sorting with the standard libraries method
	std::stable_sort(primitiveOrder.begin(), primitiveOrder.end());

	// Convert from the list to an array
	int *order = new int[primitiveList.GetCount()];
	for (i = 0; i < primitiveList.GetCount(); i++)
		order[i] = primitiveOrder[i].i;

	// Re-order the list
	primitiveList.ReorderObjects(order);

	// Clean up memory
	delete [] order;
	order = NULL;

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		ConvertMatrixToGL
//
// Description:		Converts from Matrix type to a row-appended vector that
//					represents the matrix.  Converts to array as required by
//					OpenGL.
//
// Input Argurments:
//		matrix	= const Matrix& containing the original data
//
// Output Arguments:
//		gl		= double[] in the form expected by OpenGL
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::ConvertMatrixToGL(const Matrix& matrix, double gl[])
{
	unsigned int i, j;
	for (i = 0; i < matrix.GetNumberOfRows(); i++)
	{
		for (j = 0; j < matrix.GetNumberOfColumns(); j++)
			gl[i * matrix.GetNumberOfColumns() + j] = matrix(j, i);
	}

	return;
}

//==========================================================================
// Class:			RenderWindow
// Function:		ConvertGLToMatrix
//
// Description:		Converts from OpenGL array to Matrix type.  Size of matrix
//					must be set before this call.
//
// Input Argurments:
//		gl		= double[] in the form expected by OpenGL
//
// Output Arguments:
//		matrix	= const Matrix& containing the original data
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::ConvertGLToMatrix(Matrix& matrix, const double gl[])
{
	unsigned int i, j;
	for (i = 0; i < matrix.GetNumberOfRows(); i++)
	{
		for (j = 0; j < matrix.GetNumberOfColumns(); j++)
			matrix(j, i) = gl[i * matrix.GetNumberOfColumns() + j];
	}

	return;
}