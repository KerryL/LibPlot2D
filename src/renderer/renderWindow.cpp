/*=============================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

=============================================================================*/

// File:  renderWindow.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Desc:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//        event handlers for various mouse and keyboard interactions.  All
//        object in the scene must be added to the PrimitivesList in order to
//        be drawn.  Objects in the PrimitivesList become managed by this
//        object and are deleted automatically.

// Standard C++ headers
#include <vector>
#include <algorithm>
#include <iostream>

// wxWidgets headers
#include <wx/dcclient.h>
#include <wx/image.h>

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/utilities/math/plotMath.h"

namespace LibPlot2D
{

//=============================================================================
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
//=============================================================================
const std::string RenderWindow::modelviewName("modelviewMatrix");
const std::string RenderWindow::projectionName("projectionMatrix");
const std::string RenderWindow::positionName("position");
const std::string RenderWindow::colorName("color");

const double RenderWindow::exactPixelShift(0.375);

//=============================================================================
// Class:			RenderWindow
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
//=============================================================================
const std::string RenderWindow::defaultVertexShader(
	"#version 300 es\n"
	"\n"
	"uniform mat4 modelviewMatrix;\n"
	"uniform mat4 projectionMatrix;\n"
	"\n"
	"layout(location = 0) in highp vec4 position;\n"
	"layout(location = 1) in highp vec4 color;\n"
	"\n"
	"out highp vec4 vertexColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vertexColor = color;\n"
	"    gl_Position = projectionMatrix * modelviewMatrix * position;\n"
	"}\n"
);

//=============================================================================
// Class:			RenderWindow
// Function:		defaultFragmentShader
//
// Description:		Default fragment shader.
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
const std::string RenderWindow::defaultFragmentShader(
	"#version 300 es\n"
	"\n"
	"in highp vec4 vertexColor;\n"
	"\n"
	"out highp vec4 outputColor;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    outputColor = vertexColor;\n"
	"}\n"
);

//=============================================================================
// Class:			RenderWindow
// Function:		RenderWindow
//
// Description:		Constructor for RenderWindow class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		parent		= wxWindow& reference to the owner of this object
//		id			= wxWindowID to identify this window
//		attr		= const wxGLAttributes& NOTE: Under GTK, must contain WX_GL_DOUBLEBUFFER at minimum
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
//=============================================================================
RenderWindow::RenderWindow(wxWindow &parent, wxWindowID id, const wxGLAttributes& attr,
    const wxPoint& position, const wxSize& size, long style) : wxGLCanvas(
	&parent, attr, id, position, size, style | wxFULL_REPAINT_ON_RESIZE)
{
	context = nullptr;
	glewInitialized = false;

	wireFrame = false;
	view3D = true;
	viewOrthogonal = false;

	// Make some assumptions to compute the horizontal viewing range
	topMinusBottom = 100.0;
	nearClip = 1.0;
	farClip = 500.0;

	AutoSetFrustum();

	modelviewMatrix.Resize(4, 4);
	projectionMatrix.Resize(4, 4);

	SetCameraView(Vector(1.0, 0.0, 0.0), Vector(0.0, 0.0, 0.0), Vector(0.0, 0.0, 1.0));
	isInteracting = false;

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);// To avoid flashing under MSW

	modified = true;
	sizeUpdateRequired = true;
	modelviewModified = true;
	needAlphaSort = true;
	needOrderSort = true;
}

