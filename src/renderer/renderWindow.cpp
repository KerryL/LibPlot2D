/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2013

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  renderWindow.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:
//	4/25/2010	- Fixed anti-aliasing for 2D plots, K. Loux.

// Standard C++ headers
#include <vector>
#include <algorithm>

// wxWidgets headers
#include <wx/dcclient.h>
#include <wx/image.h>

// Local headers
#include "renderer/renderWindow.h"
#include "utilities/math/matrix.h"
#include "utilities/math/vector.h"
#include "utilities/math/plotMath.h"

//==========================================================================
// Class:			RenderWindow
// Function:		Constant declarations
//
// Description:		Constant declarations for RenderWindow class.
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
const double RenderWindow::exactPixelShift(0.375);

//==========================================================================
// Class:			RenderWindow
// Function:		RenderWindow
//
// Description:		Constructor for RenderWindow class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		parent		= wxWindow& reference to the owner of this object
//		id			= wxWindowID to identify this window
//		args		= int[] NOTE: Under GTK, must contain WX_GL_DOUBLEBUFFER at minimum
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
RenderWindow::RenderWindow(wxWindow &parent, wxWindowID id, int args[],
    const wxPoint& position, const wxSize& size, long style) : wxGLCanvas(
	&parent, id, args, position, size, style | wxFULL_REPAINT_ON_RESIZE)
{
	context = NULL;

	wireFrame = false;
	view3D = true;
	viewOrthogonal = false;

	AutoSetFrustum();

	modelToView = new Matrix(3, 3);
	modelToView->MakeIdentity();

	viewToModel = new Matrix(3, 3);
	viewToModel->MakeIdentity();

	cameraPosition.Set(0.0, 0.0, 0.0);
	focalPoint.Set(0.0, 0.0, 0.0);
	isInteracting = false;

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);// To avoid flashing under MSW

	modified = true;
}

