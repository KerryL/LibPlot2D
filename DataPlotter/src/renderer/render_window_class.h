/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/
// File:  render_window_class.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

#ifndef _RENDER_WINDOW_CLASS_H_
#define _RENDER_WINDOW_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/glcanvas.h>

// Local headers
#include "utilities/managed_list_class.h"
#include "utilities/math/vector_class.h"
#include "renderer/primitives/primitive.h"

// Local forward declarations
class MATRIX;

class RenderWindow : public wxGLCanvas
{
public:
	// Constructor
	RenderWindow(wxWindow &parent, wxWindowID id, int args[],
		const wxPoint& position, const wxSize& size, long style = 0);

	// Destructor
	virtual ~RenderWindow();

	// Sets up all of the open GL parameters
    void Initialize();

	// Sets up the camera
	void SetCameraView(const VECTOR &position, const VECTOR &lookAt, const VECTOR &upDirection);

	// Transforms between the model coordinate system and the view (openGL) coordinate system
	VECTOR TransformToView(const VECTOR &modelVector) const;
	VECTOR TransformToModel(const VECTOR &viewVector) const;
	VECTOR GetCameraPosition(void) const;

	// Sets the viewing frustum to match the current size of the window
	void AutoSetFrustum(void);

	// Adds actors to the primitives list
	inline void AddActor(Primitive *toAdd) { primitiveList.Add(toAdd); modified = true; };

	// Removes actors from the primitives list
	bool RemoveActor(Primitive *toRemove);

	// Private data accessors
	inline void SetWireFrame(bool _wireFrame) { wireFrame = _wireFrame; modified = true; };
	inline void SetViewOrthogonal(bool _viewOrthogonal) { viewOrthogonal = _viewOrthogonal; modified = true; };

	inline void SetVerticalFOV(double _verticalFOV) { verticalFOV = _verticalFOV; modified = true; };
	inline void SetAspectRatio(double _aspectRatio) { aspectRatio = _aspectRatio; modified = true; };
	inline void SetNearClip(double _nearClip) { nearClip = _nearClip; modified = true; };
	inline void SetFarClip(double _farClip) { farClip = _farClip; modified = true; };
	inline void SetView3D(bool _view3D) { view3D = _view3D; modified = true; };

	inline void SetBackgroundColor(Color _backgroundColor) { backgroundColor = _backgroundColor; modified = true; };
	inline Color GetBackgroundColor(void) { return backgroundColor; };

	inline bool GetWireFrame(void) const { return wireFrame; };
	inline bool GetViewOrthogonal(void) const { return viewOrthogonal; };
	inline bool GetView3D(void) const { return view3D; };

	// Returns a string containing any OpenGL errors
	wxString GetGLError(void) const;

	// Writes the current image to file
	bool WriteImageToFile(wxString pathAndFileName) const;

	// Determines if a particular primitive is in the scene owned by this object
	bool IsThisRendererSelected(const Primitive *pickedObject) const;

private:
	// Flags describing the options for this object's functionality
	bool wireFrame;
	bool viewOrthogonal;

	// The parameters that describe the viewing frustum
	double verticalFOV;
	double aspectRatio;
	double nearClip;
	double farClip;

	// The background color
	Color backgroundColor;

	// List of item indexes and alphas for sorting by alpha
	struct ListItem
	{
		ListItem(const double& _alpha, const int &_i)
		{
			alpha = _alpha;
			i = _i;
		};

		double alpha;
		int i;

		bool operator< (const ListItem &right) const
		{
			return alpha < right.alpha;
		};
	};

	// Event handlers-----------------------------------------------------
	// Interactor events
	virtual void OnMouseWheelEvent(wxMouseEvent &event);
	virtual void OnMouseMoveEvent(wxMouseEvent &event);
	virtual void OnMouseUpEvent(wxMouseEvent &event);

	// Window events
	void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
	void OnEnterWindow(wxMouseEvent &event);
	// End event handlers-------------------------------------------------

	// The main render method - re-draws the scene
    void Render();

	// The type of ineraction to perform
	enum InteractionType
	{
		interactionDollyDrag,// zoom
		interactionDollyWheel,// zoom
		interactionPan,// translate
		interactionRotate
	};

	// Perfoms the computations and transformations associated with the specified interaction
	void PerformInteraction(InteractionType interaction, wxMouseEvent &event);

	// The interaction events (called from within the real event handlers)
	void DoRotate(wxMouseEvent &event);
	void DoWheelDolly(wxMouseEvent &event);
	void DoDragDolly(wxMouseEvent &event);
	void DoPan(wxMouseEvent &event);

	// Updates the transformation matricies according to the current modelview matrix
	void UpdateTransformationMatricies(void);

	// The transformation matricies
	Matrix *modelToView;
	Matrix *viewToModel;

	// The camera position
	VECTOR cameraPosition;

	// The focal point for performing interactions
	VECTOR focalPoint;

	// Method for re-organizing the PrimitiveList so opaque objects are at the beginning and
	// transparent objects are at the end
	void SortPrimitivesByAlpha(void);

protected:
	// Flag indicating whether or not this is a 3D rendering
	bool view3D;

	// Flag indicating whether or not we need to re-initialize this object
	bool modified;

	// The list of objects to create in this scene
	MANAGED_LIST<Primitive> primitiveList;

	// Some interactions require the previous mouse position (window coordinates)
	long lastMousePosition[2];

	// Stores the last mouse position variables
	void StoreMousePosition(wxMouseEvent &event);

	// Flag indicating whether or not we should select a new focal point for the interactions
	bool isInteracting;

	static void ConvertMatrixToGL(const Matrix& matrix, double gl[]);
	static void ConvertGLToMatrix(Matrix& matrix, const double gl[]);

	// For the event table
	DECLARE_EVENT_TABLE()
};

#endif// _RENDER_WINDOW_CLASS_H_