//=============================================================================
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
//=============================================================================
RenderWindow::~RenderWindow()
{
	primitiveList.Clear();

	delete GetContext();
	context = nullptr;
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
wxGLContext* RenderWindow::GetContext()
{
	if (!context)
		context = new wxGLContext(this);

	return context;
}

//=============================================================================
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
//=============================================================================
void RenderWindow::Render()
{
	if (!GetContext() || !IsShownOnScreen())
		return;

	SetCurrent(*context);
	wxPaintDC(this);

	const unsigned int shaderCount(shaders.size());

	assert(!GLHasError());

	if (!glewInitialized)
	{
		if (glewInit() != GLEW_OK)
			return;
		BuildShaders();
		glewInitialized = true;
	}

	if (sizeUpdateRequired)
		DoResize();

	if (modified)
		Initialize();
	else if (modelviewModified)
		UpdateModelviewMatrix();

	glClearColor((float)backgroundColor.GetRed(), (float)backgroundColor.GetGreen(),
		(float)backgroundColor.GetBlue(), (float)backgroundColor.GetAlpha());

	if (view3D)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);

	// Sort the primitives by Color.GetAlpha to ensure that transparent objects are rendered last
	if (needAlphaSort)
	{
		std::sort(primitiveList.begin(), primitiveList.end(), AlphaSortPredicate);
		needAlphaSort = false;
	}

	// Generally, all objects will have the same draw order and this won't do anything,
	// but for some cases we do want to override the draw order just before rendering
	if (needOrderSort)
	{
		std::stable_sort(primitiveList.begin(), primitiveList.end(), OrderSortPredicate);
		needOrderSort = false;
	}

	// NOTE:  Any primitive that uses it's own program should re-load the default program
	// by calling RenderWindow::UseDefaultProgram() at the end of GenerateGeometry()
	unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
		primitiveList[i]->Draw();

	glFlush();
	SwapBuffers();

	// If shaders are added mid-render, we need to re-render to ensure everything gets displayed
	if (shaders.size() != shaderCount)
		Render();

	assert(!GLHasError());
}

//=============================================================================
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
//=============================================================================
void RenderWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    Render();
}

//=============================================================================
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
//=============================================================================
void RenderWindow::OnSize(wxSizeEvent& WXUNUSED(event))
{
    sizeUpdateRequired = true;
	Refresh();
}

//=============================================================================
// Class:			RenderWindow
// Function:		DoResize
//
// Description:		Handles actions required to update the screen after resizing.
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
void RenderWindow::DoResize()
{
	// set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
	int w, h;
	GetClientSize(&w, &h);
	glViewport(0, 0, w, h);

	AutoSetFrustum();// This takes care of any change in aspect ratio

	sizeUpdateRequired = false;
	modelviewModified = true;
	modified = true;
}

//=============================================================================
// Class:			RenderWindow
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
//=============================================================================
void RenderWindow::OnEnterWindow(wxMouseEvent &event)
{
	//SetFocus();
	event.Skip();
}

//=============================================================================
// Class:			RenderWindow
// Function:		RemoveActor
//
// Description:		Removes the specified actor from the display list, if it
//					is in the list.
//
// Input Arguments:
//		toRemove	= Primitive* pointing to the object to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//=============================================================================
bool RenderWindow::RemoveActor(Primitive *toRemove)
{
	if (!toRemove)
		return false;

	unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
	{
		if (toRemove == primitiveList[i].get())
		{
			primitiveList.Remove(i);
			return true;
		}
	}

	return false;
}

//=============================================================================
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
//=============================================================================
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

	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	float glMatrix[16];
	GLuint i;
	for (i = shaders.size(); i > 0; i--)
	{
		if (shaders[i - 1].needsModelview || shaders[i - 1].needsProjection)
			glUseProgram(shaders[i - 1].programId);

		ConvertMatrixToGL(modelviewMatrix, glMatrix);
		if (shaders[i - 1].needsModelview && modelviewModified)
			glUniformMatrix4fv(shaders[i - 1].modelViewLocation, 1, GL_FALSE, glMatrix);
		modelviewModified = false;

		ConvertMatrixToGL(projectionMatrix, glMatrix);
		if (shaders[i - 1].needsProjection)
			glUniformMatrix4fv(shaders[i - 1].projectionLocation, 1, GL_FALSE, glMatrix);
	}

	modified = false;
}