//==========================================================================
// Class:			RenderWindow
// Function:		~RenderWindow
//
// Description:		Destructor for RenderWindow class.  Deletes objects in
//					the scene and other dynamic variables.
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
RenderWindow::~RenderWindow()
{
	primitiveList.Clear();

	delete modelToView;
	modelToView = NULL;

	delete viewToModel;
	viewToModel = NULL;

	delete GetContext();
	context = NULL;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Event Table
//
// Description:		Event Table for the RENDER_WINDOW class.
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
BEGIN_EVENT_TABLE(RenderWindow, wxGLCanvas)
	// Window events
	EVT_SIZE(				RenderWindow::OnSize)
	EVT_PAINT(				RenderWindow::OnPaint)
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
// Function:		GetContext
//
// Description:		Gets (or creates, if it doesn't yet exist) the GL context.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxGLContext*
//
//==========================================================================
wxGLContext* RenderWindow::GetContext(void)
{
	if (!context)
		context = new wxGLContext(this);

	return context;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Render
//
// Description:		Updates the scene with all of this object's options and
//					re-draws the image.
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
void RenderWindow::Render()
{
	if (!GetContext() || !IsShownOnScreen())
		return;

	SetCurrent(*context);
	wxPaintDC(this);

	if (modified)
		Initialize();

	glClearColor((float)backgroundColor.GetRed(), (float)backgroundColor.GetGreen(),
		(float)backgroundColor.GetBlue(), (float)backgroundColor.GetAlpha());

	if (view3D)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// Sort the primitives by Color.GetAlpha to ensure that transparent objects are rendered last
	if (!view3D)// For 3D views, we need to render back-to-front
		SortPrimitivesByAlpha();

	SortPrimitivesByDrawOrder();

	unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
		primitiveList[i]->Draw();

	SwapBuffers();
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnPaint
//
// Description:		Event handler for the paint event.  Obtains the device
//					context and re-renders the scene.
//
// Input Arguments:
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
    Render();
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnSize
//
// Description:		Event handler for the window re-size event.
//
// Input Arguments:
//		event	= wxSizeEvent& (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
	int w, h;
	GetClientSize(&w, &h);

	if (GetContext() && IsShownOnScreen())
	{
		SetCurrent(*context);
		glViewport(0, 0, (GLint) w, (GLint) h);
	}
	Refresh();

	// This takes care of any change in aspect ratio
	AutoSetFrustum();
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnEnterWindow
//
// Description:		Event handler for the enter window event.
//
// Input Arguments:
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
	//SetFocus();
	event.Skip();
}

//==========================================================================
// Class:			RenderWindow
// Function:		RemoveActor
//
// Description:		Removes the specified actor from the display list, if it
//					is in the list.
//
// Input Arguments:
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
	if (!toRemove)
		return false;

	unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
	{
		if (toRemove == primitiveList[i])
		{
			primitiveList.Remove(i);
			return true;
		}
	}

	return false;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Initialize
//
// Description:		Sets up the renderer's parameters.  Called on startup
//					and any time an option changes (wireframe vs. polygon, etc.)
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
void RenderWindow::Initialize()
{
	Matrix projectionMatrix;
	if (view3D)
	{
		Initialize3D();
		projectionMatrix = Generate3DProjectionMatrix();
	}
	else
	{
		Initialize2D();
		projectionMatrix = Generate2DProjectionMatrix();
	}

	glEnable(GL_COLOR_MATERIAL);

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glMatrixMode(GL_PROJECTION);

	// Convert from double** to double* where rows are appended to create the single vector representing the matrix
	double glMatrix[16];
	ConvertMatrixToGL(projectionMatrix, glMatrix);
	glLoadMatrixd(glMatrix);

	modified = false;
}

//==========================================================================
// Class:			RenderWindow
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
void RenderWindow::OnMouseWheelEvent(wxMouseEvent &event)
{
	PerformInteraction(InteractionDollyWheel, event);
}

//==========================================================================
// Class:			RenderWindow
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
void RenderWindow::OnMouseMoveEvent(wxMouseEvent &event)
{
	if (!event.Dragging())
	{
		StoreMousePosition(event);
		return;
	}

	InteractionType interaction;
	if (view3D)
	{
		if (!Determine3DInteraction(event, interaction))
		{
			StoreMousePosition(event);
			return;
		}
	}
	else
	{
		if (!Determine2DInteraction(event, interaction))
		{
			StoreMousePosition(event);
			return;
		}
	}

	PerformInteraction(interaction, event);
	StoreMousePosition(event);
}

//==========================================================================
// Class:			RenderWindow
// Function:		PerformInteraction
//
// Description:		Performs the specified interaction.
//					NOTE:  Modifying the modelview matrix moves the scene
//					relative to the eyepoint in the scene's coordinate system!
//
// Input Arguments:
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
	SetCurrent(*GetContext());
	UpdateTransformationMatricies();
	glMatrixMode(GL_MODELVIEW);

	if (!isInteracting)
	{
		// TODO:  Get focal point in order to perform interactions around the cursor
		//FocalPoint.Set(0.0, 0.0, 0.0);

		// Don't re-compute the focal point until the next interaction
		isInteracting = true;
	}

	if (interaction == InteractionDollyWheel)
		DoWheelDolly(event);
	else if (interaction == InteractionDollyDrag)
		DoDragDolly(event);
	else if (interaction == InteractionPan)
		DoPan(event);
	else if (interaction == InteractionRotate)
		DoRotate(event);

	Refresh();
}

//==========================================================================
// Class:			RenderWindow
// Function:		StoreMousePosition
//
// Description:		Stores the current mouse position to a class member.
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
void RenderWindow::StoreMousePosition(wxMouseEvent &event)
{
	// Store the current position in the last position variables
	lastMousePosition[0] = event.GetX();
	lastMousePosition[1] = event.GetY();
}

//==========================================================================
// Class:			RenderWindow
// Function:		OnMouseUpEvent
//
// Description:		Event handler for a button becoming "unclicked."
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
void RenderWindow::OnMouseUpEvent(wxMouseEvent& WXUNUSED(event))
{
	// Reset the flag that indicates an interaction is in progress
	isInteracting = false;
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoRotate
//
// Description:		Performs the rotate event.  Read through comments below
//					for more information.
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
void RenderWindow::DoRotate(wxMouseEvent &event)
{
	if (!view3D)
		return;

	Vector upDirection(0.0, 1.0, 0.0), normal(0.0, 0.0, 1.0), leftDirection;
	upDirection = TransformToModel(upDirection);
	normal = TransformToModel(normal);
	leftDirection = normal.Cross(upDirection);

	Vector mouseVector = upDirection * double(GetSize().GetHeight() / 2 - event.GetY())
		+ leftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
	Vector lastMouseVector = upDirection * double(GetSize().GetHeight() / 2 - lastMousePosition[1])
		+ leftDirection * double(GetSize().GetWidth() / 2 - lastMousePosition[0]);

	// Get a vector that represents the mouse motion (projected onto a plane with the camera
	// position as a normal)
	Vector mouseMotion = mouseVector - lastMouseVector;
	Vector axisOfRotation = normal.Cross(mouseMotion);

	long xDistance = GetSize().GetWidth() / 2 - event.GetX();
	long yDistance = GetSize().GetHeight() / 2 - event.GetY();
	long lastXDistance = GetSize().GetWidth() / 2 - lastMousePosition[0];
	long lastYDistance = GetSize().GetHeight() / 2 - lastMousePosition[1];

	// The angle is determined by how much the mouse moved.  800 pixels of movement will result in
	// a full 360 degrees rotation.
	// TODO:  Add user-adjustable rotation sensitivity (actually, all of the interactions can be adjustable)
	double angle = sqrt(fabs(double((xDistance - lastXDistance) * (xDistance - lastXDistance))
		+ double((yDistance - lastYDistance) * (yDistance - lastYDistance)))) / 800.0 * 360.0;// [deg]

	glTranslated(focalPoint.x, focalPoint.y, focalPoint.z);
	glRotated(angle, axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);
	glTranslated(-focalPoint.x, -focalPoint.y, -focalPoint.z);
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoWheelDolly
//
// Description:		Performs a dolly event triggered by a mouse wheel roll.
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
void RenderWindow::DoWheelDolly(wxMouseEvent &event)
{
	// Handle 3D dollying differently than 2D dollying
	if (view3D)
	{
		// Always dolly a constant distance
		double dollyDistance = 0.05;

		// TODO:  Adjust the dolly distance so it is slower closer to the focal point and slower farther away

		// Get the normal direction (along which we will translate)
		Vector normal(0.0, 0.0, 1.0);
		normal = TransformToModel(normal);

		// Apply the dolly distance and flip the distance depending on whether we're wheeling in or out
		normal *= dollyDistance * event.GetWheelRotation();

		// Apply the translation
		glTranslated(normal.x, normal.y, normal.z);
	}
	else
	{
		// Nothing here!
	}
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoDragDolly
//
// Description:		Performs a dolly event triggered by mouse movement.
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
void RenderWindow::DoDragDolly(wxMouseEvent &event)
{
	if (view3D)
	{
		double dollyDistance = 0.1;

		// Convert up and normal vectors from openGL coordinates to model coordinates
		Vector upDirection(0.0, 1.0, 0.0), normal(0.0, 0.0, 1.0), leftDirection;
		upDirection = TransformToModel(upDirection);
		normal = TransformToModel(normal);
		leftDirection = normal.Cross(upDirection);

		// Get a vector that represents the mouse position relative to the center of the screen
		Vector mouseVector = upDirection * double(GetSize().GetHeight() / 2 - event.GetY())
			+ leftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
		Vector lastMouseVector = upDirection * double(GetSize().GetHeight() / 2 - lastMousePosition[1])
			+ leftDirection * double(GetSize().GetWidth() / 2 - lastMousePosition[0]);

		// Get a vector that represents the mouse motion (projected onto a plane with the camera
		// position as a normal)
		Vector mouseMotion = mouseVector - lastMouseVector;

		mouseMotion = TransformToView(mouseMotion);
		normal *= dollyDistance * mouseMotion.y;
		glTranslated(normal.x, normal.y, normal.z);
	}
	else
	{
		// Nothing here!
	}
}

//==========================================================================
// Class:			RenderWindow
// Function:		DoPan
//
// Description:		Performs a pan event.
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
void RenderWindow::DoPan(wxMouseEvent &event)
{
	// Handle 3D panning differently from 2D panning
	if (view3D)
	{
		// Convert up and normal vectors from openGL coordinates to model coordinates
		Vector upDirection(0.0, 1.0, 0.0), normal(0.0, 0.0, 1.0), leftDirection;
		upDirection = TransformToModel(upDirection);
		normal = TransformToModel(normal);
		leftDirection = normal.Cross(upDirection);

		// Get a vector that represents the mouse position relative to the center of the screen
		Vector mouseVector = upDirection * double(GetSize().GetHeight() / 2 - event.GetY())
			+ leftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
		Vector lastMouseVector = upDirection * double(GetSize().GetHeight() / 2 - lastMousePosition[1])
			+ leftDirection * double(GetSize().GetWidth() / 2 - lastMousePosition[0]);

		// Get a vector that represents the mouse motion (projected onto a plane with the camera
		// position as a normal)
		Vector mouseMotion = mouseVector - lastMouseVector;

		// Determine and apply the motion factor
		double motionFactor = 0.15;
		mouseMotion *= motionFactor;

		// Apply the translation
		glTranslated(mouseMotion.x, mouseMotion.y, mouseMotion.z);

		// Update the focal point
		focalPoint -= mouseMotion;
	}
	else
	{
		// Nothing here!
	}
}

//==========================================================================
// Class:			RenderWindow
// Function:		SetCameraView
//
// Description:		Sets the camera view as specified.
//
// Input Arguments:
//		position	= const Vector& specifying the camera position
//		lookAt		= const Vector& specifying the object at which the camera
//					  is to be pointed
//		upDirection	= const Vector& used to specify the final camera orientation DOF
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RenderWindow::SetCameraView(const Vector &position, const Vector &lookAt, const Vector &upDirection)
{
	SetCurrent(*GetContext());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Compute the MODELVIEW matrix
	// (Use calculations from gluLookAt documentation)
	Vector f = (lookAt - position).Normalize();
	Vector up = upDirection.Normalize();
	Vector s = f.Cross(up);
	if (!PlotMath::IsZero(s))
	{
		Vector u = s.Cross(f);
		Matrix modelViewMatrix(4, 4, s.x, s.y, s.z, 0.0,
									 u.x, u.y, u.z, 0.0,
									 -f.x, -f.y, -f.z, 0.0,
									 0.0, 0.0, 0.0, 1.0);

		double glMatrix[16];
		ConvertMatrixToGL(modelViewMatrix, glMatrix);
		glLoadMatrixd(glMatrix);
	}

	glTranslated(-position.x, -position.y, -position.z);
	focalPoint = lookAt;

	UpdateTransformationMatricies();
}

//==========================================================================
// Class:			RenderWindow
// Function:		TransformToView
//
// Description:		Returns a vector equivalent to the specified vector
//					(assumed to be in model coordinates) in view coordinates.
//
// Input Arguments:
//		modelVector	= const Vector& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector
//
//==========================================================================
Vector RenderWindow::TransformToView(const Vector &modelVector) const
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
// Input Arguments:
//		viewVector	= const Vector& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		Vector
//
//==========================================================================
Vector RenderWindow::TransformToModel(const Vector &viewVector) const
{
	return (*viewToModel) * viewVector;
}

//==========================================================================
// Class:			RenderWindow
// Function:		UpdateTransformationMatricies
//
// Description:		Updates the matrices for transforming from model coordinates
//					to view coordinates and vice-versa.  Also updates the camera
//					position variable.
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
void RenderWindow::UpdateTransformationMatricies(void)
{
	Matrix modelViewMatrix(4, 4);
	double glMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, glMatrix);
	ConvertGLToMatrix(modelViewMatrix, glMatrix);

	// Extract the orientation matrices
	(*modelToView) = modelViewMatrix.GetSubMatrix(0, 0, 3, 3);
	(*viewToModel) = (*modelToView);
	*viewToModel = viewToModel->GetTranspose();

	// Get the last column of the modelview matrix, which contains the translation information
	cameraPosition.x = modelViewMatrix.GetElement(0, 3);
	cameraPosition.y = modelViewMatrix.GetElement(1, 3);
	cameraPosition.z = modelViewMatrix.GetElement(2, 3);

	// Transform the camera position into model coordinates
	cameraPosition = TransformToModel(cameraPosition);
}

//==========================================================================
// Class:			RenderWindow
// Function:		AutoSetFrustum
//
// Description:		Updates the view frustum to correctly match the viewport size.
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
	verticalFOV = 20.0 * PlotMath::pi / 180.0;

	// Set the clipping plane distances to something reasonable
	// TODO:  Make this be smarter, or user-adjustable (distance between camera and focal point)
	nearClip = 5.0;
	farClip = 500.0;

	// Tell it that we're modified
	modified = true;
}

//==========================================================================
// Class:			RenderWindow
// Function:		GetGLError
//
// Description:		Returns a string describing any openGL errors.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the error description
//
//==========================================================================
wxString RenderWindow::GetGLError(void) const
{
	int error = glGetError();

	if (error == GL_NO_ERROR)
		return _T("No errors");
	else if (error == GL_INVALID_ENUM)
		return _T("Invalid enumeration");
	else if (error == GL_INVALID_VALUE)
		return _T("Invalid value");
	else if (error == GL_INVALID_OPERATION)
		return _T("Invalid operation");
	else if (error == GL_STACK_OVERFLOW)
		return _T("Stack overflow");
	else if (error == GL_STACK_UNDERFLOW)
		return _T("Stack underflow");
	else if (error == GL_OUT_OF_MEMORY)
		return _T("Out of memory");

	return _T("Unrecognized error");
}

//==========================================================================
// Class:			RenderWindow
// Function:		WriteImageToFile
//
// Description:		Writes the contents of the render window to file.  Various
//					different file types are supported, specified by the file
//					extension.
//
// Input Arguments:
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
// Function:		GetImage
//
// Description:		Returns an image object representing the contents of the
//					window.
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
wxImage RenderWindow::GetImage(void) const
{
	unsigned int height = GetSize().GetHeight();
	unsigned int width = GetSize().GetWidth();

	GLubyte *imageBuffer = (GLubyte*)malloc(width * height * sizeof(GLubyte) * 3);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageBuffer);

	wxImage newImage(width, height, imageBuffer, true);
	newImage = newImage.Mirror(false);

	free(imageBuffer);

	return newImage;
}

//==========================================================================
// Class:			RenderWindow
// Function:		IsThisRendererSelected
//
// Description:		Writes the contents of the render window to file.  Various
//					different file types are supported, specified by the file
//					extension.
//
// Input Arguments:
//		pickedObject	= const Primitive* pointing to the selected primitive
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
	// If one of them has the same address as our argument, return true
	unsigned int i;
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
void RenderWindow::SortPrimitivesByAlpha(void)
{
	unsigned int i;
	std::vector<ListItem> primitiveOrder;
	for (i = 0; i < primitiveList.GetCount(); i++)
		primitiveOrder.push_back(ListItem(primitiveList[i]->GetColor().GetAlpha(), i));

	std::stable_sort(primitiveOrder.begin(), primitiveOrder.end());

	std::vector<unsigned int> order;
	for (i = 0; i < primitiveOrder.size(); i++)
		order.push_back(primitiveOrder[i].i);

	primitiveList.ReorderObjects(order);
}

//==========================================================================
// Class:			RenderWindow
// Function:		SortPrimitivesByDrawOrder
//
// Description:		Sorts the PrimitiveList by draw order.
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
void RenderWindow::SortPrimitivesByDrawOrder()
{
	unsigned int i;
	std::vector<ListItem> primitiveOrder;
	for (i = 0; i < primitiveList.GetCount(); i++)
		primitiveOrder.push_back(ListItem(primitiveList[i]->GetDrawOrder(), i));

	std::stable_sort(primitiveOrder.begin(), primitiveOrder.end());

	std::vector<unsigned int> order;
	for (i = 0; i < primitiveOrder.size(); i++)
		order.push_back(primitiveOrder[i].i);

	primitiveList.ReorderObjects(order);
}

//==========================================================================
// Class:			RenderWindow
// Function:		ConvertMatrixToGL
//
// Description:		Converts from Matrix type to a row-appended vector that
//					represents the matrix.  Converts to array as required by
//					OpenGL.
//
// Input Arguments:
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
}

