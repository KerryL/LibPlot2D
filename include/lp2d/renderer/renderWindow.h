/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  renderWindow.h
// Date:  5/14/2009
// Auth:  K. Loux
// Desc:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//        event handlers for various mouse and keyboard interactions.  All
//        object in the scene must be added to the PrimitivesList in order to
//        be drawn.  Objects in the PrimitivesList become managed by this
//        object and are deleted automatically.

#ifndef RENDER_WINDOW_H_
#define RENDER_WINDOW_H_

// Local headers
#include "lp2d/utilities/managedList.h"
#include "lp2d/renderer/primitives/primitive.h"

// Eigen headers
#include <Eigen/Eigen>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/glcanvas.h>

// Standard C++ headers
#include <memory>

namespace LibPlot2D
{

class RenderWindow : public wxGLCanvas
{
public:
	RenderWindow(wxWindow &parent, wxWindowID id, const wxGLAttributes& attr,
		const wxPoint& position, const wxSize& size, long style = 0);
	~RenderWindow() = default;

	void Initialize();
	void SetCameraView(const Eigen::Vector3d &position,
		const Eigen::Vector3d &lookAt, const Eigen::Vector3d &upDirection);

	// Transforms between the model coordinate system and the view (openGL) coordinate system
	Eigen::Vector3d TransformToView(const Eigen::Vector3d &modelVector) const;
	Eigen::Vector3d TransformToModel(const Eigen::Vector3d &viewVector) const;
	Eigen::Vector3d GetCameraPosition() const;

	// Sets the viewing frustum to match the current size of the window
	void AutoSetFrustum();

	// Adds actors to the primitives list
	inline void AddActor(std::unique_ptr<Primitive> toAdd)
	{ mPrimitiveList.Add(std::move(toAdd)); mModified = true; }

	// Removes actors from the primitives list
	bool RemoveActor(Primitive *toRemove);

	// Private data accessors
	inline void SetWireFrame(const bool& wireFrame)
	{ mWireFrame = wireFrame; mModified = true; }
	void SetViewOrthogonal(const bool& viewOrthogonal);

	inline void SetTopMinusBottom(const double& topMinusBottom)
	{ mTopMinusBottom = topMinusBottom; mModified = true; }
	inline void SetAspectRatio(const double& aspectRatio)
	{ mAspectRatio = aspectRatio; mModified = true; }
	inline void SetNearClip(const double& nearClip)
	{ mNearClip = nearClip; mModified = true; }
	inline void SetFarClip(const double& farClip)
	{ mFarClip = farClip; mModified = true; }
	inline void SetView3D(const bool& view3D)
	{ mView3D = view3D; mModified = true; }

	virtual void SetBackgroundColor(const Color& backgroundColor)
	{ mBackgroundColor = backgroundColor; mModified = true; }
	inline Color GetBackgroundColor() { return mBackgroundColor; }

	inline bool GetWireFrame() const { return mWireFrame; }
	inline bool GetViewOrthogonal() const { return mViewOrthogonal; }
	inline bool GetView3D() const { return mView3D; }

	inline double GetAspectRatio() const { return mAspectRatio; }

	// Returns a string containing any OpenGL errors
	static wxString GetGLError();
	static wxString GetGLError(const GLint& e);
	static bool GLHasError();

	// Writes the current image to file
	bool WriteImageToFile(wxString pathAndFileName) const;
	wxImage GetImage() const;

	// Determines if a particular primitive is in the scene owned by this object
	bool IsThisRendererSelected(const Primitive *pickedObject) const;

	void SetNeedAlphaSort() { mNeedAlphaSort = true; }
	void SetNeedOrderSort() { mNeedOrderSort = true; }

	static GLuint CreateShader(const GLenum& type,
		const std::string& shaderContents);
	static GLuint CreateProgram(const std::vector<GLuint>& shaderList);

	void ShiftForExactPixelization();
	void UseDefaultProgram() const;

	GLuint GetPositionLocation() const { return mPositionAttributeLocation; }
	GLuint GetColorLocation() const { return mColorAttributeLocation; }

	virtual unsigned int GetVertexDimension() const { return 4; }