//=============================================================================
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
//=============================================================================
void RenderWindow::OnMouseWheelEvent(wxMouseEvent &event)
{
	PerformInteraction(InteractionDollyWheel, event);
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
// Class:			RenderWindow
// Function:		PerformInteraction
//
// Description:		Performs the specified interaction.
//					NOTE:  Modifying the modelview matrix moves the scene
//					relative to the eyepoint in the scene's coordinate system!
//
// Input Arguments:
//		interaction	= InteractionType specifying which type of motion to create
//		event		= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::PerformInteraction(InteractionType interaction, wxMouseEvent &event)
{
	SetCurrent(*GetContext());

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

//=============================================================================
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
//=============================================================================
void RenderWindow::StoreMousePosition(wxMouseEvent &event)
{
	lastMousePosition[0] = event.GetX();
	lastMousePosition[1] = event.GetY();
}

//=============================================================================
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
//=============================================================================
void RenderWindow::OnMouseUpEvent(wxMouseEvent& WXUNUSED(event))
{
	isInteracting = false;
}

//=============================================================================
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
//=============================================================================
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

	Translate(modelviewMatrix, focalPoint.x, focalPoint.y, focalPoint.z);
	Rotate(modelviewMatrix, angle, axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);
	Translate(modelviewMatrix, -focalPoint.x, -focalPoint.y, -focalPoint.z);
	modelviewModified = true;
}

//=============================================================================
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
//=============================================================================
void RenderWindow::DoWheelDolly(wxMouseEvent &event)
{
	// Handle 3D dollying differently than 2D dollying
	if (view3D)
	{
		const double dollyFactor(0.05);
		const double nominalWheelRotation(120.0);
		SetTopMinusBottom(topMinusBottom * (1.0 + event.GetWheelRotation() / nominalWheelRotation * dollyFactor));
	}
	else
	{
		// Nothing here!
	}
}

//=============================================================================
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
//=============================================================================
void RenderWindow::DoDragDolly(wxMouseEvent &event)
{
	if (view3D)
	{
		const double dollyFactor(0.05);
		double deltaMouse = lastMousePosition[1] - event.GetY();
		SetTopMinusBottom(topMinusBottom * (1.0 + deltaMouse * dollyFactor));
	}
	else
	{
		// Nothing here!
	}
}

//=============================================================================
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
//=============================================================================
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

		double motionFactor = 0.15;
		mouseMotion *= motionFactor;

		Translate(modelviewMatrix, mouseMotion.x, mouseMotion.y, mouseMotion.z);
		modelviewModified = true;

		focalPoint -= mouseMotion;
	}
	else
	{
		// Nothing here!
	}
}

//=============================================================================
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
//=============================================================================
void RenderWindow::SetCameraView(const Vector &position, const Vector &lookAt,
	const Vector &upDirection)
{
	modelviewModified = true;

	// Compute the MODELVIEW matrix
	// (Use calculations from gluLookAt documentation)
	Vector f = (lookAt - position).Normalize();
	Vector up = upDirection.Normalize();
	Vector s = f.Cross(up);
	if (!PlotMath::IsZero(s))
	{
		Vector u = s.Cross(f);
		modelviewMatrix.Set(s.x, s.y, s.z, 0.0,
							u.x, u.y, u.z, 0.0,
							-f.x, -f.y, -f.z, 0.0,
							0.0, 0.0, 0.0, 1.0);
		
		Translate(modelviewMatrix, -position.x, -position.y, -position.z);
		modelviewModified = true;
	}

	focalPoint = lookAt;
}

//=============================================================================
// Class:			RenderWindow
// Function:		UpdateModelviewMatrix
//
// Description:		Makes the openGL calls to update the modelview matrix.
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
void RenderWindow::UpdateModelviewMatrix()
{
	float glModelviewMatrix[16];
	ConvertMatrixToGL(modelviewMatrix, glModelviewMatrix);

	unsigned int i;
	for (i = shaders.size(); i > 0; i--)
	{
		if (shaders[i - 1].needsModelview)
		{
			glUseProgram(shaders[i - 1].programId);
			glUniformMatrix4fv(shaders[i - 1].modelViewLocation, 1, GL_FALSE, glModelviewMatrix);
		}
	}
	modelviewModified = false;
}

//=============================================================================
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
//=============================================================================
Vector RenderWindow::TransformToView(const Vector &modelVector) const
{
	return modelviewMatrix.GetSubMatrix(0, 0, 3, 3) * modelVector;
}