//==========================================================================
// Class:			RenderWindow
// Function:		ConvertGLToMatrix
//
// Description:		Converts from OpenGL array to Matrix type.  Size of matrix
//					must be set before this call.
//
// Input Arguments:
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
}

//==========================================================================
// Class:			RenderWindow
// Function:		Initialize2D
//
// Description:		Configures openGL for drawing 2D scenes.
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
void RenderWindow::Initialize2D(void) const
{
	// Disable Z-buffering, but allow testing
	//glEnable(GL_DEPTH_TEST);// NOTE:  Can't uncomment this line or the app fails to paint on any target machine (don't know why)
	glDepthMask(GL_FALSE);

	// Turn lighting off
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	ShiftForExactPixelization();

	// Enable the parameters required for anti-aliasing of lines
	glDisable(GL_MULTISAMPLE);// Supposedly degrades quality of anti-aliasing
	/*glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);*/
	/*int a;
	glGetIntegerv(GL_SAMPLE_BUFFERS, &a);
	wxString s;
	s.Printf("%i", a);
	wxMessageBox(s);
	glGetIntegerv(GL_SAMPLES, &a);
	s.Printf("%i", a);
	wxMessageBox(s);//*/
	//GL_SAMPLE_BUFFERS
	//GL_SAMPLES
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	/*glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// TODO:  Something isn't right under GTK - MSW looks nice bt GTK does not
}

//==========================================================================
// Class:			RenderWindow
// Function:		Initialize3D
//
// Description:		Configures openGL for drawing 3D scenes.
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
void RenderWindow::Initialize3D(void) const
{
	// Turn Z-buffering on
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Z-buffer settings
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);

	// Turn lighting on
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Smooth shading for nice-looking object
	glShadeModel(GL_SMOOTH);

	// Don't disable this:  required for anti-aliasing
	// Disable alpha blending (this is enabled as-needed when rendering objects)
	//glDisable(GL_BLEND);

	// Enable anti-aliasing for polygons
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//==========================================================================
// Class:			RenderWindow
// Function:		Generate2DProjectionMatrix
//
// Description:		Returns projection matrix for 2D scenes.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix RenderWindow::Generate2DProjectionMatrix(void) const
{
	// Set up an orthogonal 2D projection matrix (this puts (0,0) at the lower left-hand corner of the window)
	Matrix projectionMatrix(4, 4);
	projectionMatrix.SetElement(0, 0, 2.0 / GetSize().GetWidth());
	projectionMatrix.SetElement(1, 1, 2.0 / GetSize().GetHeight());
	projectionMatrix.SetElement(2, 2, -2.0);
	projectionMatrix.SetElement(0, 3, -1.0);
	projectionMatrix.SetElement(1, 3, -1.0);
	projectionMatrix.SetElement(2, 3, -1.0);
	projectionMatrix.SetElement(3, 3, 1.0);

	return projectionMatrix;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Generate3DProjectionMatrix
//
// Description:		Returns projection matrix for 3D scenes.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		Matrix
//
//==========================================================================
Matrix RenderWindow::Generate3DProjectionMatrix(void) const
{
	Matrix projectionMatrix(4, 4);
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

	return projectionMatrix;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Determine2DInteraction
//
// Description:		Determines type of 2D interaction (if any).
//
// Input Arguments:
//		event		= wxMouseEvent&
//
// Output Arguments:
//		interaction	= InteractionType&
//
// Return Value:
//		bool
//
//==========================================================================
bool RenderWindow::Determine2DInteraction(const wxMouseEvent &event, InteractionType &interaction) const
{
	// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
	if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
		interaction = InteractionDollyDrag;

	// PAN:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
		interaction = InteractionPan;

	else
		return false;

	return true;
}

//==========================================================================
// Class:			RenderWindow
// Function:		Determine3DInteraction
//
// Description:		Determines type of 3D interaction (if any).
//
// Input Arguments:
//		event		= wxMouseEvent&
//
// Output Arguments:
//		interaction	= InteractionType&
//
// Return Value:
//		bool
//
//==========================================================================
bool RenderWindow::Determine3DInteraction(const wxMouseEvent &event, InteractionType &interaction) const
{
	// PAN:  Left mouse button + Shift OR Right mouse button
	if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
		interaction = InteractionPan;

	// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
	else if ((event.LeftIsDown() && (event.CmdDown() || event.AltDown()))
		|| event.MiddleIsDown())
		interaction = InteractionDollyDrag;

	// ROTATE:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
		interaction = InteractionRotate;

	else
		return false;

	return true;
}

//==========================================================================
// Class:			RenderWindow
// Function:		ShiftForExactPixelization
//
// Description:		Applies shift trick to enabled exact pixelization.
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
void RenderWindow::ShiftForExactPixelization() const
{
	glTranslated(exactPixelShift, exactPixelShift, 0.0);
}