	static void Translate(Eigen::Matrix4d& m, const Eigen::Vector3d& v);
	static void Rotate(Eigen::Matrix4d& m, const double& angle,
		Eigen::Vector3d axis);
	static void Scale(Eigen::Matrix4d& m, const Eigen::Vector3d& v);

	struct ShaderInfo
	{
		GLuint programId;

		bool needsProjection;
		GLuint projectionLocation;

		bool needsModelview;
		GLuint modelViewLocation;
	};

	void AddShader(const ShaderInfo& shader);
	static void SendUniformMatrix(const Eigen::Matrix4d& m,
		const GLuint& location);

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	std::unique_ptr<wxGLContext> mContext;
	wxGLContext* GetContext();

	static const double mExactPixelShift;

	// Flags describing the options for this object's functionality
	bool mWireFrame = false;
	bool mViewOrthogonal = false;

	// The parameters that describe the viewing frustum
	double mTopMinusBottom = 100.0;// in model-space units
	double mAspectRatio;
	double mNearClip = 1.0;
	double mFarClip = 500.0;

	Color mBackgroundColor;

	static bool AlphaSortPredicate(const std::unique_ptr<Primitive>& p1,
		const std::unique_ptr<Primitive>& p2);
	static bool OrderSortPredicate(const std::unique_ptr<Primitive>& p1,
		const std::unique_ptr<Primitive>& p2);

	bool mNeedAlphaSort = true;
	bool mNeedOrderSort = true;

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

	enum class Interaction
	{
		DollyDrag,// zoom
		DollyWheel,// zoom
		Pan,// translate
		Rotate
	};

	// Performs the computations and transformations associated with the
	// specified interaction
	void PerformInteraction(Interaction interaction, wxMouseEvent &event);

	// The interaction events (called from within the real event handlers)
	void DoRotate(wxMouseEvent &event);
	void DoWheelDolly(wxMouseEvent &event);
	void DoDragDolly(wxMouseEvent &event);
	void DoPan(wxMouseEvent &event);

	// Updates the transformation matrices according to the current modelview
	// matrix
	//void UpdateTransformationMatricies();
	void UpdateModelviewMatrix();

	static const std::string mModelviewName;
	static const std::string mProjectionName;
	static const std::string mPositionName;
	static const std::string mColorName;

	static const std::string mDefaultVertexShader;
	static const std::string mDefaultFragmentShader;

	void BuildShaders();

	GLuint CreateDefaultVertexShader();
	GLuint CreateDefaultFragmentShader();

	GLuint mPositionAttributeLocation;
	GLuint mColorAttributeLocation;

	Eigen::Vector3d mFocalPoint;

	void DoResize();

	bool mGlewInitialized = false;

protected:
	bool mView3D = true;
	bool mModified = true;
	bool mSizeUpdateRequired = true;

	ManagedList<Primitive> mPrimitiveList;

	long mLastMousePosition[2];
	void StoreMousePosition(wxMouseEvent &event);

	bool Determine2DInteraction(const wxMouseEvent &event,
		Interaction &interaction) const;
	bool Determine3DInteraction(const wxMouseEvent &event,
		Interaction &interaction) const;

	// Flag indicating whether or not we should select a new focal point for
	// the interactions
	bool mIsInteracting = false;

	static void ConvertMatrixToGL(const Eigen::Matrix4d& matrix, float gl[]);
	static void ConvertGLToMatrix(Eigen::Matrix4d& matrix, const float gl[]);

	void Initialize2D();
	void Initialize3D();

	Eigen::Matrix4d Generate2DProjectionMatrix() const;
	Eigen::Matrix4d Generate3DProjectionMatrix() const;

	bool mModelviewModified = true;
	Eigen::Matrix4d mModelviewMatrix;

	std::vector<ShaderInfo> mShaders;

	DECLARE_EVENT_TABLE()

	virtual std::string GetDefaultVertexShader() const
	{ return mDefaultVertexShader; }
	virtual std::string GetDefaultFragmentShader() const
	{ return mDefaultFragmentShader; }
};

}// namespace LibPlot2D

#endif// RENDER_WINDOW_H_