//=============================================================================
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
//=============================================================================
Vector RenderWindow::TransformToModel(const Vector &viewVector) const
{
	return modelviewMatrix.GetSubMatrix(0, 0, 3, 3).Transpose() * viewVector;
}

//=============================================================================
// Class:			RenderWindow
// Function:		GetCameraPosition
//
// Description:		Extracts the camera position from the modelview matrix.
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
Vector RenderWindow::GetCameraPosition() const
{
	Vector cameraPosition(modelviewMatrix(0, 3), modelviewMatrix(1, 3), modelviewMatrix(2, 3));
	return TransformToModel(cameraPosition);
}

//=============================================================================
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
//=============================================================================
void RenderWindow::AutoSetFrustum()
{
	modified = true;

	// This method is really for 3D renderers - for 2D, we just re-initialize to handle change in aspect ratio/size
	if (!view3D)
		return;

	wxSize windowSize = GetSize();
	aspectRatio = (double)windowSize.GetWidth() / (double)windowSize.GetHeight();
}

//=============================================================================
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
//=============================================================================
wxString RenderWindow::GetGLError()
{
	int e = glGetError();

	return GetGLError(e);
}

//=============================================================================
// Class:			RenderWindow
// Function:		GetGLError
//
// Description:		Returns a string describing any openGL errors.
//
// Input Arguments:
//		e	= const GLint&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the error description
//
//=============================================================================
wxString RenderWindow::GetGLError(const GLint& e)
{
	if (e == GL_NO_ERROR)
		return _T("No errors");
	else if (e == GL_INVALID_ENUM)
		return _T("Invalid enumeration");
	else if (e == GL_INVALID_VALUE)
		return _T("Invalid value");
	else if (e == GL_INVALID_OPERATION)
		return _T("Invalid operation");
	else if (e == GL_STACK_OVERFLOW)
		return _T("Stack overflow");
	else if (e == GL_STACK_UNDERFLOW)
		return _T("Stack underflow");
	else if (e == GL_OUT_OF_MEMORY)
		return _T("Out of memory");

	return _T("Unrecognized error");
}

//=============================================================================
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
//=============================================================================
bool RenderWindow::WriteImageToFile(wxString pathAndFileName) const
{
	wxImage newImage(GetImage());
	wxInitAllImageHandlers();
	return newImage.SaveFile(pathAndFileName);
}

//=============================================================================
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
//=============================================================================
wxImage RenderWindow::GetImage() const
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

//=============================================================================
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
//=============================================================================
bool RenderWindow::IsThisRendererSelected(const Primitive *pickedObject) const
{
	// Iterate through the list of primitives in the scene
	// If one of them has the same address as our argument, return true
	unsigned int i;
	for (i = 0; i < primitiveList.GetCount(); i++)
	{
		if (primitiveList[i].get() == pickedObject)
			return true;
	}

	return false;
}

//=============================================================================
// Class:			RenderWindow
// Function:		AlphaSortPredicate
//
// Description:		Predicate for sorting by alpha.
//
// Input Arguments:
//		p1	= const std::unique_ptr<Primitive>&
//		p2	= const std::unique_ptr<Primitive>&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if p1's alpha value is lower than that of p2
//
//=============================================================================
bool RenderWindow::AlphaSortPredicate(const std::unique_ptr<Primitive>& p1,
	const std::unique_ptr<Primitive>& p2)
{
	return p1->GetColor().GetAlpha() > p2->GetColor().GetAlpha();
}

//=============================================================================
// Class:			RenderWindow
// Function:		OrderSortPredicate
//
// Description:		Predicate for sorting by draw order.
//
// Input Arguments:
//		p1	= const std::unique_ptr<Primitive>&
//		p2	= const std::unique_ptr<Primitive>&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if p1 should be drawn first
//
//=============================================================================
bool RenderWindow::OrderSortPredicate(const std::unique_ptr<Primitive>& p1,
	const std::unique_ptr<Primitive>& p2)
{
	return p1->GetDrawOrder() < p2->GetDrawOrder();
}

