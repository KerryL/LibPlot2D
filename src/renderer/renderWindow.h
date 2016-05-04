/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  renderWindow.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:

#ifndef RENDER_WINDOW_H_
#define RENDER_WINDOW_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/glcanvas.h>

// Local headers
#include "utilities/managedList.h"
#include "utilities/math/vector.h"
#include "utilities/math/matrix.h"
#include "renderer/primitives/primitive.h"

class RenderWindow : public wxGLCanvas
{
public:
	RenderWindow(wxWindow &parent, wxWindowID id, const wxGLAttributes& attr,
		const wxPoint& position, const wxSize& size, long style = 0);
	virtual ~RenderWindow();

	void Initialize();
	void SetCameraView(const Vector &position, const Vector &lookAt, const Vector &upDirection);

	// Transforms between the model coordinate system and the view (openGL) coordinate system
	Vector TransformToView(const Vector &modelVector) const;
	Vector TransformToModel(const Vector &viewVector) const;
	Vector GetCameraPosition() const;

	// Sets the viewing frustum to match the current size of the window
	void AutoSetFrustum();

	// Adds actors to the primitives list
	inline void AddActor(Primitive *toAdd) { primitiveList.Add(toAdd); modified = true; }

	// Removes actors from the primitives list
	bool RemoveActor(Primitive *toRemove);

	// Private data accessors
	inline void SetWireFrame(const bool& wireFrame) { this->wireFrame = wireFrame; modified = true; }
	void SetViewOrthogonal(const bool& viewOrthogonal);

	inline void SetTopMinusBottom(const double& topMinusBottom) { this->topMinusBottom = topMinusBottom; modified = true; }
	inline void SetAspectRatio(const double& aspectRatio) { this->aspectRatio = aspectRatio; modified = true; }
	inline void SetNearClip(const double& nearClip) { this->nearClip = nearClip; modified = true; }
	inline void SetFarClip(const double& farClip) { this->farClip = farClip; modified = true; }
	inline void SetView3D(const bool& view3D) { this->view3D = view3D; modified = true; }

	virtual void SetBackgroundColor(const Color& backgroundColor) { this->backgroundColor = backgroundColor; modified = true; }
	inline Color GetBackgroundColor() { return backgroundColor; }

	inline bool GetWireFrame() const { return wireFrame; };
	inline bool GetViewOrthogonal() const { return viewOrthogonal; }
	inline bool GetView3D() const { return view3D; }

	inline double GetAspectRatio() const { return aspectRatio; }

	// Returns a string containing any OpenGL errors
	wxString GetGLError() const;

	// Writes the current image to file
	bool WriteImageToFile(wxString pathAndFileName) const;
	wxImage GetImage() const;

	// Determines if a particular primitive is in the scene owned by this object
	bool IsThisRendererSelected(const Primitive *pickedObject) const;

	void SetNeedAlphaSort() { needAlphaSort = true; }
	void SetNeedOrderSort() { needOrderSort = true; }

	static GLuint CreateShader(const GLenum& type, const std::string& shaderContents);
	static GLuint CreateProgram(const std::vector<GLuint>& shaderList);

	void ShiftForExactPixelization();
	void UseDefaultProgram();

	GLuint GetPositionLocation() const { return positionAttributeLocation; }
	GLuint GetColorLocation() const { return colorAttributeLocation; }

	virtual unsigned int GetVertexDimension() const { return 4; }

private:
	wxGLContext *context;
	wxGLContext* GetContext();

	static const double exactPixelShift;

	// Flags describing the options for this object's functionality
	bool wireFrame;
	bool viewOrthogonal;

	// The parameters that describe the viewing frustum
	double topMinusBottom;// in model-space units
	double aspectRatio;
	double nearClip;
	double farClip;

	Color backgroundColor;

	static bool AlphaSortPredicate(const Primitive* p1, const Primitive* p2);
	static bool OrderSortPredicate(const Primitive* p1, const Primitive* p2);

	bool needAlphaSort;
	bool needOrderSort;

	// Event handlers-----------------------------------------------------
	// Interactor events
	virtual void OnMouseWheelEvent(wxMouseEvent &event);
	virtual void OnMouseMoveEvent(wxMouseEvent &event);
	virtual void OnMouseUpEvent(wxMouseEvent &event);

	// Window events
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEnterWindow(wxMouseEvent &event);
	// End event handlers-------------------------------------------------

	void Render();

	enum InteractionType
	{
		InteractionDollyDrag,// zoom
		InteractionDollyWheel,// zoom
		InteractionPan,// translate
		InteractionRotate
	};

	// Performs the computations and transformations associated with the specified interaction
	void PerformInteraction(InteractionType interaction, wxMouseEvent &event);

	// The interaction events (called from within the real event handlers)
	void DoRotate(wxMouseEvent &event);
	void DoWheelDolly(wxMouseEvent &event);
	void DoDragDolly(wxMouseEvent &event);
	void DoPan(wxMouseEvent &event);

	// Updates the transformation matrices according to the current modelview matrix
	//void UpdateTransformationMatricies();
	void UpdateModelviewMatrix();

	bool modelviewModified;
	Matrix modelviewMatrix;
	Matrix projectionMatrix;

	GLint modelviewLocation;
	GLint projectionLocation;

	static const std::string modelviewName;
	static const std::string projectionName;
	static const std::string positionName;
	static const std::string colorName;

	static const std::string defaultVertexShader;
	static const std::string defaultFragmentShader;

	GLuint defaultProgram;
	void BuildShaders();

	GLuint CreateDefaultVertexShader();
	GLuint CreateDefaultFragmentShader();

	GLuint positionAttributeLocation;
	GLuint colorAttributeLocation;

	Vector focalPoint;

	void DoResize();

	bool glewInitialized;

protected:
	bool view3D;
	bool modified;
	bool sizeUpdateRequired;

	ManagedList<Primitive> primitiveList;

	long lastMousePosition[2];
	void StoreMousePosition(wxMouseEvent &event);

	bool Determine2DInteraction(const wxMouseEvent &event, InteractionType &interaction) const;
	bool Determine3DInteraction(const wxMouseEvent &event, InteractionType &interaction) const;

	// Flag indicating whether or not we should select a new focal point for the interactions
	bool isInteracting;

	static void ConvertMatrixToGL(const Matrix& matrix, float gl[]);
	static void ConvertGLToMatrix(Matrix& matrix, const float gl[]);

	void Initialize2D();
	void Initialize3D();

	Matrix Generate2DProjectionMatrix() const;
	Matrix Generate3DProjectionMatrix() const;

	DECLARE_EVENT_TABLE()

	virtual std::string GetDefaultVertexShader() const { return defaultVertexShader; }
	virtual std::string GetDefaultFragmentShader() const { return defaultFragmentShader; }

	static void Translate(Matrix& m, const double& x, const double& y, const double& z);
	static void Rotate(Matrix& m, const double& angle, const double& x, const double& y, const double& z);
	static void Scale(Matrix& m, const double& x, const double& y, const double& z);
};

#endif// RENDER_WINDOW_H_