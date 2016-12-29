/*=============================================================================
                                   LibPlot2D
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

// GLEW headers
#include <GL/glew.h>// Must be included before gl.h (so, before renderWindow.h)

// Local headers
#include "lp2d/renderer/renderWindow.h"
#include "lp2d/utilities/math/plotMath.h"

// Eigen headers
#include <Eigen/Geometry>

// wxWidgets headers
#include <wx/dcclient.h>
#include <wx/image.h>

// Standard C++ headers
#include <vector>
#include <algorithm>
#include <iostream>

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
RenderWindow::RenderWindow(wxWindow &parent, wxWindowID id,
	const wxGLAttributes& attr, const wxPoint& position, const wxSize& size,
	long style) : wxGLCanvas(&parent, attr, id, position, size,
		style | wxFULL_REPAINT_ON_RESIZE)
{
	AutoSetFrustum();

	SetCameraView((Eigen::Vector3d() << 1.0, 0.0, 0.0).finished(),
		Eigen::Vector3d::Zero(),
		(Eigen::Vector3d() << 0.0, 0.0, 1.0).finished());

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);// To avoid flashing under MSW
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
	{
		wxGLContextAttrs attributes;
		attributes.PlatformDefaults().OGLVersion(3, 0).EndList();
		context = std::make_unique<wxGLContext>(this, nullptr, &attributes);
		assert(context->IsOK() && "Minimum OpenGL verison not met (requires 3.0)");
	}

	return context.get();
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
	for (auto& p : primitiveList)
		p->Draw();

	SwapBuffers();// TODO:  Memory leak here?

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
	for (i = 0; i < primitiveList.GetCount(); ++i)
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
	Eigen::Matrix4d projectionMatrix;
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
	for (i = shaders.size(); i > 0; --i)
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
	PerformInteraction(Interaction::DollyWheel, event);
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

	Interaction interaction;
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
//		interaction	= Interaction specifying which type of motion to create
//		event		= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::PerformInteraction(Interaction interaction, wxMouseEvent &event)
{
	SetCurrent(*GetContext());

	if (!isInteracting)
	{
		// TODO:  Get focal point in order to perform interactions around the cursor
		//FocalPoint.Set(0.0, 0.0, 0.0);

		// Don't re-compute the focal point until the next interaction
		isInteracting = true;
	}

	if (interaction == Interaction::DollyWheel)
		DoWheelDolly(event);
	else if (interaction == Interaction::DollyDrag)
		DoDragDolly(event);
	else if (interaction == Interaction::Pan)
		DoPan(event);
	else if (interaction == Interaction::Rotate)
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

	Eigen::Vector3d upDirection(0.0, 1.0, 0.0);
	Eigen::Vector3d normal(0.0, 0.0, 1.0);

	upDirection = TransformToModel(upDirection);
	normal = TransformToModel(normal);
	Eigen::Vector3d leftDirection{ normal.cross(upDirection) };

	Eigen::Vector3d mouseVector{ upDirection *
		static_cast<double>(GetSize().GetHeight() / 2 - event.GetY())
		+ leftDirection *
		static_cast<double>(GetSize().GetWidth() / 2 - event.GetX()) };
	Eigen::Vector3d lastMouseVector{ upDirection *
		static_cast<double>(GetSize().GetHeight() / 2 - lastMousePosition[1])
		+ leftDirection *
		static_cast<double>(GetSize().GetWidth() / 2 - lastMousePosition[0]) };

	// Get a vector that represents the mouse motion (projected onto a plane with the camera
	// position as a normal)
	Eigen::Vector3d mouseMotion{ mouseVector - lastMouseVector };
	Eigen::Vector3d axisOfRotation{ normal.cross(mouseMotion) };

	long xDistance{ GetSize().GetWidth() / 2 - event.GetX() };
	long yDistance{ GetSize().GetHeight() / 2 - event.GetY() };
	long lastXDistance{ GetSize().GetWidth() / 2 - lastMousePosition[0] };
	long lastYDistance{ GetSize().GetHeight() / 2 - lastMousePosition[1] };

	// The angle is determined by how much the mouse moved.  800 pixels of movement will result in
	// a full 360 degrees rotation.
	// TODO:  Add user-adjustable rotation sensitivity (actually, all of the interactions can be adjustable)
	double angle{ sqrt(fabs(static_cast<double>((xDistance - lastXDistance)
		* (xDistance - lastXDistance))
		+ static_cast<double>((yDistance - lastYDistance)
		* (yDistance - lastYDistance)))) / 800.0 * 360.0 };// [deg]

	Translate(modelviewMatrix, focalPoint);
	Rotate(modelviewMatrix, angle, axisOfRotation);
	Translate(modelviewMatrix, -focalPoint);
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
		// Convert up and normal vectors from openGL coordinates to model
		// coordinates
		Eigen::Vector3d upDirection(0.0, 1.0, 0.0);
		Eigen::Vector3d normal(0.0, 0.0, 1.0);

		upDirection = TransformToModel(upDirection);
		normal = TransformToModel(normal);
		Eigen::Vector3d leftDirection{ normal.cross(upDirection) };

		// Get a vector that represents the mouse position relative to the
		// center of the screen
		Eigen::Vector3d mouseVector{ upDirection *
			static_cast<double>(GetSize().GetHeight() / 2 - event.GetY())
			+ leftDirection *
			static_cast<double>(GetSize().GetWidth() / 2 - event.GetX()) };
		Eigen::Vector3d lastMouseVector{ upDirection *
			static_cast<double>(GetSize().GetHeight() / 2 - lastMousePosition[1])
			+ leftDirection *
			static_cast<double>(GetSize().GetWidth() / 2 - lastMousePosition[0]) };

		// Get a vector that represents the mouse motion (projected onto a
		// plane with the camera position as a normal)
		Eigen::Vector3d mouseMotion = mouseVector - lastMouseVector;

		double motionFactor = 0.15;
		mouseMotion *= motionFactor;

		Translate(modelviewMatrix, mouseMotion);
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
//		position	= const Eigen::Vector3d& specifying the camera position
//		lookAt		= const Eigen::Vector3d& specifying the object at which the camera
//					  is to be pointed
//		upDirection	= const Eigen::Vector3d& used to specify the final camera orientation DOF
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::SetCameraView(const Eigen::Vector3d &position,
	const Eigen::Vector3d &lookAt, const Eigen::Vector3d &upDirection)
{
	modelviewModified = true;

	// Compute the MODELVIEW matrix
	// (Use calculations from gluLookAt documentation)
	Eigen::Vector3d f{ (lookAt - position).normalized() };
	Eigen::Vector3d up{ upDirection.normalized() };
	Eigen::Vector3d s{ f.cross(up) };
	if (!PlotMath::IsZero(s))
	{
		Eigen::Vector3d u{ s.cross(f) };
		modelviewMatrix << s(0), s(1), s(2), 0.0,
							u(0), u(1), u(2), 0.0,
							-f(0), -f(1), -f(2), 0.0,
							0.0, 0.0, 0.0, 1.0;
		
		Translate(modelviewMatrix, -position);
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
	for (i = shaders.size(); i > 0; --i)
	{
		if (shaders[i - 1].needsModelview)
		{
			glUseProgram(shaders[i - 1].programId);
			glUniformMatrix4fv(shaders[i - 1].modelViewLocation, 1,
				GL_FALSE, glModelviewMatrix);
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
//		modelVector	= const Eigen::Vector3d& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::Vector3d
//
//=============================================================================
Eigen::Vector3d RenderWindow::TransformToView(
	const Eigen::Vector3d &modelVector) const
{
	return modelviewMatrix.topLeftCorner<3, 3>() * modelVector;
}

//=============================================================================
// Class:			RenderWindow
// Function:		TransformToModel
//
// Description:		Returns a vector equivalent to the specified vector
//					(assumed to be in view coordinates) in model coordinates.
//
// Input Arguments:
//		viewVector	= const Eigen::Vector3d& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::Vector3d
//
//=============================================================================
Eigen::Vector3d RenderWindow::TransformToModel(
	const Eigen::Vector3d &viewVector) const
{
	return modelviewMatrix.topLeftCorner<3, 3>().transpose() * viewVector;
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
Eigen::Vector3d RenderWindow::GetCameraPosition() const
{
	Eigen::Vector3d cameraPosition(modelviewMatrix(0, 3),
		modelviewMatrix(1, 3), modelviewMatrix(2, 3));
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
	for (const auto& p : primitiveList)
	{
		if (p.get() == pickedObject)
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
//		matrix	= const Eigen::Matrix4d& containing the original data
//
// Output Arguments:
//		gl		= float[] in the form expected by OpenGL
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::ConvertMatrixToGL(const Eigen::Matrix4d& matrix, float gl[])
{
	// TODO:  Can this be eliminated by using built-in eigen routines?
	int i;
	for (i = 0; i < matrix.rows(); ++i)
	{
		int j;
		for (j = 0; j < matrix.cols(); ++j)
			gl[i * matrix.cols() + j] = matrix(j, i);
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
//		matrix	= const Eigen::Matrix4d& containing the original data
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::ConvertGLToMatrix(Eigen::Matrix4d& matrix, const float gl[])
{
	int i;
	for (i = 0; i < matrix.rows(); ++i)
	{
		int j;
		for (j = 0; j < matrix.cols(); ++j)
			matrix(j, i) = gl[i * matrix.cols() + j];
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

	modelviewMatrix.setIdentity();
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
//		Eigen::Matrix4d
//
//=============================================================================
Eigen::Matrix4d RenderWindow::Generate2DProjectionMatrix() const
{
	// Set up an orthogonal 2D projection matrix (this puts (0,0) at the lower left-hand corner of the window)
	Eigen::Matrix4d projectionMatrix(Eigen::Matrix4d::Zero());
	projectionMatrix(0, 0) = 2.0 / GetSize().GetWidth();
	projectionMatrix(1, 1) = 2.0 / GetSize().GetHeight();
	projectionMatrix(2, 2) = -2.0;
	projectionMatrix(0, 3) = -1.0;
	projectionMatrix(1, 3) = -1.0;
	projectionMatrix(2, 3) = -1.0;
	projectionMatrix(3, 3) = 1.0;

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
//		Eigen::Matrix4d
//
//=============================================================================
Eigen::Matrix4d RenderWindow::Generate3DProjectionMatrix() const
{
	// For orthogonal projections, top - bottom and left - right give size in
	// screen coordinates.  For perspective projections, these combined with
	// the near clipping plane give FOV (top - bottom specifies screen height at
	// the near clipping plane).
	//  hFOV = atan(nearClip * 2.0 / leftMinusRight);// [rad]
	//  vFOV = atan(nearClip * 2.0 / topMinusBottom);// [rad]
	// The distance at which unity scaling occurs is the cotangent of (top - bottom) / 2.
	// We can use the distance set in SetCameraView() to determine 
	Eigen::Matrix4d projectionMatrix(Eigen::Matrix4d::Zero());
	double rightMinusLeft(topMinusBottom * aspectRatio);
	if (viewOrthogonal)
	{
		projectionMatrix(0, 0) = 2.0 / rightMinusLeft;
		projectionMatrix(1, 1) = 2.0 / topMinusBottom;
		projectionMatrix(2, 2) = 2.0 / (nearClip - farClip);
		projectionMatrix(3, 3) = 1.0;
		// For symmetric frustums, elements (0,3) and (1,3) are zero
		projectionMatrix(2, 3) = (nearClip + farClip) / (nearClip - farClip);
	}
	else
	{
		projectionMatrix(0, 0) = 2.0 * nearClip / rightMinusLeft;
		projectionMatrix(1, 1) = 2.0 * nearClip / topMinusBottom;
		projectionMatrix(2, 2) = (nearClip + farClip) / (nearClip - farClip);
		projectionMatrix(2, 3) = 2.0 * farClip * nearClip / (nearClip - farClip);
		projectionMatrix(3, 2) = -1.0;
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
	double nominalDistance = (GetCameraPosition() - focalPoint).norm();
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
//		interaction	= Interaction&
//
// Return Value:
//		bool
//
//=============================================================================
bool RenderWindow::Determine2DInteraction(const wxMouseEvent &event,
	Interaction &interaction) const
{
	// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
	if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
		interaction = Interaction::DollyDrag;

	// PAN:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
		interaction = Interaction::Pan;

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
//		interaction	= Interaction&
//
// Return Value:
//		bool
//
//=============================================================================
bool RenderWindow::Determine3DInteraction(const wxMouseEvent &event,
	Interaction &interaction) const
{
	// PAN:  Left mouse button + Shift OR Right mouse button
	if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
		interaction = Interaction::Pan;

	// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
	else if ((event.LeftIsDown() && (event.CmdDown() || event.AltDown()))
		|| event.MiddleIsDown())
		interaction = Interaction::DollyDrag;

	// ROTATE:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
		interaction = Interaction::Rotate;

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
	for (const auto& shader : shaderList)
		glAttachShader(program, shader);

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

	for (const auto& shader : shaderList)
	{
		glDetachShader(program, shader);
		glDeleteShader(shader);
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
//		m	= Eigen::Matrix4d&
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
void RenderWindow::Translate(Eigen::Matrix4d& m, const Eigen::Vector3d& v)
{
	Eigen::Matrix4d translation(Eigen::Matrix4d::Identity());
	translation(0, 3) = v(0);
	translation(1, 3) = v(1);
	translation(2, 3) = v(2);
	m *= translation;
}

//=============================================================================
// Class:			RenderWindow
// Function:		Rotate
//
// Description:		Applies the specified rotation to the specified matrix.
//
// Input Arguments:
//		m		= Eigen::Matrix4d&
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
void RenderWindow::Rotate(Eigen::Matrix4d& m, const double& angle,
	Eigen::Vector3d axis)
{
	//Eigen::Vector3d axis(x, y, z);
	axis.normalize();
	Eigen::Matrix3d rotation3d(Eigen::AngleAxisd(angle, axis).toRotationMatrix());
	Eigen::Matrix4d rotation4d(Eigen::Matrix4d::Identity());

	rotation4d.topLeftCorner<3, 3>() = rotation3d;
	m *= rotation4d;
}

//=============================================================================
// Class:			RenderWindow
// Function:		Scale
//
// Description:		Applies the specified scaling to the specified matrix.
//
// Input Arguments:
//		m		= Eigen::Matrix4d&
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
void RenderWindow::Scale(Eigen::Matrix4d& m, const Eigen::Vector3d& v)
{
	Eigen::Matrix4d scale(Eigen::Matrix4d::Identity());
	scale(0,0) = v(0);
	scale(1,1) = v(1);
	scale(2,2) = v(2);

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
	Translate(modelviewMatrix,
		Eigen::Vector3d(exactPixelShift, exactPixelShift, 0.0));
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
//		m			= const Eigen::Matrix4d&
//		location	= const GLuint&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//=============================================================================
void RenderWindow::SendUniformMatrix(const Eigen::Matrix4d& m, const GLuint& location)
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

	//wxString errorString = GetGLError(e);

	return true;
}

}// namespace LibPlot2D