//=============================================================================
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
//		gl		= float[] in the form expected by OpenGL
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::ConvertMatrixToGL(const Matrix& matrix, float gl[])
{
	unsigned int i, j;
	for (i = 0; i < matrix.GetNumberOfRows(); i++)
	{
		for (j = 0; j < matrix.GetNumberOfColumns(); j++)
			gl[i * matrix.GetNumberOfColumns() + j] = matrix(j, i);
	}
}

//=============================================================================
// Class:			RenderWindow
// Function:		ConvertGLToMatrix
//
// Description:		Converts from OpenGL array to Matrix type.  Size of matrix
//					must be set before this call.
//
// Input Arguments:
//		gl		= float[] in the form expected by OpenGL
//
// Output Arguments:
//		matrix	= const Matrix& containing the original data
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::ConvertGLToMatrix(Matrix& matrix, const float gl[])
{
	unsigned int i, j;
	for (i = 0; i < matrix.GetNumberOfRows(); i++)
	{
		for (j = 0; j < matrix.GetNumberOfColumns(); j++)
			matrix(j, i) = gl[i * matrix.GetNumberOfColumns() + j];
	}
}

//=============================================================================
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
//=============================================================================
void RenderWindow::Initialize2D()
{
	// Disable unused options to speed-up 2D rendering
	glDepthMask(GL_FALSE);
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	glDisable(GL_DEPTH_TEST);
	glPixelZoom(1.0, 1.0);

	// Enable blending to support font rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	modelviewMatrix.MakeIdentity();
	ShiftForExactPixelization();
	modelviewModified = true;

	// Enable antialiasing
	glEnable(GL_MULTISAMPLE);
}

//=============================================================================
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
//=============================================================================
void RenderWindow::Initialize3D()
{
	// Turn Z-buffering on
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Z-buffer settings
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);

	// Enable antialiasing
	glEnable(GL_MULTISAMPLE);
}

//=============================================================================
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
//=============================================================================
Matrix RenderWindow::Generate2DProjectionMatrix() const
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

//=============================================================================
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
//=============================================================================
Matrix RenderWindow::Generate3DProjectionMatrix() const
{
	// For orthogonal projections, top - bottom and left - right give size in
	// screen coordinates.  For perspective projections, these combined with
	// the near clipping plane give FOV (top - bottom specifies screen height at
	// the near clipping plane).
	//  hFOV = atan(nearClip * 2.0 / leftMinusRight);// [rad]
	//  vFOV = atan(nearClip * 2.0 / topMinusBottom);// [rad]
	// The distance at which unity scaling occurs is the cotangent of (top - bottom) / 2.
	// We can use the distance set in SetCameraView() to determine 
	Matrix projectionMatrix(4, 4);
	double rightMinusLeft(topMinusBottom * aspectRatio);
	if (viewOrthogonal)
	{
		projectionMatrix.SetElement(0, 0, 2.0 / rightMinusLeft);
		projectionMatrix.SetElement(1, 1, 2.0 / topMinusBottom);
		projectionMatrix.SetElement(2, 2, 2.0 / (nearClip - farClip));
		projectionMatrix.SetElement(3, 3, 1.0);
		// For symmetric frustums, elements (0,3) and (1,3) are zero
		projectionMatrix.SetElement(2, 3, (nearClip + farClip) / (nearClip - farClip));
	}
	else
	{
		projectionMatrix.SetElement(0, 0, 2.0 * nearClip / rightMinusLeft);
		projectionMatrix.SetElement(1, 1, 2.0 * nearClip / topMinusBottom);
		projectionMatrix.SetElement(2, 2, (nearClip + farClip) / (nearClip - farClip));
		projectionMatrix.SetElement(2, 3, 2.0 * farClip * nearClip / (nearClip - farClip));
		projectionMatrix.SetElement(3, 2, -1.0);
	}

	return projectionMatrix;
}

//=============================================================================
// Class:			RenderWindow
// Function:		SetViewOrthogonal
//
// Description:		Switches between perspective and orthogonal projections
//					while maintaining nominal scale.
//
// Input Arguments:
//		viewOrthogonal	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::SetViewOrthogonal(const bool &viewOrthogonal)
{
	if (this->viewOrthogonal == viewOrthogonal)
		return;

	this->viewOrthogonal = viewOrthogonal;
	modified = true;

	// TODO:  Would be better to have some parameter that is common between the
	// two modes and to just compute the projection matrix accordingly.
	
	// We can compute the distance at which we are focused, and then determine
	// the correct value of SetTopMinusBottom() in order to maintain unit scale
	// at this distance.
	double nominalDistance = GetCameraPosition().Distance(focalPoint);
	if (viewOrthogonal)// was perspective
		topMinusBottom *= nominalDistance / nearClip;
	else// was orthogonal
		topMinusBottom *= nearClip / nominalDistance;
}

//=============================================================================
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
//=============================================================================
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

//=============================================================================
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
//=============================================================================
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

//=============================================================================
// Class:			RenderWindow
// Function:		CreateDefaultVertexShader
//
// Description:		Builds the default vertex shader and returns its index.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		GLuint
//
//=============================================================================
GLuint RenderWindow::CreateDefaultVertexShader()
{
	return CreateShader(GL_VERTEX_SHADER, GetDefaultVertexShader());
}

//=============================================================================
// Class:			RenderWindow
// Function:		CreateDefaultFragmentShader
//
// Description:		Builds the default fragment shader and returns its index.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		GLuint
//
//=============================================================================
GLuint RenderWindow::CreateDefaultFragmentShader()
{
	return CreateShader(GL_FRAGMENT_SHADER, GetDefaultFragmentShader());
}

//=============================================================================
// Class:			RenderWindow
// Function:		CreateShader
//
// Description:		Compiles the specified shader.
//
// Input Arguments:
//		type			= const GLenum&
//		shaderContents	= const std::string& containing the actual string
//						  contents of the shader
//
// Output Arguments:
//		None
//
// Return Value:
//		GLuint specifying the index of the shader
//
//=============================================================================
GLuint RenderWindow::CreateShader(const GLenum& type, const std::string& shaderContents)
{
	GLuint shader = glCreateShader(type);
	const char* shaderString = shaderContents.c_str();
	glShaderSource(shader, 1, &shaderString, nullptr);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        
		std::unique_ptr<GLchar[]> strInfoLog = std::make_unique<GLchar[]>(infoLogLength + 1);
		glGetShaderInfoLog(shader, infoLogLength, nullptr, strInfoLog.get());
		std::cerr << strInfoLog.get() << std::endl;
		assert(false);
	}

	return shader;
}

//=============================================================================
// Class:			RenderWindow
// Function:		CreateProgram
//
// Description:		Builds the default program.
//
// Input Arguments:
//		shaderList	= const std::vector<GLuint>&
//
// Output Arguments:
//		None
//
// Return Value:
//		GLuint specifying the index of the program
//
//=============================================================================
GLuint RenderWindow::CreateProgram(const std::vector<GLuint>& shaderList)
{
	GLuint program = glCreateProgram();
	size_t i;
	for (i = 0; i < shaderList.size(); i++)
		glAttachShader(program, shaderList[i]);

	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
		std::unique_ptr<GLchar[]> strInfoLog = std::make_unique<GLchar[]>(infoLogLength + 1);
		glGetProgramInfoLog(program, infoLogLength, nullptr, strInfoLog.get());
		std::cerr << strInfoLog.get() << std::endl;
		assert(false);
	}

	for (i = 0; i < shaderList.size(); i++)
	{
		glDetachShader(program, shaderList[i]);
		glDeleteShader(shaderList[i]);
	}

	return program;
}

//=============================================================================
// Class:			RenderWindow
// Function:		BuildShaders
//
// Description:		Builds the default shaders and sets the indices to the
//					matrices we need to track.
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
void RenderWindow::BuildShaders()
{
	std::vector<GLuint> shaderList;
	shaderList.push_back(CreateDefaultVertexShader());
	shaderList.push_back(CreateDefaultFragmentShader());

	ShaderInfo s;
	s.programId = CreateProgram(shaderList);
	s.needsModelview = true;
	s.needsProjection = true;

	s.modelViewLocation = glGetUniformLocation(s.programId, modelviewName.c_str());
	s.projectionLocation = glGetUniformLocation(s.programId, projectionName.c_str());

	positionAttributeLocation = glGetAttribLocation(s.programId, positionName.c_str());
	colorAttributeLocation = glGetAttribLocation(s.programId, colorName.c_str());

	AddShader(s);
}

//=============================================================================
// Class:			RenderWindow
// Function:		Translate
//
// Description:		Applies the specified translation to the specified matrix.
//
// Input Arguments:
//		m	= Matrix&
//		x	= const double&
//		y	= const double&
//		z	= const double&
//
// Output Arguments:
//		m	= Matrix&
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::Translate(Matrix& m, const double& x, const double& y, const double& z)
{
	Matrix translation(4, 4, 1.0, 0.0, 0.0, x,
							 0.0, 1.0, 0.0, y,
							 0.0, 0.0, 1.0, z,
							 0.0, 0.0, 0.0, 1.0);
	m *= translation;
}

//=============================================================================
// Class:			RenderWindow
// Function:		Rotate
//
// Description:		Applies the specified rotation to the specified matrix.
//
// Input Arguments:
//		m		= Matrix&
//		angle	= const double& (radians)
//		x		= const double& axis of rotation x-component
//		y		= const double& axis of rotation y-component
//		z		= const double& axis of rotation z-component
//
// Output Arguments:
//		m	= Matrix&
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::Rotate(Matrix& m, const double& angle,
	const double& x, const double& y, const double& z)
{
	Vector axis(x, y, z);
	Matrix rotation3 = Vector::GenerateRotationMatrix(angle, axis);
	Matrix rotation4(4, 4);
	rotation4.MakeIdentity();

	unsigned int r, c;
	for (r = 0; r < 3; r++)
	{
		for (c = 0; c < 3; c++)
			rotation4(r, c) = rotation3(r, c);
	}

	m *= rotation4;
}

//=============================================================================
// Class:			RenderWindow
// Function:		Scale
//
// Description:		Applies the specified scaling to the specified matrix.
//
// Input Arguments:
//		m		= Matrix&
//		x		= const double&
//		y		= const double&
//		z		= const double&
//
// Output Arguments:
//		m	= Matrix&
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::Scale(Matrix& m, const double& x, const double& y, const double& z)
{
	Matrix scale(4, 4);
	scale.MakeIdentity();
	scale(0,0) = x;
	scale(1,1) = y;
	scale(2,2) = z;

	m *= scale;
}

//=============================================================================
// Class:			RenderWindow
// Function:		UseDefaultProgram
//
// Description:		Loads the default OpenGL program.
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
void RenderWindow::UseDefaultProgram() const
{
	glUseProgram(shaders[0].programId);
}

//=============================================================================
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
//=============================================================================
void RenderWindow::ShiftForExactPixelization()
{
	Translate(modelviewMatrix, exactPixelShift, exactPixelShift, 0.0);
}

//=============================================================================
// Class:			RenderWindow
// Function:		AddShader
//
// Description:		Adds a shader to our list of managed shaders.
//
// Input Arguments:
//		shader	= const ShaderInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::AddShader(const ShaderInfo& shader)
{
	shaders.push_back(shader);
	modified = true;
}

//=============================================================================
// Class:			RenderWindow
// Function:		SendUniformMatrix
//
// Description:		Loads uniform matrix to openGL.
//
// Input Arguments:
//		shader	= const ShaderInfo&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::SendUniformMatrix(const Matrix& m, const GLuint& location)
{
	float glMatrix[16];
	ConvertMatrixToGL(m, glMatrix);

	glUniformMatrix4fv(location, 1, GL_FALSE, glMatrix);
}

//=============================================================================
// Class:			RenderWindow
// Function:		GLHasError
//
// Description:		Returns true if OpenGL has any errors.  Intended as a
//					useful place to break on OpenGL errors.  Allows checking
//					error codes.
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
bool RenderWindow::GLHasError()
{
	int e = glGetError();
	if (e == GL_NO_ERROR)
		return false;

	wxString errorString = GetGLError(e);

	return true;
}

}// namespace LibPlot2D
